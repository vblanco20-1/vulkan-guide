---
layout: default
title: Gpu Driven Rendering DRAFT
nav_order: 1
---

## GPU Driven Rendering

Over the last few years, the bleeding edge render engines have been moving more and more towards having the rendering itself calculated in gpu compute shaders.
Thanks to the apperance of MultiDrawIndirect and similar features, its now possible to do a very big amount of the work for the rendering inside compute shaders. The benefits are clear

* GPUs have orders of magnitude higher perf than CPU on data-parallel algorithms. Rendering is almost all data parallel algorithms
* With the GPU deciding its own work, latencies are minimized as there is no roundtrip from cpu to gpu and back.
* Frees the CPU from a lot of work, that can now be used on other things.

The result is an order of magnitude or more scene complexity and object counts. In the code that we will walk through in later chapters, based on the engine at the end of chapter 5 tutorial, we can run 250.000 "drawcalls", on Nintendo Switch, at more than 60 fps. On PC it reaches 1000 fps. We essentially have a nearly unlimited object count, with the bottleneck moved into just how many triangles are we trying to make the gpu draw.

Techniques based on compute-shader-rendering have been becoming more popular in the last 5 years. Famously, Assassins Creed Unity and sequels use these techniques to achieve an order of magnitude more complex scenes, with Paris having an inmense amount of objects as its also rendering interiors for a lot of buildings. Frostbite engine from EA also uses these techniques to have very high geometry detail on Dragon Age Inquisition. These techniques are also the reason Rainbow Six Siege can have  thousands of dynamic rubble objects created from its destruction systems. The techniques have become very popular on the PS4 and Xbox One generation of consoles as they can get easily bottlenecked on triangle throughput, so having very accurate culling gives very great performance gains. 

The core of the idea revolves around the use of Draw Indirect support in the graphics APIs. These techniques work on all apis, but they work best on Vulkan or DX12 because they allow better control over low level memory management and compute barriers. They also work great on the Ps4 and Xbox One consoles, and the next generation has features that leans even more into it, with features such as Mesh Shaders and raytracing.

Draw indirect is a drawcall that takes its parameters from a GPU buffer instead of from the call itself. When using draw indirect, you just start the draw based on a position on a gpu buffer, and then the GPU will execute the draw command in that buffer. 

Because it takes its parameters from a buffer, its possible to use compute shaders to write into these buffers, and do culling or LOD selection in compute shaders. Doing culling this way is one of the simplest and most performant ways of doing culling. Due to the power of the GPU you can easily expect to cull more than a million objects in less than half a milisecond. Normal scenes dont tend to go as far.

When you design a gpu-driven renderer, the main idea is that all of the scene should be on the GPU. In chapter 4, we saw how to store the matrices for all loaded objects into a big SSBO. On GPU driven pipelines, we also want to store more data, such as material ID and cull bounds. Once we have a renderer where everything is stored in big GPU buffers, and we dont use PushConstants or descriptor sets per object, we are ready to go with a gpu-driven-renderer. The design of the tutorial engine is one that maps well into refactoring for a extreme performance compute based engine.

Due to the fact that you want to have as much things on the GPU as possible, this pipeline maps very well if you combine it with "Bindless" techniques, where you stop needing to bind descriptor sets per material or changing vertex buffers. In Doom Eternal engine, they go all-in on bindless, and the engine ends up doing very few drawcalls per frame. On this guide we will not use bindless textures as their support is limited, so we will do 1 draw-indirect call per material used. We will still merge all of the meshes into a big vertex buffer to avoid having to constantly bind it between draws. Having a bindless renderer also makes raytracing much more performant and effective. 

* Assassins Creed Unity Engine + others: https://www.advances.realtimerendering.com/s2015/aaltonenhaar_siggraph2015_combined_final_footer_220dpi.pdf 
* Dragon Age Inquisition mesh culling: https://www.gdcvault.com/play/1023109/Optimizing-the-Graphics-Pipeline-With
* Rainbow Six Siege engine: https://www.gdcvault.com/play/1022990/Rendering-Rainbow-Six-Siege
* Doom Eternal engine: https://advances.realtimerendering.com/s2020/RenderingDoomEternal.pdf
* Nvidia Advanced Scenegraph: https://on-demand.gputechconf.com/gtc/2013/presentations/S3032-Advanced-Scenegraph-Rendering-Pipeline.pdf

## Overview of Vkguide engine architecture for compute rendering.
The techniques here can be directly implemented after the 5 chapters of the core tutorial. The engine also has the things from Extra chapter implemented, but should be easy to follow.

The first thing is to go all in on object data in GPU buffers.Per-object PushConstants are removed, per-object dynamic uniform buffers are removed, and everything is replaced by ObjectBuffer where we store the object matrix and we index into it from the shader. 

We also change the way the meshes work. After loading a scene, we create a BIG vertex buffer, and stuff all of the meshes of the entire map into it. This way we will avoid having to rebind vertex buffers.

With the data management done, we can implement the indirect draw itself. 

The code that manages indirect draws is on the RenderScene class, which will grab all of the objects in a meshpass, and sort them into batches. A Batch is a set of objects that matches material and mesh. Each batch will be rendered with one DrawIndirect call that does instanced drawing. Each mesh pass (forward pass, shadow pass, others) is an array of batches. A given object can be on multiple mesh-passes at once.

In the main ObjectBuffer, we store the object matrix alongside the cull bounds per each object loaded in the engine. 

When starting the frame, we sync the objects that are on each mesh pass into a buffer. This buffer will be an array of ObjectID + BatchID. The BatchID maps directly as an index into the batch array of the mesh-pass.

Once we have that buffer uploaded and synced, we execute a compute shader that performs the culling.
For every object in said array of ObjectID + BatchID pairs, we access ObjectID in the ObjectBuffer, and check if its visible.
If its visible, we insert it into the Batches array, which contains the draw indirect calls, increasing the instance count. We also write it into the indirection buffer that maps from the instance ID of each batch into the ObjectID.

With that done, on the CPU side we execute 1 draw indirect call per batch in a mesh-pass. The gpu will use the parameters it just wrote into from the culling pass, and render everything.


{: .fs-6 .fw-300 }
{% include comments.html term="GPU Driven Rendering" %}