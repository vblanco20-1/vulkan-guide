#include <algorithm>
#include <cstdlib>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

// All headers not in the root directory require this
#define GLM_ENABLE_EXPERIMENTAL 1

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

#include <fastgltf/base64.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/math.hpp>
#include "gltf_path.hpp"
#include <simdjson.h>

constexpr auto noOptions = fastgltf::Options::None;

TEST_CASE("Component type tests", "[gltf-loader]") {
    using namespace fastgltf;

    // clang-format off
    REQUIRE(fastgltf::getNumComponents(AccessorType::Scalar) ==  1);
    REQUIRE(fastgltf::getNumComponents(AccessorType::Vec2)   ==  2);
    REQUIRE(fastgltf::getNumComponents(AccessorType::Vec3)   ==  3);
    REQUIRE(fastgltf::getNumComponents(AccessorType::Vec4)   ==  4);
    REQUIRE(fastgltf::getNumComponents(AccessorType::Mat2)   ==  4);
    REQUIRE(fastgltf::getNumComponents(AccessorType::Mat3)   ==  9);
    REQUIRE(fastgltf::getNumComponents(AccessorType::Mat4)   == 16);

    REQUIRE(fastgltf::getComponentBitSize(ComponentType::Byte)          ==  8);
    REQUIRE(fastgltf::getComponentBitSize(ComponentType::UnsignedByte)  ==  8);
    REQUIRE(fastgltf::getComponentBitSize(ComponentType::Short)         == 16);
    REQUIRE(fastgltf::getComponentBitSize(ComponentType::UnsignedShort) == 16);
    REQUIRE(fastgltf::getComponentBitSize(ComponentType::UnsignedInt)   == 32);
    REQUIRE(fastgltf::getComponentBitSize(ComponentType::Float)         == 32);
    REQUIRE(fastgltf::getComponentBitSize(ComponentType::Double)        == 64);
    REQUIRE(fastgltf::getComponentBitSize(ComponentType::Invalid)       ==  0);

    REQUIRE(fastgltf::getElementByteSize(AccessorType::Scalar, ComponentType::Byte)  == 1);
    REQUIRE(fastgltf::getElementByteSize(AccessorType::Vec4,   ComponentType::Byte)  == 4);
    REQUIRE(fastgltf::getElementByteSize(AccessorType::Vec4,   ComponentType::Short) == 8);

    REQUIRE(fastgltf::getComponentType(5120) == ComponentType::Byte);
    REQUIRE(fastgltf::getComponentType(5121) == ComponentType::UnsignedByte);
    REQUIRE(fastgltf::getComponentType(5122) == ComponentType::Short);
    REQUIRE(fastgltf::getComponentType(5123) == ComponentType::UnsignedShort);
    REQUIRE(fastgltf::getComponentType(5125) == ComponentType::UnsignedInt);
    REQUIRE(fastgltf::getComponentType(5126) == ComponentType::Float);
    REQUIRE(fastgltf::getComponentType(5130) == ComponentType::Double);
    REQUIRE(fastgltf::getComponentType(5131) == ComponentType::Invalid);

	REQUIRE(fastgltf::getGLComponentType(ComponentType::Byte) == 5120);
	REQUIRE(fastgltf::getGLComponentType(ComponentType::UnsignedByte) == 5121);
	REQUIRE(fastgltf::getGLComponentType(ComponentType::Short) == 5122);
	REQUIRE(fastgltf::getGLComponentType(ComponentType::UnsignedShort) == 5123);
	REQUIRE(fastgltf::getGLComponentType(ComponentType::UnsignedInt) == 5125);
	REQUIRE(fastgltf::getGLComponentType(ComponentType::Float) == 5126);
	REQUIRE(fastgltf::getGLComponentType(ComponentType::Double) == 5130);
	REQUIRE(fastgltf::getGLComponentType(ComponentType::Invalid) == 0);
    // clang-format on
}

