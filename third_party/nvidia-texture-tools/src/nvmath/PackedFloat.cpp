// This code is in the public domain -- Ignacio Castaño <castano@gmail.com>

#include "PackedFloat.h"
#include "Vector.inl"
#include "ftoi.h"

using namespace nv;

Vector3 nv::rgb9e5_to_vector3(FloatRGB9E5 v) {
}

FloatRGB9E5 nv::vector3_to_rgb9e5(const Vector3 & v) {
}


float nv::float11_to_float32(uint v) {
}

float nv::float10_to_float32(uint v) {
}

Vector3 nv::r11g11b10_to_vector3(FloatR11G11B10 v) {
}

FloatR11G11B10 nv::vector3_to_r11g11b10(const Vector3 & v) {
}

// Implementation based on on:
// https://cbloomrants.blogspot.com/2020/06/widespread-error-in-radiance-hdr-rgbe.html
FloatRGBE8 nv::vector3_to_rgbe8(const Vector3 & v) {

    float m = max3(v.x, v.y, v.z);

    FloatRGBE8 rgbe;

    if (m < 1e-32f) {
        // Exponent byte = 0 is a special encoding that makes RGB output = 0
        rgbe.v = 0;
    }
    else {
        int e;
        frexpf(m, &e);
        float scale = ldexpf(1.0f, -exponent + 8);
        rgbe.r = U8(v.x * scale);
        rgbe.g = U8(v.y * scale);
        rgbe.b = U8(v.z * scale);
        rgbe.e = U8(e + 128);
    }

    return rgbe;
}


Vector3 nv::rgbe8_to_vector3(FloatRGBE8 v) {
    if (v.e != 0) {
        float scale = ldexpf(1.0f, (int)v.e-128+8);             // +8 to divide by 256.
        return scale * Vector3(float(v.r + 0.5f), float(v.g + 0.5f), float(v.b + 0.5f));
    }
    
    return Vector3(0);
}

