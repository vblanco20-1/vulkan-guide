#pragma once
#include <asset_loader.h>


namespace assets {


	struct Vertex_f32_PNCV {

		float position[3];
		float normal[3];
		float color[3];
		float uv[2];
	};

	enum class VertexFormat : uint32_t
	{
		Unknown = 0,
		PNCV_F32
	};


	struct MeshInfo {
		uint64_t vertexBuferSize;
		uint64_t indexBuferSize;
		VertexFormat vertexFormat;
		char indexSize;
		CompressionMode compressionMode;
		std::string originalFile;
	};

	MeshInfo read_mesh_info(AssetFile* file);

	void unpack_mesh(MeshInfo* info, const char* sourcebuffer, size_t sourceSize, char* vertexBufer, char* indexBuffer);

	AssetFile pack_mesh(MeshInfo* info, char* vertexData, char* indexData);
}