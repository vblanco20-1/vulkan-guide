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

struct GeoSurface {
	uint32_t startIndex;
	uint32_t vertexOffset;
	uint32_t count;
};

struct GLTFMesh {
	std::string name;

	std::vector<uint32_t> indices;
	std::vector<Vertex> vertices;
	std::vector<GeoSurface> surfaces;
};

struct LoadedGLTF {
	std::vector<std::shared_ptr<GLTFMesh>> meshes;
};

std::optional<LoadedGLTF> loadGltf(const std::filesystem::path& filePath);
