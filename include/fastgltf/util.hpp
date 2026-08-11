/*
 * Copyright (C) 2022 - 2026 Sean Apeler
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

#ifndef FASTGLTF_UTIL_HPP
#define FASTGLTF_UTIL_HPP

#if !defined(FASTGLTF_USE_STD_MODULE) || !FASTGLTF_USE_STD_MODULE
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <string_view>
#include <type_traits>
#include <variant>
#endif

#ifndef FASTGLTF_EXPORT
#define FASTGLTF_EXPORT
#endif

// Macros to determine C++ standard version
#if (!defined(_MSVC_LANG) && __cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#define FASTGLTF_CPP_17 1
#else
#error "fastgltf requires C++17"
#endif

#if (!defined(_MSVC_LANG) && __cplusplus >= 202002L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
#define FASTGLTF_CPP_20 1
#include <version>
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
#if !defined(FASTGLTF_USE_STD_MODULE) || !FASTGLTF_USE_STD_MODULE
#include <bit>
#endif
#else
#define FASTGLTF_HAS_BIT 0
#endif

#if FASTGLTF_CPP_20 && defined(__cpp_concepts) && __cpp_concepts >= 201907L
#define FASTGLTF_HAS_CONCEPTS 1
#if !defined(FASTGLTF_USE_STD_MODULE) || !FASTGLTF_USE_STD_MODULE
#include <concepts>
#endif
#else
#define FASTGLTF_HAS_CONCEPTS 0
#endif

#if FASTGLTF_CPP_23
#define FASTGLTF_UNREACHABLE std::unreachable();
#elif defined(__GNUC__) || defined(__clang__)
#define FASTGLTF_UNREACHABLE __builtin_unreachable();
#elif defined(_MSC_VER)
#define FASTGLTF_UNREACHABLE __assume(false);
#else
#define FASTGLTF_UNREACHABLE assert(0);
#endif

#if defined(__has_builtin)
#define FASTGLTF_HAS_BUILTIN(x) __has_builtin(x)
#else
#define FASTGLTF_HAS_BUILTIN(x) 0
#endif

#if defined(__x86_64__) || defined(_M_AMD64) || defined(_M_IX86)
#define FASTGLTF_IS_X86 1
#elif defined(_M_ARM64) || defined(__aarch64__)
// __ARM_NEON is only for general Neon availability. It does not guarantee the full A64 instruction set.
#define FASTGLTF_IS_A64 1
#endif

#if FASTGLTF_CPP_20 || (defined(__clang__) && __clang_major__ >= 12) || (defined(__GNUC__) && __GNUC__ >= 9)
// These attributes were introduced with C++20, but Clang 12 already supports them since C++11.
#define FASTGLTF_LIKELY [[likely]]
#define FASTGLTF_UNLIKELY [[unlikely]]
#else
#define FASTGLTF_LIKELY
#define FASTGLTF_UNLIKELY
#endif

#if (_MSC_VER && !defined(__clang__)) || FASTGLTF_CPP_20 && __has_cpp_attribute(msvc::intrinsic)
#define FASTGLTF_INTRINSIC [[msvc::intrinsic]]
#else
#define FASTGLTF_INTRINSIC
#endif

#if defined(_MSC_VER)
#define FASTGLTF_FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define FASTGLTF_FORCEINLINE [[gnu::always_inline]] inline
#else
// On other compilers we need the inline specifier, so that the functions in this compilation unit
// can be properly inlined without the "function body can be overwritten at link time" error.
#define FASTGLTF_FORCEINLINE inline
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 5030) // attribute 'x' is not recognized
#pragma warning(disable : 4514) // unreferenced inline function has been removed
#endif

namespace fastgltf {
	FASTGLTF_EXPORT template<typename T>
#if FASTGLTF_HAS_CONCEPTS
	requires std::is_enum_v<T>
#endif
	[[nodiscard]] FASTGLTF_INTRINSIC constexpr std::underlying_type_t<T> to_underlying(T t) noexcept {
#if !FASTGLTF_HAS_CONCEPTS
		static_assert(std::is_enum_v<T>, "to_underlying only works with enum types.");
#endif
		return static_cast<std::underlying_type_t<T>>(t);
	}

	FASTGLTF_EXPORT template <typename T, typename U>
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

	template <typename T, typename U>
	[[nodiscard]] constexpr T alignUp(T base, U alignment) {
		static_assert(std::is_signed_v<U>, "alignUp requires type U to be signed.");
		return (base + alignment - 1) & -alignment;
	}

	template <typename T>
	[[nodiscard]] constexpr T alignDown(T base, T alignment) {
		return base - (base % alignment);
	}

	FASTGLTF_EXPORT template <typename T>
#if FASTGLTF_HAS_CONCEPTS
	requires requires (T t) {
		{ t > t } -> std::same_as<bool>;
	}
#endif
	[[nodiscard]] constexpr const T& max(const T& a, const T& b) noexcept {
		return (a > b) ? a : b;
	}

	FASTGLTF_EXPORT template <typename T>
#if FASTGLTF_HAS_CONCEPTS
	requires requires (T t) {
		{ t < t } -> std::same_as<bool>;
	}
#endif
	[[nodiscard]] constexpr const T& min(const T& a, const T& b) noexcept {
		return (a < b) ? a : b;
	}

	template<typename T, typename... A>
	[[noreturn]] constexpr void raise([[maybe_unused]] A&&... args) {
#ifdef __cpp_exceptions
		throw T(std::forward<A>(args)...);
#else
		std::abort();
#endif
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

#if defined(FASTGLTF_IS_X86)
	/**
	 * Variant of crc32 that uses SSE4.2 instructions to increase performance. Note that this does not
	 * check for availability of said instructions.
	 */
	[[gnu::hot, gnu::const]] std::uint32_t sse_crc32c(std::string_view str) noexcept;
	[[gnu::hot, gnu::const]] std::uint32_t sse_crc32c(const std::uint8_t* d, std::size_t len) noexcept;
