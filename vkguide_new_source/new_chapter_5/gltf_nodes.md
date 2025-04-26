---
layout: default
title: GLTF Scene Nodes
parent: "5. GLTF loading"
nav_order: 5
---

We will split GLTF loading into two, first one dealing with nodes and meshes, second one dealing with materials and textures.

The old function for GLTF loading is going to be removed, and we will replace it with the new one that can load scenenodes directly, not just meshes. You can keep the old one around for copy-paste or debugging but it will no longer be needed.

The first thing we are going to need is to create the LoadedGLTF class. This will contain ALL resources needed for handling a single GLTF file worth of data. We werent doing resource management properly before for simpicity purposes, but with this class, it will release the mesh buffers and textures when it gets destroyed. By doing that, we move from having to deal with resources on an individual basis (like keeping track of individual meshes/textures to unload) into a much simpler scheme where we have them grouped. The goal with this is that a user would load one GLTF as a level, and that gltf contains all of the textures, meshes, objects needed for that entire level. The user would then also load another GLTF with characters or objects, and keep it loaded for the game. Just because we load an scene into 1 class doesnt mean its not possible to call the Draw() function on indivual nodes within that LoadedGLTF.

It looks like this, Add it to vk_loader.h

```cpp
struct LoadedGLTF : public IRenderable {

    // storage for all the data on a given glTF file
    std::unordered_map<std::string, std::shared_ptr<MeshAsset>> meshes;
    std::unordered_map<std::string, std::shared_ptr<Node>> nodes;
    std::unordered_map<std::string, AllocatedImage> images;
    std::unordered_map<std::string, std::shared_ptr<GLTFMaterial>> materials;

    // nodes that dont have a parent, for iterating through the file in tree order
    std::vector<std::shared_ptr<Node>> topNodes;

    std::vector<VkSampler> samplers;

    DescriptorAllocatorGrowable descriptorPool;

    AllocatedBuffer materialDataBuffer;

    VulkanEngine* creator;

    ~LoadedGLTF() { clearAll(); };

    virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx);

private:

    void clearAll();
};
```

Objects in GLTF have names, so we are going to store them all into unordered maps. Everything is stored through shared_ptr. This way, we protect against cases like unnamed nodes in case they happen. The entire file is going to form a "graph" of shared ptr connections and keep itself alive.

We have a map of meshes, this will hold the mesh buffers. its the same we had in the mesh-only loader. Then we have a map of scenenodes, created from the transformation tree in the file. Then a map of textures, and another of materials. Textures and materials are separated becuase its common to have the same texture used for multiple materials. The GLTF file format also separates them in this way. 

We store also a vector of the non-parented nodes. This way we can draw() them recursively from the Draw() function in the LoadedGLTF. It can be useful for other reasons too like displaying the nodes in an editor.

Array of VkSamplers too, which matches GLTF format. Its likely this one will barely be a couple. We could be hashing and storing them globally in the engine, but if we keep to the concept that a loaded gltf controls all of the vulkan resources, it simplifies the engine.

We then have a descriptor pool made explicitly for this glTF file. That way way dont need to deal with descriptor sets individually in any point, and can free that to delete all descriptors for the materials in the file.
 
The materialDataBuffer will contain the material data as seen in the GLTFMetallicRoughness material. We will use a single big buffer for the material data of every material in the file.

The rest is some destruction function, the Draw() function, and storing the VulkanEngine in the file so that clearAll can release resources properly. We could be using a singleton instead to avoid storing this pointer if we wanted.

Lets begin writing the new loader code in vk_loader.cpp . We will make it from scratch, and will leave all the materials as default for now. 

```cpp
std::optional<std::shared_ptr<LoadedGLTF>> loadGltf(VulkanEngine* engine,std::string_view filePath)
{
^code load_1 chapter-5/vk_loader.cpp
}
```

We will begin by loading the file. As this is going to be more generic than the one that loaded only meshes, we are adding checking to change between GLTF File and GLB file. Other than that its mostly the same as it was in the mesh only loader.

^code load_2 chapter-5/vk_loader.cpp

Next, we initialize the descriptor pool with an stimate of the amount of descriptors we need. In case we overflow the pool, its using the growable pool so it will just add more VkDescriptorPool as needed.

Lets load the samplers. GLTF samplers are using the numbers and properties from OpenGL, which do not match the vulkan ones, so we are going to have to create some conversion functions.

