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


int main(int argc, char *argv[])
{
    if (argc != 2) return EXIT_FAILURE;

    const char * inputFileName = argv[1];

    // Init context.
    nvtt::Context context;
    context.enableCudaAcceleration(false);

    // Load input image.
    nvtt::Surface image;
    if (!image.load(inputFileName)) {
        return EXIT_FAILURE;
    }

    // Setup compression options.
    nvtt::CompressionOptions compressionOptions;
    compressionOptions.setFormat(nvtt::Format_BC3);
    //compressionOptions.setFormat(nvtt::Format_RGBA);

    // Setup output options.
    nvtt::OutputOptions outputOptions;

    nv::Path outputFileName(inputFileName);
    outputFileName.stripExtension();
    outputFileName.append(".dds");

    outputOptions.setFileName(outputFileName.str());

    // Output compressed image.
    context.outputHeader(image, image.countMipmaps(), compressionOptions, outputOptions);

    image.flipY();
    image.setAlphaMode(nvtt::AlphaMode_Transparency);

    // Output first mipmap.
    context.compress(image, 0, 0, compressionOptions, outputOptions);

    float gamma = 2.2f;
    image.toLinear(gamma);

    float alphaRef = 0.95f;
    float coverage = image.alphaTestCoverage(alphaRef);

    // Build mimaps.
    int m = 1;
    while (image.buildNextMipmap(nvtt::MipmapFilter_Kaiser))
    {
        nvtt::Surface tmpImage = image;
        tmpImage.toGamma(gamma);

        tmpImage.scaleAlphaToCoverage(coverage, alphaRef);

        context.compress(tmpImage, 0, m, compressionOptions, outputOptions);
        m++;
    }

    return EXIT_SUCCESS;
}

