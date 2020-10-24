---
layout: default
title: Vertex buffers
parent: Chapter 3
nav_order: 10
---

Right now, we are rendering triangles by hardcoding the vertex arrays in the shader. We are now going to change the code to render the triangles in the same way, but without hardcoding the vertices, using CPU arrays for them, and vertex input layouts.

## Vertex buffers
In Vulkan, you can allocate memory visible to the GPU, and read it from the shaders. You can allocate memory for two purposes, for images and buffers.
We have already been using images a bit as part of the render pass and the swapchain, but using them in the shaders can be complicated so we leave it for a later chapter. Images are used mostly for 2d or 3d data, like textures, where you want to access any part of it from the shaders.

The other type is buffers, which are just a bunch of memory that the GPU can see and write/read to. There are multiple types of buffers, and they can be used in different ways. In here, we are going to create a buffer and use it as a vertex buffer, which will allow the GPU to read the data from the buffer and send it to our vertex shader.

To read a vertex buffer from a shader, you need to set the vertex input state on the pipeline. This will let Vulkan know how to interpret a given buffer as vertex data. Once this is set up, we will be able to get vertex information into the vertex shader, like vertex colors or vertex positions, automatically.

This type of buffers work more or less the same as they do on OpenGL and DirectX, if you are familiar with those.

## Memory allocation
All buffers and images need to be created on top of a memory allocation. As we don't want to implement a memory allocator in this tutorial, we will be using the library Vulkan Memory Allocator (VMA), created by AMD, which will abstract this for us.

The number of total allocations is fixed by the driver, and can be a number as low as 1024. This minimum number can be easily exceeded if each buffer or image are placed in their own memory allocation, so a typical strategy is to allocate big chunks of memory from the driver, and then sub-allocate your buffers and images into it, using a library like Vulkan Memory Allocator (VMA) or by implementing the allocator yourself.

If you look at a desktop computer, you will see that the GPU has separate memory (VRAM) from the main system memory (RAM). These different domains are called heaps. Within these heaps, the driver can expose memory regions optimized for specific usecases, called memory types. Even on integrated GPUs (without dedicated VRAM), multiple memory types might be available from a single heap. Vulkan gives complete control over where to allocate your data, be it on native GPU VRAM, or on the CPU RAM. The VMA library abstracts this slightly for us, but its still very important to know.

For now, we are going to be allocating our buffers all on CPU RAM, as it is significantly easier to implement, and at the moment we don't need the performance.

## Vertex Input Layout
If you come from OpenGL, you may know about vertex attributes, where you let OpenGL know how should vertex data be read on your vertex shader, and then OpenGL fetches the data for you.
In Vulkan, it works in a very similar way. When you create the pipeline, one of the editable objects is the vertex input layout, which have left empty for now. We can fill it out to let Vulkan know how to intepret the vertex data for your shader.

Lets go ahead and implement what we need to turn our hardcoded triangle into a non-hardcoded one!

Next: [Implementing vertex buffers]({{ site.baseurl }}{% link docs/chapter-3/triangle_mesh_code.md %})



{% include comments.html term="Chapter 3 Comments" %}