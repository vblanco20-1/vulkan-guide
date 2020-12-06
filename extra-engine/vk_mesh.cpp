#include <vk_mesh.h>
#include <iostream>
#include <asset_loader.h>
#include <mesh_asset.h>
#include "glm/common.hpp"
#include "glm/detail/func_geometric.inl"
#include "logger.h"

VertexInputDescription Vertex::get_vertex_description()
{
	VertexInputDescription description;

	//we will have just 1 vertex buffer binding, with a per-vertex rate
	VkVertexInputBindingDescription mainBinding = {};
	mainBinding.binding = 0;
	mainBinding.stride = sizeof(Vertex);
	mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	description.bindings.push_back(mainBinding);

	//Position will be stored at Location 0
	VkVertexInputAttributeDescription positionAttribute = {};
	positionAttribute.binding = 0;
	positionAttribute.location = 0;
	positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionAttribute.offset = offsetof(Vertex, position);

	//Normal will be stored at Location 1
	VkVertexInputAttributeDescription normalAttribute = {};
	normalAttribute.binding = 0;
	normalAttribute.location = 1;
	normalAttribute.format = VK_FORMAT_R8G8_UNORM;//VK_FORMAT_R32G32B32_SFLOAT;
	normalAttribute.offset = offsetof(Vertex, oct_normal);

	//Position will be stored at Location 2
	VkVertexInputAttributeDescription colorAttribute = {};
	colorAttribute.binding = 0;
	colorAttribute.location = 2;
	colorAttribute.format = VK_FORMAT_R8G8B8_UNORM;//VK_FORMAT_R32G32B32_SFLOAT;
	colorAttribute.offset = offsetof(Vertex, color);

	//UV will be stored at Location 2
	VkVertexInputAttributeDescription uvAttribute = {};
	uvAttribute.binding = 0;
	uvAttribute.location = 3;
	uvAttribute.format = VK_FORMAT_R32G32_SFLOAT;
	uvAttribute.offset = offsetof(Vertex, uv);


	description.attributes.push_back(positionAttribute);
	description.attributes.push_back(normalAttribute);
	description.attributes.push_back(colorAttribute);
	description.attributes.push_back(uvAttribute);
	return description;
}
using namespace glm;
vec2 OctNormalWrap(vec2 v)
{
	vec2 wrap;
	wrap.x = (1.0f - glm::abs(v.y)) * (v.x >= 0.0f ? 1.0f : -1.0f);
	wrap.y = (1.0f - glm::abs(v.x)) * (v.y >= 0.0f ? 1.0f : -1.0f);
	return wrap;
}

vec2 OctNormalEncode(vec3 n)
{
	n /= (glm::abs(n.x) + glm::abs(n.y) + glm::abs(n.z));

	vec2 wrapped = OctNormalWrap(n);

	vec2 result;
	result.x = n.z >= 0.0f ? n.x : wrapped.x;
	result.y = n.z >= 0.0f ? n.y : wrapped.y;

	result.x = result.x * 0.5f + 0.5f;
	result.y = result.y * 0.5f + 0.5f;

	return result;
}

vec3 OctNormalDecode(vec2 encN)
{
	encN = encN * 2.0f - 1.0f;

	// https://twitter.com/Stubbesaurus/status/937994790553227264
	vec3 n = vec3(encN.x, encN.y, 1.0f - abs(encN.x) - abs(encN.y));
	float t = glm::clamp(-n.z, 0.0f, 1.0f);

	n.x += n.x >= 0.0f ? -t : t;
	n.y += n.y >= 0.0f ? -t : t;

	n = glm::normalize(n);
	return (n);
}


void Vertex::pack_normal(glm::vec3 n)
{
	vec2 oct = OctNormalEncode(n);

	oct_normal.x = uint8_t(oct.x * 255);
	oct_normal.y = uint8_t(oct.y * 255);
}

void Vertex::pack_color(glm::vec3 c)
{
	color.r = static_cast<uint8_t>(c.x * 255);
	color.g = static_cast<uint8_t>(c.y * 255);
	color.b = static_cast<uint8_t>(c.z * 255);
}