^code filters chapter-5/vk_loader.cpp

Those 2 functions are global functions on vk_loader.cpp. We wont be needed them outside. 
In vulkan, sampler filter is separated from mipmap mode. So we begin by extracting the filter from the GLTF filter option. `extract_filter()` only looks at the filtering, so it returns either `VK_FILTER_NEAREST` or  `VK_FILTER_LINEAR`. On `extract_mipmap_mode()` we look at the mipmap part, which we return as `VK_SAMPLER_MIPMAP_MODE_NEAREST` or `VK_SAMPLER_MIPMAP_MODE_LINEAR`. Linear will blend mipmaps, while nearest will use a single one with no blending.

Now we can load the samplers from the glTF file.

^code load_samplers chapter-5/vk_loader.cpp

We make a `VkSamplerCreateInfo`, default its max/min LOD setting, and set its filtering settings using the above extract functions. Unlike the default samplers from last chapter, we are setting min/max lod so that we can have them use mipmaps. We will be storing the VkSamplers directly on the LoadedGLTF struct.

Before we begin loading meshes, We are going to create some arrays to hold the structures. In GLTF files, everything works through indices, so we need a way to handle that . For example a mesh node will give the mesh index, not name or anything similar.

^code load_arrays chapter-5/vk_loader.cpp

Now we have to load everything in order. MeshNodes depend on meshes, meshes depend on materials, and materials on textures. So we need to create them in the correct order. We start with the textures. For them, we are just going to copy the default textures we have on the engine, as we will load them later. Lets use the error checkerboard texture as it will be used for images that fail to load after all.

```cpp
// load all textures
for (fastgltf::Image& image : gltf.images) {
   
    images.push_back(engine->_errorCheckerboardImage);
}
```

On materials, we have to precalculate what size we need the buffer that holds all of the material parameters to be. We only have 1 material type, so no issues here, just multiply the size of the uniform buffer structure by the amount of materials

^code load_buffer chapter-5/vk_loader.cpp

We will store the mapped pointer in sceneMaterialConstants to write it.

Now write the loop to load materials.

^code load_material chapter-5/vk_loader.cpp

First, we begin writing the MaterialConstants, loading them from the material information in the GLTF. We load the base color factor, and metallic/roughness factors.

Then, we need to fill the MaterialResources structure. We are going to default the textures and sampler to the default white one. We then hook the materialDataBuffer at the correct offset for the data. Then we check if the material has a color texture (textures on a GLTF material are optional. If they arent set, they generally default to white). If there is a color texture, we hook the texture by index, and sampler by index. We also check if the material is transparent and set the MaterialPass to Blend if thats the case.

Once we have everything, we pass the parameters into the metalRoughMaterial class and write the material. 

Next is loading meshes. We are going to do more or less the same we had in the older loader, with the difference that we are storing the meshes in a different way

```cpp
// use the same vectors for all meshes so that the memory doesnt reallocate as
// often
std::vector<uint32_t> indices;
std::vector<Vertex> vertices;

for (fastgltf::Mesh& mesh : gltf.meshes) {
    std::shared_ptr<MeshAsset> newmesh = std::make_shared<MeshAsset>();
    meshes.push_back(newmesh);
    file.meshes[mesh.name.c_str()] = newmesh;
    newmesh->name = mesh.name;

    // clear the mesh arrays each mesh, we dont want to merge them by error
    indices.clear();
    vertices.clear();

    for (auto&& p : mesh.primitives) {
        GeoSurface newSurface;
        newSurface.startIndex = (uint32_t)indices.size();
        newSurface.count = (uint32_t)gltf.accessors[p.indicesAccessor.value()].count;

        size_t initial_vtx = vertices.size();

        // load indexes
        {
            fastgltf::Accessor& indexaccessor = gltf.accessors[p.indicesAccessor.value()];
            indices.reserve(indices.size() + indexaccessor.count);

            fastgltf::iterateAccessor<std::uint32_t>(gltf, indexaccessor,
                [&](std::uint32_t idx) {
                    indices.push_back(idx + initial_vtx);
                });
        }

        // load vertex positions
        {
            fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->second];
            vertices.resize(vertices.size() + posAccessor.count);

            fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor,
                [&](glm::vec3 v, size_t index) {
                    Vertex newvtx;
                    newvtx.position = v;
                    newvtx.normal = { 1, 0, 0 };
                    newvtx.color = glm::vec4 { 1.f };
                    newvtx.uv_x = 0;
                    newvtx.uv_y = 0;
                    vertices[initial_vtx + index] = newvtx;
                });
        }

        // load vertex normals
        auto normals = p.findAttribute("NORMAL");
        if (normals != p.attributes.end()) {

            fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[(*normals).second],
                [&](glm::vec3 v, size_t index) {
                    vertices[initial_vtx + index].normal = v;
                });
        }

        // load UVs
        auto uv = p.findAttribute("TEXCOORD_0");
        if (uv != p.attributes.end()) {

            fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[(*uv).second],
                [&](glm::vec2 v, size_t index) {
                    vertices[initial_vtx + index].uv_x = v.x;
                    vertices[initial_vtx + index].uv_y = v.y;
                });
        }

        // load vertex colors
        auto colors = p.findAttribute("COLOR_0");
        if (colors != p.attributes.end()) {

            fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[(*colors).second],
                [&](glm::vec4 v, size_t index) {
                    vertices[initial_vtx + index].color = v;
                });
        }

        if (p.materialIndex.has_value()) {
            newSurface.material = materials[p.materialIndex.value()];
        } else {
            newSurface.material = materials[0];
        }

        newmesh->surfaces.push_back(newSurface);
    }

    newmesh->meshBuffers = engine->uploadMesh(indices, vertices);
}
```

