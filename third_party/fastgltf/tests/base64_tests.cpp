#include <fstream>
#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <fastgltf/base64.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/parser.hpp>
#include "gltf_path.hpp"

constexpr std::string_view testBase64 = "SGVsbG8gV29ybGQuIEhlbGxvIFdvcmxkLiBIZWxsbyBXb3JsZC4=";

TEST_CASE("Check base64 utility functions", "[base64]") {
    REQUIRE(fastgltf::base64::getPadding("Li==") == 2);
    REQUIRE(fastgltf::base64::getPadding("Li4=") == 1);
    REQUIRE(fastgltf::base64::getPadding("Li4u") == 0);

    REQUIRE(fastgltf::base64::getOutputSize(4, 0) == 3); // Li4u
    REQUIRE(fastgltf::base64::getOutputSize(4, 1) == 2); // Li4=
    REQUIRE(fastgltf::base64::getOutputSize(4, 2) == 1); // Li==
}

TEST_CASE("Check base64 decoding", "[base64]") {
    // This is "Hello World. Hello World.". The decode function
    // uses the best possible SIMD version of the algorithm.
    auto bytes = fastgltf::base64::decode(testBase64);
    std::string strings(bytes.begin(), bytes.end());
    REQUIRE(strings == "Hello World. Hello World. Hello World.");
}

TEST_CASE("Check all base64 decoders", "[base64]") {
    // Checks that the base64 decoders return the same.
    auto bytes = fastgltf::base64::fallback_decode(testBase64);
    std::string strings(bytes.begin(), bytes.end());
    REQUIRE(strings == "Hello World. Hello World. Hello World.");

#if defined(__x86_64__) || defined(_M_AMD64) || defined(_M_IX86)
    REQUIRE(bytes == fastgltf::base64::avx2_decode(testBase64));
    REQUIRE(bytes == fastgltf::base64::sse4_decode(testBase64));
#endif
#if defined(__aarch64__)
    REQUIRE(bytes == fastgltf::base64::neon_decode(testBase64));
#endif
}

TEST_CASE("Check big base64 data decoding", "[base64]") {
    std::ifstream file(path / "base64.txt");
    REQUIRE(file.is_open());

    std::stringstream buffer;
    buffer << file.rdbuf();

    auto encodedBytes = buffer.str();
    auto bytes = fastgltf::base64::decode(encodedBytes);
    REQUIRE(!bytes.empty());

    std::ifstream output(path / "base64.txt.out", std::ios::binary | std::ios::ate);
    REQUIRE(output.is_open());
    std::vector<uint8_t> decodedBytes(output.tellg());
    output.seekg(0);
    output.read(reinterpret_cast<char*>(decodedBytes.data()), static_cast<std::streamsize>(decodedBytes.size()));

    REQUIRE(bytes == decodedBytes);
}

TEST_CASE("Test base64 buffer decoding", "[base64]") {
    fastgltf::Parser parser;
    fastgltf::Image texture;
    std::string bufferData;

    auto cylinderEngine = sampleModels / "2.0" / "2CylinderEngine" / "glTF-Embedded";
    auto boxTextured = sampleModels / "2.0" / "BoxTextured" / "glTF-Embedded";

    auto tceJsonData = std::make_unique<fastgltf::GltfDataBuffer>();
    REQUIRE(tceJsonData->loadFromFile(cylinderEngine / "2CylinderEngine.gltf"));
    auto btJsonData = std::make_unique<fastgltf::GltfDataBuffer>();
    REQUIRE(btJsonData->loadFromFile(boxTextured / "BoxTextured.gltf"));

    SECTION("Validate large buffer load from glTF") {
        auto asset = parser.loadGLTF(tceJsonData.get(), cylinderEngine, fastgltf::Options::None, fastgltf::Category::Buffers);
        REQUIRE(asset.error() == fastgltf::Error::None);

        REQUIRE(asset->buffers.size() == 1);

        // Load the buffer from the parsed glTF file.
        auto& buffer = asset->buffers.front();
        REQUIRE(buffer.byteLength == 1794612);
        auto bufferVector = std::get_if<fastgltf::sources::Vector>(&buffer.data);
        REQUIRE(bufferVector != nullptr);
        REQUIRE(bufferVector->mimeType == fastgltf::MimeType::OctetStream);
        REQUIRE(!bufferVector->bytes.empty());
    }

    SECTION("Validate base64 buffer and image load from glTF") {
        auto asset = parser.loadGLTF(btJsonData.get(), boxTextured, fastgltf::Options::None, fastgltf::Category::Images | fastgltf::Category::Buffers);
        REQUIRE(asset.error() == fastgltf::Error::None);

        REQUIRE(asset->buffers.size() == 1);
        REQUIRE(asset->images.size() == 1);

        auto& buffer = asset->buffers.front();
        REQUIRE(buffer.byteLength == 840);
        auto bufferVector = std::get_if<fastgltf::sources::Vector>(&buffer.data);
        REQUIRE(bufferVector != nullptr);
        REQUIRE(bufferVector->mimeType == fastgltf::MimeType::OctetStream);
        REQUIRE(!bufferVector->bytes.empty());

        auto& image = asset->images.front();
        auto imageVector = std::get_if<fastgltf::sources::Vector>(&image.data);
        REQUIRE(imageVector != nullptr);
        REQUIRE(imageVector->mimeType == fastgltf::MimeType::PNG);
        REQUIRE(!imageVector->bytes.empty());
    }
}
