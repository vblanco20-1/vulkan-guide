---
layout: default
title: Storage buffers
parent:  "4. Buffers, Shader input/output"
nav_order: 20
---

Uniform buffers are great for small, read only data. But what if you want data you don't know the size of in the shader? Or data that can be writeable. You use Storage buffers for that. 
Storage buffers are usually slightly slower than uniform buffers, but they can be much, much bigger. If you want to stuff your entire scene into one buffer, you have to use them. Make sure to profile it to know the performance.

With storage buffers, you can have an unsized array in a shader with whatever data you want. A common use for them is to store the data of all the objects in the scene.

We are going to use them to remove the usage of push-constants for the object matrices, which will let us upload the matrices at the beginning of the frame in bulk, and then we no longer need to do individual push constant calls every draw.
This also will mean that we will hold all the object matrices into one array, which can be used for interesting things in compute shaders.

## Creating the Shader Storage Buffer
We are continuing on the `init_descriptors()` function, as its where we initialize all the buffers for shader parameters.
In there, we are going to initialize one big storage buffer per frame, to hold the data for the objects. This is because we want the objects to still be dynamic. If we had fully static objects, we wouldn't need one buffer per frame, and one total would be enough.

```cpp
struct FrameData {
	AllocatedBuffer objectBuffer;
};

struct GPUObjectData{
	glm::mat4 modelMatrix;
}
```

```cpp
void VulkanEngine::init_descriptors()
{
	// other code ...
	for (int i = 0; i < FRAME_OVERLAP; i++)
	{
		const int MAX_OBJECTS = 10000;
		_frames[i].objectBuffer = create_buffer(sizeof(ObjectData) * MAX_OBJECTS, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		//other code ....
	}
}
```
Shader Storage buffers are created in the same way as uniform buffers. They also work in mostly the same way, they just have different properties like increased maximum size, and being writeable in shaders.
We are going to reserve an array of 10000 ObjectDatas per frame. This means that we can hold up to 10000 object matrices, rendering 10000 objects per frame. Its a small number, but at the moment its not a problem. Unreal Engine grows their object buffer as needed when the engine loads more objects, but we don't have any growable buffer abstraction so we reserve upfront.
While the size here is 1000, you can increase it to whatever you want. The maximum sizes for storage buffers are quite big, in most GPUs they can be as big as the VRAM can fit, so you can do this with 100 million matrices if you want.

We will now need to add it to the descriptor sets.
We have been adding everything into descriptor set number 0, but for this, we are going to use descriptor set number 1. This means we need another descriptor set layout for it, and also hook it to the pipeline creation.

## New descriptor set

```cpp
struct FrameData {
	AllocatedBuffer objectBuffer;
	VkDescriptorSet objectDescriptor;
};
```

```cpp
class VulkanEngine {
	VkDescriptorSetLayout _globalSetLayout;
	VkDescriptorSetLayout _objectSetLayout;
}
```
We are going to follow a similar approach as the camera buffer, where we will have one descriptor pointing to one buffer. Because this is a new descriptor set, we need to also store its layout for hooking to the pipelines.

Back on `init_descriptors()`, we are going to need to reserve space for it on the descriptor pool. 
```cpp
	std::vector<VkDescriptorPoolSize> sizes =
	{
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10 }
	};
```

A bit below, we are going to initialize the set layout, which will only have 1 binding for the big buffer.

```cpp
	VkDescriptorSetLayoutBinding objectBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);

	VkDescriptorSetLayoutCreateInfo set2info = {};
	set2info.bindingCount = 1;
	set2info.flags = 0;
	set2info.pNext = nullptr;
	set2info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	set2info.pBindings = &objectBind;

	vkCreateDescriptorSetLayout(_device, &set2info, nullptr, &_objectSetLayout);
```

Same as with the other set, we create a layout for the new set that will point to 1 storage buffer.

Now we need to create the descriptor sets to point to the buffer.
```cpp
for (int i = 0; i < FRAME_OVERLAP; i++)
{
		//allocation for the other descriptor and other code....

		//allocate the descriptor set that will point to object buffer
		VkDescriptorSetAllocateInfo objectSetAlloc = {};
		objectSetAlloc.pNext = nullptr;
		objectSetAlloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		objectSetAlloc.descriptorPool = _descriptorPool;
		objectSetAlloc.descriptorSetCount = 1;
		objectSetAlloc.pSetLayouts = &_objectSetLayout;

		vkAllocateDescriptorSets(_device, &objectSetAlloc, &_frames[i].objectDescriptor);


		VkDescriptorBufferInfo cameraInfo;
		cameraInfo.buffer = _frames[i].cameraBuffer._buffer;
		cameraInfo.offset = 0;
		cameraInfo.range = sizeof(GPUCameraData);

		VkDescriptorBufferInfo sceneInfo;
		sceneInfo.buffer = _sceneParameterBuffer._buffer;
		sceneInfo.offset = 0;
		sceneInfo.range = sizeof(GPUSceneData);

		VkDescriptorBufferInfo objectBufferInfo;
		objectBufferInfo.buffer = _frames[i].objectBuffer._buffer;
		objectBufferInfo.offset = 0;
		objectBufferInfo.range = sizeof(GPUObjectData) * MAX_OBJECTS;


		VkWriteDescriptorSet cameraWrite = vkinit::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _frames[i].globalDescriptor,&cameraInfo,0);
		
		VkWriteDescriptorSet sceneWrite = vkinit::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, _frames[i].globalDescriptor, &sceneInfo, 1);

		VkWriteDescriptorSet objectWrite = vkinit::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, _frames[i].objectDescriptor, &objectBufferInfo, 0);

		VkWriteDescriptorSet setWrites[] = { cameraWrite,sceneWrite,objectWrite };

		vkUpdateDescriptorSets(_device, 3, setWrites, 0, nullptr);
}
```

