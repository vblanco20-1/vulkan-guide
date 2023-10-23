#include <vk_mesh.h>
#include <vk_loader.h>
#include <iostream>
#include "stb_image.h"

#include <fastgltf/parser.hpp>
#include <fastgltf/tools.hpp>
#include "vk_engine.h"
#include "vk_types.h"
#include "../chapter-2/vk_initializers.h"



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
struct fastgltf::ElementTraits<glm::vec2> : fastgltf::ElementTraitsBase<glm::vec2, AccessorType::Vec2, float> {};
template<>
struct fastgltf::ElementTraits<glm::vec3> : fastgltf::ElementTraitsBase<glm::vec3, AccessorType::Vec3, float> {};
template<>
struct fastgltf::ElementTraits<glm::vec4> : fastgltf::ElementTraitsBase<glm::vec4, AccessorType::Vec4, float> {};
// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

std::optional<std::shared_ptr<LoadedGLTF>> loadGltf(const std::filesystem::path& filePath, VulkanEngine* engine)
{
	std::cout << "Loading GLTF: " << filePath << std::endl;
	
	std::shared_ptr<LoadedGLTF> scene = std::make_shared<LoadedGLTF>();
	LoadedGLTF& file = *scene.get();
	
	fastgltf::Parser parser{};
	

	std::vector<VkDescriptorPoolSize> sizes =
	{
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3 }
	};

	file.descriptorPool.init_pool(engine->_device, 10000, sizes);



	constexpr auto gltfOptions =
		fastgltf::Options::DontRequireValidAssetMember |
		fastgltf::Options::AllowDouble |
		fastgltf::Options::LoadGLBBuffers |
		fastgltf::Options::LoadExternalBuffers |
		fastgltf::Options::LoadExternalImages;
	
	fastgltf::GltfDataBuffer data;
	data.loadFromFile(filePath);
	
	fastgltf::Asset gltf;
	
	auto type = fastgltf::determineGltfFileType(&data);
	if (type == fastgltf::GltfType::glTF) {
		auto load = parser.loadGLTF(&data, filePath.parent_path(), gltfOptions);
		if (load)
		{
			gltf = std::move(load.get());
		}
		else {
			std::cerr << "Failed to load glTF: " << fastgltf::to_underlying(load.error()) << std::endl;
			return {};
		}
	}
	else if (type == fastgltf::GltfType::GLB) {
		auto load = parser.loadBinaryGLTF(&data, filePath.parent_path(), gltfOptions);
		if (load)
		{
			gltf = std::move(load.get());
		}
		else {
			std::cerr << "Failed to load glTF: " << fastgltf::to_underlying(load.error()) << std::endl;
			return {};
		}
	}
	else {
		std::cerr << "Failed to determine glTF container" << std::endl;
		return {};
	}
	
	auto asset = &gltf;
	
	//load samplers
	for (fastgltf::Sampler& sampler : asset->samplers) {
		VkSampler newSampler;


		VkSamplerCreateInfo sampl = {};
		sampl.pNext = nullptr;
		sampl.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

		vkCreateSampler(engine->_device, &sampl, nullptr, &newSampler);


		file.samplers.push_back(newSampler);
	}
	//load all textures
	for (fastgltf::Image& image : asset->images) {
		
		AllocatedImage newImage;
		
		int width, height, nrChannels;

		std::visit(fastgltf::visitor{
		[](auto& arg) {},
		[&](fastgltf::sources::URI& filePath) {
			assert(filePath.fileByteOffset == 0); // We don't support offsets with stbi.
			assert(filePath.uri.isLocalPath()); // We're only capable of loading local files.

			const std::string path(filePath.uri.path().begin(), filePath.uri.path().end()); // Thanks C++.
			unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);

			VkExtent3D imagesize;
			imagesize.width = width;
			imagesize.height = height;
			imagesize.depth = 1;

			newImage= engine->create_image(data, imagesize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

			stbi_image_free(data);
		},
		[&](fastgltf::sources::Vector& vector) {

			unsigned char* data = stbi_load_from_memory(vector.bytes.data(), static_cast<int>(vector.bytes.size()), &width, &height, &nrChannels, 4);
			
			VkExtent3D imagesize;
			imagesize.width = width;
			imagesize.height = height;
			imagesize.depth = 1;

			newImage = engine->create_image(data, imagesize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

			stbi_image_free(data);
		},
		[&](fastgltf::sources::BufferView& view) {
			auto& bufferView = asset->bufferViews[view.bufferViewIndex];
			auto& buffer = asset->buffers[bufferView.bufferIndex];
			// Yes, we've already loaded every buffer into some GL buffer. However, with GL it's simpler
			// to just copy the buffer data again for the texture. Besides, this is just an example.
			std::visit(fastgltf::visitor {
				// We only care about VectorWithMime here, because we specify LoadExternalBuffers, meaning
				// all buffers are already loaded into a vector.
				[](auto& arg) {},
				[&](fastgltf::sources::Vector& vector) {

					unsigned char* data = stbi_load_from_memory(vector.bytes.data() + bufferView.byteOffset, static_cast<int>(bufferView.byteLength), &width, &height, &nrChannels, 4);

					VkExtent3D imagesize;
					imagesize.width = width;
					imagesize.height = height;
					imagesize.depth = 1;

					newImage = engine->create_image(data, imagesize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

					stbi_image_free(data);
				}
			}, buffer.data);
		},
		}, image.data);

		file.images.push_back(newImage);
	}

	for (fastgltf::Material& mat : asset->materials) {

		std::shared_ptr<GLTFMaterial> newMat = std::make_shared<GLTFMaterial>();

		newMat->matSet = engine->_defaultGLTFdescriptor;
		if (mat.pbrData.baseColorTexture.has_value())
		{
			newMat->matSet = file.descriptorPool.allocate(engine->_device, engine->_gltfMatDescriptorLayout);

			int img = asset->textures[mat.pbrData.baseColorTexture.value().textureIndex].imageIndex.value();
			int sampler = asset->textures[mat.pbrData.baseColorTexture.value().textureIndex].samplerIndex.value();

			VkDescriptorImageInfo imgInfo{};
			imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imgInfo.imageView = file.images[img]._imageView;
			imgInfo.sampler = file.samplers[sampler];
			

			VkWriteDescriptorSet cameraWrite = vkinit::write_descriptor_image(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, newMat->matSet, &imgInfo, 0);

			vkUpdateDescriptorSets(engine->_device, 1, &cameraWrite, 0, nullptr);
		}


		file.materials.push_back(newMat);
	}
	
	for (fastgltf::Mesh& mesh : asset->meshes)
	{
		//engine->uploadMesh(mesh, gltf);
		std::shared_ptr<GLTFMesh> newmesh = std::make_shared<GLTFMesh>();
		file.meshes.push_back(newmesh);
		newmesh->name = mesh.name;

		for (auto&& p : mesh.primitives)
		{
			GeoSurface newSurface;
			newSurface.startIndex = newmesh->indices.size();
			newSurface.vertexOffset = newmesh->vertices.size();
			newSurface.count = asset->accessors[p.indicesAccessor.value()].count;

			{
				fastgltf::Accessor& indexaccessor = asset->accessors[p.indicesAccessor.value()];

				fastgltf::iterateAccessor<std::uint32_t>(*asset, indexaccessor, [&](std::uint32_t idx) {
					newmesh->indices.push_back(idx + newSurface.vertexOffset);
					});
			}

			fastgltf::Accessor& posAccessor = asset->accessors[p.findAttribute("POSITION")->second];

			newmesh->vertices.resize(newSurface.vertexOffset + posAccessor.count);

			size_t vidx = newSurface.vertexOffset;
			fastgltf::iterateAccessor<glm::vec3>(*asset, posAccessor, [&](glm::vec3 v) {
				newmesh->vertices[vidx++].position = v;
				});

			auto normals = p.findAttribute("NORMAL");
			if (normals != p.attributes.end())
			{
				vidx = newSurface.vertexOffset;
				fastgltf::iterateAccessor<glm::vec3>(*asset, asset->accessors[(*normals).second], [&](glm::vec3 v) {
					newmesh->vertices[vidx++].normal = v;
					});
			}

			auto uv = p.findAttribute("TEXCOORD_0");
			if (uv != p.attributes.end())
			{
				vidx = newSurface.vertexOffset;
				fastgltf::iterateAccessor<glm::vec2>(*asset, asset->accessors[(*uv).second], [&](glm::vec2 v) {
					int idx = vidx;
					newmesh->vertices[idx].uv_x = v.x;
					newmesh->vertices[idx].uv_y = v.y;
					vidx++;
				});
			}

			auto colors = p.findAttribute("COLOR_0");
			if (colors != p.attributes.end())
			{
				vidx = newSurface.vertexOffset;
				fastgltf::iterateAccessor<glm::vec4>(*asset, asset->accessors[(*colors).second], [&](glm::vec4 v) {
					newmesh->vertices[vidx++].color = v;
					});
			}
			else {
				for (auto& v : newmesh->vertices) {
					v.color = glm::vec4(1.f);
				}
			}
			
			if (p.materialIndex.has_value()) {
				newSurface.material = file.materials[p.materialIndex.value()];
			}
			else {
				newSurface.material = file.materials[0];
			}
			
			newmesh->surfaces.push_back(newSurface);
		
			engine->uploadMesh(newmesh.get());
			std::cout << "out";
		}
	}


	//load all nodes and their meshes
	for (fastgltf::Node& node : asset->nodes)
	{
		std::shared_ptr<GLTFNode> newNode = std::make_shared<GLTFNode>();
		file.nodes.push_back(newNode);
		if (node.meshIndex.has_value()) {
			newNode->mesh = file.meshes[*node.meshIndex];
		}
	
		std::visit(overloaded{ [&](fastgltf::Node::TransformMatrix matrix) {
	
			memcpy(&newNode->transform,matrix.data(),sizeof(matrix));
		},
		[&](fastgltf::Node::TRS transform) {
	
			glm::vec3 tl(transform.translation[0],transform.translation[1], transform.translation[2]);
			glm::quat rot(transform.rotation[3],transform.rotation[0], transform.rotation[1], transform.rotation[2]);
			glm::vec3 sc(transform.scale[0], transform.scale[1], transform.scale[2]);
	
	
			glm::mat4 tm = glm::translate(glm::mat4(1.f), tl);
			glm::mat4 rm = glm::toMat4(rot);
			glm::mat4 sm = glm::scale(glm::mat4(1.f), sc);
	
			newNode->transform = tm * rm * sm;
	
		} }, node.transform);
	}
	
	//run loop again to setup transform hierarchy
	for (int i = 0; i < asset->nodes.size(); i++) {
		fastgltf::Node& node = asset->nodes[i];
		std::shared_ptr<GLTFNode>& sceneNode = file.nodes[i];
	
		for (auto c : node.children) {
			sceneNode->children.push_back(file.nodes[c]);
			file.nodes[c]->parent = sceneNode;
		}
	}
	
	return scene;
}

void LoadedGLTF::Draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
	glm::mat4 gltfFlip = glm::mat4(1.f);
	//gltfFlip[1][1] = -1;
	glm::mat4 top = gltfFlip*topMatrix  ;
	//create renderables from the scenenodes
	for (auto& n : nodes)
	{
		if (n->mesh) {
			//for (auto & s : n->mesh->surfaces) {
			RenderObject def;
			def.mesh = &ctx.engine->loadedSurfaces[n->mesh->name];
			def.material = &ctx.engine->_defaultMat;
			def.transform = top* n->calculateWorldTransform();
			//def.matBind = ctx.engine->_defaultGLTFdescriptor;
			def.matBind = n->mesh->surfaces[0].material->matSet;
			ctx.SurfacesToDraw.push_back(def);
			//}
			
		}
	}
}
