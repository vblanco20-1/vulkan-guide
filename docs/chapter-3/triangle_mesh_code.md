---
layout: default
title: Implementing vertex buffers
parent:  "3. Drawing meshes"
nav_order: 11
---


## Initializing the allocator
We are going to begin by making sure that the Vulkan Memory Allocator is initialized so that we can allocate buffers from it easily.

Add the `_allocator` member to the VulkanEngine class, to use it.

```cpp
class VulkanEngine {
public:

// other code .....
VmaAllocator _allocator; //vma lib allocator
// other code....

}

```

At the end of the `init_vulkan` function we are going to initialize the allocator.

```cpp
void VulkanEngine::init_vulkan()
{
    // other code....

    //initialize the memory allocator
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = _chosenGPU;
    allocatorInfo.device = _device;
    allocatorInfo.instance = _instance;
    vmaCreateAllocator(&allocatorInfo, &_allocator);
}
```

With that, the allocator is set up and we can now use it to allocate buffers.
If you try to compile the project now, you will find that VMA is missing function definitions and giving linker errors. To solve that, add this to one .cpp file in the project (its recommended you add it to vk_engine.cpp, but can be other).
```cpp
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
```
This will include the implementation of the VMA library itself.

As the last thing for the allocator, we are going to add a struct to represent an allocated buffer to vk_types.h

`vk_types.h`
```cpp
#include <vk_mem_alloc.h>

struct AllocatedBuffer {
    VkBuffer _buffer;
    VmaAllocation _allocation;
};
```

`AllocatedBuffer` is going to hold the buffer we allocate, alongside its allocation data. VkBuffer is a handle to a GPU side Vulkan buffer, and VmaAllocation holds the state that the VMA library uses, like the memory that buffer was allocated from, and its size. We use the VmaAllocation object to manage the buffer allocation itself.

## The Mesh class
As we are going to have a lot of mesh-related code, we are going to create some new files, `vk_mesh.h` and `vk_mesh.cpp`, where we are going to put the mesh-related logic and structures. We are going to place those files alongside the rest of engine files. You can look at the Github code as example. 
Make sure to add it to the CMake and re-run it so that the project updates.

`vk_mesh.h`
```cpp
#pragma once

#include <vk_types.h>
#include <vector>
#include <glm/vec3.hpp>

struct Vertex {

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

struct Mesh {
	std::vector<Vertex> _vertices;

	AllocatedBuffer _vertexBuffer;
};
```
`vk_mesh.cpp`
```cpp
#include <vk_mesh.h>
//just that for now
```

We are creating 2 structs, `Vertex` and `Mesh`.
The `Vertex` will hold position, normal (we will use this later), and color. Each of them is a `vec3`.
This vertex format is not optimal as the data can be packed much better, but we will use this for simplicity. Optimized vertex formats will be a topic for later.

Our Mesh class will hold a `std::vector` of `Vertex` for our vertex data, and an `AllocatedBuffer` which is where we will store the GPU copy of that data.

## Initializing the triangle mesh

Now that we have a `Mesh` class, we are going to make it hold the triangle, and upload it to the GPU.

Back on `VulkanEngine` class, we are going to add a triangle mesh member, and a function to init it. Alongside the mesh, we will also store a `VkPipeline` that doesn't hardcode the triangle. We will also add a general `upload_mesh` function.

`vk_engine.h`
```cpp
//add the include for the vk_mesh header
#include <vk_mesh.h>

class VulkanEngine {
public:

	//other code....

	VkPipeline _meshPipeline;
	Mesh _triangleMesh;

private:

	//other code ....
	void load_meshes();

	void upload_mesh(Mesh& mesh);
}
```

Make sure to add `load_meshes` call on the `init()` function, at the end
```cpp
void VulkanEngine::init()
{
	//other code ....
	load_meshes();

	//everything went fine
	_isInitialized = true;
}

```


Lets begin filling the `load_meshes` function. The first thing we are going to do is to fill the `_vertices` vector with the vertex data for the triangle, and then just call `upload_mesh` with the triangle

```cpp
void VulkanEngine::load_meshes()
{
	//make the array 3 vertices long
	_triangleMesh._vertices.resize(3);

	//vertex positions
	_triangleMesh._vertices[0].position = { 1.f, 1.f, 0.0f };
	_triangleMesh._vertices[1].position = {-1.f, 1.f, 0.0f };
	_triangleMesh._vertices[2].position = { 0.f,-1.f, 0.0f };

	//vertex colors, all green
	_triangleMesh._vertices[0].color = { 0.f, 1.f, 0.0f }; //pure green
	_triangleMesh._vertices[1].color = { 0.f, 1.f, 0.0f }; //pure green
	_triangleMesh._vertices[2].color = { 0.f, 1.f, 0.0f }; //pure green
	
	//we don't care about the vertex normals

	upload_mesh(_triangleMesh);
}
```

Now its time to create the vertex buffer. We will fill the `upload_mesh` function

