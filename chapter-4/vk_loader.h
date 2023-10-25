// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>

#include "vk_descriptors.h"
#include <unordered_map>

class VulkanEngine;

struct GLTFMaterial {

    bool IsTransparent;

    MaterialData data;
};

struct GeoSurface {
    uint32_t startIndex;
    uint32_t vertexOffset;
    uint32_t count;
    std::shared_ptr<GLTFMaterial> material;
};

struct GLTFMesh {
    std::string name;

    std::vector<GeoSurface> surfaces;
    GPUMesh meshBuffers;
};

struct GltfMeshNode : public Node {

    std::shared_ptr<GLTFMesh> mesh;

    virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx);
};

struct LoadedGLTF : public IRenderable {

    // storage for all the data on a given gltf file
    std::unordered_map<std::string, std::shared_ptr<GLTFMesh>> meshes;
    std::unordered_map<std::string, std::shared_ptr<Node>> nodes;
    std::unordered_map<std::string, AllocatedImage> images;
    std::unordered_map<std::string, std::shared_ptr<GLTFMaterial>> materials;

    // nodes that dont have a parent, for iterating through the file in tree order
    std::vector<std::shared_ptr<Node>> topNodes;

    std::vector<VkSampler> samplers;

    DescriptorAllocator descriptorPool;

    AllocatedBuffer materialDataBuffer;

    // gltf defaults to a default white image for things not found
    AllocatedImage defaultImage;

    ~LoadedGLTF()
    {
        clearAll();
    };

    virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx);

private:
    void clearAll();
};

std::optional<std::shared_ptr<LoadedGLTF>> loadGltf(std::string_view filePath);
