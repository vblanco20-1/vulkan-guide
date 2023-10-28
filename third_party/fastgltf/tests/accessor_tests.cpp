#include <catch2/catch_test_macros.hpp>

#include <glm/vec3.hpp>

#include <fastgltf/parser.hpp>
#include <fastgltf/tools.hpp>
#include "gltf_path.hpp"

template<>
struct fastgltf::ElementTraits<glm::vec3> : fastgltf::ElementTraitsBase<glm::vec3, AccessorType::Vec3, float> {};

static const std::byte* getBufferData(const fastgltf::Buffer& buffer) {
	const std::byte* result = nullptr;

	std::visit(fastgltf::visitor {
		[](auto&) {},
		[&](const fastgltf::sources::Vector& vec) {
			result = reinterpret_cast<const std::byte*>(vec.bytes.data());
		},
		[&](const fastgltf::sources::ByteView& bv) {
			result = bv.bytes.data();
		},
	}, buffer.data);
	
	return result;
}

TEST_CASE("Test accessor", "[gltf-tools]") {
    auto lightsLamp = sampleModels / "2.0" / "LightsPunctualLamp" / "glTF";
    fastgltf::GltfDataBuffer jsonData;
    REQUIRE(jsonData.loadFromFile(lightsLamp / "LightsPunctualLamp.gltf"));

    fastgltf::Parser parser(fastgltf::Extensions::KHR_lights_punctual);
    auto asset = parser.loadGLTF(&jsonData, lightsLamp, fastgltf::Options::LoadExternalBuffers,
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

		auto* checkData = reinterpret_cast<const glm::vec3*>(bufferData + view.byteOffset
				+ secondAccessor.byteOffset);

		SECTION("getAccessorElement<glm::vec3>") {
			REQUIRE(*checkData == fastgltf::getAccessorElement<glm::vec3>(asset.get(), secondAccessor, 0));
		}

		SECTION("iterateAccessor") {
			auto dstCopy = std::make_unique<glm::vec3[]>(secondAccessor.count);
			std::size_t i = 0;

			fastgltf::iterateAccessor<glm::vec3>(asset.get(), secondAccessor, [&](auto&& v3) {
				dstCopy[i++] = std::forward<glm::vec3>(v3);
			});

			REQUIRE(std::memcmp(dstCopy.get(), checkData, secondAccessor.count * sizeof(glm::vec3)) == 0);
		}

		SECTION("copyFromAccessor") {
			auto dstCopy = std::make_unique<glm::vec3[]>(secondAccessor.count);
			fastgltf::copyFromAccessor<glm::vec3>(asset.get(), secondAccessor, dstCopy.get());
			REQUIRE(std::memcmp(dstCopy.get(), checkData, secondAccessor.count * sizeof(glm::vec3)) == 0);
		}

		SECTION("Iterator test") {
			auto dstCopy = std::make_unique<glm::vec3[]>(secondAccessor.count);
			auto accessor = fastgltf::iterateAccessor<glm::vec3>(asset.get(), secondAccessor);
			for (auto it = accessor.begin(); it != accessor.end(); ++it) {
				dstCopy[std::distance(accessor.begin(), it)] = *it;
			}
			REQUIRE(std::memcmp(dstCopy.get(), checkData, secondAccessor.count * sizeof(glm::vec3)) == 0);
		}
	}
}

TEST_CASE("Test sparse accessor", "[gltf-tools]") {
    auto simpleSparseAccessor = sampleModels / "2.0" / "SimpleSparseAccessor" / "glTF";
    auto jsonData = std::make_unique<fastgltf::GltfDataBuffer>();
    REQUIRE(jsonData->loadFromFile(simpleSparseAccessor / "SimpleSparseAccessor.gltf"));

    fastgltf::Parser parser;
    auto asset = parser.loadGLTF(jsonData.get(), simpleSparseAccessor, fastgltf::Options::LoadExternalBuffers,
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
	auto* dataValues = reinterpret_cast<const glm::vec3*>(getBufferData(asset->buffers[viewValues.bufferIndex])
			+ viewValues.byteOffset + secondAccessor.sparse->valuesByteOffset);

	auto checkValues = std::make_unique<glm::vec3[]>(secondAccessor.count);

	for (std::size_t i = 0, sparseIndex = 0; i < secondAccessor.count; ++i) {
		if (sparseIndex < secondAccessor.sparse->count && dataIndices[sparseIndex] == i) {
			checkValues[i] = dataValues[sparseIndex];
			++sparseIndex;
		} else {
			checkValues[i] = *reinterpret_cast<const glm::vec3*>(bufferData + dataStride * i);
		}
	}

	SECTION("getAccessorElement") {
		for (std::size_t i = 0; i < secondAccessor.count; ++i) {
			REQUIRE(checkValues[i] == fastgltf::getAccessorElement<glm::vec3>(asset.get(), secondAccessor, i));
		}
	}

	SECTION("iterateAccessor") {
		auto dstCopy = std::make_unique<glm::vec3[]>(secondAccessor.count);
		std::size_t i = 0;

		fastgltf::iterateAccessor<glm::vec3>(asset.get(), secondAccessor, [&](auto&& v3) {
			dstCopy[i++] = std::forward<glm::vec3>(v3);
		});

		REQUIRE(std::memcmp(dstCopy.get(), checkValues.get(), secondAccessor.count * sizeof(glm::vec3)) == 0);
	}

	SECTION("iterateAccessor with idx") {
		auto dstCopy = std::make_unique<glm::vec3[]>(secondAccessor.count);

		fastgltf::iterateAccessorWithIndex<glm::vec3>(asset.get(), secondAccessor, [&](auto&& v3, std::size_t i) {
			dstCopy[i] = std::forward<glm::vec3>(v3);
		});

		REQUIRE(std::memcmp(dstCopy.get(), checkValues.get(), secondAccessor.count * sizeof(glm::vec3)) == 0);
	}

	SECTION("copyFromAccessor") {
		auto dstCopy = std::make_unique<glm::vec3[]>(secondAccessor.count);
		fastgltf::copyFromAccessor<glm::vec3>(asset.get(), secondAccessor, dstCopy.get());
		REQUIRE(std::memcmp(dstCopy.get(), checkValues.get(), secondAccessor.count * sizeof(glm::vec3)) == 0);
	}

	SECTION("Iterator test") {
		auto dstCopy = std::make_unique<glm::vec3[]>(secondAccessor.count);
		auto accessor = fastgltf::iterateAccessor<glm::vec3>(asset.get(), secondAccessor);
		for (auto it = accessor.begin(); it != accessor.end(); ++it) {
			dstCopy[std::distance(accessor.begin(), it)] = *it;
		}
		REQUIRE(std::memcmp(dstCopy.get(), checkValues.get(), secondAccessor.count * sizeof(glm::vec3)) == 0);
	}
}
