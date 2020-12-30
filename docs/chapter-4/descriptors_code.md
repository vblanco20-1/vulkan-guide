---
layout: default
title: Setting up descriptor sets
parent:  "4. Buffers, Shader input/output"
nav_order: 12
---

Now that descriptor sets are explained, let's look at using them in practise in a minimal example. We are going to modify the codebase and shaders, so that instead of sending the final transform matrix of the object through push constant, multiplying it in the CPU, we read the camera matrix on the shader and multiply it by the object matrix, with the multiplication being done in the shader.
To get that to work, we are going to need to create a uniform buffer for our camera matrices, and expose that to the shader using a single descriptor set.

## Setting up camera buffers
We are going to create one camera buffer for each of our frames. This is so that we can overlap the data correctly, and modify the camera matrix while the GPU is rendering the last frame.

Given that we are starting to create a lot of buffers, we are going to abstract buffer creation into a function first.

Add the function declaration to the VulkanEngine class too
vk_engine.cpp
```cpp
AllocatedBuffer VulkanEngine::create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
{
	//allocate vertex buffer
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = nullptr;

	bufferInfo.size = allocSize;
	bufferInfo.usage = usage;

	
	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = memoryUsage;

	AllocatedBuffer newBuffer;

	//allocate the buffer
	VK_CHECK(vmaCreateBuffer(_allocator, &bufferInfo, &vmaallocInfo,
		&newBuffer._buffer,
		&newBuffer._allocation,
		nullptr));

	return newBuffer;
}
```

On the function we will just ask for buffer size, buffer usage, and memory usage. This is all we need for basic buffer creation. This is the similar code as we use for vertex buffers.

Now, we are going to add a variable to hold the camera buffer to our FrameData struct, and create a struct for the camera data.
```cpp

struct GPUCameraData{
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 viewproj;
};

struct FrameData {
	// other code ...
	//buffer that holds a single GPUCameraData to use when rendering
	AllocatedBuffer cameraBuffer;
	
	VkDescriptorSet globalDescriptor;
};
```

The GPUCameraData struct just holds a few matrices we are going to need. View matrix (camera location/transform), Projection matrix (for perspective), and ViewProj, which is just both of them multiplied together, to avoid multiplying them in the shader.

On FrameData, we are adding the AllocatedBuffer for it, but we also add a `VkDescriptorSet` that we will cache to hold the global descriptor. We will be adding a few more things to it than just the camera uniform buffer.

We are going to add another initialization function, `init_descriptors()` to VulkanEngine class. Also add it to the main init function, but before `init_pipelines()` call. Some of the descriptor things we initialize there will be needed when creating the pipelines.

```cpp
class VulkanEngine {
	//other code....
	AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
	void init_descriptors();
}
```

```cpp
void VulkanEngine::init()
{
	// other code .... 

	init_sync_structures();

	init_descriptors();

	init_pipelines();	

	//other code ....
}
```

Now that the function and data is added, we need to create those camera buffers.


```cpp
void VulkanEngine::init_descriptors()
{
	for (int i = 0; i < FRAME_OVERLAP; i++)
	{
		_frames[i].cameraBuffer = create_buffer(sizeof(GPUCameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	}
}
```
For creating the buffers, we are to use the Uniform Buffer usage, and `CPU_TO_GPU` memory type.
Uniform buffers are the best for this sort of small, read only shader data. They have a size limitation, but they are very fast to access in the shaders.


Try to run this and see if the validation layers complain. They shouldn't.

## Descriptor sets: shader
We are going to start doing the shader data itself. First thing is modifying the shader that we use, to use the matrix there.

```glsl
#version 450
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;

layout (location = 0) out vec3 outColor;

layout(set = 0, binding = 0) uniform  CameraBuffer{   
	mat4 view;
	mat4 proj;
	mat4 viewproj;
} cameraData;

//push constants block
layout( push_constant ) uniform constants
{
 vec4 data;
 mat4 render_matrix;
} PushConstants;

void main() 
{	
	mat4 transformMatrix = (cameraData.viewproj * PushConstants.render_matrix);
	gl_Position = transformMatrix * vec4(vPosition, 1.0f);
	outColor = vColor;
}

```