TEST_CASE("Test extension stringification", "[gltf-loader]") {
	auto stringified = stringifyExtension(fastgltf::Extensions::EXT_meshopt_compression);
	REQUIRE(stringified == fastgltf::extensions::EXT_meshopt_compression);

	stringified = stringifyExtension(fastgltf::Extensions::EXT_meshopt_compression | fastgltf::Extensions::EXT_texture_webp);
	REQUIRE(stringified == fastgltf::extensions::EXT_meshopt_compression);

	auto list = stringifyExtensionBits(fastgltf::Extensions::EXT_meshopt_compression | fastgltf::Extensions::EXT_texture_webp);
	REQUIRE(list.size() == 2);
	REQUIRE(list[0] == fastgltf::extensions::EXT_meshopt_compression);
	REQUIRE(list[1] == fastgltf::extensions::EXT_texture_webp);
}

TEST_CASE("Test if glTF type detection works", "[gltf-loader]") {
    fastgltf::Parser parser;

    SECTION("glTF") {
        auto gltfPath = sampleAssets / "Models" / "ABeautifulGame" / "glTF";
        REQUIRE(std::filesystem::exists(gltfPath));
		fastgltf::GltfFileStream jsonData(gltfPath / "ABeautifulGame.gltf");
		REQUIRE(jsonData.isOpen());

        REQUIRE(fastgltf::determineGltfFileType(jsonData) == fastgltf::GltfType::glTF);

        auto model = parser.loadGltfJson(jsonData, gltfPath);
		REQUIRE(model.error() == fastgltf::Error::None);
        REQUIRE(model.get_if() != nullptr);
		REQUIRE(fastgltf::validate(model.get()) == fastgltf::Error::None);
    }

    SECTION("GLB") {
        auto glbPath = sampleAssets / "Models" / "BoomBox" / "glTF-Binary";
        REQUIRE(std::filesystem::exists(glbPath));
		fastgltf::GltfFileStream jsonData(glbPath / "BoomBox.glb");
		REQUIRE(jsonData.isOpen());

        REQUIRE(fastgltf::determineGltfFileType(jsonData) == fastgltf::GltfType::GLB);

        auto model = parser.loadGltfBinary(jsonData, glbPath);
        REQUIRE(model.error() == fastgltf::Error::None);
		REQUIRE(model.get_if() != nullptr);
    }

    SECTION("Invalid") {
        auto fakePath = path / "base64.txt"; // Random file in the test directory that's not a glTF file.
        REQUIRE(std::filesystem::exists(fakePath));
		fastgltf::GltfFileStream jsonData(fakePath);
		REQUIRE(jsonData.isOpen());

        REQUIRE(fastgltf::determineGltfFileType(jsonData) == fastgltf::GltfType::Invalid);
    }
}

