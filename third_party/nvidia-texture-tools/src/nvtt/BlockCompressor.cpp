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

#include "BlockCompressor.h"
#include "OutputOptions.h"
#include "TaskDispatcher.h"
#include "CompressionOptions.h"

#include "nvimage/Image.h"
#include "nvimage/ColorBlock.h"
#include "nvimage/BlockDXT.h"

#include "nvmath/Vector.inl"

#include "nvcore/Memory.h"
#include "nvcore/Array.inl"

#include <new> // placement new


using namespace nv;
using namespace nvtt;


struct CompressorContext
{
    AlphaMode alphaMode;
    uint w, h, d;
    const float * data;
    const CompressionOptions::Private * compressionOptions;

    uint bw, bh, bs;
    uint8 * mem;
    CompressorInterface * compressor;
};


// Each task compresses one block.
void ColorBlockCompressorTask(void * data, int i)
{
    CompressorContext * d = (CompressorContext *) data;

    uint x = i % d->bw;
    uint y = i / d->bw;

    //for (uint x = 0; x < d->bw; x++)
    {
        ColorBlock rgba;
        rgba.init(d->w, d->h, d->data, 4*x, 4*y);

        uint8 * ptr = d->mem + (y * d->bw + x) * d->bs;
        ((ColorBlockCompressor *) d->compressor)->compressBlock(rgba, d->alphaMode, *d->compressionOptions, ptr);
    }
}

void ColorBlockCompressor::compress(AlphaMode alphaMode, uint w, uint h, uint d, const float * data, TaskDispatcher * dispatcher, const CompressionOptions::Private & compressionOptions, const OutputOptions::Private & outputOptions)
{
    nvDebugCheck(d == 1);

    CompressorContext context;
    context.alphaMode = alphaMode;
    context.w = w;
    context.h = h;
    context.d = d;
    context.data = data;
    context.compressionOptions = &compressionOptions;

    context.bs = blockSize();
    context.bw = (w + 3) / 4;
    context.bh = (h + 3) / 4;

    context.compressor = this;

    SequentialTaskDispatcher sequential;

    // Use a single thread to compress small textures.
    if (context.bh < 4) dispatcher = &sequential;

#if _DEBUG
    dispatcher = &sequential;
#endif

    const uint count = context.bw * context.bh;
    const uint size = context.bs * count;
    context.mem = new uint8[size];

    dispatcher->dispatch(ColorBlockCompressorTask, &context, count);

    outputOptions.writeData(context.mem, size);

    delete [] context.mem;
}

// Each task compresses one block.
void FloatColorCompressorTask(void * data, int i)
{
    CompressorContext * d = (CompressorContext *) data;

    // Copy image to block.
    const uint block_x = (i % d->bw);
    const uint block_y = (i / d->bw);

    const uint src_x_offset = block_x * 4;
    const uint src_y_offset = block_y * 4;

    const float * r = (const float *)d->data + d->w * d->h * d->d * 0;
    const float * g = (const float *)d->data + d->w * d->h * d->d * 1;
    const float * b = (const float *)d->data + d->w * d->h * d->d * 2;
    const float * a = (const float *)d->data + d->w * d->h * d->d * 3;

    Vector4 colors[16];
    float weights[16];

    const uint block_w = min(d->w - block_x * 4, 4U);
    const uint block_h = min(d->h - block_y * 4, 4U);

    uint x, y;
    for (y = 0; y < block_h; y++) {
        for (x = 0; x < block_w; x++) {
            uint dst_idx = 4 * y + x;
            uint src_idx = (y + src_y_offset) * d->w + (x + src_x_offset);
            colors[dst_idx].x = r[src_idx];
            colors[dst_idx].y = g[src_idx];
            colors[dst_idx].z = b[src_idx];
            colors[dst_idx].w = a[src_idx];
            weights[dst_idx] = (d->alphaMode == AlphaMode_Transparency) ? saturate(a[src_idx]) : 1.0f;
        }
        for (; x < 4; x++) {
            uint dst_idx = 4 * y + x;
            colors[dst_idx] = Vector4(0);
            weights[dst_idx] = 0.0f;
        }
    }
    for (; y < 4; y++) {
        for (x = 0; x < 4; x++) {
            uint dst_idx = 4 * y + x;
            colors[dst_idx] = Vector4(0);
            weights[dst_idx] = 0.0f;
        }
    }
    
    // Compress block.
    uint8 * output = d->mem + (block_y * d->bw + block_x) * d->bs;
    ((FloatColorCompressor *)d->compressor)->compressBlock(colors, weights, *d->compressionOptions, output);
}


