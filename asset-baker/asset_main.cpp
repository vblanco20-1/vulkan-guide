#include <iostream>
#include <json.hpp>
#include <fstream>
#include <filesystem>

#include <lz4.h>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "tiny_obj_loader.h"

#include <asset_loader.h>
#include <texture_asset.h>
#include <mesh_asset.h>
#include <material_asset.h>

#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>
#include "prefab_asset.h"

#include <nvtt.h>

namespace fs = std::filesystem;

using namespace assets;


struct ConverterState {
	fs::path asset_path;
	fs::path export_path;

	fs::path convert_to_export_relative(fs::path path)const;
};

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

	TextureInfo texinfo;
	texinfo.textureSize = texture_size;
	
	texinfo.textureFormat = TextureFormat::RGBA8;
	texinfo.originalFile = input.string();
	auto start = std::chrono::high_resolution_clock::now();

	std::vector<char> all_buffer;

	struct DumbHandler : nvtt::OutputHandler {
		// Output data. Compressed data is output as soon as it's generated to minimize memory allocations.
		virtual bool writeData(const void* data, int size) {
			for (int i = 0; i < size; i++) {
				buffer.push_back(((char*)data)[i]);
			}
			return true;
		}
		virtual void beginImage(int size, int width, int height, int depth, int face, int miplevel) { };

		// Indicate the end of the compressed image. (New in NVTT 2.1)
		virtual void endImage() {};
		std::vector<char> buffer;
	};


	
	nvtt::Compressor compressor;

	nvtt::CompressionOptions optiuns;
	nvtt::OutputOptions outputOptions;
	nvtt::Surface surface;

	DumbHandler handler;
	outputOptions.setOutputHandler(&handler);

	surface.setImage(nvtt::InputFormat::InputFormat_BGRA_8UB, texWidth, texHeight, 1, pixels);


	while (surface.canMakeNextMipmap(1))
	{
			surface.buildNextMipmap(nvtt::MipmapFilter_Box);

			optiuns.setFormat(nvtt::Format::Format_RGBA);
			optiuns.setPixelType(nvtt::PixelType_UnsignedNorm);

			compressor.compress(surface, 0, 0, optiuns, outputOptions);

			texinfo.pages.push_back({});
			texinfo.pages.back().width = surface.width();
			texinfo.pages.back().height = surface.height();
			texinfo.pages.back().originalSize = handler.buffer.size();

			all_buffer.insert(all_buffer.end(), handler.buffer.begin(), handler.buffer.end());
			handler.buffer.clear();
	}
	

	texinfo.textureSize = all_buffer.size();
	assets::AssetFile newImage = assets::pack_texture(&texinfo, all_buffer.data());

	auto  end = std::chrono::high_resolution_clock::now();



	std::cout << "compression took " << std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / 1000000.0 << "ms" << std::endl;
		

	stbi_image_free(pixels);

	save_binaryfile(output.string().c_str(), newImage);

	return true;
}


void pack_vertex(assets::Vertex_f32_PNCV& new_vert, tinyobj::real_t vx, tinyobj::real_t vy, tinyobj::real_t vz, tinyobj::real_t nx, tinyobj::real_t ny, tinyobj::real_t nz, tinyobj::real_t ux, tinyobj::real_t uy)
{
	new_vert.position[0] = vx;
	new_vert.position[1] = vy;
	new_vert.position[2] = vz;

	new_vert.normal[0] = nx;
	new_vert.normal[1] = ny;
	new_vert.normal[2] = nz;

	new_vert.uv[0] = ux;
	new_vert.uv[1] = 1 - uy;
}
void pack_vertex(assets::Vertex_P32N8C8V16& new_vert, tinyobj::real_t vx, tinyobj::real_t vy, tinyobj::real_t vz, tinyobj::real_t nx, tinyobj::real_t ny, tinyobj::real_t nz, tinyobj::real_t ux, tinyobj::real_t uy)
{
	new_vert.position[0] = vx;
	new_vert.position[1] = vy;
	new_vert.position[2] = vz;

	new_vert.normal[0] = uint8_t(  ((nx + 1.0) / 2.0) * 255);
	new_vert.normal[1] = uint8_t(  ((ny + 1.0) / 2.0) * 255);
	new_vert.normal[2] = uint8_t(  ((nz + 1.0) / 2.0) * 255);

	new_vert.uv[0] = ux;
	new_vert.uv[1] = 1 - uy;
}

