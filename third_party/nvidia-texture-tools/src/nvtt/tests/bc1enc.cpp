
#define  _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>

//#define STBI_ASSERT(x)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_DXT_IMPLEMENTATION
#include "stb_dxt.h"

#define RGBCX_IMPLEMENTATION
#include "../extern/rg/rgbcx.h"

#define ICBC_IMPLEMENTATION
#include "nvtt/icbc.h"

#define ICETC_IMPLEMENTATION
#include "nvtt/icetc.h"

//#define GOOFYTC_IMPLEMENTATION
//#include "../extern/goofy_tc.h"

#include "../extern/rg_etc1_v104/rg_etc1.h"
#include "../extern/rg_etc1_v104/rg_etc1.cpp"



#include "../extern/libsquish-1.15/squish.h"

#include "../extern/CMP_Core/source/CMP_Core.h"

#include "nvmath/Vector.inl"
#include "nvmath/Color.h"

#include "nvcore/Timer.h"
#include "nvcore/Array.inl"

using namespace nv;

typedef unsigned char u8;
typedef unsigned int u32;


#define TEST_STB 0
#define TEST_STB_HQ 0

#define TEST_GOOFY 0

#define TEST_RGBCX_FAST 0  // Level 0-3
#define TEST_RGBCX_ALL 0

#define TEST_ICBC_FAST 1
#define TEST_ICBC 0
#define TEST_ICBC_HQ 0
#define TEST_ICBC_ALL 0

#define TEST_IC_ETC 0
#define TEST_RG_ETC 0

#define TEST_SQUISH 1
#define TEST_SQUISH_HQ 0

#define TEST_AMD_CMP 1




// Returns mse.
float evaluate_dxt1_mse(uint8 * rgba, uint8 * block, int block_count, icbc::Decoder decoder = icbc::Decoder_D3D10) {
    double total = 0.0f;
    for (int b = 0; b < block_count; b++) {
        total += icbc::evaluate_dxt1_error(rgba, block, decoder);
        rgba += 4 * 4 * 4;
        block += 8;
    }
    return float(total / (16 * block_count));
}

float evaluate_etc2_mse(uint8 * rgba, uint8 * block, int block_count) {
    double total = 0.0f;
    for (int b = 0; b < block_count; b++) {
        total += icetc::evaluate_etc2_error(rgba, block);
        rgba += 4 * 4 * 4;
        block += 8;
    }
    return float(total / (16 * block_count));
}


#define MAKEFOURCC(str) (uint(str[0]) | (uint(str[1]) << 8) | (uint(str[2]) << 16) | (uint(str[3]) << 24 ))


bool output_dxt_dds (u32 w, u32 h, const u8* data, const char * filename) {

    const u32 DDSD_CAPS = 0x00000001;
    const u32 DDSD_PIXELFORMAT = 0x00001000;
    const u32 DDSD_WIDTH = 0x00000004;
    const u32 DDSD_HEIGHT = 0x00000002;
    const u32 DDSD_LINEARSIZE = 0x00080000;
    const u32 DDPF_FOURCC = 0x00000004;
    const u32 DDSCAPS_TEXTURE = 0x00001000;

    struct DDS {
        u32 fourcc = MAKEFOURCC("DDS ");
        u32 size = 124;
        u32 flags = DDSD_CAPS|DDSD_PIXELFORMAT|DDSD_WIDTH|DDSD_HEIGHT|DDSD_LINEARSIZE;
        u32 height;
        u32 width;
        u32 pitch;
        u32 depth;
        u32 mipmapcount;
        u32 reserved [11];
        struct {
            u32 size = 32;
            u32 flags = DDPF_FOURCC;
            u32 fourcc = MAKEFOURCC("DXT1");
            u32 bitcount;
            u32 rmask;
            u32 gmask;
            u32 bmask;
            u32 amask;
        } pf;
        struct {
            u32 caps1 = DDSCAPS_TEXTURE;
            u32 caps2;
            u32 caps3;
            u32 caps4;
        } caps;
        u32 notused;
    } dds;
    static_assert(sizeof(DDS) == 128, "DDS size must be 128");

    dds.width = w;
    dds.height = h;
    dds.pitch = 8 * (((w+3)/4) * ((h+3)/4)); // linear size

    FILE * fp = fopen(filename, "wb");
    if (fp == nullptr) return false;

    // Write header:
    fwrite(&dds, sizeof(dds), 1, fp);

    // Write dxt data:
    fwrite(data, dds.pitch, 1, fp);

    fclose(fp);

    return true;
}