The difference  is that at the end, we handle the material index. If there is no material, we will default on the first material in the file. Its rare that a mesh doesnt have a material index, even if its optional in the file, so we arent going to care much about it.


Now we are going to load the Nodes

^code load_nodes chapter-5/vk_loader.cpp

The node loading will be split into two. the first time, we create the nodes, either as base Node class, or MeshNode class, depending if the node has a mesh or not. Then we need to calculate its local matrix, for that, we load the GLTF transform data, and convert it into a gltf final transform matrix.

With the nodes loaded, we need to setup their parenting relationships to build the scene-graph

^code load_graph chapter-5/vk_loader.cpp

First we loop every node, find if it has children, and set the parent/child pointers. Then we loop again, but we find the nodes that dont have a parent, add them to the topNodes array, and refresh their transform. Remember from the last chapter that refreshTransform will recalculate the world matrices recursively based on the parenting relationships, so all of the nodes that are parented to something will get refreshed too.

With this, we have the loading of entire scenes done. and can try to render them. Lets fill the Draw() function.

```cpp
void LoadedGLTF::Draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
    // create renderables from the scenenodes
    for (auto& n : topNodes) {
        n->Draw(topMatrix, ctx);
    }
}
```

The Draw function only loops the top nodes and calls Draw on them, which will propagate to their children.

Leave the clearAll() function empty for now. We arent handling textures properly yet so it will be half-done.

Lets connect this to the VulkanEngine class.

We will be storing the loaded gltfs into a unordered map by name
```cpp
 std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> loadedScenes;
```

Then lets try loading one. This one is included in the project, and its a big scene with 1500 meshes to draw. In debug mode it will take a second or two to load, so be patient.

Add this at the end of the init() function. 
```cpp
    std::string structurePath = { "..\\..\\assets\\structure.glb" };
    auto structureFile = loadGltf(this,structurePath);

    assert(structureFile.has_value());

    loadedScenes["structure"] = *structureFile;
```

We load that asset, and then store it on the hashmap for later use.

The later use is to call Draw() on it as part of the update_scene() function

```cpp
	loadedScenes["structure"]->Draw(glm::mat4{ 1.f }, mainDrawContext);
```
Its a big scene, so we can use it for benchmarking by loading it multiple times at different positions.

When clearing the scenes, we need to do it at the proper moment. In the clear() function, add this. We will be clearing the hashmap, which will clear all of the maps, right after the WaitIdle call, at the start of the vulkan cleanup()

```cpp
 // make sure the gpu has stopped doing its things
 vkDeviceWaitIdle(_device);

 loadedScenes.clear();
```

Try to run the project now, and see if you can explore the map. Its a big one but you can now move the camera. For a "good" default position of the camera, you can change the camera initial position to 
```
 mainCamera.position = glm::vec3(30.f, -00.f, -085.f);
```
in the init() function. This will be a good default location.

Lets complete it by loading textures in the next article.

^nextlink

{% include comments.html term="Vkguide 2 Beta Comments" %}

