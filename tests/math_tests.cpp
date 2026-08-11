#include <random>

#include <fastgltf/math.hpp>
#include <fastgltf/core.hpp>
#include "gltf_path.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <ostream>

// We need these operator<< overloads for vector types to get proper output from Catch2 on assertions.
template <std::size_t N>
std::ostream& operator <<(std::ostream& os, const fastgltf::math::vec<float, N>& value) {
	os << "vec<float," << N << ">(" << std::setprecision(std::numeric_limits<float>::digits10);
	for (std::size_t i = 0; i < N; ++i) {
		os << value[i];
		if (i + 1 != N)
			os << ',';
	}
    os << ')';
    return os;
}

template <std::size_t N, std::size_t M>
std::ostream& operator <<(std::ostream& os, const fastgltf::math::mat<float, N, M>& value) {
	os << "mat<float," << N << ',' << M << ">(" << std::setprecision(std::numeric_limits<float>::digits10);
	for (std::size_t i = 0; i < M; ++i) {
		os << '[';
		for (std::size_t j = 0; j < N; ++j) {
			os << value.col(i)[j];
			if (j + 1 != N)
				os << ',';
		}
		os << ']';
		if (i + 1 != M)
			os << ',';
	}
	os << ')';
	return os;
}

template <glm::length_t N>
std::ostream& operator <<(std::ostream& os, const glm::vec<N, float>& value) {
	os << "glm::vec<" << N << ",float>(" << std::setprecision(std::numeric_limits<float>::digits10);
	for (glm::length_t i = 0; i < N; ++i) {
		os << value[i];
		if (i + 1 != N)
			os << ',';
	}
    os << ')';
    return os;
}

std::ostream& operator <<(std::ostream& os, const glm::quat& value) {
	os << "glm::quat<float>(" << std::setprecision(std::numeric_limits<float>::digits10);
	for (glm::length_t i = 0; i < glm::quat::length(); ++i) {
		os << value[i];
		if (i + 1 != glm::quat::length())
			os << ',';
	}
	os << ')';
	return os;
}

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Vector initialization", "[maths]") {
	SECTION("Default init") {
		fastgltf::math::fvec4 vector {};
		REQUIRE(vector.x() == 0.0f);
		REQUIRE(vector.y() == 0.0f);
		REQUIRE(vector.z() == 0.0f);
		REQUIRE(vector.w() == 0.0f);
	}

	static constexpr auto sentinel = 1.5f;
	SECTION("Single value init") {
		fastgltf::math::fvec4 vector(sentinel);
		REQUIRE(vector.x() == sentinel);
		REQUIRE(vector.y() == sentinel);
		REQUIRE(vector.z() == sentinel);
		REQUIRE(vector.w() == sentinel);
	}
	SECTION("Multi-value init with parameter pack") {
		fastgltf::math::fvec2 vec2(sentinel, sentinel + 1.f);
		REQUIRE(vec2.x() == sentinel);
		REQUIRE(vec2.y() == sentinel + 1.f);

		fastgltf::math::fvec4 vec4(sentinel, sentinel + 1.f, sentinel + 2.f, sentinel + 3.f);
		REQUIRE(vec4.x() == sentinel);
		REQUIRE(vec4.y() == sentinel + 1.f);
		REQUIRE(vec4.z() == sentinel + 2.f);
		REQUIRE(vec4.w() == sentinel + 3.f);
	}
}

