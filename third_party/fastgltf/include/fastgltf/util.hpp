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

#include <array>
#include <cmath>
#include <limits>
#include <string_view>
#include <type_traits>

// Macros to determine C++ standard version
#if (!defined(_MSVC_LANG) && __cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#define FASTGLTF_CPP_17 1
#else
#error "fastgltf requires C++17"
#endif

#if (!defined(_MSVC_LANG) && __cplusplus >= 202002L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
#define FASTGLTF_CPP_20 1
#else
#define FASTGLTF_CPP_20 0
#endif

#if (!defined(_MSVC_LANG) && __cplusplus >= 202302L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 202302L)
#define FASTGLTF_CPP_23 1
#else
#define FASTGLTF_CPP_23 0
#endif

#if FASTGLTF_CPP_20 && defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L
#define FASTGLTF_HAS_BIT 1
#include <bit>
#else
#define FASTGLTF_HAS_BIT 0
#endif

#if FASTGLTF_CPP_20 && defined(__cpp_concepts) && __cpp_concepts >= 202002L
#define FASTGLTF_HAS_CONCEPTS 1
#include <concepts>
#else
#define FASTGLTF_HAS_CONCEPTS 0
#endif

#if FASTGLTF_CPP_23
#define FASTGLTF_UNREACHABLE std::unreachable();
#elif defined(__GNUC__)
#define FASTGLTF_UNREACHABLE __builtin_unreachable();
#elif defined(_MSC_VER)
#define FASTGLTF_UNREACHABLE __assume(false);
#else
#define FASTGLTF_UNREACHABLE assert(0);
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 5030) // attribute 'x' is not recognized
#pragma warning(disable : 4514) // unreferenced inline function has been removed
#endif

namespace fastgltf {
    template<typename T>
#if FASTGLTF_HAS_CONCEPTS
    requires std::is_enum_v<T>
#endif
    [[nodiscard]] constexpr std::underlying_type_t<T> to_underlying(T t) noexcept {
#if !FASTGLTF_HAS_CONCEPTS
        static_assert(std::is_enum_v<T>, "to_underlying only works with enum types.");
#endif
        return static_cast<std::underlying_type_t<T>>(t);
    }

    template <typename T, typename U>
#if FASTGLTF_HAS_CONCEPTS
    requires ((std::is_enum_v<T> && std::integral<std::underlying_type_t<T>>) || std::integral<T>) && requires (T t, U u) {
        { t & u } -> std::same_as<U>;
    }
#endif
    [[nodiscard]] constexpr bool hasBit(T flags, U bit) {
#if !FASTGLTF_HAS_CONCEPTS
        static_assert((std::is_enum_v<T> && std::is_integral_v<std::underlying_type_t<T>>) || std::is_integral_v<T>);
#endif
        return (flags & bit) == bit;
    }

    template <typename T>
    [[nodiscard]] constexpr T alignUp(T base, T alignment) {
        static_assert(std::is_signed_v<T>, "alignUp requires type T to be signed.");
        return (base + alignment - 1) & -alignment;
    }

    template <typename T>
    [[nodiscard]] constexpr T alignDown(T base, T alignment) {
        return base - (base % alignment);
    }

    template <typename T>
#if FASTGLTF_HAS_CONCEPTS
    requires requires (T t) {
        { t > t } -> std::same_as<bool>;
    }
#endif
    [[nodiscard]] constexpr T max(T a, T b) noexcept {
        return (a > b) ? a : b;
    }

