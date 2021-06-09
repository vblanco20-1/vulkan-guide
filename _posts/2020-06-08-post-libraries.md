---
layout: post
title: "Code: Space Battle, Huge edition"
excerpt_separator: "<!--more-->"
feature-img: "assets/img/screenshots/hugebattle.jpg"
thumbnail: "assets/img/screenshots/hugebattle.jpg"
categories:
  - Code Experiment
tags:
  - C++
  - Code
  - ECS
  - Experiment
  - DirectX
---

## Project layout

For the Vulkan engine, we use a specific folder layout.

- `/assets` will contain textures and 3d models that we use over the guide
- `/bin` is where the executables will get built. We don't use the normal CMake build folders to simplify the paths to assets and shaders
- `/shaders` will contain all our shaders, and their compiled SPIR-V output
- `/src` contains the source code for the main application and engine
- `/third_party` contains all of the libraries we use, vendored in

## Libraries

On the engine, we use a set of libraries, most stored in `/third_party`. The only library that isn't vendored-in is SDL

All the libraries in third-party are vendored in, and Cmake will build them automatically. For SDL, it's required to build it separated from the project, and tell Cmake where to find it.

- [GLM (openGL Mathematics)](https://github.com/g-truc/glm) Mathematics library, Header only. We use this library to use its matrix and vector math functionality. It's a library that contains types that are directly compatible with GLSL types in most cases. For example, a `glm::mat4` has similar operations and is directly compatible with a `mat4` in a shader. It says OpenGL in the name, but it works great with Vulkan too.
- [SDL](https://www.libsdl.org/) Windowing and input library, Separate build. SDL is one of the most used libraries to create a window and have input on a crossplatform way. SDL works in almost every platform, and it's generally very well supported. Used in Unreal Engine, Unity, Source, and others. We use it in the project to have a easy and fast way to open a window, and have detailed keyboard input.
- [dear IMGUI](https://github.com/ocornut/imgui) Easy to use immediate User-Interface library. This library allows us to create editable widgets such as sliders and windows for user interface. It's widely used in the game industry for debug tools. On the project, we use it to create interactive options for the rendering.

- [STB Image](https://github.com/nothings/stb) Image load library, header only. Small and easy to use library to load image files. It can load a few common formats such as BMP, PNG, JPEG, and others. Part of a set of other similar single-file libraries.

- [Tiny Obj Loader](https://github.com/tinyobjloader/tinyobjloader) .Obj model loader library, header only. Fast and small library to load the .obj 3d model format that we will use when loading 3d models.

- [Vk Bootstrap](https://github.com/charles-lunarg/vk-bootstrap/blob/master/src/VkBootstrap.cpp) Abstracts a big amount of boilerplate that Vulkan has when setting up. Most of that code is written once and never touched again, so we will skip most of it using this library. This library simplifies instance creation, swapchain creation, and extension loading. It will be removed from the project eventually in an optional chapter that explains how to initialize that Vulkan boilerplate the "manual" way.

- [VMA (Vulkan memory allocator)](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) Implements memory allocators for Vulkan, header only. In Vulkan, the user has to deal with the memory allocation of buffers, images, and other resources on their own. This can be very difficult to get right in a performant and safe way. Vulkan Memory Allocator does it for us and allows us to simplify the creation of images and other resources. Widely used in personal Vulkan engines or smaller scale projects like emulators. Very high end projects like Unreal Engine or AAA engines write their own memory allocators.