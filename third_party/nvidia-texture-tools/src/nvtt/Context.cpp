// Copyright (c) 2009-2011 Ignacio Castano <castano@gmail.com>
// Copyright (c) 2008-2009 NVIDIA Corporation -- Ignacio Castano <icastano@nvidia.com>
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

#include "Context.h"

#include "nvtt.h"

#include "InputOptions.h"
#include "CompressionOptions.h"
#include "OutputOptions.h"
#include "Surface.h"
#include "icbc.h"

#include "CompressorDX9.h"
#include "CompressorDX10.h"
#include "CompressorDX11.h"
#include "CompressorRGB.h"
#include "cuda/CudaUtils.h"
#include "cuda/CudaCompressorDXT.h"

#include "nvimage/DirectDrawSurface.h"
#include "nvimage/KtxFile.h"
#include "nvimage/ColorBlock.h"
#include "nvimage/BlockDXT.h"
#include "nvimage/Image.h"
#include "nvimage/FloatImage.h"
#include "nvimage/Filter.h"
#include "nvimage/Quantize.h"
#include "nvimage/NormalMap.h"
#include "nvimage/PixelFormat.h"
#include "nvimage/ColorSpace.h"

#include "nvcore/Memory.h"
#include "nvcore/Ptr.h"
#include "nvcore/Array.inl"

using namespace nv;
using namespace nvtt;

Compressor::Compressor() : m(*new Compressor::Private())
{
    // CUDA initialization.
    m.cudaSupported = cuda::isHardwarePresent();
    m.cudaEnabled = false;
    m.cuda = NULL;

    enableCudaAcceleration(m.cudaSupported);

    m.dispatcher = &m.defaultDispatcher;

    icbc::init_dxt1();
}

Compressor::~Compressor()
{
    delete &m;
}


void Compressor::enableCudaAcceleration(bool enable)
{
    if (m.cudaSupported)
    {
        m.cudaEnabled = enable;
    }

    if (m.cudaEnabled && m.cuda == NULL)
    {
        m.cuda = new CudaContext();

        if (!m.cuda->isValid())
        {
            m.cudaEnabled = false;
            m.cuda = NULL;
        }
    }
}

bool Compressor::isCudaAccelerationEnabled() const
{
    return m.cudaEnabled;
}

void Compressor::setTaskDispatcher(TaskDispatcher * disp)
{
    if (disp == NULL) {
        m.dispatcher = &m.defaultDispatcher;
    }
    else {
        m.dispatcher = disp;
    }
}


// Input Options API.
bool Compressor::process(const InputOptions & inputOptions, const CompressionOptions & compressionOptions, const OutputOptions & outputOptions) const
{
    return m.compress(inputOptions.m, compressionOptions.m, outputOptions.m);
}

int Compressor::estimateSize(const InputOptions & inputOptions, const CompressionOptions & compressionOptions) const
{
    int w = inputOptions.m.width;
    int h = inputOptions.m.height;
    int d = inputOptions.m.depth;
    
    getTargetExtent(&w, &h, &d, inputOptions.m.maxExtent, inputOptions.m.roundMode, inputOptions.m.textureType);

    int mipmapCount = 1;
    if (inputOptions.m.generateMipmaps) {
        mipmapCount = countMipmaps(w, h, d);
        if (inputOptions.m.maxLevel > 0) mipmapCount = min(mipmapCount, inputOptions.m.maxLevel);
    }

    return inputOptions.m.faceCount * estimateSize(w, h, d, mipmapCount, compressionOptions);
}


// Surface API.
bool Compressor::outputHeader(const Surface & tex, int mipmapCount, const CompressionOptions & compressionOptions, const OutputOptions & outputOptions) const
{
    return m.outputHeader(tex.type(), tex.width(), tex.height(), tex.depth(), 1, mipmapCount, tex.isNormalMap(), compressionOptions.m, outputOptions.m);
}

bool Compressor::compress(const Surface & tex, int face, int mipmap, const CompressionOptions & compressionOptions, const OutputOptions & outputOptions) const
{
    return m.compress(tex, face, mipmap, compressionOptions.m, outputOptions.m);
}

int Compressor::estimateSize(const Surface & tex, int mipmapCount, const CompressionOptions & compressionOptions) const
{
    const int w = tex.width();
    const int h = tex.height();
    const int d = tex.depth();

    return estimateSize(w, h, d, mipmapCount, compressionOptions);
}

bool Compressor::outputHeader(const CubeSurface & cube, int mipmapCount, const CompressionOptions & compressionOptions, const OutputOptions & outputOptions) const
{
    return m.outputHeader(TextureType_Cube, cube.edgeLength(), cube.edgeLength(), 1, 1, mipmapCount, false, compressionOptions.m, outputOptions.m);
}

bool Compressor::compress(const CubeSurface & cube, int mipmap, const CompressionOptions & compressionOptions, const OutputOptions & outputOptions) const
{
    for (int i = 0; i < 6; i++) {
        if(!m.compress(cube.face(i), i, mipmap, compressionOptions.m, outputOptions.m)) {
            return false;
        }
    }
    return true;
}

int Compressor::estimateSize(const CubeSurface & cube, int mipmapCount, const CompressionOptions & compressionOptions) const
{
    return 6 * estimateSize(cube.edgeLength(), cube.edgeLength(), 1, mipmapCount, compressionOptions);
}


// Raw API.
bool Compressor::outputHeader(TextureType type, int w, int h, int d, int arraySize, int mipmapCount, bool isNormalMap, const CompressionOptions & compressionOptions, const OutputOptions & outputOptions) const
{
    return m.outputHeader(type, w, h, d, arraySize, mipmapCount, isNormalMap, compressionOptions.m, outputOptions.m);
}