#elif defined(FASTGLTF_IS_A64) && !defined(_MSC_VER) && !defined(__ANDROID__)
	// Both MSVC stdlib and Android NDK don't include the arm intrinsics. TODO: Find a workaround?
#define FASTGLTF_ENABLE_ARMV8_CRC 1
	[[gnu::hot, gnu::const]] std::uint32_t armv8_crc32c(std::string_view str) noexcept;
	[[gnu::hot, gnu::const]] std::uint32_t armv8_crc32c(const std::uint8_t* d, std::size_t len) noexcept;
#endif

	/**
	 * Helper to force evaluation of constexpr functions at compile-time in C++17. One example of
	 * this is with crc32: force_consteval<crc32("string")>. No matter the context, this will
	 * always be evaluated to a constant.
	 */
	template <auto V>
	static constexpr auto force_consteval = V;

	/**
	 * Essentially the same as std::same<T, U> but it accepts multiple different types for U,
	 * checking if T is any of U...
	 */
	template <typename T, typename... Ts>
	using is_any_of = std::disjunction<std::is_same<T, Ts>...>;

	template <typename T, typename... Ts>
	constexpr bool is_any_of_v = is_any_of<T, Ts...>::value;

	// Simple reimplementation of std::remove_cvref, which was only added in C++20.
	template <typename T>
	struct remove_cvref {
		using type = std::remove_cv_t<std::remove_reference_t<T>>;
	};
	template <typename T>
	using remove_cvref_t = typename remove_cvref<T>::type;

	/**
	 * Helper type in order to allow building a visitor out of multiple lambdas within a call to
	 * std::visit
	 */
	FASTGLTF_EXPORT template<class... Ts>
	struct visitor : Ts... {
		using Ts::operator()...;
	};

	FASTGLTF_EXPORT template<class... Ts> visitor(Ts...) -> visitor<Ts...>;

	template <typename Visitor, typename Variant, std::size_t... i>
	constexpr bool is_exhaustive_visitor(std::integer_sequence<std::size_t, i...>) noexcept {
		return std::conjunction_v<std::is_invocable<Visitor, std::variant_alternative_t<i, remove_cvref_t<Variant>>>...>;
	}

	/**
	 * Simple wrapper around std::visit for a single variant that checks at compile-time if the given visitor contains
	 * overloads for *all* required alternatives. This is meant to guarantee correctness, since using something like
	 * fastgltf::visitor could fail unexpectedly due to const-issues without any compile-time errors or warnings.
	 * @note This currently does not support auto parameters.
	 */
	FASTGLTF_EXPORT template<typename Visitor, typename Variant>
	constexpr decltype(auto) visit_exhaustive(Visitor&& visitor, Variant&& variant) {
		static_assert(is_exhaustive_visitor<Visitor, Variant>(std::make_index_sequence<std::variant_size_v<remove_cvref_t<Variant>>>()),
			"The visitor does not include all necessary overloads for the given variant");
		return std::visit(std::forward<Visitor>(visitor), std::forward<Variant>(variant));
	}

	template <auto callback>
	struct UniqueDeleter {
		template <typename T>
		constexpr void operator()(T* t) const {
			callback(t);
		}
	};

	/** Allows the quick creation of a unique_ptr type that can take any function of the form void(T*) for its deleter */
	template <typename T, auto callback>
	using deletable_unique_ptr = std::unique_ptr<T, UniqueDeleter<callback>>;
	namespace internal {
		template <typename T, std::size_t N, std::size_t... I>
		constexpr std::array<std::remove_cv_t<T>, N> to_array_impl(T (&a)[N], std::index_sequence<I...>) {
			return {{ a[I]... }};
		}
		template <typename T, std::size_t N, std::size_t... I>
		constexpr std::array<std::remove_cv_t<T>, N> to_array_impl(T (&&a)[N], std::index_sequence<I...>) {
			return {{ std::move(a[I])... }};
		}
	} // namespace internal

	// Basic reimplementation of std::to_array
	template <typename T, std::size_t N>
	constexpr decltype(auto) to_array(T (&a)[N]) {
		return internal::to_array_impl(a, std::make_index_sequence<N>{});
	}
	template <typename T, std::size_t N>
	constexpr decltype(auto) to_array(T (&&a)[N]) {
		return internal::to_array_impl(a, std::make_index_sequence<N>{});
	}

	// For simple ops like &, |, +, - taking a left and right operand.
