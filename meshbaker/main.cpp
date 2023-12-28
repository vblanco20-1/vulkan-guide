#include "stb_image.h"
#include <iostream>

#include "vk_types.h"
#include <glm/gtx/quaternion.hpp>

#include <fastgltf/parser.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include "fmt/format.h"
#include <fstream>

template <> struct fmt::formatter< glm::vec3> {
	constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator {
		return ctx.begin();
	}

	auto format(const  glm::vec3& v, format_context& ctx) const -> format_context::iterator {
		return fmt::format_to(ctx.out(), "{{ {},{},{} }}", v.x, v.y, v.z);
	}
};
template <> struct fmt::formatter< glm::vec4> {
	constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator {
		return ctx.begin();
	}
	auto format(const  glm::vec4& v, format_context& ctx) const -> format_context::iterator {
		return fmt::format_to(ctx.out(), "{{ {},{},{},{} }}", v.x, v.y, v.z,v.w);
	}
};
template <> struct fmt::formatter< Vertex> {
	constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator {
		return ctx.begin();
	}
	auto format(const Vertex& v, format_context& ctx) const -> format_context::iterator {
		return fmt::format_to(ctx.out(), "Vertex{{ {}, {}, {}, {} ,{} }}",
			v.position, v.uv_x, v.normal, v.uv_y, v.color);
	}
	};
std::string write_mesh_vertices(std::span<Vertex> vtx, std::string_view name) {
	std::string numbers = fmt::format("{}", fmt::join(vtx,","));

	return fmt::format(" Vertex {}[] = {{ {} }}; \n ",name, numbers);
}
std::string write_mesh_indices(std::span<uint32_t> idx, std::string_view name) {

	std::string numbers = fmt::format("{}", fmt::join(idx,","));

	return fmt::format(" uint32_t {}[] = {{ {} }}; \n ",name, numbers);
}

void output_meshes(std::filesystem::path target_file, std::filesystem::path source_gltf) {

	fastgltf::Parser parser{};

	constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble | fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;
	// fastgltf::Options::LoadExternalImages;

	fastgltf::GltfDataBuffer data;
	data.loadFromFile(source_gltf);

	fastgltf::Asset gltf;

	auto type = fastgltf::determineGltfFileType(&data);
	if (type == fastgltf::GltfType::glTF) {
		auto load = parser.loadGLTF(&data, source_gltf.parent_path(), gltfOptions);
		if (load) {
			gltf = std::move(load.get());
		}
		else {
			std::cerr << "Failed to load glTF: " << fastgltf::getErrorMessage(load.error()) << std::endl;
		}
	}
	else if (type == fastgltf::GltfType::GLB) {
		auto load = parser.loadBinaryGLTF(&data, source_gltf.parent_path(), gltfOptions);
		if (load) {
			gltf = std::move(load.get());
		}
		else {
			std::cerr << "Failed to load glTF: " << fastgltf::getErrorMessage(load.error()) << std::endl;
		}
	}
	else {
		std::cerr << "Failed to determine glTF container" << std::endl;
	}

	std::vector<uint32_t> indices;
	std::vector<Vertex> vertices;

	std::ofstream ofs (target_file, std::ofstream::out);
	ofs<<  "#include <meshes.h> \n";
	for (fastgltf::Mesh& mesh : gltf.meshes) {
	
		// clear the mesh arrays each mesh, we dont want to merge them by error
		indices.clear();
		vertices.clear();

		for (auto&& p : mesh.primitives) {
			
			{
				fastgltf::Accessor& indexaccessor = gltf.accessors[p.indicesAccessor.value()];

				fastgltf::iterateAccessor<std::uint32_t>(gltf, indexaccessor, [&](std::uint32_t idx) {
					indices.push_back(idx);
					});
			}

			fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->second];

			vertices.resize(posAccessor.count);

			size_t vidx = 0;
			fastgltf::iterateAccessor<glm::vec3>(gltf, posAccessor,
				[&](glm::vec3 v) { vertices[vidx++].position = v; });

			auto normals = p.findAttribute("NORMAL");
			if (normals != p.attributes.end()) {
				vidx = 0;
				fastgltf::iterateAccessor<glm::vec3>(gltf, gltf.accessors[(*normals).second],
					[&](glm::vec3 v) { vertices[vidx++].normal = v; });
			}

			auto uv = p.findAttribute("TEXCOORD_0");
			if (uv != p.attributes.end()) {
				vidx = 0;
				fastgltf::iterateAccessor<glm::vec2>(gltf, gltf.accessors[(*uv).second], [&](glm::vec2 v) {

					vertices[vidx].uv_x = v.x;
					vertices[vidx].uv_y = v.y;
					vidx++;
				});
			}

			auto colors = p.findAttribute("COLOR_0");
			if (colors != p.attributes.end()) {
				vidx = 0;
				fastgltf::iterateAccessor<glm::vec4>(gltf, gltf.accessors[(*colors).second],
					[&](glm::vec4 v) { vertices[vidx++].color = v; });
			}
			else {
				for (auto& v : vertices) {
					v.color = glm::vec4(1.f);
				}
			}

			
			std::string verts = write_mesh_vertices(vertices,mesh.name + "_vtx");
			std::string inds = write_mesh_indices(indices, mesh.name + "_idx");
			
			
			ofs << fmt::format("uint32_t {}_count = {};\n",mesh.name + "_vtx", vertices.size());
			ofs << fmt::format("uint32_t {}_count = {};\n",mesh.name + "_idx", indices.size());
			ofs << verts;

			ofs<<inds;
		}
	}
}

void main() {


	output_meshes("meshes.cpp","../../assets/basicmesh.glb");
}