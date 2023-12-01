---
layout: default
title: Mesh Loading
parent: "New 3. Graphics Pipelines"
nav_order: 12
---

We will do proper scene loading later, but until we go there, we need something better than a rectangle as a mesh. For that, we will begin to load GLTF files but in a very simplified and wrong way, getting only the geometry data and ignoring everything else. 

There is a gltf file that came with the starting-point repository, called basic.glb. That one has a cube, a sphere, and a monkey head meshes centered in the origin. Being a file as simple as that one is, its easy to load it correctly without having to setup up real gltf loading.

A GLTF file will contain a list of meshes, each mesh with multiple primitives on it. This separation is for meshes that use multiple materials, and thus need multiple draw calls to draw it. The file also contains a scene-tree of scenenodes, some of them containing meshes. We will only be loading the meshes now, but later we will have the full scene-tree and materials loaded. 

Our loading code will all be on the files vk_loader.cpp/h . 

Lets start by adding a couple classes for the loaded meshes

```cpp
struct GeoSurface {
    uint32_t startIndex;
    uint32_t count;
};

struct MeshAsset {
    std::string name;

    std::vector<GeoSurface> surfaces;
    GPUMeshBuffers meshBuffers;
};
```

A given mesh asset will have a name, loaded from the file, and then the mesh buffers. But it will also have an array of GeoSurface that has the sub-meshes of this specific mesh. When rendering each submesh will be its own draw.  We will use StartIndex and count for that drawcall as we will be appending all the vertex data of each surface into the same buffer.

Our load function will be this
```cpp
std::optional<std::vector<MeshAsset>> loadGltfMeshes(VulkanEngine* engine, std::string_view filePath);
```

This is the first time see see std::optional being used. This is standard class that wraps a type (the vector of mesh assets here) and allows for it to be errored/null. As file loading can fail for many reasons, it returning null is a good idea. We will be using fastGltf library, which uses all of those new stl features for its loading.

Lets begin by opening the file
```cpp
std::cout << "Loading GLTF: " << filePath << std::endl;

constexpr auto gltfOptions = fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;

fastgltf::GltfDataBuffer data;
data.loadFromFile(filePath);

auto load = parser.loadBinaryGLTF(&data, filePath.parent_path(), gltfOptions);
if (load) {
    gltf = std::move(load.get());
}
else {
    fmt::print("Failed to load glTF: {} \n",fastgltf::to_underlying(load.error()));		
    return {};
}
```

We will only be supporting binary GLTF for this for now. So first we open the file with `loadFromFile` and then we call loadBinaryGLTF to open it. This requires the parent path to find relative paths even if we wont have it yet.

Next we will loop each mesh, copy the vertices and indices into temporary std::vector, and upload them as a mesh to the engine. We will be building an array of `MeshAsset` from this.

```cpp
	std::vector<uint32_t> indices;
	std::vector<Vertex> vertices;
	for (fastgltf::Mesh& mesh : gltf.meshes) {
		MeshAsset newmesh;
		
		newmesh.name = mesh.name;

		// clear the mesh arrays each mesh, we dont want to merge them by error
		indices.clear();
		vertices.clear();

		for (auto&& p : mesh.primitives) {
			GeoSurface newSurface;
			newSurface.startIndex = (uint32_t)indices.size();
			newSurface.count = (uint32_t)gltf.accessors[p.indicesAccessor.value()].count;

            size_t initial_vtx = vertices.size();
			{
				fastgltf::Accessor& indexaccessor = gltf.accessors[p.indicesAccessor.value()];

				fastgltf::iterateAccessor<std::uint32_t>(gltf, indexaccessor, [&](std::uint32_t idx) {
					indices.push_back(idx + initial_vtx);
				});
			}

			fastgltf::Accessor& posAccessor =gltf.accessors[p.findAttribute("POSITION")->second];

			size_t vidx = initial_vtx;
			fastgltf::iterateAccessor<glm::vec3>(gltf, posAccessor,
				[&](glm::vec3 v) { vertices[vidx++].position = v; 
                    Vertex newvtx;
                    newvtx.position = v;
                    newvtx.normal = {1,0,0};
                    newvtx.color = glm::vec4{1.f};
                    newvtx.uv_x = 0;
                    newvtx.uv_y = 0;
                    vertices.push_back(newvtx);
             });

			auto normals = p.findAttribute("NORMAL");
			if (normals != p.attributes.end()) {
				vidx = initial_vtx;
				fastgltf::iterateAccessor<glm::vec3>(gltf, gltf.accessors[(*normals).second],
					[&](glm::vec3 v) { vertices[vidx++].normal = v; });
			}

			auto uv = p.findAttribute("TEXCOORD_0");
			if (uv != p.attributes.end()) {
				vidx = initial_vtx;
				fastgltf::iterateAccessor<glm::vec2>(gltf, gltf.accessors[(*uv).second], [&](glm::vec2 v) {

					vertices[vidx].uv_x = v.x;
					vertices[vidx].uv_y = v.y;
					vidx++;
				});
			}

			auto colors = p.findAttribute("COLOR_0");
			if (colors != p.attributes.end()) {
				vidx = initial_vtx;
				fastgltf::iterateAccessor<glm::vec4>(gltf, gltf.accessors[(*colors).second],
					[&](glm::vec4 v) { vertices[vidx++].color = v; });
			}

			newmesh.surfaces.push_back(newSurface);
		}

		newmesh.meshBuffers = engine->uploadMesh(indices, vertices);

        meshes.emplace_back(std::move(newmesh));
	}

	return meshes;
```

As we iterate each primitive within a mesh, we use the iterateAccessor functions to access the vertex data we want. We also build the index buffer properly while appending the different primitives into the vertex arrays. At the end, we call uploadMesh to create the final buffers, then we return the mesh list.

The Position array is going to be there always, so we use that to initialize the Vertex structures. For all the other attributes we need to do it checking that the data exists.

Lets draw them.

```
	GPUMeshBuffers rectangle;
	std::vector<MeshAsset> testMeshes;
```

we begin by adding them to the VulkanEngine class. Lets load them from init_default_data()

```
testMeshes = loadGltfMeshes(this,"..\\..\\assets\\structure.glb").value();
```

In the file provided, index 0 is a cube, index 1 is a sphere, and index 2 is a blender monkeyhead. we will be drawing that last one, draw it right after drawing the rectangle from before

```cpp
vkCmdPushConstants(cmd, _meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VkDeviceAddress), &testMeshes[2].meshBuffers.vertexBufferAddress);
vkCmdBindIndexBuffer(cmd, testMeshes[2].meshBuffers.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

vkCmdDrawIndexed(cmd, testMeshes[2].surfaces[0].count, 1, testMeshes[2].surfaces[0].startIndex, 0, 0);
```

You will see that the monkey head is completely white due to the vertex color. Lets modify the loading code a little bit to force color to be the vertex normal. this way we can display something better for now until we have fancier shaders.

```cpp
for (Vertex& vtx : vertices) {
    vtx.color = glm::vec4(vtx.normal, 1.f);
}

newmesh.meshBuffers = engine->uploadMesh(indices, vertices);
```

Now we have the monkey head and its visible, but its also upside down. we need to pass object matrix to the shader.