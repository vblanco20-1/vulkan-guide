---
layout: default
title: Home
nav_order: 1
description: "Guide to the Vulkan API"
permalink: /
---


![scene]({{site.baseurl}}/diagrams/fullscene.png)
End stage after the tutorial

## Welcome to VulkanGuide

Welcome to a new work-in-progress Vulkan guide. The focus of this guide is to understand Vulkan correctly, and act as a stepping stone for then working in your own projects. Unlike most samples and other Vulkan guides, which like to hardcode rendering loops, in here we will have a focus around dynamic rendering, so that it can act as a better base code for a game engine.

While the guide is focused around game rendering, its concepts can be used in CAD and visualization just fine.

We will be using C++20, but we aren't doing complex C++ unique features, so following this guide on C or Rust can be doable.

This guide is meant for the people who already know the basics about 3d graphics, and have worked with either OpenGL or DirectX in the past. This guide will not explain 3d rendering basics such as linear algebra math.

To contact the authors, email `contact@vkguide.dev`

The code uses vulkan 1.3, and directly uses those new features to simplify the tutorial and engine architecture. If you need support for an older standard, the older version of the guide can be found here [Legacy VKGuide]({{ site.baseurl }}{% link docs/old_vkguide.md %})

The guide is separated into multiple chapters for code organization.

- **Introduction**: Overview about Vulkan API and the libraries used by this project
-  **Chapter 0**: Setting up the guide initial code
-  **Chapter 1**: Vulkan initialization and render loop setup. (Draws a flashing clear color)
-  **Chapter 2**: Vulkan compute shaders and drawing (Uses a compute shader to draw)
-  **Chapter 3**: Vulkan mesh drawing (Draws meshes using the graphics pipeline)
-  **Chapter 4**: Textures and better descriptor set management
-  **Chapter 5**: Full GLTF scene loading and high performance rendering.

Some extra sections of the guide contain more information that is not part of the main tutorial. Some of them are meant for the Legacy version of vkguide, and will be outdated if you are going with the current version. Outdated articles are flagged at the top. 

- **GPU Driven Rendering**: Using compute shaders to handle rendering for maximum scalability and hundreds of thousands of meshes
- **Extra chapter**: Extra articles for various improvements to the engine shown in the tutorial.


Next: [Introduction to vulkan]({{ site.baseurl }}{% link docs/introduction/vulkan_overview.md %})
{% include comments.html term="Index Comments" %}