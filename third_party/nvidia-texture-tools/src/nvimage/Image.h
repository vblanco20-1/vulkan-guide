// This code is in the public domain -- castanyo@yahoo.es
#pragma once

#include "nvimage.h"
#include "nvcore/Debug.h"

#if NV_USE_ALTIVEC
#undef pixel
#endif

namespace nv
{
    class Color32;

    // 32 bit ARGB image.
    class Image
    {
    public:

        enum Format : uint8 {
            Format_XRGB,
            Format_ARGB,
        };

        Image() {}
        Image(const Image & img);
        ~Image();

        const Image & operator=(const Image & img);


        void allocate(uint w, uint h, uint d = 1);
        void acquire(Color32 * data, uint w, uint h, uint d = 1);
        bool load(const char * name);
        void free();

        void resize(uint w, uint h, uint d = 1);

        void wrap(void * data, uint w, uint h, uint d = 1);
        void unwrap();

        const Color32 * scanline(uint h) const;
        Color32 * scanline(uint h);

        const Color32 * pixels() const;
        Color32 * pixels();

        const Color32 & pixel(uint idx) const;
        Color32 & pixel(uint idx);

        const Color32 & pixel(uint x, uint y, uint z = 0) const;
        Color32 & pixel(uint x, uint y,  uint z = 0);

        void fill(Color32 c);


        uint width = 0;
        uint height = 0;
        uint depth = 0;
        Format format = Format_XRGB;
        bool sRGB = false;
        Color32 * data = NULL;
    };


    inline const Color32 & Image::pixel(uint x, uint y, uint z) const
    {
        nvDebugCheck(x < width && y < height && z < depth);
        return pixel((z * height + y) * width + x);
    }

    inline Color32 & Image::pixel(uint x, uint y, uint z)
    {
        nvDebugCheck(x < width && y < height && z < depth);
        return pixel((z * height + y) * width + x);
    }

} // nv namespace