bool output_etc(u32 bw, u32 bh, const u8* block_data, const char *filename) {

    u8 * rgba_data = (u8 *)malloc(bw * bh * 4);

    for (u32 by = 0; by < bh; by += 4) {
        for (u32 bx = 0; bx < bw; bx += 4) {

            u8 rgba_block[64];
            icetc::decompress_etc(block_data, rgba_block);

            block_data += 8;

            for (u32 y = 0; y < 4; y++) {
                for (u32 x = 0; x < 4; x++) {
                    u32 idx = ((by + y) * bw + (bx + x)) * 4;
                    rgba_data[idx + 0] = rgba_block[4 * (4 * y + x) + 0];
                    rgba_data[idx + 1] = rgba_block[4 * (4 * y + x) + 1];
                    rgba_data[idx + 2] = rgba_block[4 * (4 * y + x) + 2];
                    rgba_data[idx + 3] = rgba_block[4 * (4 * y + x) + 3];
                }
            }
        }
    }

    return stbi_write_png(filename, bw, bh, 4, rgba_data, bw * 4) != 0;
}


struct Stats {
    const char * compressorName;
    Array<float> mseArray;
    Array<float> timeArray;
};


bool test_bc1(const char * inputFileName, int index, Stats * stats) {

    int w, h, n;
    unsigned char *input_data = stbi_load(inputFileName, &w, &h, &n, 4);
    defer { stbi_image_free(input_data); };

    if (input_data == nullptr) {
        printf("Failed to load input image '%s'.\n", inputFileName);
        return false;
    }


    int block_count = (w / 4) * (h / 4);
    u8 * rgba_block_data = (u8 *)malloc(block_count * 4 * 4 * 4);
    defer { free(rgba_block_data); };

    int bw = 4 * (w / 4); // Round down.
    int bh = 4 * (h / 4);

    // Convert to block layout.
    for (int y = 0, b = 0; y < bh; y += 4) {
        for (int x = 0; x < bw; x += 4, b++) {
            for (int yy = 0; yy < 4; yy++) {
                for (int xx = 0; xx < 4; xx++) {
                    if (x + xx < w && y + yy < h) {
                        rgba_block_data[b * 4 * 4 * 4 + (yy * 4 + xx) * 4 + 0] = input_data[((y + yy) * w + x + xx) * 4 + 0];
                        rgba_block_data[b * 4 * 4 * 4 + (yy * 4 + xx) * 4 + 1] = input_data[((y + yy) * w + x + xx) * 4 + 1];
                        rgba_block_data[b * 4 * 4 * 4 + (yy * 4 + xx) * 4 + 2] = input_data[((y + yy) * w + x + xx) * 4 + 2];
                        rgba_block_data[b * 4 * 4 * 4 + (yy * 4 + xx) * 4 + 3] = input_data[((y + yy) * w + x + xx) * 4 + 3];
                    }
                    else {
                        rgba_block_data[b * 4 * 4 * 4 + (yy * 4 + xx) * 4 + 0] = 0;
                        rgba_block_data[b * 4 * 4 * 4 + (yy * 4 + xx) * 4 + 1] = 0;
                        rgba_block_data[b * 4 * 4 * 4 + (yy * 4 + xx) * 4 + 2] = 0;
                        rgba_block_data[b * 4 * 4 * 4 + (yy * 4 + xx) * 4 + 3] = 0;
                    }
                }
            }
        }
    }

    u8 * block_data = (u8 *)malloc(block_count * 8);

    Timer timer;

    // Warmup.
    for (int b = 0; b < block_count; b++) {
        stb_compress_dxt_block(block_data + b * 8, rgba_block_data + b * 4 * 4 * 4, 0, STB_DXT_NORMAL);
    }

#if _DEBUG
    const int repeat_count = 1;
#else
    const int repeat_count = 1; // 8
#endif

    if (TEST_STB) {
        memset(block_data, 0, block_count * 8);

        timer.start();
        for (int i = 0; i < repeat_count; i++) {
            for (int b = 0; b < block_count; b++) {
                stb_compress_dxt_block(block_data + b * 8, rgba_block_data + b * 4 * 4 * 4, 0, STB_DXT_NORMAL);
            }
        }
        timer.stop();

        float mse = evaluate_dxt1_mse(rgba_block_data, block_data, block_count);

        if (stats) {
            stats->compressorName = "stb";
            stats->mseArray[index] = mse;
            stats->timeArray[index] = timer.elapsed() / repeat_count;
            stats++;
        }
        else {
            output_dxt_dds(bw, bh, block_data, "stb_dxt.dds");
        }
    }

    if (TEST_STB_HQ) {
        memset(block_data, 0, block_count * 8);

        timer.start();
        for (int i = 0; i < repeat_count; i++) {
            for (int b = 0; b < block_count; b++) {
                stb_compress_dxt_block(block_data + b * 8, rgba_block_data + b * 4 * 4 * 4, 0, STB_DXT_HIGHQUAL);
            }
        }
        timer.stop();

        float mse = evaluate_dxt1_mse(rgba_block_data, block_data, block_count);
        
        if (stats) {
            stats->compressorName = "stb-hq";
            stats->mseArray[index] = mse;
            stats->timeArray[index] = timer.elapsed() / repeat_count;
            stats++;
        }
        else {
            output_dxt_dds(bw, bh, block_data, "stb_dxt_hq.dds");
        }
    }

    if (TEST_GOOFY) {
        memset(block_data, 0, block_count * 8);

        timer.start();
        for (int i = 0; i < repeat_count; i++) {
            goofy::compressDXT1(block_data, input_data, w, h, w * 4);
        }
        timer.stop();

        float mse = evaluate_dxt1_mse(rgba_block_data, block_data, block_count);

        if (stats) {
            stats->compressorName = "goofy-dxt";
            stats->mseArray[index] = mse;
            stats->timeArray[index] = timer.elapsed() / repeat_count;
            stats++;
        }
        else {
            output_dxt_dds(bw, bh, block_data, "goofy_dxt.dds");
        }

        timer.start();
        for (int i = 0; i < repeat_count; i++) {
            goofy::compressETC1(block_data, input_data, w, h, w * 4);
        }
        timer.stop();

        mse = evaluate_dxt1_mse(rgba_block_data, block_data, block_count);

        if (stats) {
            stats->compressorName = "goofy-etc";
            stats->mseArray[index] = mse;
            stats->timeArray[index] = timer.elapsed() / repeat_count;
            stats++;
        }
        else {
            output_etc(bw, bh, block_data, "goofy_etc.png");
        }
    }

    if (TEST_RGBCX_FAST) {
        memset(block_data, 0, block_count * 8);

        rgbcx::init();

        for (int l = 0; l < 4; l++) {
            timer.start();
            for (int i = 0; i < repeat_count; i++) {
                for (int b = 0; b < block_count; b++) {
                    rgbcx::encode_bc1(l, (block_data + b * 8), rgba_block_data + b * 4 * 4 * 4, /*allow_3color=*/true, /*use_transparent_texels_for_black=*/true);
                }
            }
            timer.stop();

            float mse = evaluate_dxt1_mse(rgba_block_data, block_data, block_count);

            if (stats) {
                stats->compressorName = "rgbcx";
                stats->mseArray[index] = mse;
                stats->timeArray[index] = timer.elapsed() / repeat_count;
                stats++;
            }
            else {
                output_dxt_dds(bw, bh, block_data, "rgbcx.dds");
            }
        }
    }

    if (TEST_RGBCX_ALL) {
        memset(block_data, 0, block_count * 8);

        rgbcx::init();

        for (int l = 0; l < 19; l++) {
            timer.start();
            for (int i = 0; i < repeat_count; i++) {
                for (int b = 0; b < block_count; b++) {
                    rgbcx::encode_bc1(l, (block_data + b * 8), rgba_block_data + b * 4 * 4 * 4, /*allow_3color=*/true, /*use_transparent_texels_for_black=*/true);
                }
            }
            timer.stop();

            float mse = evaluate_dxt1_mse(rgba_block_data, block_data, block_count);

            if (stats) {
                stats->compressorName = "rgbcx";
                stats->mseArray[index] = mse;
                stats->timeArray[index] = timer.elapsed() / repeat_count;
                stats++;
            }
            else {
                output_dxt_dds(bw, bh, block_data, "rgbcx.dds");
            }
        }
    }

    bool three_color_mode = true;
    bool three_color_black = true;

    if (TEST_ICBC_FAST) {
        memset(block_data, 0, block_count * 8);
        float color_weights[3] = { 1, 1, 1 };

        timer.start();
        for (int i = 0; i < repeat_count; i++) {
            for (int b = 0; b < block_count; b++) {
                float input_colors[16*4];
                float input_weights[16];
                for (int j = 0; j < 16; j++) {
                    input_colors[4*j+0] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 0] / 255.0f;
                    input_colors[4*j+1] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 1] / 255.0f;
                    input_colors[4*j+2] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 2] / 255.0f;
                    input_colors[4*j+3] = 1.0f;
                    input_weights[j] = 1.0f;
                }

                icbc::compress_dxt1(icbc::Quality_Fast, input_colors, input_weights, color_weights, three_color_mode, three_color_black, (block_data + b * 8));
            }
        }
        timer.stop();

        float mse = evaluate_dxt1_mse(rgba_block_data, block_data, block_count);
        
        if (stats) {
            stats->compressorName = "icbc-fast";
            stats->mseArray[index] = mse;
            stats->timeArray[index] = timer.elapsed() / repeat_count;
            stats++;
        }
        else {
            output_dxt_dds(bw, bh, block_data, "icbc_fast.dds");
        }
    }

    if (TEST_ICBC) {
        memset(block_data, 0, block_count * 8);
        float color_weights[3] = { 1, 1, 1 };

        timer.start();
        for (int i = 0; i < repeat_count; i++) {
            for (int b = 0; b < block_count; b++) {
                float input_colors[16*4];
                float input_weights[16];
                for (int j = 0; j < 16; j++) {
                    input_colors[4*j+0] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 0] / 255.0f;
                    input_colors[4*j+1] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 1] / 255.0f;
                    input_colors[4*j+2] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 2] / 255.0f;
                    input_colors[4*j+3] = 1.0f;
                    input_weights[j] = 1.0f;
                }

                icbc::compress_dxt1(icbc::Quality_Default, input_colors, input_weights, color_weights, three_color_mode, three_color_black, (block_data + b * 8));
            }
        }
        timer.stop();

        float mse = evaluate_dxt1_mse(rgba_block_data, block_data, block_count);

        if (stats) {
            stats->compressorName = "icbc";
            stats->mseArray[index] = mse;
            stats->timeArray[index] = timer.elapsed() / repeat_count;
            stats++;
        }
        else {
            output_dxt_dds(bw, bh, block_data, "icbc.dds");
        }
    }

    if (TEST_ICBC_HQ) {
        memset(block_data, 0, block_count * 8);
        float color_weights[3] = { 1, 1, 1 };

        timer.start();
        for (int i = 0; i < repeat_count; i++) {
            for (int b = 0; b < block_count; b++) {
                float input_colors[16 * 4];
                float input_weights[16];
                for (int j = 0; j < 16; j++) {
                    input_colors[4 * j + 0] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 0] / 255.0f;
                    input_colors[4 * j + 1] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 1] / 255.0f;
                    input_colors[4 * j + 2] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 2] / 255.0f;
                    input_colors[4 * j + 3] = 1.0f;
                    input_weights[j] = 1.0f;
                }

                icbc::compress_dxt1(icbc::Quality_Max, input_colors, input_weights, color_weights, three_color_mode, three_color_black, (block_data + b * 8));
            }
        }
        timer.stop();

        float mse = evaluate_dxt1_mse(rgba_block_data, block_data, block_count);

        if (stats) {
            stats->compressorName = "icbc-hq";
            stats->mseArray[index] = mse;
            stats->timeArray[index] = timer.elapsed() / repeat_count;
            stats++;
        }
        else {
            output_dxt_dds(bw, bh, block_data, "icbc_hq.dds");
        }
    }

    if (TEST_ICBC_ALL) {
        memset(block_data, 0, block_count * 8);
        float color_weights[3] = { 1, 1, 1 };

        for (int l = 0; l <= icbc::Quality_Max; l++) {
            timer.start();
            for (int i = 0; i < repeat_count; i++) {
                for (int b = 0; b < block_count; b++) {
                    float input_colors[16 * 4];
                    float input_weights[16];
                    for (int j = 0; j < 16; j++) {
                        input_colors[4 * j + 0] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 0] / 255.0f;
                        input_colors[4 * j + 1] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 1] / 255.0f;
                        input_colors[4 * j + 2] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 2] / 255.0f;
                        input_colors[4 * j + 3] = 1.0f;
                        input_weights[j] = 1.0f;
                    }

                    icbc::compress_dxt1((icbc::Quality)l, input_colors, input_weights, color_weights, true, true, (block_data + b * 8));
                }
            }
            timer.stop();

            float mse = evaluate_dxt1_mse(rgba_block_data, block_data, block_count);

            if (stats) {
                stats->compressorName = "icbc";
                stats->mseArray[index] = mse;
                stats->timeArray[index] = timer.elapsed() / repeat_count;
                stats++;
            }
            else {
                output_dxt_dds(bw, bh, block_data, "icbc.dds");
            }
        }
    }

    if (TEST_IC_ETC) {
        memset(block_data, 0, block_count * 8);
        float color_weights[3] = { 1, 1, 1 };

        //int b_stop = (1132 / 4) * (bw / 4) + 1028 / 4;

        timer.start();
        for (int i = 0; i < repeat_count; i++) {
            for (int b = 0; b < block_count; b++) {
                float input_colors[16 * 4];
                float input_weights[16];
                for (int j = 0; j < 16; j++) {
                    input_colors[4 * j + 0] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 0] / 255.0f;
                    input_colors[4 * j + 1] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 1] / 255.0f;
                    input_colors[4 * j + 2] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 2] / 255.0f;
                    input_colors[4 * j + 3] = 1.0f;
                    input_weights[j] = 1.0f;
                }

                icetc::compress_etc2(input_colors, input_weights, color_weights, (block_data + b * 8));
            }
        }
        timer.stop();

        float mse = evaluate_etc2_mse(rgba_block_data, block_data, block_count);

        if (stats) {
            stats->compressorName = "ic_etc";
            stats->mseArray[index] = mse;
            stats->timeArray[index] = timer.elapsed() / repeat_count;
            stats++;
        }
        else {
            output_etc(bw, bh, block_data, "ic_etc.png");
        }
    }

    if (TEST_RG_ETC) {
        memset(block_data, 0, block_count * 8);
        float color_weights[3] = { 1, 1, 1 };

        rg_etc1::etc1_pack_params pack_params;
        pack_params.m_quality = rg_etc1::cLowQuality;
        //pack_params.m_quality = rg_etc1::cMediumQuality;
        //pack_params.m_quality = rg_etc1::cHighQuality;

        //int b_stop = (1132 / 4) * (bw / 4) + 1028 / 4;

        timer.start();
        for (int i = 0; i < repeat_count; i++) {
            for (int b = 0; b < block_count; b++) {
                float input_colors[16 * 4];
                float input_weights[16];
                for (int j = 0; j < 16; j++) {
                    input_colors[4 * j + 0] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 0] / 255.0f;
                    input_colors[4 * j + 1] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 1] / 255.0f;
                    input_colors[4 * j + 2] = rgba_block_data[b * 4 * 4 * 4 + j * 4 + 2] / 255.0f;
                    input_colors[4 * j + 3] = 1.0f;
                    input_weights[j] = 1.0f;
                }

                rg_etc1::pack_etc1_block((block_data + b * 8), (const uint*)rgba_block_data + b * 4 * 4, pack_params);
            }
        }
        timer.stop();

        float mse = evaluate_etc2_mse(rgba_block_data, block_data, block_count);

        if (stats) {
            stats->compressorName = "rg_etc";
            stats->mseArray[index] = mse;
            stats->timeArray[index] = timer.elapsed() / repeat_count;
            stats++;
        }
        else {
            output_etc(bw, bh, block_data, "rg_etc.png");
        }
    }

    if (TEST_SQUISH) {
        memset(block_data, 0, block_count * 8);

        timer.start();
        for (int i = 0; i < repeat_count; i++) {
            for (int b = 0; b < block_count; b++) {
                squish::Compress(rgba_block_data + b * 4 * 4 * 4, block_data + b * 8, squish::kDxt1);
            }
        }
        timer.stop();

        float mse = evaluate_dxt1_mse(rgba_block_data, block_data, block_count);

        if (stats) {
            stats->compressorName = "squish";
            stats->mseArray[index] = mse;
            stats->timeArray[index] = timer.elapsed() / repeat_count;
            stats++;
        }
        else {
            output_dxt_dds(bw, bh, block_data, "squish.dds");
        }
    }

    if (TEST_SQUISH_HQ) {
        memset(block_data, 0, block_count * 8);

        timer.start();
        for (int i = 0; i < repeat_count; i++) {
            for (int b = 0; b < block_count; b++) {
                squish::Compress(rgba_block_data + b * 4 * 4 * 4, block_data + b * 8, squish::kDxt1 | squish::kColourIterativeClusterFit);
            }
        }
        timer.stop();

        float mse = evaluate_dxt1_mse(rgba_block_data, block_data, block_count);

        if (stats) {
            stats->compressorName = "squish-hq";
            stats->mseArray[index] = mse;
            stats->timeArray[index] = timer.elapsed() / repeat_count;
            stats++;
        }
        else {
            output_dxt_dds(bw, bh, block_data, "squish_hq.dds");
        }
    }

    if (TEST_AMD_CMP) {
        memset(block_data, 0, block_count * 8);

        void *options;
        CreateOptionsBC1(&options);
        SetQualityBC1(options, 1.0); // It seems AMD has only two compressors for 0.0 and 1.0 values.

        timer.start();
        for (int i = 0; i < repeat_count; i++) {
            for (int b = 0; b < block_count; b++) {
                CompressBlockBC1(rgba_block_data + b * 4 * 4 * 4, 16, block_data + b * 8, options);
            }
        }
        timer.stop();

        DestroyOptionsBC1(options);

        float mse = evaluate_dxt1_mse(rgba_block_data, block_data, block_count);

        if (stats) {
            stats->compressorName = "cmp";
            stats->mseArray[index] = mse;
            stats->timeArray[index] = timer.elapsed() / repeat_count;
            stats++;
        }
        else {
            output_dxt_dds(bw, bh, block_data, "squish.dds");
        }
    }

    return false;
}



