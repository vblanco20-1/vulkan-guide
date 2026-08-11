#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include "gltf_path.hpp"

static auto getBufferData(const fastgltf::Buffer& buffer) {
	return std::visit(fastgltf::visitor {
		[](auto&) -> const std::byte* {
			assert(false);
			return nullptr;
		},
		[&](const fastgltf::sources::Array& vec) {
			return reinterpret_cast<const std::byte*>(vec.bytes.data());
		},
		[&](const fastgltf::sources::ByteView& bv) {
			return bv.bytes.data();
		},
	}, buffer.data);
}

TEST_CASE("Test data type conversion", "[gltf-tools]") {
	// normalized int-to-float and normalized float-to-int
	for (auto i = std::numeric_limits<std::int8_t>::min(); i < std::numeric_limits<std::int8_t>::max(); ++i) {
		auto converted = fastgltf::internal::convertComponent<float>(i, true);
		REQUIRE(converted == Catch::Approx(fastgltf::max<float>(i / 127.0f, -1)));
		REQUIRE(fastgltf::internal::convertComponent<std::int8_t>(converted, true) == std::round(converted * 127.0f));
	}
	for (auto i = std::numeric_limits<std::uint8_t>::min(); i < std::numeric_limits<std::uint8_t>::max(); ++i) {
		auto converted = fastgltf::internal::convertComponent<float>(i, true);
		REQUIRE(converted == Catch::Approx(i / 255.0f));
		REQUIRE(fastgltf::internal::convertComponent<std::uint8_t>(converted, true) == std::round(converted * 255.0f));
	}
	for (auto i = std::numeric_limits<std::int16_t>::min(); i < std::numeric_limits<std::int16_t>::max(); ++i) {
		auto converted = fastgltf::internal::convertComponent<float>(i, true);
		REQUIRE(converted == Catch::Approx(fastgltf::max<float>(i / 32767.0f, -1)));
		REQUIRE(fastgltf::internal::convertComponent<std::int16_t>(converted, true) == std::round(converted * 32767.0f));
	}
	for (auto i = std::numeric_limits<std::uint16_t>::min(); i < std::numeric_limits<std::uint16_t>::max(); ++i) {
		auto converted = fastgltf::internal::convertComponent<float>(i, true);
		REQUIRE(converted == Catch::Approx(i / 65535.0f));
		REQUIRE(fastgltf::internal::convertComponent<std::uint16_t>(converted, true) == std::round(converted * 65535.0f));
	}
}

TEST_CASE("Test little-endian correctness", "[gltf-tools]") {
    // The test here is merely to verify that the internal deserialization functions correctly treat
    // the input bytes as little-endian, regardless of system endianness.
    // This test is effectively useless on little endian systems, but it should still make sense to keep it.
    std::array<std::byte, 4> integer {{ std::byte(0x0A), std::byte(0x0B), std::byte(0x0C), std::byte(0x0D) }};
    auto deserialized = fastgltf::internal::deserializeComponent<std::uint32_t>(integer.data(), 0);
    REQUIRE(deserialized == 0x0D0C0B0A);
}

