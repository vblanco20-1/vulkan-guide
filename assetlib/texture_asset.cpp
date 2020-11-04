#include <texture_asset.h>
#include <json.hpp>
#include <lz4.h>

assets::TextureFormat parse_format(const char* f) {

	if (strcmp(f, "RGBA8") == 0)
	{
		return assets::TextureFormat::RGBA8;
	}
	else {
		return assets::TextureFormat::Unknown;
	}
}

assets::TextureInfo assets::read_texture_info(AssetFile* file)
{
	TextureInfo info;

	nlohmann::json texture_metadata = nlohmann::json::parse(file->json);

	std::string formatString = texture_metadata["format"];
	info.textureFormat = parse_format(formatString.c_str());

	std::string compressionString = texture_metadata["compression"];
	info.compressionMode = parse_compression(compressionString.c_str());

	info.pixelsize[0] = texture_metadata["width"];
	info.pixelsize[1] = texture_metadata["height"];
	info.textureSize = texture_metadata["buffer_size"];
	info.originalFile = texture_metadata["original_file"];

	return info;
}

void assets::unpack_texture(TextureInfo* info, const char* sourcebuffer, size_t sourceSize, char* destination)
{
	if (info->compressionMode == CompressionMode::LZ4) {
	
		LZ4_decompress_safe(sourcebuffer, destination, sourceSize, info->textureSize);
	}
	else {
		memcpy(destination, sourcebuffer, sourceSize);
	}
}
#include <iostream>
assets::AssetFile assets::pack_texture(TextureInfo* info, void* pixelData)
{
	nlohmann::json texture_metadata;
	texture_metadata["format"] = "RGBA8";
	texture_metadata["width"] = info->pixelsize[0];
	texture_metadata["height"] = info->pixelsize[1];
	texture_metadata["buffer_size"] = info->textureSize;	
	texture_metadata["original_file"] = info->originalFile;


	//core file header
	AssetFile file;	
	file.type[0] = 'T';
	file.type[1] = 'E';
	file.type[2] = 'X';
	file.type[3] = 'I';
	file.version = 1;

	
	//compress buffer into blob
	int compressStaging = LZ4_compressBound(info->textureSize);

	file.binaryBlob.resize(compressStaging);

	int compressedSize = LZ4_compress_default((const char*)pixelData, file.binaryBlob.data(), info->textureSize, compressStaging);

	file.binaryBlob.resize(compressedSize);

	texture_metadata["compression"] = "LZ4";

	float compression_rate = float(compressedSize) / float(info->textureSize);
	std::cout << "compression rate = " << compression_rate << std::endl;

	//if the compression is more than 80% of the original size, its not worth to use it
	if (compression_rate > 0.8)
	{
		file.binaryBlob.resize(info->textureSize);

		memcpy(file.binaryBlob.data(), pixelData, info->textureSize);

		texture_metadata["compression"] = "None";
	}
	

	std::string stringified = texture_metadata.dump();
	file.json = stringified;
	

	return file;
}

