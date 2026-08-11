#include <limits>
#include <random>
#include <string>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include <fastgltf/util.hpp>

TEST_CASE("Test all variants of CRC32-C hashing", "[gltf-loader]") {
	// TODO: Determine SSE4.2 support here.
	for (std::size_t i = 0; i < 256; ++i) {
		// Generate a random string up to 256 chars long.
		static constexpr std::string_view chars =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		static std::mt19937 rng(std::random_device{}());
		static std::uniform_int_distribution<std::string::size_type> pick(0, chars.size() - 1);
		std::string str(i, '\0');
		for (std::size_t j = 0; j < i; ++j)
			str[j] = chars[pick(rng)];

		// We'll try and test if the hardware accelerated version generates the same, correct results.
#if defined(FASTGLTF_IS_X86)
		REQUIRE(fastgltf::crc32c(str) == fastgltf::sse_crc32c(str));
#elif defined(FASTGLTF_IS_A64)
		REQUIRE(fastgltf::crc32c(str) == fastgltf::armv8_crc32c(str));
#endif
	}
}

TEST_CASE("Verify uabs functionality", "[util-tests]") {
	REQUIRE(fastgltf::uabs(-5) == 5);
	REQUIRE(fastgltf::uabs(-5) == fastgltf::uabs(5));
	REQUIRE(fastgltf::uabs(0) == 0);
	REQUIRE(fastgltf::uabs(0) == 0);

	REQUIRE(std::is_unsigned_v<decltype(fastgltf::uabs(-5))>);

	// The issue with 2's complementary not being able to represent INT_MIN is fixed with uabs.
	REQUIRE(fastgltf::uabs(std::numeric_limits<int>::min()) == 2147483648);
	REQUIRE(fastgltf::uabs(std::numeric_limits<int>::min() + 1) == 2147483647);
	REQUIRE(fastgltf::uabs(std::numeric_limits<int>::max()) == 2147483647);
}
