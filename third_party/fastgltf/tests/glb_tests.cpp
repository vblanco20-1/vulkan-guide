#include <fstream>

#include <catch2/catch_test_macros.hpp>

#include <fastgltf/parser.hpp>
#include <fastgltf/types.hpp>
#include "gltf_path.hpp"

TEST_CASE("Load basic GLB file", "[gltf-loader]") {
    fastgltf::Parser parser;
    auto folder = sampleModels / "2.0" / "Box" / "glTF-Binary";
    fastgltf::GltfDataBuffer jsonData;
    REQUIRE(jsonData.loadFromFile(folder / "Box.glb"));

    SECTION("Load basic Box.glb") {
        auto asset = parser.loadBinaryGLTF(&jsonData, folder, fastgltf::Options::None, fastgltf::Category::Buffers);
        REQUIRE(asset.error() == fastgltf::Error::None);
		REQUIRE(fastgltf::validate(asset.get()) == fastgltf::Error::None);

        REQUIRE(asset->buffers.size() == 1);

        auto& buffer = asset->buffers.front();
        auto* bufferView = std::get_if<fastgltf::sources::ByteView>(&buffer.data);
        REQUIRE(bufferView != nullptr);
        auto jsonSpan = fastgltf::span<std::byte>(jsonData);
        REQUIRE(bufferView->bytes.data() - jsonSpan.data() == 1016);
        REQUIRE(jsonSpan.size() == 1664);
    }

    SECTION("Load basic Box.glb and load buffers") {
        auto asset = parser.loadBinaryGLTF(&jsonData, folder, fastgltf::Options::LoadGLBBuffers, fastgltf::Category::Buffers);
        REQUIRE(asset.error() == fastgltf::Error::None);
		REQUIRE(fastgltf::validate(asset.get()) == fastgltf::Error::None);

        REQUIRE(asset->buffers.size() == 1);

        auto& buffer = asset->buffers.front();
        auto* bufferVector = std::get_if<fastgltf::sources::Vector>(&buffer.data);
        REQUIRE(bufferVector != nullptr);
        REQUIRE(!bufferVector->bytes.empty());
        REQUIRE(static_cast<uint64_t>(bufferVector->bytes.size() - buffer.byteLength) < 3);
    }

    SECTION("Load GLB by bytes") {
        std::ifstream file(folder / "Box.glb", std::ios::binary | std::ios::ate);
        auto length = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ifstream::beg);
        std::vector<uint8_t> bytes(length + fastgltf::getGltfBufferPadding());
        file.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(length));

        fastgltf::GltfDataBuffer byteBuffer;
        REQUIRE(byteBuffer.fromByteView(bytes.data(), length, length + fastgltf::getGltfBufferPadding()));

        auto asset = parser.loadBinaryGLTF(&byteBuffer, folder, fastgltf::Options::LoadGLBBuffers, fastgltf::Category::Buffers);
        REQUIRE(asset.error() == fastgltf::Error::None);
    }
}
