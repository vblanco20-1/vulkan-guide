#include <fstream>

#include <catch2/catch_test_macros.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include "gltf_path.hpp"

TEST_CASE("Load basic GLB file", "[gltf-loader]") {
    auto folder = sampleAssets / "Models" / "Box" / "glTF-Binary";
	auto jsonData = fastgltf::GltfDataBuffer::FromPath(folder / "Box.glb");
	REQUIRE(jsonData.error() == fastgltf::Error::None);

	fastgltf::Parser parser;
    SECTION("Load basic Box.glb") {
        auto asset = parser.loadGltfBinary(jsonData.get(), folder, fastgltf::Options::None, fastgltf::Category::Buffers);
        REQUIRE(asset.error() == fastgltf::Error::None);
		REQUIRE(fastgltf::validate(asset.get()) == fastgltf::Error::None);

        REQUIRE(asset->buffers.size() == 1);

        auto& buffer = asset->buffers.front();
		auto* array = std::get_if<fastgltf::sources::Array>(&buffer.data);
        REQUIRE(array != nullptr);
		REQUIRE(array->bytes.size() == 1664 - 1016);
    }

    SECTION("Load basic Box.glb and load buffers") {
        auto asset = parser.loadGltfBinary(jsonData.get(), folder, fastgltf::Options::None, fastgltf::Category::Buffers);
        REQUIRE(asset.error() == fastgltf::Error::None);
		REQUIRE(fastgltf::validate(asset.get()) == fastgltf::Error::None);

        REQUIRE(asset->buffers.size() == 1);

        auto& buffer = asset->buffers.front();
        auto* bufferVector = std::get_if<fastgltf::sources::Array>(&buffer.data);
        REQUIRE(bufferVector != nullptr);
        REQUIRE(!bufferVector->bytes.empty());
        REQUIRE(static_cast<uint64_t>(bufferVector->bytes.size() - buffer.byteLength) < 3);
    }

    SECTION("Load GLB by bytes") {
        std::ifstream file(folder / "Box.glb", std::ios::binary | std::ios::ate);
        auto length = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ifstream::beg);
        std::vector<uint8_t> bytes(length);
        file.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(length));

		auto byteBuffer = fastgltf::GltfDataBuffer::FromBytes(
				reinterpret_cast<const std::byte*>(bytes.data()), length);
		REQUIRE(byteBuffer.error() == fastgltf::Error::None);

        auto asset = parser.loadGltfBinary(byteBuffer.get(), folder, fastgltf::Options::None, fastgltf::Category::Buffers);
        REQUIRE(asset.error() == fastgltf::Error::None);
    }
}