TEST_CASE("Loading some basic glTF", "[gltf-loader]") {
    fastgltf::Parser parser;
    SECTION("Loading basic invalid glTF files") {
		fastgltf::GltfFileStream jsonData(path / "empty_json.gltf");
		REQUIRE(jsonData.isOpen());

        auto emptyGltf = parser.loadGltfJson(jsonData, path);
        REQUIRE(emptyGltf.error() == fastgltf::Error::InvalidOrMissingAssetField);
    }

    SECTION("Load basic glTF file") {
		fastgltf::GltfFileStream jsonData(path / "basic_gltf.gltf");
		REQUIRE(jsonData.isOpen());

		auto basicGltf = parser.loadGltfJson(jsonData, path);
        REQUIRE(basicGltf.error() == fastgltf::Error::None);
		REQUIRE(fastgltf::validate(basicGltf.get()) == fastgltf::Error::None);
    }

    SECTION("Loading basic Cube.gltf") {
        auto cubePath = sampleAssets / "Models" / "Cube" / "glTF";
		fastgltf::GltfFileStream jsonData(cubePath / "Cube.gltf");
		REQUIRE(jsonData.isOpen());

        auto cube = parser.loadGltfJson(jsonData, cubePath, noOptions, fastgltf::Category::OnlyRenderable);
        REQUIRE(cube.error() == fastgltf::Error::None);
		REQUIRE(fastgltf::validate(cube.get()) == fastgltf::Error::None);

        REQUIRE(cube->scenes.size() == 1);
        REQUIRE(cube->scenes.front().nodeIndices.size() == 1);
        REQUIRE(cube->scenes.front().nodeIndices.front() == 0);

        REQUIRE(cube->nodes.size() == 1);
        REQUIRE(cube->nodes.front().name == "Cube");
        REQUIRE(std::holds_alternative<fastgltf::TRS>(cube->nodes.front().transform));

        REQUIRE(cube->accessors.size() == 5);
        REQUIRE(cube->accessors[0].type == fastgltf::AccessorType::Scalar);
        REQUIRE(cube->accessors[0].componentType == fastgltf::ComponentType::UnsignedShort);
        REQUIRE(cube->accessors[1].type == fastgltf::AccessorType::Vec3);
        REQUIRE(cube->accessors[1].componentType == fastgltf::ComponentType::Float);

        REQUIRE(cube->bufferViews.size() == 5);
        REQUIRE(cube->buffers.size() == 1);

        REQUIRE(cube->materials.size() == 1);
        auto& material = cube->materials.front();
        REQUIRE(material.name == "Cube");
        REQUIRE(material.pbrData.baseColorTexture.has_value());
        REQUIRE(material.pbrData.baseColorTexture->textureIndex == 0);
		REQUIRE(material.pbrData.metallicFactor == 0.);
		REQUIRE(material.pbrData.roughnessFactor == fastgltf::num(0.079));
        REQUIRE(!material.normalTexture.has_value());
        REQUIRE(!material.emissiveTexture.has_value());
        REQUIRE(!material.occlusionTexture.has_value());
    }

    SECTION("Loading basic Box.gltf") {
        auto boxPath = sampleAssets / "Models" / "Box" / "glTF";
		fastgltf::GltfFileStream jsonData(boxPath / "Box.gltf");
		REQUIRE(jsonData.isOpen());

        auto box = parser.loadGltfJson(jsonData, boxPath, noOptions, fastgltf::Category::OnlyRenderable);
        REQUIRE(box.error() == fastgltf::Error::None);
		REQUIRE(fastgltf::validate(box.get()) == fastgltf::Error::None);

        REQUIRE(box->defaultScene.has_value());
        REQUIRE(box->defaultScene.value() == 0);

        REQUIRE(box->nodes.size() == 2);
        REQUIRE(box->nodes[0].children.size() == 1);
        REQUIRE(box->nodes[0].children[0] == 1);
        REQUIRE(box->nodes[1].children.empty());
        REQUIRE(box->nodes[1].meshIndex.has_value());
        REQUIRE(box->nodes[1].meshIndex.value() == 0);

        REQUIRE(box->materials.size() == 1);
        REQUIRE(box->materials[0].name == "Red");
        REQUIRE(box->materials[0].pbrData.baseColorFactor[3] == 1.0f);
        REQUIRE(box->materials[0].pbrData.metallicFactor == 0.0f);
    }
}