    /**
     * Decomposes a transform matrix into the translation, rotation, and scale components. This
     * function does not support skew, shear, or perspective. This currently uses a quick algorithm
     * to calculate the quaternion from the rotation matrix, which might occasionally loose some
     * precision, though we try to use doubles here.
     */
    inline void decomposeTransformMatrix(std::array<float, 16> matrix, std::array<float, 3>& scale, std::array<float, 4>& rotation, std::array<float, 3>& translation) {
        // Extract the translation. We zero the translation out, as we reuse the matrix as
        // the rotation matrix at the end.
        translation = {matrix[12], matrix[13], matrix[14]};
        matrix[12] = matrix[13] = matrix[14] = 0;

        // Extract the scale. We calculate the euclidean length of the columns. We then
        // construct a vector with those lengths. My gcc's stdlib doesn't include std::sqrtf
        // for some reason...
        auto s1 = sqrtf(matrix[0] * matrix[0] + matrix[1] * matrix[1] +  matrix[2] * matrix[2]);
        auto s2 = sqrtf(matrix[4] * matrix[4] + matrix[5] * matrix[5] +  matrix[6] * matrix[6]);
        auto s3 = sqrtf(matrix[8] * matrix[8] + matrix[9] * matrix[9] + matrix[10] * matrix[10]);
        scale = {s1, s2, s3};

        // Remove the scaling from the matrix, leaving only the rotation. matrix is now the
        // rotation matrix.
        matrix[0] /= s1; matrix[1] /= s1;  matrix[2] /= s1;
        matrix[4] /= s2; matrix[5] /= s2;  matrix[6] /= s2;
        matrix[8] /= s3; matrix[9] /= s3; matrix[10] /= s3;

        // Construct the quaternion. This algo is copied from here:
        // https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/christian.htm.
        // glTF orders the components as x,y,z,w
        rotation = {
            max(.0f, 1 + matrix[0] - matrix[5] - matrix[10]),
            max(.0f, 1 - matrix[0] + matrix[5] - matrix[10]),
            max(.0f, 1 - matrix[0] - matrix[5] + matrix[10]),
            max(.0f, 1 + matrix[0] + matrix[5] + matrix[10]),
        };
        for (auto& x : rotation) {
            x = static_cast<float>(std::sqrt(static_cast<double>(x)) / 2);
        }
        rotation[0] = std::copysignf(rotation[0], matrix[6] - matrix[9]);
        rotation[1] = std::copysignf(rotation[1], matrix[8] - matrix[2]);
        rotation[2] = std::copysignf(rotation[2], matrix[1] - matrix[4]);
    }