```cpp
void VulkanEngine::upload_mesh(Mesh& mesh)
{
	//allocate vertex buffer
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	//this is the total size, in bytes, of the buffer we are allocating
	bufferInfo.size = mesh._vertices.size() * sizeof(Vertex);
	//this buffer is going to be used as a Vertex Buffer
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;


	//let the VMA library know that this data should be writeable by CPU, but also readable by GPU
	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

	//allocate the buffer
	VK_CHECK(vmaCreateBuffer(_allocator, &bufferInfo, &vmaallocInfo, 
		&mesh._vertexBuffer._buffer, 
		&mesh._vertexBuffer._allocation, 
		nullptr));

	//add the destruction of triangle mesh buffer to the deletion queue
	_mainDeletionQueue.push_function([=]() {

        vmaDestroyBuffer(_allocator, mesh._vertexBuffer._buffer, mesh._vertexBuffer._allocation);
    });
}
```

This will allocate the buffer using VMA library, and add its deallocation to the destruction queue.
Note how we need to send the `VmaAllocation` object to `vmaDestroyBuffer`, this is the reason we keep them together.

When you create a buffer, you can decide in which memory it will be created. With VMA library, this is abstracted by the `VMA_MEMORY_USAGE_` enums. This will let VMA decide where to allocate the memory.
The `VMA_MEMORY_USAGE_CPU_TO_GPU` usage is very useful for dynamic data. It can be written from the CPU, and VMA will attempt to place the allocation in directly GPU-accessible memory.

There are many possible ways to use a given buffer (as texture storage, as uniforms, as writeable data, etc), so Vulkan needs to know what exactly are you going to use that buffer for. We will use it strictly as a vertex buffer, so we will just put the `VK_BUFFER_USAGE_VERTEX_BUFFER_BIT` flag in the `VkBufferCreateInfo::usage` parameter. If you use a buffer in a way you didn't specify, the validation layers will complain.

Now that we got a memory spot for our vertex data, we copy from our `_vertices` vector into this GPU-readable data.

```cpp
void VulkanEngine::upload_mesh(Mesh& mesh)
{
	// other code ....

	//copy vertex data
	void* data;
	vmaMapMemory(_allocator, mesh._vertexBuffer._allocation, &data);

	memcpy(data, mesh._vertices.data(), mesh._vertices.size() * sizeof(Vertex));

	vmaUnmapMemory(_allocator, mesh._vertexBuffer._allocation);
}
```

To push data into a `VkBuffer`, we need to map it first. Mapping a buffer will give us a pointer (`data` here), and then we can write into it.
When we are done with the writing, we unmap the data.
It is possible to keep the pointer mapped and not unmap it immediately, but that is an advanced technique mostly used for streaming data, which we don't need right now. Mapping and then unmapping the pointer lets the driver know that the write is finished, and will be safer.

To copy the data, we use `memcpy` directly. Note that its not necesary to use memcpy, but in many implementations `memcpy` will be the fastest way to copy a chunk of memory.

Our `upload_mesh` function is finished (for now), and we are uploading the triangle.
There should be no validation errors when you try to run the application.

## Vertex input layout
The triangle is now in GPU-accessible memory, so we now have to change the pipeline to render it.
We begin by creating a struct to hold the input layout data, and a static function in `Vertex` that creates a description to match the format in `Vertex`.

`vk_mesh.h`
```cpp
struct VertexInputDescription {

	std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;

	VkPipelineVertexInputStateCreateFlags flags = 0;
};


struct Vertex {

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;

	static VertexInputDescription get_vertex_description();
};
```

Lets fill the `get_vertex_description` function with the correct settings.
`vk_mesh.cpp`
```cpp

VertexInputDescription Vertex::get_vertex_description()
{
	VertexInputDescription description;

	//we will have just 1 vertex buffer binding, with a per-vertex rate
	VkVertexInputBindingDescription mainBinding = {};
	mainBinding.binding = 0;
	mainBinding.stride = sizeof(Vertex);
	mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	description.bindings.push_back(mainBinding);

	//Position will be stored at Location 0
	VkVertexInputAttributeDescription positionAttribute = {};
	positionAttribute.binding = 0;
	positionAttribute.location = 0;
	positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionAttribute.offset = offsetof(Vertex, position);

	 //Normal will be stored at Location 1
	VkVertexInputAttributeDescription normalAttribute = {};
	normalAttribute.binding = 0;
	normalAttribute.location = 1;
	normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
	normalAttribute.offset = offsetof(Vertex, normal);

	//Color will be stored at Location 2
	VkVertexInputAttributeDescription colorAttribute = {};
	colorAttribute.binding = 0;
	colorAttribute.location = 2;
	colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
	colorAttribute.offset = offsetof(Vertex, color);

	description.attributes.push_back(positionAttribute);
	description.attributes.push_back(normalAttribute);
	description.attributes.push_back(colorAttribute);
	return description;
}

```

`VertexInputBindingDescription` defines the vertex buffers that act as input. In this case, we are using just 1 vertex buffer, so we will only need one binding.
The stride is `sizeof(Vertex)`, as we have our vertex data tightly packed, each vertex taking up `sizeof(Vertex)` size.