TEST_CASE("Matrix initialization", "[maths]") {
	SECTION("Default init") {
		fastgltf::math::fmat4x4 matrix;
		REQUIRE(matrix.col(0)[0] == 1.f);
		REQUIRE(matrix.col(1)[1] == 1.f);
		REQUIRE(matrix.col(2)[2] == 1.f);
		REQUIRE(matrix.col(3)[3] == 1.f);

		REQUIRE(matrix.row(0)[0] == 1.f);
		REQUIRE(matrix.row(1)[1] == 1.f);
		REQUIRE(matrix.row(2)[2] == 1.f);
		REQUIRE(matrix.row(3)[3] == 1.f);

		// Make sure the rest is zeroed.
		REQUIRE(matrix.col(0)[1] == 0.f);
		REQUIRE(matrix.col(0)[2] == 0.f);
		REQUIRE(matrix.col(0)[3] == 0.f);
		REQUIRE(matrix.col(1)[0] == 0.f);
		REQUIRE(matrix.col(1)[2] == 0.f);
		REQUIRE(matrix.col(1)[3] == 0.f);
		REQUIRE(matrix.col(2)[0] == 0.f);
		REQUIRE(matrix.col(2)[1] == 0.f);
		REQUIRE(matrix.col(2)[3] == 0.f);
		REQUIRE(matrix.col(3)[0] == 0.f);
		REQUIRE(matrix.col(3)[1] == 0.f);
		REQUIRE(matrix.col(3)[2] == 0.f);
	}

	SECTION("Multi-value init with parameter pack") {
		static constexpr auto sentinel2 = fastgltf::math::fvec2(1, 2);
		fastgltf::math::fmat2x2 mat2(sentinel2, sentinel2 + 1.f);
		REQUIRE(mat2.col(0).size() == 2);
		REQUIRE(mat2.col(0)[1] == 2.f);
		REQUIRE(mat2.col(0) == sentinel2);
		REQUIRE(mat2.col(1) == sentinel2 + 1.f);

		static constexpr auto sentinel = fastgltf::math::fvec4(1, 2, 3, 4);
		fastgltf::math::fmat4x4 mat4(sentinel, sentinel + 1.f, sentinel + 2.f, sentinel + 3.f);
		REQUIRE(mat4.col(0) == sentinel);
		REQUIRE(mat4.col(1) == sentinel + 1.f);
		REQUIRE(mat4.col(2) == sentinel + 2.f);
		REQUIRE(mat4.col(3) == sentinel + 3.f);
	}

	SECTION("Conversion") {
		const fastgltf::math::fmat2x2 mat2(2.f, 3.f, 4.f, 5.f);
		fastgltf::math::fmat3x3 mat3(mat2);
		REQUIRE(mat3.col(0) == fastgltf::math::fvec3(2.f, 3.f, 0.f));
		REQUIRE(mat3.col(1) == fastgltf::math::fvec3(4.f, 5.f, 0.f));
		REQUIRE(mat3.col(2) == fastgltf::math::fvec3(0.f, 0.f, 1.f));
		REQUIRE(mat3.row(2) == fastgltf::math::fvec3(0.f, 0.f, 1.f));

		fastgltf::math::fmat2x2 loop(mat3);
		REQUIRE(loop.col(0) == fastgltf::math::fvec2(2.f, 3.f));
		REQUIRE(loop.col(1) == fastgltf::math::fvec2(4.f, 5.f));
	}
}

TEST_CASE("Vector operations", "[maths]") {
	SECTION("Addition") {
		auto vres = fastgltf::math::u32vec2(3, 4) + fastgltf::math::u32vec2(6, 7);
		REQUIRE(vres == fastgltf::math::u32vec2(9, 11));

		auto sres = fastgltf::math::u32vec2(3, 4) + std::uint32_t(5);
		REQUIRE(sres == fastgltf::math::u32vec2(8, 9));
	}
	SECTION("Multiplication") {
		auto sres = fastgltf::math::fvec2(3, 4) * 2;
		REQUIRE(sres == fastgltf::math::fvec2(6, 8));

		auto vres = fastgltf::math::fvec2(3, 4) * fastgltf::math::fvec2(4, 2);
		REQUIRE(vres == fastgltf::math::fvec2(12, 8));
	}

	SECTION("Length") {
		fastgltf::math::fvec2 v(3, 4);
		REQUIRE(length(v) == 5);
	}

	SECTION("Normalize") {
		fastgltf::math::fvec2 v(3, 4);
		REQUIRE(normalize(v) == fastgltf::math::fvec2(0.6f, 0.8f));
	}

	SECTION("Dot product") {
		fastgltf::math::fvec2 a2(4, 5);
		fastgltf::math::fvec2 b2(2, 3);
		REQUIRE(dot(a2, b2) == 4 * 2 + 5 * 3);

		fastgltf::math::fvec3 a3(4, 5, 6);
		fastgltf::math::fvec3 b3(2, 3, 4);
		REQUIRE(dot(a3, b3) == 4 * 2 + 5 * 3 + 6 * 4);
	}

	SECTION("Cross product") {
		fastgltf::math::fvec3 a(2, 2, -1);
		fastgltf::math::fvec3 b(3, -1, 2);
		REQUIRE(cross(a, b) == fastgltf::math::fvec3(3, -7, -8));
	}
}

TEST_CASE("Matrix operations", "[maths]") {
	SECTION("Transposing") {
		fastgltf::math::fmat<3, 2> m(
			fastgltf::math::fvec3(1, 2, 3),
			fastgltf::math::fvec3(4, 1, 5)
		);
		REQUIRE(m.row(0) == fastgltf::math::fvec2(1, 4));
		REQUIRE(m.row(1) == fastgltf::math::fvec2(2, 1));
		REQUIRE(m.row(2) == fastgltf::math::fvec2(3, 5));

		auto t = transpose(m);
		REQUIRE(t.col(0) == fastgltf::math::fvec2(1, 4));
		REQUIRE(t.col(1) == fastgltf::math::fvec2(2, 1));
		REQUIRE(t.col(2) == fastgltf::math::fvec2(3, 5));
	}

	SECTION("Determinant", "[maths]") {
		fastgltf::math::fmat3x3 m(
			fastgltf::math::fvec3(7, 0, -3),
			fastgltf::math::fvec3(2, 3, 4),
			fastgltf::math::fvec3(1, -1, -2));
		REQUIRE(determinant(m) == 1);
	}
}

