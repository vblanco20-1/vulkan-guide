---
layout: default
title: Implementing vertex buffers
parent: Chapter 3
nav_order: 15
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

On the `init_vulkan` function, we are going to initialize the allocator at the end.

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

With that, the allocator is set and we can now use it to allocate buffers.

As the last thing for the allocator, we are going to add a struct to hold an allocated buffer to vk_types.h

`vk_types.h`
```cpp
#include <vk_mem_alloc.h>

struct AllocatedBuffer {
    VkBuffer _buffer;
    VmaAllocation _allocation;
};
```

`AllocatedBuffer` is going to hold the buffer we allocate, alongside its allocation data. This is quite important, as for many operations, we will need to use the VmaAllocation object. 
VkBuffer is a handle to a GPU side Vulkan buffer, and VmaAllocation holds the state that the VMA library uses, like the place that buffer was allocated from, and its size.

## the Mesh class

As we are going to have a lot of mesh related code, we are going to create some new files, `vk_mesh.h` and `vk_mesh.cpp`, where we are going to put the mesh related logic and structures. We are going to place those files alongside the rest of engine files. You can look at the github code as example.

`vk_mesh.h`
```cpp
#pragma once

#include <vk_types.h>
#include <vector>

struct Vertex {

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
}

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

We are creating 2 structs, Vertex and Mesh.
The Vertex will hold Position, Normal (we will use this later), and Color. Each of them is a vec3.
This vertex format is very suboptimal, the data can be packed much better, but we will use this for simplicity. Optimized vertex formats will be a topic for later.

Our Mesh class will hold a std::vector of Vertex for our vertex data, and an AllocatedBuffer which is where we will store the GPU copy of that data.

## Initializing the triangle mesh

Now that we have a Mesh class, we are going to make it hold the triangle, and upload it to the GPU.

Back on VulkanEngine class, we are going to add a triangle mesh member, and a function to init it. Alongside the mesh, we will also store a Pipeline that doesn't hardcode the triangle. We will also add a general upload_mesh function.

`vk_engine.h`
```cpp
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

Make sure to add load_meshes call on the `init()` function, at the end
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
    _triangleMesh._vertices[0].position = { 1.f,1.f, 0.0f };
	_triangleMesh._vertices[1].position = { -1.f,1.f, 0.0f };
	_triangleMesh._vertices[2].position = { 0.f,-1.f, 0.0f };

    //vertex colors, all green
	_triangleMesh._vertices[0].color = { 0.f,1.f, 0.0f }; //pure green
	_triangleMesh._vertices[1].color = { 0.f,1.f, 0.0f }; //pure green
	_triangleMesh._vertices[2].color = { 0.f,1.f, 0.0f }; //pure green
    //we dont care about the vertex normals

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
    bufferInfo.pNext = nullptr;
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
Note how we need to send the VmaAlocation object to `vmaDestroyBuffer`, this is why we keep them together.

When you create a buffer, you can decide in which memory it will be created. With VMA library, you have this abstracted using the `VMA_MEMORY_USAGE_` enums. This will let VMA decide where to allocate the memory.
The CPU_TO_GPU usage is a great one for dynamic data. It can be written from the CPU (but can be slow-ish), and its allocated on directly GPU-accesible memory. To keep the renderer simple, this is a good one to use here.

An alternative would be to allocate the vertex buffer in `VMA_MEMORY_USAGE_GPU_ONLY` memory. This is the fastest approach, but you cant write to it from CPU, so you would need to create another buffer with `VMA_MEMORY_USAGE_CPU_ONLY` usage, and then use a vkCmdCopyBuffer command to copy the data from CPU RAM into GPU VRAM. We will explain how to do that on the Textures chapter.

`VkBufferCreateInfo::usage` parameter is absolutely critical. There are many possible ways to use a given buffer (as texture storage, as uniforms, as writeable data, etc), so Vulkan needs to know what exactly are you going to use that buffer for. We will use it strictly as a vertex buffer, so we will just put the `VK_BUFFER_USAGE_VERTEX_BUFFER_BIT` flag. If you use a buffer in a way you didn't mark, the validation layers will complain.