void FloatColorCompressor::compress(AlphaMode alphaMode, uint w, uint h, uint d, const float * data, TaskDispatcher * dispatcher, const CompressionOptions::Private & compressionOptions, const OutputOptions::Private & outputOptions)
{
    nvDebugCheck(d == 1);   // @@ Add support for compressed 3D textures.

    CompressorContext context;
    context.alphaMode = alphaMode;
    context.w = w;
    context.h = h;
    context.d = d;
    context.data = data;
    context.compressionOptions = &compressionOptions;

    context.bs = blockSize(compressionOptions);
    context.bw = (w + 3) / 4;
    context.bh = (h + 3) / 4;

    context.compressor = this;

    SequentialTaskDispatcher sequential;

    // Use a single thread to compress small textures.
    if (context.bh < 4) dispatcher = &sequential;

#if _DEBUG
    dispatcher = &sequential;
#endif

    const uint count = context.bw * context.bh;
    const uint size = context.bs * count;
    context.mem = new uint8[size];

    dispatcher->dispatch(FloatColorCompressorTask, &context, count);

    outputOptions.writeData(context.mem, size);

    delete [] context.mem;
}


// BC1
#include "icbc.h"

inline icbc::Quality qualityLevel(const CompressionOptions::Private & compressionOptions) {
    if (compressionOptions.quality == Quality_Fastest) 
        return icbc::Quality_Fast;
    else if (compressionOptions.quality == Quality_Production) 
        return icbc::Quality_Max;
    return icbc::Quality_Default;
}

void CompressorDXT1::compressBlock(Vector4 colors[16], float weights[16], const CompressionOptions::Private & compressionOptions, void * output)
{
    icbc::compress_dxt1(qualityLevel(compressionOptions), (float*)colors, weights, compressionOptions.colorWeight.component, /*three_color_mode*/true, /*three_color_black*/true, output);
}


// @@ BC1a

// @@ BC2

// @@ BC3


// BC3_RGBM
#include "CompressorDXT5_RGBM.h"

void CompressorBC3_RGBM::compressBlock(Vector4 colors[16], float weights[16], const CompressionOptions::Private & compressionOptions, void * output)
{
    compress_dxt5_rgbm(colors, weights, compressionOptions.rgbmThreshold, (BlockDXT5 *)output);
}


// ETC
#include "CompressorETC.h"

void CompressorETC1::compressBlock(Vector4 colors[16], float weights[16], const CompressionOptions::Private & compressionOptions, void * output)
{
    compress_etc1(colors, weights, compressionOptions.colorWeight.xyz(), output);
}
void CompressorETC2_R::compressBlock(Vector4 colors[16], float weights[16], const CompressionOptions::Private & compressionOptions, void * output)
{
    // @@ Change radius based on quality.
    compress_eac(colors, weights, /*input_channel=*/1, /*search_radius=*/1, /*use_11bit_mode=*/true, output);
}
void CompressorETC2_RG::compressBlock(Vector4 colors[16], float weights[16], const CompressionOptions::Private & compressionOptions, void * output)
{
    //compress_eac_rg(colors, weights, 1, 2, output);
}
void CompressorETC2_RGB::compressBlock(Vector4 colors[16], float weights[16], const CompressionOptions::Private & compressionOptions, void * output)
{
    // @@ Tweak quality options.
    compress_etc2(colors, weights, compressionOptions.colorWeight.xyz(), output);
}
void CompressorETC2_RGBA::compressBlock(Vector4 colors[16], float weights[16], const CompressionOptions::Private & compressionOptions, void * output)
{
    // @@ Tweak quality options.
    // @@ Change radius based on quality.
    compress_etc2_eac(colors, weights, compressionOptions.colorWeight.xyz(), output);
}
/*void CompressorETC2_RG::compressBlock(Vector4 colors[16], float weights[16], const CompressionOptions::Private & compressionOptions, void * output)
{
    // @@ Change radius based on quality.
    compress_eac_rg(colors, weights, compressionOptions.colorWeight.xyz(), output);
}*/
void CompressorETC2_RGBM::compressBlock(Vector4 colors[16], float weights[16], const CompressionOptions::Private & compressionOptions, void * output)
{
    compress_etc2_rgbm(colors, weights, compressionOptions.rgbmThreshold, output);
}



