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

#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

#ifdef _MSC_VER
#pragma warning(push) // attribute 'x' is not recognized
#pragma warning(disable : 5030)
#endif

#if defined(__x86_64__) || defined(_M_AMD64) || defined(_M_IX86)
#define FASTGLTF_IS_X86
#elif defined(_M_ARM64) || defined(__aarch64__)
// __ARM_NEON is only for general Neon availability. It does not guarantee the full A64 instruction set.
#define FASTGLTF_IS_A64
#endif

namespace fastgltf::base64 {
    /**
     * Calculates the amount of base64 padding chars ('=') at the end of the encoded string.
     * @note There's at most 2 padding chars, and this function expects that the input string
     * points to the original string that has a size that is a multiple of 4 and is at least
     * 4 chars long.
     */
    [[gnu::always_inline]] constexpr std::size_t getPadding(std::string_view string) {
        assert(string.size() >= 4 && string.size() % 4 == 0);
        const auto size = string.size();
        for (auto i = 1; i < 4; ++i)
            if (string[size - i] != '=')
                return i - 1;
        return 0;
    }

    /**
     * Calculates the size of the decoded string based on the size of the base64 encoded string and
     * the amount of padding the encoded data contains.
     */
    [[gnu::always_inline]] constexpr std::size_t getOutputSize(std::size_t encodedSize, std::size_t padding) noexcept {
        assert(encodedSize % 4 == 0);
        return (encodedSize / 4) * 3 - padding;
    }

#if defined(FASTGLTF_IS_X86)
    void sse4_decode_inplace(std::string_view encoded, std::uint8_t* output, std::size_t padding);
    void avx2_decode_inplace(std::string_view encoded, std::uint8_t* output, std::size_t padding);

    [[nodiscard]] std::vector<std::uint8_t> sse4_decode(std::string_view encoded);
    [[nodiscard]] std::vector<std::uint8_t> avx2_decode(std::string_view encoded);
#elif defined(FASTGLTF_IS_A64)
    void neon_decode_inplace(std::string_view encoded, std::uint8_t* output, std::size_t padding);
    [[nodiscard]] std::vector<std::uint8_t> neon_decode(std::string_view encoded);
#endif
    void fallback_decode_inplace(std::string_view encoded, std::uint8_t* output, std::size_t padding);
    void decode_inplace(std::string_view encoded, std::uint8_t* output, std::size_t padding);

    [[nodiscard]] std::vector<std::uint8_t> fallback_decode(std::string_view encoded);
    [[nodiscard]] std::vector<std::uint8_t> decode(std::string_view encoded);
} // namespace fastgltf::base64

#ifdef _MSC_VER
#pragma warning(pop)
#endif
