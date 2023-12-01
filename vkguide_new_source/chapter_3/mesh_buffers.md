---
layout: default
title: Mesh buffers
parent: "New 3. Graphics Pipelines"
nav_order: 10
---

To render objects properly, we need to send our vertex data to the vertex shader. Right now, we are using a hardcoded array, but that will not work for anything other than a single triangle or similar geometry. 

As we arent using the fixed function vertex attribute fetch logic on the pipeline, we have total freedom on how exactly do we load our vertex data in the shaders. We will be loading the vertices from big gpu buffers passed through Buffer Device Adress, which gives high performance and great flexibility.

## Vulkan Buffers
In vulkan, we can allocate general usage memory through buffers. They are different from images in that they dont need samplers and act more as a typical cpu side structure or array. We can access them in the shaders just as structures or array of structures.

When a buffer is created, we need to set the usage flags for it. We will be setting those as we use them.

For the general read/write operations in the shaders, there are 2 types of buffers. Uniform Buffers and Storage Buffers

With uniform buffers (UBO), only a small amount can be accessed in the shader (vendor dependant, 16 kilobytes guaranteed minimum) and the memory will be read-only. On the other side, this offers the fastest access possible as the GPU might pre-cache it when loading the pipeline. The size limit is only on the part that is bound to the shader. Its completely fine to create a single big uniform buffer, and then bind to the shader only small sections of it. Depending on the hardware, push-constants can be implemented as a type of uniform-buffer handled by the driver.

Storage buffers (SSBO) are fully generic read-write buffers with very high size. Spec minimum size is 128 megabytes, and the modern PC gpus we are targetting with this tutorial all have it at 4 gigabits, and only because its what a uint32 size can hold. Storage buffers dont get preloaded in the same way uniform buffers can, and are more "generic" data load/store. 

Due to the small size of uniform buffers, we cant use them for vertex geometry. But they are great for material parameters and global scene configuration. 

The exact speed difference between uniform buffers and storage buffers depends on the specific gpu and what the shader is doing, so its quite common to use storage buffers for almost everything and take advantage of their greater flexibility, as the possible speed difference might end up not mattering for the project. 

In this benchmark, different ways of accessing buffers are compared https://github.com/sebbbi/perftest . 

When creating the descriptors, its also possible to have them as Dynamic buffer. If you use that, you can control the offset the buffer is bound to when writing the commands. This lets you use 1 descriptor set for multiple objects draws, by storing the uniform data for multiple objects into a big buffer, and then binding that descriptor at different offsets within that. It works well for uniform buffers, but for storage buffers its better to go with device-address.

## Buffer Device Adress
Normally, buffers will need to be bound through descriptor sets, where we would bind 1 buffer of a given type. This means we need to know the specific buffer dimensions from the CPU (for uniform buffers) and need to deal with the lifetime of descriptor sets. For this project, as we are targetting vulkan 1.3, we can take advantage of a different way of accessing buffers, Buffer Device Adress. 
This essentially lets us send a int64 pointer to the gpu (through whatever way) and then access it in the shader, and its even allowed to do pointer math with it. Its essentially the same mechanics as a Cpp pointer would have, with things like linked lists and indirect accesses allowed.

We will be using this for our vertices because accessing a SSBO through device address is faster than accessing it through descriptor sets, and we can send it through push constants for a really fast and really easy way of binding the vertex data to the shaders. 

## Creating buffers
Lets begin writing the code needed to upload a mesh to gpu. First we need a way to create buffers.


```cpp
struct AllocatedBuffer {
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo info;
};
```

We will use this structure to hold the data for a given buffer. We have the `VkBuffer` which is the vulkan handle, and the VmaAllocation and VmaAllocationInfo wich contains metadata about the buffer and its allocation, needed to be able to free the buffer.

Lets add a function into VulkanEngine to create it

```cpp
AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
```

We will take an allocation size, the usage flags, and the vma memory usage so that we can control where the buffer memory is.

This is the implementation side

^code alloc_buffer chapter-3/vk_engine.cpp

First we need to fill the VkBuffercreateInfo structure from vulkan. It takes a size and usage flags.
Then we create the AllocationCreateInfo for the properties needed by VMA. We can use the `VmaMemoryUsage` flags to control where VMA will put our buffer.
With images, we were creating them in device local memory, which is the fastest memory possible as its on GPU VRAM, but with buffers, we have to decide if we want them to be writeable from cpu directly or not. These would be the main usages we can use.