// External compressors.

#if defined(HAVE_D3DX)

void D3DXCompressorDXT1::compress(InputFormat inputFormat, AlphaMode alphaMode, uint w, uint h, uint d, void * data, const CompressionOptions::Private & compressionOptions, const OutputOptions::Private & outputOptions)
{
    nvDebugCheck(d == 1);

    IDirect3D9 * d3d = Direct3DCreate9(D3D_SDK_VERSION);

    D3DPRESENT_PARAMETERS presentParams;
    ZeroMemory(&presentParams, sizeof(presentParams));
    presentParams.Windowed = TRUE;
    presentParams.SwapEffect = D3DSWAPEFFECT_COPY;
    presentParams.BackBufferWidth = 8;
    presentParams.BackBufferHeight = 8;
    presentParams.BackBufferFormat = D3DFMT_UNKNOWN;

    HRESULT err;

    IDirect3DDevice9 * device = NULL;
    err = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, GetDesktopWindow(), D3DCREATE_SOFTWARE_VERTEXPROCESSING, &presentParams, &device);

    IDirect3DTexture9 * texture = NULL;
    err = D3DXCreateTexture(device, w, h, 1, 0, D3DFMT_DXT1, D3DPOOL_SYSTEMMEM, &texture);

    IDirect3DSurface9 * surface = NULL;
    err = texture->GetSurfaceLevel(0, &surface);

    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.bottom = h;
    rect.right = w;

    if (inputFormat == InputFormat_BGRA_8UB)
    {
        err = D3DXLoadSurfaceFromMemory(surface, NULL, NULL, data, D3DFMT_A8R8G8B8, w * 4, NULL, &rect, D3DX_DEFAULT, 0);
    }
    else
    {
        err = D3DXLoadSurfaceFromMemory(surface, NULL, NULL, data, D3DFMT_A32B32G32R32F, w * 16, NULL, &rect, D3DX_DEFAULT, 0);
    }

    if (err != D3DERR_INVALIDCALL && err != D3DXERR_INVALIDDATA)
    {
        D3DLOCKED_RECT rect;
        ZeroMemory(&rect, sizeof(rect));

        err = surface->LockRect(&rect, NULL, D3DLOCK_READONLY);

        if (outputOptions.outputHandler != NULL) {
            int size = rect.Pitch * ((h + 3) / 4);
            outputOptions.outputHandler->writeData(rect.pBits, size);
        }

        err = surface->UnlockRect();
    }

    surface->Release();
    device->Release();
    d3d->Release();
}

#endif // defined(HAVE_D3DX)


#if defined(HAVE_STB)

#define STB_DEFINE
#include "stb/stb_dxt.h"

void StbCompressorDXT1::compressBlock(ColorBlock & rgba, AlphaMode alphaMode, const CompressionOptions::Private & compressionOptions, void * output)
{
    rgba.swizzle(2, 1, 0, 3); // Swap R and B
    stb_compress_dxt_block((unsigned char *)output, (unsigned char *)rgba.colors(), 0, 0);
}

#endif // defined(HAVE_STB)


#if defined(HAVE_ETCLIB)
#include "Etc.h"

