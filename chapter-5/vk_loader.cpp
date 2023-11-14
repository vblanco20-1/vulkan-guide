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

std::optional<std::shared_ptr<LoadedGLTF>> loadGltf(std::string_view filePath)
{
    VulkanEngine* engine = &VulkanEngine::Get();

    std::cout << "Loading GLTF: " << filePath << std::endl;

    std::shared_ptr<LoadedGLTF> scene = std::make_shared<LoadedGLTF>();
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
    std::vector<DescriptorAllocator::PoolSizeRatio> sizes = { { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1 } };

    file.descriptorPool.init_pool(engine->_device, gltf.meshes.size() + gltf.materials.size(), sizes);

    // default white image descriptor
    uint32_t whitepixel = 0xFFFFFFFF;
    file.defaultImage = engine->create_image((void*)&whitepixel, VkExtent3D { 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_SAMPLED_BIT);

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
            images.push_back(file.defaultImage);
            std::cout << "gltf failed to load texture " << image.name << std::endl;
        }
    }

    // create buffer to hold the material data
    file.materialDataBuffer = engine->create_buffer(sizeof(GPUGLTFMaterial) * asset->materials.size(),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    int data_index = 0;
    GPUGLTFMaterial* materialData = (GPUGLTFMaterial*)file.materialDataBuffer.info.pMappedData;

    for (fastgltf::Material& mat : asset->materials) {
        std::shared_ptr<GLTFMaterial> newMat = std::make_shared<GLTFMaterial>();
        materials.push_back(newMat);
        file.materials[mat.name.c_str()] = newMat;

        GPUGLTFMaterial matData;
        matData.colorFactors.x = mat.pbrData.baseColorFactor[0];
        matData.colorFactors.y = mat.pbrData.baseColorFactor[1];
        matData.colorFactors.z = mat.pbrData.baseColorFactor[2];
        matData.colorFactors.w = mat.pbrData.baseColorFactor[3];

        matData.metal_rough_factors.x = mat.pbrData.metallicFactor;
        matData.metal_rough_factors.y = mat.pbrData.roughnessFactor;

        materialData[data_index] = matData;

        if (mat.alphaMode == fastgltf::AlphaMode::Blend) {
            newMat->data = engine->_gltfDefaultTranslucent;
            newMat->IsTransparent = true;
        } else {
            newMat->data = engine->_gltfDefaultOpaque;
            newMat->IsTransparent = false;
        }

        newMat->data.materialSet = file.descriptorPool.allocate(engine->_device, engine->_gltfMatDescriptorLayout);

        VkDescriptorImageInfo imageInfo {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = engine->_whiteImage.imageView;
        imageInfo.sampler = engine->_defaultSampler;

        if (mat.pbrData.baseColorTexture.has_value()) {
            size_t img = asset->textures[mat.pbrData.baseColorTexture.value().textureIndex].imageIndex.value();
            size_t sampler = asset->textures[mat.pbrData.baseColorTexture.value().textureIndex].samplerIndex.value();

            imageInfo.imageView = images[img].imageView;
            imageInfo.sampler = file.samplers[sampler];
        }

        VkWriteDescriptorSet cameraWrite = vkinit::write_descriptor_image(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            newMat->data.materialSet, &imageInfo, 1);

        VkDescriptorBufferInfo binfo = vkinit::buffer_info(
            file.materialDataBuffer.buffer, data_index * sizeof(GPUGLTFMaterial), sizeof(GPUGLTFMaterial));
        VkWriteDescriptorSet bufferWrite = vkinit::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, newMat->data.materialSet, &binfo, 0);

        VkWriteDescriptorSet writes[] = { cameraWrite, bufferWrite };
        vkUpdateDescriptorSets(engine->_device, 2, writes, 0, nullptr);

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
            newSurface.vertexOffset = (uint32_t)vertices.size();
            newSurface.count = (uint32_t)asset->accessors[p.indicesAccessor.value()].count;

            {
                fastgltf::Accessor& indexaccessor = asset->accessors[p.indicesAccessor.value()];

                fastgltf::iterateAccessor<std::uint32_t>(*asset, indexaccessor, [&](std::uint32_t idx) {
                    indices.push_back(idx + newSurface.vertexOffset);
                });
            }

            fastgltf::Accessor& posAccessor = asset->accessors[p.findAttribute("POSITION")->second];

            vertices.resize(newSurface.vertexOffset + posAccessor.count);

            size_t vidx = newSurface.vertexOffset;
            fastgltf::iterateAccessor<glm::vec3>(*asset, posAccessor,
                [&](glm::vec3 v) { vertices[vidx++].position = v; });

            auto normals = p.findAttribute("NORMAL");
            if (normals != p.attributes.end()) {
                vidx = newSurface.vertexOffset;
                fastgltf::iterateAccessor<glm::vec3>(*asset, asset->accessors[(*normals).second],
                    [&](glm::vec3 v) { vertices[vidx++].normal = v; });
            }

            auto uv = p.findAttribute("TEXCOORD_0");
            if (uv != p.attributes.end()) {
                vidx = newSurface.vertexOffset;
                fastgltf::iterateAccessor<glm::vec2>(*asset, asset->accessors[(*uv).second], [&](glm::vec2 v) {

                    vertices[vidx].uv_x = v.x;
                    vertices[vidx].uv_y = v.y;
                    vidx++;
                });
            }

            auto colors = p.findAttribute("COLOR_0");
            if (colors != p.attributes.end()) {
                vidx = newSurface.vertexOffset;
                fastgltf::iterateAccessor<glm::vec4>(*asset, asset->accessors[(*colors).second],
                    [&](glm::vec4 v) { vertices[vidx++].color = v; });
            } else {
                for (auto& v : vertices) {
                    v.color = glm::vec4(1.f);
                }
            }

            if (p.materialIndex.has_value()) {
                newSurface.material = materials[p.materialIndex.value()];
            } else {
                newSurface.material = materials[0];
            }

            newmesh->surfaces.push_back(newSurface);
        }

        newmesh->meshBuffers = engine->uploadMesh(indices, vertices, &file.descriptorPool);
    }

    // load all nodes and their meshes
    for (fastgltf::Node& node : asset->nodes) {
        std::shared_ptr<Node> newNode;

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

void LoadedGLTF::Draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
    // create renderables from the scenenodes
    for (auto& n : topNodes) {
        n->Draw(topMatrix, ctx);
    }
}

