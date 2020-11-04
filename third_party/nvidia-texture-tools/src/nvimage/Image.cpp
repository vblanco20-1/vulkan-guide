// This code is in the public domain -- castanyo@yahoo.es

#include "Image.h"
#include "ImageIO.h"

#include "nvmath/Color.h"

#include "nvcore/Debug.h"
#include "nvcore/Ptr.h"
#include "nvcore/Utils.h" // swap
#include "nvcore/Memory.h" // realloc, free

#include <string.h> // memcpy


using namespace nv;

Image::Image(const Image & img)
{
	allocate(img.width, img.height, img.depth);
    format = img.format;
    memcpy(data, img.data, sizeof(Color32) * width * height * depth);
}

Image::~Image()
{
    free();
}

const Image & Image::operator=(const Image & img)
{
    allocate(img.width, img.height, depth);
    format = img.format;
    memcpy(data, img.data, sizeof(Color32) * width * height * depth);
    return *this;
}


void Image::allocate(uint w, uint h, uint d/*= 1*/)
{
    width = w;
    height = h;
	depth = d;
    data = realloc<Color32>(data, w * h * d);
}

void Image::acquire(Color32 * data, uint w, uint h, uint d/*= 1*/)
{
    free();
    width = w;
    height = h;
    depth = d;
    this->data = data;
}

void Image::free()
{
    ::free(data);
    width = height = depth = 0;
    data = NULL;
}

void Image::resize(uint w, uint h, uint d/*= 1*/) {

    Image img;
    img.allocate(w, h, d);

    Color32 background(0,0,0,0);

    // Copy image.
    uint x, y, z;
    for(z = 0; z < min(d, depth); z++) {
        for(y = 0; y < min(h, height); y++) {
            for(x = 0; x < min(w, width); x++) {
                img.pixel(x, y, z) = pixel(x, y, z);
            }
            for(; x < w; x++) {
                img.pixel(x, y, z) = background;
            }
        }
        for(; y < h; y++) {
            for(x = 0; x < w; x++) {
                img.pixel(x, y, z) = background;
            }
        }
    }
    for(; z < d; z++) {
        for(y = 0; y < h; y++) {
            for(x = 0; x < w; x++) {
                img.pixel(x, y, z) = background;
            }
        }
    }

    swap(width, img.width);
    swap(height, img.height);
	swap(depth, img.depth);
    swap(format, img.format);
    swap(sRGB, img.sRGB);
    swap(data, img.data);
}

bool Image::load(const char * name)
{
    free();

    AutoPtr<Image> img(ImageIO::load(name));
    if (img == NULL) {
        return false;
    }

    swap(width, img->width);
    swap(height, img->height);
	swap(depth, img->depth);
    swap(format, img->format);
    swap(sRGB, img->sRGB);
    swap(data, img->data);

    return true;
}

void Image::wrap(void * _data, uint w, uint h, uint d)
{
    free();
    data = (Color32 *)_data;
    width = w;
    height = h;
	depth = d;
}

void Image::unwrap()
{
    data = NULL;
    width = 0;
    height = 0;
	depth = 0;
}


const Color32 * Image::scanline(uint h) const
{
    nvDebugCheck(h < height);
    return data + h * width;
}

Color32 * Image::scanline(uint h)
{
    nvDebugCheck(h < height);
    return data + h * width;
}

const Color32 * Image::pixels() const
{
    return data;
}

Color32 * Image::pixels()
{
    return data;
}

const Color32 & Image::pixel(uint idx) const
{
    nvDebugCheck(idx < width * height * depth);
    return data[idx];
}

Color32 & Image::pixel(uint idx)
{
    nvDebugCheck(idx < width * height * depth);
    return data[idx];
}

void Image::fill(Color32 c)
{
    const uint size = width * height * depth;
    for (uint i = 0; i < size; ++i)
    {
        data[i] = c;
    }
}