TEST_CASE("Test matrix data padding", "[gltf-tools]") {
    // First a case that doesn't require any padding
    std::array<std::uint16_t, 4> unpaddedMat2 {{
       1, 2,
       3, 4
    }};
    REQUIRE(fastgltf::getElementByteSize(fastgltf::AccessorType::Mat2, fastgltf::ComponentType::UnsignedShort) == unpaddedMat2.size() * sizeof(std::uint16_t));
    auto umat2 = fastgltf::internal::getAccessorElementAt<fastgltf::math::fmat2x2>(
            fastgltf::ComponentType::UnsignedShort,
            reinterpret_cast<const std::byte*>(unpaddedMat2.data()));
    REQUIRE(umat2[0] == fastgltf::math::fvec2(1, 2));
    REQUIRE(umat2[1] == fastgltf::math::fvec2(3, 4));

	for (std::size_t i = 0; i < fastgltf::getNumComponents(fastgltf::AccessorType::Mat2); ++i) {
		auto val = fastgltf::internal::getAccessorComponentAt<std::uint16_t>(
			fastgltf::ComponentType::UnsignedShort, fastgltf::AccessorType::Mat2, reinterpret_cast<const std::byte*>(unpaddedMat2.data()), i, false);
		REQUIRE(std::uint16_t(i + 1) == val);
	}

    // This will simulate a padded 2x2 matrix with the correct 4-byte padding per column
    std::array<std::uint8_t, 8> paddedMat2 {{
        1, 2, 0, 0,
        3, 4, 0, 0
    }};
    REQUIRE(fastgltf::getElementByteSize(fastgltf::AccessorType::Mat2, fastgltf::ComponentType::UnsignedByte) == paddedMat2.size());
    auto mat2 = fastgltf::internal::getAccessorElementAt<fastgltf::math::fmat2x2>(
            fastgltf::ComponentType::UnsignedByte,
            reinterpret_cast<const std::byte*>(paddedMat2.data()));
    REQUIRE(mat2[0] == fastgltf::math::fvec2(1, 2));
    REQUIRE(mat2[1] == fastgltf::math::fvec2(3, 4));

	for (std::size_t i = 0; i < fastgltf::getNumComponents(fastgltf::AccessorType::Mat2); ++i) {
		auto val = fastgltf::internal::getAccessorComponentAt<std::uint8_t>(
			fastgltf::ComponentType::UnsignedByte, fastgltf::AccessorType::Mat2, reinterpret_cast<const std::byte*>(paddedMat2.data()), i, false);
		REQUIRE(std::uint8_t(i + 1) == val);
	}

    std::array<std::uint8_t, 12> paddedMat3 {{
        1, 2, 3, 0,
        4, 5, 6, 0,
        7, 8, 9, 0
    }};
    REQUIRE(fastgltf::getElementByteSize(fastgltf::AccessorType::Mat3, fastgltf::ComponentType::UnsignedByte) == paddedMat3.size());
    auto mat3 = fastgltf::internal::getAccessorElementAt<fastgltf::math::fmat3x3>(
            fastgltf::ComponentType::UnsignedByte,
            reinterpret_cast<const std::byte*>(paddedMat3.data()));
    REQUIRE(mat3[0] == fastgltf::math::fvec3(1, 2, 3));
    REQUIRE(mat3[1] == fastgltf::math::fvec3(4, 5, 6));
    REQUIRE(mat3[2] == fastgltf::math::fvec3(7, 8, 9));

	for (std::size_t i = 0; i < fastgltf::getNumComponents(fastgltf::AccessorType::Mat3); ++i) {
		auto val = fastgltf::internal::getAccessorComponentAt<std::uint8_t>(
			fastgltf::ComponentType::UnsignedByte, fastgltf::AccessorType::Mat3, reinterpret_cast<const std::byte*>(paddedMat3.data()), i, false);
		REQUIRE(std::uint8_t(i + 1) == val);
	}

    // This now uses 16-bit shorts for the component types.
    std::array<std::uint16_t, 12> padded2BMat3 {{
        1, 2, 3, 0,
        4, 5, 6, 0,
        7, 8, 9, 0
    }};
    REQUIRE(fastgltf::getElementByteSize(fastgltf::AccessorType::Mat3, fastgltf::ComponentType::UnsignedShort) == paddedMat3.size() * sizeof(std::uint16_t));
    auto mat3_2 = fastgltf::internal::getAccessorElementAt<fastgltf::math::fmat3x3>(
            fastgltf::ComponentType::UnsignedShort,
            reinterpret_cast<const std::byte*>(padded2BMat3.data()));
    REQUIRE(mat3_2[0] == fastgltf::math::fvec3(1, 2, 3));
    REQUIRE(mat3_2[1] == fastgltf::math::fvec3(4, 5, 6));
    REQUIRE(mat3_2[2] == fastgltf::math::fvec3(7, 8, 9));

	for (std::size_t i = 0; i < fastgltf::getNumComponents(fastgltf::AccessorType::Mat3); ++i) {
		auto val = fastgltf::internal::getAccessorComponentAt<std::uint16_t>(
			fastgltf::ComponentType::UnsignedShort, fastgltf::AccessorType::Mat3, reinterpret_cast<const std::byte*>(padded2BMat3.data()), i, false);
		REQUIRE(std::uint16_t(i + 1) == val);
	}
}

