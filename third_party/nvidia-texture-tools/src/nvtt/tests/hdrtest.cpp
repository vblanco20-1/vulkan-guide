// Copyright (c) 2009-2011 Ignacio Castano <castano@gmail.com>
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

#include <nvtt/nvtt.h>
#include <nvimage/Image.h>
#include <nvimage/ImageIO.h>
#include <nvimage/BlockDXT.h>
#include <nvimage/ColorBlock.h>
#include <nvcore/Ptr.h>
#include <nvcore/Debug.h>
#include <nvcore/StrLib.h>
#include <nvcore/StdStream.h>
#include <nvcore/TextWriter.h>
#include <nvcore/FileSystem.h>
#include <nvcore/Timer.h>
#include <nvcore/Array.inl>

#include <stdlib.h> // free
#include <string.h> // memcpy
#include <float.h> // FLT_MAX

#include "../tools/cmdline.h"

#include "GoogleCharts.h"

using namespace nv;
using namespace nvtt;

static const char * s_hdrImageSet[] = {
    "specruin.dds",
    "cottage.dds",
    "tower.dds",
};


struct MyOutputHandler : public nvtt::OutputHandler
{
    MyOutputHandler() : m_data(NULL), m_ptr(NULL) {}
    ~MyOutputHandler()
    {
        free(m_data);
    }

    virtual void beginImage(int size, int width, int height, int depth, int face, int miplevel)
    {
        m_size = size;
        m_width = width;
        m_height = height;
        free(m_data);
        m_data = (unsigned char *)malloc(size);
        m_ptr = m_data;
    }

    virtual void endImage()
    {
    }

    virtual bool writeData(const void * data, int size)
    {
        memcpy(m_ptr, data, size);
        m_ptr += size;
        return true;
    }

    nvtt::Surface decompress(nvtt::Format format, nvtt::Decoder decoder)
    {
        nvtt::Surface img;
        img.setImage2D(format, decoder, m_width, m_height, m_data);
        return img;
    }

    int m_size;
    int m_width;
    int m_height;
    unsigned char * m_data;
    unsigned char * m_ptr;
};



// Compare two HDR surfaces tone mapped at the given exposure.
float compare(const Surface & hdr0, const Surface & hdr1, float exposure)
{
    Surface ldr0 = hdr0;
    ldr0.scaleBias(0, exposure, 0);
    ldr0.scaleBias(1, exposure, 0);
    ldr0.scaleBias(2, exposure, 0);
    ldr0.toneMap(nvtt::ToneMapper_Halo, NULL);
    ldr0.toSrgb();

    Surface ldr1 = hdr1;
    ldr1.scaleBias(0, exposure, 0);
    ldr1.scaleBias(1, exposure, 0);
    ldr1.scaleBias(2, exposure, 0);
    ldr1.toneMap(nvtt::ToneMapper_Halo, NULL);
    ldr1.toSrgb();

    return nvtt::rmsError(ldr0, ldr1);
}

// Compare two HDR images at different exposures.
void compare(const Surface & hdr0, const Surface & hdr1, const Array<float> & exposures, Array<float> & errors)
{
    const uint exposureCount = exposures.count();

    errors.resize(exposureCount);

    for (uint i = 0; i < exposureCount; i++) {
        errors[i] = compare(hdr0, hdr1, exposures[i]);
    }
}


void updatePointSet(const Array<float> & exposures, Array<float> & errors, GooglePointSet & pointSet)
{
    uint pointCount = exposures.count();
    nvDebugCheck(pointCount == errors.count());

    pointSet.points.resize(pointCount);

    for (uint i = 0; i < pointCount; i++) {
        pointSet.points[i].x = exposures[i];
        pointSet.points[i].y = errors[i];
    }
}

Surface loadInput(const char * fileName) {
    Surface src;
    src.load(fileName);
    src.clamp(0, 0, FLT_MAX);
    src.clamp(1, 0, FLT_MAX);
    src.clamp(2, 0, FLT_MAX);
    return src;
}

