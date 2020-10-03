---
layout: default
title: Great resources
nav_order: 100
has_children: false
permalink: /docs/great_resources
---


This guide does not cover everything you could need, so this is a compilation of high quality vulkan related materials and resources.

## Other Tutorials
- [Vulkan-Tutorial](https://vulkan-tutorial.com/). The most popular vulkan tutorial on the internet. Goes a lot more into detail on vulkan initialization, also talks about things we dont do here.
- [LearnOpenGL](https://learnopengl.com/). OpenGL tutorial. While we use vulkan, this tutorial talks a lot more about graphics programming basics such as graphics pipeline, transformation math, and lighting techniques. Recomended you try to go through its chapters but using vulkan instead.
- [Filament Engine PBR Documentation](https://google.github.io/filament/Filament.html). Best in class documentation about high performance PBR lighting shaders. 


## Talks/Slides
- [Destiny's Multithreaded Rendering Architecture](https://www.youtube.com/watch?v=0nTDFLMLX9k). Advanced. Talks about the design and general architecture of the game engine for the game Destiny.
- [The Devil is in the details](http://advances.realtimerendering.com/s2016/) Talk about the Doom 2016 render engine. Goes alongside its [graphics study](http://www.adriancourreges.com/blog/2016/09/09/doom-2016-graphics-study/)
- [Rendering the Hellscape of Doom Eternal](http://advances.realtimerendering.com/s2020/index.htm). Advanced. Doom Eternal engine architecture, bleeding edge Vulkan usage. Goes with the [graphics study](https://simoncoenen.com/blog/programming/graphics/DoomEternalStudy.html) where the exact details are shown.
- [Vulkan Fast Paths, AMD](http://gpuopen.com/wp-content/uploads/2016/03/VulkanFastPaths.pdf). Intermediate. How vulkan structures map to the AMD GPUs, and whats the fastest approach to use.
- [GPU Driven Rendering, NVidia](https://on-demand.gputechconf.com/gtc/2016/presentation/s6138-christoph-kubisch-pierre-boudier-gpu-driven-rendering.pdf) How to use vulkan to do very high object counts for CAD. Also talks about how NVidia gpus execute vulkan.
- [Vulkan and DX12, lessons learned](https://www.gdcvault.com/play/1024122/D3D12-Vulkan-Lessons). Best practises when using vulkan and directx12 for high performance games.
- [Most common mistakes](https://gpuopen.com/wp-content/uploads/2016/05/Most-common-mistakes-in-Vulkan-apps.pdf). Slides by AMD talking about common errors by vulkan implementations and how to do them right.


## Code
- [Sasha Willems Vulkan samples](https://github.com/SaschaWillems/Vulkan). Great set of vulkan samples, with a lot of features implemented on them.
- [Official Vulkan samples](https://github.com/KhronosGroup/Vulkan-Samples). Newer samples, has tutorials about specific ways of implementing things well.
- [V-EZ](https://github.com/GPUOpen-LibrariesAndSDKs/V-EZ). Abandoned. Minimal simplification layer on top of vulkan. Very useful to see how can one abstract vulkan.
- [Filament Engine](https://github.com/google/filament). Mobile first rendering engine by google. Great vulkan backend for mobile.
- [Niagara Engine](https://github.com/zeux/niagara). Small vulkan-first rendering engine built on stream. Uses very advanced draw indirect and mesh shader techniques to render huge object counts.
- [Quake 1 Vulkan port](https://github.com/Novum/vkQuake). Port of quake 1 engine to vulkan.
- [Doom 3 vulkan port](https://github.com/DustinHLand/vkDOOM3). Port of doom 3 engine to vulkan. More advanced and modern than the quake 1 port.

## Debuggers/tools
- [Renderdoc](https://renderdoc.org/). Vendor agnostic graphics debugger. 
- [Nsight](https://developer.nvidia.com/nsight-visual-studio-edition) Nvidia profiler and debugger.
- [Radeon tools](https://gpuopen.com/vulkan/). Profilers and debuggers from AMD.
- [Intel GPA](https://software.intel.com/content/www/us/en/develop/tools/graphics-performance-analyzers.html) Profiler and debugger from Intel.
- [Tracy](https://github.com/wolfpld/tracy). Great profiler library for Cpp programs. It can also profile GPU executions.


## Blogs/websites
- [Vulkan Spec](https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/index.html) Highly necessary resource for information about the API.
- [Vulkan GPU Info](https://vulkan.gpuinfo.org/) Database of specifications for the different GPU vulkan implementations.
- [GPU Open](https://gpuopen.com/) AMD graphics resources and blogposts.
- [Nvidia gameworks](https://developer.nvidia.com/gameworks) Nvidia graphics resources and blogposts/tutorials.
- [TheMaister blog](https://themaister.net/blog/2020/01/) Low level graphics programming blog. Lots of vulkan posts.
- [Bitsquid engine blog](https://bitsquid.blogspot.com/) Blog of game engine development by the developers of Bitsquid/Stingray game engine. Can be a bit old.
- [OurMachinery engine blog](https://ourmachinery.com/post/). From the same devs as bitsquid/stingray, their next development blog. A lot of vulkan related articles.