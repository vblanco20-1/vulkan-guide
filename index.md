---
layout: default
title: Home
nav_order: 1
description: "Guide to the Vulkan API"
permalink: /
---


## Welcome to VulkanGuide

Welcome to a new work-in-progress Vulkan guide. The focus of this guide is to understand Vulkan correctly, and act as a stepping stone for then working in your own projects. Unlike most samples and other Vulkan guides, which like to hardcode rendering loops, in here we will have a focus around dynamic rendering, so that it can act as a better base code for a game engine.

While the guide is focused around game rendering, its concepts can be used in CAD and visualization just fine.

We will be using C++, but we arent doing complex C++ unique features, so following this guide on C or Rust can be doable.

This guide is meant for the people who already know the basics about 3d graphics, and have worked with either OpenGL or DirectX in the past. This guide will not explain 3d rendering basics such as linear algebra math.


To contact the authors, email `contact@vkguide.dev`

The code in the guide is proven to work in Nintendo Switch. If you are a registered dev, contact authors asking for the Nintendo switch code.


The guide is separated into multiple chapters. Each chapter focuses on a narrow part of Vulkan.

- **Introduction**: Overview about Vulkan API and the libraries used by this project
-  **Chapter 0**: Setting up the guide initial code
-  **Chapter 1**: Vulkan initialization and render loop setup. (Draws a flashing clear color)
-  **Chapter 2**: Vulkan triangle drawing (Draws a hardcoded triangle)
-  **Chapter 3**: Vulkan mesh drawing (Draws meshes loaded from .obj files)
-  **Chapter 4**: Vulkan shader input/output, Descriptor Sets.
-  **Chapter 5**: Vulkan textured rendering

Next: [Introduction to vulkan]({{ site.baseurl }}{% link docs/introduction/vulkan_overview.md %})