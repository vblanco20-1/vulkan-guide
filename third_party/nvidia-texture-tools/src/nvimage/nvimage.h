// This code is in the public domain -- castanyo@yahoo.es
#pragma once

#include "nvcore/nvcore.h"
#include "nvcore/Debug.h" // nvDebugCheck
#include "nvcore/Utils.h" // isPowerOfTwo

namespace nv {

    // Some utility functions:
    inline uint computeBitPitch(uint w, uint bitsize, uint alignmentInBits)
    {
        nvDebugCheck(isPowerOfTwo(alignmentInBits));

        return ((w * bitsize +  alignmentInBits - 1) / alignmentInBits) * alignmentInBits;
    }

    inline uint computeBytePitch(uint w, uint bitsize, uint alignmentInBytes)
    {
        uint pitch = computeBitPitch(w, bitsize, 8*alignmentInBytes);
        nvDebugCheck((pitch & 7) == 0);

        return (pitch + 7) / 8;
    }

} // nv namespace
