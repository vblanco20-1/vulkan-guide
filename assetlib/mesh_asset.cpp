#include "mesh_asset.h"
#include "json.hpp"
#include "lz4.h"

assets::MeshInfo assets::read_mesh_info(AssetFile* file)
{
	MeshInfo info;

	nlohmann::json metadata = nlohmann::json::parse(file->json);

	info.vertexFormat = VertexFormat::PNCV_F32;
	info.vertexBuferSize = metadata["vertex_buffer_size"];	
	info.vertexBuferSize = metadata["vertex_buffer_size"];
	info.indexBuferSize = metadata["index_buffer_size"];
	info.indexSize = (uint8_t) metadata["index_size"];
	info.originalFile = metadata["original_file"];

    return info;
}

void assets::unpack_mesh(MeshInfo* info, const char* sourcebuffer, size_t sourceSize, char* vertexBufer, char* indexBuffer)
{
	std::vector<char> decompressedBuffer;
	decompressedBuffer.resize(info->vertexBuferSize + info->indexBuferSize);

	LZ4_decompress_safe(sourcebuffer, decompressedBuffer.data(), sourceSize, decompressedBuffer.size());

	memcpy(vertexBufer, decompressedBuffer.data(), info->vertexBuferSize);

	//copy index buffer
	memcpy(indexBuffer, decompressedBuffer.data() + info->vertexBuferSize, info->indexBuferSize);
}

assets::AssetFile assets::pack_mesh(MeshInfo* info, char* vertexData, char* indexData)
{
    AssetFile file;
	file.type[0] = 'M';
	file.type[1] = 'E';
	file.type[2] = 'S';
	file.type[3] = 'H';
	file.version = 1;

	nlohmann::json metadata;
	metadata["vertex_format"] = "f32_PNCV";
	metadata["vertex_buffer_size"] = info->vertexBuferSize;
	metadata["index_buffer_size"] = info->indexBuferSize;
	metadata["index_size"] = info->indexSize;
	metadata["original_file"] = info->originalFile;

	size_t fullsize = info->vertexBuferSize + info->indexBuferSize;

	std::vector<char> merged_buffer;
	merged_buffer.resize(fullsize);

	//copy vertex buffer
	memcpy(merged_buffer.data(), vertexData, info->vertexBuferSize);

	//copy index buffer
	memcpy(merged_buffer.data() + info->vertexBuferSize, indexData, info->indexBuferSize);


	//compress buffer and copy it into the file struct
	size_t compressStaging = LZ4_compressBound(fullsize);

	file.binaryBlob.resize(compressStaging);

	int compressedSize = LZ4_compress_default(merged_buffer.data(), file.binaryBlob.data(), merged_buffer.size(), compressStaging);
	file.binaryBlob.resize(compressedSize);

	metadata["compression"] = "LZ4";

	file.json = metadata.dump();

	return file;
}