template<typename V>
void extract_mesh_from_obj(std::vector<tinyobj::shape_t>& shapes, tinyobj::attrib_t& attrib, std::vector<uint32_t>& _indices, std::vector<V>& _vertices)
{
	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {

			//hardcode loading to triangles
			int fv = 3;

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to assets::Vertex_f32_PNCV
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
				V new_vert;
				pack_vertex(new_vert, vx, vy, vz, nx, ny, nz, ux, uy);


				_indices.push_back(_vertices.size());
				_vertices.push_back(new_vert);
			}
			index_offset += fv;
		}
	}
}

bool convert_mesh(const fs::path& input, const fs::path& output)
{
	//attrib will contain the assets::Vertex_f32_PNCV arrays of the file
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

	using VertexFormat = assets::Vertex_P32N8C8V16;
	auto VertexFormatEnum = assets::VertexFormat::P32N8C8V16;

	std::vector<VertexFormat> _vertices;
	std::vector<uint32_t> _indices;

	extract_mesh_from_obj(shapes, attrib, _indices, _vertices);


	MeshInfo meshinfo;
	meshinfo.vertexFormat = VertexFormatEnum;
	meshinfo.vertexBuferSize = _vertices.size() * sizeof(VertexFormat);
	meshinfo.indexBuferSize = _indices.size() * sizeof(uint32_t);
	meshinfo.indexSize = sizeof(uint32_t);
	meshinfo.originalFile = input.string();	

	//pack mesh file
	auto start = std::chrono::high_resolution_clock::now();

	assets::AssetFile newFile = assets::pack_mesh(&meshinfo, (char*)_vertices.data(), (char*)_indices.data());
	
	auto  end = std::chrono::high_resolution_clock::now();

	diff = end - start;

	std::cout << "compression took " << std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / 1000000.0 << "ms" << std::endl;

	//save to disk
	save_binaryfile(output.string().c_str(), newFile);
	
	return true;
}