The new block is the CameraBuffer uniform declaration. In there, you can see that it follows the same syntax as the push constant block, but with a different `layout()`.
By having `set = 0` and `binding = 0`, we are declaring that the CameraBuffer uniform will be grabbed from the descriptor set bound at slot 0, and it's binding 0 within that descriptor set.

In the core of the vertex shader, we multiply the render matrix from the push-constant with the viewproj matrix on the CameraBuffer. This will get the final transformation matrix, and then we can multiply vertex position by it.

Lets now set it up on the cpp side. The first thing we will need is to create the descriptor set layout.

## Descriptor Set Layout

Add a new member variable to vulkan engine. We are going to use it to store the descriptor layout for our global data.
Also add a member for the descriptor pool that we will need later
```cpp
class VulkanEngine {
VkDescriptorSetLayout _globalSetLayout;
VkDescriptorPool _descriptorPool;
}

```

A `VkDescriptorSetLayout` holds information about the shape of a descriptor set. In this case, our descriptor set is going to be a set that holds a single uniform buffer reference at binding 0. 


```cpp
void VulkanEngine::init_descriptors()
{

	//information about the binding.
	VkDescriptorSetLayoutBinding camBufferBinding = {};
	camBufferBinding.binding=  0;
	camBufferBinding.descriptorCount = 1;
	// it's a uniform buffer binding
	camBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; 

	// we use it from the vertex shader
	camBufferBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; 
	
	
	VkDescriptorSetLayoutCreateInfo setInfo= {};
	setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	setInfo.pNext = nullptr;
	
	//we are going to have 1 binding
	setInfo.bindingCount = 1;
	//no flags
	setInfo.flags = 0;
	//point to the camera buffer binding
	setInfo.pBindings = &camBufferBinding;

	vkCreateDescriptorSetLayout(_device, &setInfo, nullptr, &_globalSetLayout);

	// other code ....
}
```

To create a descriptor set layout, we need another CreateInfo struct. The create-info will point into an array of `VkDescriptorSetLayoutBinding` structs. Each of those structs will contain information about the descriptor itself. In this case, we have only a single binding, which is binding 0, and it's a Uniform Buffer.

We now have the descriptor set layout for our descriptor created, so we need to hook it to the pipeline creation. When you create a pipeline, you also need to let the pipeline know what descriptors will be bound to it.

Back into `init_pipelines()`. We need to modify the creation of the `VkPipelineLayout` by hooking the descriptor layout to it. 

```cpp

//push-constant setup
mesh_pipeline_layout_info.pPushConstantRanges = &push_constant;
mesh_pipeline_layout_info.pushConstantRangeCount = 1;

//hook the global set layout 
mesh_pipeline_layout_info.setLayoutCount = 1;
mesh_pipeline_layout_info.pSetLayouts = &_globalSetLayout;

VkPipelineLayout meshPipLayout;
VK_CHECK(vkCreatePipelineLayout(_device, &mesh_pipeline_layout_info, nullptr, &meshPipLayout));
```

Now our pipeline builder will connect the pipeline layout to everything, which will allow the pipelines to access the descriptor sets once we bind them.

The pipeline setup is done, so now we have to allocate a descriptor set, and bind it when rendering.

## Allocating descriptorsets

Back to `init_descriptors()`, we first need to create a `VkDescriptorPool` to allocate the descriptors from.


```cpp
void VulkanEngine::init_descriptors()
{

	//create a descriptor pool that will hold 10 uniform buffers
	std::vector<VkDescriptorPoolSize> sizes =
	{
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 }
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = 0;
	pool_info.maxSets = 10;
	pool_info.poolSizeCount = (uint32_t)sizes.size();
	pool_info.pPoolSizes = sizes.data();
	
	vkCreateDescriptorPool(_device, &pool_info, nullptr, &_descriptorPool);

	// other code ....
}
```

In this case, we know exactly what we will need to allocate from the pool, which is descriptor sets that point to uniform buffers. When creating a descriptor pool, you need to speficy how many descriptors of each type you will need, and what's the maximum number of sets to allocate from it. For now, we are going to reserve 10 uniform buffer pointers/handles, and a maximum of 10 descriptor sets allocated from the pool.

We can now allocate the descriptors from it. For it, continue on the `init_descriptors()` function, inside the FRAME_OVERLAP loop.