    /**
     * Constants generated using 0x82f63b79u CRC poly.
     */
    static constexpr std::array<std::uint32_t, 256> crcHashTable = {{
        0x00000000, 0xf26b8303, 0xe13b70f7, 0x1350f3f4, 0xc79a971f, 0x35f1141c, 0x26a1e7e8, 0xd4ca64eb,
        0x8ad958cf, 0x78b2dbcc, 0x6be22838, 0x9989ab3b, 0x4d43cfd0, 0xbf284cd3, 0xac78bf27, 0x5e133c24,
        0x105ec76f, 0xe235446c, 0xf165b798, 0x030e349b, 0xd7c45070, 0x25afd373, 0x36ff2087, 0xc494a384,
        0x9a879fa0, 0x68ec1ca3, 0x7bbcef57, 0x89d76c54, 0x5d1d08bf, 0xaf768bbc, 0xbc267848, 0x4e4dfb4b,
        0x20bd8ede, 0xd2d60ddd, 0xc186fe29, 0x33ed7d2a, 0xe72719c1, 0x154c9ac2, 0x061c6936, 0xf477ea35,
        0xaa64d611, 0x580f5512, 0x4b5fa6e6, 0xb93425e5, 0x6dfe410e, 0x9f95c20d, 0x8cc531f9, 0x7eaeb2fa,
        0x30e349b1, 0xc288cab2, 0xd1d83946, 0x23b3ba45, 0xf779deae, 0x05125dad, 0x1642ae59, 0xe4292d5a,
        0xba3a117e, 0x4851927d, 0x5b016189, 0xa96ae28a, 0x7da08661, 0x8fcb0562, 0x9c9bf696, 0x6ef07595,
        0x417b1dbc, 0xb3109ebf, 0xa0406d4b, 0x522bee48, 0x86e18aa3, 0x748a09a0, 0x67dafa54, 0x95b17957,
        0xcba24573, 0x39c9c670, 0x2a993584, 0xd8f2b687, 0x0c38d26c, 0xfe53516f, 0xed03a29b, 0x1f682198,
        0x5125dad3, 0xa34e59d0, 0xb01eaa24, 0x42752927, 0x96bf4dcc, 0x64d4cecf, 0x77843d3b, 0x85efbe38,
        0xdbfc821c, 0x2997011f, 0x3ac7f2eb, 0xc8ac71e8, 0x1c661503, 0xee0d9600, 0xfd5d65f4, 0x0f36e6f7,
        0x61c69362, 0x93ad1061, 0x80fde395, 0x72966096, 0xa65c047d, 0x5437877e, 0x4767748a, 0xb50cf789,
        0xeb1fcbad, 0x197448ae, 0x0a24bb5a, 0xf84f3859, 0x2c855cb2, 0xdeeedfb1, 0xcdbe2c45, 0x3fd5af46,
        0x7198540d, 0x83f3d70e, 0x90a324fa, 0x62c8a7f9, 0xb602c312, 0x44694011, 0x5739b3e5, 0xa55230e6,
        0xfb410cc2, 0x092a8fc1, 0x1a7a7c35, 0xe811ff36, 0x3cdb9bdd, 0xceb018de, 0xdde0eb2a, 0x2f8b6829,
        0x82f63b78, 0x709db87b, 0x63cd4b8f, 0x91a6c88c, 0x456cac67, 0xb7072f64, 0xa457dc90, 0x563c5f93,
        0x082f63b7, 0xfa44e0b4, 0xe9141340, 0x1b7f9043, 0xcfb5f4a8, 0x3dde77ab, 0x2e8e845f, 0xdce5075c,
        0x92a8fc17, 0x60c37f14, 0x73938ce0, 0x81f80fe3, 0x55326b08, 0xa759e80b, 0xb4091bff, 0x466298fc,
        0x1871a4d8, 0xea1a27db, 0xf94ad42f, 0x0b21572c, 0xdfeb33c7, 0x2d80b0c4, 0x3ed04330, 0xccbbc033,
        0xa24bb5a6, 0x502036a5, 0x4370c551, 0xb11b4652, 0x65d122b9, 0x97baa1ba, 0x84ea524e, 0x7681d14d,
        0x2892ed69, 0xdaf96e6a, 0xc9a99d9e, 0x3bc21e9d, 0xef087a76, 0x1d63f975, 0x0e330a81, 0xfc588982,
        0xb21572c9, 0x407ef1ca, 0x532e023e, 0xa145813d, 0x758fe5d6, 0x87e466d5, 0x94b49521, 0x66df1622,
        0x38cc2a06, 0xcaa7a905, 0xd9f75af1, 0x2b9cd9f2, 0xff56bd19, 0x0d3d3e1a, 0x1e6dcdee, 0xec064eed,
        0xc38d26c4, 0x31e6a5c7, 0x22b65633, 0xd0ddd530, 0x0417b1db, 0xf67c32d8, 0xe52cc12c, 0x1747422f,
        0x49547e0b, 0xbb3ffd08, 0xa86f0efc, 0x5a048dff, 0x8ecee914, 0x7ca56a17, 0x6ff599e3, 0x9d9e1ae0,
        0xd3d3e1ab, 0x21b862a8, 0x32e8915c, 0xc083125f, 0x144976b4, 0xe622f5b7, 0xf5720643, 0x07198540,
        0x590ab964, 0xab613a67, 0xb831c993, 0x4a5a4a90, 0x9e902e7b, 0x6cfbad78, 0x7fab5e8c, 0x8dc0dd8f,
        0xe330a81a, 0x115b2b19, 0x020bd8ed, 0xf0605bee, 0x24aa3f05, 0xd6c1bc06, 0xc5914ff2, 0x37faccf1,
        0x69e9f0d5, 0x9b8273d6, 0x88d28022, 0x7ab90321, 0xae7367ca, 0x5c18e4c9, 0x4f48173d, 0xbd23943e,
        0xf36e6f75, 0x0105ec76, 0x12551f82, 0xe03e9c81, 0x34f4f86a, 0xc69f7b69, 0xd5cf889d, 0x27a40b9e,
        0x79b737ba, 0x8bdcb4b9, 0x988c474d, 0x6ae7c44e, 0xbe2da0a5, 0x4c4623a6, 0x5f16d052, 0xad7d5351,
    }};

    [[gnu::hot, gnu::const]] constexpr std::uint32_t crc32c(std::string_view str) noexcept {
        std::uint32_t crc = 0;
        for (auto c : str)
            crc = (crc >> 8) ^ crcHashTable[(crc ^ static_cast<std::uint8_t>(c)) & 0xff];
        return crc;
    }

