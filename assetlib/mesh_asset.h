#pragma once
#include <asset_loader.h>


namespace assets {


	struct Vertex_f32_PNCV {

		float position[3];
		float normal[3];
		float color[3];
		float uv[2];
	};
	struct Vertex_P32N8C8V16 {

		float position[3];
		uint8_t normal[3];
		uint8_t color[3];
		float uv[2];
	};



	enum class VertexFormat : uint32_t
	{
		Unknown = 0,
		PNCV_F32, //everything at 32 bits
		P32N8C8V16 //position at 32 bits, normal at 8 bits, color at 8 bits, uvs at 16 bits float
	};

	struct MeshBounds {
		
		float origin[3];
		float radius;
		float extents[3];
	};


	struct MeshInfo {
		uint64_t vertexBuferSize;
		uint64_t indexBuferSize;
		MeshBounds bounds;
		VertexFormat vertexFormat;
		char indexSize;
		CompressionMode compressionMode;
		std::string originalFile;
	};

	MeshInfo read_mesh_info(AssetFile* file);

	void unpack_mesh(MeshInfo* info, const char* sourcebuffer, size_t sourceSize, char* vertexBufer, char* indexBuffer);

	AssetFile pack_mesh(MeshInfo* info, char* vertexData, char* indexData);

	MeshBounds calculateBounds(Vertex_f32_PNCV* vertices, size_t count);
}