```cpp
for (int i = 0; i < FRAME_OVERLAP; i++)
	{
		_frames[i].cameraBuffer = create_buffer(sizeof(GPUCameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		//allocate one descriptor set for each frame
		VkDescriptorSetAllocateInfo allocInfo ={};
		allocInfo.pNext = nullptr;
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		//using the pool we just set
		allocInfo.descriptorPool = _descriptorPool;
		//only 1 descriptor
		allocInfo.descriptorSetCount = 1;
		//using the global data layout
		allocInfo.pSetLayouts = &_globalSetLayout;

		vkAllocateDescriptorSets(_device, &allocInfo, &_frames[i].globalDescriptor);
	}
```

With this, We now have a descriptor stored in our frame struct. But this descriptor is not pointing to any buffer yet, so we need to make it point into our camera buffer.


```cpp

for (int i = 0; i < FRAME_OVERLAP; i++)
	{
		// allocation code ... 

		//information about the buffer we want to point at in the descriptor
		VkDescriptorBufferInfo binfo;
		//it will be the camera buffer
		binfo.buffer = _frames[i].cameraBuffer._buffer;
		//at 0 offset
		binfo.offset = 0;
		//of the size of a camera data struct
		binfo.range = sizeof(GPUCameraData);

		VkWriteDescriptorSet setWrite = {};
		setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		setWrite.pNext = nullptr;

		//we are going to write into binding number 0
		setWrite.dstBinding = 0;
		//of the global descriptor
		setWrite.dstSet = _frames[i].globalDescriptor;

		setWrite.descriptorCount = 1;
		//and the type is uniform buffer
		setWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		setWrite.pBufferInfo = &binfo;
	   

		vkUpdateDescriptorSets(_device, 1, &setWrite, 0, nullptr);    
	}
```

We need to fill a `VkDescriptorBufferInfo` with the data of the buffer we want to have in the descriptor set. Because we have defined that our camera buffer is on binding 0, then we need to set it here, and with enough size to hold the struct.

Now we have a filled descriptor set, so we can use it when rendering.
In the `draw_objects()` function, we will start by writing to the camera buffer with the current camera matrix. This is the code that we had before for the push constants, but we now fill a GPUCameraData struct, and then copy it into the buffer. If you have implemented a moving camera, you will need to modify this code.

```cpp

void VulkanEngine::draw_objects(VkCommandBuffer cmd,RenderObject* first, int count)
{
	
	//camera view
	glm::vec3 camPos = { 0.f,-6.f,-10.f };

	glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
	//camera projection
	glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
	projection[1][1] *= -1;

	//fill a GPU camera data struct
	GPUCameraData camData;
	camData.proj = projection;
	camData.view = view;
	camData.viewproj = projection * view;

	//and copy it to the buffer
	void* data;
	vmaMapMemory(_allocator, get_current_frame().cameraBuffer._allocation, &data);

	memcpy(data, &camData, sizeof(GPUCameraData));

	vmaUnmapMemory(_allocator, get_current_frame().cameraBuffer._allocation);
}

```

We fill the struct, and then copy it to the buffer with the same pattern that we used when dealing with vertex buffers. First you map the buffer into a void pointer, then you memcpy the data into it, and then unmap the buffer.

The buffer now holds the proper camera data, so now we can bind it.

```cpp
//only bind the pipeline if it doesnt match with the already bound one
if (object.material != lastMaterial) {

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipeline);
	lastMaterial = object.material;
	//bind the descriptor set when changing pipeline
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelineLayout, 0, 1, &get_current_frame().globalDescriptor, 0, nullptr);
}
```

We will bind the set whenever we switch pipeline. Its not strictly necesary right now, as all our pipelines are the same, but it will be easier.

Last thing is to modify the push constants code, to make it not multiply the matrix there, and just pushconstant the model matrix.

```cpp
MeshPushConstants constants;
constants.render_matrix = object.transformMatrix;

//upload the mesh to the GPU via pushconstants
vkCmdPushConstants(cmd, object.material->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);
```

If you run the code right now, everything should be working fine. But we have now a way to let the shader read some data from a buffer, instead of having to pushconstant all of the data of the shader.




Next: [Dynamic descriptor sets]({{ site.baseurl }}{% link docs/chapter-4/descriptors_code_more.md %})


{% include comments.html term="Chapter 4 Comments" %}