void unpack_gltf_buffer(tinygltf::Model& model, tinygltf::Accessor& accesor, std::vector<uint8_t> &outputBuffer)
{
	int bufferID = accesor.bufferView;
	size_t elementSize = tinygltf::GetComponentSizeInBytes(accesor.componentType);

	tinygltf::BufferView& bufferView = model.bufferViews[bufferID];	

	tinygltf::Buffer& bufferData = (model.buffers[bufferView.buffer]);


	uint8_t* dataptr = bufferData.data.data() + accesor.byteOffset + bufferView.byteOffset;

	int components = tinygltf::GetNumComponentsInType(accesor.type);

	elementSize *= components;

	size_t stride = bufferView.byteStride;
	if (stride == 0)
	{
		stride = elementSize;
		
	}

	outputBuffer.resize(accesor.count * elementSize);

	for (int i = 0; i < accesor.count; i++) {
		uint8_t* dataindex = dataptr + stride * i;
		uint8_t* targetptr = outputBuffer.data() + elementSize * i;

		memcpy(targetptr, dataindex, elementSize);	
	}
}
void extract_gltf_vertices(tinygltf::Primitive& primitive, tinygltf::Model& model, std::vector<assets::Vertex_f32_PNCV>& _vertices)
{
	
	tinygltf::Accessor& pos_accesor = model.accessors[primitive.attributes["POSITION"]];

	_vertices.resize(pos_accesor.count);

	std::vector<uint8_t> pos_data;
	unpack_gltf_buffer(model, pos_accesor, pos_data);
	

	for (int i = 0; i < _vertices.size(); i++) {
		if (pos_accesor.type == TINYGLTF_TYPE_VEC3)
		{
			if (pos_accesor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
			{
				float* dtf = (float*)pos_data.data();

				//vec3f 
				_vertices[i].position[0] = *(dtf + (i * 3) + 0);
				_vertices[i].position[1] = *(dtf + (i * 3) + 1);
				_vertices[i].position[2] = *(dtf + (i * 3) + 2);
			}
			else {
				assert(false);
			}
		}
		else {
			assert(false);
		}
	}

	tinygltf::Accessor& normal_accesor = model.accessors[primitive.attributes["NORMAL"]];

	std::vector<uint8_t> normal_data;
	unpack_gltf_buffer(model, normal_accesor, normal_data);


	for (int i = 0; i < _vertices.size(); i++) {
		if (normal_accesor.type == TINYGLTF_TYPE_VEC3)
		{
			if (normal_accesor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
			{
				float* dtf = (float*)normal_data.data();

				//vec3f 
				_vertices[i].normal[0] = *(dtf + (i * 3) + 0);
				_vertices[i].normal[1] = *(dtf + (i * 3) + 1);
				_vertices[i].normal[2] = *(dtf + (i * 3) + 2);

				_vertices[i].color[0] = *(dtf + (i * 3) + 0);
				_vertices[i].color[1] = *(dtf + (i * 3) + 1);
				_vertices[i].color[2] = *(dtf + (i * 3) + 2);
			}
			else {
				assert(false);
			}
		}
		else {
			assert(false);
		}
	}

	tinygltf::Accessor& uv_accesor = model.accessors[primitive.attributes["TEXCOORD_0"]];

	std::vector<uint8_t> uv_data;
	unpack_gltf_buffer(model, uv_accesor, uv_data);


	for (int i = 0; i < _vertices.size(); i++) {
		if (uv_accesor.type == TINYGLTF_TYPE_VEC2)
		{
			if (uv_accesor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
			{
				float* dtf = (float*)uv_data.data();

				//vec3f 
				_vertices[i].uv[0] = *(dtf + (i * 2) + 0);
				_vertices[i].uv[1] = *(dtf + (i * 2) + 1);
			}
			else {
				assert(false);
			}
		}
		else {
			assert(false);
		}
	}

	return;
}


void extract_gltf_indices(tinygltf::Primitive& primitive, tinygltf::Model& model, std::vector<uint32_t>& _primindices)
{
	int indexaccesor = primitive.indices;	

	int indexbuffer = model.accessors[indexaccesor].bufferView;
	int componentType = model.accessors[indexaccesor].componentType;
	size_t indexsize = tinygltf::GetComponentSizeInBytes(componentType);

	tinygltf::BufferView& indexview = model.bufferViews[indexbuffer];
	int bufferidx = indexview.buffer;

	tinygltf::Buffer& buffindex = (model.buffers[bufferidx]);

	uint8_t* dataptr = buffindex.data.data() + indexview.byteOffset;

	std::vector<uint8_t> unpackedIndices;
	unpack_gltf_buffer(model, model.accessors[indexaccesor], unpackedIndices);

	for (int i = 0; i < model.accessors[indexaccesor].count; i++) {

		uint32_t index;
		switch (componentType) {
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
		{
			uint16_t* bfr = (uint16_t*)unpackedIndices.data();
			index = *(bfr + i);
		}
		break;
		case TINYGLTF_COMPONENT_TYPE_SHORT:
		{
			int16_t* bfr = (int16_t*)unpackedIndices.data();
			index = *(bfr + i);
		}
		break;
		default:
			assert(false);
		}

		_primindices.push_back(index);
	}
}

std::string calculate_gltf_mesh_name(tinygltf::Model& model, int meshIndex, int primitiveIndex)
{
	char buffer0[50];
	char buffer1[50];
	itoa(meshIndex, buffer0, 10);
	itoa(primitiveIndex, buffer1, 10);

	std::string meshname = "MESH_" + std::string{ &buffer0[0] } + "_" + model.meshes[meshIndex].name;

	bool multiprim = model.meshes[meshIndex].primitives.size() > 1;
	if (multiprim)
	{
		meshname += "_PRIM_" + std::string{ &buffer1[0] };

		
	}
	
	return meshname;
}
bool extract_gltf_meshes(tinygltf::Model& model, const fs::path& input, const fs::path& outputFolder, const ConverterState& convState)
{
	tinygltf::Model* glmod = &model;
	for (auto meshindex = 0; meshindex < model.meshes.size(); meshindex++){

		auto& glmesh = model.meshes[meshindex];
		

		using VertexFormat = assets::Vertex_f32_PNCV;
		auto VertexFormatEnum = assets::VertexFormat::PNCV_F32;

		std::vector<VertexFormat> _vertices;
		std::vector<uint32_t> _indices;

		for (auto primindex = 0; primindex < glmesh.primitives.size(); primindex++){

			_vertices.clear();
			_indices.clear();

			std::string meshname = calculate_gltf_mesh_name(model, meshindex, primindex);

			auto& primitive = glmesh.primitives[primindex];
			
			extract_gltf_indices(primitive, model, _indices);
			extract_gltf_vertices(primitive, model, _vertices);
			

			MeshInfo meshinfo;
			meshinfo.vertexFormat = VertexFormatEnum;
			meshinfo.vertexBuferSize = _vertices.size() * sizeof(VertexFormat);
			meshinfo.indexBuferSize = _indices.size() * sizeof(uint32_t);
			meshinfo.indexSize = sizeof(uint32_t);
			meshinfo.originalFile = input.string();


			assets::AssetFile newFile = assets::pack_mesh(&meshinfo, (char*)_vertices.data(), (char*)_indices.data());

			fs::path meshpath = outputFolder / (meshname + ".mesh");


			//save to disk
			save_binaryfile(meshpath.string().c_str(), newFile);
		}
	}
	return true;
}


std::string calculate_gltf_material_name(tinygltf::Model& model, int materialIndex)
{
	char buffer[50];

	itoa(materialIndex, buffer, 10);
	std::string matname = "MAT_" + std::string{ &buffer[0] } + "_" + model.materials[materialIndex].name;
	return matname;
}

void extract_gltf_materials(tinygltf::Model& model, const fs::path& input, const fs::path& outputFolder, const ConverterState& convState)
{

	int nm = 0;
	for (auto& glmat : model.materials) {
		std::string matname = calculate_gltf_material_name(model, nm);


		nm++;
		auto& pbr = glmat.pbrMetallicRoughness;


		assets::MaterialInfo newMaterial;
		newMaterial.baseEffect = "defaultPBR";

		auto baseColor = model.textures[pbr.baseColorTexture.index];
		auto baseImage = model.images[baseColor.source];

		fs::path baseColorPath = outputFolder.parent_path() / baseImage.uri;

		baseColorPath.replace_extension(".tx");

		baseColorPath = convState.convert_to_export_relative(baseColorPath);

		newMaterial.textures["baseColor"] = baseColorPath.string();

		fs::path materialPath = outputFolder / (matname + ".mat");

		

		assets::AssetFile newFile = assets::pack_material(&newMaterial);

		//save to disk
		save_binaryfile(materialPath.string().c_str(), newFile);
	}
}

void extract_gltf_nodes(tinygltf::Model& model, const fs::path& input, const fs::path& outputFolder, const ConverterState& convState)
{
	assets::PrefabInfo prefab;

	std::vector<uint64_t> meshnodes;
	for (int i = 0; i < model.nodes.size(); i++)
	{
		auto& node = model.nodes[i];

		std::string nodename = node.name;

		prefab.node_names[i] = nodename;

		//node has a matrix
		if (node.matrix.size() > 0)
		{
			std::array<float, 16> matrix;

			for (int n = 0; n < 16; n++) {
				matrix[n] = node.matrix[n];
			}

			prefab.node_matrices[i] = prefab.matrices.size();
			prefab.matrices.push_back(matrix);
		}
		if (node.mesh >= 0)
		{
			auto mesh = model.meshes[node.mesh];

			if (mesh.primitives.size() > 1) {			
				meshnodes.push_back(i);
			}
			else {
				auto primitive = mesh.primitives[0];
				std::string meshname = calculate_gltf_mesh_name(model, node.mesh, 0);
				
				fs::path meshpath = outputFolder / (meshname + ".mesh");

				int material = primitive.material;

				std::string matname = calculate_gltf_material_name(model, material);

				fs::path materialpath = outputFolder / (matname + ".mat");

				assets::PrefabInfo::NodeMesh nmesh;
				nmesh.mesh_path = convState.convert_to_export_relative(meshpath).string();
				nmesh.material_path = convState.convert_to_export_relative(materialpath).string();

				prefab.node_meshes[i] = nmesh;
			}
		}
	}


	int nodeindex = model.nodes.size();
	//iterate nodes with mesh, convert each submesh into a node
	for (int i = 0; i < meshnodes.size(); i++)
	{
		auto& node = model.nodes[i];

		auto mesh = model.meshes[node.mesh];
		

		for (int primindex = 0 ; primindex < mesh.primitives.size(); primindex++)
		{
			auto primitive = mesh.primitives[primindex];
			int newnode = nodeindex++;

			char buffer[50];

			itoa(primindex, buffer, 10);

			prefab.node_names[newnode] = prefab.node_names[i] +  "_PRIM_" + &buffer[0];

			int material = primitive.material;
			auto mat = model.materials[material];
			std::string matname = calculate_gltf_material_name(model, material);
			std::string meshname = calculate_gltf_mesh_name(model, node.mesh, primindex);

			fs::path materialpath = outputFolder / (matname + ".mat");
			fs::path meshpath = outputFolder / (meshname + ".mesh");

			assets::PrefabInfo::NodeMesh nmesh;
			nmesh.mesh_path = convState.convert_to_export_relative(meshpath).string();
			nmesh.material_path = convState.convert_to_export_relative(materialpath).string();

			prefab.node_meshes[newnode] = nmesh;
		}
		
	}


	assets::AssetFile newFile = assets::pack_prefab(prefab);

	fs::path scenefilepath = (outputFolder.parent_path()) / input.stem();

	scenefilepath.replace_extension(".pfb");

	//save to disk
	save_binaryfile(scenefilepath.string().c_str(), newFile);
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
		
		fs::path exported_dir = path.parent_path() / "assets_export";

		std::cout << "loaded asset directory at " << directory << std::endl;

		ConverterState convstate;
		convstate.asset_path = path;
		convstate.export_path = exported_dir;

		for (auto& p : fs::recursive_directory_iterator(directory))
		{
			std::cout << "File: " << p << std::endl;

			auto relative = p.path().lexically_proximate(directory);

			auto export_path = exported_dir / relative;			

			if (!fs::is_directory(export_path.parent_path()))
			{
				fs::create_directory(export_path.parent_path());
			}

			//if (p.path().extension() == ".png" || p.path().extension() == ".jpg") {
			//	std::cout << "found a texture" << std::endl;
			//
			//	auto newpath = p.path();
			//
			//	
			//	export_path.replace_extension(".tx");
			//
			//	convert_image(p.path(), export_path);
			//}
			//if (p.path().extension() == ".obj") {
			//	std::cout << "found a mesh" << std::endl;
			//
			//	export_path.replace_extension(".mesh");
			//	convert_mesh(p.path(), export_path);
			//}
			if (p.path().extension() == ".gltf")
			{
				using namespace tinygltf;
				Model model;
				TinyGLTF loader;
				std::string err;
				std::string warn;
			
				bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, p.path().string().c_str());
			
				if (!warn.empty()) {
					printf("Warn: %s\n", warn.c_str());
				}
			
				if (!err.empty()) {
					printf("Err: %s\n", err.c_str());
				}
			
				if (!ret) {
					printf("Failed to parse glTF\n");
					return -1;
				}
				else {
					auto folder = export_path.parent_path() / (p.path().stem().string() + "_GLTF");
					fs::create_directory(folder);
			
					extract_gltf_meshes(model, p.path(), folder, convstate);
			
					extract_gltf_materials(model, p.path(), folder, convstate);
			
					extract_gltf_nodes(model, p.path(), folder, convstate);
				}
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

fs::path ConverterState::convert_to_export_relative(fs::path path) const
{
	return path.lexically_proximate(export_path);
}
