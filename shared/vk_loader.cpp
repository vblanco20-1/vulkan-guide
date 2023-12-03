#include "stb_image.h"
#include <iostream>
#include <vk_loader.h>

#include "vk_engine.h"
#include "vk_initializers.h"
#include "vk_types.h"
#include <glm/gtx/quaternion.hpp>

#include <fastgltf/parser.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/glm_element_traits.hpp>

//needed for the fastgltf variants
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

#if LOADER_STAGE>=2

std::optional<AllocatedImage> load_image(VulkanEngine* engine, fastgltf::Asset& asset, fastgltf::Image& image)
{
    AllocatedImage newImage {};

    int width, height, nrChannels;

    std::visit(
        fastgltf::visitor {
            [](auto& arg) {},
            [&](fastgltf::sources::URI& filePath) {
                assert(filePath.fileByteOffset == 0); // We don't support offsets with stbi.
                assert(filePath.uri.isLocalPath()); // We're only capable of loading
                                                    // local files.

                const std::string path(filePath.uri.path().begin(),
                    filePath.uri.path().end()); // Thanks C++.
                unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);
                if (data) {
                    VkExtent3D imagesize;
                    imagesize.width = width;
                    imagesize.height = height;
                    imagesize.depth = 1;

                    newImage = engine->create_image(data, imagesize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

                    stbi_image_free(data);
                }
            },
            [&](fastgltf::sources::Vector& vector) {
                unsigned char* data = stbi_load_from_memory(vector.bytes.data(), static_cast<int>(vector.bytes.size()),
                    &width, &height, &nrChannels, 4);
                if (data) {
                    VkExtent3D imagesize;
                    imagesize.width = width;
                    imagesize.height = height;
                    imagesize.depth = 1;

                    newImage = engine->create_image(data, imagesize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

                    stbi_image_free(data);
                }
            },
            [&](fastgltf::sources::BufferView& view) {
                auto& bufferView = asset.bufferViews[view.bufferViewIndex];
                auto& buffer = asset.buffers[bufferView.bufferIndex];

                std::visit(fastgltf::visitor { // We only care about VectorWithMime here, because we
                                               // specify LoadExternalBuffers, meaning all buffers
                                               // are already loaded into a vector.
                               [](auto& arg) {},
                               [&](fastgltf::sources::Vector& vector) {
                                   unsigned char* data = stbi_load_from_memory(vector.bytes.data() + bufferView.byteOffset,
                                       static_cast<int>(bufferView.byteLength),
                                       &width, &height, &nrChannels, 4);
                                   if (data) {
                                       VkExtent3D imagesize;
                                       imagesize.width = width;
                                       imagesize.height = height;
                                       imagesize.depth = 1;

                                       newImage = engine->create_image(data, imagesize, VK_FORMAT_R8G8B8A8_UNORM,
                                           VK_IMAGE_USAGE_SAMPLED_BIT);

                                       stbi_image_free(data);
                                   }
                               } },
                    buffer.data);
            },
        },
        image.data);

    // if any of the attempts to load the data failed, we havent written the image
    // so handle is null
    if (newImage.image == VK_NULL_HANDLE) {
        return {};
    } else {
        return newImage;
    }
}

std::optional<std::shared_ptr<LoadedGLTF>> loadGltf(VulkanEngine* engine,std::string_view filePath)
{

    std::cout << "Loading GLTF: " << filePath << std::endl;

    std::shared_ptr<LoadedGLTF> scene = std::make_shared<LoadedGLTF>();
    scene->creator = engine;
    LoadedGLTF& file = *scene.get();

    fastgltf::Parser parser {};

    constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble | fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;
    // fastgltf::Options::LoadExternalImages;

    fastgltf::GltfDataBuffer data;
    data.loadFromFile(filePath);

    fastgltf::Asset gltf;

    std::filesystem::path path = filePath;

    auto type = fastgltf::determineGltfFileType(&data);
    if (type == fastgltf::GltfType::glTF) {
        auto load = parser.loadGLTF(&data, path.parent_path(), gltfOptions);
        if (load) {
            gltf = std::move(load.get());
        } else {
            std::cerr << "Failed to load glTF: " << fastgltf::to_underlying(load.error()) << std::endl;
            return {};
        }
    } else if (type == fastgltf::GltfType::GLB) {
        auto load = parser.loadBinaryGLTF(&data, path.parent_path(), gltfOptions);
        if (load) {
            gltf = std::move(load.get());
        } else {
            std::cerr << "Failed to load glTF: " << fastgltf::to_underlying(load.error()) << std::endl;
            return {};
        }
    } else {
        std::cerr << "Failed to determine glTF container" << std::endl;
        return {};
    }

    auto asset = &gltf;

    // we can stimate the descriptors we will need accurately
    std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> sizes = { { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1 } };

    file.descriptorPool.init(engine->_device, gltf.materials.size(), sizes);

    // load samplers
    for (fastgltf::Sampler& sampler : asset->samplers) {
        VkSampler newSampler;

        VkSamplerCreateInfo sampl = {};
        sampl.pNext = nullptr;
        sampl.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

        vkCreateSampler(engine->_device, &sampl, nullptr, &newSampler);

        file.samplers.push_back(newSampler);
    }

    // temporal arrays for all the objects to use while creating the GLTF data
    std::vector<std::shared_ptr<MeshAsset>> meshes;
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<AllocatedImage> images;
    std::vector<std::shared_ptr<GLTFMaterial>> materials;

    // load all textures
    for (fastgltf::Image& image : asset->images) {
        std::optional<AllocatedImage> img = load_image(engine, *asset, image);

        if (img.has_value()) {
            images.push_back(*img);
            file.images[image.name.c_str()] = *img;
        } else {
            // we failed to load, so lets give the slot a default white texture to not
            // completely break loading
            images.push_back(engine->_errorCheckerboardImage);
            std::cout << "gltf failed to load texture " << image.name << std::endl;
        }
    }

    // create buffer to hold the material data
    file.materialDataBuffer = engine->create_buffer(sizeof(GLTFMetallic_Roughness::MaterialConstants) * asset->materials.size(),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    int data_index = 0;
    GLTFMetallic_Roughness::MaterialConstants* sceneMaterialConstants = (GLTFMetallic_Roughness::MaterialConstants*)file.materialDataBuffer.info.pMappedData;

    for (fastgltf::Material& mat : asset->materials) {
        std::shared_ptr<GLTFMaterial> newMat = std::make_shared<GLTFMaterial>();
        materials.push_back(newMat);
        file.materials[mat.name.c_str()] = newMat;

        GLTFMetallic_Roughness::MaterialConstants constants;
       constants.colorFactors.x = mat.pbrData.baseColorFactor[0];
       constants.colorFactors.y = mat.pbrData.baseColorFactor[1];
       constants.colorFactors.z = mat.pbrData.baseColorFactor[2];
       constants.colorFactors.w = mat.pbrData.baseColorFactor[3];

        constants.metal_rough_factors.x = mat.pbrData.metallicFactor;
        constants.metal_rough_factors.y = mat.pbrData.roughnessFactor;
        //write material parameters to buffer
        sceneMaterialConstants[data_index] = constants;

        MaterialPass passType = MaterialPass::MainColor;
        if (mat.alphaMode == fastgltf::AlphaMode::Blend) {
            passType = MaterialPass::Transparent;
        }

        GLTFMetallic_Roughness::MaterialResources materialResources;
        //default the material textures
        materialResources.colorImage = engine->_whiteImage;
        materialResources.colorSampler = engine->_defaultSamplerLinear;
		materialResources.metalRoughImage = engine->_whiteImage;
		materialResources.metalRoughSampler = engine->_defaultSamplerLinear;

        //set the uniform buffer for the material data
        materialResources.dataBuffer = file.materialDataBuffer.buffer;
        materialResources.dataBufferOffset = data_index * sizeof(GLTFMetallic_Roughness::MaterialConstants);
        //grab textures from gltf file
        if (mat.pbrData.baseColorTexture.has_value()) {
            size_t img = asset->textures[mat.pbrData.baseColorTexture.value().textureIndex].imageIndex.value();
            size_t sampler = asset->textures[mat.pbrData.baseColorTexture.value().textureIndex].samplerIndex.value();

			materialResources.colorImage = images[img];
			materialResources.colorSampler = file.samplers[sampler];
        }
        //build material
       newMat->data = engine->metalRoughMaterial.write_material(engine->_device,passType,materialResources, file.descriptorPool);

        data_index++;
    }

    // use the same vectors for all meshes so that the memory doesnt reallocate as
    // often
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;

    for (fastgltf::Mesh& mesh : asset->meshes) {
        std::shared_ptr<MeshAsset> newmesh = std::make_shared<MeshAsset>();
        meshes.push_back(newmesh);
        file.meshes[mesh.name.c_str()] = newmesh;
        newmesh->name = mesh.name;

        // clear the mesh arrays each mesh, we dont want to merge them by error
        indices.clear();
        vertices.clear();

        for (auto&& p : mesh.primitives) {
            GeoSurface newSurface;
            newSurface.startIndex = (uint32_t)indices.size();
          
            newSurface.count = (uint32_t)asset->accessors[p.indicesAccessor.value()].count;

			size_t initial_vtx = vertices.size();
			{
				fastgltf::Accessor& indexaccessor = gltf.accessors[p.indicesAccessor.value()];

				fastgltf::iterateAccessor<std::uint32_t>(gltf, indexaccessor, [&](std::uint32_t idx) {
					indices.push_back(idx + initial_vtx);
					});
			}

			fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->second];

			size_t vidx = initial_vtx;
			fastgltf::iterateAccessor<glm::vec3>(gltf, posAccessor,
				[&](glm::vec3 v) {
					Vertex newvtx;
					newvtx.position = v;
					newvtx.normal = { 1,0,0 };
					newvtx.color = glm::vec4{ 1.f };
					newvtx.uv_x = 0;
					newvtx.uv_y = 0;
					vertices.push_back(newvtx);
				});

			auto normals = p.findAttribute("NORMAL");
			if (normals != p.attributes.end()) {
				vidx = initial_vtx;
				fastgltf::iterateAccessor<glm::vec3>(gltf, gltf.accessors[(*normals).second],
					[&](glm::vec3 v) { vertices[vidx++].normal = v; });
			}

			auto uv = p.findAttribute("TEXCOORD_0");
			if (uv != p.attributes.end()) {
				vidx = initial_vtx;
				fastgltf::iterateAccessor<glm::vec2>(gltf, gltf.accessors[(*uv).second], [&](glm::vec2 v) {

					vertices[vidx].uv_x = v.x;
					vertices[vidx].uv_y = v.y;
					vidx++;
					});
			}

			auto colors = p.findAttribute("COLOR_0");
			if (colors != p.attributes.end()) {
				vidx = initial_vtx;
				fastgltf::iterateAccessor<glm::vec4>(gltf, gltf.accessors[(*colors).second],
					[&](glm::vec4 v) { vertices[vidx++].color = v; });
			}

            if (p.materialIndex.has_value()) {
                newSurface.material = materials[p.materialIndex.value()];
            } else {
                newSurface.material = materials[0];
            }

            newmesh->surfaces.push_back(newSurface);
        }

        newmesh->meshBuffers = engine->uploadMesh(indices, vertices);
    }

    // load all nodes and their meshes
    for (fastgltf::Node& node : asset->nodes) {
        std::shared_ptr<Node> newNode;

        //find if the node has a mesh, and if it does hook it to the mesh pointer and allocate it with the meshnode class
        if (node.meshIndex.has_value()) {
            newNode = std::make_shared<MeshNode>();
            static_cast<MeshNode*>(newNode.get())->mesh = meshes[*node.meshIndex];
        } else {
            newNode = std::make_shared<Node>();
        }

        nodes.push_back(newNode);
        file.nodes[node.name.c_str()];

        std::visit(overloaded { [&](fastgltf::Node::TransformMatrix matrix) {
                                   memcpy(&newNode->localTransform, matrix.data(), sizeof(matrix));
                               },
                       [&](fastgltf::Node::TRS transform) {
                           glm::vec3 tl(transform.translation[0], transform.translation[1],
                               transform.translation[2]);
                           glm::quat rot(transform.rotation[3], transform.rotation[0], transform.rotation[1],
                               transform.rotation[2]);
                           glm::vec3 sc(transform.scale[0], transform.scale[1], transform.scale[2]);

                           glm::mat4 tm = glm::translate(glm::mat4(1.f), tl);
                           glm::mat4 rm = glm::toMat4(rot);
                           glm::mat4 sm = glm::scale(glm::mat4(1.f), sc);

                           newNode->localTransform = tm * rm * sm;
                       } },
            node.transform);
    }

    // run loop again to setup transform hierarchy
    for (int i = 0; i < asset->nodes.size(); i++) {
        fastgltf::Node& node = asset->nodes[i];
        std::shared_ptr<Node>& sceneNode = nodes[i];

        for (auto c : node.children) {
            sceneNode->children.push_back(nodes[c]);
            nodes[c]->parent = sceneNode;
        }
    }

    // find the top nodes, with no parents
    for (auto& node : nodes) {
        if (node->parent.lock() == nullptr) {
            file.topNodes.push_back(node);
            node->refreshTransform(glm::mat4 { 1.f });
        }
    }

    return scene;
}
#endif
std::optional<std::vector<std::shared_ptr<MeshAsset>>> loadGltfMeshes(VulkanEngine* engine, std::filesystem::path filePath)
{
	std::cout << "Loading GLTF: " << filePath << std::endl;	

	fastgltf::GltfDataBuffer data;
	data.loadFromFile(filePath);

    constexpr auto gltfOptions = fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;

	fastgltf::Asset gltf;
    fastgltf::Parser parser{};

	auto load = parser.loadBinaryGLTF(&data, filePath.parent_path(), gltfOptions);
	if (load) {
		gltf = std::move(load.get());
	}
	else {
        fmt::print("Failed to load glTF: {} \n",fastgltf::to_underlying(load.error()));		
		return {};
     }

     std::vector<std::shared_ptr<MeshAsset>> meshes;

	// use the same vectors for all meshes so that the memory doesnt reallocate as
	// often
	std::vector<uint32_t> indices;
	std::vector<Vertex> vertices;
	for (fastgltf::Mesh& mesh : gltf.meshes) {
		MeshAsset newmesh;
		
		newmesh.name = mesh.name;

		// clear the mesh arrays each mesh, we dont want to merge them by error
		indices.clear();
		vertices.clear();

		for (auto&& p : mesh.primitives) {
			GeoSurface newSurface;
			newSurface.startIndex = (uint32_t)indices.size();
			newSurface.count = (uint32_t)gltf.accessors[p.indicesAccessor.value()].count;

            size_t initial_vtx = vertices.size();
			{
				fastgltf::Accessor& indexaccessor = gltf.accessors[p.indicesAccessor.value()];

				fastgltf::iterateAccessor<std::uint32_t>(gltf, indexaccessor, [&](std::uint32_t idx) {
					indices.push_back(idx + initial_vtx);
				});
			}

			fastgltf::Accessor& posAccessor =gltf.accessors[p.findAttribute("POSITION")->second];

			size_t vidx = initial_vtx;
			fastgltf::iterateAccessor<glm::vec3>(gltf, posAccessor,
				[&](glm::vec3 v) { 
                    Vertex newvtx;
                    newvtx.position = v;
                    newvtx.normal = {1,0,0};
                    newvtx.color = glm::vec4{1.f};
                    newvtx.uv_x = 0;
                    newvtx.uv_y = 0;
                    vertices.push_back(newvtx);
             });

			auto normals = p.findAttribute("NORMAL");
			if (normals != p.attributes.end()) {
				vidx = initial_vtx;
				fastgltf::iterateAccessor<glm::vec3>(gltf, gltf.accessors[(*normals).second],
					[&](glm::vec3 v) { vertices[vidx++].normal = v; });
			}

			auto uv = p.findAttribute("TEXCOORD_0");
			if (uv != p.attributes.end()) {
				vidx = initial_vtx;
				fastgltf::iterateAccessor<glm::vec2>(gltf, gltf.accessors[(*uv).second], [&](glm::vec2 v) {

					vertices[vidx].uv_x = v.x;
					vertices[vidx].uv_y = v.y;
					vidx++;
				});
			}

			auto colors = p.findAttribute("COLOR_0");
			if (colors != p.attributes.end()) {
				vidx = initial_vtx;
				fastgltf::iterateAccessor<glm::vec4>(gltf, gltf.accessors[(*colors).second],
					[&](glm::vec4 v) { vertices[vidx++].color = v; });
			}




			newmesh.surfaces.push_back(newSurface);
		}

		for (Vertex& vtx : vertices) {
            vtx.color = glm::vec4(vtx.normal, 1.f);
		}

		newmesh.meshBuffers = engine->uploadMesh(indices, vertices);

        meshes.emplace_back(std::make_shared<MeshAsset>(std::move(newmesh)));
	}

	return meshes;
}


#if LOADER_STAGE  >= 2
void LoadedGLTF::Draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
    // create renderables from the scenenodes
    for (auto& n : topNodes) {
        n->Draw(topMatrix, ctx);
    }
}

