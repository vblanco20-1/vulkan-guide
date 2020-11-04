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

#include <nvcore/StrLib.h>
#include <nvtt/nvtt.h>

#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h> // printf

int main(int argc, char *argv[])
{
    if (argc != 2 || argc != 3) {
        printf("Usage:\n  process_alpha_map color_map [normal_map].\n");
        return EXIT_FAILURE;
    }

    const char * inputFileNameColor = argv[1];
    const char * inputFileNameNormal = NULL;
    if (argc == 3) inputFileNameNormal = argv[2];

    // Init context.
    nvtt::Context context;
    context.enableCudaAcceleration(false);

    // Load color map.
    nvtt::Surface colorMap;
    if (!colorMap.load(inputFileNameColor)) {
        printf("Image '%s' could not be loaded.\n", inputFileNameColor);
        return EXIT_FAILURE;
    }

    // Setup compression options.
    nvtt::CompressionOptions colorCompressionOptions;
    colorCompressionOptions.setFormat(nvtt::Format_BC3);

    // Setup output options.
    nvtt::OutputOptions colorOutputOptions;

    nv::Path outputFileNameColor(inputFileNameColor);
    outputFileNameColor.stripExtension();
    outputFileNameColor.append(".dds");

    colorOutputOptions.setFileName(outputFileNameColor.str());

    // Load normal map.
    nvtt::Surface normalMap;
    if (inputFileNameNormal != NULL) {
        if (!normalMap.load(inputFileNameColor)) {
            printf("Image '%s' could not be loaded.\n", inputFileNameNormal);
            return EXIT_FAILURE;
        }
        
        if (colorMap.width() != normalMap.width() || colorMap.height() != normalMap.height()) {
            printf("Image '%s' and '%s' do not have the same extents.\n", inputFileNameColor, inputFileNameNormal);
            return EXIT_FAILURE;
        }
    }

    // Setup compression options.
    nvtt::CompressionOptions normalCompressionOptions;
    normalCompressionOptions.setFormat(nvtt::Format_BC3n);

    // Setup output options.
    nvtt::OutputOptions normalOutputOptions;

    if (inputFileNameNormal != NULL) {
        nv::Path outputFileNameNormal(inputFileNameNormal);
        outputFileNameNormal.stripExtension();
        outputFileNameNormal.append(".dds");

        normalOutputOptions.setFileName(outputFileNameNormal.str());
    }


    // Output header and first mipmap.
    context.outputHeader(colorMap, colorMap.countMipmaps(), colorCompressionOptions, colorOutputOptions);

    colorMap.flipY();
    colorMap.setAlphaMode(nvtt::AlphaMode_Transparency);

    context.compress(colorMap, 0, 0, colorCompressionOptions, colorOutputOptions);
    
    if (inputFileNameNormal != NULL) {
        context.outputHeader(normalMap, normalMap.countMipmaps(), normalCompressionOptions, normalOutputOptions);

        normalMap.flipY();
        normalMap.setAlphaMode(nvtt::AlphaMode_Transparency);
        normalMap.normalizeNormalMap();
        normalMap.copyChannel(colorMap, 3); // Copy alpha channel from color to normal map.
        
        context.compress(normalMap, 0, 0, normalCompressionOptions, normalOutputOptions);
    }

    const float gamma = 2.2f;
    colorMap.toLinear(gamma);

    const float alphaRef = 0.95f;
    const float coverage = colorMap.alphaTestCoverage(alphaRef);

    // Build and output mipmaps.
    int m = 1;
    while (colorMap.buildNextMipmap(nvtt::MipmapFilter_Kaiser))
    {
        colorMap.scaleAlphaToCoverage(coverage, alphaRef);
            
        nvtt::Surface tmpColorMap = colorMap;
        tmpColorMap.toGamma(gamma);

        context.compress(tmpColorMap, 0, m, colorCompressionOptions, colorOutputOptions);
        
        if (inputFileNameNormal != NULL) {
            normalMap.buildNextMipmap(nvtt::MipmapFilter_Kaiser);
            normalMap.normalizeNormalMap();
            normalMap.copyChannel(tmpColorMap, 3);
                        
            context.compress(normalMap, 0, m, normalCompressionOptions, normalOutputOptions);            
        }

        m++;
    }

    return EXIT_SUCCESS;
}