bool Compressor::compress(int w, int h, int d, int face, int mipmap, const float * rgba, const CompressionOptions & compressionOptions, const OutputOptions & outputOptions) const
{
    return m.compress(AlphaMode_None, w, h, d, face, mipmap, rgba, compressionOptions.m, outputOptions.m);
}

int Compressor::estimateSize(int w, int h, int d, int mipmapCount, const CompressionOptions & compressionOptions) const
{
    const Format format = compressionOptions.m.format;

    const uint bitCount = compressionOptions.m.getBitCount();
    const uint pitchAlignment = compressionOptions.m.pitchAlignment;

    int size = 0;
    for (int m = 0; m < mipmapCount; m++)
    {
        size += computeImageSize(w, h, d, bitCount, pitchAlignment, format);

        // Compute extents of next mipmap:
        w = max(1, w / 2);
        h = max(1, h / 2);
        d = max(1, d / 2);
    }

    return size;
}





bool Compressor::Private::compress(const InputOptions::Private & inputOptions, const CompressionOptions::Private & compressionOptions, const OutputOptions::Private & outputOptions) const
{
    // Make sure enums match.
    nvStaticCheck(FloatImage::WrapMode_Clamp == (FloatImage::WrapMode)WrapMode_Clamp);
    nvStaticCheck(FloatImage::WrapMode_Mirror == (FloatImage::WrapMode)WrapMode_Mirror);
    nvStaticCheck(FloatImage::WrapMode_Repeat == (FloatImage::WrapMode)WrapMode_Repeat);

    // Get output handler.
    if (!outputOptions.hasValidOutputHandler()) {
        outputOptions.error(Error_FileOpen);
        return false;
    }

    const int faceCount = inputOptions.faceCount;
    int width = inputOptions.width;
    int height = inputOptions.height;
    int depth = inputOptions.depth;
    int arraySize = inputOptions.textureType == TextureType_Array ? faceCount : 1;

    nv::getTargetExtent(&width, &height, &depth, inputOptions.maxExtent, inputOptions.roundMode, inputOptions.textureType);

    // If the extents have not changed, then we can use source images for all mipmaps.
    bool canUseSourceImages = (inputOptions.width == width && inputOptions.height == height && inputOptions.depth == depth);

    int mipmapCount = 1;
    if (inputOptions.generateMipmaps) {
        mipmapCount = countMipmaps(width, height, depth);
        if (inputOptions.maxLevel > 0) mipmapCount = min(mipmapCount, inputOptions.maxLevel);
    }

    if (!outputHeader(inputOptions.textureType, width, height, depth, arraySize, mipmapCount, inputOptions.isNormalMap, compressionOptions, outputOptions)) {
        return false;
    }


    if (outputOptions.container != Container_KTX)
    {
        nvtt::Surface img;
        img.setWrapMode(inputOptions.wrapMode);
        img.setAlphaMode(inputOptions.alphaMode);
        img.setNormalMap(inputOptions.isNormalMap);

        // Output each face from the largest mipmap to the smallest.
        for (int f = 0; f < faceCount; f++)
        {
            int w = width;
            int h = height;
            int d = depth;
            bool canUseSourceImagesForThisFace = canUseSourceImages;

            img.setImage(inputOptions.inputFormat, inputOptions.width, inputOptions.height, inputOptions.depth, inputOptions.images[f]);

            // To normal map.
            if (inputOptions.convertToNormalMap) {
                img.toGreyScale(inputOptions.heightFactors.x, inputOptions.heightFactors.y, inputOptions.heightFactors.z, inputOptions.heightFactors.w);
                img.toNormalMap(inputOptions.bumpFrequencyScale.x, inputOptions.bumpFrequencyScale.y, inputOptions.bumpFrequencyScale.z, inputOptions.bumpFrequencyScale.w);
            }

            // To linear space.
            if (!img.isNormalMap()) {
                img.toLinear(inputOptions.inputGamma);
            }

            // Resize input.
            img.resize(w, h, d, ResizeFilter_Box);

            nvtt::Surface tmp = img;
            if (!img.isNormalMap()) {
                tmp.toGamma(inputOptions.outputGamma);
            }

            quantize(tmp, compressionOptions);
            compress(tmp, f, 0, compressionOptions, outputOptions);

            for (int m = 1; m < mipmapCount; m++) {
                w = max(1, w/2);
                h = max(1, h/2);
                d = max(1, d/2);

                int idx = m * faceCount + f;

                bool useSourceImages = false;
                if (canUseSourceImagesForThisFace) {
                    if (inputOptions.images[idx] == NULL) { // One face is missing in this mipmap level.
                        canUseSourceImagesForThisFace = false; // If one level is missing, ignore the following source images.
                    }
                    else {
                        useSourceImages = true;
                    }
                }

                if (useSourceImages) {
                    img.setImage(inputOptions.inputFormat, w, h, d, inputOptions.images[idx]);

                    // For already generated mipmaps, we need to convert to linear.
                    if (!img.isNormalMap()) {
                        img.toLinear(inputOptions.inputGamma);
                    }
                }
                else {
                    if (inputOptions.mipmapFilter == MipmapFilter_Kaiser) {
                        float params[2] = { inputOptions.kaiserAlpha, inputOptions.kaiserStretch };
                        img.buildNextMipmap(MipmapFilter_Kaiser, inputOptions.kaiserWidth, params);
                    }
                    else {
                        img.buildNextMipmap(inputOptions.mipmapFilter);
                    }
                }
                nvDebugCheck(img.width() == w);
                nvDebugCheck(img.height() == h);
                nvDebugCheck(img.depth() == d);

                if (img.isNormalMap()) {
                    if (inputOptions.normalizeMipmaps) {
                        img.expandNormals();
                        img.normalizeNormalMap();
                        img.packNormals();
                    }
                    tmp = img;
                }
                else {
                    tmp = img;
                    tmp.toGamma(inputOptions.outputGamma);
                }

                quantize(tmp, compressionOptions);
                compress(tmp, f, m, compressionOptions, outputOptions);
            }
        }
    }
    else
    {
        // KTX files expect face mipmaps to be interleaved.
        Array<nvtt::Surface> images(faceCount);
        Array<bool> mipChainBroken(faceCount);

        int w = width;
        int h = height;
        int d = depth;

        // https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/#2.16
        uint imageSize = estimateSize(w, h, 1, 1, compressionOptions) * faceCount;
        outputOptions.writeData(&imageSize, sizeof(uint32));

        for (int f = 0; f < faceCount; f++)
        {
            nvtt::Surface s;
            s.setWrapMode(inputOptions.wrapMode);
            s.setAlphaMode(inputOptions.alphaMode);
            s.setNormalMap(inputOptions.isNormalMap);

            s.setImage(inputOptions.inputFormat, inputOptions.width, inputOptions.height, inputOptions.depth, inputOptions.images[f]);

            // To normal map.
            if (inputOptions.convertToNormalMap) {
                s.toGreyScale(inputOptions.heightFactors.x, inputOptions.heightFactors.y, inputOptions.heightFactors.z, inputOptions.heightFactors.w);
                s.toNormalMap(inputOptions.bumpFrequencyScale.x, inputOptions.bumpFrequencyScale.y, inputOptions.bumpFrequencyScale.z, inputOptions.bumpFrequencyScale.w);
            }

            // To linear space.
            if (!s.isNormalMap()) {
                s.toLinear(inputOptions.inputGamma);
            }

            // Resize input.
            s.resize(w, h, d, ResizeFilter_Box);

            nvtt::Surface tmp = s;
            if (!s.isNormalMap()) {
                tmp.toGamma(inputOptions.outputGamma);
            }

            quantize(tmp, compressionOptions);
            compress(tmp, f, 0, compressionOptions, outputOptions);

            images.push_back(s);
            mipChainBroken.push_back(false);
        }

        static const unsigned char padding[3] = {0, 0, 0};
        for (int m = 1; m < mipmapCount; m++)
        {
            w = max(1, w/2);
            h = max(1, h/2);
            d = max(1, d/2);

            // https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/#2.16
            imageSize = estimateSize(w, h, d, 1, compressionOptions) * faceCount;

            outputOptions.writeData(&imageSize, sizeof(uint32));

            nvtt::Surface tmp;

            for (int f = 0; f < faceCount; f++)
            {
                nvtt::Surface& img = images[f];
                int idx = m * faceCount + f;

                bool useSourceImages = false;
                if (!mipChainBroken[f]) {
                    if (inputOptions.images[idx] == NULL) { // One face is missing in this mipmap level.
                        mipChainBroken[f] = false; // If one level is missing, ignore the following source images.
                    }
                    else {
                        useSourceImages = true;
                    }
                }

                if (useSourceImages) {
                    img.setImage(inputOptions.inputFormat, w, h, d, inputOptions.images[idx]);

                    // For already generated mipmaps, we need to convert to linear.
                    if (!img.isNormalMap()) {
                        img.toLinear(inputOptions.inputGamma);
                    }
                }
                else {
                    if (inputOptions.mipmapFilter == MipmapFilter_Kaiser) {
                        float params[2] = { inputOptions.kaiserStretch, inputOptions.kaiserAlpha };
                        img.buildNextMipmap(MipmapFilter_Kaiser, inputOptions.kaiserWidth, params);
                    }
                    else {
                        img.buildNextMipmap(inputOptions.mipmapFilter);
                    }
                }
                nvDebugCheck(img.width() == w);
                nvDebugCheck(img.height() == h);
                nvDebugCheck(img.depth() == d);

                if (img.isNormalMap()) {
                    if (inputOptions.normalizeMipmaps) {
                        img.normalizeNormalMap();
                    }
                    tmp = img;
                }
                else {
                    tmp = img;
                    tmp.toGamma(inputOptions.outputGamma);
                }

                quantize(tmp, compressionOptions);
                compress(tmp, f, m, compressionOptions, outputOptions);

                //cube padding
                if (faceCount == 6 && arraySize == 1)
                {
                    //TODO calc offset for uncompressed images
                }
            }

            int mipPadding = 3 - ((imageSize + 3) % 4);
            if (mipPadding != 0) {
                outputOptions.writeData(&padding, mipPadding);
            }
        }
    }

    return true;
}