TEST_CASE("Loading glTF animation", "[gltf-loader]") {
    auto animatedCube = sampleAssets / "Models" / "AnimatedCube" / "glTF";
	fastgltf::GltfFileStream jsonData(animatedCube / "AnimatedCube.gltf");
	REQUIRE(jsonData.isOpen());

    fastgltf::Parser parser;
    auto asset = parser.loadGltfJson(jsonData, animatedCube, noOptions, fastgltf::Category::OnlyAnimations);
    REQUIRE(asset.error() == fastgltf::Error::None);
	REQUIRE(fastgltf::validate(asset.get()) == fastgltf::Error::None);

    REQUIRE(!asset->animations.empty());

    auto& animation = asset->animations.front();
    REQUIRE(animation.name == "animation_AnimatedCube");

    REQUIRE(!animation.channels.empty());
    REQUIRE(animation.channels.front().nodeIndex == 0U);
    REQUIRE(animation.channels.front().samplerIndex == 0);
    REQUIRE(animation.channels.front().path == fastgltf::AnimationPath::Rotation);

    REQUIRE(!animation.samplers.empty());
    REQUIRE(animation.samplers.front().interpolation == fastgltf::AnimationInterpolation::Linear);
    REQUIRE(animation.samplers.front().inputAccessor == 0);
    REQUIRE(animation.samplers.front().outputAccessor == 1);
}

TEST_CASE("Loading glTF skins", "[gltf-loader]") {
    auto simpleSkin = sampleAssets / "Models" / "SimpleSkin" / "glTF";
	fastgltf::GltfFileStream jsonData(simpleSkin / "SimpleSkin.gltf");
	REQUIRE(jsonData.isOpen());

    fastgltf::Parser parser;
    auto asset = parser.loadGltfJson(jsonData, simpleSkin, noOptions, fastgltf::Category::Skins | fastgltf::Category::Nodes);
    REQUIRE(asset.error() == fastgltf::Error::None);
	REQUIRE(fastgltf::validate(asset.get()) == fastgltf::Error::None);

    REQUIRE(!asset->skins.empty());

    auto& skin = asset->skins.front();
    REQUIRE(skin.joints.size() == 2);
    REQUIRE(skin.joints[0] == 1);
    REQUIRE(skin.joints[1] == 2);
    REQUIRE(skin.inverseBindMatrices.has_value());
    REQUIRE(skin.inverseBindMatrices.value() == 4);

    REQUIRE(!asset->nodes.empty());

    auto& node = asset->nodes.front();
    REQUIRE(node.skinIndex.has_value());
    REQUIRE(node.skinIndex == 0U);
}

TEST_CASE("Loading glTF cameras", "[gltf-loader]") {
    auto cameras = sampleAssets / "Models" / "Cameras" / "glTF";
	fastgltf::GltfFileStream jsonData(cameras / "Cameras.gltf");
	REQUIRE(jsonData.isOpen());

    fastgltf::Parser parser;
    auto asset = parser.loadGltfJson(jsonData, cameras, noOptions, fastgltf::Category::Cameras);
    REQUIRE(asset.error() == fastgltf::Error::None);
	REQUIRE(fastgltf::validate(asset.get()) == fastgltf::Error::None);

    REQUIRE(asset->cameras.size() == 2);

    REQUIRE(std::holds_alternative<fastgltf::Camera::Perspective>(asset->cameras[0].camera));
    REQUIRE(std::holds_alternative<fastgltf::Camera::Orthographic>(asset->cameras[1].camera));

    const auto* pPerspective = std::get_if<fastgltf::Camera::Perspective>(&asset->cameras[0].camera);
    REQUIRE(pPerspective != nullptr);
    REQUIRE(pPerspective->aspectRatio == 1.0f);
    REQUIRE(pPerspective->yfov == 0.7f);
    REQUIRE(pPerspective->zfar == 100);
    REQUIRE(pPerspective->znear == 0.01f);

    const auto* pOrthographic = std::get_if<fastgltf::Camera::Orthographic>(&asset->cameras[1].camera);
    REQUIRE(pOrthographic != nullptr);
    REQUIRE(pOrthographic->xmag == 1.0f);
    REQUIRE(pOrthographic->ymag == 1.0f);
    REQUIRE(pOrthographic->zfar == 100);
    REQUIRE(pOrthographic->znear == 0.01f);
}