#define FASTGLTF_ARITHMETIC_OP_TEMPLATE_MACRO(T1, T2, op) \
	FASTGLTF_EXPORT constexpr T1 operator op(const T1& a, const T2& b) noexcept { \
		static_assert(std::is_enum_v<T1> && std::is_enum_v<T2>); \
		return static_cast<T1>(to_underlying(a) op to_underlying(b)); \
	}

	// For any ops like |=, &=, +=, -=
#define FASTGLTF_ASSIGNMENT_OP_TEMPLATE_MACRO(T1, T2, op) \
	FASTGLTF_EXPORT constexpr T1& operator op##=(T1& a, const T2& b) noexcept { \
		static_assert(std::is_enum_v<T1> && std::is_enum_v<T2>); \
		return a = static_cast<T1>(to_underlying(a) op to_underlying(b)), a; \
	}

	// For unary +, unary -, and bitwise NOT
#define FASTGLTF_UNARY_OP_TEMPLATE_MACRO(T, op) \
	FASTGLTF_EXPORT constexpr T operator op(const T& a) noexcept { \
		static_assert(std::is_enum_v<T>); \
		return static_cast<T>(op to_underlying(a)); \
	}

#if FASTGLTF_CPP_20 && defined(__cpp_lib_bit_cast) && __cpp_lib_bit_cast >= 201806L
#define FASTGLTF_CONSTEXPR_BITCAST 1
	template<typename To, typename From>
	[[nodiscard]] constexpr To bit_cast(const From& from) noexcept {
		return std::bit_cast<To>(from);
	}