bool Compressor::Private::compress(const Surface & tex, int face, int mipmap, const CompressionOptions::Private & compressionOptions, const OutputOptions::Private & outputOptions) const
{
    if (!compress(tex.alphaMode(), tex.width(), tex.height(), tex.depth(), face, mipmap, tex.data(), compressionOptions, outputOptions)) {
        return false;
    }

    return true;
}

bool Compressor::Private::compress(AlphaMode alphaMode, int w, int h, int d, int face, int mipmap, const float * rgba, const CompressionOptions::Private & compressionOptions, const OutputOptions::Private & outputOptions) const
{
    int size = computeImageSize(w, h, d, compressionOptions.getBitCount(), compressionOptions.pitchAlignment, compressionOptions.format);
    outputOptions.beginImage(size, w, h, d, face, mipmap);

    // Decide what compressor to use.
    AutoPtr<CompressorInterface> compressor;
#if defined HAVE_CUDA
    if (cudaEnabled && w * h >= 512)
    {
        compressor = chooseGpuCompressor(compressionOptions);
    }
#endif
    if (compressor == NULL)
    {
        compressor = chooseCpuCompressor(compressionOptions);
    }

    if (compressor == NULL)
    {
        outputOptions.error(Error_UnsupportedFeature);
    }
    else
    {
        compressor->compress(alphaMode, w, h, d, rgba, dispatcher, compressionOptions, outputOptions);
    }

    outputOptions.endImage();

    return true;
}


void Compressor::Private::quantize(Surface & img, const CompressionOptions::Private & compressionOptions) const
{
    if (compressionOptions.enableColorDithering) {
        if (compressionOptions.format >= Format_BC1 && compressionOptions.format <= Format_BC3) {
            img.quantize(0, 5, true, true);
            img.quantize(1, 6, true, true);
            img.quantize(2, 5, true, true);
        }
        else if (compressionOptions.format == Format_RGB) {
            img.quantize(0, compressionOptions.rsize, true, true);
            img.quantize(1, compressionOptions.gsize, true, true);
            img.quantize(2, compressionOptions.bsize, true, true);
        }
    }
    if (compressionOptions.enableAlphaDithering) {
        if (compressionOptions.format == Format_RGB) {
            img.quantize(3, compressionOptions.asize, true, true);
        }
    }
    else if (compressionOptions.binaryAlpha) {
        img.binarize(3, float(compressionOptions.alphaThreshold)/255.0f, compressionOptions.enableAlphaDithering);
    }
}

namespace
{
    enum
    {
        // internal format
        GL_RGB8 = 0x8051,
        GL_RGBA8 = 0x8058,
        GL_R16 = 0x822A,
        GL_RGBA16F = 0x881A,
        GL_R11F_G11F_B10F = 0x8C3A,
        
        // type
        GL_UNSIGNED_BYTE = 0x1401,
        GL_HALF_FLOAT = 0x140B,
        GL_UNSIGNED_INT_10F_11F_11F_REV = 0x8C3B,
        GL_UNSIGNED_SHORT = 0x1403,
        
        // format
        GL_RED = 0x1903,
        GL_RGB = 0x1907,
        GL_RGBA = 0x1908,
        GL_BGR = 0x80E0,
        GL_BGRA = 0x80E1,
    };

    struct GLFormatDescriptor
    {
        uint glFormat; // for uncompressed texture glBaseInternalFormat == glFormat
        uint glInternalFormat;
        uint glType;
        uint glTypeSize;
        RGBAPixelFormat pixelFormat;
    };

    static const GLFormatDescriptor s_glFormats[] =
    {
        { GL_BGR,  GL_RGB8,  GL_UNSIGNED_BYTE, 1, { 24, 0xFF0000,   0xFF00,     0xFF,       0 } },
        { GL_BGRA, GL_RGBA8, GL_UNSIGNED_BYTE, 1, { 32, 0xFF0000,   0xFF00,     0xFF,       0xFF000000 } },
        { GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE, 1, { 32, 0xFF,       0xFF00,     0xFF0000,   0xFF000000 } },
    };

    static const uint s_glFormatCount = NV_ARRAY_SIZE(s_glFormats);

    static const GLFormatDescriptor* findGLFormat(uint bitcount, uint rmask, uint gmask, uint bmask, uint amask)
    {
        for (int i = 0; i < s_glFormatCount; i++)
        {
            if (s_glFormats[i].pixelFormat.bitcount == bitcount &&
            	s_glFormats[i].pixelFormat.rmask == rmask &&
            	s_glFormats[i].pixelFormat.gmask == gmask &&
            	s_glFormats[i].pixelFormat.bmask == bmask &&
            	s_glFormats[i].pixelFormat.amask == amask)
            {
                return &s_glFormats[i];
            }
        }
        
        return nullptr;
    }
}

