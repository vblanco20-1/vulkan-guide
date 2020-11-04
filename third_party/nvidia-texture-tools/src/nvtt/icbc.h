// icbc.h v1.05
// A High Quality SIMD BC1 Encoder by Ignacio Castano <castano@gmail.com>.
//
// LICENSE:
//  MIT license at the end of this file.

#ifndef ICBC_H
#define ICBC_H

namespace icbc {

    enum Decoder {
        Decoder_D3D10 = 0,
        Decoder_NVIDIA = 1,
        Decoder_AMD = 2
    };

    void init_dxt1(Decoder decoder = Decoder_D3D10);

    enum Quality {
        Quality_Level1,  // Box fit + least squares fit.
        Quality_Level2,  // Cluster fit 4, threshold = 24.
        Quality_Level3,  // Cluster fit 4, threshold = 32.
        Quality_Level4,  // Cluster fit 4, threshold = 48.
        Quality_Level5,  // Cluster fit 4, threshold = 64.
        Quality_Level6,  // Cluster fit 4, threshold = 96.
        Quality_Level7,  // Cluster fit 4, threshold = 128.
        Quality_Level8,  // Cluster fit 4+3, threshold = 256.
        Quality_Level9,  // Cluster fit 4+3, threshold = 256 + Refinement.

        Quality_Fast = Quality_Level1,
        Quality_Default = Quality_Level8,
        Quality_Max = Quality_Level9,
    };

    void decode_dxt1(const void * block, unsigned char rgba_block[16 * 4], Decoder decoder = Decoder_D3D10);
    float evaluate_dxt1_error(const unsigned char rgba_block[16 * 4], const void * block, Decoder decoder = Decoder_D3D10);

    float compress_dxt1(Quality level, const float * input_colors, const float * input_weights, const float color_weights[3], bool three_color_mode, bool three_color_black, void * output);
}

#endif // ICBC_H

#ifdef ICBC_IMPLEMENTATION

// Instruction level support must be chosen at compile time setting ICBC_SIMD to one of these values:
#define ICBC_SCALAR 0
#define ICBC_SSE2   1
#define ICBC_SSE41  2
#define ICBC_AVX1   3
#define ICBC_AVX2   4
#define ICBC_AVX512 5
#define ICBC_NEON   -1
#define ICBC_VMX    -2

#if defined(__i386__) || defined(_M_IX86) || defined(__x86_64__) || defined(_M_X64)
    #define ICBC_X86 1
#endif

#if (defined(__arm__) || defined(_M_ARM))
    #define ICBC_ARM 1
#endif

#if (defined(__PPC__) || defined(_M_PPC))
    #define ICBC_PPC 1
#endif

// SIMD version.
#ifndef ICBC_SIMD
    #if ICBC_X86
        #if __AVX512F__
            #define ICBC_SIMD ICBC_AVX512
        #elif __AVX2__
            #define ICBC_SIMD ICBC_AVX2
        #elif __AVX__
            #define ICBC_SIMD ICBC_AVX1
        #elif __SSE4_1__
            #define ICBC_SIMD ICBC_SSE41
        #elif __SSE2__
            #define ICBC_SIMD ICBC_SSE2
        #else
            #define ICBC_SIMD ICBC_SCALAR
        #endif
    #endif

    #if ICBC_ARM
        #if __ARM_NEON__
            #define ICBC_SIMD ICBC_NEON
        #else
            #define ICBC_SIMD ICBC_SCALAR
        #endif
    #endif

    #if ICBC_PPC
        #define ICBC_SIMD ICBC_VMX
    #endif
#endif

// AVX1 does not require FMA, and depending on whether it's Intel or AMD you may have FMA3 or FMA4. What a mess.
#ifndef ICBC_USE_FMA
//#define ICBC_USE_FMA 3
//#define ICBC_USE_FMA 4
#endif

#if ICBC_SIMD >= ICBC_AVX2
#define ICBC_BMI2 1
#endif

// Apparently rcp is not deterministic (different precision on Intel and AMD), enable if you don't care about that for a small performance boost.
//#define ICBC_USE_RCP 1

#if ICBC_SIMD == ICBC_AVX2
#define ICBC_USE_AVX2_PERMUTE2 1    // Using permutevar8x32 and bitops.
#endif

#if ICBC_SIMD == ICBC_AVX512
#define ICBC_USE_AVX512_PERMUTE 1
#endif

#if ICBC_SIMD == ICBC_NEON
#define ICBC_USE_NEON_VTL 0         // Not tested.
#endif


// Some experimental knobs:
#define ICBC_PERFECT_ROUND 0        // Enable perfect rounding to compute cluster fit residual.


#if ICBC_SIMD >= ICBC_SSE2
#include <emmintrin.h>
#endif

#if ICBC_SIMD >= ICBC_SSE41
#include <smmintrin.h>
#endif

#if ICBC_SIMD >= ICBC_AVX1
#include <immintrin.h>
#endif

#if ICBC_SIMD >= ICBC_AVX512 && _MSC_VER
#include <zmmintrin.h>
#endif

#if ICBC_SIMD == ICBC_NEON
#include <arm_neon.h>
#endif

#if ICBC_SIMD == ICBC_VMX
#include <altivec.h>
#endif

#if _MSC_VER
#include <intrin.h> // _BitScanReverse
#endif

#include <stdint.h>
#include <stdlib.h> // abs
#include <string.h> // memset
#include <math.h>   // fabsf
#include <float.h>  // FLT_MAX

#ifndef ICBC_ASSERT
#if _DEBUG
#define ICBC_ASSERT assert
#include <assert.h>
#else
#define ICBC_ASSERT(x)
#endif
#endif