TEST_CASE("Test matrix transpose", "[gltf-tools]") {
	// First a case that doesn't require any padding
	std::array<float, 4> mat2 {{
		1, 2,
		3, 4
	}};
	REQUIRE(fastgltf::getElementByteSize(fastgltf::AccessorType::Mat2, fastgltf::ComponentType::Float) == mat2.size() * sizeof(float));

	static constexpr std::array<float, 4> transposed {{
		1, 3, 2, 4
	}};
	for (std::size_t i = 0; i < 4; ++i) {
		REQUIRE(float(i + 1) == fastgltf::internal::getAccessorComponentAt<float>(
				fastgltf::ComponentType::Float, fastgltf::AccessorType::Mat2, reinterpret_cast<std::byte*>(mat2.data()), i, false, false));
		REQUIRE(transposed[i] == fastgltf::internal::getAccessorComponentAt<float>(
				fastgltf::ComponentType::Float, fastgltf::AccessorType::Mat2, reinterpret_cast<std::byte*>(mat2.data()), i, false, true));
	}
}

TEST_CASE("Test accessor", "[gltf-tools]") {
    auto lightsLamp = sampleAssets / "Models" / "LightsPunctualLamp" / "glTF";

	fastgltf::GltfFileStream jsonData(lightsLamp / "LightsPunctualLamp.gltf");
	REQUIRE(jsonData.isOpen());

    fastgltf::Parser parser(fastgltf::Extensions::KHR_lights_punctual);
    auto asset = parser.loadGltfJson(jsonData, lightsLamp, fastgltf::Options::LoadExternalBuffers,
								 fastgltf::Category::Buffers | fastgltf::Category::BufferViews | fastgltf::Category::Accessors);
    REQUIRE(asset.error() == fastgltf::Error::None);

    REQUIRE(asset->accessors.size() == 15);
    auto& accessors = asset->accessors;

    SECTION("getAccessorElement<std::uint16_t>") {
        auto& firstAccessor = accessors[0];
		REQUIRE(firstAccessor.type == fastgltf::AccessorType::Scalar);
		REQUIRE(firstAccessor.componentType == fastgltf::ComponentType::UnsignedShort);

		REQUIRE(firstAccessor.bufferViewIndex.has_value());
		auto& view = asset->bufferViews[*firstAccessor.bufferViewIndex];

		auto* bufferData = getBufferData(asset->buffers[view.bufferIndex]);
		REQUIRE(bufferData != nullptr);

		auto* checkData = reinterpret_cast<const std::uint16_t*>(bufferData + view.byteOffset
				+ firstAccessor.byteOffset);

		REQUIRE(*checkData == fastgltf::getAccessorElement<std::uint16_t>(asset.get(), firstAccessor, 0));
    }

	{
        auto& secondAccessor = accessors[1];
		REQUIRE(secondAccessor.type == fastgltf::AccessorType::Vec3);
		REQUIRE(secondAccessor.componentType == fastgltf::ComponentType::Float);

		REQUIRE(secondAccessor.bufferViewIndex.has_value());
		auto& view = asset->bufferViews[*secondAccessor.bufferViewIndex];

		auto* bufferData = getBufferData(asset->buffers[view.bufferIndex]);
		REQUIRE(bufferData != nullptr);

		auto* checkData = reinterpret_cast<const fastgltf::math::fvec3*>(bufferData + view.byteOffset
				+ secondAccessor.byteOffset);

		SECTION("getAccessorElement<fastgltf::math::fvec3>") {
			REQUIRE(*checkData == fastgltf::getAccessorElement<fastgltf::math::fvec3>(asset.get(), secondAccessor, 0));
		}

		SECTION("iterateAccessor") {
			auto dstCopy = std::make_unique<fastgltf::math::fvec3[]>(secondAccessor.count);
			std::size_t i = 0;

			fastgltf::iterateAccessor<fastgltf::math::fvec3>(asset.get(), secondAccessor, [&](auto&& v3) {
				dstCopy[i++] = std::forward<fastgltf::math::fvec3>(v3);
			});

			REQUIRE(std::memcmp(dstCopy.get(), checkData, secondAccessor.count * sizeof(fastgltf::math::fvec3)) == 0);
		}

		SECTION("copyFromAccessor") {
			auto dstCopy = std::make_unique<fastgltf::math::fvec3[]>(secondAccessor.count);
			fastgltf::copyFromAccessor<fastgltf::math::fvec3>(asset.get(), secondAccessor, dstCopy.get());
			REQUIRE(std::memcmp(dstCopy.get(), checkData, secondAccessor.count * sizeof(fastgltf::math::fvec3)) == 0);
		}

		SECTION("Iterator test") {
			auto dstCopy = std::make_unique<fastgltf::math::fvec3[]>(secondAccessor.count);
			auto accessor = fastgltf::iterateAccessor<fastgltf::math::fvec3>(asset.get(), secondAccessor);
			for (auto it = accessor.begin(); it != accessor.end(); ++it) {
				dstCopy[std::distance(accessor.begin(), it)] = *it;
			}
			REQUIRE(std::memcmp(dstCopy.get(), checkData, secondAccessor.count * sizeof(fastgltf::math::fvec3)) == 0);
		}

		SECTION("copyComponentsFromAccessor<float>") {
			auto componentCount = fastgltf::getNumComponents(secondAccessor.type);
			auto dstCopy = std::make_unique<float[]>(secondAccessor.count * componentCount);
			fastgltf::copyComponentsFromAccessor<float>(asset.get(), secondAccessor, dstCopy.get());
			fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset.get(), secondAccessor, [&](auto&& p1, std::size_t idx) {
				auto p2 = fastgltf::math::fvec3(dstCopy.get()[idx * componentCount + 0],
				                                dstCopy.get()[idx * componentCount + 1],
				                                dstCopy.get()[idx * componentCount + 2]);
				REQUIRE(p1 == p2);
			});
		}
	}
}