TEST_CASE("Validate models with re-used parser", "[gltf-loader]") {
	auto sponza = sampleAssets / "Models" / "Sponza" / "glTF";
	fastgltf::Parser parser;

	SECTION("Validate Sponza.gltf") {
		fastgltf::GltfFileStream jsonData(sponza / "Sponza.gltf");
		REQUIRE(jsonData.isOpen());

		auto model = parser.loadGltfJson(jsonData, sponza);
		REQUIRE(model.error() == fastgltf::Error::None);
		REQUIRE(fastgltf::validate(model.get()) == fastgltf::Error::None);
	}

	SECTION("Validate BrainStem.gltf") {
		auto brainStem = sampleAssets / "Models" / "BrainStem" / "glTF";
		fastgltf::GltfFileStream jsonData(brainStem / "BrainStem.gltf");
		REQUIRE(jsonData.isOpen());

		auto model = parser.loadGltfJson(jsonData, brainStem);
		REQUIRE(model.error() == fastgltf::Error::None);
		REQUIRE(fastgltf::validate(model.get()) == fastgltf::Error::None);
	}
}

TEST_CASE("Test allocation callbacks for embedded buffers", "[gltf-loader]") {
    auto boxPath = sampleAssets / "Models" / "Box" / "glTF-Embedded";
	fastgltf::GltfFileStream jsonData(boxPath / "Box.gltf");
	REQUIRE(jsonData.isOpen());

    std::vector<void*> allocations;

    auto mapCallback = [](uint64_t bufferSize, void* userPointer) -> fastgltf::BufferInfo {
        REQUIRE(userPointer != nullptr);
        auto* allocations = static_cast<std::vector<void*>*>(userPointer);
        allocations->emplace_back(std::malloc(bufferSize));
        return fastgltf::BufferInfo {
            allocations->back(),
            allocations->size() - 1,
        };
    };

    fastgltf::Parser parser;
    parser.setUserPointer(&allocations);
    parser.setBufferAllocationCallback(mapCallback, nullptr);
	auto asset = parser.loadGltfJson(jsonData, boxPath, noOptions, fastgltf::Category::Buffers);
	REQUIRE(asset.error() == fastgltf::Error::None);
	REQUIRE(fastgltf::validate(asset.get()) == fastgltf::Error::None);

    REQUIRE(allocations.size() == 1);
    REQUIRE(asset->buffers.size() == 1);

    auto& buffer = asset->buffers.front();
    const auto* customBuffer = std::get_if<fastgltf::sources::CustomBuffer>(&buffer.data);
    REQUIRE(customBuffer != nullptr);
    REQUIRE(customBuffer->id == 0);

    for (auto& allocation : allocations) {
        REQUIRE(allocation != nullptr);
        std::free(allocation);
    }
}

TEST_CASE("Test base64 decoding callbacks", "[gltf-loader]") {
    auto boxPath = sampleAssets / "Models" / "Box" / "glTF-Embedded";
	fastgltf::GltfFileStream jsonData(boxPath / "Box.gltf");
	REQUIRE(jsonData.isOpen());

    size_t decodeCounter = 0;
    auto decodeCallback = [](const std::string_view encodedData, uint8_t* outputData,
        const size_t padding, [[maybe_unused]] size_t outputSize, void* userPointer) {
        (*static_cast<size_t*>(userPointer))++;
        fastgltf::base64::decode_inplace(encodedData, outputData, padding);
    };

    fastgltf::Parser parser;
    parser.setUserPointer(&decodeCounter);
    parser.setBase64DecodeCallback(decodeCallback);
    auto model = parser.loadGltfJson(jsonData, boxPath, noOptions, fastgltf::Category::Buffers);
    REQUIRE(model.error() == fastgltf::Error::None);
	REQUIRE(fastgltf::validate(model.get()) == fastgltf::Error::None);
    REQUIRE(decodeCounter != 0);
}