TEST_CASE("Test TRS parsing and optional decomposition", "[maths]") {
	SECTION("Test decomposition on glTF asset") {
		fastgltf::GltfFileStream jsonData(path / "transform_matrices.gltf");
		REQUIRE(jsonData.isOpen());

		// Parse once without decomposing, once with decomposing the matrix.
		fastgltf::Parser parser;
		auto assetWithMatrix = parser.loadGltfJson(jsonData, path, fastgltf::Options::None, fastgltf::Category::Nodes | fastgltf::Category::Cameras);
		REQUIRE(assetWithMatrix.error() == fastgltf::Error::None);
		REQUIRE(fastgltf::validate(assetWithMatrix.get()) == fastgltf::Error::None);

		auto assetDecomposed = parser.loadGltfJson(jsonData, path, fastgltf::Options::DecomposeNodeMatrices, fastgltf::Category::Nodes | fastgltf::Category::Cameras);
		REQUIRE(assetDecomposed.error() == fastgltf::Error::None);
		REQUIRE(fastgltf::validate(assetDecomposed.get()) == fastgltf::Error::None);

		REQUIRE(assetWithMatrix->cameras.size() == 1);
		REQUIRE(assetDecomposed->cameras.size() == 1);
		REQUIRE(assetWithMatrix->nodes.size() == 2);
		REQUIRE(assetDecomposed->nodes.size() == 2);
		REQUIRE(std::holds_alternative<fastgltf::math::fmat4x4>(assetWithMatrix->nodes.back().transform));
		REQUIRE(std::holds_alternative<fastgltf::TRS>(assetDecomposed->nodes.back().transform));

		// Get the TRS components from the first node and use them as the test data for decomposing.
		const auto* pDefaultTRS = std::get_if<fastgltf::TRS>(&assetWithMatrix->nodes.front().transform);
		REQUIRE(pDefaultTRS != nullptr);
		auto translation = glm::make_vec3(pDefaultTRS->translation.data());
		auto rotation = glm::make_quat(pDefaultTRS->rotation.data());
		auto scale = glm::make_vec3(pDefaultTRS->scale.data());
		auto rotationMatrix = glm::toMat4(rotation);
		auto transform = glm::translate(glm::mat4(1.0f), translation) * rotationMatrix * glm::scale(glm::mat4(1.0f), scale);

		// Check if the parsed matrix is correct.
		const auto* pMatrix = std::get_if<fastgltf::math::fmat4x4>(&assetWithMatrix->nodes.back().transform);
		REQUIRE(pMatrix != nullptr);
		REQUIRE(glm::make_mat4x4(&pMatrix->col(0)[0]) == transform);

		// Check if the decomposed components equal the original components.
		const auto* pDecomposedTRS = std::get_if<fastgltf::TRS>(&assetDecomposed->nodes.back().transform);
		REQUIRE(glm::make_vec3(pDecomposedTRS->translation.data()) == translation);
		REQUIRE(glm::make_quat(pDecomposedTRS->rotation.data()) == rotation);
		REQUIRE(glm::make_vec3(pDecomposedTRS->scale.data()) == scale);
	}

	SECTION("Test decomposition against glm decomposition") {
		// Some random complex transform matrix from one of the glTF sample models.
		const fastgltf::math::fmat4x4 matrix(
			fastgltf::math::fvec4(-0.4234085381031037F, -0.9059388637542724F, -7.575183536001616e-11F, 0.0F),
			fastgltf::math::fvec4(-0.9059388637542724F, 0.4234085381031037F, -4.821281221478735e-11F, 0.0F),
			fastgltf::math::fvec4(7.575183536001616e-11F, 4.821281221478735e-11F, -1.0F, 0.0F),
			fastgltf::math::fvec4(-90.59386444091796F, -24.379817962646489F, -40.05522918701172F, 1.0F)
		);

		fastgltf::math::fvec3 translation, scale;
		fastgltf::math::fquat rotation;
		fastgltf::math::decomposeTransformMatrix(matrix, scale, rotation, translation);

		auto glmMatrix = glm::make_mat4x4(&matrix.col(0)[0]);
		glm::vec3 glmScale, glmTranslation, glmSkew;
		glm::quat glmRotation;
		glm::vec4 glmPerspective;
		glm::decompose(glmMatrix, glmScale, glmRotation, glmTranslation, glmSkew, glmPerspective);

		// I use glm::epsilon<float>() * 10 here because some matrices I tested this with resulted
		// in an error margin greater than the normal epsilon value. I will investigate this in the
		// future, but I suspect using double in the decompose functions should help mitigate most
		// of it.
		REQUIRE(glm::make_vec3(translation.data()) == glmTranslation);
		REQUIRE(glm::all(glm::epsilonEqual(glm::make_quat(rotation.data()), glmRotation, glm::epsilon<float>() * 10)));
		REQUIRE(glm::all(glm::epsilonEqual(glm::make_vec3(scale.data()), glmScale, glm::epsilon<float>())));
	}
}