void EtcLibCompressor::compress(AlphaMode alphaMode, uint w, uint h, uint d, const float * data, TaskDispatcher * dispatcher, const CompressionOptions::Private & compressionOptions, const OutputOptions::Private & outputOptions)
{
    //nvCheck(d == 1);  // Encode one layer at a time?

    Etc::Image::Format format;
    if (compressionOptions.format == Format_ETC1) {
        format = Etc::Image::Format::ETC1;
    }
    else if (compressionOptions.format == Format_ETC2_R) {
        format = Etc::Image::Format::R11;
    }
    else if (compressionOptions.format == Format_ETC2_RG) {
        format = Etc::Image::Format::RG11;
    }
    else if (compressionOptions.format == Format_ETC2_RGB) {
        format = Etc::Image::Format::RGB8;
        //format = Etc::Image::Format::SRGB8;
    }
    else if (compressionOptions.format == Format_ETC2_RGBA) {
        format = Etc::Image::Format::RGBA8;
        //format = Etc::Image::Format::SRGBA8;
    }
    else if (compressionOptions.format == Format_ETC2_RGB_A1) {
        format = Etc::Image::Format::RGB8A1;
        //format = Etc::Image::Format::SRGB8A1;
    }
    else {
        nvCheck(false);
        return;
    }

    Etc::ErrorMetric error_metric = Etc::ErrorMetric::RGBA;

    // @@ Use normal compression metric for normals?
    //if (compressionOptions.)

    // @@ Adjust based on quality.
    int effort = ETCCOMP_DEFAULT_EFFORT_LEVEL;

    // @@ What are the defaults?
    uint jobs = 4;
    uint max_jobs = 4;

    uint8 * out_data = NULL;
    uint out_size = 0;
    uint out_width = 0;
    uint out_height = 0;
    int out_time = 0;

    // Swizzle color data.
    nv::Array<float> tmp;
    uint count = w * h;
    tmp.resize(4 * count);
    for (uint i = 0; i < count; i++) {
        tmp[4*i+0] = data[count*0 + i];
        tmp[4*i+1] = data[count*1 + i];
        tmp[4*i+2] = data[count*2 + i];
        tmp[4*i+3] = data[count*3 + i];
    }

    Etc::Encode(tmp.buffer(), w, h, format, error_metric, effort, jobs, max_jobs, &out_data, &out_size, &out_width, &out_height, &out_time);

    if (outputOptions.outputHandler != NULL) {
        outputOptions.outputHandler->writeData(out_data, I32(out_size));
    }
}

#endif

#if defined(HAVE_RGETC)
#include "rg_etc1.h"

NV_AT_STARTUP(rg_etc1::pack_etc1_block_init()); // @@ Do this in context init.

void RgEtcCompressor::compressBlock(ColorBlock & rgba, AlphaMode alphaMode, const CompressionOptions::Private & compressionOptions, void * output)
{
    rg_etc1::etc1_pack_params pack_params;

    pack_params.m_quality = rg_etc1::cMediumQuality;
    if (compressionOptions.quality == Quality_Fastest) pack_params.m_quality = rg_etc1::cLowQuality;
    else if (compressionOptions.quality == Quality_Production) pack_params.m_quality = rg_etc1::cHighQuality;
    else if (compressionOptions.quality == Quality_Highest) pack_params.m_quality = rg_etc1::cHighQuality;
    else if (compressionOptions.quality == Quality_Normal) pack_params.m_quality = rg_etc1::cMediumQuality;

    rgba.swizzle(2, 1, 0, 3);
    rg_etc1::pack_etc1_block(output, (uint *)rgba.colors(), pack_params);

    //Vector4 result[16];
    //nv::decompress_etc(output, result);

}

#endif

#if defined(HAVE_ETCPACK)

void EtcPackCompressor::compress(nvtt::AlphaMode alphaMode, uint w, uint h, uint d, const float * data, nvtt::TaskDispatcher * dispatcher, const nvtt::CompressionOptions::Private & compressionOptions, const nvtt::OutputOptions::Private & outputOptions) 
{
    uint8 *imgdec = (uint8 *)malloc(expandedwidth*expandedheight * 3);

    uint32 block1, block2;

    if (compressionOptions.quality == Quality_Fastest) {
        compressBlockDiffFlipFast(img, imgdec, expandedwidth, expandedheight, 4 * x, 4 * y, block1, block2);
    }
    else {
        compressBlockETC1Exhaustive(img, imgdec, expandedwidth, expandedheight, 4 * x, 4 * y, block1, block2);
    }
}

#endif

#if defined(HAVE_ETCINTEL)
#include "kernel_ispc.h"

void EtcIntelCompressor::compress(nvtt::AlphaMode alphaMode, uint w, uint h, uint d, const float * data, nvtt::TaskDispatcher * dispatcher, const nvtt::CompressionOptions::Private & compressionOptions, const nvtt::OutputOptions::Private & outputOptions)
{
    nvCheck(d == 1);

    // Allocate and convert input.
    nv::Array<uint8> src;
    const uint count = w * h;
    src.resize(4 * count);

    for (uint i = 0; i < count; i++) {
        src[4 * i + 0] = data[count * 0 + i]; // @@ Scale by 256?
        src[4 * i + 1] = data[count * 1 + i];
        src[4 * i + 2] = data[count * 2 + i];
        src[4 * i + 3] = data[count * 3 + i];
    }

    int bw = (w + 3) / 4;
    int bw = (w + 3) / 4;

    // Allocate output.
    nv::Array<uint8> dst;
    dst.resize(bw * bh * 4);

    ispc::rgba_surface surface;
    surface.ptr = src.buffer();
    surface.width = w;
    surface.height = h;
    surface.stride = w * 4;

    ispc::CompressBlocksBC1_ispc(&surface, dst)
}