void LoadedGLTF::clearAll()
{
    // we need to copy all of the data that has to be destroyed into arrays to
    // pass into the lambda. shared_ptr deletes the actual objects for us once all
    // references are gone (after the destruction callback is run)

    std::vector<std::shared_ptr<MeshAsset>> meshesToDestroy;
    std::vector<AllocatedImage> imagesToDestroy;
    std::vector<std::shared_ptr<GLTFMaterial>> materialsToDestroy;

    for (auto& [k, v] : meshes) {
        meshesToDestroy.push_back(v);
    }

    for (auto& [k, v] : images) {
        imagesToDestroy.push_back(v);
    }

    for (auto& [k, v] : materials) {
        materialsToDestroy.push_back(v);
    }

    auto materialBuffer = materialDataBuffer;
    auto samplersToDestroy = samplers;


	VkDevice dv = creator->_device;

	descriptorPool.destroy_pools(dv);

	for (auto& i : imagesToDestroy) {
        creator->destroy_image(i);
	}
	for (auto& mesh : meshesToDestroy) {
		creator->destroy_buffer(mesh->meshBuffers.indexBuffer);
		creator->destroy_buffer(mesh->meshBuffers.vertexBuffer);
	}

    creator->destroy_buffer(materialBuffer);

	for (auto& sampler : samplersToDestroy) {
		vkDestroySampler(dv, sampler, nullptr);
	}
}


#endif