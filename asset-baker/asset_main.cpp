#include <iostream>
#include <json.hpp>
#include <fstream>
#include <filesystem>

#include <lz4.h>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "tiny_obj_loader.h"

namespace fs = std::filesystem;

struct File {
	char type[4];
	int version;
	std::string json;
	std::vector<char> binaryBlob;
};

bool save_binaryfile(const fs::path& path,const File& file) {
	std::ofstream outfile;
	outfile.open(path, std::ios::binary | std::ios::out);

	outfile.write(file.type, 4);
	uint32_t version = file.version;
	//version
	outfile.write((const char*)&version, sizeof(uint32_t));

	//json lenght
	uint32_t lenght = file.json.size();
	outfile.write((const char*)&lenght, sizeof(uint32_t));

	//blob lenght
	uint32_t bloblenght = file.binaryBlob.size();
	outfile.write((const char*)&bloblenght, sizeof(uint32_t));

	//json stream
	outfile.write(file.json.data(), lenght);
	//pixel data
	outfile.write(file.binaryBlob.data(), file.binaryBlob.size());

	outfile.close();

	return true;
}

bool load_binaryfile(const fs::path& path, File& outputFile) {
	
	std::ifstream infile;
	infile.open(path, std::ios::binary);

	if (!infile.is_open()) return false;

	infile.seekg(0);

	
	infile.read(outputFile.type, 4);
	uint32_t vers;
	infile.read((char*)&outputFile.version, sizeof(uint32_t));

	uint32_t jsonlen = 0;
	infile.read((char*)&jsonlen, sizeof(uint32_t));

	uint32_t bloblen = 0;
	infile.read((char*)&bloblen, sizeof(uint32_t));

	outputFile.json.resize(jsonlen);

	infile.read(outputFile.json.data(), jsonlen);

	outputFile.binaryBlob.resize(bloblen);
	infile.read(outputFile.binaryBlob.data(), bloblen);

	return true;
}
bool convert_image(const fs::path& input, const fs::path& output)
{
	int texWidth, texHeight, texChannels;

	auto pngstart = std::chrono::high_resolution_clock::now();

	stbi_uc* pixels = stbi_load(input.u8string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	auto pngend = std::chrono::high_resolution_clock::now();

	auto diff = pngend - pngstart;

	std::cout << "png took " << std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / 1000000.0 << "ms"  << std::endl;

	if (!pixels) {
		std::cout << "Failed to load texture file " << input << std::endl;
		return false;
	}
	
	int texture_size = texWidth * texHeight * 4;

	nlohmann::json texture_metadata;
	texture_metadata["format"] = "RGBA8";
	texture_metadata["original_file"] = input.string();
	texture_metadata["width"] = texWidth;
	texture_metadata["height"] = texHeight;
	texture_metadata["buffer_size"] = texture_size;

	std::string stringified = texture_metadata.dump();

	File newImage;
	newImage.json = stringified;
	newImage.version = 1;
	newImage.type[0] = 'T';
	newImage.type[1] = 'E';
	newImage.type[2] = 'X';
	newImage.type[3] = 'I';

	auto start = std::chrono::high_resolution_clock::now();

	int compressStaging = LZ4_compressBound(texture_size);

	newImage.binaryBlob.resize(compressStaging);

	int compressedSize = LZ4_compress_default((const char*)pixels, newImage.binaryBlob.data(), texture_size, compressStaging);

	newImage.binaryBlob.resize(compressedSize);

	auto  end = std::chrono::high_resolution_clock::now();

	diff = end - start;

	std::cout << "compression took " << std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / 1000000.0 << "ms" << std::endl;
	


	newImage.binaryBlob.resize(compressedSize);
	

	stbi_image_free(pixels);

	save_binaryfile(output, newImage);

	File loadImage;

	

	

	start = std::chrono::high_resolution_clock::now();
	load_binaryfile(output, loadImage);

	std::vector<char> blob;
	blob.resize(texture_size);
	
	LZ4_decompress_safe(loadImage.binaryBlob.data(), blob.data(),loadImage.binaryBlob.size(), blob.size());

	end = std::chrono::high_resolution_clock::now();

	diff = end - start;

	std::cout << "loading and decompression took " << std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / 1000000.0 << "ms" << std::endl;


	return true;
}
struct Vertex {

	float position[3];
	float normal[3];
	float color[3];
	float uv[2];
	
};

bool convert_mesh(const fs::path& input, const fs::path& output)

{
	//attrib will contain the vertex arrays of the file
	tinyobj::attrib_t attrib;
	//shapes contains the info for each separate object in the file
	std::vector<tinyobj::shape_t> shapes;
	//materials contains the information about the material of each shape, but we wont use it.
	std::vector<tinyobj::material_t> materials;

	//error and warning output from the load function
	std::string warn;
	std::string err;
	auto pngstart = std::chrono::high_resolution_clock::now();

	//load the OBJ file
	tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, input.string().c_str(),
		nullptr);

	auto pngend = std::chrono::high_resolution_clock::now();

	auto diff = pngend - pngstart;

	std::cout << "obj took " << std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / 1000000.0 << "ms" << std::endl;
	
	//make sure to output the warnings to the console, in case there are issues with the file
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}
	//if we have any error, print it to the console, and break the mesh loading. 
	//This happens if the file cant be found or is malformed
	if (!err.empty()) {
		std::cerr << err << std::endl;
		return false;
	}

	std::vector<Vertex> _vertices;
	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {

			//hardcode loading to triangles
			int fv = 3;

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

				//vertex position
				tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
				tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
				tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
				//vertex normal
				tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
				tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
				tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

				//vertex uv
				tinyobj::real_t ux = attrib.texcoords[2 * idx.texcoord_index + 0];
				tinyobj::real_t uy = attrib.texcoords[2 * idx.texcoord_index + 1];

				//copy it into our vertex
				Vertex new_vert;
				new_vert.position[0] = vx;
				new_vert.position[1] = vy;
				new_vert.position[2] = vz;

				new_vert.normal[0] = nx;
				new_vert.normal[1] = ny;
				new_vert.normal[2] = nz;


				new_vert.uv[0] = ux;
				new_vert.uv[1] = 1 - uy;

				//we are setting the vertex color as the vertex normal. This is just for display purposes
				//new_vert.color = new_vert.normal;


				_vertices.push_back(new_vert);
			}
			index_offset += fv;
		}
	}

	auto buffer_size = _vertices.size() * sizeof(Vertex);
	nlohmann::json metadata;
	metadata["format"] = "VERTEX";
	metadata["original_file"] = input.string();
	
	metadata["buffer_size"] = buffer_size;

	std::string stringified = metadata.dump();

	File newFile;
	newFile.json = stringified;
	newFile.version = 1;
	newFile.type[0] = 'M';
	newFile.type[1] = 'E';
	newFile.type[2] = 'S';
	newFile.type[3] = 'H';

	auto start = std::chrono::high_resolution_clock::now();

	int compressStaging = LZ4_compressBound(buffer_size);

	newFile.binaryBlob.resize(compressStaging);

	int compressedSize = LZ4_compress_default((const char*)_vertices.data(), newFile.binaryBlob.data(), buffer_size, compressStaging);

	newFile.binaryBlob.resize(compressedSize);
	//newFile.binaryBlob.resize(buffer_size);
	auto  end = std::chrono::high_resolution_clock::now();

	diff = end - start;

	std::cout << "compression took " << std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / 1000000.0 << "ms" << std::endl;

	save_binaryfile(output, newFile);

	File loadImage;





	start = std::chrono::high_resolution_clock::now();
	load_binaryfile(output, loadImage);

	std::vector<char> blob;
	blob.resize(buffer_size);

	LZ4_decompress_safe(loadImage.binaryBlob.data(), blob.data(), loadImage.binaryBlob.size(), blob.size());

	end = std::chrono::high_resolution_clock::now();

	diff = end - start;

	std::cout << "loading and decompression took " << std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / 1000000.0 << "ms" << std::endl;

	return true;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "You need to put the path to the info file";
		return -1;
	}
	else {
		
		fs::path path{ argv[1] };
	
		fs::path directory = path;
		
		std::cout << "loaded asset directory at " << directory << std::endl;

		for (auto& p : fs::directory_iterator(directory))
		{
			std::cout << "File: " << p;

			if (p.path().extension() == ".png") {
				std::cout << "found a texture" << std::endl;

				auto newpath = p.path();
				newpath.replace_extension(".tx");
				convert_image(p.path(), newpath);
			}
			if (p.path().extension() == ".obj") {
				std::cout << "found a mesh" << std::endl;

				auto newpath = p.path();
				newpath.replace_extension(".mesh");
				convert_mesh(p.path(), newpath);
			}
		}

		//else 
		{
			std::cout << "Invalid path: " << argv[1];
			return -1;
		}
	}
	//VulkanEngine engine;
	//
	//engine.init();
	//
	//engine.run();
	//
	//engine.cleanup();
	//
	return 0;
}