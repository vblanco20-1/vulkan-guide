---
layout: default
title: Vertex buffers
parent: Chapter 3
nav_order: 14
---

Right now, we are rendering triangles by hardcoding the vertex arrays in the shader. We are now going to change the code to render the triangles in the same way, but without hardcoding the vertices, using CPU arrays for them, and vertex input layouts.

## Vertex buffers
In Vulkan, you can allocate memory on the GPU, and read it from the shaders. There are 2 main types of memory you can use, Images and Buffers.
We have already been using images a bit as part of the render pass and the swapchain, but using them in the shaders can be complicated so we leave it for a later chapter. Images are used mostly for 2d or 3d data, like textures, where you want to access any part of it from the shaders.

The other type is buffers, which are just a bunch of memory that the GPU can see and write/read to. There are multiple types of buffers, and they can be used in different ways. In here, we are going to create a buffer and use it as a Vertex Buffer, which will allow the GPU to read the data from the buffer and send it to our vertex shader.

To read a vertex buffer from a shader, you need to set Vertex Input State on the pipeline. This will let Vulkan know how to interpret a given buffer as vertex data. Once this is setup, we will be able to get vertex information on the vertex shader, like vertex color or vertex position, automatically.

This type of buffers work more or less the same as they do on OpenGL and DirectX, if you are familiar with those.

## Memory allocation
All buffers and images need to be created on top of a memory allocation. As we dont want to create our own allocation here, we will be using the library Vulkan Memory Allocator, created by AMD, which will abstract this for us.

The number of total allocations is fixed by the driver, and can be a number as low as 1024. This is too low if we have a big amount of data, so the normal way is to use a library like Vulkan Memory Allocator (VMA) or create the allocator yourself. You allocate big chunks of memory from the driver, and then sub-allocate your buffers and images on it.

In Vulkan, you have direct control over what kind of memory is requested in an allocation. The VMA library abstracts this slightly for us, but its still very important to know.

If you look at a desktop computer, you will see that the GPU has its own RAM, and so does the CPU. There are 2 different memory systems on the computer. Vulkan lets you decide where to allocate your data, be it on native GPU VRAM, or on the CPU RAM. For integrated GPUs, there is only CPU RAM, so all allocations go there, but even then, its possible that the GPU has some memory that it accesses better.

When allocating buffers and images, its very important to decide where to put them, as it will directly affect performance of the program.
The CPU can only write to buffers allocated in CPU RAM, and then the GPU can read those, but slower. Memory allocated directly on the GPU is much faster to use, but the CPU cant access it, so you need to *transfer* the memory from CPU RAM to GPU RAM if you want to do that.

For now, we are going to be allocating our buffers all on CPU side RAM, as its significantly easier to implement, and at the moment we dont need the performance. Once we setup rendering textures, we will see how those transfers work.

## Vertex Input Layout
If you come from OpenGL, you may know about vertex attributes, where you let OpenGL know how should vertex data be read on your vertex shader, and then OpenGL fetches the data for you.
In Vulkan, it works in a very similar way. When you create the pipeline, one of the editable objects is the Vertex Input Layout, which we are leaving empty now. In there, you put the data to let Vulkan know how to intepret the vertex data for your shader.

When the driver compiles a pipeline, it will patch the vertex shader with the code needed to fetch the vertex data you have configured. In modern GPUs, you dont need to use it, and you can just fetch the data from a buffer yourself in the shader. Similar to what we are doing with our triangle, but using a Buffer instead of having the data in a hardcoded array. Doing that is known as "vertex pulling", opposed to the typical vertex attribute way, which is known as "vertex pushing". On older gpus, using vertex attributes will have higher performance than doing it yourself. From personal benchmarking (https://github.com/nlguillemot/ProgrammablePulling) GPUs from the NVidia Turing family (RTX 2060 and more) will have the same performance. While older gpus such as a 970 GTX will see a 20-30% perf hit.

Lets go ahead and implement what we need to turn our hardcoded triangle into a non-hardcoded one

Next: [Implementing vertex buffers]({{ site.baseurl }}{% link docs/chapter-3/triangle_mesh_code.md %})