#elif (defined(__clang__) || __clang_major__ >= 9) || (defined(__GNUC__) && __GNUC__ >= 11) || FASTGLTF_HAS_BUILTIN(__builtin_bit_cast)
#define FASTGLTF_CONSTEXPR_BITCAST 1
	template<typename To, typename From>
	[[nodiscard]] constexpr To bit_cast(const From& from) noexcept {
		// Available since Clang 9, GCC 11.1, and MSVC 16.6. Otherwise, this function could not be constexpr.
		return __builtin_bit_cast(To, from);
	}
#else
#define FASTGLTF_CONSTEXPR_BITCAST 0
	template<typename To, typename From>
	[[nodiscard]] To bit_cast(const From& from) noexcept {
		static_assert(std::is_trivially_constructible_v<To>);
		To dst;
		std::memcpy(&dst, &from, sizeof(To));
		return dst;
	}
#endif

#if FASTGLTF_CPP_20 && defined(__cpp_lib_byteswap) && __cpp_lib_byteswap >= 202110L
	template<class T>
	constexpr T byteswap(T n) noexcept {
		return std::byteswap(n);
	}
#else
	template<typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true> [[nodiscard]]
#if FASTGLTF_CONSTEXPR_BITCAST
	constexpr
#endif
	auto byteswap(T value) noexcept {
		static_assert(std::has_unique_object_representations_v<T>, "T may not have padding bits");
		auto bytes = bit_cast<std::array<std::byte, sizeof(T)>>(value);
		bytes = decltype(bytes)(bytes.rbegin(), bytes.rend());
		return bit_cast<T>(bytes);
	}
#endif

	/**
	 * Counts the leading zeros from starting the most significant bit. Returns a std::uint8_t as there
	 * can only ever be 2^6 zeros with 64-bit types.
	 */
	template <typename T>
#if FASTGLTF_HAS_CONCEPTS
	requires std::integral<T>
#endif
	[[nodiscard, gnu::const]] constexpr auto clz(T value) {
#if !FASTGLTF_HAS_CONCEPTS
		static_assert(std::is_integral_v<T>);
#endif
#if FASTGLTF_HAS_BIT
		return static_cast<std::uint_fast8_t>(std::countl_zero(value));
#else
		// Very naive but working implementation of counting zero bits. Any sane compiler will
		// optimise this away, like instead use the bsr x86 instruction.
		if (value == 0) return static_cast<std::uint_fast8_t>(sizeof(T) * 8);
		std::uint_fast8_t count = 0;
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
#if FASTGLTF_HAS_CONCEPTS
	requires std::integral<T>
#endif
	[[nodiscard, gnu::const]] constexpr auto popcount(T value) {
#if !FASTGLTF_HAS_CONCEPTS
		static_assert(std::is_integral_v<T>);
#endif
#if FASTGLTF_HAS_BIT
		return static_cast<std::uint_fast8_t>(std::popcount(value));
#else
		std::uint_fast8_t bits = 0;
		while (value) {
			if (value & 1)
				++bits;
			value >>= 1;
		}
		return bits;
#endif
	}

	/**
	 * Returns the absolute value of the given integer in its unsigned type.
	 * This avoids the issue with two complementary signed integers not being able to represent INT_MIN.
	 */
	template <typename T>
	[[nodiscard]] constexpr std::make_unsigned_t<T> uabs(T val) {
		if constexpr (std::is_signed_v<T>) {
			using unsigned_t = std::make_unsigned_t<T>;
			return (val < 0)
				? static_cast<unsigned_t>(-(val + 1)) + 1
				: static_cast<unsigned_t>(val);
		} else {
			return val;
		}
	}

	/**
	 * Simple function to check if the given string starts with a given set of characters.
	 */
	inline bool startsWith(std::string_view str, std::string_view search) {
		return str.rfind(search, 0) == 0;
	}
} // namespace fastgltf

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif
