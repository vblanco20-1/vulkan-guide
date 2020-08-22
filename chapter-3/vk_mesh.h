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

    static VertexInputDescription getVertexInputState() {
        VertexInputDescription description;

        VkVertexInputBindingDescription mainBinding = {};
        mainBinding.binding = 0;
        mainBinding.stride = sizeof(Vertex);
        mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        description.bindings.push_back(mainBinding);

        VkVertexInputAttributeDescription positionAttribute = {};
        positionAttribute.binding = 0;
        positionAttribute.location = 0;
        positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
        positionAttribute.offset = offsetof(Vertex, position);

        VkVertexInputAttributeDescription normalAttribute = {};
        normalAttribute.binding = 0;
        normalAttribute.location = 1;
        normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
        normalAttribute.offset = offsetof(Vertex, normal);

        description.attributes.push_back(positionAttribute);
        description.attributes.push_back(normalAttribute);

        return description;
    }
};

struct Mesh {
	std::vector<Vertex> _vertices;
	std::vector<uint32_t> _indices;

	AllocatedBuffer _vertexBuffer;

	void bind_vertex_buffer(VkCommandBuffer cmd);
};

namespace vkutil {

	bool load_mesh_from_obj(const std::string& filename, std::vector<Vertex>& outVertices, std::vector<uint32_t>& outIndices);
}