    [[gnu::hot, gnu::const]] constexpr std::uint32_t crc32c(const std::uint8_t* d, std::size_t len) noexcept {
        std::uint32_t crc = 0;
        for (std::size_t i = 0; i < len; ++i)
            crc = (crc >> 8) ^ crcHashTable[(crc ^ d[i]) & 0xff];
        return crc;
    }

#if defined(__x86_64__) || defined(_M_AMD64) || defined(_M_IX86)
    /**
     * Variant of crc32 that uses SSE4.2 instructions to increase performance. Note that this does not
     * check for availability of said instructions.
     */
    [[gnu::hot, gnu::const]] std::uint32_t hwcrc32c(std::string_view str) noexcept;
    [[gnu::hot, gnu::const]] std::uint32_t hwcrc32c(const std::uint8_t* d, std::size_t len) noexcept;
#endif

    /**
     * Helper to force evaluation of constexpr functions at compile-time in C++17. One example of
     * this is with crc32: force_consteval<crc32("string")>. No matter the context, this will
     * always be evaluated to a constant.
     */
    template <auto V>
    static constexpr auto force_consteval = V;

    /**
     * Counts the leading zeros from starting the most significant bit. Returns a std::uint8_t as there
     * can only ever be 2^6 zeros with 64-bit types.
     */
     template <typename T>
#if FASTGLTF_HAS_CONCEPTS
    requires std::integral<T>
#endif
    [[gnu::const]] inline std::uint8_t clz(T value) {
        static_assert(std::is_integral_v<T>);
#if FASTGLTF_HAS_BIT
        return static_cast<std::uint8_t>(std::countl_zero(value));
#else
        // Very naive but working implementation of counting zero bits. Any sane compiler will
        // optimise this away, like instead use the bsr x86 instruction.
        if (value == 0) return 64;
        std::uint8_t count = 0;
        for (auto i = std::numeric_limits<T>::digits - 1; i > 0; --i) {
            if ((value >> i) == 1) {
                return count;
            }
            ++count;
        }
        return count;
#endif
    }

	template <typename T>
	[[gnu::const]] inline std::uint8_t popcount(T value) {
		static_assert(std::is_integral_v<T>);
#if FASTGLTF_HAS_BIT
		return static_cast<std::uint8_t>(std::popcount(value));
#else
		std::uint8_t bits = 0;
		while (value) {
			if (value & 1)
				++bits;
			value >>= 1;
		}
		return bits;
#endif
	}

    /**
     * Essentially the same as std::same<T, U> but it accepts multiple different types for U,
     * checking if T is any of U...
     */
    template <typename T, typename... Ts>
    using is_any = std::disjunction<std::is_same<T, Ts>...>;

    /**
     * Simple function to check if the given string starts with a given set of characters.
     */
    inline bool startsWith(std::string_view str, std::string_view search) {
        return str.rfind(search, 0) == 0;
    }

	/**
	 * Helper type in order to allow building a visitor out of multiple lambdas within a call to
	 * std::visit
	 */
	template<class... Ts> 
	struct visitor : Ts... {
		using Ts::operator()...;
	};

	template<class... Ts> visitor(Ts...) -> visitor<Ts...>;

    // For simple ops like &, |, +, - taking a left and right operand.
#define FASTGLTF_ARITHMETIC_OP_TEMPLATE_MACRO(T1, T2, op) \
    constexpr T1 operator op(const T1& a, const T2& b) noexcept { \
        static_assert(std::is_enum_v<T1> && std::is_enum_v<T2>); \
        return static_cast<T1>(to_underlying(a) op to_underlying(b)); \
    }

    // For any ops like |=, &=, +=, -=
#define FASTGLTF_ASSIGNMENT_OP_TEMPLATE_MACRO(T1, T2, op) \
    constexpr T1& operator op##=(T1& a, const T2& b) noexcept { \
        static_assert(std::is_enum_v<T1> && std::is_enum_v<T2>); \
        return a = static_cast<T1>(to_underlying(a) op to_underlying(b)), a; \
    }

    // For unary +, unary -, and bitwise NOT
#define FASTGLTF_UNARY_OP_TEMPLATE_MACRO(T, op) \
    constexpr T operator op(const T& a) noexcept { \
        static_assert(std::is_enum_v<T>); \
        return static_cast<T>(op to_underlying(a)); \
    }
} // namespace fastgltf

#ifdef _MSC_VER
#pragma warning(pop)
#endif