bool analyze_bc1(const char * inputFileName) {

    int w, h, n;
    unsigned char *input_data = stbi_load(inputFileName, &w, &h, &n, 4);
    defer { stbi_image_free(input_data); };

    if (input_data == nullptr) {
        printf("Failed to load input image '%s'.\n", inputFileName);
        return false;
    }

    int block_count = (w / 4) * (h / 4);
    u8 * rgba_block_data = (u8 *)malloc(block_count * 4 * 4 * 4);
    defer { free(rgba_block_data); };

    int bw = 4 * (w / 4); // Round down.
    int bh = 4 * (h / 4);

    // Convert to block layout.
    for (int y = 0, b = 0; y < bh; y += 4) {
        for (int x = 0; x < bw; x += 4, b++) {
            for (int yy = 0; yy < 4; yy++) {
                for (int xx = 0; xx < 4; xx++) {
                    if (x + xx < w && y + yy < h) {
                        rgba_block_data[b * 4 * 4 * 4 + (yy * 4 + xx) * 4 + 0] = input_data[((y + yy) * w + x + xx) * 4 + 0];
                        rgba_block_data[b * 4 * 4 * 4 + (yy * 4 + xx) * 4 + 1] = input_data[((y + yy) * w + x + xx) * 4 + 1];
                        rgba_block_data[b * 4 * 4 * 4 + (yy * 4 + xx) * 4 + 2] = input_data[((y + yy) * w + x + xx) * 4 + 2];
                        rgba_block_data[b * 4 * 4 * 4 + (yy * 4 + xx) * 4 + 3] = input_data[((y + yy) * w + x + xx) * 4 + 3];
                    }
                    else {
                        rgba_block_data[b * 4 * 4 * 4 + (yy * 4 + xx) * 4 + 0] = 0;
                        rgba_block_data[b * 4 * 4 * 4 + (yy * 4 + xx) * 4 + 1] = 0;
                        rgba_block_data[b * 4 * 4 * 4 + (yy * 4 + xx) * 4 + 2] = 0;
                        rgba_block_data[b * 4 * 4 * 4 + (yy * 4 + xx) * 4 + 3] = 0;
                    }
                }
            }
        }
    }

    u8 * block_data = (u8 *)malloc(block_count * 8);
    memset(block_data, 0, block_count * 8);

    Timer timer;

    int stb_better_than_icbc_fast = 0;
    int stb_better_than_icbc = 0;
    int stb_better_than_icbc_hq = 0;
    int squish_better_than_icbc_hq = 0;

    int icbc_hq_wins = 0;

    int this_should_never_happen = 0;
        
    float color_weights[3] = { 1, 1, 1 };

    for (int b = 0; b < block_count; b++) {

        uint8 * rgba_block = rgba_block_data + b * 4 * 4 * 4;
        uint8 * dxt_block = block_data + b * 8;

        float input_colors[16*4];
        float input_weights[16];
        for (int j = 0; j < 16; j++) {
            input_colors[4*j+0] = rgba_block[j * 4 + 0] / 255.0f;
            input_colors[4*j+1] = rgba_block[j * 4 + 1] / 255.0f;
            input_colors[4*j+2] = rgba_block[j * 4 + 2] / 255.0f;
            input_colors[4*j+3] = 255.0f;
            input_weights[j] = 1.0f;
        }

        // Compare all the different modes on the same block:

        stb_compress_dxt_block(dxt_block, rgba_block, 0, STB_DXT_NORMAL);
        float mse_stb = icbc::evaluate_dxt1_error(rgba_block, dxt_block);

        stb_compress_dxt_block(dxt_block, rgba_block, 0, STB_DXT_HIGHQUAL);
        float mse_stb_hq = icbc::evaluate_dxt1_error(rgba_block, dxt_block);

        icbc::compress_dxt1(icbc::Quality_Fast, input_colors, input_weights, color_weights, true, true, dxt_block);
        float mse_icbc_fast = icbc::evaluate_dxt1_error(rgba_block, dxt_block);

        icbc::compress_dxt1(icbc::Quality_Default, input_colors, input_weights, color_weights, true, true, dxt_block);
        float mse_icbc = icbc::evaluate_dxt1_error(rgba_block, dxt_block);

        icbc::compress_dxt1(icbc::Quality_Max, input_colors, input_weights, color_weights, true, true, dxt_block);
        float mse_icbc_hq = icbc::evaluate_dxt1_error(rgba_block, dxt_block);

        squish::Compress(rgba_block, dxt_block, squish::kDxt1);
        float mse_squish = icbc::evaluate_dxt1_error(rgba_block, dxt_block);

        squish::Compress(rgba_block, dxt_block, squish::kDxt1 | squish::kColourIterativeClusterFit);
        float mse_squish_hq = icbc::evaluate_dxt1_error(rgba_block, dxt_block);

        if (mse_stb < mse_icbc_fast) {
            stb_better_than_icbc_fast++;
        }
        if (mse_stb < mse_icbc) {
            stb_better_than_icbc++;
        }
        if (mse_stb < mse_icbc_hq) {
            stb_better_than_icbc_hq++;
        }
        if (mse_icbc_hq < mse_icbc) {
            icbc_hq_wins++;
        }
        if (mse_squish < mse_icbc_hq) {
            squish_better_than_icbc_hq++;
        }
        if (mse_icbc_fast < mse_icbc_hq) {
            this_should_never_happen++;
        }
    }

    return true;
}



