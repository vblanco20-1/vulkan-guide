// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>

#include <string>
#include <vector>



struct VertexInputDescription {
    std::vector<VkVertexInputBindingDescription> bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;

    VkPipelineVertexInputStateCreateFlags flags = 0;
};

struct Vertex {

    glm::vec3 position;
    glm::vec3 normal;
	glm::vec3 color;

    static VertexInputDescription get_vertex_description();
};

struct Mesh {
	std::vector<Vertex> _vertices;
	std::vector<uint32_t> _indices;

	AllocatedBuffer _vertexBuffer;

	void bind_vertex_buffer(VkCommandBuffer cmd);

	bool load_from_obj(const std::string& filename);
};

namespace vkutil {

	bool load_mesh_from_obj(const std::string& filename, std::vector<Vertex>& outVertices, std::vector<uint32_t>& outIndices);
}
