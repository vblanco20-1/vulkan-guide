// This code is in the public domain -- castanyo@yahoo.es
#pragma once

#include "nvimage.h"

namespace nv
{
    class Image;
    class FloatImage;
    class Stream;

    namespace ImageIO
    {
        Image * load(const char * fileName);
        Image * load(const char * fileName, Stream & s);

        FloatImage * loadFloat(const char * fileName);
        FloatImage * loadFloat(const char * fileName, Stream & s);

        bool save(const char * fileName, const Image * img, const char ** tags=NULL); // NULL terminated list.
        bool save(const char * fileName, Stream & s, const Image * img, const char ** tags=NULL);

        bool saveFloat(const char * fileName, const FloatImage * fimage, uint baseComponent, uint componentCount);
        bool saveFloat(const char * fileName, Stream & s, const FloatImage * fimage, uint baseComponent, uint componentCount);

    } // ImageIO namespace

} // nv namespace
