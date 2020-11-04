#pragma once
#ifndef NV_MATH_HALF_H
#define NV_MATH_HALF_H

#include "nvmath.h"

namespace nv {

    uint32 half_to_float( uint16 h );
    uint16 half_from_float( uint32 f );

    // vin,vout must be 16 byte aligned. count must be a multiple of 8.
    // implement a non-SSE version if we need it. For now, this naming makes it clear this is only available when SSE2 is
    void half_to_float_array_SSE2(const uint16 * vin, float * vout, int count);

    void half_init_tables();

    extern uint32 mantissa_table[2048];
    extern uint32 exponent_table[64];
    extern uint32 offset_table[64];

    // Fast half to float conversion based on:
    // http://www.fox-toolkit.org/ftp/fasthalffloatconversion.pdf
    inline uint32 fast_half_to_float(uint16 h)
    {
		// Initialize table if necessary.
		if (mantissa_table[0] != 0)
			half_init_tables();
	    uint exp = h >> 10;
	    return mantissa_table[offset_table[exp] + (h & 0x3ff)] + exponent_table[exp];
    }


    inline uint16 to_half(float c) {
        union { float f; uint32 u; } f;
        f.f = c;
        return nv::half_from_float( f.u );
    }

    inline float to_float(uint16 c) {
        union { float f; uint32 u; } f;
        f.u = nv::fast_half_to_float( c );
        return f.f;
    }


    union Half {
        uint16 raw;
        struct {
        #if NV_BIG_ENDIAN
            uint negative:1;
            uint biasedexponent:5;
            uint mantissa:10;
        #else
            uint mantissa:10;
            uint biasedexponent:5;
            uint negative:1;
        #endif
        } field;
    };


    inline float TestHalfPrecisionAwayFromZero(float input)
    {
        Half h;
        h.raw = to_half(input);
        h.raw += 1;

        float f = to_float(h.raw);
        
        // Subtract the initial value to find our precision
        float delta = f - input;

        return delta;
    }
     
    inline float TestHalfPrecisionTowardsZero(float input)
    {
        Half h;
        h.raw = to_half(input);
        h.raw -= 1;

        float f = to_float(h.raw);

        // Subtract the initial value to find our precision
        float delta = f - input;

        return -delta;
    }

} // nv namespace

#endif // NV_MATH_HALF_H
