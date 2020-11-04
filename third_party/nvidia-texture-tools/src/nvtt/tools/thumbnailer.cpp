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

#include <nvcore/Ptr.h>
#include <nvcore/StrLib.h>
#include <nvcore/StdStream.h>
#include <nvcore/Array.inl>

#include <nvimage/Image.h>
#include <nvimage/ImageIO.h>
#include <nvimage/FloatImage.h>
#include <nvimage/Filter.h>
#include <nvimage/DirectDrawSurface.h>

#include <nvmath/Color.h>
#include <nvmath/Vector.h>

#include <math.h>

#include "cmdline.h"

static bool loadImage(nv::Image & image, const char * fileName)
{
    if (nv::strCaseDiff(nv::Path::extension(fileName), ".dds") == 0)
    {
        nv::DirectDrawSurface dds;
        if (!dds.load(fileName) || !dds.isValid())
        {
            fprintf(stderr, "The file '%s' is not a valid DDS file.\n", fileName);
            return false;
        }

        return imageFromDDS(&image, dds, 0, 0); // get first image
    }
    else
    {
        // Regular image.
        if (!image.load(fileName))
        {
                fprintf(stderr, "The file '%s' is not a supported image type.\n", fileName);
                return false;
        }

        return true;
    }
}


int main(int argc, char *argv[])
{
    //MyAssertHandler assertHandler;
    MyMessageHandler messageHandler;

    float gamma = 2.2f;
    nv::Path input;
    nv::Path output;
    uint size = 128;

    // Parse arguments.
    for (int i = 1; i < argc; i++)
    {
        // Input options.
        if (strcmp("-s", argv[i]) == 0)
        {
            if (i+1 < argc && argv[i+1][0] != '-') {
                size = (uint)atoi(argv[i+1]);
                i++;
            }
        }
        else if (argv[i][0] != '-')
        {
            input = argv[i];

            if (i+1 < argc && argv[i+1][0] != '-') {
                output = argv[i+1];
            }
            else {
                fprintf(stderr, "No output filename.\n");
                return 1;
            }

            break;
        }
    }

    if (input.isNull() || output.isNull())
    {
        printf("NVIDIA Texture Tools - Copyright NVIDIA Corporation 2007\n\n");

        printf("usage: nv-gnome-thumbnailer [options] input output\n\n");

        printf("Options:\n");
        printf("  -s size\tThumbnail size (default = 128)\n");

        return 1;
    }

    nv::Image image;
    if (!loadImage(image, input.str())) return 1;

    nv::StringBuilder widthString;
    widthString.number(image.width);
    nv::StringBuilder heightString;
    heightString.number(image.height);

    nv::Array<const char *> metaData;
    metaData.append("Thumb::Image::Width");
    metaData.append(widthString.str());
    metaData.append("Thumb::Image::Height");
    metaData.append(heightString.str());
    metaData.append(NULL);
    metaData.append(NULL);

    if ((image.width > size) || (image.height > size))
    {
        nv::FloatImage fimage(&image);
        fimage.toLinear(0, 3, gamma);

        uint thumbW, thumbH;
        if (image.width > image.height)
        {
            thumbW = size;
            thumbH = uint ((float (image.height) / float (image.width)) * size);
        }
        else
        {
            thumbW = uint ((float (image.width) / float (image.height)) * size);
            thumbH = size;
        }
        nv::AutoPtr<nv::FloatImage> fresult(fimage.resize(nv::BoxFilter(), thumbW, thumbH, nv::FloatImage::WrapMode_Clamp));

        nv::AutoPtr<nv::Image> result(fresult->createImageGammaCorrect(gamma));
        result->format = nv::Image::Format_ARGB;

        nv::StdOutputStream stream(output.str());
        nv::ImageIO::save(output.str(), stream, result.ptr(), metaData.buffer());
    }
    else
    {
        nv::StdOutputStream stream(output.str());
        nv::ImageIO::save(output.str(), stream, &image, metaData.buffer());
    }

    return 0;
}