void LoadedGLTF::clearAll()
{
    VkDescriptorPool poolToDestroy = descriptorPool.pool;

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

    VulkanEngine::Get().get_current_frame()._deletionQueue.push_function(
        [materialBuffer, poolToDestroy, meshesToDestroy, imagesToDestroy, materialsToDestroy, samplersToDestroy]() {
            VkDevice dv = VulkanEngine::Get()._device;

            vkDestroyDescriptorPool(dv, poolToDestroy, nullptr);

            for (auto& i : imagesToDestroy) {
                VulkanEngine::Get().destroy_image(i);
            }
            for (auto& mesh : meshesToDestroy) {
                VulkanEngine::Get().destroy_buffer(mesh->meshBuffers.indexBuffer);
                VulkanEngine::Get().destroy_buffer(mesh->meshBuffers.vertexBuffer);
            }

            VulkanEngine::Get().destroy_buffer(materialBuffer);

            for (auto& sampler : samplersToDestroy) {
                vkDestroySampler(dv, sampler, nullptr);
            }
        });
}

void MeshNode::Draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
    glm::mat4 nodeMatrix = topMatrix * worldTransform;

    for (auto& s : mesh->surfaces) {
        RenderObject def;
        def.indexCount = s.count;
        def.firstIndex = s.startIndex;
        def.mesh = &mesh->meshBuffers;
        def.material = &s.material->data;

        def.transform = nodeMatrix;

        if (s.material->IsTransparent) {
            ctx.TransparentSurfaces.push_back(def);
        } else {
            ctx.OpaqueSurfaces.push_back(def);
        }
    }

    // recurse down
    Node::Draw(topMatrix, ctx);
}
