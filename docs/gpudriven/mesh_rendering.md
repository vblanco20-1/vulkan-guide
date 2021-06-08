---
layout: default
title: Mesh Rendering
parent: GPU Driven Rendering
nav_order: 10
---

Now that the material system is explained, we can go forward to the meat of it, the mesh rendering system.

Most of the system code is on `vk_scene.h/cpp` files. The part that does the rendering commands for it is done in `vk_engine_scenerender.h/cpp` files.

The design of the system is inspired by work shown in Unreal Engine and OurMachinery blogs and presentations. It's not the best solution, but it's one that does work fairly well.


## Mesh Passes

The main idea of the system is that rendering is done through a `MeshPass`, a given mesh pass holds the information needed to render a pass of meshes for a part of the renderer. At the time of writing, the engine has 3 meshpasses, Forward rendering, Sun Shadow render, and Transparent render.

There can be many more mesh passes with no issues. If you wanted to have pointlights that also cast shadow, you would have 1 mesh-pass per shadowcasting light, and if you had portals wich require more cameras, you would have more Forward meshpasses. A very clear improvement over the code is to have a MeshPass for dynamic objects, to have it separated from the static level load objects. Another improvement would be to split the big forward-meshpass into tiles for an open world game, so that the different forward passes can update at different rates.

A MeshPass can be expensive to rebuild, even with the systems that allow incremental rebuild of it. A good part of its structures need to be rebuilt if objects are added or removed from the renderpass. Moving objects or changing their properties does not require rebuilding a MeshPass, only add/remove or material switch needs it.

Because the MeshPass is a fully standalone collection of drawcalls, they can be saved, restored, and recalculated in parallel. This automatically gives the possibility of rebuilding drawcalls for shadowcasting in parallel without any issues.

A meshpass is composed of a set of flat arrays of the data inside it. Being fully flat, it makes it easy to syncronize its data with the GPU.

```cpp
    // final draw-indirect segments
    std::vector<RenderScene::Multibatch> multibatches;
    // draw indirect batches
	std::vector<RenderScene::IndirectBatch> batches;
    // sorted list of objects in the pass
	std::vector<RenderScene::RenderBatch> flat_batches;

    //unsorted object data
	std::vector<PassObject> objects;

    //objects pending addition
    std::vector<Handle<RenderObject>> unbatchedObjects;

    //indicides for the objects array that can be reused
	std::vector<Handle<PassObject>> reusableObjects;

    //objects pending removal
	std::vector<Handle<PassObject>> objectsToDelete;
```

The main rendering ones are `multibatches`, `batches`, and `flat_batches`. Each of them is sorted, and they map directly into drawcalls.

```cpp
struct RenderBatch {
		Handle<PassObject> object;
		uint64_t sortKey;
```

Flat_batches is the individual non-instanced draws for every object in the pass. This one can get very big if there are enough objects. If you don't want to use draw indirect, you can render this array directly. They are just a handle to the objects array + the calculated sort key. Flat-batches array will be kept in-order allways.

```cpp
struct IndirectBatch {
		Handle<DrawMesh> meshID;
		PassMaterial material;
		uint32_t first;
		uint32_t count;
	};
```
Batches is an array of DrawIndirect data, each of them covering a range on the flat-batches array. A given batch maps directly to a single VkDrawIndirectCommand, using instanced rendering to draw a set of objects.

```cpp
struct Multibatch {
		uint32_t first;
		uint32_t count;
	};
```
Multibatches is another level of indirection for the Batches array. It contains segments of draw commands that can be executed together. This way each VkDrawIndirect call can render multiple VkDrawIndirectCommands at the same time.


```cpp
struct PassMaterial {
		VkDescriptorSet materialSet;
		vkutil::ShaderPass* shaderPass;
	};
struct PassObject {
		PassMaterial material;
		Handle<DrawMesh> meshID;
		Handle<RenderObject> original;
		uint32_t customKey;
	};
```

