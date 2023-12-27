---
layout: default
title: Vulkan Images
parent: "(OLD) 5. Textures"
grand_parent: (old) Vkguide
nav_order: 12
---


Images are the other "storage" type that Vulkan has, besides Buffers. Unlike buffers, images are more complicated due to all the logic they manage, and their settings.

A `VkImage` is what holds the actual texture data. It holds the pixels and main memory of the texture, but doesn't contain a lot of information on how to read it.

A `VkImageView` is a wrapper on top of the `VkImage`. It holds information about how to interpret the data of the texture, for example if you want to only access a region or layer, and if you want to shuffle the pixel channels in a specific way.

Last, we have `VkSampler`, which holds the data for the specific shader access to the texture. A Sampler is equivalent to the same thing in OpenGL, and it holds information about how to blend the pixels, or how to do mipmapping. Samplers are used with `VkImageView`s in descriptors.

## VkImage
We will be creating `VkImages` by using VulkanMemoryAllocator. Because `VkImage` are composed of the actual pixel data, it's necessary to allocate them on some memory, preferably on GPU local memory (`VK_MEMORY_HEAP_DEVICE_LOCAL_BIT` or VMA `VMA_MEMORY_USAGE_GPU_ONLY`). Images generally have very high memory traffic usages, so you want them to be as fast as possible to read and write. 
You can think of images as a layer on top of buffers, but with a lot of extra logic and data that is needed for all the fixed pipeline texture logic in the GPUs, and features such as texture layers or mipmaps. It's possible to copy image data to buffers, and reverse. 
While an image holds the pixel data in whatever format you specify, and has a specific layout, it does not specify how to interpret the data from shaders. That is calculated by the `VkImageView` and samplers.

## Image formats and layouts

Images can be made from many different memory formats. You can have images where 1 pixel is just a single integer character at 8 bits, or you can have images that hold 4 32 bit floats per pixels. There are many different formats you can use, and some of them can only be used in specific ways. When you want to use a format in your codebase, you need to check if that format is supported for what you want to do. Usually there are a lot of formats you can read from in your shaders, but if you want to use an image as framebuffer for rendering into it, the selection is more limited.  Always make sure to check what image formats are available in your target GPUs. You can look in here to check what's the availability for your GPU. https://vulkan.gpuinfo.org/listformats.php . You want to check that your target format can be read on an Optimal layout, not Linear.

Layouts are the way the pixels are arranged in memory. The basic Linear layout is just a packed 2d array of pixels, with each row being contiguous. This format matches the typical formats used in CPU memory, and it's great to load the texture. The problem with it is that reading from a texture with a Linear layout is generally very slow, or even not supported. For the fastest reading, you want to switch the texture into a OPTIMAL layout. Optimal layouts are completely opaque, and change between driver vendors. Changing a texture into optimal layout tells the driver to shuffle the pixels however it wants for optimal hardware usage.
 
Images always begin their lifetime in `VK_IMAGE_LAYOUT_UNDEFINED` layout, so you need to convert them into another layout, such as `VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL`. To change the layout of a texture, you need to perform a Pipeline Barrier on your command buffer. Pipeline barriers are generally used to control the execution of shaders to synchronize access to a resource such as a buffer or a image. If you use them for an image, the image can change its layout.

## VkImageView
An image view is a reference to a `VkImage`. Unlike `VkImage`, it does not need to be allocated on GPU memory, so you create them directly from the Vulkan API. Think of an image view as a fancy pointer into the pixel data of a `VkImage`. When you create a image view, you specify the format of the texture, and other things such as the base mipmap level or the specific layer of a texture array.
One of the things you can do with them, is to interpret a given pixel format as another format. This is something that can be used for some specific effects, but you have to be careful with it. It's also possible to use an image view to shuffle the color channels of an image, for example interpreting an RGBA texture with an only-alpha image view which would be a different 1-channel format. 
When using images in the vulkan API, almost always you will use them through a VkImageView. 

## VkSampler
Samplers hold how exactly the texture is accessed in the shader. They control fixed pipeline state such as mipmap blending or nearest filtering. They can control anisotropic filtering, or control what happens when you get out of bounds of the texture as you sample it. 
Creating a `VkSampler` is a completely separate path from creating `VkImage` objects and `VkImageView` objects. You don't create samplers connected to a texture, as they are standalone. In engines, it's quite common to cache the sampler objects in a hashmap, and allocate them as needed. You will generally only end up with a few different samplers for an entire engine.

When you bind textures to a shader, there are different ways, but we will be using `VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER` for the descriptors. This will expose image + sampler pairs to the shader, instead of sampler and image being a separated thing in the shaders. When creating those descriptors, you will need an imageView for the image you want, alongside a sampler for how to access it.



Next: [Loading images]({{ site.baseurl }}{% link docs/chapter-5/loading_images.md %})


{% include comments.html term="Chapter 5 Comments" %}