static float mse_to_psnr(float mse) {
    float rms = sqrtf(mse);
    float psnr = rms ? (float)clamp(log10(255.0 / rms) * 20.0, 0.0, 300.0) : 1e+10f;
    return psnr;
}


const char * image_set[] = {
    "testsuite/kodak/kodim01.png",
    "testsuite/kodak/kodim02.png",
    "testsuite/kodak/kodim03.png",
    "testsuite/kodak/kodim04.png",
    "testsuite/kodak/kodim05.png",
    "testsuite/kodak/kodim06.png",
    "testsuite/kodak/kodim07.png",
    "testsuite/kodak/kodim08.png",
    "testsuite/kodak/kodim09.png",
    "testsuite/kodak/kodim10.png",
    "testsuite/kodak/kodim11.png",
    "testsuite/kodak/kodim12.png",
    "testsuite/kodak/kodim13.png",
    "testsuite/kodak/kodim14.png",
    "testsuite/kodak/kodim15.png",
    "testsuite/kodak/kodim16.png",
    "testsuite/kodak/kodim17.png",
    "testsuite/kodak/kodim18.png",
    "testsuite/kodak/kodim19.png",
    "testsuite/kodak/kodim20.png",
    "testsuite/kodak/kodim21.png",
    "testsuite/kodak/kodim22.png",
    "testsuite/kodak/kodim23.png",
    "testsuite/kodak/kodim24.png",
    "testsuite/waterloo/clegg.png",
    "testsuite/waterloo/frymire.png",
    "testsuite/waterloo/lena.png",
    "testsuite/waterloo/monarch.png",
    "testsuite/waterloo/peppers.png",
    "testsuite/waterloo/sail.png",
    "testsuite/waterloo/serrano.png",
    "testsuite/waterloo/tulips.png",
    //"testsuite/artificial.png",
};

