---
layout: default
title: Vulkan API
nav_order: 1
parent: introduction
---

![VulkanLogo](/assets/images/Vulkan_170px_Dec16.jpg)

## What is Vulkan?

Vulkan is a cross platform graphics API (Application Program Interface), developed by the Khronos Group.
The Vulkan API is a standard and therefore it can be implemented on multiple devices, platforms and in a variety
of different programming languages.

Vulkan is designed to be used in high performance multithreaded applications.
Vulkan especially benefits from its design as it is more explicit, reducing the guesswork performed by the graphics driver. 
Using Vulkan correctly yields high level performance, low latency, and consistent frame times.

## Overview of Vulkan API

The Vulkan API provides a list of functions, what arguments to pass in, and the return type.
The API is written in the C programming language, and should be compatible with a vast majority of languages.

Although this tutorial is written in C++, porting it to other low level languages such as Rust or even plain C should be straight forward.

## When to use Vulkan

If your main bottleneck is GPU-side, it is unlikely that implementing Vulkan will improve performance enough to be worth it.
It should be mentioned that Vulkan is significantly harder to use compared to OpenGL, and its extra complexity often means longer development times.

If your application consists of big maps, dynamic worlds, or CAD type scenes with a lot of objects, then Vulkan's multithreaded capabilities will likely boost performance.

The mobile market has a problem with fragmentation. Not every phone implements Vulkan (especially older phones) and of those that do, you may have problems with driver quality. The phones with good drivers for Vulkan especially benefit from the lower CPU overhead and often see extensive performance boosts all while using less battery.

## Cross platform

Vulkan's design is unique compared to other Graphics APIs because it offers the same API for both Mobile and Desktop.
This means that many features in Vulkan are optional and using them requires explicitly turning them on. 
It is common to have features that are unsupported on Mobile but supported on most Desktop GPUs.

When running a Vulkan powered application on both Desktop and Mobile, you have the option of using one or two core render paths.
It is recommended to run two core render path as the alternative will be at the cost of sub optimal performance.
The API being unified means the two render paths can share code between them.

This tutorial will be focused on writing Vulkan for Desktop, however we will cover many of the differences between the two types of GPUs.

## Validation layers

The vulkan API is vast and as a result it is easy to make mistakes, but this is where Validation Layers comes to the rescue.

Validation Layers is an optional feature in Vulkan that detects and reports incorrect usage of the API.

Validation Layers work by intercepting your function calls and performs validation on the data. If all data is correctly validated a call to the driver will be performed. It should be noted that intercepting functions and running validation comes with a performance loss.

The Validation Layers are useful for catching errors such as using incorrect configurations, using wrong enums, synchronization issues, and object lifetimes. It is recommended to enable Validation Layers while developing to ensure that your code follows the specification requirements. Running your application without any reported validation errors is a good sign, however this should not be used as an indicator for how well your application will run on different hardware. We recommend to test your application on as many different devices as possible.

It is important to note that the Validation Layers does not catch bugs like uninitialized memory and bad pointers. It is highly recommended to use tools such as Address Sanitizer and Valgrind. Be aware that graphics drivers often create false positives while using these tools, making their output quite noisy.

## Usage and general mindset

In the Vulkan API, almost everything is designed around objects that you create manually and then use. This is not only for the actual GPU resources such as Images/Textures and Buffers (for memory or things like Vertex data) but also for a lot of "internal" configuration structures.

For example, things like GPU fixed function, like rasterizer mode, are stored into a Pipeline object which holds shaders and other configuration. In Opengl and DX11, this is calculated "on the fly" while rendering.
When you use Vulkan, you need to think if its worth to cache these objects, or create them while rendering on-the-fly. Some objects like Pipelines are expensive to create, so its best to create them on load screens or background threads. Other objects are cheaper to create, such as DescriptorSets and its fine to create them when you need them during rendering.

Because everything in Vulkan is "pre-built" by default, it means that most of the state validation in the GPU will be done when you create the object, and the rendering itself does less work and is faster. Good understanding of how these objects are created and used will allow you to control how everything executes in a way that will make the framerate smooth.

When doing actual GPU commands, all of the work on the GPU has to be recorded into a CommandBuffer, and submitted into a Queue. You first allocate a command buffer, start encoding things on it, and then you execute it by adding it to a Queue. When you submit a command buffer into a queue, it will start executing on the GPU side. You have tools to control when that execution has finished. If you submit multiple command buffers into different queues, it is possible that they execute in parallel.

There is no concept of a frame in Vulkan. This means that the way you render is entirely up to you. The only thing that matters is when you have to display the frame to the screen, which is done through a swapchain. But there is no fundamental difference between rendering and then sending the images over the network, or saving the images into a file, or displaying it into the screen through the swapchain.

This means it is possible to use Vulkan in an entirely headless mode, where nothing is displayed to the screen. You can render the images and then store them onto the disk (very useful for testing!) or use Vulkan as a way to perform GPU calculations such as a raytracer or other compute tasks.

Next: [Vulkan Render flow]({{ site.baseurl }}{% link docs/introduction/vulkan_execution.md %})


{% include comments.html term="Introduction Comments" %}