bool Compressor::Private::outputHeader(nvtt::TextureType textureType, int w, int h, int d, int arraySize, int mipmapCount, bool isNormalMap, const CompressionOptions::Private & compressionOptions, const OutputOptions::Private & outputOptions) const
{
    if (w <= 0 || h <= 0 || d <= 0 || arraySize <= 0 || mipmapCount <= 0)
    {
        outputOptions.error(Error_InvalidInput);
        return false;
    }

    if (!outputOptions.outputHeader)
    {
        return true;
    }

    // Output DDS header.
    if (outputOptions.container == Container_DDS || outputOptions.container == Container_DDS10)
    {
        DDSHeader header;

        header.setUserVersion(outputOptions.version);

        if (textureType == TextureType_2D) {
            nvCheck(arraySize == 1);
            header.setTexture2D();
        }
        else if (textureType == TextureType_Cube) {
            nvCheck(arraySize == 1);
            header.setTextureCube();
        }
        else if (textureType == TextureType_3D) {
            nvCheck(arraySize == 1);
            header.setTexture3D();
            header.setDepth(d);
        }
        else if (textureType == TextureType_Array) {
            header.setTextureArray(arraySize);
        }

        header.setWidth(w);
        header.setHeight(h);
        header.setMipmapCount(mipmapCount);

        bool supported = true;

        if (outputOptions.container == Container_DDS10)
        {
            if (compressionOptions.format == Format_RGBA)
            {
                const uint bitcount = compressionOptions.getBitCount();

                if (compressionOptions.pixelType == PixelType_Float) {
                    if (compressionOptions.rsize == 16 && compressionOptions.gsize == 16 && compressionOptions.bsize == 16 && compressionOptions.asize == 16) {
                        header.setDX10Format(DXGI_FORMAT_R16G16B16A16_FLOAT);
                    }
                    else if (compressionOptions.rsize == 11 && compressionOptions.gsize == 11 && compressionOptions.bsize == 10 && compressionOptions.asize == 0) {
                        header.setDX10Format(DXGI_FORMAT_R11G11B10_FLOAT);
                    }
                    else {
                        supported = false;
                    }
                }
                else {
                    if (bitcount == 16 && compressionOptions.rsize == 16) {
                        header.setDX10Format(DXGI_FORMAT_R16_UNORM);
                    }
                    else {
                        uint format = findDXGIFormat(compressionOptions.bitcount,
                                                     compressionOptions.rmask,
                                                     compressionOptions.gmask,
                                                     compressionOptions.bmask,
                                                     compressionOptions.amask);

                        if (format != DXGI_FORMAT_UNKNOWN) {
                            header.setDX10Format(format);
                        }
                        else {
                            supported = false;
                        }
                    }
                }
            }
            else
            {
                if (compressionOptions.format == Format_DXT1 || compressionOptions.format == Format_DXT1a || compressionOptions.format == Format_DXT1n) {
                    header.setDX10Format(outputOptions.srgb ? DXGI_FORMAT_BC1_UNORM_SRGB : DXGI_FORMAT_BC1_UNORM);
                    if (compressionOptions.format == Format_DXT1a) header.setHasAlphaFlag(true);
                    if (isNormalMap) header.setNormalFlag(true);
                }
                else if (compressionOptions.format == Format_DXT3) {
                    header.setDX10Format(outputOptions.srgb ? DXGI_FORMAT_BC2_UNORM_SRGB : DXGI_FORMAT_BC2_UNORM);
                }
                else if (compressionOptions.format == Format_DXT5 || compressionOptions.format == Format_BC3_RGBM) {
                    header.setDX10Format(outputOptions.srgb ? DXGI_FORMAT_BC3_UNORM_SRGB : DXGI_FORMAT_BC3_UNORM);
                }
                else if (compressionOptions.format == Format_DXT5n) {
                    header.setDX10Format(DXGI_FORMAT_BC3_UNORM);
                    if (isNormalMap) header.setNormalFlag(true);
                }
                else if (compressionOptions.format == Format_BC4) {
                    header.setDX10Format(DXGI_FORMAT_BC4_UNORM); // DXGI_FORMAT_BC4_SNORM ?
                }
                else if (compressionOptions.format == Format_BC5 /*|| compressionOptions.format == Format_BC5_Luma*/) {
                    header.setDX10Format(DXGI_FORMAT_BC5_UNORM); // DXGI_FORMAT_BC5_SNORM ?
                    if (isNormalMap) header.setNormalFlag(true);
                }
                else if (compressionOptions.format == Format_BC6) {
                    if (compressionOptions.pixelType == PixelType_Float) header.setDX10Format(DXGI_FORMAT_BC6H_SF16);
                    /*if (compressionOptions.pixelType == PixelType_UnsignedFloat)*/ header.setDX10Format(DXGI_FORMAT_BC6H_UF16); // By default we assume unsigned.
                }
                else if (compressionOptions.format == Format_BC7) {
                    header.setDX10Format(outputOptions.srgb ? DXGI_FORMAT_BC7_UNORM_SRGB : DXGI_FORMAT_BC7_UNORM);
                    if (isNormalMap) header.setNormalFlag(true);
                }
                else if (compressionOptions.format == Format_CTX1) {
                    supported = false;
                }
                else {
                    supported = false;
                }
            }
        }
        else
        {
            if (compressionOptions.format == Format_RGBA)
            {
                // Get output bit count.
                header.setPitch(computeBytePitch(w, compressionOptions.getBitCount(), compressionOptions.pitchAlignment));

                if (compressionOptions.pixelType == PixelType_Float)
                {
                    if (compressionOptions.rsize == 16 && compressionOptions.gsize == 0 && compressionOptions.bsize == 0 && compressionOptions.asize == 0)
                    {
                        header.setFormatCode(111); // D3DFMT_R16F
                    }
                    else if (compressionOptions.rsize == 16 && compressionOptions.gsize == 16 && compressionOptions.bsize == 0 && compressionOptions.asize == 0)
                    {
                        header.setFormatCode(112); // D3DFMT_G16R16F
                    }
                    else if (compressionOptions.rsize == 16 && compressionOptions.gsize == 16 && compressionOptions.bsize == 16 && compressionOptions.asize == 16)
                    {
                        header.setFormatCode(113); // D3DFMT_A16B16G16R16F
                    }
                    else if (compressionOptions.rsize == 32 && compressionOptions.gsize == 0 && compressionOptions.bsize == 0 && compressionOptions.asize == 0)
                    {
                        header.setFormatCode(114); // D3DFMT_R32F
                    }
                    else if (compressionOptions.rsize == 32 && compressionOptions.gsize == 32 && compressionOptions.bsize == 0 && compressionOptions.asize == 0)
                    {
                        header.setFormatCode(115); // D3DFMT_G32R32F
                    }
                    else if (compressionOptions.rsize == 32 && compressionOptions.gsize == 32 && compressionOptions.bsize == 32 && compressionOptions.asize == 32)
                    {
                        header.setFormatCode(116); // D3DFMT_A32B32G32R32F
                    }
                    else
                    {
                        supported = false;
                    }
                }
                else // Fixed point
                {
                    const uint bitcount = compressionOptions.getBitCount();

                    if (compressionOptions.bitcount != 0)
                    {
                        // Masks already computed.
                        header.setPixelFormat(compressionOptions.bitcount, compressionOptions.rmask, compressionOptions.gmask, compressionOptions.bmask, compressionOptions.amask);
                    }
                    else if (bitcount <= 32)
                    {
                        // Compute pixel format masks.
                        const uint ashift = 0;
                        const uint bshift = ashift + compressionOptions.asize;
                        const uint gshift = bshift + compressionOptions.bsize;
                        const uint rshift = gshift + compressionOptions.gsize;

                        const uint rmask = ((1 << compressionOptions.rsize) - 1) << rshift;
                        const uint gmask = ((1 << compressionOptions.gsize) - 1) << gshift;
                        const uint bmask = ((1 << compressionOptions.bsize) - 1) << bshift;
                        const uint amask = ((1 << compressionOptions.asize) - 1) << ashift;

                        header.setPixelFormat(bitcount, rmask, gmask, bmask, amask);
                    }
                    else
                    {
                        supported = false;
                    }
                }
            }
            else
            {
                header.setLinearSize(computeImageSize(w, h, d, compressionOptions.bitcount, compressionOptions.pitchAlignment, compressionOptions.format));

                if (compressionOptions.format == Format_DXT1 || compressionOptions.format == Format_DXT1a || compressionOptions.format == Format_DXT1n) {
                    header.setFourCC('D', 'X', 'T', '1');
                    if (isNormalMap) header.setNormalFlag(true);
                }
                else if (compressionOptions.format == Format_DXT3) {
                    header.setFourCC('D', 'X', 'T', '3');
                }
                else if (compressionOptions.format == Format_DXT5 || compressionOptions.format == Format_BC3_RGBM) {
                    header.setFourCC('D', 'X', 'T', '5');
                }
                else if (compressionOptions.format == Format_DXT5n) {
                    header.setFourCC('D', 'X', 'T', '5');
                    if (isNormalMap) {
                        header.setNormalFlag(true);
                        header.setSwizzleCode('A', '2', 'D', '5');
                        //header.setSwizzleCode('x', 'G', 'x', 'R');
                    }
                }
                else if (compressionOptions.format == Format_BC4) {
                    header.setFourCC('A', 'T', 'I', '1');
                }
                else if (compressionOptions.format == Format_BC5 /*|| compressionOptions.format == Format_BC5_Luma*/) {
                    header.setFourCC('A', 'T', 'I', '2');
                    if (isNormalMap) {
                        header.setNormalFlag(true);
                        header.setSwizzleCode('A', '2', 'X', 'Y');
                    }
                }
                else if (compressionOptions.format == Format_BC6) {
                    header.setFourCC('Z', 'O', 'H', ' ');               // This is not supported by D3DX. Always use DX10 header with BC6-7 formats.
                    supported = false;
                }
                else if (compressionOptions.format == Format_BC7) {
                    header.setFourCC('Z', 'O', 'L', 'A');               // This is not supported by D3DX. Always use DX10 header with BC6-7 formats.
                    if (isNormalMap) header.setNormalFlag(true);
                    supported = false;
                }
                else if (compressionOptions.format == Format_CTX1) {
                    header.setFourCC('C', 'T', 'X', '1');
                    if (isNormalMap) header.setNormalFlag(true);
                }
                else {
                    supported = false;
                }
            }

            if (outputOptions.srgb) header.setSrgbFlag(true);
        }

        if (!supported)
        {
            // This container does not support the requested format.
            outputOptions.error(Error_UnsupportedOutputFormat);
            return false;
        }

        uint headerSize = 128;
        if (header.hasDX10Header())
        {
            nvStaticCheck(sizeof(DDSHeader) == 128 + 20);
            headerSize = 128 + 20;
        }

        // Swap bytes if necessary.
        header.swapBytes();

        bool writeSucceed = outputOptions.writeData(&header, headerSize);
        if (!writeSucceed)
        {
            outputOptions.error(Error_FileWrite);
        }

        return writeSucceed;
    }
    else if (outputOptions.container == Container_KTX) 
    {
        KtxHeader header;
        // TODO cube arrays
        if (textureType == TextureType_2D) {
            nvCheck(arraySize == 1);
            header.numberOfArrayElements = 0;
            header.numberOfFaces = 1;
            header.pixelDepth = 0;
        }
        else if (textureType == TextureType_Cube) {
            nvCheck(arraySize == 1);
            header.numberOfArrayElements = 0;
            header.numberOfFaces = 6;
            header.pixelDepth = 0;
        }
        else if (textureType == TextureType_3D) {
            nvCheck(arraySize == 1);
            header.numberOfArrayElements = 0;
            header.numberOfFaces = 1;
            header.pixelDepth = d;
        }
        else if (textureType == TextureType_Array) {
            header.numberOfArrayElements = arraySize;
            header.numberOfFaces = 1;
            header.pixelDepth = 0; // Is it?
        }

        header.pixelWidth = w;
        header.pixelHeight = h;
        header.numberOfMipmapLevels = mipmapCount;

        bool supported = true;

        if (compressionOptions.format == Format_RGBA)
        {
            const uint bitcount = compressionOptions.getBitCount();
            
            if (compressionOptions.pixelType == PixelType_Float) {
                if (compressionOptions.rsize == 16 && compressionOptions.gsize == 16 && compressionOptions.bsize == 16 && compressionOptions.asize == 16) {
                    header.glType = GL_HALF_FLOAT;
                    header.glTypeSize = 2;
                    header.glFormat = GL_RGBA;
                    header.glInternalFormat = GL_RGBA16F;
                    header.glBaseInternalFormat = GL_RGBA;
                }
                else if (compressionOptions.rsize == 11 && compressionOptions.gsize == 11 && compressionOptions.bsize == 10 && compressionOptions.asize == 0) {
                    header.glType = GL_UNSIGNED_INT_10F_11F_11F_REV;
                    header.glTypeSize = 4;
                    header.glFormat = GL_RGB;
                    header.glInternalFormat = GL_R11F_G11F_B10F;
                    header.glBaseInternalFormat = GL_RGB;
                }
                else {
                    supported = false;
                }
            }
            else {
                if (bitcount == 16 && compressionOptions.rsize == 16) {
                    header.glType = GL_UNSIGNED_SHORT;
                    header.glTypeSize = 2;
                    header.glFormat = GL_RED;
                    header.glInternalFormat = GL_R16;
                    header.glBaseInternalFormat = GL_RED;
                }
                else {
                    const GLFormatDescriptor* glFormatDesc = findGLFormat(compressionOptions.bitcount, compressionOptions.rmask, compressionOptions.gmask, compressionOptions.bmask, compressionOptions.amask);
                    
                    if (glFormatDesc) {
                        header.glType = glFormatDesc->glType;
                        header.glTypeSize = glFormatDesc->glTypeSize;
                        header.glFormat = glFormatDesc->glFormat;
                        header.glInternalFormat = glFormatDesc->glInternalFormat;
                        header.glBaseInternalFormat = header.glFormat;
                    }
                    else {
                        supported = false;
                    }
                }
            }
        }
        else
        {
            header.glType = 0;
            header.glTypeSize = 1;
            header.glFormat = 0;
            
            if (compressionOptions.format == Format_DXT1 || compressionOptions.format == Format_DXT1n) {
                header.glInternalFormat = outputOptions.srgb ? KTX_INTERNAL_COMPRESSED_SRGB_S3TC_DXT1 : KTX_INTERNAL_COMPRESSED_RGB_S3TC_DXT1;
                header.glBaseInternalFormat = KTX_BASE_INTERNAL_RGB;
            }
            else if (compressionOptions.format == Format_DXT1a) {
                header.glInternalFormat = outputOptions.srgb ? KTX_INTERNAL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1 : KTX_INTERNAL_COMPRESSED_RGBA_S3TC_DXT1;
                header.glBaseInternalFormat = KTX_BASE_INTERNAL_RGBA;
            }
            else if (compressionOptions.format == Format_DXT3) {
                header.glInternalFormat = outputOptions.srgb ? KTX_INTERNAL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3 : KTX_INTERNAL_COMPRESSED_RGBA_S3TC_DXT3;
                header.glBaseInternalFormat = KTX_BASE_INTERNAL_RGBA;
            }
            else if (compressionOptions.format == Format_DXT5 || compressionOptions.format == Format_DXT5n || compressionOptions.format == Format_BC3_RGBM) {
                header.glInternalFormat = outputOptions.srgb ? KTX_INTERNAL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5 : KTX_INTERNAL_COMPRESSED_RGBA_S3TC_DXT5;
                header.glBaseInternalFormat = KTX_BASE_INTERNAL_RGBA;
            }
            else if (compressionOptions.format == Format_BC4) {
                header.glInternalFormat = KTX_INTERNAL_COMPRESSED_RED_RGTC1; // KTX_INTERNAL_COMPRESSED_SIGNED_RED_RGTC1 ?
                header.glBaseInternalFormat = KTX_BASE_INTERNAL_RED;
            }
            else if (compressionOptions.format == Format_BC5) {
                header.glInternalFormat = KTX_INTERNAL_COMPRESSED_RG_RGTC2; // KTX_INTERNAL_COMPRESSED_SIGNED_RG_RGTC2 ?
                header.glBaseInternalFormat = KTX_BASE_INTERNAL_RG;
            }
            else if (compressionOptions.format == Format_BC6) {
                if (compressionOptions.pixelType == PixelType_Float) header.glInternalFormat = KTX_INTERNAL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
                else /*if (compressionOptions.pixelType == PixelType_UnsignedFloat)*/ header.glInternalFormat = KTX_INTERNAL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT; // By default we assume unsigned.
                header.glBaseInternalFormat = KTX_BASE_INTERNAL_RGB;
            }
            else if (compressionOptions.format == Format_BC7) {
                header.glInternalFormat = outputOptions.srgb ? KTX_INTERNAL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM : KTX_INTERNAL_COMPRESSED_RGBA_BPTC_UNORM;
                header.glBaseInternalFormat = KTX_BASE_INTERNAL_RGBA;
            }
            else if (compressionOptions.format == Format_ETC1) {
                header.glInternalFormat = outputOptions.srgb ? KTX_INTERNAL_COMPRESSED_SRGB_ETC1 : KTX_INTERNAL_COMPRESSED_RGB_ETC1;
                header.glBaseInternalFormat = KTX_BASE_INTERNAL_RGB;
            }
            else if (compressionOptions.format == Format_ETC2_R) {
                header.glInternalFormat = KTX_INTERNAL_COMPRESSED_RED_EAC;
                header.glBaseInternalFormat = KTX_BASE_INTERNAL_RED;
            }
            else if (compressionOptions.format == Format_ETC2_RG) {
                header.glInternalFormat = KTX_INTERNAL_COMPRESSED_RG_EAC;
                header.glBaseInternalFormat = KTX_BASE_INTERNAL_RG;
            }
            else if (compressionOptions.format == Format_ETC2_RGB) {
                header.glInternalFormat = outputOptions.srgb ? KTX_INTERNAL_COMPRESSED_SRGB_ETC2 : KTX_INTERNAL_COMPRESSED_RGB_ETC2;
                header.glBaseInternalFormat = KTX_BASE_INTERNAL_RGB;
            }
            else if (compressionOptions.format == Format_ETC2_RGBA) {
                header.glInternalFormat = outputOptions.srgb ? KTX_INTERNAL_COMPRESSED_SRGB_ALPHA_ETC2_EAC : KTX_INTERNAL_COMPRESSED_RGBA_ETC2_EAC;
                header.glBaseInternalFormat = KTX_BASE_INTERNAL_RGBA;
            }
            else {
                supported = false;
            }
        }
        
        if (!supported)
        {
            // This container does not support the requested format.
            outputOptions.error(Error_UnsupportedOutputFormat);
            return false;
        }

        const uint headerSize = 64;
        nvStaticCheck(sizeof(KtxHeader) == 64);

        bool writeSucceed = outputOptions.writeData(&header, headerSize);
        if (!writeSucceed)
        {
            outputOptions.error(Error_FileWrite);
        }

        return writeSucceed;
    }

    return true;
}


CompressorInterface * Compressor::Private::chooseCpuCompressor(const CompressionOptions::Private & compressionOptions) const
{
    if (compressionOptions.format == Format_RGB)
    {
        return new PixelFormatConverter;
    }
    else if (compressionOptions.format == Format_DXT1)
    {
#if defined(HAVE_D3DX)
        if (compressionOptions.externalCompressor == "d3dx") return new D3DXCompressorDXT1;
        else
#endif

#if defined(HAVE_STB)
        if (compressionOptions.externalCompressor == "stb") return new StbCompressorDXT1;
        else
#endif

        return new CompressorDXT1;
    }
    else if (compressionOptions.format == Format_DXT1a)
    {
        if (compressionOptions.quality == Quality_Fastest)
        {
            return new FastCompressorDXT1a;
        }

        return new CompressorDXT1a;
    }
    else if (compressionOptions.format == Format_DXT1n)
    {
        // Not supported.
    }
    else if (compressionOptions.format == Format_DXT3)
    {
        if (compressionOptions.quality == Quality_Fastest)
        {
            return new FastCompressorDXT3;
        }

        return new CompressorDXT3;
    }
    else if (compressionOptions.format == Format_DXT5)
    {
        if (compressionOptions.quality == Quality_Fastest)
        {
            return new FastCompressorDXT5;
        }

        return new CompressorDXT5;
    }
    else if (compressionOptions.format == Format_DXT5n)
    {
        if (compressionOptions.quality == Quality_Fastest)
        {
            return new FastCompressorDXT5n;
        }

        return new CompressorDXT5n;
    }
    else if (compressionOptions.format == Format_BC4)
    {
        if (compressionOptions.quality == Quality_Fastest || compressionOptions.quality == Quality_Normal)
        {
            return new FastCompressorBC4;
        }

        return new ProductionCompressorBC4;
    }
    else if (compressionOptions.format == Format_BC5)
    {
        if (compressionOptions.quality == Quality_Fastest || compressionOptions.quality == Quality_Normal)
        {
            return new FastCompressorBC5;
        }

        return new ProductionCompressorBC5;
    }
    else if (compressionOptions.format == Format_CTX1)
    {
        // Not supported.
    }
    else if (compressionOptions.format == Format_BC6)
    {
        return new CompressorBC6;
    }
    else if (compressionOptions.format == Format_BC7)
    {
        return new CompressorBC7;
    }
    else if (compressionOptions.format == Format_BC3_RGBM)
    {
        return new CompressorBC3_RGBM;
    }
    else if (compressionOptions.format >= Format_ETC1 && compressionOptions.format <= Format_ETC2_RGB_A1)
    {
#if defined(HAVE_RGETC)
        if (compressionOptions.format == Format_ETC1 && compressionOptions.externalCompressor == "rg_etc") return new RgEtcCompressor;
#endif
#if defined(HAVE_ETCLIB)
        if (compressionOptions.externalCompressor == "etclib") return new EtcLibCompressor;
#endif
#if defined(HAVE_ETCPACK)
        if (compressionOptions.format == Format_ETC1 && compressionOptions.externalCompressor == "etcpack") return new EtcPackCompressor;
#endif
#if defined(HAVE_ETCINTEL)
        if (compressionOptions.format == Format_ETC1 && compressionOptions.externalCompressor == "intel") return new EtcIntelCompressor;
#endif
        if (compressionOptions.format == Format_ETC1) return new CompressorETC1;
        else if (compressionOptions.format == Format_ETC2_R) return new CompressorETC2_R;
        //else if (compressionOptions.format == Format_ETC2_RG) return new CompressorETC2_RG;
        else if (compressionOptions.format == Format_ETC2_RGB) return new CompressorETC2_RGB;
        else if (compressionOptions.format == Format_ETC2_RGBA) return new CompressorETC2_RGBA;
    }
    else if (compressionOptions.format == Format_ETC2_RGBM)
    {
        return new CompressorETC2_RGBM;
    }
    else if (compressionOptions.format >= Format_PVR_2BPP_RGB && compressionOptions.format <= Format_PVR_4BPP_RGBA)
    {
#if defined(HAVE_PVRTEXTOOL)
        return new CompressorPVR;
#endif
    }
    return NULL;
}