We then create 3 `VertexInputAttributeDescription`s, one per each vertex attribute we have. On each of them, we set the format as `VK_FORMAT_R32G32B32_SFLOAT`, which maps directly to what a glm::vec3 is (three `float` components of 32-bit each), and use the offset of the member in the `Vertex` struct.

With this, we now map directly our `Vertex` struct into what Vulkan expects on the pipeline vertex input.

## New vertex shader
Lets now create a new shader, `mesh.vert`, that will use these vertex inputs. This vertex shader will be used with the `colored_triangle.frag` fragment shader. Make sure to refresh CMake so that it finds the new shader and compiles it.

`tri_mesh.vert`
```glsl
#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;

layout (location = 0) out vec3 outColor;

void main() 
{	
	gl_Position = vec4(vPosition, 1.0f);
	outColor = vColor;
}
```

We no longer need to use `vertexID` to do anything, we can just send `vPosition` directly into `gl_Position`. Grabbing the data by vertex ID will be done automatically by the driver when it compiles the pipeline.
All 3 attributes work in the same way, according to the `VkVertexInputAttributeDescription`s we just wrote.

## Putting all together
We now have the buffer uploaded, the shader written, and the input description filled. Its now time to compile the `_meshPipeline`, and use it to render our new triangle, which should be green.

Go to the function `init_pipelines` of our VulkanEngine, as we will create the pipeline at the end of it.


```cpp
void VulkanEngine::init_pipelines()
{
	//other code


	//build the mesh pipeline

	VertexInputDescription vertexDescription = Vertex::get_vertex_description();

	//connect the pipeline builder vertex input info to the one we get from Vertex
	pipelineBuilder._vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes.data();
	pipelineBuilder._vertexInputInfo.vertexAttributeDescriptionCount = vertexDescription.attributes.size();

	pipelineBuilder._vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings.data();
	pipelineBuilder._vertexInputInfo.vertexBindingDescriptionCount = vertexDescription.bindings.size();

	//clear the shader stages for the builder
	pipelineBuilder._shaderStages.clear();

	//compile mesh vertex shader
    

	VkShaderModule meshVertShader;
	if (!load_shader_module("../../shaders/tri_mesh.vert.spv", &meshVertShader))
	{
		std::cout << "Error when building the triangle vertex shader module" << std::endl;
	}
	else {
		std::cout << "Red Triangle vertex shader succesfully loaded" << std::endl;
	}

	//add the other shaders
	pipelineBuilder._shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, meshVertShader));

	//make sure that triangleFragShader is holding the compiled colored_triangle.frag
	pipelineBuilder._shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader));

	//build the mesh triangle pipeline
	_meshPipeline = pipelineBuilder.build_pipeline(_device, _renderPass);

	//deleting all of the vulkan shaders
	vkDestroyShaderModule(_device, meshVertShader, nullptr);
	vkDestroyShaderModule(_device, redTriangleVertShader, nullptr);
	vkDestroyShaderModule(_device, redTriangleFragShader, nullptr);
	vkDestroyShaderModule(_device, triangleFragShader, nullptr);
	vkDestroyShaderModule(_device, triangleVertexShader, nullptr);

	//adding the pipelines to the deletion queue
	_mainDeletionQueue.push_function([=]() {
		vkDestroyPipeline(_device, _redTrianglePipeline, nullptr);
		vkDestroyPipeline(_device, _trianglePipeline, nullptr);
		vkDestroyPipeline(_device, _meshPipeline, nullptr);

		vkDestroyPipelineLayout(_device, _trianglePipelineLayout, nullptr);
	});
}
```

There is not much here, other than connecting the vertex input info to the pipeline builder. With that and adding the `tri_mesh.vert` vertex shader, thats all we need. We also make sure that each shader module is correctly deleted at the end of the function.

Now we are holding a `_meshPipeline` that knows how to render a colored mesh. Lets replace the inner loop of `draw()` function to use the new pipeline and draw the mesh. 

```cpp
VulkanEngine::draw()
{
	//other code .... 
	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _meshPipeline);

	//bind the mesh vertex buffer with offset 0
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(cmd, 0, 1, &_triangleMesh._vertexBuffer._buffer, &offset);

	//we can now draw the mesh
	vkCmdDraw(cmd, _triangleMesh._vertices.size(), 1, 0, 0);

	//finalize the render pass
	vkCmdEndRenderPass(cmd);

	// other code...
}
```


![triangle]({{site.baseurl}}/diagrams/greenTriangle.png)


That's it, if you run this, you should see a green triangle.
The magic of it is that this triangle is not hardcoded. It doesn't even have to be a triangle. Using this code you can render any mesh you want.

The draw code is almost the same as before, except we now do `vkCmdBindVertexBuffers`.
With that call, we tell Vulkan where to fetch the vertex data from, connecting the shaders to the buffer where we stored the triangle data.

Next: [Push Constants]({{ site.baseurl }}{% link docs/chapter-3/push_constants.md %})



{% include comments.html term="Chapter 3 Comments" %}
 