const char * roblox_set[] = {
    "Roblox/asphalt_side/diffuse.tga",
    "Roblox/asphalt_top/diffuse.tga",
    "Roblox/basalt/diffuse.tga",
    "Roblox/brick/diffuse.tga",
    "Roblox/cobblestone_side/diffuse.tga",
    "Roblox/cobblestone_top/diffuse.tga",
    "Roblox/concrete_side/diffuse.tga",
    "Roblox/concrete_top/diffuse.tga",
    "Roblox/crackedlava/diffuse.tga",
    "Roblox/glacier_bottom/diffuse.tga",
    "Roblox/glacier_side/diffuse.tga",
    "Roblox/glacier_top/diffuse.tga",
    "Roblox/grass_bottom/diffuse.tga",
    "Roblox/grass_side/diffuse.tga",
    "Roblox/grass_top/diffuse.tga",
    "Roblox/ground/diffuse.tga",
    "Roblox/ice_side/diffuse.tga",
    "Roblox/ice_top/diffuse.tga",
    "Roblox/leafygrass_side/diffuse.tga",
    "Roblox/leafygrass_top/diffuse.tga",
    "Roblox/limestone_side/diffuse.tga",
    "Roblox/limestone_top/diffuse.tga",
    "Roblox/mud/diffuse.tga",
    "Roblox/pavement_side/diffuse.tga",
    "Roblox/pavement_top/diffuse.tga",
    "Roblox/rock/diffuse.tga",
    "Roblox/salt_side/diffuse.tga",
    "Roblox/salt_top/diffuse.tga",
    "Roblox/sand_side/diffuse.tga",
    "Roblox/sand_top/diffuse.tga",
    "Roblox/sandstone_bottom/diffuse.tga",
    "Roblox/sandstone_side/diffuse.tga",
    "Roblox/sandstone_top/diffuse.tga",
    "Roblox/slate/diffuse.tga",
    "Roblox/snow/diffuse.tga",
    "Roblox/woodplanks/diffuse.tga",
};


