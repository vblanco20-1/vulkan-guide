// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vk_mesh.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <optional>
#include <string>
#include <memory>
#include <filesystem>
#include "vk_descriptors.h"
#include <unordered_map>

class VulkanEngine;

struct GLTFTexture {
	std::vector<char> textureData;
};

struct  GPUGLTFMaterial {
	glm::vec4 colorFactors;
	glm::vec4 metal_rough_factors;
	glm::vec4 extra[14];
};

static_assert(sizeof(GPUGLTFMaterial) == 256);

struct GLTFMaterial {
	glm::vec4 colorFactors;
	float metallicFactor;
	float roughnessFactor;

	//VkDescriptorSet matSet;

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
	Surface surface;
};


struct Node : public IRenderable {
	std::weak_ptr<Node> parent;
	std::vector<std::shared_ptr<Node>> children;

	

	glm::mat4 localTransform;
	glm::mat4 worldTransform;

	void refreshTransform(const glm::mat4& parentMatrix) {
		worldTransform = parentMatrix * localTransform;
		for (auto c : children) {
			c->refreshTransform(worldTransform);
		}
	}

	virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx);
};

struct MeshNode : public Node {

	std::shared_ptr<GLTFMesh> mesh;

	virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx);
};

struct LoadedGLTF: public IRenderable {

	//storage for all the data on a given gltf file

	std::unordered_map<std::string,std::shared_ptr<GLTFMesh>> meshes;
	std::unordered_map<std::string,std::shared_ptr<Node>> nodes;
	std::unordered_map<std::string, AllocatedImage> images;
	std::unordered_map<std::string, std::shared_ptr<GLTFMaterial>> materials;

	//nodes that dont have a parent, for iterating through the file in tree order
	std::vector<std::shared_ptr<Node>> topNodes;

	//std::vector<std::shared_ptr<GLTFMesh>> meshes;
	//std::vector<std::shared_ptr<GLTFNode>> nodes;

	//std::vector< AllocatedImage> images;
	//std::vector< std::shared_ptr<GLTFMaterial>> materials;

	std::vector<VkSampler> samplers;

	DescriptorAllocator descriptorPool;

	AllocatedBuffer materialDataBuffer;

	virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx);
};


std::optional<std::shared_ptr<LoadedGLTF>> loadGltf(const std::filesystem::path& filePath, VulkanEngine* engine);