* `VMA_MEMORY_USAGE_GPU_ONLY` Is for purely GPU-local memory. This memory wont be writeable or readable from CPU because its on GPU VRAM, but its the fastest to both read and write with shaders. 
* `VMA_MEMORY_USAGE_CPU_ONLY` Is for memory that is on the CPU RAM. This is memory we can write to from CPU, but the GPU can still read from it. Keep in mind that because this is on CPU ram which is outside of the GPU, the accesses to this will come at a performance hit. It is still quite useful if we have data that changes every frame or small amounts of data where slower access wont matter
* `VMA_MEMORY_USAGE_CPU_TO_GPU` Is also writeable from CPU, but might be faster to access from GPU. On vulkan 1.2 and forwards, GPUs have a small memory region on their own VRAM that is still writeable from CPU. Its size is limited unless we use Resizable BAR, but its memory that is both cpu-writeable and fast to access in GPU
* `VMA_MEMORY_USAGE_GPU_TO_CPU` Used on memory that we want to be safely readable from CPU.

We are using the `VMA_ALLOCATION_CREATE_MAPPED_BIT` on all our buffer allocations. This would map the pointer automatically so we can write to the memory, as long as the buffer is accesible from CPU. VMA will store that pointer as part of the allocationInfo.

With a create buffer function, we also need a destroy buffer function. The only thing we need to do is to call vmaDestroyBuffer

```cpp
void VulkanEngine::destroy_buffer(const AllocatedBuffer& buffer)
{
    vmaDestroyBuffer(_allocator, buffer.buffer, buffer.allocation);
}
```


With this we can create our mesh structure and setup the vertex buffer.

## mesh buffers on gpu

vk_types.h
^code vbuf_types shared/vk_types.h

We need a vertex format, so lets use this one. when creating a vertex format its very important to compact the data as much as possible, but for the current stage of the tutorial it wont matter. We will optimize this vertex format later. The reason the uv parameters are interleaved is due to alignement limitations on GPUs. We want this structure to match the shader version so interleaving it like this improves it.

We store our mesh data into a GPUMeshBuffers struct, which will contain the allocated buffer for both indices and vertices, plus the buffer device adress for the vertices. 

We will create a struct for the push-constants we want to draw the mesh, it will contain the transform matrix for the object, and the device adress for the mesh buffer.

Now we need a function to create those buffers and fill them on the gpu.

^code mesh_create_1 chapter-3/vk_engine.cpp

The function will take a std::span of integers for its indices, and of Vertex for vertices. A span is a pointer + size pair. You can convert from C style arrays or std::vector into it, so its great to use here to avoid data copies.

First we do is to calculate how big the buffers need to be. Then, we create our buffers on GPU-only memory. On the vertex buffer we use these Usage flags: 
 `VK_BUFFER_USAGE_STORAGE_BUFFER_BIT` because its a SSBO, `VK_BUFFER_USAGE_TRANSFER_DST_BIT` because we will do buffer transfer to it, and `VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT` because will be taking its adress. On the index buffer we use `VK_BUFFER_USAGE_INDEX_BUFFER_BIT` to signal that we are going to be using that buffer for indexed draws.

 To take the buffer address, we need to call `vkGetBufferDeviceAddress`, giving it the VkBuffer we want to do that on. Once we have the VkDeviceAddress, we can do pointer math with it if we want, which is useful if we are sub-allocating from a bigger buffer.

 With the buffers allocated, we need to write the data into them. For that, we will be using a staging buffer. This is a very common pattern with vulkan. As GPU_ONLY memory cant be written on CPU, we first write the memory on a temporal staging buffer that is CPU writeable, and then execute a copy command to copy this buffer into the GPU buffers. Its not necesary for meshes to use GPU_ONLY vertex buffers, but its highly recomended unless its something like a CPU side particle system or other dynamic effects.

 ^code mesh_create_2 chapter-3/vk_engine.cpp

We first create the staging buffer, which will be 1 buffer for both of the copies to index and vertex buffers. Its memory type is CPU_ONLY, and its usage flag is `VK_BUFFER_USAGE_TRANSFER_SRC_BIT` as the only thing we will do with it is a copy command.

Once we have the buffer, we can take its mapped adress with GetMappedData(), this gives us a void* pointer we can write to. So we do 2 memcpy commands to copy both spans into it.

With the staging buffer written, we run an `immediate_submit` to run a GPU side command to perform this copy. The command will run 2 VkCmdCopyBuffer commands, which are roughly the same as a memcpy but done by the GPU. You can see how the VkBufferCopy structures mirror directly the memcpys we did to write the staging buffer.

Once the immediate submit is done, we can safely dispose of the staging buffer and delete it.

