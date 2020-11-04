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

	//info.pixelsize[0] = texture_metadata["width"];
	//info.pixelsize[1] = texture_metadata["height"];
	info.textureSize = texture_metadata["buffer_size"];
	info.originalFile = texture_metadata["original_file"];

	for (auto& [key, value] : texture_metadata["pages"].items())
	{
		PageInfo page;

		page.compressedSize = value["compressed_size"];
		page.originalSize = value["original_size"];
		page.width = value["width"];
		page.height = value["height"];

		info.pages.push_back(page);
	}
		

	return info;
}

void assets::unpack_texture(TextureInfo* info, const char* sourcebuffer, size_t sourceSize, char* destination)
{
	if (info->compressionMode == CompressionMode::LZ4) {
	
		char* source;
		for (auto& page : info->pages)
		{
			LZ4_decompress_safe(source, destination, page.compressedSize, page.originalSize);
			source += page.compressedSize;
			destination += page.originalSize;
		}
		
	}
	else {
		memcpy(destination, sourcebuffer, sourceSize);
	}
}

void assets::unpack_texture_page(TextureInfo* info, int pageIndex, char* sourcebuffer, char* destination)
{


	if (info->compressionMode == CompressionMode::LZ4) {

		char* source = sourcebuffer;
		for (int i = 0; i < pageIndex; i++) {
			source += info->pages[i].compressedSize;
		}
		
		LZ4_decompress_safe(source, destination, info->pages[pageIndex].compressedSize, info->pages[pageIndex].originalSize);
	}
	//else {
	//	memcpy(destination, sourcebuffer, sourceSize);
	//}
}

#include <iostream>
assets::AssetFile assets::pack_texture(TextureInfo* info, void* pixelData)
{
	//core file header
	AssetFile file;	
	file.type[0] = 'T';
	file.type[1] = 'E';
	file.type[2] = 'X';
	file.type[3] = 'I';
	file.version = 1;


	char* pixels = (char*)pixelData;
	std::vector<char> page_buffer;
	for (auto& p : info->pages)
	{
		page_buffer.resize(p.originalSize);


		//compress buffer into blob
		int compressStaging = LZ4_compressBound(p.originalSize);

		page_buffer.resize(compressStaging);

		int compressedSize = LZ4_compress_default(pixels, page_buffer.data(), p.originalSize, compressStaging);

		page_buffer.resize(compressedSize);

		

		p.compressedSize = compressedSize;

		file.binaryBlob.insert(file.binaryBlob.end(), page_buffer.begin(), page_buffer.end());

		//advance pixel pointer to next page
		pixels += p.originalSize;

		//float compression_rate = float(compressedSize) / float(info->textureSize);
		//std::cout << "compression rate = " << compression_rate << std::endl;
		//
		////if the compression is more than 80% of the original size, its not worth to use it
		//if (compression_rate > 0.8)
		//{
		//	file.binaryBlob.resize(info->textureSize);
		//
		//	memcpy(file.binaryBlob.data(), pixelData, info->textureSize);
		//
		//	texture_metadata["compression"] = "None";
		//}

	}
	nlohmann::json texture_metadata;
	texture_metadata["format"] = "RGBA8";

	texture_metadata["buffer_size"] = info->textureSize;
	texture_metadata["original_file"] = info->originalFile;
	texture_metadata["compression"] = "LZ4";

	std::vector<nlohmann::json> page_json;
	for (auto& p : info->pages) {
		nlohmann::json page;
		page["compressed_size"] = p.compressedSize;
		page["original_size"] = p.originalSize;
		page["width"] = p.width;
		page["height"] = p.height;
		page_json.push_back(page);
	}
	texture_metadata["pages"] = page_json;

	std::string stringified = texture_metadata.dump();
	file.json = stringified;
	

	return file;
}