Pass Object array is a list of the objects handled by this meshpass. Each of them holds a simplified material (only material descriptor set + pass) and handles to the mesh and original render object. When the meshpass updates, it will use this array to build the actual drawcalls.
The `objects` array will have holes, as when objects get deleted, they just get sent to null, and the indices will be reused later. We hold those indices into the `reusableObjects` array.

To render a meshpass, first you have to execute the culling compute shader that will build its draw-indirect buffers, and then you can go over the `multibatch` array executing the drawcalls. This is shown in `vk_engine_scenerender.cpp` on the `execute_draw_commands()` function.


## Render Scene

The mesh passes have to be stored somewhere, and that is the RenderScene. The Render Scene is what handles all of the objects and inserts them into the correct meshpass. Once a renderable object is inserted into the render scene, it is transformed into a more optimal and small structure that maps better to the data inside the RenderScene.

The RenderScene stores objects, meshes, and materials in flat arrays, so that the meshpasses can then refer to those using an integer handle, which is safer to use and smaller in memory than a normal pointer.

```cpp
std::vector<RenderObject> renderables;
std::vector<DrawMesh> meshes;
std::vector<vkutil::Material*> materials;

std::vector<Handle<RenderObject>> dirtyObjects;
```
It also keeps a dirtyObjects list to know which objects it has to reupload to the GPU. The RenderScene will handle keeping the object data in its proper buffers, as all the meshpasses will use the same object data for culling and object transform.

Objects are registered into the RenderScene by calling the `register_object_batch()` or `register_object` functions, giving them a MeshObject.
```cpp
struct MeshObject {
	Mesh* mesh{ nullptr };

	vkutil::Material* material;
	uint32_t customSortKey;
	glm::mat4 transformMatrix;

	RenderBounds bounds;

	uint32_t bDrawForwardPass : 1;
	uint32_t bDrawShadowPass : 1;
};
```

When an object is registered, it gets converted into a RenderObject and inserted into the `renderables` array. For the mesh and material, they will look up into a hashmap, and if the material isn't already in the array, it will get added there.

Registering an object will also add it to the relevant mesh passes.
```cpp
Handle<RenderObject> RenderScene::register_object(MeshObject* object)
{
    //convert it into a RenderObject
	RenderObject newObj;
	newObj.bounds = object->bounds;
	newObj.transformMatrix = object->transformMatrix;
	newObj.material = getMaterialHandle(object->material);
	newObj.meshID = getMeshHandle(object->mesh);
	newObj.updateIndex = (uint32_t)-1;
	newObj.customSortKey = object->customSortKey;
	newObj.passIndices.clear(-1);
	Handle<RenderObject> handle;
	handle.handle = static_cast<uint32_t>(renderables.size());

	renderables.push_back(newObj);

    //add to relevant mesh passes
	if (object->bDrawForwardPass)
	{
		if (object->material->original->passShaders[MeshpassType::Transparency])
		{
			_transparentForwardPass.unbatchedObjects.push_back(handle);
		}
		if (object->material->original->passShaders[MeshpassType::Forward])
		{
			_forwardPass.unbatchedObjects.push_back(handle);
		}
	}
	if (object->bDrawShadowPass)
	{
		if (object->material->original->passShaders[MeshpassType::DirectionalShadow])
		{
			_shadowPass.unbatchedObjects.push_back(handle);
		}
	}

    //flag as changed so that its data is uploaded to gpu
	update_object(handle);
	return handle;
}
```

When adding an object into a given meshpass, it looks up if the material actually has shaders for that type of pass. An object whose material is transparent will be registered into the transparent meshpass, and an object that doesn't have a DirectionalShadow shader effect won't cast shadows.

This is done so that each pass is fully standalone and doesn't do any work it doesn't have to. In the test scenes, there are generally very few transparent objects, so the transparent meshpass is always very lightweight.


## Meshpass update logic

When objects are added or removed from a meshpass, the meshpass needs to update its main flat_batches array, and its parents. This is done from the `refresh_pass` function, which has to be whenever there are changes to apply. It's not necesary to call this every frame, and it's safe to call from multiple threads.

The code in the repo for this part is still work in progress, and the optimizations done around partial updates can be tricky to follow. But the general logic works like this for the full-rebuild.