TEST_CASE("Validate sparse accessor parsing", "[gltf-loader]") {
    auto simpleSparseAccessor = sampleAssets / "Models" / "SimpleSparseAccessor" / "glTF";
	fastgltf::GltfFileStream jsonData(simpleSparseAccessor / "SimpleSparseAccessor.gltf");
	REQUIRE(jsonData.isOpen());

    fastgltf::Parser parser;
    auto asset = parser.loadGltfJson(jsonData, simpleSparseAccessor, noOptions, fastgltf::Category::Accessors);
    REQUIRE(asset.error() == fastgltf::Error::None);
	REQUIRE(fastgltf::validate(asset.get()) == fastgltf::Error::None);

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
}

TEST_CASE("Validate morph target parsing", "[gltf-loader]") {
    auto simpleMorph = sampleAssets / "Models" / "SimpleMorph" / "glTF";
	fastgltf::GltfFileStream jsonData(simpleMorph / "SimpleMorph.gltf");
	REQUIRE(jsonData.isOpen());

    fastgltf::Parser parser;
    auto asset = parser.loadGltfJson(jsonData, simpleMorph, noOptions, fastgltf::Category::Meshes);
    REQUIRE(asset.error() == fastgltf::Error::None);
	REQUIRE(fastgltf::validate(asset.get()) == fastgltf::Error::None);

    REQUIRE(asset->meshes.size() == 1);
    REQUIRE(asset->meshes.front().weights.size() == 2);
    REQUIRE(asset->meshes.front().primitives.size() == 1);

    auto& primitive = asset->meshes.front().primitives.front();

	auto position = primitive.findAttribute("POSITION");
	REQUIRE(position != primitive.attributes.end());
	REQUIRE(position->name == "POSITION");
	REQUIRE(position->accessorIndex == 1);

    REQUIRE(primitive.targets.size() == 2);

	auto positionTarget0 = primitive.findTargetAttribute(0, "POSITION");
    REQUIRE(positionTarget0 != primitive.targets[0].end());
	REQUIRE(positionTarget0->name == "POSITION");
    REQUIRE(positionTarget0->accessorIndex == 2);

	auto positionTarget1 = primitive.findTargetAttribute(1, "POSITION");
    REQUIRE(positionTarget1 != primitive.targets[1].end());
	REQUIRE(positionTarget1->name == "POSITION");
    REQUIRE(positionTarget1->accessorIndex == 3);
}

