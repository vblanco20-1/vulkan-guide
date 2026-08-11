#include <catch2/catch_test_macros.hpp>

#include <fastgltf/types.hpp>

TEST_CASE("Test basic Optional interface", "[optional-tests]") {
	// We have no specialization for std::uint32_t, and therefore this is just
	// a std::optional with a bool field padded by 3 bytes.
	fastgltf::Optional<std::uint32_t> optional;
	static_assert(sizeof(optional) > sizeof(std::uint32_t));
}

TEST_CASE("Test Optional float specialization", "[optional-tests]") {
	fastgltf::Optional<double> doptional;
	static_assert((std::numeric_limits<double>::is_iec559 && sizeof(doptional) == sizeof(double)) || !std::numeric_limits<double>::is_iec559);

	fastgltf::Optional<float> foptional;
	static_assert((std::numeric_limits<float>::is_iec559 && sizeof(foptional) == sizeof(float)) || !std::numeric_limits<float>::is_iec559);
}