#endif

#if defined(HAVE_PVRTEXTOOL)

#include <PVRTextureUtilities.h> // for CPVRTexture, CPVRTextureHeader, PixelType, Transcode

#include "nvmath/Color.inl"

void CompressorPVR::compress(AlphaMode alphaMode, uint w, uint h, uint d, const float * data, TaskDispatcher * dispatcher, const CompressionOptions::Private & compressionOptions, const OutputOptions::Private & outputOptions)
{
    EPVRTColourSpace color_space = ePVRTCSpacelRGB;

    //pvrtexture::PixelType src_pixel_type = pvrtexture::PixelType('b','g','r','a',8,8,8,8);
    pvrtexture::PixelType src_pixel_type = pvrtexture::PixelType('r','g','b',0,8,8,8,0);
    pvrtexture::CPVRTextureHeader header(src_pixel_type.PixelTypeID, h, w, d, 1/*num mips*/, 1/*num array*/, 1/*num faces*/, color_space, ePVRTVarTypeUnsignedByteNorm);

    /*
    uint count = w * h * d;
    Array<Color32> tmp;
    tmp.resize(count);

    for (uint i = 0; i < count; i++) {
        tmp[i] = toColor32(Vector4(data[0*count + i], data[1*count + i], data[2*count + i], data[3*count + i]));
    }
    */

    uint count = w * h * d;
    Array<uint8> tmp;
    tmp.resize(3 * count);

    for (uint i = 0; i < count; i++) {
        tmp[3*i+0] = data[0*count + i] * 255.0f;
        tmp[3*i+1] = data[1*count + i] * 255.0f;
        tmp[3*i+2] = data[2*count + i] * 255.0f;
    }

    pvrtexture::CPVRTexture texture(header, tmp.buffer());

    pvrtexture::PixelType dst_pixel_type = pvrtexture::PixelType(ePVRTPF_PVRTCI_2bpp_RGB);

    if (compressionOptions.format == Format_PVR_2BPP_RGB) dst_pixel_type = pvrtexture::PixelType(ePVRTPF_PVRTCI_2bpp_RGB);
    else if (compressionOptions.format == Format_PVR_4BPP_RGB) dst_pixel_type = pvrtexture::PixelType(ePVRTPF_PVRTCI_4bpp_RGB);
    else if (compressionOptions.format == Format_PVR_2BPP_RGBA) dst_pixel_type = pvrtexture::PixelType(ePVRTPF_PVRTCI_2bpp_RGBA);
    else if (compressionOptions.format == Format_PVR_4BPP_RGBA) dst_pixel_type = pvrtexture::PixelType(ePVRTPF_PVRTCI_4bpp_RGBA);

    bool success = pvrtexture::Transcode(texture, dst_pixel_type, ePVRTVarTypeUnsignedByteNorm, color_space, pvrtexture::ePVRTCNormal, false);

    if (success) {
        uint size = 0;
        if (compressionOptions.format == Format_PVR_2BPP_RGB || compressionOptions.format == Format_PVR_2BPP_RGBA) {
            // 2 bpp
            const uint bpp = 2u;
            const uint block_size = 8u * 4u;
            const uint size_factor=(block_size*bpp)>>3u;
            const uint block_width=nv::max((w>>3u), 2u);
            const uint block_height=nv::max((h>>2u), 2u);
            size = d * block_width * block_height * size_factor;
        }
        else {
            // 4 bpp
            const uint bpp = 4u;
            const uint block_size = 4u * 4u;
            const uint size_factor = (block_size*bpp) >> 3u;
            const uint block_width = max((w>>2u), 2u);
            const uint block_height = max((h>>2u), 2u);
            size = d * block_width * block_height * size_factor;
        }

        if (outputOptions.outputHandler != NULL) {
            outputOptions.outputHandler->writeData(texture.getDataPtr(), I32(size));
        }
    }
}

#endif