For each object in the `unbatchedObjects` array, it gets converted and inserted into the `objects` array. The correct descriptorset is grabbed and stored as part of the PassObject conversion.

Once the PassObject array is filled, we go over it, and calculate the draw hashes for each of the objects in there. This draw hashes are used so that we can sort the newly built (and unsorted) flat_batches array.

Once we have the flat_batches array sorted, we go over it, and compact the draws into the IndirectBatch array. Multiple draws in the flat_batches array will become a single IndirectBatch, which is an instanced draw.

After that array is made, it's compacted again into the multibatch array.


## GPU Side buffers
Every frame, the compute shader that takes care of the culling will build the final draw indirect commands. This is handled through a few GPU buffers that get used in said shader.

```cpp
struct GPUInstance {
	uint32_t objectID;
	uint32_t batchID;
};
struct GPUIndirectObject {
	VkDrawIndexedIndirectCommand command;
	uint32_t objectID;
	uint32_t batchID;
};

AllocatedBuffer<uint32_t> compactedInstanceBuffer;
AllocatedBuffer<GPUInstance> instanceBuffer;

AllocatedBuffer<GPUIndirectObject> drawIndirectBuffer;
AllocatedBuffer<GPUIndirectObject> clearIndirectBuffer;

```

`drawIndirectBuffer` and `clearIndirectBuffer` are both created from the meshpass batches array,and they hold the draw-indirect commands.
`clearIndirectBuffer` is a CPU-writeable buffer, and it has the correct data, but with `command.instanceCount` set to 0. From the culling shader we keep adding instances, so we use this one to "reset" `drawIndirectBuffer` every frame.

`drawIndirectBuffer` is a GPU-side buffer, and it's the one we actually use for rendering.

`passObjectsBuffer` is the main array used for the GPU compute pass. Each of the GPUInstance objects holds objectID and batchID. batchID will be used to access the proper draw-command if the cull passes, and object ID is just the global index of the object, to access its data.

This array is built directly from the `objects` array in the meshpass.

Last one is `compactedInstanceBuffer`, this is the one that maps gl_InstanceID into objectID. This written from the culling shader, and used from the actual vertex shaders to access the correct ObjectID when rendering each object.

Uploading buffers is all done from `ready_mesh_draw()` function in `vk_engine_scenerender.cpp`. For each messpass, if the meshpass changed, it uploads the flat arrays into a gpu buffers.

For the `clearIndirectBuffer`, it is filled from the "batches" array in the meshpass, like this.

```cpp
void RenderScene::fill_indirectArray(GPUIndirectObject* data, MeshPass& pass)
{
	int dataIndex = 0;
	for (int i = 0; i < pass.batches.size(); i++) {

		auto batch = pass.batches[i];

		data[dataIndex].command.firstInstance = batch.first;
		//set instance Count to 0 because it will be filled from the compute shader
		data[dataIndex].command.instanceCount = 0;
		data[dataIndex].command.firstIndex = get_mesh(batch.meshID)->firstIndex;
		data[dataIndex].command.vertexOffset = get_mesh(batch.meshID)->firstVertex;
		data[dataIndex].command.indexCount = get_mesh(batch.meshID)->indexCount;
		data[dataIndex].objectID = 0;
		data[dataIndex].batchID = i;

		dataIndex++;
	}
}
```

The instances array will be filled from the flat batches array, copying from the ranges that the batches array holds
```cpp
void RenderScene::fill_instancesArray(GPUInstance* data, MeshPass& pass)
{
	int dataIndex = 0;
	for (int i = 0; i < pass.batches.size(); i++) {

		auto batch = pass.batches[i];

		for (int b = 0; b < batch.count; b++)
		{
			data[dataIndex].objectID = pass.get(pass.flat_batches[b + batch.first].object)->original.handle;
			data[dataIndex].batchID = i;
			dataIndex++;
		}
	}
}
```


When uploading the buffers, it will check if there is enough space already in the buffer, and if the size has grown, it will destroy the old buffer, and allocate a new bigger one.



{: .fs-6 .fw-300 }
{% include comments.html term="GPU Driven Rendering" %}