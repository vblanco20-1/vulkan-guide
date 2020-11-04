// Copyright NVIDIA Corporation 2007 -- Ignacio Castano <icastano@nvidia.com>
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

//#include <nvcore/StrLib.h>
#include <nvcore/Timer.h>
#include <nvtt/nvtt.h>
#include <nvmath/nvmath.h>

#include "../tools/cmdline.h"

#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h> // printf


int main(int argc, char *argv[])
{
    MyAssertHandler assertHandler;
    MyMessageHandler messageHandler;

    // Init context.
    nvtt::Context context;

    const char * fileName = "envmap.dds";
    if (argc > 1) fileName = argv[1];

    // Load cubemap.
    nvtt::CubeSurface envmap;
    if (!envmap.load(fileName, 0)) {
        printf("Error loading envmap.dds\n");
        return EXIT_FAILURE;
    }

    //envmap.toLinear(2.2f);


    // Setup compression options.
    nvtt::CompressionOptions compressionOptions;
    compressionOptions.setFormat(nvtt::Format_RGBA);
    compressionOptions.setPixelType(nvtt::PixelType_Float);
    compressionOptions.setPixelFormat(16, 16, 16, 16);


    // Setup output options.
    nvtt::OutputOptions outputOptions;
    outputOptions.setFileName("filtered_envmap.dds");
    //outputOptions.setSrgbFlag(true);


    const int MAX_MIPMAP_COUNT = 7; // nv::log2(64) + 1;
    //const int mipmapCount = MAX_MIPMAP_COUNT;
    const int mipmapCount = 4;
    //const int mipmapCount = 1;
    const int firstMipmap = 0;

    int topSize = 64;
    float topPower = 64;

    // Output header.
    context.outputHeader(nvtt::TextureType_Cube, topSize >> firstMipmap, topSize >> firstMipmap, 1, 1, mipmapCount-firstMipmap, false, compressionOptions, outputOptions);

    nv::Timer timer;
    timer.start();

    nvtt::CubeSurface filteredEnvmap[mipmapCount];

    // Output filtered mipmaps.
    for (int m = firstMipmap; m < mipmapCount; m++) {
        int size = topSize >> m;                            // 64, 32, 16, 8
        float cosine_power = topPower / (1 << (2 * m));     // 64, 16,  4, 1
        cosine_power = nv::max(1.0f, cosine_power);

        printf("filtering step: %d/%d\n", m+1, mipmapCount);

        filteredEnvmap[m] = envmap.cosinePowerFilter(size, cosine_power, nvtt::EdgeFixup_Warp);
        //filteredEnvmap[m].toGamma(2.2f);
    }

    for (int f = 0; f < 6; f++) {
        for (int m = firstMipmap; m < mipmapCount; m++) {
            context.compress(filteredEnvmap[m].face(f), f, m-firstMipmap, compressionOptions, outputOptions);
        }
    }

    timer.stop();

    printf("done in %f seconds\n", timer.elapsed());

    return EXIT_SUCCESS;
}