int main(int argc, char *argv[])
{
    //const char * inputFileName = "testsuite/artificial.png";
    //const char * inputFileName = "testsuite/kodak/kodim14.png";
    const char * inputFileName = "testsuite/kodak/kodim18.png";
    //const char * inputFileName = "testsuite/kodak/kodim15.png";
    //const char * inputFileName = "testsuite/waterloo/frymire.png";
    //const char * inputFileName = "Roblox/leafygrass_top/diffuse.tga";

    icbc::init_dxt1();
    icetc::init();
    rgbcx::init();
    rg_etc1::pack_etc1_block_init();

    //test_bc1(inputFileName, 0, NULL);
    //analyze_bc1(inputFileName);

    //const char ** set = roblox_set;
    //int count = sizeof(roblox_set) / sizeof(char*);

    const char ** set = image_set;
    int count = sizeof(image_set) / sizeof(char*);

    const int MAX_COMPRESSOR_COUNT = 64;
    Stats stats[MAX_COMPRESSOR_COUNT];

    for (int i = 0; i < MAX_COMPRESSOR_COUNT; i++) {
        stats[i].compressorName = nullptr;
        stats[i].mseArray.resize(count, 0.0f);
        stats[i].timeArray.resize(count, 0.0f);
    }

    for (int i = 0; i < count; i++) {
        printf("\nImage '%s'\n", set[i]);

        test_bc1(set[i], i, stats);

        for (int c = 0; c < MAX_COMPRESSOR_COUNT; c++) {
            if (stats[c].compressorName) {
                printf("%-16s %f\t%f\t%f\n", stats[c].compressorName, sqrtf(stats[c].mseArray[i]), mse_to_psnr(stats[c].mseArray[i]), stats[c].timeArray[i]);
            }
        }
    }

    // Print stats.
    printf("\nAverage Results:\n");
    for (int c = 0; c < MAX_COMPRESSOR_COUNT; c++) {
        if (stats[c].compressorName) {
            float sum = 0.0f;
            for (float it : stats[c].mseArray) {
                sum += it;
            }
            sum /= count;

            float time = 0.0f;
            for (float it : stats[c].timeArray) {
                time += it;
            }

            printf("%-16s %f\t%f\t%f\n", stats[c].compressorName, sqrtf(sum), mse_to_psnr(sum), time);
        }
    }

    for (int i = 0; i < 16; i++) {
        printf("%d, ", icbc::s_fourClusterTotal[i]);
    }
    printf("\n");

    for (int i = 0; i < 16; i++) {
        printf("%d, ", icbc::s_threeClusterTotal[i]);
    }
    printf("\n\n");

    /*
    for (int i = 0; i < 968; i++) {
        printf("%lld, ", icbc::s_fourClusterHistogram[i]);
    } printf("\n\n");

    for (int i = 0; i < 968; i++) {
        printf("%d, ", icbc::s_fourCluster[i].c0);
    } printf("\n\n");
    
    for (int i = 0; i < 968; i++) {
        printf("%d, ", icbc::s_fourCluster[i].c1 - icbc::s_fourCluster[i].c0);
    } printf("\n\n");
    
    for (int i = 0; i < 968; i++) {
        printf("%d, ", icbc::s_fourCluster[i].c2 - icbc::s_fourCluster[i].c1);
    }
    */
    printf("\n");


    /*for (int c = 0; c < MAX_COMPRESSOR_COUNT; c++) {
        if (stats[c].compressorName) {
            printf("%s:\n", stats[c].compressorName);
            for (int i = 0; i < count; i++) {
                printf("%f, ", mse_to_psnr(stats[c].mseArray[i]));
            }
            puts("\n");
            
            for (int i = 0; i < count; i++) {
                printf("%f, ", stats[c].timeArray[i]);
            }
            puts("\n");
        }
    }*/


    return EXIT_SUCCESS;
}