Surface process(const Surface & src, int method) {
    Surface dst;

    float rMin, rMax, gMin, gMax, bMin, bMax;
    src.range(0, &rMin, &rMax);
    src.range(1, &gMin, &gMax);
    src.range(2, &bMin, &bMax);

    if (method == 0) {
        // Only clamp.
        dst = src;

        dst.scaleBias(0, 1.0f / 4, 0);
        dst.scaleBias(1, 1.0f / 4, 0);
        dst.scaleBias(2, 1.0f / 4, 0);

        dst.clamp(0, 0, 1);
        dst.clamp(1, 0, 1);
        dst.clamp(2, 0, 1);

        dst.toGamma(0, 2);
        dst.toGamma(1, 2);
        dst.toGamma(2, 2);

        dst.quantize(0, 10, /*exactEndPoints*/true, false);
        dst.quantize(1, 10, /*exactEndPoints*/true, false);
        dst.quantize(2, 10, /*exactEndPoints*/true, false);

        dst.toLinear(0, 2);
        dst.toLinear(1, 2);
        dst.toLinear(2, 2);

        dst.scaleBias(0, 4, 0);
        dst.scaleBias(1, 4, 0);
        dst.scaleBias(2, 4, 0);
    }
    else if (method == 1) {
        // Scale and bias. Use full range.
        dst = src;

        float gamma = 3;

        dst.scaleBias(0, 1.0f / rMax, 0);
        dst.scaleBias(1, 1.0f / gMax, 0);
        dst.scaleBias(2, 1.0f / bMax, 0);

        dst.clamp(0, 0, 1);
        dst.clamp(1, 0, 1);
        dst.clamp(2, 0, 1);

        dst.toGamma(0, gamma);
        dst.toGamma(1, gamma);
        dst.toGamma(2, gamma);

        dst.quantize(0, 10, /*exactEndPoints*/true, false);
        dst.quantize(1, 10, /*exactEndPoints*/true, false);
        dst.quantize(2, 10, /*exactEndPoints*/true, false);

        dst.toLinear(0, gamma);
        dst.toLinear(1, gamma);
        dst.toLinear(2, gamma);

        dst.scaleBias(0, rMax, 0);
        dst.scaleBias(1, gMax, 0);
        dst.scaleBias(2, bMax, 0);
    }
    else if (method == 2) {
        // Scale and bias. Use full range.
        dst = src;

        // @@ Experiment with log/exp transform!
        float gamma = 2.2f;

        dst.scaleBias(0, 1.0f / rMax, 0);
        dst.scaleBias(1, 1.0f / gMax, 0);
        dst.scaleBias(2, 1.0f / bMax, 0);

        dst.clamp(0, 0, 1);
        dst.clamp(1, 0, 1);
        dst.clamp(2, 0, 1);

        //dst.toGamma(0, gamma);
        //dst.toGamma(1, gamma);
        //dst.toGamma(2, gamma);
        dst.toLogScale(0, 2);
        dst.toLogScale(1, 2);
        dst.toLogScale(2, 2);

        dst.quantize(0, 10, /*exactEndPoints*/true, false);
        dst.quantize(1, 10, /*exactEndPoints*/true, false);
        dst.quantize(2, 10, /*exactEndPoints*/true, false);

        dst.fromLogScale(0, 2);
        dst.fromLogScale(1, 2);
        dst.fromLogScale(2, 2);

        //dst.toLinear(0, gamma);
        //dst.toLinear(1, gamma);
        //dst.toLinear(2, gamma);

        dst.scaleBias(0, rMax, 0);
        dst.scaleBias(1, gMax, 0);
        dst.scaleBias(2, bMax, 0);
    }
    else if (method == 3) {
        // Scale and bias. Use full range.
        dst = src;

        // @@ Experiment with log/exp transform!
        float gamma = 0.5;

        dst.scaleBias(0, 1.0f / rMax, 0);
        dst.scaleBias(1, 1.0f / gMax, 0);
        dst.scaleBias(2, 1.0f / bMax, 0);

        dst.clamp(0, 0, 1);
        dst.clamp(1, 0, 1);
        dst.clamp(2, 0, 1);

        dst.toGamma(0, gamma);
        dst.toGamma(1, gamma);
        dst.toGamma(2, gamma);

        dst.toLogScale(0, 2);
        dst.toLogScale(1, 2);
        dst.toLogScale(2, 2);

        dst.quantize(0, 8, /*exactEndPoints*/true, false);
        dst.quantize(1, 8, /*exactEndPoints*/true, false);
        dst.quantize(2, 8, /*exactEndPoints*/true, false);

        dst.fromLogScale(0, 2);
        dst.fromLogScale(1, 2);
        dst.fromLogScale(2, 2);

        dst.toLinear(0, gamma);
        dst.toLinear(1, gamma);
        dst.toLinear(2, gamma);

        dst.scaleBias(0, rMax, 0);
        dst.scaleBias(1, gMax, 0);
        dst.scaleBias(2, bMax, 0);
    }

    return dst;
}

void printImageInfo(const Surface & img) {
    float rMin, rMax, gMin, gMax, bMin, bMax;
    img.range(0, &rMin, &rMax);
    img.range(1, &gMin, &gMax);
    img.range(2, &bMin, &bMax);

    printf("R: %f %f\n", rMin, rMax);
    printf("G: %f %f\n", gMin, gMax);
    printf("B: %f %f\n", bMin, bMax);
}

int main(int argc, char *argv[])
{
    MyAssertHandler assertHandler;
    MyMessageHandler messageHandler;

    GoogleLineChart chart;
    chart.pointSetArray.resize(2);

    Array<float> errors;
    Array<float> exposures;
    for (int i = 0; i < 48; i++) {
        //exposures.append(8 * float(i)/63);
        exposures.append(lerp(0.22f, 22, float(i)/47));
    }

    Surface src = loadInput("hdr/34017_03.dds");
    //Surface src = loadInput("hdr/49002_1F.dds");
    if (src.isNull()) {
        printf("Error loading image.\n");
        return EXIT_FAILURE;
    }

    printImageInfo(src);

    //compare(src, process(src, 0), exposures, errors);
    //updatePointSet(exposures, errors, chart.pointSetArray[0]);
    //chart.pointSetArray[0].legend = "Clamp";
    compare(src, process(src, 0), exposures, errors);
    updatePointSet(exposures, errors, chart.pointSetArray[0]);
    chart.pointSetArray[0].legend = "Default";

    compare(src, process(src, 3), exposures, errors);
    updatePointSet(exposures, errors, chart.pointSetArray[1]);
    chart.pointSetArray[1].legend = "Log + Gamma 2.2";
    chart.pointSetArray[1].lineColor = Vector3(0.19f, 0.45f, 0.95f);


    chart.autoScale();

    StringBuilder builder;
    chart.build(builder);

    printf("%s\n", builder.str());

    return EXIT_SUCCESS;
}

