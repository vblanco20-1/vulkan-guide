#include "CompressorDXT5_RGBM.h"
#include "icbc.h"

#include "OptimalCompressDXT.h"
#include "QuickCompressDXT.h"
#include "CompressorETC.h"

#include "nvimage/ColorBlock.h"
#include "nvimage/BlockDXT.h"

#include "nvmath/Color.inl"
#include "nvmath/Vector.inl"
#include "nvmath/Fitting.h"
#include "nvmath/ftoi.h"

#include "nvthread/Atomic.h"
#include <stdio.h>

using namespace nv;



static void convert_to_rgbm(const Vector4 input_colors[16], const float input_weights[16], float min_m, Vector4 rgbm_colors[16], float rgb_weights[16]) {
    
    float weight_sum = 0;
    
    for (uint i = 0; i < 16; i++) {
        const Vector4 & c = input_colors[i];
        
        float R = saturate(c.x);
        float G = saturate(c.y);
        float B = saturate(c.z);
        
        float M = max(max(R, G), max(B, min_m));
        float r = R / M;
        float g = G / M;
        float b = B / M;
        float a = (M - min_m) / (1 - min_m);
        
        rgbm_colors[i] = Vector4(r, g, b, a);
        rgb_weights[i] = input_weights[i] * M;
        weight_sum += input_weights[i];
    }
    
    if (weight_sum == 0) {
        for (uint i = 0; i < 16; i++) rgb_weights[i] = 1;
    }
    
}


//static uint atomic_counter = 0;

float nv::compress_dxt5_rgbm(const Vector4 input_colors[16], const float input_weights[16], float min_m, BlockDXT5 * output) {

    // Convert to RGBM.
    Vector4 input_colors_rgbm[16]; // @@ Write over input_colors?
    float rgb_weights[16];
    convert_to_rgbm(input_colors, input_weights, min_m, input_colors_rgbm, rgb_weights);

    float color_weights[3] = { 1.0f,1.0f,1.0f };

    // Compress RGB.
    icbc::compress_dxt1(icbc::Quality_Default, (float *)input_colors_rgbm, rgb_weights, color_weights, /*three_color_mode=*/false, /*hq=*/false, &output->color);

    // Decompress RGB/M block.
    nv::ColorBlock RGB;
    output->color.decodeBlock(&RGB);

    // Compute M values to compensate for RGB's error.
    AlphaBlock4x4 M;
    for (int i = 0; i < 16; i++) {
        const Vector4 & c = input_colors[i];

        float R = saturate(c.x);
        float G = saturate(c.y);
        float B = saturate(c.z);

        float rm = RGB.color(i).r / 255.0f;
        float gm = RGB.color(i).g / 255.0f;
        float bm = RGB.color(i).b / 255.0f;

        // compute m such that m * (r/M, g/M, b/M) == RGB
    
        // Three equations, one unknown:
        //  m * r/M == R
        //  m * g/M == G
        //  m * b/M == B
        
        // Solve in the least squares sense!

        // m (rm gm bm) (rm gm bm)^T == (rm gm bm) (R G B)^T

        // m == dot(rgb, RGB) / dot(rgb, rgb)

        float m = dot(Vector3(rm, gm, bm), Vector3(R, G, B)) / dot(Vector3(rm, gm, bm), Vector3(rm, gm, bm));

        m = (m - min_m) / (1 - min_m);

#if 0
        // IC: This does indeed happen. What does that mean? The best choice of m is above the available range. If this happened too often it would make sense to scale m in
        // the pixel shader to allow for more accurate reconstruction. However, that scaling would reduce the precision over the [0-1] range. I haven't measured how much
        // error is introduced by the clamping vs. how much the error would change with the increased range.
        if (m > 1.0f) {
            uint counter = atomicIncrement(&atomic_counter);
            printf("It happens %u times!", counter);
        }
#endif

        M.alpha[i] = U8(ftoi_round(saturate(m) * 255.0f));
        M.weights[i] = input_weights[i];
    }

    // Compress M.
    //if (compressionOptions.quality == Quality_Fastest) {
    //    QuickCompress::compressDXT5A(M, &output->alpha);
    /*}
    else {*/
        OptimalCompress::compressDXT5A(M, &output->alpha);
    //}


#if 0   // Multiple iterations do not seem to help.
    // Decompress M.
    output->alpha.decodeBlock(&M);

    // Feed it back to the input RGB block.
    for (uint i = 0; i < 16; i++) {
        const Vector4 & c = input_colors[i];

        float R = saturate(c.x);
        float G = saturate(c.y);
        float B = saturate(c.z);

        float m = float(M.alpha[i]) / 255.0f * (1 - min_m) + min_m;

        float r = R / m;
        float g = G / m;
        float b = B / m;
        float a = float(M.alpha[i]) / 255.0f;

        input_colors_rgbm[i] = Vector4(r, g, b, a);
        rgb_weights[i] = input_weights[i] * m;
    }
#endif

    return 0; // @@ 
}


float nv::compress_etc2_rgbm(Vector4 input_colors[16], float input_weights[16], float min_m, void * output) {
    
    // Convert to RGBM.
    Vector4 rgbm_colors[16];
    float rgb_weights[16];
    convert_to_rgbm(input_colors, input_weights, min_m, rgbm_colors, rgb_weights);
    
    void * etc_output = (uint8 *)output + 8;
    void * eac_output = output;
    
    // Compress RGB.
    compress_etc2(rgbm_colors, rgb_weights, Vector3(1), etc_output);
    
    // Decompress RGB/M block.
    decompress_etc(etc_output, rgbm_colors);
    
    // Compute M values to compensate for RGB's error.
    for (int i = 0; i < 16; i++) {
        const Vector4 & c = input_colors[i];
        
        float R = saturate(c.x);
        float G = saturate(c.y);
        float B = saturate(c.z);
        
        float rm = rgbm_colors[i].x;
        float gm = rgbm_colors[i].y;
        float bm = rgbm_colors[i].z;
        
        // compute m such that m * (r/M, g/M, b/M) == RGB
        
        // Three equations, one unknown:
        //  m * r/M == R
        //  m * g/M == G
        //  m * b/M == B
        
        // Solve in the least squares sense!
        
        // m (rm gm bm) (rm gm bm)^T == (rm gm bm) (R G B)^T
        
        // m == dot(rgb, RGB) / dot(rgb, rgb)
        
        float m = dot(Vector3(rm, gm, bm), Vector3(R, G, B)) / dot(Vector3(rm, gm, bm), Vector3(rm, gm, bm));
        if (!isFinite(m)) {
            m = 1;
        }

        m = (m - min_m) / (1 - min_m);
        
        // Store M in alpha channel.
        rgbm_colors[i].w = saturate(m);     // @@ What it we don't saturate?
    }
    
    // Compress M.
    compress_eac(rgbm_colors, input_weights, /*input_channel=*/3, /*search_radius=*/1, /*11bit_mode*/false, eac_output);

    return 0; // @@ Compute error.
}