namespace icbc {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Basic Templates

template <typename T> inline void swap(T & a, T & b) {
    T temp(a);
    a = b;
    b = temp;
}

template <typename T> inline T max(const T & a, const T & b) {
    return (b < a) ? a : b;
}

template <typename T> inline T min(const T & a, const T & b) {
    return (a < b) ? a : b;
}

template <typename T> inline T clamp(const T & x, const T & a, const T & b) {
    return min(max(x, a), b);
}

template <typename T> inline T square(const T & a) {
    return a * a;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Basic Types

typedef uint8_t uint8;
typedef int8_t int8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint32_t uint;


struct Color16 {
    union {
        struct {
            uint16 b : 5;
            uint16 g : 6;
            uint16 r : 5;
        };
        uint16 u;
    };
};

struct Color32 {
    union {
        struct {
            uint8 b, g, r, a;
        };
        uint32 u;
    };
};

struct BlockDXT1 {
    Color16 col0;
    Color16 col1;
    uint32 indices;
};


struct Vector3 {
    float x;
    float y;
    float z;

    inline void operator+=(Vector3 v) {
        x += v.x; y += v.y; z += v.z;
    }
    inline void operator*=(Vector3 v) {
        x *= v.x; y *= v.y; z *= v.z;
    }
    inline void operator*=(float s) {
        x *= s; y *= s; z *= s;
    }
};

struct Vector4 {
    union {
        struct {
            float x, y, z, w;
        };
        Vector3 xyz;
    };
};


inline Vector3 operator*(Vector3 v, float s) {
    return { v.x * s, v.y * s, v.z * s };
}

inline Vector3 operator*(float s, Vector3 v) {
    return { v.x * s, v.y * s, v.z * s };
}

inline Vector3 operator*(Vector3 a, Vector3 b) {
    return { a.x * b.x, a.y * b.y, a.z * b.z };
}

inline float dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vector3 operator+(Vector3 a, Vector3 b) {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

inline Vector3 operator-(Vector3 a, Vector3 b) {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

inline Vector3 operator/(Vector3 v, float s) {
    return { v.x / s, v.y / s, v.z / s };
}

inline float saturate(float x) {
    return clamp(x, 0.0f, 1.0f);
}

inline Vector3 saturate(Vector3 v) {
    return { saturate(v.x), saturate(v.y), saturate(v.z) };
}

inline Vector3 min(Vector3 a, Vector3 b) {
    return { min(a.x, b.x), min(a.y, b.y), min(a.z, b.z) };
}

inline Vector3 max(Vector3 a, Vector3 b) {
    return { max(a.x, b.x), max(a.y, b.y), max(a.z, b.z) };
}

inline bool operator==(const Vector3 & a, const Vector3 & b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

inline Vector3 scalar_to_vector3(float f) {
    return {f, f, f};
}

inline float lengthSquared(Vector3 v) {
    return dot(v, v);
}

inline bool equal(float a, float b, float epsilon = 0.0001) {
    // http://realtimecollisiondetection.net/blog/?p=89
    //return fabsf(a - b) < epsilon * max(1.0f, max(fabsf(a), fabsf(b)));
    return fabsf(a - b) < epsilon;
}

inline bool equal(Vector3 a, Vector3 b, float epsilon) {
    return equal(a.x, b.x, epsilon) && equal(a.y, b.y, epsilon) && equal(a.z, b.z, epsilon);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// SIMD

#ifndef ICBC_ALIGN_16
#if __GNUC__
#   define ICBC_ALIGN_16 __attribute__ ((__aligned__ (16)))
#else // _MSC_VER
#   define ICBC_ALIGN_16 __declspec(align(16))
#endif
#endif

#if __GNUC__
#define ICBC_FORCEINLINE inline __attribute__((always_inline))
#else
#define ICBC_FORCEINLINE __forceinline
#endif


// Count trailing zeros (BSR).
ICBC_FORCEINLINE int ctz(uint mask) {
#if __GNUC__
    return __builtin_ctz(mask);
#else
    unsigned long index;
    _BitScanReverse(&index, mask);
    return (int)index;
#endif
}


#if ICBC_SIMD == ICBC_SCALAR  // Purely scalar version.

constexpr int VEC_SIZE = 1;

using VFloat = float;
using VMask = bool;

ICBC_FORCEINLINE float & lane(VFloat & v, int i) { return v; }
ICBC_FORCEINLINE VFloat vzero() { return 0.0f; }
ICBC_FORCEINLINE VFloat vbroadcast(float x) { return x; }
ICBC_FORCEINLINE VFloat vload(const float * ptr) { return *ptr; }
ICBC_FORCEINLINE VFloat vrcp(VFloat a) { return 1.0f / a; }
ICBC_FORCEINLINE VFloat vmadd(VFloat a, VFloat b, VFloat c) { return a * b + c; }
ICBC_FORCEINLINE VFloat vmsub(VFloat a, VFloat b, VFloat c) { return a * b - c; }
ICBC_FORCEINLINE VFloat vm2sub(VFloat a, VFloat b, VFloat c, VFloat d) { return a * b - c * d; }
ICBC_FORCEINLINE VFloat vsaturate(VFloat a) { return min(max(a, 0.0f), 1.0f); }
ICBC_FORCEINLINE VFloat vround01(VFloat a) { return float(int(a + 0.5f)); }
ICBC_FORCEINLINE VFloat lane_id() { return 0; }
ICBC_FORCEINLINE VFloat vselect(VMask mask, VFloat a, VFloat b) { return mask ? b : a; }
ICBC_FORCEINLINE VMask vbroadcast(bool b) { return b; }
ICBC_FORCEINLINE bool all(VMask m) { return m; }
ICBC_FORCEINLINE bool any(VMask m) { return m; }
ICBC_FORCEINLINE uint mask(VMask m) { return (uint)m; }
ICBC_FORCEINLINE int reduce_min_index(VFloat v) { return 0; }
ICBC_FORCEINLINE void vtranspose4(VFloat & a, VFloat & b, VFloat & c, VFloat & d) {}

#elif ICBC_SIMD == ICBC_SSE2 || ICBC_SIMD == ICBC_SSE41

constexpr int VEC_SIZE = 4;

#if __GNUC__
// GCC needs a struct so that we can overload operators.
union VFloat {
    __m128 v;
    float m128_f32[VEC_SIZE];

    VFloat() {}
    VFloat(__m128 v) : v(v) {}
    operator __m128 & () { return v; }
};
union VMask {
    __m128 m;

    VMask() {}
    VMask(__m128 m) : m(m) {}
    operator __m128 & () { return m; }
};
#else
using VFloat = __m128;
using VMask = __m128;
#endif

ICBC_FORCEINLINE float & lane(VFloat & v, int i) {
    return v.m128_f32[i];
}

ICBC_FORCEINLINE VFloat vzero() {
    return _mm_setzero_ps();
}

ICBC_FORCEINLINE VFloat vbroadcast(float x) {
    return _mm_set1_ps(x);
}

ICBC_FORCEINLINE VFloat vload(const float * ptr) {
    return _mm_load_ps(ptr);
}

ICBC_FORCEINLINE VFloat vgather(const float * base, VFloat index) {
    VFloat v;
    for (int i = 0; i < VEC_SIZE; i++) {
        lane(v, i) = base[int(lane(index, i))];
    }
    return v;
}

ICBC_FORCEINLINE VFloat operator+(VFloat a, VFloat b) {
    return _mm_add_ps(a, b);
}

ICBC_FORCEINLINE VFloat operator-(VFloat a, VFloat b) {
    return _mm_sub_ps(a, b);
}

ICBC_FORCEINLINE VFloat operator*(VFloat a, VFloat b) {
    return _mm_mul_ps(a, b);
}

ICBC_FORCEINLINE VFloat vrcp(VFloat a) {
#if ICBC_USE_RCP
    VFloat r = _mm_rcp_ps(a);
    return _mm_mul_ps(r, _mm_sub_ps(vbroadcast(2.0f), _mm_mul_ps(r, a)));   // r * (2 - r * a)
#else
    return _mm_div_ps(vbroadcast(1.0f), a);
#endif
}

// a*b+c
ICBC_FORCEINLINE VFloat vmadd(VFloat a, VFloat b, VFloat c) {
    return a * b + c;
}

ICBC_FORCEINLINE VFloat vmsub(VFloat a, VFloat b, VFloat c) {
    return a * b - c;
}

ICBC_FORCEINLINE VFloat vm2sub(VFloat a, VFloat b, VFloat c, VFloat d) {
    return a * b - c * d;
}

ICBC_FORCEINLINE VFloat vsaturate(VFloat a) {
    auto zero = _mm_setzero_ps();
    auto one = _mm_set1_ps(1.0f);
    return _mm_min_ps(_mm_max_ps(a, zero), one);
}

// Assumes a is in [0, 1] range.
ICBC_FORCEINLINE VFloat vround01(VFloat a) {
#if ICBC_SIMD == ICBC_SSE41
    return _mm_round_ps(a, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
#else
    return _mm_cvtepi32_ps(_mm_cvttps_epi32(a + vbroadcast(0.5f)));
#endif
}

ICBC_FORCEINLINE VFloat vtruncate(VFloat a) {
#if ICBC_SIMD == ICBC_SSE41
    return _mm_round_ps(a, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
#else
    return _mm_cvtepi32_ps(_mm_cvttps_epi32(a));
#endif
}

ICBC_FORCEINLINE VFloat lane_id() {
    return _mm_set_ps(3, 2, 1, 0);
}

ICBC_FORCEINLINE VMask operator> (VFloat A, VFloat B) { return _mm_cmpgt_ps(A, B); }
ICBC_FORCEINLINE VMask operator>=(VFloat A, VFloat B) { return _mm_cmpge_ps(A, B); }
ICBC_FORCEINLINE VMask operator< (VFloat A, VFloat B) { return _mm_cmplt_ps(A, B); }
ICBC_FORCEINLINE VMask operator<=(VFloat A, VFloat B) { return _mm_cmple_ps(A, B); }

ICBC_FORCEINLINE VMask operator| (VMask A, VMask B) { return _mm_or_ps(A, B); }
ICBC_FORCEINLINE VMask operator& (VMask A, VMask B) { return _mm_and_ps(A, B); }
ICBC_FORCEINLINE VMask operator^ (VMask A, VMask B) { return _mm_xor_ps(A, B); }

// mask ? b : a
ICBC_FORCEINLINE VFloat vselect(VMask mask, VFloat a, VFloat b) {
#if ICBC_SIMD == ICBC_SSE41
    return _mm_blendv_ps(a, b, mask);
#else
    return _mm_or_ps(_mm_andnot_ps(mask, a), _mm_and_ps(mask, b));
#endif
}

ICBC_FORCEINLINE VMask vbroadcast(bool b) { 
    return _mm_castsi128_ps(_mm_set1_epi32(-int32_t(b)));
}

ICBC_FORCEINLINE bool all(VMask m) {
    int value = _mm_movemask_ps(m);
    return value == 0x7;
}

ICBC_FORCEINLINE bool any(VMask m) {
    int value = _mm_movemask_ps(m);
    return value != 0;
}

ICBC_FORCEINLINE uint mask(VMask m) {
    return (uint)_mm_movemask_ps(m);
}

ICBC_FORCEINLINE int reduce_min_index(VFloat v) {

    // First do an horizontal reduction.                            // v = [ D C | B A ]
    VFloat shuf = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 3, 0, 1));    //     [ C D | A B ]
    VFloat mins = _mm_min_ps(v, shuf);                              // mins = [ D+C C+D | B+A A+B ]
    shuf        = _mm_movehl_ps(shuf, mins);                        //        [   C   D | D+C C+D ]  // let the compiler avoid a mov by reusing shuf
    mins        = _mm_min_ss(mins, shuf);
    mins =      _mm_shuffle_ps(mins, mins, _MM_SHUFFLE(0, 0, 0, 0));

    // Then find the index.
    uint mask = _mm_movemask_ps(v <= mins);
    return ctz(mask);
}

// https://gcc.gnu.org/legacy-ml/gcc-patches/2005-10/msg00324.html
ICBC_FORCEINLINE void vtranspose4(VFloat & r0, VFloat & r1, VFloat & r2, VFloat & r3) {
    VFloat t0 = _mm_unpacklo_ps(r0, r1);
    VFloat t1 = _mm_unpacklo_ps(r2, r3);
    VFloat t2 = _mm_unpackhi_ps(r0, r1);
    VFloat t3 = _mm_unpackhi_ps(r2, r3);
    r0 = _mm_movelh_ps(t0, t1);
    r1 = _mm_movehl_ps(t1, t0);
    r2 = _mm_movelh_ps(t2, t3);
    r3 = _mm_movehl_ps(t3, t2);
}


#elif ICBC_SIMD == ICBC_AVX1 || ICBC_SIMD == ICBC_AVX2

constexpr int VEC_SIZE = 8;

#if __GNUC__
union VFloat {
    __m256 v;
    float m256_f32[VEC_SIZE];

    VFloat() {}
    VFloat(__m256 v) : v(v) {}
    operator __m256 & () { return v; }
};
union VInt {
    __m256i v;
    int m256_i32[VEC_SIZE];

    VInt() {}
    VInt(__m256i v) : v(v) {}
    operator __m256i & () { return v; }
};
union VMask {
    __m256 m;

    VMask() {}
    VMask(__m256 m) : m(m) {}
    operator __m256 & () { return m; }
};
#else
using VFloat = __m256;
using VInt = __m256i;
using VMask = __m256;   // Emulate mask vector using packed float.
#endif

ICBC_FORCEINLINE float & lane(VFloat & v, int i) {
    return v.m256_f32[i];
}

ICBC_FORCEINLINE VFloat vzero() {
    return _mm256_setzero_ps();
}

ICBC_FORCEINLINE VFloat vbroadcast(float a) {
    return _mm256_set1_ps(a);
}

ICBC_FORCEINLINE VFloat vload(const float * ptr) {
    return _mm256_load_ps(ptr);
}

ICBC_FORCEINLINE VFloat vgather(const float * base, VFloat index) {
#if ICBC_SIMD == ICBC_AVX2
    return _mm256_i32gather_ps(base, _mm256_cvtps_epi32(index), 4);
#else
    VFloat v;
    for (int i = 0; i < VEC_SIZE; i++) {
        lane(v, i) = base[int(lane(index, i))];
    }
    return v;
#endif
}

ICBC_FORCEINLINE VFloat operator+(VFloat a, VFloat b) {
    return _mm256_add_ps(a, b);
}

ICBC_FORCEINLINE VFloat operator-(VFloat a, VFloat b) {
    return _mm256_sub_ps(a, b);
}

ICBC_FORCEINLINE VFloat operator*(VFloat a, VFloat b) {
    return _mm256_mul_ps(a, b);
}

ICBC_FORCEINLINE VFloat vrcp(VFloat a) {
#if ICBC_USE_RCP
    #if ICBC_SIMD == ICBC_AVX512
        VFloat r = _mm256_rcp14_ps(a);
    #else
        VFloat r = _mm256_rcp_ps(a);
    #endif

    // r = r * (2 - r * a)
    #if ICBC_USE_FMA == 3 || ICBC_AVX2
        return _mm256_mul_ps(r, _mm256_fnmadd_ps(r, a, vbroadcast(2.0f)));
    #else
        return _mm256_mul_ps(r, _mm256_sub_ps(vbroadcast(2.0f), _mm256_mul_ps(r, a)));
    #endif
#else
    return _mm256_div_ps(vbroadcast(1.0f), a);
#endif
}

// a*b+c
ICBC_FORCEINLINE VFloat vmadd(VFloat a, VFloat b, VFloat c) {
#if ICBC_USE_FMA == 3 || ICBC_SIMD == ICBC_AVX2
    return _mm256_fmadd_ps(a, b, c);
#elif ICBC_USE_FMA == 4
    return _mm256_macc_ps(a, b, c);
#else
    return ((a * b) + c);
#endif
}

ICBC_FORCEINLINE VFloat vmsub(VFloat a, VFloat b, VFloat c) {
#if ICBC_USE_FMA == 3 || ICBC_SIMD == ICBC_AVX2
    return _mm256_fmsub_ps(a, b, c);
#elif ICBC_USE_FMA == 4
    return _mm256_msub_ps(a, b, c);
#else
    return ((a * b) - c);
#endif
}

ICBC_FORCEINLINE VFloat vm2sub(VFloat a, VFloat b, VFloat c, VFloat d) {
    return vmsub(a, b, c * d);
}

ICBC_FORCEINLINE VFloat vsaturate(VFloat a) {
    __m256 zero = _mm256_setzero_ps();
    __m256 one = _mm256_set1_ps(1.0f);
    return _mm256_min_ps(_mm256_max_ps(a, zero), one);
}

ICBC_FORCEINLINE VFloat vround01(VFloat a) {
    return _mm256_round_ps(a, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}

ICBC_FORCEINLINE VFloat vtruncate(VFloat a) {
    return _mm256_round_ps(a, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
}

ICBC_FORCEINLINE VFloat lane_id() {
    return _mm256_set_ps(7, 6, 5, 4, 3, 2, 1, 0);
}

ICBC_FORCEINLINE VMask operator> (VFloat A, VFloat B) { return _mm256_cmp_ps(A, B, _CMP_GT_OQ); }
ICBC_FORCEINLINE VMask operator>=(VFloat A, VFloat B) { return _mm256_cmp_ps(A, B, _CMP_GE_OQ); }
ICBC_FORCEINLINE VMask operator< (VFloat A, VFloat B) { return _mm256_cmp_ps(A, B, _CMP_LT_OQ); }
ICBC_FORCEINLINE VMask operator<=(VFloat A, VFloat B) { return _mm256_cmp_ps(A, B, _CMP_LE_OQ); }

ICBC_FORCEINLINE VMask operator| (VMask A, VMask B) { return _mm256_or_ps(A, B); }
ICBC_FORCEINLINE VMask operator& (VMask A, VMask B) { return _mm256_and_ps(A, B); }
ICBC_FORCEINLINE VMask operator^ (VMask A, VMask B) { return _mm256_xor_ps(A, B); }

// mask ? b : a
ICBC_FORCEINLINE VFloat vselect(VMask mask, VFloat a, VFloat b) {
    return _mm256_blendv_ps(a, b, mask);
}

ICBC_FORCEINLINE VMask vbroadcast(bool b) { 
    return _mm256_castsi256_ps(_mm256_set1_epi32(-int32_t(b)));
}

ICBC_FORCEINLINE bool all(VMask m) {
    __m256 zero = _mm256_setzero_ps();
    return _mm256_testc_ps(_mm256_cmp_ps(zero, zero, _CMP_EQ_UQ), m) == 0;
}

ICBC_FORCEINLINE bool any(VMask m) {
    return _mm256_testz_ps(m, m) == 0;
}

ICBC_FORCEINLINE uint mask(VMask m) {
    return (uint)_mm256_movemask_ps(m);
}

// This is missing on some GCC versions.
#if !defined _mm256_set_m128
#define _mm256_set_m128(hi, lo) _mm256_insertf128_ps(_mm256_castps128_ps256(lo), (hi), 0x1)
#endif

ICBC_FORCEINLINE int reduce_min_index(VFloat v) {

    __m128 vlow  = _mm256_castps256_ps128(v);
    __m128 vhigh = _mm256_extractf128_ps(v, 1);
           vlow  = _mm_min_ps(vlow, vhigh);

    // First do an horizontal reduction.                                // v = [ D C | B A ]
    __m128 shuf = _mm_shuffle_ps(vlow, vlow, _MM_SHUFFLE(2, 3, 0, 1));  //     [ C D | A B ]
    __m128 mins = _mm_min_ps(vlow, shuf);                            // mins = [ D+C C+D | B+A A+B ]
    shuf        = _mm_movehl_ps(shuf, mins);                         //        [   C   D | D+C C+D ]
    mins        = _mm_min_ss(mins, shuf);

    VFloat vmin = _mm256_permute_ps(_mm256_set_m128(mins, mins), 0); // _MM256_PERMUTE(0, 0, 0, 0, 0, 0, 0, 0)

    // Then find the index.
    uint mask = _mm256_movemask_ps(v <= vmin);
    return ctz(mask);
}

// AoS to SoA
ICBC_FORCEINLINE void vtranspose4(VFloat & a, VFloat & b, VFloat & c, VFloat & d) {
    VFloat r0 = _mm256_unpacklo_ps(a, b);
    VFloat r1 = _mm256_unpacklo_ps(c, d);
    VFloat r2 = _mm256_permute2f128_ps(r0, r1, 0x20);
    VFloat r3 = _mm256_permute2f128_ps(r0, r1, 0x31);
    r0 = _mm256_unpackhi_ps(a, b);
    r1 = _mm256_unpackhi_ps(c, d);
    a = _mm256_unpacklo_ps(r2, r3);
    b = _mm256_unpackhi_ps(r2, r3);
    r2 = _mm256_permute2f128_ps(r0, r1, 0x20);
    r3 = _mm256_permute2f128_ps(r0, r1, 0x31);
    c = _mm256_unpacklo_ps(r2, r3);
    d = _mm256_unpackhi_ps(r2, r3);
}

ICBC_FORCEINLINE VInt vzeroi() {
    return _mm256_setzero_si256();
}

ICBC_FORCEINLINE VInt vbroadcast(int a) {
    return _mm256_set1_epi32(a);
}

ICBC_FORCEINLINE VInt vload(const int * ptr) {
    return _mm256_load_si256((const __m256i*)ptr);
}

ICBC_FORCEINLINE VInt operator- (VInt A, int b) { return _mm256_sub_epi32(A, _mm256_set1_epi32(b)); }
ICBC_FORCEINLINE VInt operator& (VInt A, int b) { return _mm256_and_si256(A, _mm256_set1_epi32(b)); }
ICBC_FORCEINLINE VInt operator>> (VInt A, int b) { return _mm256_srli_epi32(A, b); }

ICBC_FORCEINLINE VMask operator> (VInt A, int b) { return _mm256_castsi256_ps(_mm256_cmpgt_epi32(A, _mm256_set1_epi32(b))); }
ICBC_FORCEINLINE VMask operator== (VInt A, int b) { return _mm256_castsi256_ps(_mm256_cmpeq_epi32(A, _mm256_set1_epi32(b))); }

// mask ? v[idx] : 0
ICBC_FORCEINLINE VFloat vpermuteif(VMask mask, VFloat v, VInt idx) {
    return _mm256_and_ps(_mm256_permutevar8x32_ps(v, idx), mask);
}

// mask ? (idx > 8 ? vhi[idx] : vlo[idx]) : 0
ICBC_FORCEINLINE VFloat vpermute2if(VMask mask, VFloat vlo, VFloat vhi, VInt idx) {
#if 0
    VMask mhi = idx > 7;
    vlo = _mm256_permutevar8x32_ps(vlo, idx);
    vhi = _mm256_permutevar8x32_ps(vhi, idx);
    VFloat v = _mm256_blendv_ps(vlo, vhi, mhi);
    return _mm256_and_ps(v, mask);
#else
    // Fabian Giesen says not to mix _mm256_blendv_ps and _mm256_permutevar8x32_ps since they contend for the same gates and instead suggests the following:
    vhi = _mm256_xor_ps(vhi, vlo);
    VFloat v = _mm256_permutevar8x32_ps(vlo, idx);
    VMask mhi = idx > 7;
    v = _mm256_xor_ps(v, _mm256_and_ps(_mm256_permutevar8x32_ps(vhi, idx), mhi));
    return _mm256_and_ps(v, mask);
#endif
}


#elif ICBC_SIMD == ICBC_AVX512

constexpr int VEC_SIZE = 16;

#if __GNUC__
union VFloat {
    __m512 v;
    float m512_f32[VEC_SIZE];

    VFloat() {}
    VFloat(__m512 v) : v(v) {}
    operator __m512 & () { return v; }
};
union VInt {
    __m512i v;
    int m512i_i32[VEC_SIZE];

    VInt() {}
    VInt(__m512i v) : v(v) {}
    operator __m512i & () { return v; }
};
#else
using VFloat = __m512;
using VInt = __m512i;
#endif
struct VMask { __mmask16 m; };

ICBC_FORCEINLINE float & lane(VFloat & v, int i) {
    return v.m512_f32[i];
}

ICBC_FORCEINLINE VFloat vzero() {
    return _mm512_setzero_ps();
}

ICBC_FORCEINLINE VFloat vbroadcast(float a) {
    return _mm512_set1_ps(a);
}

ICBC_FORCEINLINE VFloat vload(const float * ptr) {
    return _mm512_load_ps(ptr);
}

ICBC_FORCEINLINE VFloat vload(VMask mask, const float * ptr) {
    return _mm512_mask_load_ps(_mm512_undefined(), mask.m, ptr);
}

ICBC_FORCEINLINE VFloat vload(VMask mask, const float * ptr, float fallback) {
    return _mm512_mask_load_ps(_mm512_set1_ps(fallback), mask.m, ptr);
}

ICBC_FORCEINLINE VFloat vgather(const float * base, VFloat index) {
    return _mm512_i32gather_ps(_mm512_cvtps_epi32(index), base, 4);
}

ICBC_FORCEINLINE VFloat operator+(VFloat a, VFloat b) {
    return _mm512_add_ps(a, b);
}

ICBC_FORCEINLINE VFloat operator-(VFloat a, VFloat b) {
    return _mm512_sub_ps(a, b);
}

ICBC_FORCEINLINE VFloat operator*(VFloat a, VFloat b) {
    return _mm512_mul_ps(a, b);
}

ICBC_FORCEINLINE VFloat vrcp(VFloat a) {
#if ICBC_USE_RCP
    VFloat r = _mm512_rcp14_ps(a);

    // r = r * (2 - r * a)
    return _mm512_mul_ps(r, _mm512_fnmadd_ps(r, a, vbroadcast(2.0f)));
#else
    return _mm512_div_ps(vbroadcast(1.0f), a);
#endif
}

// a*b+c
ICBC_FORCEINLINE VFloat vmadd(VFloat a, VFloat b, VFloat c) {
    return _mm512_fmadd_ps(a, b, c);
}

ICBC_FORCEINLINE VFloat vmsub(VFloat a, VFloat b, VFloat c) {
    return _mm512_fmsub_ps(a, b, c);
}

ICBC_FORCEINLINE VFloat vm2sub(VFloat a, VFloat b, VFloat c, VFloat d) {
    return vmsub(a, b, c * d);
}

ICBC_FORCEINLINE VFloat vsaturate(VFloat a) {
    auto zero = _mm512_setzero_ps();
    auto one = _mm512_set1_ps(1.0f);
    return _mm512_min_ps(_mm512_max_ps(a, zero), one);
}

ICBC_FORCEINLINE VFloat vround01(VFloat a) {
    return _mm512_roundscale_ps(a, _MM_FROUND_TO_NEAREST_INT);
}

ICBC_FORCEINLINE VFloat vtruncate(VFloat a) {
    return _mm512_roundscale_ps(a, _MM_FROUND_TO_ZERO);
}

ICBC_FORCEINLINE VFloat lane_id() {
    return _mm512_set_ps(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
}

ICBC_FORCEINLINE VMask operator> (VFloat A, VFloat B) { return { _mm512_cmp_ps_mask(A, B, _CMP_GT_OQ) }; }
ICBC_FORCEINLINE VMask operator>=(VFloat A, VFloat B) { return { _mm512_cmp_ps_mask(A, B, _CMP_GE_OQ) }; }
ICBC_FORCEINLINE VMask operator< (VFloat A, VFloat B) { return { _mm512_cmp_ps_mask(A, B, _CMP_LT_OQ) }; }
ICBC_FORCEINLINE VMask operator<=(VFloat A, VFloat B) { return { _mm512_cmp_ps_mask(A, B, _CMP_LE_OQ) }; }

ICBC_FORCEINLINE VMask operator! (VMask A) { return { _mm512_knot(A.m) }; }
ICBC_FORCEINLINE VMask operator| (VMask A, VMask B) { return { _mm512_kor(A.m, B.m) }; }
ICBC_FORCEINLINE VMask operator& (VMask A, VMask B) { return { _mm512_kand(A.m, B.m) }; }
ICBC_FORCEINLINE VMask operator^ (VMask A, VMask B) { return { _mm512_kxor(A.m, B.m) }; }

// mask ? b : a
ICBC_FORCEINLINE VFloat vselect(VMask mask, VFloat a, VFloat b) {
    return _mm512_mask_blend_ps(mask.m, a, b);
}

ICBC_FORCEINLINE VMask vbroadcast(bool b) { 
    return { __mmask16(-int16_t(b)) };
}

ICBC_FORCEINLINE bool all(VMask mask) {
    return mask.m == 0xFFFFFFFF;
}

ICBC_FORCEINLINE bool any(VMask mask) {
    return mask.m != 0;
}

ICBC_FORCEINLINE uint mask(VMask mask) {
    return mask.m;
}

ICBC_FORCEINLINE int reduce_min_index(VFloat v) {

    // First do an horizontal reduction.
    VFloat vmin = vbroadcast(_mm512_reduce_min_ps(v));

    // Then find the index.
    VMask mask = (v <= vmin);
    return ctz(mask.m);
}

//ICBC_FORCEINLINE void vtranspose4(VFloat & a, VFloat & b, VFloat & c, VFloat & d); // @@

ICBC_FORCEINLINE int lane(VInt v, int i) {
    //return _mm256_extract_epi32(v, i);
    return v.m512i_i32[i];
}

ICBC_FORCEINLINE VInt vzeroi() {
    return _mm512_setzero_epi32();
}

ICBC_FORCEINLINE VInt vbroadcast(int a) {
    return _mm512_set1_epi32(a);
}

ICBC_FORCEINLINE VInt vload(const int * ptr) {
    return _mm512_load_epi32(ptr);
}

ICBC_FORCEINLINE VInt operator- (VInt A, int b) { return _mm512_sub_epi32(A, vbroadcast(b)); }
ICBC_FORCEINLINE VInt operator& (VInt A, int b) { return _mm512_and_epi32(A, vbroadcast(b)); }
ICBC_FORCEINLINE VInt operator>> (VInt A, int b) { return _mm512_srli_epi32(A, b); }

ICBC_FORCEINLINE VMask operator> (VInt A, int b) { return { _mm512_cmpgt_epi32_mask(A, vbroadcast(b)) }; }
ICBC_FORCEINLINE VMask operator>=(VInt A, int b) { return { _mm512_cmpge_epi32_mask(A, vbroadcast(b)) }; }
ICBC_FORCEINLINE VMask operator== (VInt A, int b) { return { _mm512_cmpeq_epi32_mask(A, vbroadcast(b)) }; }

// mask ? v[idx] : 0
ICBC_FORCEINLINE VFloat vpermuteif(VMask mask, VFloat v, VInt idx) {
    return _mm512_maskz_permutexvar_ps(mask.m, idx, v);
}


#elif ICBC_SIMD == ICBC_NEON

constexpr int VEC_SIZE = 4;

#if __GNUC__
union VFloat {
    float32x4_t v;
    float e[4];
    VFloat() {}
    VFloat(float32x4_t v) : v(v) {}
    operator float32x4_t & () { return v; }
};
struct VMask {
    uint32x4_t v;
    VMask() {}
    VMask(uint32x4_t v) : v(v) {}
    operator uint32x4_t & () { return v; }
};
#else
using VFloat = float32x4_t;
using VMask = uint32x4_t;
#endif

ICBC_FORCEINLINE float & lane(VFloat & v, int i) {
#if defined(__clang__)
    return v.e[i];
#elif defined(_MSC_VER)
    return v.n128_f32[i];
#else
    return v.v[i];
#endif
}

ICBC_FORCEINLINE VFloat vzero() {
    return vdupq_n_f32(0.0f);
}

ICBC_FORCEINLINE VFloat vbroadcast(float a) {
    return vdupq_n_f32(a);
}

ICBC_FORCEINLINE VFloat vload(const float * ptr) {
    return vld1q_f32(ptr);
}

ICBC_FORCEINLINE VFloat operator+(VFloat a, VFloat b) {
    return vaddq_f32(a, b);
}

ICBC_FORCEINLINE VFloat operator-(VFloat a, VFloat b) {
    return vsubq_f32(a, b);
}

ICBC_FORCEINLINE VFloat operator*(VFloat a, VFloat b) {
    return vmulq_f32(a, b);
}

ICBC_FORCEINLINE VFloat vrcp(VFloat a) {
//#if ICBC_USE_RCP
    VFloat rcp = vrecpeq_f32(a);
    //return rcp;
    return vmulq_f32(vrecpsq_f32(a, rcp), rcp);
//#else
//    return vdiv_f32(vbroadcast(1.0f), a);    // @@ ARMv8 only?
//#endif
}

// a*b+c
ICBC_FORCEINLINE VFloat vmadd(VFloat a, VFloat b, VFloat c) {
    return vmlaq_f32(c, a, b);
}

ICBC_FORCEINLINE VFloat vmsub(VFloat a, VFloat b, VFloat c) {
    return a * b - c;
}

ICBC_FORCEINLINE VFloat vm2sub(VFloat a, VFloat b, VFloat c, VFloat d) {
    // vmlsq_f32(a, b, c) == a - (b * c)
    return vmlsq_f32(a * b, c, d);
}

ICBC_FORCEINLINE VFloat vsaturate(VFloat a) {
    return vminq_f32(vmaxq_f32(a, vzero()), vbroadcast(1));
}

ICBC_FORCEINLINE VFloat vround01(VFloat a) {
#if __ARM_RACH >= 8
    return vrndqn_f32(a);   // Round to integral (to nearest, ties to even)
#else
    return vcvtq_f32_s32(vcvtq_s32_f32(a + vbroadcast(0.5)));
#endif
}

ICBC_FORCEINLINE VFloat vtruncate(VFloat a) {
#if __ARM_RACH >= 8
    return vrndq_f32(a);
#else
    return vcvtq_f32_s32(vcvtq_s32_f32(a));
#endif
}

ICBC_FORCEINLINE VFloat lane_id() {
    ICBC_ALIGN_16 float data[4] = { 0, 1, 2, 3 };
	return vld1q_f32(data);
}

ICBC_FORCEINLINE VMask operator> (VFloat A, VFloat B) { return { vcgtq_f32(A, B) }; }
ICBC_FORCEINLINE VMask operator>=(VFloat A, VFloat B) { return { vcgeq_f32(A, B) }; }
ICBC_FORCEINLINE VMask operator< (VFloat A, VFloat B) { return { vcltq_f32(A, B) }; }
ICBC_FORCEINLINE VMask operator<=(VFloat A, VFloat B) { return { vcleq_f32(A, B) }; }

ICBC_FORCEINLINE VMask operator| (VMask A, VMask B) { return { vorrq_u32(A, B) }; }
ICBC_FORCEINLINE VMask operator& (VMask A, VMask B) { return { vandq_u32(A, B) }; }
ICBC_FORCEINLINE VMask operator^ (VMask A, VMask B) { return { veorq_u32(A, B) }; }

// mask ? b : a
ICBC_FORCEINLINE VFloat vselect(VMask mask, VFloat a, VFloat b) {
    return vbslq_f32(mask, b, a);
}

ICBC_FORCEINLINE bool all(VMask mask) {
    uint32x2_t m2 = vpmin_u32(vget_low_u32(mask), vget_high_u32(mask));
    uint32x2_t m1 = vpmin_u32(m2, m2);
#if defined(_MSC_VER)
    return m1.n64_u32[0] != 0;
#else
    return m1[0] != 0;
#endif
}

ICBC_FORCEINLINE bool any(VMask mask) {
    uint32x2_t m2 = vpmax_u32(vget_low_u32(mask), vget_high_u32(mask));
    uint32x2_t m1 = vpmax_u32(m2, m2);
#if defined(_MSC_VER)
    return m1.n64_u32[0] != 0;
#else
    return m1[0] != 0;
#endif
}

// @@ Is this the best we can do?
// From: https://github.com/jratcliff63367/sse2neon/blob/master/SSE2NEON.h
ICBC_FORCEINLINE uint mask(VMask mask) {
	static const uint32x4_t movemask = { 1, 2, 4, 8 };
	static const uint32x4_t highbit = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
	uint32x4_t t1 = vtstq_u32(mask, highbit);
	uint32x4_t t2 = vandq_u32(t1, movemask);
	uint32x2_t t3 = vorr_u32(vget_low_u32(t2), vget_high_u32(t2));
	return vget_lane_u32(t3, 0) | vget_lane_u32(t3, 1);
}

inline int reduce_min_index(VFloat v) {
#if 0
    float32x2_t m2 = vpmin_f32(vget_low_u32(V), vget_high_u32(v));
    float32x2_t m1 = vpmin_f32(m2, m2);
    float min_value = vget_lane_f32(m1, 0);
    VFloat vmin = vbroadcast(min_value);

    // (v <= vmin)

    // @@ Find the lane that contains minValue?
#endif

    // @@ Is there a better way to do this reduction?
    int min_idx = 0;
    float min_value = lane(v, 0);

    for (int i = 1; i < VEC_SIZE; i++) {
        float value = lane(v, i);
        if (value < min_value) {
            min_value = value;
            min_idx = i;
        }
    }

    return min_idx;
}

// https://github.com/Maratyszcza/NNPACK/blob/master/src/neon/transpose.h
ICBC_FORCEINLINE void vtranspose4(VFloat & a, VFloat & b, VFloat & c, VFloat & d) {
    // row0 = ( x00 x01 x02 x03 )
    // row1 = ( x10 x11 x12 x13 )
    // row2 = ( x20 x21 x22 x23 )
    // row3 = ( x30 x31 x32 x33 )

    // row01 = ( x00 x10 x02 x12 ), ( x01 x11 x03, x13 )
    // row23 = ( x20 x30 x22 x32 ), ( x21 x31 x23, x33 )
    float32x4x2_t row01 = vtrnq_f32(a, b);
    float32x4x2_t row23 = vtrnq_f32(c, d);

    // row0 = ( x00 x10 x20 x30 )
    // row1 = ( x01 x11 x21 x31 )
    // row2 = ( x02 x12 x22 x32 )
    // row3 = ( x03 x13 x23 x33 )
    a = vcombine_f32(vget_low_f32(row01.val[0]), vget_low_f32(row23.val[0]));
    b = vcombine_f32(vget_low_f32(row01.val[1]), vget_low_f32(row23.val[1]));
    c = vcombine_f32(vget_high_f32(row01.val[0]), vget_high_f32(row23.val[0]));
    d = vcombine_f32(vget_high_f32(row01.val[1]), vget_high_f32(row23.val[1]));
}

#elif ICBC_SIMD == ICBC_VMX

constexpr int VEC_SIZE = 4;

union VFloat {
    vectro float v;
    float e[4];
    VFloat() {}
    VFloat(vector float v) : v(v) {}
    operator vector float & () { return v; }
};
struct VMask {
    vector unsigned int v;
    VMask() {}
    VMask(vector unsigned int v) : v(v) {}
    operator vector unsigned int & () { return v; }
};

ICBC_FORCEINLINE float & lane(VFloat & v, int i) {
    return v.e[i];
}

ICBC_FORCEINLINE VFloat vzero() {
    return vec_splats(0.0f);
}

ICBC_FORCEINLINE VFloat vbroadcast(float a) {
    return vec_splats(a);
}

ICBC_FORCEINLINE VFloat vload(const float * ptr) {
    return vec_ld(ptr)
}

ICBC_FORCEINLINE VFloat operator+(VFloat a, VFloat b) {
    return vec_add(a, b);
}

ICBC_FORCEINLINE VFloat operator-(VFloat a, VFloat b) {
    return vec_sub(a, b);
}

ICBC_FORCEINLINE VFloat operator*(VFloat a, VFloat b) {
    return vec_madd(a, b, vec_splats(-0.0f));
}

ICBC_FORCEINLINE VFloat vrcp(VFloat a) {
#if ICBC_USE_RCP
    // get the reciprocal estimate
    vector float estimate = vec_re( v.vec );

    // one round of Newton-Rhaphson refinement
    vector float diff = vec_nmsub( estimate, v.vec, vec_splats( 1.0f ) );
    return vec_madd(diff, estimate, estimate );
#else
    return vec_div(vec_splats(1),a);
#endif
}

// a*b+c
ICBC_FORCEINLINE VFloat vmadd(VFloat a, VFloat b, VFloat c) {
    return vec_madd(a, b, c);
}

ICBC_FORCEINLINE VFloat vmsub(VFloat a, VFloat b, VFloat c) {
    return vec_msub(a, b, c); // @@ Is this right?
}

ICBC_FORCEINLINE VFloat vm2sub(VFloat a, VFloat b, VFloat c, VFloat d) {
    return vmsub(a, b, c * d);
}

ICBC_FORCEINLINE VFloat vsaturate(VFloat a) {
    return vec_min(vec_max(a, vzero()), vbroadcast(1));
}

ICBC_FORCEINLINE VFloat vround01(VFloat a) {
    // @@ Assumes a is positive and ~small
    return vec_trunc(a + vbroadcast(0.5));
}

ICBC_FORCEINLINE VFloat vtruncate(VFloat a) {
    return vec_trunc(a);
}

ICBC_FORCEINLINE VFloat lane_id() {
    return (VFloat){ 0, 1, 2, 3 };
}

ICBC_FORCEINLINE VMask operator> (VFloat A, VFloat B) { return { vec_cmpgt(A, B) }; }
ICBC_FORCEINLINE VMask operator>=(VFloat A, VFloat B) { return { vec_cmpge(A, B) }; }
ICBC_FORCEINLINE VMask operator< (VFloat A, VFloat B) { return { vec_cmplt(A, B) }; }
ICBC_FORCEINLINE VMask operator<=(VFloat A, VFloat B) { return { vec_cmple(A, B) }; }

ICBC_FORCEINLINE VMask operator| (VMask A, VMask B) { return { vec_or(A, B) }; }
ICBC_FORCEINLINE VMask operator& (VMask A, VMask B) { return { vec_and(A, B) }; }
ICBC_FORCEINLINE VMask operator^ (VMask A, VMask B) { return { vec_xor(A, B) }; }

// mask ? b : a
ICBC_FORCEINLINE VFloat vselect(VMask mask, VFloat a, VFloat b) {
    return vec_sel(a, b, mask);
}

ICBC_FORCEINLINE int reduce_min_index(VFloat v) {

    //VFloat vmin =  //@@ Horizontal min?
    //return vec_cmpeq_idx(v, vmin);

    // @@ Is there a better way to do this reduction?
    int min_idx = 0;
    float min_value = lane(v, 0);

    for (int i = 1; i < VEC_SIZE; i++) {
        float value = lane(v, i);
        if (value < min_value) {
            min_value = value;
            min_idx = i;
        }
    }

    return min_idx;
}

ICBC_FORCEINLINE void vtranspose4(VFloat & a, VFloat & b, VFloat & c, VFloat & d) {
    VFloat t1 = vec_mergeh(a, c);
    VFloat t2 = vec_mergel(a, c);
    VFloat t3 = vec_mergeh(b, d);
    VFloat t4 = vec_mergel(b, d);
    a = vec_mergeh(t1, t3);
    b = vec_mergel(t1, t3);
    c = vec_mergeh(t2, t4);
    d = vec_mergel(t2, t4);
}

#endif // ICBC_SIMD == *

#if ICBC_SIMD != ICBC_SCALAR
ICBC_FORCEINLINE VFloat vmadd(VFloat a, float b, VFloat c) {
    VFloat vb = vbroadcast(b);
    return vmadd(a, vb, c);
}
#endif

struct VVector3 {
    VFloat x;
    VFloat y;
    VFloat z;
};

ICBC_FORCEINLINE VVector3 vbroadcast(Vector3 v) {
    VVector3 v8;

    v8.x = vbroadcast(v.x);
    v8.y = vbroadcast(v.y);
    v8.z = vbroadcast(v.z);
    return v8;
}

ICBC_FORCEINLINE VVector3 vbroadcast(float x, float y, float z) {
    VVector3 v8;
    v8.x = vbroadcast(x);
    v8.y = vbroadcast(y);
    v8.z = vbroadcast(z);
    return v8;
}

/*ICBC_FORCEINLINE VVector3 vload(const Vector3 * v) {
    // @@ See this for a 8x3 transpose: https://software.intel.com/content/www/us/en/develop/articles/3d-vector-normalization-using-256-bit-intel-advanced-vector-extensions-intel-avx.html
}*/

ICBC_FORCEINLINE VVector3 vload(const Vector4 * ptr) {
#if ICBC_SIMD == ICBC_AVX512

    // @@ AVX512 transpose not implemented.
    __m512i vindex = _mm512_set_epi32(4 * 15, 4 * 14, 4 * 13, 4 * 12, 4 * 11, 4 * 10, 4 * 9, 4 * 8, 4 * 7, 4 * 6, 4 * 5, 4 * 4, 4 * 3, 4 * 2, 4 * 1, 0);

    VVector3 v;
    v.x = _mm512_i32gather_ps(vindex, &ptr->x, 4);
    v.y = _mm512_i32gather_ps(vindex, &ptr->y, 4);
    v.z = _mm512_i32gather_ps(vindex, &ptr->z, 4);
    return v;

#else

    VVector3 v;
    v.x = vload(&ptr->x + 0 * VEC_SIZE);
    v.y = vload(&ptr->x + 1 * VEC_SIZE);
    v.z = vload(&ptr->x + 2 * VEC_SIZE);
    VFloat tmp = vload(&ptr->x + 3 * VEC_SIZE);

    vtranspose4(v.x, v.y, v.z, tmp);

    return v;
#endif
}


ICBC_FORCEINLINE VVector3 operator+(VVector3 a, VVector3 b) {
    VVector3 v;
    v.x = (a.x + b.x);
    v.y = (a.y + b.y);
    v.z = (a.z + b.z);
    return v;
}

ICBC_FORCEINLINE VVector3 operator-(VVector3 a, VVector3 b) {
    VVector3 v8;
    v8.x = (a.x - b.x);
    v8.y = (a.y - b.y);
    v8.z = (a.z - b.z);
    return v8;
}

ICBC_FORCEINLINE VVector3 operator*(VVector3 a, VVector3 b) {
    VVector3 v8;
    v8.x = (a.x * b.x);
    v8.y = (a.y * b.y);
    v8.z = (a.z * b.z);
    return v8;
}

ICBC_FORCEINLINE VVector3 operator*(VVector3 a, VFloat b) {
    VVector3 v8;
    v8.x = (a.x * b);
    v8.y = (a.y * b);
    v8.z = (a.z * b);
    return v8;
}

ICBC_FORCEINLINE VVector3 vmadd(VVector3 a, VVector3 b, VVector3 c) {
    VVector3 v8;
    v8.x = vmadd(a.x, b.x, c.x);
    v8.y = vmadd(a.y, b.y, c.y);
    v8.z = vmadd(a.z, b.z, c.z);
    return v8;
}

ICBC_FORCEINLINE VVector3 vmadd(VVector3 a, VFloat b, VVector3 c) {
    VVector3 v8;
    v8.x = vmadd(a.x, b, c.x);
    v8.y = vmadd(a.y, b, c.y);
    v8.z = vmadd(a.z, b, c.z);
    return v8;
}

#if ICBC_SIMD != ICBC_SCALAR
ICBC_FORCEINLINE VVector3 vmadd(VVector3 a, float b, VVector3 c) {
    VVector3 v8;
    VFloat vb = vbroadcast(b);
    v8.x = vmadd(a.x, vb, c.x);
    v8.y = vmadd(a.y, vb, c.y);
    v8.z = vmadd(a.z, vb, c.z);
    return v8;
}
#endif

ICBC_FORCEINLINE VVector3 vmsub(VVector3 a, VFloat b, VFloat c) {
    VVector3 v8;
    v8.x = vmsub(a.x, b, c);
    v8.y = vmsub(a.y, b, c);
    v8.z = vmsub(a.z, b, c);
    return v8;
}

ICBC_FORCEINLINE VVector3 vmsub(VVector3 a, VFloat b, VVector3 c) {
    VVector3 v8;
    v8.x = vmsub(a.x, b, c.x);
    v8.y = vmsub(a.y, b, c.y);
    v8.z = vmsub(a.z, b, c.z);
    return v8;
}

ICBC_FORCEINLINE VVector3 vm2sub(VVector3 a, VFloat b, VVector3 c, VFloat d) {
    VVector3 v;
    v.x = vm2sub(a.x, b, c.x, d);
    v.y = vm2sub(a.y, b, c.y, d);
    v.z = vm2sub(a.z, b, c.z, d);
    return v;
}

ICBC_FORCEINLINE VVector3 vm2sub(VVector3 a, VVector3 b, VVector3 c, VFloat d) {
    VVector3 v;
    v.x = vm2sub(a.x, b.x, c.x, d);
    v.y = vm2sub(a.y, b.y, c.y, d);
    v.z = vm2sub(a.z, b.z, c.z, d);
    return v;
}

ICBC_FORCEINLINE VVector3 vm2sub(VVector3 a, VVector3 b, VVector3 c, VVector3 d) {
    VVector3 v;
    v.x = vm2sub(a.x, b.x, c.x, d.x);
    v.y = vm2sub(a.y, b.y, c.y, d.y);
    v.z = vm2sub(a.z, b.z, c.z, d.z);
    return v;
}

ICBC_FORCEINLINE VFloat vdot(VVector3 a, VVector3 b) {
    VFloat r;
    r = a.x * b.x + a.y * b.y + a.z * b.z;
    return r;
}

// Length squared.
ICBC_FORCEINLINE VFloat vlen2(VVector3 v) {
    return vdot(v, v);
}


// mask ? b : a
ICBC_FORCEINLINE VVector3 vselect(VMask mask, VVector3 a, VVector3 b) {
    VVector3 r;
    r.x = vselect(mask, a.x, b.x);
    r.y = vselect(mask, a.y, b.y);
    r.z = vselect(mask, a.z, b.z);
    return r;
}




///////////////////////////////////////////////////////////////////////////////////////////////////
// Color conversion functions.

static const float midpoints5[32] = {
    0.015686f, 0.047059f, 0.078431f, 0.111765f, 0.145098f, 0.176471f, 0.207843f, 0.241176f, 0.274510f, 0.305882f, 0.337255f, 0.370588f, 0.403922f, 0.435294f, 0.466667f, 0.5f,
    0.533333f, 0.564706f, 0.596078f, 0.629412f, 0.662745f, 0.694118f, 0.725490f, 0.758824f, 0.792157f, 0.823529f, 0.854902f, 0.888235f, 0.921569f, 0.952941f, 0.984314f, FLT_MAX
};

static const float midpoints6[64] = {
    0.007843f, 0.023529f, 0.039216f, 0.054902f, 0.070588f, 0.086275f, 0.101961f, 0.117647f, 0.133333f, 0.149020f, 0.164706f, 0.180392f, 0.196078f, 0.211765f, 0.227451f, 0.245098f,
    0.262745f, 0.278431f, 0.294118f, 0.309804f, 0.325490f, 0.341176f, 0.356863f, 0.372549f, 0.388235f, 0.403922f, 0.419608f, 0.435294f, 0.450980f, 0.466667f, 0.482353f, 0.500000f,
    0.517647f, 0.533333f, 0.549020f, 0.564706f, 0.580392f, 0.596078f, 0.611765f, 0.627451f, 0.643137f, 0.658824f, 0.674510f, 0.690196f, 0.705882f, 0.721569f, 0.737255f, 0.754902f,
    0.772549f, 0.788235f, 0.803922f, 0.819608f, 0.835294f, 0.850980f, 0.866667f, 0.882353f, 0.898039f, 0.913725f, 0.929412f, 0.945098f, 0.960784f, 0.976471f, 0.992157f, FLT_MAX
};

/*void init_tables() {
    for (int i = 0; i < 31; i++) {
        float f0 = float(((i+0) << 3) | ((i+0) >> 2)) / 255.0f;
        float f1 = float(((i+1) << 3) | ((i+1) >> 2)) / 255.0f;
        midpoints5[i] = (f0 + f1) * 0.5;
    }
    midpoints5[31] = FLT_MAX;

    for (int i = 0; i < 63; i++) {
        float f0 = float(((i+0) << 2) | ((i+0) >> 4)) / 255.0f;
        float f1 = float(((i+1) << 2) | ((i+1) >> 4)) / 255.0f;
        midpoints6[i] = (f0 + f1) * 0.5;
    }
    midpoints6[63] = FLT_MAX;
}*/

ICBC_FORCEINLINE VFloat vround5(VFloat x) {
    const VFloat rb_scale = vbroadcast(31.0f);
    const VFloat rb_inv_scale = vbroadcast(1.0f / 31.0f);

#if ICBC_PERFECT_ROUND
    VFloat q = vtruncate(x * rb_scale);
    VFloat mp = vgather(midpoints5, q);
    //return (q + (vbroadcast(1.0f) & (x > mp))) * rb_inv_scale;
    return (q + vselect(x > mp, vzero(), vbroadcast(1))) * rb_inv_scale;
#else
    return vround01(x * rb_scale) * rb_inv_scale;
#endif
}

ICBC_FORCEINLINE VFloat vround6(VFloat x) {
    const VFloat g_scale = vbroadcast(63.0f);
    const VFloat g_inv_scale = vbroadcast(1.0f / 63.0f);

#if ICBC_PERFECT_ROUND
    VFloat q = vtruncate(x * g_scale);
    VFloat mp = vgather(midpoints6, q);
    //return (q + (vbroadcast(1) & (x > mp))) * g_inv_scale;
    return (q + vselect(x > mp, vzero(), vbroadcast(1))) * g_inv_scale;
#else
    return vround01(x * g_scale) * g_inv_scale;
#endif
}

ICBC_FORCEINLINE VVector3 vround_ept(VVector3 v) {
    VVector3 r;
    r.x = vround5(vsaturate(v.x));
    r.y = vround6(vsaturate(v.y));
    r.z = vround5(vsaturate(v.z));
    return r;
}

static Color16 vector3_to_color16(const Vector3 & v) {

    // Truncate.
    uint r = uint(clamp(v.x * 31.0f, 0.0f, 31.0f));
	uint g = uint(clamp(v.y * 63.0f, 0.0f, 63.0f));
	uint b = uint(clamp(v.z * 31.0f, 0.0f, 31.0f));

    // Round exactly according to 565 bit-expansion.
    r += (v.x > midpoints5[r]);
    g += (v.y > midpoints6[g]);
    b += (v.z > midpoints5[b]);

    Color16 c;
    c.u = uint16((r << 11) | (g << 5) | b);
    return c;
}

static Color32 bitexpand_color16_to_color32(Color16 c16) {
    Color32 c32;
    //c32.b = (c16.b << 3) | (c16.b >> 2);
    //c32.g = (c16.g << 2) | (c16.g >> 4);
    //c32.r = (c16.r << 3) | (c16.r >> 2);
    //c32.a = 0xFF;

    c32.u = ((c16.u << 3) & 0xf8) | ((c16.u << 5) & 0xfc00) | ((c16.u << 8) & 0xf80000);
    c32.u |= (c32.u >> 5) & 0x070007;
    c32.u |= (c32.u >> 6) & 0x000300;

    return c32;
}

inline Vector3 color_to_vector3(Color32 c) {
    return { c.r / 255.0f, c.g / 255.0f, c.b / 255.0f };
}

inline Color32 vector3_to_color32(Vector3 v) {
    Color32 color;
    color.r = uint8(saturate(v.x) * 255 + 0.5f);
    color.g = uint8(saturate(v.y) * 255 + 0.5f);
    color.b = uint8(saturate(v.z) * 255 + 0.5f);
    color.a = 255;
    return color;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Input block processing.

inline bool is_black(Vector3 c) {
    // This large threshold seems to improve compression. This is not forcing these texels to be black, just
    // causes them to be ignored during PCA.
    // @@ We may want to adjust this based on the quality level, since this increases the number of blocks that try cluster-3 fitting.
    //return c.x < midpoints5[0] && c.y < midpoints6[0] && c.z < midpoints5[0];
    //return c.x < 1.0f / 32 && c.y < 1.0f / 32 && c.z < 1.0f / 32;
    return c.x < 1.0f / 8 && c.y < 1.0f / 8 && c.z < 1.0f / 8;
}

// Find similar colors and combine them together.
static int reduce_colors(const Vector4 * input_colors, const float * input_weights, int count, float threshold, Vector3 * colors, float * weights, bool * any_black)
{
#if 0
    for (int i = 0; i < 16; i++) {
        colors[i] = input_colors[i].xyz;
        weights[i] = input_weights[i];
    }
    return 16;
#else
    *any_black = false;

    int n = 0;
    for (int i = 0; i < count; i++)
    {
        Vector3 ci = input_colors[i].xyz;
        float wi = input_weights[i];

        if (wi > 0) {
            // Find matching color.
            int j;
            for (j = 0; j < n; j++) {
                if (equal(colors[j], ci, threshold)) {
                    colors[j] = (colors[j] * weights[j] + ci * wi) / (weights[j] + wi);
                    weights[j] += wi;
                    break;
                }
            }

            // No match found. Add new color.
            if (j == n) {
                colors[n] = ci;
                weights[n] = wi;
                n++;
            }

            if (is_black(ci)) {
                *any_black = true;
            }
        }
    }

    ICBC_ASSERT(n <= count);

    return n;
#endif
}

static int skip_blacks(const Vector3 * input_colors, const float * input_weights, int count, Vector3 * colors, float * weights)
{
    int n = 0;
    for (int i = 0; i < count; i++)
    {
        Vector3 ci = input_colors[i];
        float wi = input_weights[i];

        if (is_black(ci)) {
            continue;
        }

        colors[n] = ci;
        weights[n] = wi;
        n += 1;
    }

    return n;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// PCA

static Vector3 computeCentroid(int n, const Vector3 *__restrict points, const float *__restrict weights)
{
    Vector3 centroid = { 0 };
    float total = 0.0f;

    for (int i = 0; i < n; i++)
    {
        total += weights[i];
        centroid += weights[i] * points[i];
    }
    centroid *= (1.0f / total);

    return centroid;
}

static Vector3 computeCovariance(int n, const Vector3 *__restrict points, const float *__restrict weights, float *__restrict covariance)
{
    // compute the centroid
    Vector3 centroid = computeCentroid(n, points, weights);

    // compute covariance matrix
    for (int i = 0; i < 6; i++)
    {
        covariance[i] = 0.0f;
    }

    for (int i = 0; i < n; i++)
    {
        Vector3 a = (points[i] - centroid);    // @@ I think weight should be squared, but that seems to increase the error slightly.
        Vector3 b = weights[i] * a;

        covariance[0] += a.x * b.x;
        covariance[1] += a.x * b.y;
        covariance[2] += a.x * b.z;
        covariance[3] += a.y * b.y;
        covariance[4] += a.y * b.z;
        covariance[5] += a.z * b.z;
    }

    return centroid;
}

// @@ We should be able to do something cheaper...
static Vector3 estimatePrincipalComponent(const float * __restrict matrix)
{
    const Vector3 row0 = { matrix[0], matrix[1], matrix[2] };
    const Vector3 row1 = { matrix[1], matrix[3], matrix[4] };
    const Vector3 row2 = { matrix[2], matrix[4], matrix[5] };

    float r0 = lengthSquared(row0);
    float r1 = lengthSquared(row1);
    float r2 = lengthSquared(row2);

    if (r0 > r1 && r0 > r2) return row0;
    if (r1 > r2) return row1;
    return row2;
}

static inline Vector3 firstEigenVector_PowerMethod(const float *__restrict matrix)
{
    if (matrix[0] == 0 && matrix[3] == 0 && matrix[5] == 0)
    {
        return {0};
    }

    Vector3 v = estimatePrincipalComponent(matrix);

    const int NUM = 8;
    for (int i = 0; i < NUM; i++)
    {
        float x = v.x * matrix[0] + v.y * matrix[1] + v.z * matrix[2];
        float y = v.x * matrix[1] + v.y * matrix[3] + v.z * matrix[4];
        float z = v.x * matrix[2] + v.y * matrix[4] + v.z * matrix[5];

        float norm = max(max(x, y), z);

        v = { x, y, z };
        v *= (1.0f / norm);
    }

    return v;
}

static Vector3 computePrincipalComponent_PowerMethod(int n, const Vector3 *__restrict points, const float *__restrict weights)
{
    float matrix[6];
    computeCovariance(n, points, weights, matrix);

    return firstEigenVector_PowerMethod(matrix);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// SAT

struct SummedAreaTable {
    ICBC_ALIGN_16 float r[16];
    ICBC_ALIGN_16 float g[16];
    ICBC_ALIGN_16 float b[16];
    ICBC_ALIGN_16 float w[16];
};

int compute_sat(const Vector3 * colors, const float * weights, int count, SummedAreaTable * sat)
{
    // I've tried using a lower quality approximation of the principal direction, but the best fit line seems to produce best results.
    Vector3 principal = computePrincipalComponent_PowerMethod(count, colors, weights);

    // build the list of values
    int order[16];
    float dps[16];
    for (int i = 0; i < count; ++i)
    {
        order[i] = i;
        dps[i] = dot(colors[i], principal);
    }

    // stable sort
    for (int i = 0; i < count; ++i)
    {
        for (int j = i; j > 0 && dps[j] < dps[j - 1]; --j)
        {
            swap(dps[j], dps[j - 1]);
            swap(order[j], order[j - 1]);
        }
    }

    float w = weights[order[0]];
    sat->r[0] = colors[order[0]].x * w;
    sat->g[0] = colors[order[0]].y * w;
    sat->b[0] = colors[order[0]].z * w;
    sat->w[0] = w;

    for (int i = 1; i < count; i++) {
        w = weights[order[i]];
        sat->r[i] = sat->r[i - 1] + colors[order[i]].x * w;
        sat->g[i] = sat->g[i - 1] + colors[order[i]].y * w;
        sat->b[i] = sat->b[i - 1] + colors[order[i]].z * w;
        sat->w[i] = sat->w[i - 1] + w;
    }

    // Try incremental decimation:
    /*if (count > 4)
    {
        float threshold = 1.0f / 4;

        for (uint i = 1; i < count; ++i)
        {
            if (sat->r[i] - sat->r[i - 1] < threshold &&
                sat->g[i] - sat->g[i - 1] < threshold &&
                sat->b[i] - sat->b[i - 1] < threshold)
            {
                for (int j = i+1; j < count; j++) {
                    sat->r[j - 1] = sat->r[j];
                    sat->g[j - 1] = sat->g[j];
                    sat->b[j - 1] = sat->b[j];
                    sat->w[j - 1] = sat->w[j];
                }
                count -= 1;
                i -= 1;
                if (count == 4) break;
            }
        }
    }*/

    for (int i = count; i < 16; i++) {
        sat->r[i] = FLT_MAX;
        sat->g[i] = FLT_MAX;
        sat->b[i] = FLT_MAX;
        sat->w[i] = FLT_MAX;
    }

    return count;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Cluster Fit

struct Combinations {
    uint8 c0, c1, c2, pad;
};

static ICBC_ALIGN_16 int s_fourClusterTotal[16];
static ICBC_ALIGN_16 int s_threeClusterTotal[16];
static ICBC_ALIGN_16 Combinations s_fourCluster[968 + 8];
static ICBC_ALIGN_16 Combinations s_threeCluster[152 + 8];

#if ICBC_USE_NEON_VTL
static uint8 s_neon_vtl_index0_4[4 * 968];
static uint8 s_neon_vtl_index1_4[4 * 968];
static uint8 s_neon_vtl_index2_4[4 * 968];

static uint8 s_neon_vtl_index0_3[4 * 152];
static uint8 s_neon_vtl_index1_3[4 * 152];
#endif

static void init_cluster_tables() {

    for (int t = 1, i = 0; t <= 16; t++) {
        for (int c0 = 0; c0 <= t; c0++) {
            for (int c1 = 0; c1 <= t - c0; c1++) {
                for (int c2 = 0; c2 <= t - c0 - c1; c2++) {

                    // Skip this cluster so that the total is a multiple of 8
                    if (c0 == 0 && c1 == 0 && c2 == 0) continue;

                    bool found = false;
                    if (t > 1) {
                        for (int j = 0; j < s_fourClusterTotal[t-2]; j++) {
                            if (s_fourCluster[j].c0 == c0 && s_fourCluster[j].c1 == c0+c1 && s_fourCluster[j].c2 == c0+c1+c2) {
                                found = true;
                                break;
                            }
                        }
                    }

                    if (!found) {
                        s_fourCluster[i].c0 = uint8(c0);
                        s_fourCluster[i].c1 = uint8(c0+c1);
                        s_fourCluster[i].c2 = uint8(c0+c1+c2);
                        i++;
                    }
                }
            }
        }

        s_fourClusterTotal[t - 1] = i;
    }

    // Replicate last entry.
    for (int i = 0; i < 8; i++) {
        s_fourCluster[968 + i] = s_fourCluster[968-1];
    }

    for (int t = 1, i = 0; t <= 16; t++) {
        for (int c0 = 0; c0 <= t; c0++) {
            for (int c1 = 0; c1 <= t - c0; c1++) {

                // Skip this cluster so that the total is a multiple of 8
                if (c0 == 0 && c1 == 0) continue;

                bool found = false;
                if (t > 1) {
                    for (int j = 0; j < s_threeClusterTotal[t - 2]; j++) {
                        if (s_threeCluster[j].c0 == c0 && s_threeCluster[j].c1 == c0+c1) {
                            found = true;
                            break;
                        }
                    }
                }

                if (!found) {
                    s_threeCluster[i].c0 = uint8(c0);
                    s_threeCluster[i].c1 = uint8(c0 + c1);
                    i++;
                }
            }
        }

        s_threeClusterTotal[t - 1] = i;
    }

    // Replicate last entry.
    for (int i = 0; i < 8; i++) {
        s_threeCluster[152 + i] = s_threeCluster[152 - 1];
    }

#if ICBC_USE_NEON_VTL
    for (int i = 0; i < 968; i++) {
        int c0 = (s_fourCluster[i].c0) - 1;
        s_neon_vtl_index0_4[4 * i + 0] = uint8(c0 * 4 + 0);
        s_neon_vtl_index0_4[4 * i + 1] = uint8(c0 * 4 + 1);
        s_neon_vtl_index0_4[4 * i + 2] = uint8(c0 * 4 + 2);
        s_neon_vtl_index0_4[4 * i + 3] = uint8(c0 * 4 + 3);

        int c1 = (s_fourCluster[i].c1) - 1;
        s_neon_vtl_index1_4[4 * i + 0] = uint8(c1 * 4 + 0);
        s_neon_vtl_index1_4[4 * i + 1] = uint8(c1 * 4 + 1);
        s_neon_vtl_index1_4[4 * i + 2] = uint8(c1 * 4 + 2);
        s_neon_vtl_index1_4[4 * i + 3] = uint8(c1 * 4 + 3);

        int c2 = (s_fourCluster[i].c2) - 1;
        s_neon_vtl_index2_4[4 * i + 0] = uint8(c2 * 4 + 0);
        s_neon_vtl_index2_4[4 * i + 1] = uint8(c2 * 4 + 1);
        s_neon_vtl_index2_4[4 * i + 2] = uint8(c2 * 4 + 2);
        s_neon_vtl_index2_4[4 * i + 3] = uint8(c2 * 4 + 3);
    }

    for (int i = 0; i < 152; i++) {
        int c0 = (s_threeCluster[i].c0) - 1;
        s_neon_vtl_index0_3[4 * i + 0] = uint8(c0 * 4 + 0);
        s_neon_vtl_index0_3[4 * i + 1] = uint8(c0 * 4 + 1);
        s_neon_vtl_index0_3[4 * i + 2] = uint8(c0 * 4 + 2);
        s_neon_vtl_index0_3[4 * i + 3] = uint8(c0 * 4 + 3);

        int c1 = (s_threeCluster[i].c1) - 1;
        s_neon_vtl_index1_3[4 * i + 0] = uint8(c1 * 4 + 0);
        s_neon_vtl_index1_3[4 * i + 1] = uint8(c1 * 4 + 1);
        s_neon_vtl_index1_3[4 * i + 2] = uint8(c1 * 4 + 2);
        s_neon_vtl_index1_3[4 * i + 3] = uint8(c1 * 4 + 3);
    }
#endif
}



static void cluster_fit_three(const SummedAreaTable & sat, int count, Vector3 metric_sqr, Vector3 * start, Vector3 * end)
{
    const float r_sum = sat.r[count-1];
    const float g_sum = sat.g[count-1];
    const float b_sum = sat.b[count-1];
    const float w_sum = sat.w[count-1];

    VFloat vbesterror = vbroadcast(FLT_MAX);
    VVector3 vbeststart = { vzero(), vzero(), vzero() };
    VVector3 vbestend = { vzero(), vzero(), vzero() };

    // check all possible clusters for this total order
    const int total_order_count = s_threeClusterTotal[count - 1];

    for (int i = 0; i < total_order_count; i += VEC_SIZE)
    {
        VVector3 x0, x1;
        VFloat w0, w1;

#if ICBC_USE_AVX512_PERMUTE

        auto loadmask = lane_id() < vbroadcast(float(count));

        // Load sat in one register:
        VFloat vrsat = vload(loadmask, sat.r, FLT_MAX);
        VFloat vgsat = vload(loadmask, sat.g, FLT_MAX);
        VFloat vbsat = vload(loadmask, sat.b, FLT_MAX);
        VFloat vwsat = vload(loadmask, sat.w, FLT_MAX);

        // Load 4 uint8 per lane.
        VInt packedClusterIndex = vload((int *)&s_threeCluster[i]);

        VInt c0 = (packedClusterIndex & 0xFF) - 1;
        VInt c1 = (packedClusterIndex >> 8) - 1;

        x0.x = vpermuteif(c0 >= 0, vrsat, c0);
        x0.y = vpermuteif(c0 >= 0, vgsat, c0);
        x0.z = vpermuteif(c0 >= 0, vbsat, c0);
        w0   = vpermuteif(c0 >= 0, vwsat, c0);

        x1.x = vpermuteif(c1 >= 0, vrsat, c1);
        x1.y = vpermuteif(c1 >= 0, vgsat, c1);
        x1.z = vpermuteif(c1 >= 0, vbsat, c1);
        w1   = vpermuteif(c1 >= 0, vwsat, c1);

#elif ICBC_USE_AVX2_PERMUTE2

        // Load 4 uint8 per lane. @@ Ideally I should pack this better and load only 2.
        VInt packedClusterIndex = vload((int *)&s_threeCluster[i]);

        VInt c0 = (packedClusterIndex & 0xFF);
        VInt c1 = ((packedClusterIndex >> 8)); // No need for & 0xFF

        if (count <= 8) {
            // Load sat.r in one register:
            VFloat rLo = vload(sat.r);
            VFloat gLo = vload(sat.g);
            VFloat bLo = vload(sat.b);
            VFloat wLo = vload(sat.w);

            x0.x = vpermuteif(c0>0, rLo, c0-1);
            x0.y = vpermuteif(c0>0, gLo, c0-1);
            x0.z = vpermuteif(c0>0, bLo, c0-1);
            w0   = vpermuteif(c0>0, wLo, c0-1);

            x1.x = vpermuteif(c1>0, rLo, c1-1);
            x1.y = vpermuteif(c1>0, gLo, c1-1);
            x1.z = vpermuteif(c1>0, bLo, c1-1);
            w1   = vpermuteif(c1>0, wLo, c1-1);
        }
        else {
            // Load sat.r in two registers:
            VFloat rLo = vload(sat.r); VFloat rHi = vload(sat.r + 8);
            VFloat gLo = vload(sat.g); VFloat gHi = vload(sat.g + 8);
            VFloat bLo = vload(sat.b); VFloat bHi = vload(sat.b + 8);
            VFloat wLo = vload(sat.w); VFloat wHi = vload(sat.w + 8);

            x0.x = vpermute2if(c0>0, rLo, rHi, c0-1);
            x0.y = vpermute2if(c0>0, gLo, gHi, c0-1);
            x0.z = vpermute2if(c0>0, bLo, bHi, c0-1);
            w0   = vpermute2if(c0>0, wLo, wHi, c0-1);

            x1.x = vpermute2if(c1>0, rLo, rHi, c1-1);
            x1.y = vpermute2if(c1>0, gLo, gHi, c1-1);
            x1.z = vpermute2if(c1>0, bLo, bHi, c1-1);
            w1   = vpermute2if(c1>0, wLo, wHi, c1-1);
        }

#elif ICBC_USE_NEON_VTL

        uint8x16_t idx0 = (uint8x16_t &)s_neon_vtl_index0_3[4*i];
        uint8x16_t idx1 = (uint8x16_t &)s_neon_vtl_index1_3[4*i];

        if (count <= 4) {
            uint8x16_t rsat1 = vld1q_u8((uint8*)sat.r);
            uint8x16_t gsat1 = vld1q_u8((uint8*)sat.g);
            uint8x16_t bsat1 = vld1q_u8((uint8*)sat.b);
            uint8x16_t wsat1 = vld1q_u8((uint8*)sat.w);

            x0.x = vreinterpretq_f32_u8(vqtbl1q_u8(rsat1, idx0));
            x0.y = vreinterpretq_f32_u8(vqtbl1q_u8(gsat1, idx0));
            x0.z = vreinterpretq_f32_u8(vqtbl1q_u8(bsat1, idx0));
            w0   = vreinterpretq_f32_u8(vqtbl1q_u8(wsat1, idx0));

            x1.x = vreinterpretq_f32_u8(vqtbl1q_u8(rsat1, idx1));
            x1.y = vreinterpretq_f32_u8(vqtbl1q_u8(gsat1, idx1));
            x1.z = vreinterpretq_f32_u8(vqtbl1q_u8(bsat1, idx1));
            w1   = vreinterpretq_f32_u8(vqtbl1q_u8(wsat1, idx1));
        }
        else if (count <= 8) {
            uint8x16x2_t rsat2 = vld2q_u8((uint8*)sat.r);
            uint8x16x2_t gsat2 = vld2q_u8((uint8*)sat.g);
            uint8x16x2_t bsat2 = vld2q_u8((uint8*)sat.b);
            uint8x16x2_t wsat2 = vld2q_u8((uint8*)sat.w);

            x0.x = vreinterpretq_f32_u8(vqtbl2q_u8(rsat2, idx0));
            x0.y = vreinterpretq_f32_u8(vqtbl2q_u8(gsat2, idx0));
            x0.z = vreinterpretq_f32_u8(vqtbl2q_u8(bsat2, idx0));
            w0   = vreinterpretq_f32_u8(vqtbl2q_u8(wsat2, idx0));

            x1.x = vreinterpretq_f32_u8(vqtbl2q_u8(rsat2, idx1));
            x1.y = vreinterpretq_f32_u8(vqtbl2q_u8(gsat2, idx1));
            x1.z = vreinterpretq_f32_u8(vqtbl2q_u8(bsat2, idx1));
            w1   = vreinterpretq_f32_u8(vqtbl2q_u8(wsat2, idx1));
        }
        else if (count <= 12) {
            uint8x16x3_t rsat3 = vld3q_u8((uint8*)sat.r);
            uint8x16x3_t gsat3 = vld3q_u8((uint8*)sat.g);
            uint8x16x3_t bsat3 = vld3q_u8((uint8*)sat.b);
            uint8x16x3_t wsat3 = vld3q_u8((uint8*)sat.w);

            x0.x = vreinterpretq_f32_u8(vqtbl3q_u8(rsat3, idx0));
            x0.y = vreinterpretq_f32_u8(vqtbl3q_u8(gsat3, idx0));
            x0.z = vreinterpretq_f32_u8(vqtbl3q_u8(bsat3, idx0));
            w0   = vreinterpretq_f32_u8(vqtbl3q_u8(wsat3, idx0));

            x1.x = vreinterpretq_f32_u8(vqtbl3q_u8(rsat3, idx1));
            x1.y = vreinterpretq_f32_u8(vqtbl3q_u8(gsat3, idx1));
            x1.z = vreinterpretq_f32_u8(vqtbl3q_u8(bsat3, idx1));
            w1   = vreinterpretq_f32_u8(vqtbl3q_u8(wsat3, idx1));
        }
        else {
            // Load SAT.
            uint8x16x4_t rsat4 = vld4q_u8((uint8*)sat.r);
            uint8x16x4_t gsat4 = vld4q_u8((uint8*)sat.g);
            uint8x16x4_t bsat4 = vld4q_u8((uint8*)sat.b);
            uint8x16x4_t wsat4 = vld4q_u8((uint8*)sat.w);

            x0.x = vreinterpretq_f32_u8(vqtbl4q_u8(rsat4, idx0));
            x0.y = vreinterpretq_f32_u8(vqtbl4q_u8(gsat4, idx0));
            x0.z = vreinterpretq_f32_u8(vqtbl4q_u8(bsat4, idx0));
            w0   = vreinterpretq_f32_u8(vqtbl4q_u8(wsat4, idx0));

            x1.x = vreinterpretq_f32_u8(vqtbl4q_u8(rsat4, idx1));
            x1.y = vreinterpretq_f32_u8(vqtbl4q_u8(gsat4, idx1));
            x1.z = vreinterpretq_f32_u8(vqtbl4q_u8(bsat4, idx1));
            w1   = vreinterpretq_f32_u8(vqtbl4q_u8(wsat4, idx1));
        }

#else

        // Scalar path
        x0.x = vzero(); x0.y = vzero(); x0.z = vzero(); w0 = vzero();
        x1.x = vzero(); x1.y = vzero(); x1.z = vzero(); w1 = vzero();

        for (int l = 0; l < VEC_SIZE; l++) {
            int c0 = s_threeCluster[i + l].c0 - 1;
            if (c0 >= 0) {
                lane(x0.x, l) = sat.r[c0];
                lane(x0.y, l) = sat.g[c0];
                lane(x0.z, l) = sat.b[c0];
                lane(w0, l) = sat.w[c0];
            }

            int c1 = s_threeCluster[i + l].c1 - 1;
            if (c1 >= 0) {
                lane(x1.x, l) = sat.r[c1];
                lane(x1.y, l) = sat.g[c1];
                lane(x1.z, l) = sat.b[c1];
                lane(w1, l) = sat.w[c1];
            }
        }

#endif

        VFloat w2 = vbroadcast(w_sum) - w1;
        x1 = x1 - x0;
        w1 = w1 - w0;

        VFloat alphabeta_sum = w1 * vbroadcast(0.25f);
        VFloat alpha2_sum = w0 + alphabeta_sum;
        VFloat beta2_sum = w2 + alphabeta_sum;
        VFloat factor = vrcp(vm2sub(alpha2_sum, beta2_sum, alphabeta_sum, alphabeta_sum));

        VVector3 alphax_sum = x0 + x1 * vbroadcast(0.5f);
        VVector3 betax_sum = vbroadcast(r_sum, g_sum, b_sum) - alphax_sum;

        VVector3 a = vm2sub(alphax_sum, beta2_sum, betax_sum, alphabeta_sum) * factor;
        VVector3 b = vm2sub(betax_sum, alpha2_sum, alphax_sum, alphabeta_sum) * factor;

        // snap to the grid
        a = vround_ept(a);
        b = vround_ept(b);

        // compute the error
        VVector3 e2 = vm2sub(a, vmsub(b, alphabeta_sum, alphax_sum), b, betax_sum) * vbroadcast(2.0f);
        VVector3 e1 = vmadd(a * a, alpha2_sum, vmadd(b * b, beta2_sum, e2));

        // apply the metric to the error term
        VFloat error = vdot(e1, vbroadcast(metric_sqr));


        // keep the solution if it wins
        auto mask = (error < vbesterror);

        // I could mask the unused lanes here, but instead I set the invalid SAT entries to FLT_MAX.
        //mask = (mask & (vbroadcast(total_order_count) >= tid8(i))); // This doesn't seem to help. Is it OK to consider elements out of bounds?

        vbesterror = vselect(mask, vbesterror, error);
        vbeststart = vselect(mask, vbeststart, a);
        vbestend = vselect(mask, vbestend, b);
    }

    int bestindex = reduce_min_index(vbesterror);

    start->x = lane(vbeststart.x, bestindex);
    start->y = lane(vbeststart.y, bestindex);
    start->z = lane(vbeststart.z, bestindex);
    end->x = lane(vbestend.x, bestindex);
    end->y = lane(vbestend.y, bestindex);
    end->z = lane(vbestend.z, bestindex);
}


static void cluster_fit_four(const SummedAreaTable & sat, int count, Vector3 metric_sqr, Vector3 * start, Vector3 * end)
{
    const float r_sum = sat.r[count-1];
    const float g_sum = sat.g[count-1];
    const float b_sum = sat.b[count-1];
    const float w_sum = sat.w[count-1];

    VFloat vbesterror = vbroadcast(FLT_MAX);
    VVector3 vbeststart = { vzero(), vzero(), vzero() };
    VVector3 vbestend = { vzero(), vzero(), vzero() };

    // check all possible clusters for this total order
    const int total_order_count = s_fourClusterTotal[count - 1];

    for (int i = 0; i < total_order_count; i += VEC_SIZE)
    {
        VVector3 x0, x1, x2;
        VFloat w0, w1, w2;

        /*
        // Another approach would be to load and broadcast one color at a time like I do in my old CUDA implementation.
        uint akku = 0;

        // Compute alpha & beta for this permutation.
        #pragma unroll
        for (int i = 0; i < 16; i++)
        {
            const uint bits = permutation >> (2*i);

            alphax_sum += alphaTable4[bits & 3] * colors[i];
            akku += prods4[bits & 3];
        }

        float alpha2_sum = float(akku >> 16);
        float beta2_sum = float((akku >> 8) & 0xff);
        float alphabeta_sum = float(akku & 0xff);
        float3 betax_sum = 9.0f * color_sum - alphax_sum;
        */

#if ICBC_USE_AVX512_PERMUTE

        auto loadmask = lane_id() < vbroadcast(float(count));

        // Load sat in one register:
        VFloat vrsat = vload(loadmask, sat.r, FLT_MAX);
        VFloat vgsat = vload(loadmask, sat.g, FLT_MAX);
        VFloat vbsat = vload(loadmask, sat.b, FLT_MAX);
        VFloat vwsat = vload(loadmask, sat.w, FLT_MAX);

        // Load 4 uint8 per lane.
        VInt packedClusterIndex = vload((int *)&s_fourCluster[i]);

        VInt c0 = (packedClusterIndex & 0xFF) - 1;
        VInt c1 = ((packedClusterIndex >> 8) & 0xFF) - 1;
        VInt c2 = ((packedClusterIndex >> 16)) - 1; // @@ No need for &

        x0.x = vpermuteif(c0 >= 0, vrsat, c0);
        x0.y = vpermuteif(c0 >= 0, vgsat, c0);
        x0.z = vpermuteif(c0 >= 0, vbsat, c0);
        w0   = vpermuteif(c0 >= 0, vwsat, c0);

        x1.x = vpermuteif(c1 >= 0, vrsat, c1);
        x1.y = vpermuteif(c1 >= 0, vgsat, c1);
        x1.z = vpermuteif(c1 >= 0, vbsat, c1);
        w1   = vpermuteif(c1 >= 0, vwsat, c1);

        x2.x = vpermuteif(c2 >= 0, vrsat, c2);
        x2.y = vpermuteif(c2 >= 0, vgsat, c2);
        x2.z = vpermuteif(c2 >= 0, vbsat, c2);
        w2   = vpermuteif(c2 >= 0, vwsat, c2);

#elif ICBC_USE_AVX2_PERMUTE2

        // Load 4 uint8 per lane.
        VInt packedClusterIndex = vload((int *)&s_fourCluster[i]);

        VInt c0 = (packedClusterIndex & 0xFF);
        VInt c1 = ((packedClusterIndex >> 8) & 0xFF);
        VInt c2 = ((packedClusterIndex >> 16)); // @@ No need for &

        if (count <= 8) {
            // Load sat.r in one register:
            VFloat rLo = vload(sat.r);
            VFloat gLo = vload(sat.g);
            VFloat bLo = vload(sat.b);
            VFloat wLo = vload(sat.w);

            x0.x = vpermuteif(c0>0, rLo, c0-1);
            x0.y = vpermuteif(c0>0, gLo, c0-1);
            x0.z = vpermuteif(c0>0, bLo, c0-1);
            w0   = vpermuteif(c0>0, wLo, c0-1);

            x1.x = vpermuteif(c1>0, rLo, c1-1);
            x1.y = vpermuteif(c1>0, gLo, c1-1);
            x1.z = vpermuteif(c1>0, bLo, c1-1);
            w1   = vpermuteif(c1>0, wLo, c1-1);

            x2.x = vpermuteif(c2>0, rLo, c2-1);
            x2.y = vpermuteif(c2>0, gLo, c2-1);
            x2.z = vpermuteif(c2>0, bLo, c2-1);
            w2   = vpermuteif(c2>0, wLo, c2-1);
        }
        else {
            // Load sat.r in two registers:
            VFloat rLo = vload(sat.r); VFloat rHi = vload(sat.r + 8);
            VFloat gLo = vload(sat.g); VFloat gHi = vload(sat.g + 8);
            VFloat bLo = vload(sat.b); VFloat bHi = vload(sat.b + 8);
            VFloat wLo = vload(sat.w); VFloat wHi = vload(sat.w + 8);

            x0.x = vpermute2if(c0>0, rLo, rHi, c0-1);
            x0.y = vpermute2if(c0>0, gLo, gHi, c0-1);
            x0.z = vpermute2if(c0>0, bLo, bHi, c0-1);
            w0   = vpermute2if(c0>0, wLo, wHi, c0-1);

            x1.x = vpermute2if(c1>0, rLo, rHi, c1-1);
            x1.y = vpermute2if(c1>0, gLo, gHi, c1-1);
            x1.z = vpermute2if(c1>0, bLo, bHi, c1-1);
            w1   = vpermute2if(c1>0, wLo, wHi, c1-1);

            x2.x = vpermute2if(c2>0, rLo, rHi, c2-1);
            x2.y = vpermute2if(c2>0, gLo, gHi, c2-1);
            x2.z = vpermute2if(c2>0, bLo, bHi, c2-1);
            w2   = vpermute2if(c2>0, wLo, wHi, c2-1);
        }

#elif ICBC_USE_NEON_VTL

        uint8x16_t idx0 = (uint8x16_t &)s_neon_vtl_index0_4[4*i];
        uint8x16_t idx1 = (uint8x16_t &)s_neon_vtl_index1_4[4*i];
        uint8x16_t idx2 = (uint8x16_t &)s_neon_vtl_index2_4[4*i];

        if (count <= 4) {
            uint8x16_t rsat1 = vld1q_u8((uint8*)sat.r);
            uint8x16_t gsat1 = vld1q_u8((uint8*)sat.g);
            uint8x16_t bsat1 = vld1q_u8((uint8*)sat.b);
            uint8x16_t wsat1 = vld1q_u8((uint8*)sat.w);

            x0.x = vreinterpretq_f32_u8(vqtbl1q_u8(rsat1, idx0));
            x0.y = vreinterpretq_f32_u8(vqtbl1q_u8(gsat1, idx0));
            x0.z = vreinterpretq_f32_u8(vqtbl1q_u8(bsat1, idx0));
            w0   = vreinterpretq_f32_u8(vqtbl1q_u8(wsat1, idx0));

            x1.x = vreinterpretq_f32_u8(vqtbl1q_u8(rsat1, idx1));
            x1.y = vreinterpretq_f32_u8(vqtbl1q_u8(gsat1, idx1));
            x1.z = vreinterpretq_f32_u8(vqtbl1q_u8(bsat1, idx1));
            w1   = vreinterpretq_f32_u8(vqtbl1q_u8(wsat1, idx1));

            x2.x = vreinterpretq_f32_u8(vqtbl1q_u8(rsat1, idx2));
            x2.y = vreinterpretq_f32_u8(vqtbl1q_u8(gsat1, idx2));
            x2.z = vreinterpretq_f32_u8(vqtbl1q_u8(bsat1, idx2));
            w2   = vreinterpretq_f32_u8(vqtbl1q_u8(wsat1, idx2));
        }
        else if (count <= 8) {
            uint8x16x2_t rsat2 = vld2q_u8((uint8*)sat.r);
            uint8x16x2_t gsat2 = vld2q_u8((uint8*)sat.g);
            uint8x16x2_t bsat2 = vld2q_u8((uint8*)sat.b);
            uint8x16x2_t wsat2 = vld2q_u8((uint8*)sat.w);

            x0.x = vreinterpretq_f32_u8(vqtbl2q_u8(rsat2, idx0));
            x0.y = vreinterpretq_f32_u8(vqtbl2q_u8(gsat2, idx0));
            x0.z = vreinterpretq_f32_u8(vqtbl2q_u8(bsat2, idx0));
            w0   = vreinterpretq_f32_u8(vqtbl2q_u8(wsat2, idx0));

            x1.x = vreinterpretq_f32_u8(vqtbl2q_u8(rsat2, idx1));
            x1.y = vreinterpretq_f32_u8(vqtbl2q_u8(gsat2, idx1));
            x1.z = vreinterpretq_f32_u8(vqtbl2q_u8(bsat2, idx1));
            w1   = vreinterpretq_f32_u8(vqtbl2q_u8(wsat2, idx1));

            x2.x = vreinterpretq_f32_u8(vqtbl2q_u8(rsat2, idx2));
            x2.y = vreinterpretq_f32_u8(vqtbl2q_u8(gsat2, idx2));
            x2.z = vreinterpretq_f32_u8(vqtbl2q_u8(bsat2, idx2));
            w2   = vreinterpretq_f32_u8(vqtbl2q_u8(wsat2, idx2));
        }
        else if (count <= 12) {
            uint8x16x3_t rsat3 = vld3q_u8((uint8*)sat.r);
            uint8x16x3_t gsat3 = vld3q_u8((uint8*)sat.g);
            uint8x16x3_t bsat3 = vld3q_u8((uint8*)sat.b);
            uint8x16x3_t wsat3 = vld3q_u8((uint8*)sat.w);

            x0.x = vreinterpretq_f32_u8(vqtbl3q_u8(rsat3, idx0));
            x0.y = vreinterpretq_f32_u8(vqtbl3q_u8(gsat3, idx0));
            x0.z = vreinterpretq_f32_u8(vqtbl3q_u8(bsat3, idx0));
            w0   = vreinterpretq_f32_u8(vqtbl3q_u8(wsat3, idx0));

            x1.x = vreinterpretq_f32_u8(vqtbl3q_u8(rsat3, idx1));
            x1.y = vreinterpretq_f32_u8(vqtbl3q_u8(gsat3, idx1));
            x1.z = vreinterpretq_f32_u8(vqtbl3q_u8(bsat3, idx1));
            w1   = vreinterpretq_f32_u8(vqtbl3q_u8(wsat3, idx1));

            x2.x = vreinterpretq_f32_u8(vqtbl3q_u8(rsat3, idx2));
            x2.y = vreinterpretq_f32_u8(vqtbl3q_u8(gsat3, idx2));
            x2.z = vreinterpretq_f32_u8(vqtbl3q_u8(bsat3, idx2));
            w2   = vreinterpretq_f32_u8(vqtbl3q_u8(wsat3, idx2));
        }
        else {
            uint8x16x4_t rsat4 = vld4q_u8((uint8*)sat.r);
            uint8x16x4_t gsat4 = vld4q_u8((uint8*)sat.g);
            uint8x16x4_t bsat4 = vld4q_u8((uint8*)sat.b);
            uint8x16x4_t wsat4 = vld4q_u8((uint8*)sat.w);

            x0.x = vreinterpretq_f32_u8(vqtbl4q_u8(rsat4, idx0));
            x0.y = vreinterpretq_f32_u8(vqtbl4q_u8(gsat4, idx0));
            x0.z = vreinterpretq_f32_u8(vqtbl4q_u8(bsat4, idx0));
            w0   = vreinterpretq_f32_u8(vqtbl4q_u8(wsat4, idx0));

            x1.x = vreinterpretq_f32_u8(vqtbl4q_u8(rsat4, idx1));
            x1.y = vreinterpretq_f32_u8(vqtbl4q_u8(gsat4, idx1));
            x1.z = vreinterpretq_f32_u8(vqtbl4q_u8(bsat4, idx1));
            w1   = vreinterpretq_f32_u8(vqtbl4q_u8(wsat4, idx1));

            x2.x = vreinterpretq_f32_u8(vqtbl4q_u8(rsat4, idx2));
            x2.y = vreinterpretq_f32_u8(vqtbl4q_u8(gsat4, idx2));
            x2.z = vreinterpretq_f32_u8(vqtbl4q_u8(bsat4, idx2));
            w2   = vreinterpretq_f32_u8(vqtbl4q_u8(wsat4, idx2));
        }

#else

        // Scalar path
        x0.x = vzero(); x0.y = vzero(); x0.z = vzero(); w0 = vzero();
        x1.x = vzero(); x1.y = vzero(); x1.z = vzero(); w1 = vzero();
        x2.x = vzero(); x2.y = vzero(); x2.z = vzero(); w2 = vzero();

        for (int l = 0; l < VEC_SIZE; l++) {
            int c0 = s_fourCluster[i + l].c0 - 1;
            if (c0 >= 0) {
                lane(x0.x, l) = sat.r[c0];
                lane(x0.y, l) = sat.g[c0];
                lane(x0.z, l) = sat.b[c0];
                lane(w0, l) = sat.w[c0];
            }

            int c1 = s_fourCluster[i + l].c1 - 1;
            if (c1 >= 0) {
                lane(x1.x, l) = sat.r[c1];
                lane(x1.y, l) = sat.g[c1];
                lane(x1.z, l) = sat.b[c1];
                lane(w1, l) = sat.w[c1];
            }

            int c2 = s_fourCluster[i + l].c2 - 1;
            if (c2 >= 0) {
                lane(x2.x, l) = sat.r[c2];
                lane(x2.y, l) = sat.g[c2];
                lane(x2.z, l) = sat.b[c2];
                lane(w2, l) = sat.w[c2];
            }
        }

#endif

        VFloat w3 = vbroadcast(w_sum) - w2;
        x2 = x2 - x1;
        x1 = x1 - x0;
        w2 = w2 - w1;
        w1 = w1 - w0;

        VFloat alpha2_sum = vmadd(w2, (1.0f / 9.0f), vmadd(w1, (4.0f / 9.0f), w0));
        VFloat beta2_sum  = vmadd(w1, (1.0f / 9.0f), vmadd(w2, (4.0f / 9.0f), w3));

        VFloat alphabeta_sum = (w1 + w2) * vbroadcast(2.0f / 9.0f);
        VFloat factor = vrcp(vm2sub(alpha2_sum, beta2_sum, alphabeta_sum, alphabeta_sum));

        VVector3 alphax_sum = vmadd(x2, (1.0f / 3.0f), vmadd(x1, (2.0f / 3.0f), x0));
        VVector3 betax_sum = vbroadcast(r_sum, g_sum, b_sum) - alphax_sum;

        VVector3 a = vm2sub(alphax_sum, beta2_sum, betax_sum, alphabeta_sum) * factor;
        VVector3 b = vm2sub(betax_sum, alpha2_sum, alphax_sum, alphabeta_sum) * factor;

        // snap to the grid
        a = vround_ept(a);
        b = vround_ept(b);

        // compute the error
        VVector3 e2 = vm2sub(a, vmsub(b, alphabeta_sum, alphax_sum), b, betax_sum) * vbroadcast(2.0f);
        VVector3 e1 = vmadd(a * a, alpha2_sum, vmadd(b * b, beta2_sum, e2));

        // apply the metric to the error term
        VFloat error = vdot(e1, vbroadcast(metric_sqr));

        // keep the solution if it wins
        auto mask = (error < vbesterror);

        // We could mask the unused lanes here, but instead set the invalid SAT entries to FLT_MAX.
        //mask = (mask & (vbroadcast(total_order_count) >= tid8(i))); // This doesn't seem to help. Is it OK to consider elements out of bounds?

        vbesterror = vselect(mask, vbesterror, error);
        vbeststart = vselect(mask, vbeststart, a);
        vbestend = vselect(mask, vbestend, b);
    }

    int bestindex = reduce_min_index(vbesterror);

    start->x = lane(vbeststart.x, bestindex);
    start->y = lane(vbeststart.y, bestindex);
    start->z = lane(vbeststart.z, bestindex);
    end->x = lane(vbestend.x, bestindex);
    end->y = lane(vbestend.y, bestindex);
    end->z = lane(vbestend.z, bestindex);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Palette evaluation.

Decoder s_decoder = Decoder_D3D10;

// D3D10
inline void evaluate_palette4_d3d10(Color32 palette[4]) {
    palette[2].r = (2 * palette[0].r + palette[1].r) / 3;
    palette[2].g = (2 * palette[0].g + palette[1].g) / 3;
    palette[2].b = (2 * palette[0].b + palette[1].b) / 3;
    palette[2].a = 0xFF;

    palette[3].r = (2 * palette[1].r + palette[0].r) / 3;
    palette[3].g = (2 * palette[1].g + palette[0].g) / 3;
    palette[3].b = (2 * palette[1].b + palette[0].b) / 3;
    palette[3].a = 0xFF;
}
inline void evaluate_palette3_d3d10(Color32 palette[4]) {
    palette[2].r = (palette[0].r + palette[1].r) / 2;
    palette[2].g = (palette[0].g + palette[1].g) / 2;
    palette[2].b = (palette[0].b + palette[1].b) / 2;
    palette[2].a = 0xFF;
    palette[3].u = 0;
}
static void evaluate_palette_d3d10(Color16 c0, Color16 c1, Color32 palette[4]) {
    palette[0] = bitexpand_color16_to_color32(c0);
    palette[1] = bitexpand_color16_to_color32(c1);
    if (c0.u > c1.u) {
        evaluate_palette4_d3d10(palette);
    }
    else {
        evaluate_palette3_d3d10(palette);
    }
}

// NV
inline void evaluate_palette4_nv(Color16 c0, Color16 c1, Color32 palette[4]) {
    int gdiff = palette[1].g - palette[0].g;
    palette[2].r = uint8(((2 * c0.r + c1.r) * 22) / 8);
    palette[2].g = uint8((256 * palette[0].g + gdiff / 4 + 128 + gdiff * 80) / 256);
    palette[2].b = uint8(((2 * c0.b + c1.b) * 22) / 8);
    palette[2].a = 0xFF;

    palette[3].r = uint8(((2 * c1.r + c0.r) * 22) / 8);
    palette[3].g = uint8((256 * palette[1].g - gdiff / 4 + 128 - gdiff * 80) / 256);
    palette[3].b = uint8(((2 * c1.b + c0.b) * 22) / 8);
    palette[3].a = 0xFF;
}
inline void evaluate_palette3_nv(Color16 c0, Color16 c1, Color32 palette[4]) {
    int gdiff = palette[1].g - palette[0].g;
    palette[2].r = uint8(((c0.r + c1.r) * 33) / 8);
    palette[2].g = uint8((256 * palette[0].g + gdiff / 4 + 128 + gdiff * 128) / 256);
    palette[2].b = uint8(((c0.b + c1.b) * 33) / 8);
    palette[2].a = 0xFF;
    palette[3].u = 0;
}
static void evaluate_palette_nv(Color16 c0, Color16 c1, Color32 palette[4]) {
    palette[0] = bitexpand_color16_to_color32(c0);
    palette[1] = bitexpand_color16_to_color32(c1);

    if (c0.u > c1.u) {
        evaluate_palette4_nv(c0, c1, palette);
    }
    else {
        evaluate_palette3_nv(c0, c1, palette);
    }
}

// AMD
inline void evaluate_palette4_amd(Color32 palette[4]) {
    palette[2].r = uint8((43 * palette[0].r + 21 * palette[1].r + 32) >> 6);
    palette[2].g = uint8((43 * palette[0].g + 21 * palette[1].g + 32) >> 6);
    palette[2].b = uint8((43 * palette[0].b + 21 * palette[1].b + 32) >> 6);
    palette[2].a = 0xFF;

    palette[3].r = uint8((43 * palette[1].r + 21 * palette[0].r + 32) >> 6);
    palette[3].g = uint8((43 * palette[1].g + 21 * palette[0].g + 32) >> 6);
    palette[3].b = uint8((43 * palette[1].b + 21 * palette[0].b + 32) >> 6);
    palette[3].a = 0xFF;
}
inline void evaluate_palette3_amd(Color32 palette[4]) {
    palette[2].r = uint8((palette[0].r + palette[1].r + 1) / 2);
    palette[2].g = uint8((palette[0].g + palette[1].g + 1) / 2);
    palette[2].b = uint8((palette[0].b + palette[1].b + 1) / 2);
    palette[2].a = 0xFF;
    palette[3].u = 0;
}
static void evaluate_palette_amd(Color16 c0, Color16 c1, Color32 palette[4]) {
    palette[0] = bitexpand_color16_to_color32(c0);
    palette[1] = bitexpand_color16_to_color32(c1);

    if (c0.u > c1.u) {
        evaluate_palette4_amd(palette);
    }
    else {
        evaluate_palette3_amd(palette);
    }
}

inline void evaluate_palette(Color16 c0, Color16 c1, Color32 palette[4], Decoder decoder = s_decoder) {
    if (decoder == Decoder_D3D10)         evaluate_palette_d3d10(c0, c1, palette);
    else if (decoder == Decoder_NVIDIA)   evaluate_palette_nv(c0, c1, palette);
    else if (decoder == Decoder_AMD)      evaluate_palette_amd(c0, c1, palette);
}

static void evaluate_palette(Color16 c0, Color16 c1, Vector3 palette[4]) {
    Color32 palette32[4];
    evaluate_palette(c0, c1, palette32);

    for (int i = 0; i < 4; i++) {
        palette[i] = color_to_vector3(palette32[i]);
    }
}

static void decode_dxt1(const BlockDXT1 * block, unsigned char rgba_block[16 * 4], Decoder decoder)
{
    Color32 palette[4];
    evaluate_palette(block->col0, block->col1, palette, decoder);

    for (int i = 0; i < 16; i++) {
        int index = (block->indices >> (2 * i)) & 3;
        Color32 c = palette[index];
        rgba_block[4 * i + 0] = c.r;
        rgba_block[4 * i + 1] = c.g;
        rgba_block[4 * i + 2] = c.b;
        rgba_block[4 * i + 3] = c.a;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Error evaluation.

// Different ways of estimating the error.

static float evaluate_mse(const Vector3 & p, const Vector3 & c, const Vector3 & w) {
    Vector3 d = (p - c) * w * 255;
    return dot(d, d);
}

static float evaluate_mse(const Color32 & p, const Vector3 & c, const Vector3 & w) {
    Vector3 d = (color_to_vector3(p) - c) * w * 255;
    return dot(d, d);
}

static int evaluate_mse(const Color32 & p, const Color32 & c) {
    return (square(int(p.r)-c.r) + square(int(p.g)-c.g) + square(int(p.b)-c.b));
}

static int evaluate_mse(const Color32 palette[4], const Color32 & c) {
    int e0 = evaluate_mse(palette[0], c);
    int e1 = evaluate_mse(palette[1], c);
    int e2 = evaluate_mse(palette[2], c);
    int e3 = evaluate_mse(palette[3], c);
    return min(min(e0, e1), min(e2, e3));
}

// Returns MSE error in [0-255] range.
static int evaluate_mse(const BlockDXT1 * output, Color32 color, int index) {
    Color32 palette[4];
    evaluate_palette(output->col0, output->col1, palette);

    return evaluate_mse(palette[index], color);
}

// Returns weighted MSE error in [0-255] range.
static float evaluate_palette_error(Color32 palette[4], const Color32 * colors, const float * weights, int count) {

    float total = 0.0f;
    for (int i = 0; i < count; i++) {
        total += weights[i] * evaluate_mse(palette, colors[i]);
    }

    return total;
}

static float evaluate_palette_error(Color32 palette[4], const Color32 * colors, int count) {

    float total = 0.0f;
    for (int i = 0; i < count; i++) {
        total += evaluate_mse(palette, colors[i]);
    }

    return total;
}

static float evaluate_mse(const Vector4 input_colors[16], const float input_weights[16], const Vector3 & color_weights, const BlockDXT1 * output) {
    Color32 palette[4];
    evaluate_palette(output->col0, output->col1, palette);

    // evaluate error for each index.
    float error = 0.0f;
    for (int i = 0; i < 16; i++) {
        int index = (output->indices >> (2 * i)) & 3;
        error += input_weights[i] * evaluate_mse(palette[index], input_colors[i].xyz, color_weights);
    }
    return error;
}

static float evaluate_mse(const Vector4 input_colors[16], const float input_weights[16], const Vector3& color_weights, Vector3 palette[4], uint32 indices) {

    // evaluate error for each index.
    float error = 0.0f;
    for (int i = 0; i < 16; i++) {
        int index = (indices >> (2 * i)) & 3;
        error += input_weights[i] * evaluate_mse(palette[index], input_colors[i].xyz, color_weights);
    }
    return error;
}

float evaluate_dxt1_error(const uint8 rgba_block[16*4], const BlockDXT1 * block, Decoder decoder) {
    Color32 palette[4];
    evaluate_palette(block->col0, block->col1, palette, decoder);

    // evaluate error for each index.
    float error = 0.0f;
    for (int i = 0; i < 16; i++) {
        int index = (block->indices >> (2 * i)) & 3;
        Color32 c;
        c.r = rgba_block[4 * i + 0];
        c.g = rgba_block[4 * i + 1];
        c.b = rgba_block[4 * i + 2];
        c.a = 255;
        error += evaluate_mse(palette[index], c);
    }
    return error;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Index selection

// @@ Can we interleave the two uint16 at once?
inline uint32 interleave_uint16_with_zeros(uint32 input)  {
    uint32 word = input;
    word = (word ^ (word << 8 )) & 0x00ff00ff;
    word = (word ^ (word << 4 )) & 0x0f0f0f0f;
    word = (word ^ (word << 2 )) & 0x33333333;
    word = (word ^ (word << 1 )) & 0x55555555;
    return word;
}

// Interleave the bits. https://lemire.me/blog/2018/01/08/how-fast-can-you-bit-interleave-32-bit-integers/
ICBC_FORCEINLINE uint32 interleave(uint32 a, uint32 b) {
#if ICBC_BMI2
    return _pdep_u32(a, 0x55555555) | _pdep_u32(b, 0xaaaaaaaa);
#else
    return interleave_uint16_with_zeros(a) | (interleave_uint16_with_zeros(b) << 1);
#endif
}

static uint compute_indices4(const Vector4 input_colors[16], const Vector3 & color_weights, const Vector3 palette[4]) {
    uint indices0 = 0;
    uint indices1 = 0;

    VVector3 vw = vbroadcast(color_weights);
    VVector3 vp0 = vbroadcast(palette[0]) * vw;
    VVector3 vp1 = vbroadcast(palette[1]) * vw;
    VVector3 vp2 = vbroadcast(palette[2]) * vw;
    VVector3 vp3 = vbroadcast(palette[3]) * vw;

    for (int i = 0; i < 16; i += VEC_SIZE) {
        VVector3 vc = vload(&input_colors[i]) * vw;

        VFloat d0 = vlen2(vc - vp0);
        VFloat d1 = vlen2(vc - vp1);
        VFloat d2 = vlen2(vc - vp2);
        VFloat d3 = vlen2(vc - vp3);

        VMask b1 = d1 > d2;
        VMask b2 = d0 > d2;
        VMask x0 = b1 & b2;

        VMask b0 = d0 > d3;
        VMask b3 = d1 > d3;
        x0 = x0 | (b0 & b3);

        VMask b4 = d2 > d3;
        VMask x1 = b0 & b4;

        indices0 |= mask(x0) << i;
        indices1 |= mask(x1) << i;
    }

    return interleave(indices1, indices0);
}

static uint compute_indices3(const Vector4 input_colors[16], const Vector3 & color_weights, bool allow_transparent_black, const Vector3 palette[4]) {
    uint indices0 = 0;
    uint indices1 = 0;

    VVector3 vw = vbroadcast(color_weights);
    VVector3 vp0 = vbroadcast(palette[0]) * vw;
    VVector3 vp1 = vbroadcast(palette[1]) * vw;
    VVector3 vp2 = vbroadcast(palette[2]) * vw;

    if (allow_transparent_black) {
        for (int i = 0; i < 16; i += VEC_SIZE) {
            VVector3 vc = vload(&input_colors[i]) * vw;

            VFloat d0 = vlen2(vp0 - vc);
            VFloat d1 = vlen2(vp1 - vc);
            VFloat d2 = vlen2(vp2 - vc);
            VFloat d3 = vdot(vc, vc);

            VMask i1 = (d1 < d2);
            VMask i2 = (d2 <= d0) & (d2 <= d1);
            VMask i3 = (d3 <= d0) & (d3 <= d1) & (d3 <= d2);

            indices0 |= mask(i2 | i3) << i;
            indices1 |= mask(i1 | i3) << i;
        }
    }
    else {
        for (int i = 0; i < 16; i += VEC_SIZE) {
            VVector3 vc = vload(&input_colors[i]) * vw;

            VFloat d0 = vlen2(vc - vp0);
            VFloat d1 = vlen2(vc - vp1);
            VFloat d2 = vlen2(vc - vp2);

            VMask i1 = (d1 < d2);
            VMask i2 = (d2 <= d0) & (d2 <= d1);

            indices0 |= mask(i2) << i;
            indices1 |= mask(i1) << i;
        }
    }

    return interleave(indices1, indices0);
}


static uint compute_indices(const Vector4 input_colors[16], const Vector3 & color_weights, const Vector3 palette[4]) {
#if 0
    Vector3 p0 = palette[0] * color_weights;
    Vector3 p1 = palette[1] * color_weights;
    Vector3 p2 = palette[2] * color_weights;
    Vector3 p3 = palette[3] * color_weights;

    uint indices = 0;
    for (int i = 0; i < 16; i++) {
        Vector3 ci = input_colors[i].xyz * color_weights;
        float d0 = lengthSquared(p0 - ci);
        float d1 = lengthSquared(p1 - ci);
        float d2 = lengthSquared(p2 - ci);
        float d3 = lengthSquared(p3 - ci);

        uint index;
        if (d0 < d1 && d0 < d2 && d0 < d3) index = 0;
        else if (d1 < d2 && d1 < d3) index = 1;
        else if (d2 < d3) index = 2;
        else index = 3;

        indices |= index << (2 * i);
    }

    return indices;
#else
    uint indices0 = 0;
    uint indices1 = 0;

    VVector3 vw = vbroadcast(color_weights);
    VVector3 vp0 = vbroadcast(palette[0]) * vw;
    VVector3 vp1 = vbroadcast(palette[1]) * vw;
    VVector3 vp2 = vbroadcast(palette[2]) * vw;
    VVector3 vp3 = vbroadcast(palette[3]) * vw;

    for (int i = 0; i < 16; i += VEC_SIZE) {
        VVector3 vc = vload(&input_colors[i]) * vw;

        VFloat d0 = vlen2(vc - vp0);
        VFloat d1 = vlen2(vc - vp1);
        VFloat d2 = vlen2(vc - vp2);
        VFloat d3 = vlen2(vc - vp3);

        //VMask i0 = (d0 < d1) & (d0 < d2) & (d0 < d3);
        VMask i1 = (d1 <= d0) & (d1 < d2) & (d1 < d3);
        VMask i2 = (d2 <= d0) & (d2 <= d1) & (d2 < d3);
        VMask i3 = (d3 <= d0) & (d3 <= d1) & (d3 <= d2);
        //VFloat vindex = vselect(i0, vselect(i1, vselect(i2, vbroadcast(3), vbroadcast(2)), vbroadcast(1)), vbroadcast(0));

        indices0 |= mask(i2 | i3) << i;
        indices1 |= mask(i1 | i3) << i;
    }

    uint indices = interleave(indices1, indices0);
    return indices;
#endif
}


static float output_block3(const Vector4 input_colors[16], const float input_weights[16], const Vector3 & color_weights, bool allow_transparent_black, const Vector3 & v0, const Vector3 & v1, BlockDXT1 * block)
{
    Color16 color0 = vector3_to_color16(v0);
    Color16 color1 = vector3_to_color16(v1);

    if (color0.u > color1.u) {
        swap(color0, color1);
    }

    Vector3 palette[4];
    evaluate_palette(color0, color1, palette);

    block->col0 = color0;
    block->col1 = color1;
    block->indices = compute_indices3(input_colors, color_weights, allow_transparent_black, palette);

    return evaluate_mse(input_colors, input_weights, color_weights, palette, block->indices);
}

static float output_block4(const Vector4 input_colors[16], const float input_weights[16], const Vector3 & color_weights, const Vector3 & v0, const Vector3 & v1, BlockDXT1 * block)
{
    Color16 color0 = vector3_to_color16(v0);
    Color16 color1 = vector3_to_color16(v1);

    if (color0.u < color1.u) {
        swap(color0, color1);
    }

    Vector3 palette[4];
    evaluate_palette(color0, color1, palette);

    block->col0 = color0;
    block->col1 = color1;
    block->indices = compute_indices4(input_colors, color_weights, palette);

    return evaluate_mse(input_colors, input_weights, color_weights, palette, block->indices);
}


// Least squares fitting of color end points for the given indices. @@ Take weights into account.
static bool optimize_end_points4(uint indices, const Vector4 * colors, /*const float * weights,*/ int count, Vector3 * a, Vector3 * b)
{
    float alpha2_sum = 0.0f;
    float beta2_sum = 0.0f;
    float alphabeta_sum = 0.0f;
    Vector3 alphax_sum = { 0,0,0 };
    Vector3 betax_sum = { 0,0,0 };

    for (int i = 0; i < count; i++)
    {
        const uint bits = indices >> (2 * i);

        float beta = float(bits & 1);
        if (bits & 2) beta = (1 + beta) / 3.0f;
        float alpha = 1.0f - beta;

        alpha2_sum += alpha * alpha;
        beta2_sum += beta * beta;
        alphabeta_sum += alpha * beta;
        alphax_sum += alpha * colors[i].xyz;
        betax_sum += beta * colors[i].xyz;
    }

    float denom = alpha2_sum * beta2_sum - alphabeta_sum * alphabeta_sum;
    if (equal(denom, 0.0f)) return false;

    float factor = 1.0f / denom;

    *a = saturate((alphax_sum * beta2_sum - betax_sum * alphabeta_sum) * factor);
    *b = saturate((betax_sum * alpha2_sum - alphax_sum * alphabeta_sum) * factor);

    return true;
}

// Least squares optimization with custom factors.
// This allows us passing the standard [1, 0, 2/3 1/3] weights by default, but also use alternative mappings when the number of clusters is not 4.
static bool optimize_end_points4(uint indices, const Vector3 * colors, int count, float factors[4], Vector3 * a, Vector3 * b)
{
    float alpha2_sum = 0.0f;
    float beta2_sum = 0.0f;
    float alphabeta_sum = 0.0f;
    Vector3 alphax_sum = { 0,0,0 };
    Vector3 betax_sum = { 0,0,0 };

    for (int i = 0; i < count; i++)
    {
        const uint idx = (indices >> (2 * i)) & 3;
        float alpha = factors[idx];
        float beta = 1 - alpha;

        alpha2_sum += alpha * alpha;
        beta2_sum += beta * beta;
        alphabeta_sum += alpha * beta;
        alphax_sum += alpha * colors[i];
        betax_sum += beta * colors[i];
    }

    float denom = alpha2_sum * beta2_sum - alphabeta_sum * alphabeta_sum;
    if (equal(denom, 0.0f)) return false;

    float factor = 1.0f / denom;

    *a = saturate((alphax_sum * beta2_sum - betax_sum * alphabeta_sum) * factor);
    *b = saturate((betax_sum * alpha2_sum - alphax_sum * alphabeta_sum) * factor);

    return true;
}

static bool optimize_end_points4(uint indices, const Vector3 * colors, int count, Vector3 * a, Vector3 * b)
{
    float factors[4] = { 1, 0, 2.f / 3, 1.f / 3 };
    return optimize_end_points4(indices, colors, count, factors, a, b);
}


// Least squares fitting of color end points for the given indices. @@ This does not support black/transparent index. @@ Take weights into account.
static bool optimize_end_points3(uint indices, const Vector3 * colors, /*const float * weights,*/ int count, Vector3 * a, Vector3 * b)
{
    float alpha2_sum = 0.0f;
    float beta2_sum = 0.0f;
    float alphabeta_sum = 0.0f;
    Vector3 alphax_sum = { 0,0,0 };
    Vector3 betax_sum = { 0,0,0 };

    for (int i = 0; i < count; i++)
    {
        const uint bits = indices >> (2 * i);

        float beta = float(bits & 1);
        if (bits & 2) beta = 0.5f;
        float alpha = 1.0f - beta;

        alpha2_sum += alpha * alpha;
        beta2_sum += beta * beta;
        alphabeta_sum += alpha * beta;
        alphax_sum += alpha * colors[i];
        betax_sum += beta * colors[i];
    }

    float denom = alpha2_sum * beta2_sum - alphabeta_sum * alphabeta_sum;
    if (equal(denom, 0.0f)) return false;

    float factor = 1.0f / denom;

    *a = saturate((alphax_sum * beta2_sum - betax_sum * alphabeta_sum) * factor);
    *b = saturate((betax_sum * alpha2_sum - alphax_sum * alphabeta_sum) * factor);

    return true;
}


// find minimum and maximum colors based on bounding box in color space
inline static void fit_colors_bbox(const Vector3 * colors, int count, Vector3 * __restrict c0, Vector3 * __restrict c1)
{
    *c0 = { 0,0,0 };
    *c1 = { 1,1,1 };

    for (int i = 0; i < count; i++) {
        *c0 = max(*c0, colors[i]);
        *c1 = min(*c1, colors[i]);
    }
}

inline static void select_diagonal(const Vector3 * colors, int count, Vector3 * __restrict c0, Vector3 * __restrict c1)
{
    Vector3 center = (*c0 + *c1) * 0.5f;

    /*Vector3 center = colors[0];
    for (int i = 1; i < count; i++) {
        center = center * float(i-1) / i + colors[i] / i;
    }*/
    /*Vector3 center = colors[0];
    for (int i = 1; i < count; i++) {
        center += colors[i];
    }
    center /= count;*/

    float cov_xz = 0.0f;
    float cov_yz = 0.0f;
    for (int i = 0; i < count; i++) {
        Vector3 t = colors[i] - center;
        cov_xz += t.x * t.z;
        cov_yz += t.y * t.z;
    }

    float x0 = c0->x;
    float y0 = c0->y;
    float x1 = c1->x;
    float y1 = c1->y;

    if (cov_xz < 0) {
        swap(x0, x1);
    }
    if (cov_yz < 0) {
        swap(y0, y1);
    }

    *c0 = { x0, y0, c0->z };
    *c1 = { x1, y1, c1->z };
}

inline static void inset_bbox(Vector3 * __restrict c0, Vector3 * __restrict c1)
{
    float bias = (8.0f / 255.0f) / 16.0f;
    Vector3 inset = (*c0 - *c1) / 16.0f - scalar_to_vector3(bias);
    *c0 = saturate(*c0 - inset);
    *c1 = saturate(*c1 + inset);
}



// Single color lookup tables from:
// https://github.com/nothings/stb/blob/master/stb_dxt.h
static uint8 s_match5[256][2];
static uint8 s_match6[256][2];

static inline int Lerp13(int a, int b)
{
    // replace "/ 3" by "* 0xaaab) >> 17" if your compiler sucks or you really need every ounce of speed.
    return (a * 2 + b) / 3;
}

static void PrepareOptTable5(uint8 * table, Decoder decoder)
{
    uint8 expand[32];
    for (int i = 0; i < 32; i++) expand[i] = uint8((i << 3) | (i >> 2));

    for (int i = 0; i < 256; i++) {
        int bestErr = 256 * 100;

        for (int mn = 0; mn < 32; mn++) {
            for (int mx = 0; mx < 32; mx++) {
                int mine = expand[mn];
                int maxe = expand[mx];

                int err;

                int amd_r = (43 * maxe + 21 * mine + 32) >> 6;
                int amd_err = abs(amd_r - i);

                int nv_r = ((2 * mx + mn) * 22) / 8;
                int nv_err = abs(nv_r - i);

                if (decoder == Decoder_D3D10) {
                    // DX10 spec says that interpolation must be within 3% of "correct" result,
                    // add this as error term. (normally we'd expect a random distribution of
                    // +-1.5% error, but nowhere in the spec does it say that the error has to be
                    // unbiased - better safe than sorry).
                    int r = (maxe * 2 + mine) / 3;
                    err = abs(r - i) * 100 + abs(mx - mn) * 3;

                    // Another approach is to consider the worst of AMD and NVIDIA errors.
                    err = max(amd_err, nv_err);                    
                }
                else if (decoder == Decoder_NVIDIA) {
                    err = nv_err;
                }
                else /*if (decoder == Decoder_AMD)*/ {
                    err = amd_err;
                }

                if (err < bestErr) {
                    bestErr = err;
                    table[i * 2 + 0] = uint8(mx);
                    table[i * 2 + 1] = uint8(mn);
                }
            }
        }
    }
}

static void PrepareOptTable6(uint8 * table, Decoder decoder)
{
    uint8 expand[64];
    for (int i = 0; i < 64; i++) expand[i] = uint8((i << 2) | (i >> 4));

    for (int i = 0; i < 256; i++) {
        int bestErr = 256 * 100;

        for (int mn = 0; mn < 64; mn++) {
            for (int mx = 0; mx < 64; mx++) {
                int mine = expand[mn];
                int maxe = expand[mx];

                int err;

                int amd_g = (43 * maxe + 21 * mine + 32) >> 6;
                int amd_err = abs(amd_g - i);

                int nv_g = (256 * mine + (maxe - mine) / 4 + 128 + (maxe - mine) * 80) / 256;
                int nv_err = abs(nv_g - i);

                if (decoder == Decoder_D3D10) {
                    // DX10 spec says that interpolation must be within 3% of "correct" result,
                    // add this as error term. (normally we'd expect a random distribution of
                    // +-1.5% error, but nowhere in the spec does it say that the error has to be
                    // unbiased - better safe than sorry).
                    int g = (maxe * 2 + mine) / 3;
                    err = abs(g - i) * 100 + abs(mx - mn) * 3;

                    // Another approach is to consider the worst of AMD and NVIDIA errors.
                    err = max(amd_err, nv_err);
                }
                else if (decoder == Decoder_NVIDIA) {
                    err = nv_err;
                }
                else /*if (decoder == Decoder_AMD)*/ {
                    err = amd_err;
                }

                if (err < bestErr) {
                    bestErr = err;
                    table[i * 2 + 0] = uint8(mx);
                    table[i * 2 + 1] = uint8(mn);
                }
            }
        }
    }
}


static void init_single_color_tables(Decoder decoder)
{
    // Prepare single color lookup tables.
    PrepareOptTable5(&s_match5[0][0], decoder);
    PrepareOptTable6(&s_match6[0][0], decoder);
}

// Single color compressor, based on:
// https://mollyrocket.com/forums/viewtopic.php?t=392
static void compress_dxt1_single_color_optimal(Color32 c, BlockDXT1 * output)
{
    output->col0.r = s_match5[c.r][0];
    output->col0.g = s_match6[c.g][0];
    output->col0.b = s_match5[c.b][0];
    output->col1.r = s_match5[c.r][1];
    output->col1.g = s_match6[c.g][1];
    output->col1.b = s_match5[c.b][1];
    output->indices = 0xaaaaaaaa;

    if (output->col0.u < output->col1.u)
    {
        swap(output->col0.u, output->col1.u);
        output->indices ^= 0x55555555;
    }
}


static float compress_dxt1_cluster_fit(const Vector4 input_colors[16], const float input_weights[16], const Vector3 * colors, const float * weights, int count, const Vector3 & color_weights, bool three_color_mode, bool try_transparent_black, bool allow_transparent_black, BlockDXT1 * output)
{
    Vector3 metric_sqr = color_weights * color_weights;

    SummedAreaTable sat;
    int sat_count = compute_sat(colors, weights, count, &sat);

    Vector3 start, end;
    cluster_fit_four(sat, sat_count, metric_sqr, &start, &end);

    float best_error = output_block4(input_colors, input_weights, color_weights, start, end, output);

    if (three_color_mode) {
        if (try_transparent_black) {
            Vector3 tmp_colors[16];
            float tmp_weights[16];
            int tmp_count = skip_blacks(colors, weights, count, tmp_colors, tmp_weights);
            if (!tmp_count) return best_error;

            sat_count = compute_sat(tmp_colors, tmp_weights, tmp_count, &sat);
        }

        cluster_fit_three(sat, sat_count, metric_sqr, &start, &end);

        BlockDXT1 three_color_block;
        float three_color_error = output_block3(input_colors, input_weights, color_weights, allow_transparent_black, start, end, &three_color_block);

        if (three_color_error < best_error) {
            best_error = three_color_error;
            *output = three_color_block;
        }
    }

    return best_error;
}


static float refine_endpoints(const Vector4 input_colors[16], const float input_weights[16], const Vector3 & color_weights, bool three_color_mode, float input_error, BlockDXT1 * output) {
    // TODO:
    // - Optimize palette evaluation when updating only one channel.
    // - try all diagonals.

    // Things that don't help:
    // - Alternate endpoint updates.
    // - Randomize order.
    // - If one direction does not improve, test opposite direction next.

    static const int8 deltas[16][3] = {
        {1,0,0},
        {0,1,0},
        {0,0,1},

        {-1,0,0},
        {0,-1,0},
        {0,0,-1},

        {1,1,0},
        {1,0,1},
        {0,1,1},

        {-1,-1,0},
        {-1,0,-1},
        {0,-1,-1},

        {-1,1,0},
        //{-1,0,1},

        {1,-1,0},
        {0,-1,1},

        //{1,0,-1},
        {0,1,-1},
    };

    float best_error = input_error;

    int lastImprovement = 0;
    for (int i = 0; i < 256; i++) {
        BlockDXT1 refined = *output;
        int8 delta[3] = { deltas[i % 16][0], deltas[i % 16][1], deltas[i % 16][2] };

        if ((i / 16) & 1) {
            refined.col0.r += delta[0];
            refined.col0.g += delta[1];
            refined.col0.b += delta[2];
        }
        else {
            refined.col1.r += delta[0];
            refined.col1.g += delta[1];
            refined.col1.b += delta[2];
        }

        if (!three_color_mode) {
            if (refined.col0.u == refined.col1.u) refined.col1.g += 1;
            if (refined.col0.u < refined.col1.u) swap(refined.col0.u, refined.col1.u);
        }

        Vector3 palette[4];
        evaluate_palette(output->col0, output->col1, palette);

        refined.indices = compute_indices(input_colors, color_weights, palette);

        float refined_error = evaluate_mse(input_colors, input_weights, color_weights, &refined);
        if (refined_error < best_error) {
            best_error = refined_error;
            *output = refined;
            lastImprovement = i;
        }

        // Early out if the last 32 steps didn't improve error.
        if (i - lastImprovement > 32) break;
    }

    return best_error;
}

struct Options {
    float threshold = 0.0f;
    bool box_fit = false;
    bool least_squares_fit = false;
    bool cluster_fit = false;
    bool cluster_fit_3 = false;
    bool cluster_fit_3_black_only = false;
    bool endpoint_refinement = false;
};

static Options setup_options(Quality level, bool enable_three_color_mode, bool enable_transparent_black) {
    Options opt;

    switch (level) {
        case Quality_Level1:            // Box fit + least squares fit.
            opt.box_fit = true;
            opt.least_squares_fit = true;
            opt.threshold = 1.0f / 256;
            break;

        case Quality_Level2:            // Cluster fit 4, threshold = 24.
            opt.box_fit = true;
            opt.least_squares_fit = true;
            opt.cluster_fit = true;
            opt.cluster_fit_3_black_only = enable_three_color_mode && enable_transparent_black;
            opt.threshold = 1.0f / 24;
            break;

        case Quality_Level3:            // Cluster fit 4, threshold = 32.
            opt.box_fit = true;
            opt.cluster_fit = true;
            opt.cluster_fit_3_black_only = enable_three_color_mode && enable_transparent_black;
            opt.threshold = 1.0f / 32;
            break;

        case Quality_Level4:            // Cluster fit 3+4, threshold = 48.
            opt.cluster_fit = true;
            opt.cluster_fit_3_black_only = enable_three_color_mode && enable_transparent_black;
            opt.threshold = 1.0f / 48;
            break;

        case Quality_Level5:            // Cluster fit 3+4, threshold = 64.
            opt.cluster_fit = true;
            opt.cluster_fit_3_black_only = enable_three_color_mode && enable_transparent_black;
            opt.threshold = 1.0f / 64;
            break;

        case Quality_Level6:            // Cluster fit 3+4, threshold = 96.
            opt.cluster_fit = true;
            opt.cluster_fit_3_black_only = enable_three_color_mode && enable_transparent_black;
            opt.threshold = 1.0f / 96;
            break;

        case Quality_Level7:            // Cluster fit 3+4, threshold = 128.
            opt.cluster_fit = true;
            opt.cluster_fit_3_black_only = enable_three_color_mode && enable_transparent_black;
            opt.threshold = 1.0f / 128;
            break;

        case Quality_Level8:            // Cluster fit 3+4, threshold = 256.
            opt.cluster_fit = true;
            opt.cluster_fit_3 = enable_three_color_mode;
            opt.threshold = 1.0f / 256;
            break;

        case Quality_Level9:           // Cluster fit 3+4, threshold = 256 + Refinement.
            opt.cluster_fit = true;
            opt.cluster_fit_3 = enable_three_color_mode;
            opt.threshold = 1.0f / 256;
            opt.endpoint_refinement = true;
            break;
    }

    return opt;
}


static float compress_dxt1(Quality level, const Vector4 input_colors[16], const float input_weights[16], const Vector3 & color_weights, bool three_color_mode, bool three_color_black, BlockDXT1 * output)
{
    Options opt = setup_options(level, three_color_mode, three_color_black);

    Vector3 colors[16];
    float weights[16];
    bool any_black = false;
    int count;
    if (opt.cluster_fit) {
        count = reduce_colors(input_colors, input_weights, 16, opt.threshold, colors, weights, &any_black);
    }
    else {
        for (int i = 0; i < 16; i++) {
            colors[i] = input_colors[i].xyz;
        }
        count = 16;
    }

    if (count == 0) {
        // Output trivial block.
        output->col0.u = 0;
        output->col1.u = 0;
        output->indices = 0;
        return 0;
    }

    // Cluster fit cannot handle single color blocks, so encode them optimally.
    if (count == 1) {
        compress_dxt1_single_color_optimal(vector3_to_color32(colors[0]), output);
        return evaluate_mse(input_colors, input_weights, color_weights, output);
    }

    float error = FLT_MAX;

    // Quick end point selection.
    if (opt.box_fit) {
        Vector3 c0, c1;
        fit_colors_bbox(colors, count, &c0, &c1);
        inset_bbox(&c0, &c1);
        select_diagonal(colors, count, &c0, &c1);
        error = output_block4(input_colors, input_weights, color_weights, c0, c1, output);

        // Refine color for the selected indices.
        if (opt.least_squares_fit && optimize_end_points4(output->indices, input_colors, 16, &c0, &c1)) {
            BlockDXT1 optimized_block;
            float optimized_error = output_block4(input_colors, input_weights, color_weights, c0, c1, &optimized_block);

            if (optimized_error < error) {
                error = optimized_error;
                *output = optimized_block;
            }
        }
    }

    if (opt.cluster_fit) {
        // @@ Use current endpoints as input for initial PCA approximation?

        bool use_three_color_black = any_black && three_color_black;
        bool use_three_color_mode = opt.cluster_fit_3 || (use_three_color_black && opt.cluster_fit_3_black_only);

        // Try cluster fit.
        BlockDXT1 cluster_fit_output;
        float cluster_fit_error = compress_dxt1_cluster_fit(input_colors, input_weights, colors, weights, count, color_weights, use_three_color_mode, use_three_color_black, three_color_black, &cluster_fit_output);
        if (cluster_fit_error < error) {
            *output = cluster_fit_output;
            error = cluster_fit_error;
        }
    }

    if (opt.endpoint_refinement) {
        error = refine_endpoints(input_colors, input_weights, color_weights, three_color_mode, error, output);
    }

    return error;
}


// Public API

void init_dxt1(Decoder decoder) {
    s_decoder = decoder;
    init_single_color_tables(decoder);
    init_cluster_tables();
}

void decode_dxt1(const void * block, unsigned char rgba_block[16 * 4], Decoder decoder/*=Decoder_D3D10*/) {
    decode_dxt1((const BlockDXT1 *)block, rgba_block, decoder);
}

float evaluate_dxt1_error(const unsigned char rgba_block[16 * 4], const void * dxt_block, Decoder decoder/*=Decoder_D3D10*/) {
    return evaluate_dxt1_error(rgba_block, (const BlockDXT1 *)dxt_block, decoder);
}

float compress_dxt1(Quality level, const float * input_colors, const float * input_weights, const float rgb[3], bool three_color_mode, bool three_color_black, void * output) {
    return compress_dxt1(level, (Vector4*)input_colors, input_weights, { rgb[0], rgb[1], rgb[2] }, three_color_mode, three_color_black, (BlockDXT1*)output);
}

} // icbc

// // Do not polute preprocessor definitions.
// #undef ICBC_SIMD
// #undef ICBC_ASSERT

// #undef ICBC_SCALAR
// #undef ICBC_SSE2
// #undef ICBC_SSE41
// #undef ICBC_AVX1
// #undef ICBC_AVX2
// #undef ICBC_AVX512
// #undef ICBC_NEON
// #undef ICBC_VMX

// #undef ICBC_USE_FMA
// #undef ICBC_USE_AVX2_PERMUTE2
// #undef ICBC_USE_AVX512_PERMUTE
// #undef ICBC_USE_NEON_VTL

// #undef ICBC_PERFECT_ROUND

#endif // ICBC_IMPLEMENTATION

// Version History:
// v1.00 - Initial release.
// v1.01 - Added SPMD code path with AVX support.
// v1.02 - Removed SIMD code path.
// v1.03 - Quality levels. AVX512, Neon, Altivec, vectorized reduction and index selection.
// v1.04 - Automatic compile-time SIMD selection. Specify hw decoder at runtime. More optimizations.
// v1.05 - Bug fixes. Small optimizations.

// Copyright (c) 2020 Ignacio Castano <castano@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to	deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