Note that this pattern is not very efficient, as we are waiting for the GPU command to fully execute before continuing with our CPU side logic. This is something people generally put on a background thread, whose sole job is to execute uploads like this one, and deleting/reusing the staging buffers.

## Drawing a mesh
Lets proceed with making a mesh using all this, and draw it. We will be drawing a indexed rectangle, to combine with our triangle.

The shader needs to change for our vertex buffer, so while we are still going to be using `colored_triangle.frag` for our fragment shader, we will change the vertex shader to load the data from the push-constants. We will create that shader as `colored_triangle_mesh.vert`, as it will be the same as the hardcoded triangle.

```c
#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec3 outColor;

struct Vertex {

	vec3 position;
	float uv_x;
	vec3 normal;
	float uv_y;
	vec4 color;
}; 

//SSBO for vertex array
layout(buffer_reference, std430) readonly buffer VertexBuffer{ 
	Vertex vertices[];
};

//push constants block
layout( push_constant ) uniform constants
{	
	mat4 render_matrix;
	VertexBuffer vertexBuffer;
} PushConstants;

void main() 
{	
	//load vertex data from device adress
	Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

	//output data
	gl_Position = PushConstants.render_matrix * vec4(v.position, 1.0f);
	outColor = v.color.xyz;
}
```

We need to enable the `GL_EXT_buffer_reference` extension so that the shader compiler knows how to handle these buffer references.

Then we have the vertex struct, which is the exact same one as the one we have on CPU. 

After that, we declare the VertexBuffer, which is a readonly buffer that has an array (unsized) of Vertex structures. by having the `buffer_reference` in the layout, that tells the shader that this object is used from buffer adress. `std430` is the alignement rules for the structure. 

We have our push_constant block which holds a single instance of our VertexBuffer, and a matrix. Because the vertex buffer is declared as buffer_reference, this is a uint64 handle, while the matrix is a normal matrix (no references).

From our main(), we index the vertex array using `gl_VertexIndex`, same as we did with the hardcoded array. We dont have -> like in cpp when accessing pointers, in glsl buffer address is accessed as a reference so it uses `.` to access it 
With the vertex grabbed, we just output the color and position we want, multiplying the position with the render matrix. 

Lets create the pipeline now. We will create a new pipeline function, separate from  `init_triangle_pipeline()` but almost the same. Add this to vulkanEngine class

```cpp
VkPipelineLayout _meshPipelineLayout;
VkPipeline _meshPipeline;

GPUMeshBuffers rectangle;

void init_mesh_pipeline();
```

Its going to be mostly a copypaste of `init_triangle_pipeline()`


^code rectangle_shaders chapter-3/vk_engine.cpp


We change the vertex shader to load `colored_triangle_mesh.vert.spv`, and we modify the pipeline layout to give it the push constants struct we defined above.

For the rest of the function, we do the same as in the triangle pipeline function, but changing the pipeline layout and the pipeline name to be the new ones.

^code rectangle_shaders2 chapter-3/vk_engine.cpp

Now we call this function from our main `init_pipelines()` function.

```cpp
void VulkanEngine::init_pipelines()
{
	//COMPUTE PIPELINES	
	init_background_pipelines();


	// GRAPHICS PIPELINES
	init_triangle_pipeline();
	init_mesh_pipeline();
}
```

Next we need to create and upload the mesh. We create a new initialization function, `init_default_data()` for our default data in the engine. Add it into the main init() function, at the end.

^code init_data chapter-3/vk_engine.cpp

We create 2 arrays for vertices and indices, and call the uploadMesh function to convert it all into buffers.

We can now execute the draw. We will add the new draw command `on draw_geometry()` function, after the triangle we had.

```cpp
	//launch a draw command to draw 3 vertices
	vkCmdDraw(cmd, 3, 1, 0, 0);

^code drawrect chapter-3/vk_engine.cpp

	vkCmdEndRendering(cmd);
```

We bind another pipeline, this time the rectangle mesh one.

Then, we use push-constants to upload the vertexBufferAdress to the gpu. For the matrix, we will be defaulting it for now until we implement mesh transformations.

We then need to do a cmdBindIndexBuffer to bind the index buffer for graphics. Sadly there is no way of using device adress here, and you need to give it the VkBuffer and offsets.

Last, we use `vkCmdDrawIndexed` to draw 2 triangles (6 indices). This is the same as the vkCmdDraw, but it uses the currently bound index buffer to draw meshes.

Thats all, we now have a generic way of rendering any mesh. 

Next we will load mesh files from a GLTF in the most basic way so we can play around with fancier things than a rectangle.
