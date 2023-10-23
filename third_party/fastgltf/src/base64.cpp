/*
 * Copyright (C) 2022 - 2023 spnda
 * This file is part of fastgltf <https://github.com/spnda/fastgltf>.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#if !defined(__cplusplus) || (!defined(_MSVC_LANG) && __cplusplus < 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG < 201703L)
#error "fastgltf requires C++17"
#endif

#include <array>
#include <cmath>
#include <functional>

#include "simdjson.h"

#include <fastgltf/base64.hpp>

#if defined(FASTGLTF_IS_X86)
#if defined(__clang__) || defined(__GNUC__)
// The idea behind manually including all headers with the required intrinsics
// is that the usual intrin.h will only include these under Clang when -mavx or
// -mavx2 is specified, which in turn would have the entire program be compiled
// with these instructions used in optimisations.
#include <immintrin.h>
#include <smmintrin.h>
#include <avxintrin.h>
#include <avx2intrin.h>
#else
#include <intrin.h>
#endif
#elif defined(FASTGLTF_IS_A64)
#include <arm_neon.h> // Includes arm64_neon.h on MSVC
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 5030)
#endif

namespace fg = fastgltf;

#if defined(_MSC_VER)
#define FORCEINLINE __forceinline
#else
// On other compilers we need the inline specifier, so that the functions in this compilation unit
// can be properly inlined without the "function body can be overwritten at link time" error.
#define FORCEINLINE inline
#endif

namespace fastgltf::base64 {
    using DecodeFunctionInplace = std::function<void(std::string_view, std::uint8_t*, std::size_t)>;
    using DecodeFunction = std::function<std::vector<std::uint8_t>(std::string_view)>;

    struct DecodeFunctionGetter {
        DecodeFunction func;
        DecodeFunctionInplace inplace;

        explicit DecodeFunctionGetter() {
            // We use simdjson's helper functions to determine which SIMD intrinsics are available at runtime.
            // The different implementations, because they're SIMD based, require a minimum amount of chars, as
            // they load multiple at once.
            const auto& impls = simdjson::get_available_implementations();
#if defined(FASTGLTF_IS_X86)
            if (const auto* avx2 = impls["haswell"]; avx2 != nullptr && avx2->supported_by_runtime_system()) {
                func = avx2_decode;
                inplace = avx2_decode_inplace;
            } else if (const auto* sse4 = impls["westmere"]; sse4 != nullptr && sse4->supported_by_runtime_system()) {
                func = sse4_decode;
                inplace = sse4_decode_inplace;
            }
#elif defined(FASTGLTF_IS_A64)
            // _M_ARM64 always guarantees 64-bit ARM processors that support NEON, defined by MSVC.
            // __aarch64__ always guarantees 64-bit ARMv8 processors that support NEON, defined by Clang.
            // __ARM_NEON always guarantees NEON support, defined by Clang and GCC.
            if (const auto* neon = impls["arm64"]; neon && neon->supported_by_runtime_system()) {
                func = neon_decode;
                inplace = neon_decode_inplace;
            }
#else
            if (false) {}
#endif
            else {
                func = fallback_decode;
                inplace = fallback_decode_inplace;
            }
        }

        static DecodeFunctionGetter* get() {
            static DecodeFunctionGetter getter;
            return &getter;
        }
    };
} // namespace fastgltf::base64

#if defined(FASTGLTF_IS_X86)
// The AVX and SSE decoding functions are based on http://0x80.pl/notesen/2016-01-17-sse-base64-decoding.html.
// It covers various methods of en-/decoding base64 using SSE and AVX and also shows their
// performance metrics.
// TODO: Mark these functions with msvc::forceinline which is available from C++20
[[gnu::target("avx2"), gnu::always_inline]] FORCEINLINE auto avx2_lookup_pshufb_bitmask(const __m256i input) {
    const auto higher_nibble = _mm256_and_si256(_mm256_srli_epi32(input, 4), _mm256_set1_epi8(0x0f));

    const auto shiftLUT = _mm256_setr_epi8(
        0,   0,  19,   4, -65, -65, -71, -71,
        0,   0,   0,   0,   0,   0,   0,   0,

        0,   0,  19,   4, -65, -65, -71, -71,
        0,   0,   0,   0,   0,   0,   0,   0);

    const auto sh     = _mm256_shuffle_epi8(shiftLUT,  higher_nibble);
    const auto eq_2f  = _mm256_cmpeq_epi8(input, _mm256_set1_epi8(0x2f));
    const auto shift  = _mm256_blendv_epi8(sh, _mm256_set1_epi8(16), eq_2f);

    return _mm256_add_epi8(input, shift);
}

[[gnu::target("avx2"), gnu::always_inline]] FORCEINLINE auto avx2_pack_ints(__m256i input) {
    const auto merge = _mm256_maddubs_epi16(input, _mm256_set1_epi32(0x01400140));
    return _mm256_madd_epi16(merge, _mm256_set1_epi32(0x00011000));
}

[[gnu::target("avx2")]] void fg::base64::avx2_decode_inplace(std::string_view encoded, std::uint8_t* output, std::size_t padding) {
    constexpr auto dataSetSize = 32;
    constexpr auto dataOutputSize = 24;

    if (encoded.size() < dataSetSize) {
        fallback_decode_inplace(encoded, output, padding);
        return;
    }

    // We align the size to the highest size divisible by 32. By doing this, we don't need to
    // allocate any new memory to hold the encoded data and let the fallback decoder decode the
    // remaining data.
    const auto encodedSize = encoded.size();
    const auto outputSize = getOutputSize(encodedSize, padding);
    const auto alignedSize = outputSize - (outputSize % dataOutputSize);
    auto* out = output;

    // _mm256_setr_epi8 accepts only 'char' but 0xff would overflow a signed char.
    // This gets optimised to the same assembly as a call to the aformentioned intrinsic.
    static const std::array<std::uint8_t, 32> shuffleData = {{
        2,  1,  0,
        6,  5,  4,
        10,  9,  8,
        14, 13, 12,
        0xff, 0xff, 0xff, 0xff,
        2,  1,  0,
        6,  5,  4,
        10,  9,  8,
        14, 13, 12,
        0xff, 0xff, 0xff, 0xff
    }};
    __m256i shuffle;
    std::memcpy(&shuffle, shuffleData.data(), shuffleData.size());

    std::size_t pos = 0;
    while ((pos + dataSetSize) < alignedSize) {
        auto in = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&encoded[pos]));
        auto values = avx2_lookup_pshufb_bitmask(in);
        const auto merged = avx2_pack_ints(values);
        const auto shuffled = _mm256_shuffle_epi8(merged, shuffle);

        // Beware: This writes 32 bytes, we just discard the top 8 bytes.
        _mm_storeu_si128(reinterpret_cast<__m128i*>(out), _mm256_castsi256_si128(shuffled));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(out + (dataOutputSize / 2)), _mm256_extracti128_si256(shuffled, 1));

        out += dataOutputSize;
        pos += dataSetSize;
    }

    // Decode the last chunk traditionally
    fallback_decode_inplace(encoded.substr(pos, encodedSize), out, padding);
}

[[gnu::target("avx2")]] std::vector<std::uint8_t> fg::base64::avx2_decode(std::string_view encoded) {
    const auto encodedSize = encoded.size();
    const auto padding = getPadding(encoded);

    std::vector<std::uint8_t> ret(getOutputSize(encodedSize, padding));
    avx2_decode_inplace(encoded, ret.data(), padding);

    return ret;
}

[[gnu::target("sse4.1"), gnu::always_inline]] FORCEINLINE auto sse4_lookup_pshufb_bitmask(const __m128i input) {
    const auto higher_nibble = _mm_and_si128(_mm_srli_epi32(input, 4), _mm_set1_epi8(0x0f));

    const auto shiftLUT = _mm_setr_epi8(
        0,   0,  19,   4, -65, -65, -71, -71,
        0,   0,   0,   0,   0,   0,   0,   0);

    const auto sh     = _mm_shuffle_epi8(shiftLUT,  higher_nibble);
    const auto eq_2f  = _mm_cmpeq_epi8(input, _mm_set1_epi8(0x2f));
    const auto shift  = _mm_blendv_epi8(sh, _mm_set1_epi8(16), eq_2f);

    return _mm_add_epi8(input, shift);
}

[[gnu::target("sse4.1"), gnu::always_inline]] FORCEINLINE auto sse4_pack_ints(__m128i input) {
    const auto merge = _mm_maddubs_epi16(input, _mm_set1_epi32(0x01400140));
    return _mm_madd_epi16(merge, _mm_set1_epi32(0x00011000));
}

[[gnu::target("sse4.1")]] void fg::base64::sse4_decode_inplace(std::string_view encoded, std::uint8_t* output, std::size_t padding) {
    constexpr auto dataSetSize = 16;
    constexpr auto dataOutputSize = 12;

    if (encoded.size() < dataSetSize) {
        fallback_decode_inplace(encoded, output, padding);
        return;
    }

    // We align the size to the highest size divisible by 16. By doing this, we don't need to
    // allocate any new memory to hold the encoded data and let the fallback decoder decode the
    // remaining data.
    const auto encodedSize = encoded.size();
    const auto outputSize = getOutputSize(encodedSize, padding);
    const auto alignedSize = outputSize - (outputSize % dataOutputSize);
    auto* out = output;

    // _mm_setr_epi8 accepts only 'char' but 0xff would overflow a signed char.
    // This gets optimised to the same assembly as a call to the aformentioned intrinsic.
    static const std::array<std::uint8_t, 16> shuffleData = {{
        2,  1,  0,
        6,  5,  4,
        10,  9,  8,
        14, 13, 12,
        0xff, 0xff, 0xff, 0xff,
    }};
    __m128i shuffle;
    std::memcpy(&shuffle, shuffleData.data(), shuffleData.size());

    std::size_t pos = 0;
    while ((pos + dataSetSize) < alignedSize) {
        auto in = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&encoded[pos]));
        auto values = sse4_lookup_pshufb_bitmask(in);
        const auto merged = sse4_pack_ints(values);
        const auto shuffled = _mm_shuffle_epi8(merged, shuffle);

        // Beware: This writes 16 bytes, we just discard the top 4 bytes.
        _mm_storeu_si128(reinterpret_cast<__m128i*>(out), shuffled);

        out += dataOutputSize;
        pos += dataSetSize;
    }

    // Decode the last chunk traditionally
    fallback_decode_inplace(encoded.substr(pos, encodedSize), out, padding);
}

[[gnu::target("sse4.1")]] std::vector<std::uint8_t> fg::base64::sse4_decode(std::string_view encoded) {
    const auto encodedSize = encoded.size();
    const auto padding = getPadding(encoded);

    std::vector<std::uint8_t> ret(getOutputSize(encodedSize, padding));
    sse4_decode_inplace(encoded, ret.data(), padding);

    return ret;
}
#elif defined(FASTGLTF_IS_A64)
[[gnu::always_inline]] FORCEINLINE int8x16_t neon_lookup_pshufb_bitmask(const uint8x16_t input) {
    // clang-format off
    constexpr std::array<int8_t, 16> shiftLUTdata = {
        0,   0,  19,   4, -65, -65, -71, -71,
        0,   0,   0,   0,   0,   0,   0,   0
    };
    // clang-fomat on

    const uint64x2_t higher_nibble = vandq_s32(vshlq_u32(vreinterpretq_u32_u8(input), vdupq_n_s32(-4)), vdupq_n_s8(0x0f));

    const int8x16_t shiftLUT = vld1q_s8(shiftLUTdata.data());

    const int8x16_t sh = vqtbl1q_s8(shiftLUT, vandq_u8(higher_nibble, vdupq_n_u8(0x8F)));
    const uint8x16_t eq_2f = vceqq_s8(input, vdupq_n_s8(0x2F));
    const uint8x16_t shift = vbslq_u8(vshrq_n_s8(eq_2f, 7), vdupq_n_s8(16), sh);

    return vaddq_s8(input, shift);
}

[[gnu::always_inline]] FORCEINLINE int16x8_t neon_pack_ints(const int8x16_t input) {
    const uint32x4_t mask = vdupq_n_u32(0x01400140);

    const int16x8_t tl = vmulq_s16(vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(input))), vmovl_s8(vget_low_s8(mask)));
    const int16x8_t th = vmulq_s16(vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(input))), vmovl_s8(vget_high_s8(mask)));
    const int16x8_t merge = vqaddq_s16(vuzp1q_s16(tl, th), vuzp2q_s16(tl, th));

    // Multiply the 8 signed 16-bit integers from a and b and add the n and n + 1 results together,
    // resulting in 4 32-bit integers.
    const uint32x4_t mergeMask = vdupq_n_u32(0x00011000);
    const int32x4_t pl = vmull_s16(vget_low_s16(merge), vget_low_s16(mergeMask));
    const int32x4_t ph = vmull_high_s16(merge, mergeMask);
    return vpaddq_s32(pl, ph);
}

// clang-format off
[[gnu::aligned(16)]] static constexpr std::array<uint8_t, 16> shuffleData = {
        2,  1,  0,
        6,  5,  4,
        10,  9,  8,
        14, 13, 12,
        0xff, 0xff, 0xff, 0xff
};
// clang-fomat on

void fg::base64::neon_decode_inplace(std::string_view encoded, std::uint8_t* output, std::size_t padding) {
    constexpr auto dataSetSize = 16;
    constexpr auto dataOutputSize = 12;

    if (encoded.size() < dataSetSize) {
        fallback_decode_inplace(encoded, output, padding);
        return;
    }

    // We align the size to the highest size divisible by 16. By doing this, we don't need to
    // allocate any new memory to hold the encoded data and let the fallback decoder decode the
    // remaining data.
    const auto encodedSize = encoded.size();
    const auto alignedSize = encodedSize - (encodedSize % dataSetSize);
    auto* out = output;

    // Decode the first 16 long chunks with Neon intrinsics
    const auto shuffle = vld1q_u8(shuffleData.data());
    std::size_t pos = 0;
    while ((pos + dataSetSize) < alignedSize) {
        // Load 16 8-bit values into a 128-bit register.
        auto in = vld1q_u8(reinterpret_cast<const std::uint8_t*>(&encoded[pos]));
        auto values = neon_lookup_pshufb_bitmask(in);
        const auto merged = neon_pack_ints(values);

        const auto masked = vandq_u8(shuffle, vdupq_n_u8(0x8F));
        const auto shuffled = vqtbl1q_s8(merged, masked);

        // Store 16 8-bit values into output pointer
        vst1q_u8(out, shuffled);
        out += dataOutputSize;
        pos += dataSetSize;
    }

    // Decode the last chunk traditionally
    fallback_decode_inplace(encoded.substr(pos, encodedSize), out, padding);
}

std::vector<std::uint8_t> fg::base64::neon_decode(std::string_view encoded) {
    const auto encodedSize = encoded.size();
    const auto padding = getPadding(encoded);

    std::vector<std::uint8_t> ret(getOutputSize(encodedSize, padding));
    neon_decode_inplace(encoded, ret.data(), padding);

    return ret;
}
#endif

// clang-format off
// ASCII value -> base64 value LUT
static constexpr std::array<std::uint8_t, 128> base64lut = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,62,0,0,0,63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
    0,0,0,0,0,0,0,
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,
    0,0,0,0,0,0,
    26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
    0,0,0,0,0,
};
// clang-format on

void fg::base64::fallback_decode_inplace(std::string_view encoded, std::uint8_t* output, std::size_t padding) {
    std::array<std::uint8_t, 4> sixBitChars = {};
    std::array<std::uint8_t, 3> eightBitChars = {};

    // We use i here to track how many we've parsed and to batch 4 chars together.
    const auto encodedSize = encoded.size();
    std::size_t cursor = 0U;
    for (auto pos = 0U; pos < encodedSize; pos += 4) {
		std::memcpy(sixBitChars.data(), &encoded[pos], 4 * sizeof(char));

        for (std::size_t i = 0; i < 4; i++) {
            assert(static_cast<std::size_t>(sixBitChars[i]) < base64lut.size());
            sixBitChars[i] = base64lut[sixBitChars[i]];
        }

        eightBitChars[0] = (sixBitChars[0] << 2) + ((sixBitChars[1] & 0x30) >> 4);
        eightBitChars[1] = ((sixBitChars[1] & 0xf) << 4) + ((sixBitChars[2] & 0x3c) >> 2);
        eightBitChars[2] = ((sixBitChars[2] & 0x3) << 6) + sixBitChars[3];

        // Write the 3 chars to the output memory, making sure to not write over the end, assuming
		// that the array is correctly sized.
        std::size_t charsToWrite = 3 - ((pos + 4 == encodedSize) ? padding : 0);
        for (std::size_t j = 0; j < charsToWrite; ++j) {
            output[cursor++] = eightBitChars[j];
        }
    }
}

std::vector<std::uint8_t> fg::base64::fallback_decode(std::string_view encoded) {
    const auto encodedSize = encoded.size();
    const auto padding = getPadding(encoded);

    std::vector<std::uint8_t> ret(getOutputSize(encodedSize, padding));
    fallback_decode_inplace(encoded, ret.data(), padding);

    return ret;
}

void fg::base64::decode_inplace(std::string_view encoded, std::uint8_t* output, std::size_t padding) {
    assert(encoded.size() % 4 == 0);

    return DecodeFunctionGetter::get()->inplace(encoded, output, padding);
}

std::vector<std::uint8_t> fg::base64::decode(std::string_view encoded) {
    assert(encoded.size() % 4 == 0);

    return DecodeFunctionGetter::get()->func(encoded);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