Now that we got a memory spot for our vertex data, we copy from our _vertices vector into this gpu-readable data.

```cpp
void VulkanEngine::upload_mesh(Mesh& mesh)
{
    // other code ....

    //copy vertex data
    void* data;
    vmaMapMemory(_allocator, _triangleMesh._vertexBuffer._allocation, &data);

    memcpy(data, _triangleMesh._vertices.data(), _triangleMesh._vertices.size() * sizeof(Vertex));

    vmaUnmapMemory(_allocator, _triangleMesh._vertexBuffer._allocation);
}
```

To push data into a VkBuffer, we need to map it first. Mapping a buffer will give us a pointer (`data` here), and then we can write into it.
When we are done with the writing, we un-map the data.
It is possible to keep the pointer mapped and not unmap it immediately, but thats an advanced technique mostly used for streaming data, which we dont need right now. Mapping and then unmapping the pointer lets the driver know that the write is finished, and will be safer.

To copy the data, we use direct `memcpy`. Note that its not necesary to use memcpy, but in many implementations memcpy will be the fastest way to copy such a chunk of data.

Our `upload_mesh`function is now finished (for now), and we are uploading the triangle.
Try to run the application to see if there are any validation errors. There should be none.

## Vertex input layout
The triangle is now on GPU memory, so we now have to change the pipeline to render it
We begin by creating a struct to hold the input layout data, and a static function in Vertex that outputs the correct one.

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

    //Position will be stored at Location 2
	VkVertexInputAttributeDescription colorAttribute = {};
	normalAttribute.binding = 0;
	normalAttribute.location = 2;
	normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
	normalAttribute.offset = offsetof(Vertex, color);

	description.attributes.push_back(positionAttribute);
	description.attributes.push_back(normalAttribute);
	description.attributes.push_back(colorAttribute);
	return description;
}

```

VertexInputBindingDescription defines the vertex buffers that act as input. In this case, we are using just 1 vertex buffer, so we will only need one binding.
The stride is sizeof(Vertex), as we want to have it consistent with our normal vertex data

We then create 3 VertexInputAttributeDescription, one per each vertex attribute we have. On each of them, we store its format as `VK_FORMAT_R32G32B32_SFLOAT`, which maps directly to what a glm::vec3 is, (3 float numbers of 32 bits each), and use the offset in the Vertex struct.

With this, we now map directly our Vertex struct into what vulkan expects on the pipelines.

## New vertex shader
Lets now create a new shader, mesh.vert, that will use this vertex inputs. This vertex shader will be mapped with the colored_triangle.frag fragment shader. Make sure to refresh cmake so that it finds the new shader and compiles it.

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

We no longer need to use vertexID to do anything, we can just send vPosition directly into gl_Position. Grabbing the data by vertex id will be done automatically by the driver when it compiles the pipeline.
All 3 parameters will work in the same way, according to the AttributeDescription we just wrote.

## Putting all together
We now have the buffer uploaded, the shader written, and the input description filled. Its now time to compile the _meshPipeline, and use it to render our new triangle, which should be green.

Go to the function `init_pipelines` of our VulkanEngine, as we will create the pipeline at the end of it


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
}
```

There is not much here, other than connecting the vertex input info to the pipeline builder. With that and adding the tri_mesh vertex shader, thats all we need.
Now we are holding a _meshPipeline that knows how to render a colored mesh. Lets use it. Lets replace the inner loop of draw() function to use the new pipeline and draw the mesh.


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

That's it, if you run this, you should see a green triangle.
The magic of it, is that this triangle is not hardcoded. It doesn't even have to be a triangle. With this you can render any mesh you want.

The draw code is almost the same as before, except we now do vkCmdBindVertexBuffers
With that call, we tell Vulkan where to fetch the vertex data from, connecting the shaders to the buffer where we stored the triangle data.




 

