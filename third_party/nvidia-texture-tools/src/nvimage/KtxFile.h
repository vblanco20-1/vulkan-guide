// This code is in the public domain -- Ignacio Castaño <castano@gmail.com>

#pragma once
#ifndef NV_IMAGE_KTXFILE_H
#define NV_IMAGE_KTXFILE_H

#include "nvimage.h"
#include "nvcore/StrLib.h"
#include "nvcore/Array.h"

// KTX File format specification:
// http://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/#key

namespace nv
{
    class Stream;

    // GL types
    const uint KTX_UNSIGNED_BYTE = 0x1401;
    const uint KTX_BYTE = 0x1400;
    const uint KTX_UNSIGNED_SHORT = 0x1403;
    const uint KTX_SHORT = 0x1402;
    const uint KTX_UNSIGNED_INT = 0x1405;
    const uint KTX_INT = 0x1404;
    const uint KTX_FLOAT = 0x1406;
    const uint KTX_UNSIGNED_BYTE_3_3_2 = 0x8032;
    const uint KTX_UNSIGNED_BYTE_2_3_3_REV = 0x8362;
    const uint KTX_UNSIGNED_SHORT_5_6_5 = 0x8363;
    const uint KTX_UNSIGNED_SHORT_5_6_5_REV = 0x8364;
    const uint KTX_UNSIGNED_SHORT_4_4_4_4 = 0x8033;
    const uint KTX_UNSIGNED_SHORT_4_4_4_4_REV = 0x8365;
    const uint KTX_UNSIGNED_SHORT_5_5_5_1 = 0x8034;
    const uint KTX_UNSIGNED_SHORT_1_5_5_5_REV = 0x8366;
    const uint KTX_UNSIGNED_INT_8_8_8_8 = 0x8035;
    const uint KTX_UNSIGNED_INT_8_8_8_8_REV = 0x8367;
    const uint KTX_UNSIGNED_INT_10_10_10_2 = 0x8036;
    const uint KTX_UNSIGNED_INT_2_10_10_10_REV = 0x8368;

    // GL formats
    const uint KTX_FORMAT_RED = 0x1903;
    const uint KTX_FORMAT_RG = 0x8227;
    const uint KTX_FORMAT_RGB = 0x1907;
    const uint KTX_FORMAT_BGR = 0x80E0;
    const uint KTX_FORMAT_RGBA = 0x1908;
    const uint KTX_FORMAT_BGRA = 0x80E1;
    const uint KTX_FORMAT_RED_INTEGER = 0x8D94;
    const uint KTX_FORMAT_RG_INTEGER = 0x8228;
    const uint KTX_FORMAT_RGB_INTEGER = 0x8D98;
    const uint KTX_FORMAT_BGR_INTEGER = 0x8D9A;
    const uint KTX_FORMAT_RGBA_INTEGER = 0x8D99;
    const uint KTX_FORMAT_BGRA_INTEGER = 0x8D9B;
    const uint KTX_FORMAT_STENCIL_INDEX = 0x1901;
    const uint KTX_FORMAT_DEPTH_COMPONENT = 0x1902;
    const uint KTX_FORMAT_DEPTH_STENCIL = 0x84F9;

    // GL internal formats
    // BC1
    const uint KTX_INTERNAL_COMPRESSED_RGB_S3TC_DXT1 = 0x83F0;
    const uint KTX_INTERNAL_COMPRESSED_SRGB_S3TC_DXT1 = 0x8C4C;
    // BC1a
    const uint KTX_INTERNAL_COMPRESSED_RGBA_S3TC_DXT1 = 0x83F1;
    const uint KTX_INTERNAL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1 = 0x8C4D;
    // BC2
    const uint KTX_INTERNAL_COMPRESSED_RGBA_S3TC_DXT3 = 0x83F2;
    const uint KTX_INTERNAL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3 = 0x8C4E;
    // BC3
    const uint KTX_INTERNAL_COMPRESSED_RGBA_S3TC_DXT5 = 0x83F3;
    const uint KTX_INTERNAL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5 = 0x8C4F;
    // BC4
    const uint KTX_INTERNAL_COMPRESSED_RED_RGTC1 = 0x8DBB;
    const uint KTX_INTERNAL_COMPRESSED_SIGNED_RED_RGTC1 = 0x8DBC;
    // BC5
    const uint KTX_INTERNAL_COMPRESSED_RG_RGTC2 = 0x8DBD;
    const uint KTX_INTERNAL_COMPRESSED_SIGNED_RG_RGTC2 = 0x8DBE;
    // BC6
    const uint KTX_INTERNAL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT = 0x8E8F;
    const uint KTX_INTERNAL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT = 0x8E8E;
    // BC7
    const uint KTX_INTERNAL_COMPRESSED_RGBA_BPTC_UNORM = 0x8E8C;
    const uint KTX_INTERNAL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM = 0x8E8D;