bool Mesh::load_from_meshasset(const char* filename)
{
	assets::AssetFile file;
	bool loaded = assets::load_binaryfile(filename, file);

	if (!loaded) {
		std::cout << "Error when loading mesh " << filename << std::endl;;
		return false;
	}
	
		
	

	assets::MeshInfo meshinfo = assets::read_mesh_info(&file);



	std::vector<char> vertexBuffer;
	std::vector<char> indexBuffer;

	vertexBuffer.resize(meshinfo.vertexBuferSize);
	indexBuffer.resize(meshinfo.indexBuferSize);

	assets::unpack_mesh(&meshinfo, file.binaryBlob.data(), file.binaryBlob.size(), vertexBuffer.data(), indexBuffer.data());

	bounds.extents.x = meshinfo.bounds.extents[0];
	bounds.extents.y = meshinfo.bounds.extents[1];
	bounds.extents.z = meshinfo.bounds.extents[2];

	bounds.origin.x = meshinfo.bounds.origin[0];
	bounds.origin.y = meshinfo.bounds.origin[1];
	bounds.origin.z = meshinfo.bounds.origin[2];

	bounds.radius = meshinfo.bounds.radius;
	bounds.valid = true;

	_vertices.clear();
	_indices.clear();

	_indices.resize(indexBuffer.size() / sizeof(uint32_t));
	for (int i = 0; i < _indices.size(); i++) {
		uint32_t* unpacked_indices = (uint32_t*)indexBuffer.data();
		_indices[i] = unpacked_indices[i];
	}

	if (meshinfo.vertexFormat == assets::VertexFormat::PNCV_F32)
	{
		assets::Vertex_f32_PNCV* unpackedVertices = (assets::Vertex_f32_PNCV*)vertexBuffer.data();

		_vertices.resize(vertexBuffer.size() / sizeof(assets::Vertex_f32_PNCV));

		for (int i = 0; i < _vertices.size(); i++) {

			_vertices[i].position.x = unpackedVertices[i].position[0];
			_vertices[i].position.y = unpackedVertices[i].position[1];
			_vertices[i].position.z = unpackedVertices[i].position[2];

			vec3 normal = vec3( 
				unpackedVertices[i].normal[0],
				unpackedVertices[i].normal[1],
				unpackedVertices[i].normal[2] );
			_vertices[i].pack_normal(normal);

			_vertices[i].pack_color(vec3{ unpackedVertices[i].color[0] ,unpackedVertices[i].color[1] ,unpackedVertices[i].color[2] });


			_vertices[i].uv.x = unpackedVertices[i].uv[0];
			_vertices[i].uv.y = unpackedVertices[i].uv[1];
		}
	}
	else if (meshinfo.vertexFormat == assets::VertexFormat::P32N8C8V16)
	{
		assets::Vertex_P32N8C8V16* unpackedVertices = (assets::Vertex_P32N8C8V16*)vertexBuffer.data();

		_vertices.resize(vertexBuffer.size() / sizeof(assets::Vertex_P32N8C8V16));

		for (int i = 0; i < _vertices.size(); i++) {

			_vertices[i].position.x = unpackedVertices[i].position[0];
			_vertices[i].position.y = unpackedVertices[i].position[1];
			_vertices[i].position.z = unpackedVertices[i].position[2];

			_vertices[i].pack_normal(vec3{ 
				 unpackedVertices[i].normal[0]
				,unpackedVertices[i].normal[1]
				,unpackedVertices[i].normal[2] });

			_vertices[i].color.x = unpackedVertices[i].color[0];// / 255.f;
			_vertices[i].color.y = unpackedVertices[i].color[1];// / 255.f;
			_vertices[i].color.z = unpackedVertices[i].color[2];// / 255.f;

			_vertices[i].uv.x = unpackedVertices[i].uv[0];
			_vertices[i].uv.y = unpackedVertices[i].uv[1];
		}
	}

	
	if (logMeshUpload)
	{
		LOG_SUCCESS("Loaded mesh {} : Verts={}, Tris={}", filename, _vertices.size(), _indices.size() / 3);
	}

	return true;
}
