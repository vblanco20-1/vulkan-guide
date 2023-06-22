#include <vk_mesh.h>
#include <vk_loader.h>
#include <iostream>
#include <fastgltf/parser.hpp>
#include <fastgltf/tools.hpp>

static const std::byte* getBufferData(const fastgltf::Buffer& buffer) {
	const std::byte* result = nullptr;

	std::visit(fastgltf::visitor{
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


template<>
struct fastgltf::ElementTraits<glm::vec3> : fastgltf::ElementTraitsBase<glm::vec3, AccessorType::Vec3, float> {};
template<>
struct fastgltf::ElementTraits<glm::vec4> : fastgltf::ElementTraitsBase<glm::vec4, AccessorType::Vec4, float> {};
//template<>
//struct fastgltf::ElementTraits<glm::vec4> : fastgltf::ElementTraitsBase<glm::vec4, AccessorType::Vec4, float> {};


std::optional<LoadedGLTF> loadGltf(const std::filesystem::path& filePath)
{
	std::cout << "Loading GLTF: " << filePath << std::endl;


	LoadedGLTF file;

	fastgltf::Parser parser{};

	constexpr auto gltfOptions =
		fastgltf::Options::DontRequireValidAssetMember |
		fastgltf::Options::AllowDouble |
		fastgltf::Options::LoadGLBBuffers;/* |
		fastgltf::Options::LoadExternalBuffers |
		fastgltf::Options::LoadExternalImages;*/

	fastgltf::GltfDataBuffer data;
	data.loadFromFile(filePath);

	std::unique_ptr<fastgltf::glTF> gltf;

	auto type = fastgltf::determineGltfFileType(&data);
	if (type == fastgltf::GltfType::glTF) {
		gltf = parser.loadGLTF(&data, filePath.parent_path(), gltfOptions);
	}
	else if (type == fastgltf::GltfType::GLB) {
		gltf = parser.loadBinaryGLTF(&data, filePath.parent_path(), gltfOptions);
	}
	else {
		std::cerr << "Failed to determine glTF container" << std::endl;
		return {};
	}

	if (parser.getError() != fastgltf::Error::None) {
		std::cerr << "Failed to load glTF: " << fastgltf::to_underlying(parser.getError()) << std::endl;
		return{};
	}

	auto error = gltf->parse(fastgltf::Category::Scenes);
	if (error != fastgltf::Error::None) {
		std::cerr << "Failed to parse glTF: " << fastgltf::to_underlying(error) << std::endl;
		return{};
	}

	auto asset = gltf->getParsedAsset();
	for (fastgltf::Mesh & mesh : asset->meshes)
	{
		std::shared_ptr<GLTFMesh> newMesh = std::make_shared<GLTFMesh>();
		file.meshes.push_back(newMesh);
		newMesh->name = mesh.name;

		for (auto&& p : mesh.primitives)
		{
			GeoSurface newSurface;
			newSurface.startIndex = newMesh->indices.size();
			newSurface.vertexOffset = newMesh->vertices.size();
			newSurface.count = asset->accessors[p.indicesAccessor.value()].count;

			
			{
				fastgltf::Accessor& indexaccessor = asset->accessors[p.indicesAccessor.value()];

				fastgltf::iterateAccessor<std::uint32_t>(*asset,indexaccessor,[&](std::uint32_t idx){
					newMesh->indices.push_back(idx);
				});
			}

			
			fastgltf::Accessor& posAccessor = asset->accessors[p.attributes["POSITION"]];

			newMesh->vertices.resize(newSurface.vertexOffset + posAccessor.count);
			
			size_t vidx = newSurface.vertexOffset;
			fastgltf::iterateAccessor<glm::vec3>(*asset, posAccessor, [&](glm::vec3 v) {
				newMesh->vertices[vidx++].position = v;
			});

			auto normals = p.attributes.find("NORMAL");
			if (normals != p.attributes.end())
			{
				vidx = newSurface.vertexOffset;
				fastgltf::iterateAccessor<glm::vec3>(*asset, asset->accessors[(*normals).second], [&](glm::vec3 v) {
					newMesh->vertices[vidx++].normal = v;
				});
			}

			auto colors = p.attributes.find("COLOR_0");
			if (colors != p.attributes.end())
			{
				vidx = newSurface.vertexOffset;
				fastgltf::iterateAccessor<glm::vec4>(*asset, asset->accessors[(*colors).second], [&](glm::vec4 v) {
					newMesh->vertices[vidx++].color =v;
				});
			}

			newMesh->surfaces.push_back(newSurface);

			std::cout << "out";
		}
	}


	return file;
}