TEST_CASE("Test accessors min/max", "[gltf-loader]") {
	auto lightsLamp = sampleAssets / "Models" / "LightsPunctualLamp" / "glTF";
	fastgltf::GltfFileStream jsonData(lightsLamp / "LightsPunctualLamp.gltf");
	REQUIRE(jsonData.isOpen());

	fastgltf::Parser parser(fastgltf::Extensions::KHR_lights_punctual);
	auto asset = parser.loadGltfJson(jsonData, lightsLamp, noOptions, fastgltf::Category::Accessors);
	REQUIRE(asset.error() == fastgltf::Error::None);
	REQUIRE(fastgltf::validate(asset.get()) == fastgltf::Error::None);

	REQUIRE(std::find_if(asset->extensionsUsed.begin(), asset->extensionsUsed.end(), [](auto& string) {
		return string == fastgltf::extensions::KHR_lights_punctual;
	}) != asset->extensionsUsed.end());

	REQUIRE(asset->accessors.size() == 15);
	auto& accessors = asset->accessors;

	{
		auto& firstAccessor = accessors[0];
		const auto& max = firstAccessor.max;
		const auto& min = firstAccessor.min;
		REQUIRE(max.has_value());
		REQUIRE(min.has_value());
		REQUIRE(max->size() == fastgltf::getNumComponents(firstAccessor.type));
		REQUIRE(max->size() == 1);
		REQUIRE(min->size() == 1);
		REQUIRE(max->isType<std::int64_t>());
		REQUIRE(min->isType<std::int64_t>());
		REQUIRE(max->get<std::int64_t>(0) == 3211);
		REQUIRE(min->get<std::int64_t>(0) == 0);
	}

	{
    	auto& secondAccessor = accessors[1];
    	const auto& max = secondAccessor.max;
    	const auto& min = secondAccessor.min;
		REQUIRE(max.has_value());
		REQUIRE(min.has_value());
		REQUIRE(max->size() == fastgltf::getNumComponents(secondAccessor.type));
		REQUIRE(max->size() == 3);
		REQUIRE(min->size() == 3);

		REQUIRE(max->isType<double>());
		REQUIRE(min->isType<double>());

		REQUIRE(max->get<double>(0) == Catch::Approx(0.81497824192047119));
		REQUIRE(max->get<double>(1) == Catch::Approx(1.8746249675750732));
		REQUIRE(max->get<double>(2) == Catch::Approx(0.32295516133308411));

		REQUIRE(min->get<double>(0) == Catch::Approx(-0.12269512563943863));
		REQUIRE(min->get<double>(1) == Catch::Approx(0.013025385327637196));
		REQUIRE(min->get<double>(2) == Catch::Approx(-0.32393229007720947));
	}

	{
		auto& fifthAccessor = accessors[4];
		const auto& max = fifthAccessor.max;
		const auto& min = fifthAccessor.min;
		REQUIRE(max.has_value());
		REQUIRE(min.has_value());
		REQUIRE(max->size() == fastgltf::getNumComponents(fifthAccessor.type));
		REQUIRE(max->size() == 4);
		REQUIRE(min->size() == 4);
		REQUIRE(max->isType<double>());
		REQUIRE(min->isType<double>());

		REQUIRE(max->get<double>(3) == 1.0);
	}
}

TEST_CASE("Test unicode characters", "[gltf-loader]") {
#if FASTGLTF_CPP_20
	auto unicodePath = sampleAssets / "Models" / std::filesystem::path(u8"Unicode❤♻Test") / "glTF";
	fastgltf::GltfFileStream jsonData(unicodePath / std::filesystem::path(u8"Unicode❤♻Test.gltf"));
	REQUIRE(jsonData.isOpen());
#else
	auto unicodePath = sampleAssets / "Models" / std::filesystem::u8path(u8"Unicode❤♻Test") / "glTF";
	fastgltf::GltfFileStream jsonData(unicodePath / std::filesystem::u8path(u8"Unicode❤♻Test.gltf"));
	REQUIRE(jsonData.isOpen());
#endif

	fastgltf::Parser parser;
	auto asset = parser.loadGltfJson(jsonData, unicodePath);
	REQUIRE(asset.error() == fastgltf::Error::None);
	REQUIRE(fastgltf::validate(asset.get()) == fastgltf::Error::None);

	REQUIRE(!asset->materials.empty());
	REQUIRE(asset->materials[0].name == "Unicode❤♻Material");

	REQUIRE(!asset->buffers.empty());
	auto bufferUri = std::get<fastgltf::sources::URI>(asset->buffers[0].data);
	REQUIRE(bufferUri.uri.path() == "Unicode❤♻Binary.bin");
#if FASTGLTF_CPP_20
	REQUIRE(bufferUri.uri.fspath() == std::filesystem::path{ u8"Unicode❤♻Binary.bin" });
#else
	REQUIRE(bufferUri.uri.fspath() == std::filesystem::u8path("Unicode❤♻Binary.bin"));
#endif
}