TEST_CASE("Test sparse accessor", "[gltf-tools]") {
    auto simpleSparseAccessor = sampleAssets / "Models" / "SimpleSparseAccessor" / "glTF";

	fastgltf::GltfFileStream jsonData(simpleSparseAccessor / "SimpleSparseAccessor.gltf");
	REQUIRE(jsonData.isOpen());

    fastgltf::Parser parser;
    auto asset = parser.loadGltfJson(jsonData, simpleSparseAccessor, fastgltf::Options::LoadExternalBuffers,
								 fastgltf::Category::Buffers | fastgltf::Category::BufferViews | fastgltf::Category::Accessors);
    REQUIRE(asset.error() == fastgltf::Error::None);

    REQUIRE(asset->accessors.size() == 2);
    REQUIRE(!asset->accessors[0].sparse.has_value());
    REQUIRE(asset->accessors[1].sparse.has_value());
    auto& sparse = asset->accessors[1].sparse.value();
    REQUIRE(sparse.count == 3);
    REQUIRE(sparse.indicesBufferView == 2);
    REQUIRE(sparse.indicesByteOffset == 0);
    REQUIRE(sparse.valuesBufferView == 3);
    REQUIRE(sparse.valuesByteOffset == 0);
    REQUIRE(sparse.indexComponentType == fastgltf::ComponentType::UnsignedShort);

	auto& secondAccessor = asset->accessors[1];
	auto& viewIndices = asset->bufferViews[secondAccessor.sparse->indicesBufferView];
	auto& viewValues = asset->bufferViews[secondAccessor.sparse->valuesBufferView];

	auto& viewData = asset->bufferViews[*secondAccessor.bufferViewIndex];
	auto* bufferData = getBufferData(asset->buffers[viewData.bufferIndex]) + viewData.byteOffset
			+ secondAccessor.byteOffset;
	auto dataStride = viewData.byteStride ? *viewData.byteStride
			: fastgltf::getElementByteSize(secondAccessor.type, secondAccessor.componentType);

	auto* dataIndices = reinterpret_cast<const std::uint16_t*>(getBufferData(asset->buffers[viewIndices.bufferIndex])
			+ viewIndices.byteOffset + secondAccessor.sparse->indicesByteOffset);
	auto* dataValues = reinterpret_cast<const fastgltf::math::fvec3*>(getBufferData(asset->buffers[viewValues.bufferIndex])
			+ viewValues.byteOffset + secondAccessor.sparse->valuesByteOffset);

	auto checkValues = std::make_unique<fastgltf::math::fvec3[]>(secondAccessor.count);

	for (std::size_t i = 0, sparseIndex = 0; i < secondAccessor.count; ++i) {
		if (sparseIndex < secondAccessor.sparse->count && dataIndices[sparseIndex] == i) {
			checkValues[i] = dataValues[sparseIndex];
			++sparseIndex;
		} else {
			checkValues[i] = *reinterpret_cast<const fastgltf::math::fvec3*>(bufferData + dataStride * i);
		}
	}

	SECTION("getAccessorElement") {
		for (std::size_t i = 0; i < secondAccessor.count; ++i) {
			REQUIRE(checkValues[i] == fastgltf::getAccessorElement<fastgltf::math::fvec3>(asset.get(), secondAccessor, i));
		}
	}

	SECTION("iterateAccessor") {
		auto dstCopy = std::make_unique<fastgltf::math::fvec3[]>(secondAccessor.count);
		std::size_t i = 0;

		fastgltf::iterateAccessor<fastgltf::math::fvec3>(asset.get(), secondAccessor, [&](auto&& v3) {
			dstCopy[i++] = std::forward<fastgltf::math::fvec3>(v3);
		});

		REQUIRE(std::memcmp(dstCopy.get(), checkValues.get(), secondAccessor.count * sizeof(fastgltf::math::fvec3)) == 0);
	}

	SECTION("iterateAccessor with idx") {
		auto dstCopy = std::make_unique<fastgltf::math::fvec3[]>(secondAccessor.count);

		fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset.get(), secondAccessor, [&](auto&& v3, std::size_t i) {
			dstCopy[i] = std::forward<fastgltf::math::fvec3>(v3);
		});

		REQUIRE(std::memcmp(dstCopy.get(), checkValues.get(), secondAccessor.count * sizeof(fastgltf::math::fvec3)) == 0);
	}

	SECTION("copyFromAccessor") {
		auto dstCopy = std::make_unique<fastgltf::math::fvec3[]>(secondAccessor.count);
		fastgltf::copyFromAccessor<fastgltf::math::fvec3>(asset.get(), secondAccessor, dstCopy.get());
		REQUIRE(std::memcmp(dstCopy.get(), checkValues.get(), secondAccessor.count * sizeof(fastgltf::math::fvec3)) == 0);
	}

	SECTION("Iterator test") {
		auto dstCopy = std::make_unique<fastgltf::math::fvec3[]>(secondAccessor.count);
		auto accessor = fastgltf::iterateAccessor<fastgltf::math::fvec3>(asset.get(), secondAccessor);
		for (auto it = accessor.begin(); it != accessor.end(); ++it) {
			dstCopy[std::distance(accessor.begin(), it)] = *it;
		}
		REQUIRE(std::memcmp(dstCopy.get(), checkValues.get(), secondAccessor.count * sizeof(fastgltf::math::fvec3)) == 0);
	}
}