CompressorInterface * Compressor::Private::chooseGpuCompressor(const CompressionOptions::Private & compressionOptions) const
{
    nvDebugCheck(cudaSupported);

    if (compressionOptions.quality == Quality_Fastest)
    {
        // Do not use CUDA compressors in fastest quality mode.
        return NULL;
    }

#if defined HAVE_CUDA
    if (compressionOptions.format == Format_DXT1)
    {
        return new CudaCompressorDXT1(*cuda);
    }
    else if (compressionOptions.format == Format_DXT1a)
    {
        //#pragma NV_MESSAGE("TODO: Implement CUDA DXT1a compressor.")
    }
    else if (compressionOptions.format == Format_DXT1n)
    {
        // Not supported.
    }
    else if (compressionOptions.format == Format_DXT3)
    {
        //return new CudaCompressorDXT3(*cuda);
    }
    else if (compressionOptions.format == Format_DXT5)
    {
        //return new CudaCompressorDXT5(*cuda);
    }
    else if (compressionOptions.format == Format_DXT5n)
    {
        // @@ Return CUDA compressor.
    }
    else if (compressionOptions.format == Format_BC4)
    {
        // Not supported.
    }
    else if (compressionOptions.format == Format_BC5)
    {
        // Not supported.
    }
    else if (compressionOptions.format == Format_CTX1)
    {
        // @@ Return CUDA compressor.
    }
    else if (compressionOptions.format == Format_BC6)
    {
        // Not supported.
    }
    else if (compressionOptions.format == Format_BC7)
    {
        // Not supported.
    }
#endif // defined HAVE_CUDA

    return NULL;
}

int Compressor::Private::estimateSize(int w, int h, int d, int mipmapCount, const CompressionOptions::Private & compressionOptions) const
{
    const Format format = compressionOptions.format;

    const uint bitCount = compressionOptions.bitcount;
    const uint pitchAlignment = compressionOptions.pitchAlignment;

    int size = 0;
    for (int m = 0; m < mipmapCount; m++)
    {
        size += computeImageSize(w, h, d, bitCount, pitchAlignment, format);

        // Compute extents of next mipmap:
        w = max(1, w / 2);
        h = max(1, h / 2);
        d = max(1, d / 2);
    }

    return size;
}