    // ETC
    const uint KTX_INTERNAL_COMPRESSED_RGB_ETC1 = 0x8D64;
    const uint KTX_INTERNAL_COMPRESSED_SRGB_ETC1 = 0x8D64;  // ???

    // ETC2
    const uint KTX_INTERNAL_COMPRESSED_RED_EAC = 0x9270;
    const uint KTX_INTERNAL_COMPRESSED_SIGNED_RED_EAC = 0x9271;

    const uint KTX_INTERNAL_COMPRESSED_RG_EAC = 0x9272;
    const uint KTX_INTERNAL_COMPRESSED_SIGNED_RG_EAC = 0x9273;

    const uint KTX_INTERNAL_COMPRESSED_RGB_ETC2 = 0x9274;
    const uint KTX_INTERNAL_COMPRESSED_SRGB_ETC2 = 0x9275;

    const uint KTX_INTERNAL_COMPRESSED_RGB_PUNCHTHROUGH_ALPHA_ETC2 = 0x9276;
    const uint KTX_INTERNAL_COMPRESSED_SRGB_PUNCHTHROUGH_ALPHA_ETC2 = 0x9277;

    const uint KTX_INTERNAL_COMPRESSED_RGBA_ETC2_EAC = 0x9278;
    const uint KTX_INTERNAL_COMPRESSED_SRGB_ALPHA_ETC2_EAC = 0x9279;


    // GL base internal formats
    const uint KTX_BASE_INTERNAL_DEPTH_COMPONENT = 0x1902;
    const uint KTX_BASE_INTERNAL_DEPTH_STENCIL = 0x84F9;
    const uint KTX_BASE_INTERNAL_RED = 0x1903;
    const uint KTX_BASE_INTERNAL_RG = 0x8227;
    const uint KTX_BASE_INTERNAL_RGB = 0x1907;
    const uint KTX_BASE_INTERNAL_RGBA = 0x1908;
    const uint KTX_BASE_INTERNAL_STENCIL_INDEX = 0x1901;


    struct KtxHeader {
        uint8 identifier[12];
        uint32 endianness;
        uint32 glType;
        uint32 glTypeSize;
        uint32 glFormat;
        uint32 glInternalFormat;
        uint32 glBaseInternalFormat;
        uint32 pixelWidth;
        uint32 pixelHeight;
        uint32 pixelDepth;
        uint32 numberOfArrayElements;
        uint32 numberOfFaces;
        uint32 numberOfMipmapLevels;
        uint32 bytesOfKeyValueData;

        KtxHeader();

    };

    Stream & operator<< (Stream & s, KtxHeader & header);


/*    struct KtxFile {
        KtxFile();
        ~KtxFile();

        void addKeyValue(const char * key, const char * value);

    private:
        KtxHeader header;

        Array<String> keyArray;
        Array<String> valueArray;
    };

    NVIMAGE_API Stream & operator<< (Stream & s, KtxFile & file);*/


    /*
    for each keyValuePair that fits in bytesOfKeyValueData
        UInt32   keyAndValueByteSize
        Byte     keyAndValue[keyAndValueByteSize]
        Byte     valuePadding[3 - ((keyAndValueByteSize + 3) % 4)]
    end

    for each mipmap_level in numberOfMipmapLevels*
        UInt32 imageSize;
        for each array_element in numberOfArrayElements*
           for each face in numberOfFaces
               for each z_slice in pixelDepth*
                   for each row or row_of_blocks in pixelHeight*
                       for each pixel or block_of_pixels in pixelWidth
                           Byte data[format-specific-number-of-bytes]**
                       end
                   end
               end
               Byte cubePadding[0-3]
           end
        end
        Byte mipPadding[3 - ((imageSize + 3) % 4)]
    end
    */

} // nv namespace

#endif // NV_IMAGE_KTXFILE_H