We need another DescriptorBufferInfo, and another WriteDescriptorSet
Note how in here, we are using 1 `vkUpdateDescriptorSets()` call to update 2 different descriptor sets. This is completely valid to do.
Now that the buffer is initialized and has descriptors that point to it, we need to add it to the shader.

We are going to modify the `tri_mesh.vert` shader, to read the object data from SSBO instead than from the push constant. We will still keep the push constant, but it wont be used.

```glsl
#version 460
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;

layout (location = 0) out vec3 outColor;

layout(set = 0, binding = 0) uniform  CameraBuffer{   
    mat4 view;
    mat4 proj;
	mat4 viewproj;
} cameraData;

struct ObjectData{
	mat4 model;
}; 

//all object matrices
layout(std140,set = 1, binding = 0) readonly buffer ObjectBuffer{   

	ObjectData objects[];
} objectBuffer;

//push constants block
layout( push_constant ) uniform constants
{
 vec4 data;
 mat4 render_matrix;
} PushConstants;

void main() 
{	
	mat4 modelMatrix = objectBuffer.objects[gl_BaseInstance].model;
	mat4 transformMatrix = (cameraData.viewproj * modelMatrix);
	gl_Position = transformMatrix * vec4(vPosition, 1.0f);
	outColor = vColor;
}
```
We are changing the GLSL version to 460 because we want to be able to use `gl_BaseInstance` for indexing into the transforms array.


Note the way we are declaring the ObjectBuffer
```glsl
layout(std140,set = 1, binding = 0) readonly buffer ObjectBuffer{   

	ObjectData objects[];
} objectBuffer;
```
We need the std140 layout description to make the array match how arrays work in cpp. That std140 enforces some rules about how the memory is laid out, and what is its alignment.
The set is now 1, and binding is 0, referencing that its a new descriptor set slot.

We are also using `readonly buffer` instead of `uniform` when declaring it. Shader Storage buffers can be read or written to, so we need to let Vulkan know. THey are also defined with `buffer` instead of `uniform`.

The array inside is also not sized. You can only have unsized arrays in storage buffers. This will let the shader scale to whatever buffer size we have.

Another thing is the way we are accessing the correct object matrix. We are no longer using push constants, but we are doing this
```glsl
mat4 modelMatrix = objectBuffer.objects[gl_BaseInstance].model;
```
We are using `gl_BaseInstance` to access the object buffer. This is due to how Vulkan works on its normal draw calls. All the draw commands in Vulkan request "first Instance" and "instance count". We are not doing instanced rendering, so instance count is always 1. But we can still change the "first instance" parameter, and this way get `gl_BaseInstance` as a integer we can use for whatever use we want to in the shader. This gives us a simple way to send a single integer to the shader without setting up pushconstants or descriptors.

We now need to hook the descriptor layout to the pipeline.

On `init_pipelines()`, we add it to the list of descriptors when creating the pipeline layout
```cpp
VkDescriptorSetLayout setLayouts[] = { _globalSetLayout, _objectSetLayout };

mesh_pipeline_layout_info.setLayoutCount = 2;
mesh_pipeline_layout_info.pSetLayouts = setLayouts;
```

We now have the pipeline set up, so the last thing is to write into the buffer.

## Writing the shader storage buffer


On `draw_objects()`, we will write into the buffer by copying the render matrices from our render objects into it. This goes before the render loop, alongside the other memory write operations.

```cpp
void* objectData;
vmaMapMemory(_allocator, get_current_frame().objectBuffer._allocation, &objectData);

GPUObjectData* objectSSBO = (GPUObjectData*)objectData;

for (int i = 0; i < count; i++)
{
	RenderObject& object = first[i];
	objectSSBO[i].modelMatrix = object.transformMatrix;
}

vmaUnmapMemory(_allocator, get_current_frame().objectBuffer._allocation);
```

Instead of using `memcpy` here, we are doing a different trick. It is possible to cast the `void*` from mapping the buffer into another type, and write into it normally. This will work completely fine, and makes it easier to write complex types into a buffer. 

The buffer is now filled, so we now need to bind the descriptor set and use the firstIndex parameter in the draw command to access the object data in the shader.


```cpp
if (object.material != lastMaterial) {

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipeline);
	lastMaterial = object.material;

			//camera data descriptor
	uint32_t uniform_offset = pad_uniform_buffer_size(sizeof(GPUSceneData)) * frameIndex;
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelineLayout, 0, 1, &get_current_frame().globalDescriptor, 1, &uniform_offset);

	//object data descriptor
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelineLayout, 1, 1, &get_current_frame().objectDescriptor, 0, nullptr);
}

//more code ....

//we can now draw	
vkCmdDraw(cmd, object.mesh->_vertices.size(), 1,0 , i);
```

We are using the index in the loop on the `vkCmdDraw()` call to send the instance index to the shader.

Now we have multiple buffers of different kinds, and on different descriptor sets, implemented.

The last step for the tutorial is textures, which will go into the next chapter. But before going there, I heavily recommend you try to do some things with the codebase.

Right now, we have one descriptor set per frame for the Set 0 (camera and scene buffers). Try to refactor it so it only uses 1 descriptor set and 1 buffer both both camera and scene buffers, packing both the structs for all frames into the same uniform buffer, and then using dynamic offsets.

Alternatively, try to create another SSBO that holds something like ObjectColor, to use on a per-object basis, and try to use it to color the objects in different ways by modifying the shaders.


Next: [Memory transfers]({{ site.baseurl }}{% link docs/chapter-5/memory_transfers.md %})


{% include comments.html term="Chapter 4 Comments" %}