TEST_CASE("Test extras callback", "[gltf-loader]") {
	auto materialVariants = sampleAssets / "Models" / "MaterialsVariantsShoe" / "glTF";
	std::vector<std::string> nodeNames;

	// lambda callback to parse the glTF JSON from the jsonData buffer
	auto parseJson = [&](fastgltf::GltfDataGetter& data) {
		auto extrasCallback = [](simdjson::dom::object *extras, std::size_t objectIndex, fastgltf::Category category,
								 void *userPointer) {
			if (category != fastgltf::Category::Nodes)
				return;
			auto *nodeNames = static_cast<std::vector<std::string> *>(userPointer);
			nodeNames->resize(fastgltf::max(nodeNames->size(), objectIndex + 1));

			std::string_view nodeName;
			if ((*extras)["name"].get_string().get(nodeName) == simdjson::SUCCESS) {
				(*nodeNames)[objectIndex] = std::string(nodeName);
			}
		};

		fastgltf::Parser parser;
		parser.setExtrasParseCallback(extrasCallback);
		parser.setUserPointer(&nodeNames);
		return parser.loadGltfJson(data, materialVariants);
	};

	// The asset has to be reused for exporting in the next section.
	fastgltf::GltfFileStream jsonData(materialVariants / "MaterialsVariantsShoe.gltf");
	REQUIRE(jsonData.isOpen());
	auto asset = parseJson(jsonData);
	SECTION("Validate node names from extras") {
		REQUIRE(asset.error() == fastgltf::Error::None);
		REQUIRE(fastgltf::validate(asset.get()) == fastgltf::Error::None);

		REQUIRE(nodeNames.size() == 3);
		REQUIRE(nodeNames.size() == asset->nodes.size());

		REQUIRE(nodeNames[0] == "Shoe");
		REQUIRE(nodeNames[1] == "g Shoe");
		REQUIRE(nodeNames[2] == "Shoe.obj");
	}

	SECTION("Re-export asset") {
		auto extrasWriteCallback = [](std::size_t objectIndex, fastgltf::Category category,
									  void *userPointer) -> std::optional<std::string> {
			if (category != fastgltf::Category::Nodes)
				return std::nullopt;

			auto *nodeNames = static_cast<std::vector<std::string> *>(userPointer);
			if (objectIndex >= nodeNames->size())
				return std::nullopt; // TODO: Error?
			return {std::string(R"({"name":")") + (*nodeNames)[objectIndex] + "\"}"};
		};
		fastgltf::Exporter exporter;
		exporter.setUserPointer(&nodeNames);
		exporter.setExtrasWriteCallback(extrasWriteCallback);
		auto json = exporter.writeGltfJson(asset.get());
		REQUIRE(json.error() == fastgltf::Error::None);

		// Update the data buffer
		auto& string = json.get().output;
		auto reexportedJson = fastgltf::GltfDataBuffer::FromBytes(
				reinterpret_cast<const std::byte*>(string.data()), string.size());
		REQUIRE(reexportedJson.error() == fastgltf::Error::None);

		nodeNames.clear();
		auto reparsed = parseJson(reexportedJson.get());
		REQUIRE(reparsed.error() == fastgltf::Error::None);
		REQUIRE(fastgltf::validate(reparsed.get()) == fastgltf::Error::None);

		REQUIRE(nodeNames.size() == 3);
		REQUIRE(nodeNames[0] == "Shoe");
		REQUIRE(nodeNames[1] == "g Shoe");
		REQUIRE(nodeNames[2] == "Shoe.obj");
	}
}

#if defined(FASTGLTF_HAS_MEMORY_MAPPED_FILE)
TEST_CASE("Test glTF file loading", "[gltf-loader]") {
	SECTION("Mapped files") {
		auto cubePath = sampleAssets / "Models" / "Cube" / "glTF";
		auto mappedFile = fastgltf::MappedGltfFile::FromPath(cubePath / "Cube.gltf");
		REQUIRE(mappedFile.error() == fastgltf::Error::None);

		REQUIRE(fastgltf::determineGltfFileType(mappedFile.get()) == fastgltf::GltfType::glTF);

		fastgltf::Parser parser;
		auto asset = parser.loadGltfJson(mappedFile.get(), cubePath);
		REQUIRE(asset.error() == fastgltf::Error::None);
		REQUIRE(fastgltf::validate(asset.get()) == fastgltf::Error::None);
	}
}
#endif
