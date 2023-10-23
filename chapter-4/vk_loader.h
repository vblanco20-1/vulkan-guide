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

class VulkanEngine;

struct GLTFTexture {
	std::vector<char> textureData;
};

struct GLTFMaterial {
	glm::vec4 colorFactors;
	float metallicFactor;
	float roughnessFactor;

	VkDescriptorSet matSet;
};

struct GeoSurface {
	uint32_t startIndex;
	uint32_t vertexOffset;
	uint32_t count;
	std::shared_ptr<GLTFMaterial> material;
};

struct GLTFMesh {
	std::string name;

	std::vector<uint32_t> indices;
	std::vector<Vertex> vertices;
	std::vector<GeoSurface> surfaces;
};

struct GLTFNode {
	std::weak_ptr<GLTFNode> parent;
	std::vector<std::shared_ptr<GLTFNode>> children;
	glm::mat4 transform;
	std::shared_ptr<GLTFMesh> mesh;

	glm::mat4 calculateWorldTransform() {

		std::shared_ptr<GLTFNode> p = parent.lock();
		if (p) {
			return p->calculateWorldTransform()*transform;
		}
		else {
			return transform;
		}
	}
};

//struct GLTFPbrMaterial {
//
//};

struct LoadedGLTF: public IRenderable {
	std::vector<std::shared_ptr<GLTFMesh>> meshes;
	std::vector<std::shared_ptr<GLTFNode>> nodes;

	std::vector< AllocatedImage> images;
	std::vector< std::shared_ptr<GLTFMaterial>> materials;

	std::vector<VkSampler> samplers;

	DescriptorAllocator descriptorPool;

	virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx);
};


std::optional<std::shared_ptr<LoadedGLTF>> loadGltf(const std::filesystem::path& filePath, VulkanEngine* engine);
