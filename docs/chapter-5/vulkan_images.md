---
layout: default
title: Vulkan Images
parent: Chapter 5
nav_order: 12
---


Images are the other "storage" type that vulkan has, besides Buffers. Unlike buffers, images are more complicated due to all the logic they manage, and their settings.

a VkImage is what holds the actual texture data. It holds the pixels and main memory of the texture, but doesnt contain a lot of information on how to read it.

A VkImageView is a wrapper on top of the VkImage. It holds information about how to interpret the data of the texture, for example if you want to only access a region or layer, and if you want to shuffle the pixel channels in a specific way.

Last, we have VkSampler, which holds the data for the specific shader access to the texture. A Sampler is equivalent to the same thing in OpenGL, and it holds information about how to blend the pixels, or how to do mipmapping. Samplers are used with VkImageViews.

## Image formats and layouts

Images can be made from many different memory formats. You can have images where 1 pixel is just a single integer character at 8 bits, or you can have images that hold 4 32 bit floats per pixels. There are many different formats you can use, and some of them can only be used in specific ways. When you want to use a format in your codebase, you need to check if that format is supported for what you want to do. Usually there are a lot of formats you can read from in your shaders, but if you want to use a image as framebuffer for rendering into it, the selection is more limited. 

Layouts are the way the pixels are arranged in memory. The basic LINEAL layout is just a packed 2d array of pixels, with each row being contiguous. This format matches the typical formats used in CPU memory, and its great to load the texture. The problem with it is that reading from a texture with a Linear layout is generally very slow, or even not supported. For the fastest reading, you want to switch the texture into a OPTIMAL layout. Optimal layouts are completely opaque, and change beetween driver vendors. Changing a texture into optimal layout tells the driver to shuffle the pixels however it wants for optimal hardware usage.