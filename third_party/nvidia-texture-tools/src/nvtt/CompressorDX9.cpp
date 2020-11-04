// Copyright (c) 2009-2011 Ignacio Castano <castano@gmail.com>
// Copyright (c) 2007-2009 NVIDIA Corporation -- Ignacio Castano <icastano@nvidia.com>
// 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include "CompressorDX9.h"
#include "QuickCompressDXT.h"
#include "OptimalCompressDXT.h"
#include "CompressionOptions.h"
#include "OutputOptions.h"
#include "CompressorDXT5_RGBM.h"

// squish
#include "squish/colourset.h"
#include "squish/weightedclusterfit.h"

#include "nvtt.h"

#include "nvimage/Image.h"
#include "nvimage/ColorBlock.h"
#include "nvimage/BlockDXT.h"

#include "nvmath/Vector.inl"
#include "nvmath/Color.inl"

#include "nvcore/Memory.h"

#include <new> // placement new


using namespace nv;
using namespace nvtt;



void FastCompressorDXT1a::compressBlock(ColorBlock & rgba, nvtt::AlphaMode alphaMode, const nvtt::CompressionOptions::Private & compressionOptions, void * output)
{
    BlockDXT1 * block = new(output) BlockDXT1;
    QuickCompress::compressDXT1a(rgba, block);
}

void FastCompressorDXT3::compressBlock(ColorBlock & rgba, nvtt::AlphaMode alphaMode, const nvtt::CompressionOptions::Private & compressionOptions, void * output)
{
    BlockDXT3 * block = new(output) BlockDXT3;
    QuickCompress::compressDXT3(rgba, block);
}

void FastCompressorDXT5::compressBlock(ColorBlock & rgba, nvtt::AlphaMode alphaMode, const nvtt::CompressionOptions::Private & compressionOptions, void * output)
{
    BlockDXT5 * block = new(output) BlockDXT5;
    QuickCompress::compressDXT5(rgba, block);
}

void FastCompressorDXT5n::compressBlock(ColorBlock & rgba, nvtt::AlphaMode alphaMode, const nvtt::CompressionOptions::Private & compressionOptions, void * output)
{
    rgba.swizzle(4, 1, 5, 0); // 0xFF, G, 0, R

    BlockDXT5 * block = new(output) BlockDXT5;
    QuickCompress::compressDXT5(rgba, block);
}



void CompressorDXT1a::compressBlock(ColorBlock & rgba, nvtt::AlphaMode alphaMode, const nvtt::CompressionOptions::Private & compressionOptions, void * output)
{
    uint alphaMask = 0;
    for (uint i = 0; i < 16; i++)
    {
        if (rgba.color(i).a == 0) alphaMask |= (3U << (i * 2U)); // Set two bits for each color.
    }

    const bool isSingleColor = rgba.isSingleColor();

    if (isSingleColor)
    {
        BlockDXT1 * block = new(output) BlockDXT1;
        OptimalCompress::compressDXT1a(rgba.color(0), alphaMask, block);
    }
    else
    {
        nvsquish::WeightedClusterFit fit;
        fit.SetMetric(compressionOptions.colorWeight.x, compressionOptions.colorWeight.y, compressionOptions.colorWeight.z);

        int flags = nvsquish::kDxt1;
        if (alphaMode == nvtt::AlphaMode_Transparency) flags |= nvsquish::kWeightColourByAlpha;

        nvsquish::ColourSet colours((uint8 *)rgba.colors(), flags);
        fit.SetColourSet(&colours, nvsquish::kDxt1);

        fit.Compress(output);
    }
}

void CompressorDXT1_Luma::compressBlock(ColorBlock & rgba, nvtt::AlphaMode alphaMode, const nvtt::CompressionOptions::Private & compressionOptions, void * output)
{
    BlockDXT1 * block = new(output) BlockDXT1;
    OptimalCompress::compressDXT1_Luma(rgba, block);
}

void CompressorDXT3::compressBlock(ColorBlock & rgba, nvtt::AlphaMode alphaMode, const nvtt::CompressionOptions::Private & compressionOptions, void * output)
{
    BlockDXT3 * block = new(output) BlockDXT3;

    // Compress explicit alpha.
    OptimalCompress::compressDXT3A(rgba, &block->alpha);

    // Compress color.
    if (rgba.isSingleColor())
    {
        OptimalCompress::compressDXT1(rgba.color(0), &block->color);
    }
    else
    {
        nvsquish::WeightedClusterFit fit;
        fit.SetMetric(compressionOptions.colorWeight.x, compressionOptions.colorWeight.y, compressionOptions.colorWeight.z);

        int flags = 0;
        if (alphaMode == nvtt::AlphaMode_Transparency) flags |= nvsquish::kWeightColourByAlpha;

        nvsquish::ColourSet colours((uint8 *)rgba.colors(), flags);
        fit.SetColourSet(&colours, 0);
        fit.Compress(&block->color);
    }
}

void CompressorDXT5::compressBlock(ColorBlock & rgba, nvtt::AlphaMode alphaMode, const nvtt::CompressionOptions::Private & compressionOptions, void * output)
{
    BlockDXT5 * block = new(output) BlockDXT5;

    // Compress alpha.
    if (compressionOptions.quality == Quality_Highest)
    {
        OptimalCompress::compressDXT5A(rgba, &block->alpha);
    }
    else
    {
        QuickCompress::compressDXT5A(rgba, &block->alpha);
    }

    // Compress color.
    if (rgba.isSingleColor())
    {
        OptimalCompress::compressDXT1(rgba.color(0), &block->color);
    }
    else
    {
        nvsquish::WeightedClusterFit fit;
        fit.SetMetric(compressionOptions.colorWeight.x, compressionOptions.colorWeight.y, compressionOptions.colorWeight.z);

        int flags = 0;
        if (alphaMode == nvtt::AlphaMode_Transparency) flags |= nvsquish::kWeightColourByAlpha;

        nvsquish::ColourSet colours((uint8 *)rgba.colors(), flags);
        fit.SetColourSet(&colours, 0);
        fit.Compress(&block->color);
    }
}


void CompressorDXT5n::compressBlock(ColorBlock & rgba, nvtt::AlphaMode alphaMode, const nvtt::CompressionOptions::Private & compressionOptions, void * output)
{
    BlockDXT5 * block = new(output) BlockDXT5;

    // Compress Y.
    if (compressionOptions.quality == Quality_Highest)
    {
        OptimalCompress::compressDXT1G(rgba, &block->color);
    }
    else
    {
        if (rgba.isSingleColor(Color32(0, 0xFF, 0, 0))) // Mask all but green channel.
        {
                OptimalCompress::compressDXT1G(rgba.color(0).g, &block->color);
        }
        else
        {
            ColorBlock tile = rgba;
            tile.swizzle(4, 1, 5, 3); // leave alpha in alpha channel.

            nvsquish::WeightedClusterFit fit;
            fit.SetMetric(0, 1, 0);

            int flags = 0;
            if (alphaMode == nvtt::AlphaMode_Transparency) flags |= nvsquish::kWeightColourByAlpha;

            nvsquish::ColourSet colours((uint8 *)tile.colors(), flags);
            fit.SetColourSet(&colours, 0);
            fit.Compress(&block->color);
        }
    }

    rgba.swizzle(4, 1, 5, 0); // 1, G, 0, R

    // Compress X.
    if (compressionOptions.quality == Quality_Highest)
    {
        OptimalCompress::compressDXT5A(rgba, &block->alpha);
    }
    else
    {
        QuickCompress::compressDXT5A(rgba, &block->alpha);
    }
}





