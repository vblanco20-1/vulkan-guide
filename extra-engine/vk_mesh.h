#pragma once

#include <vk_types.h>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
struct VertexInputDescription {
	std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;

	VkPipelineVertexInputStateCreateFlags flags = 0;
};


struct Vertex {

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 uv;
	static VertexInputDescription get_vertex_description();
}; 
struct RenderBounds {
	glm::vec3 origin;
	float radius;
	glm::vec3 extents;
	bool valid;
};
struct Mesh {
	std::vector<Vertex> _vertices;
	std::vector<uint32_t> _indices;

	AllocatedBuffer _vertexBuffer;
	AllocatedBuffer _indexBuffer;

	RenderBounds bounds;

	bool load_from_obj(const char* filename);
	bool load_from_meshasset(const char* filename);
};