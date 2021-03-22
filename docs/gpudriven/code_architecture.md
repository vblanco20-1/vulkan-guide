---
layout: default
title: Engine architecture overview
parent: GPU Driven Rendering
nav_order: 3
---

# Code map

To find the code that implements all of the draw indirect improvements, it's on the "engine" branch of the github repo. For a direct link, here it is: [Repo](https://github.com/vblanco20-1/vulkan-guide/tree/engine)

The codebase continues from where Chapter 5 left off, but a lot of improvements and abstractions were added to it. Everything in the "Extra" chapter is on here, and there are a few things that don't have an article on Extra chapter.

* Imgui support: Added imgui UI to the engine, can be found mostly on VulkanEngine class. Explained [here]({{ site.baseurl }}{% link docs/extra-chapter/implementing_imgui.md %})
* CVars.h/cpp: Implements a CVar system for some configuration variables. Explained in [here]({{ site.baseurl }}{% link docs/extra-chapter/cvar_system.md %}). The implementation in here also has support for tweaking the variables in Imgui.
* player_camera.h : Small camera system to be able to fly on the maps.
* logger.h : Log system to have more nice info/error messages in the console.
* vk_pushbuffer : Adds data into a buffer for use with dynamic descriptors.
* vk_profiler : Adds a timing profiler to know how much each pass takes, also can display how many triangles are handled by the gpu. Connects to Imgui.
* vk_engine_scenerender.cpp : Separates the draw and cull logic from the main vk_engine.cpp . Here it executes the culling and draw commands.
* vk_scene : Contains the scene management for the draw indirect buffers and the different mesh passes.
* material_system : New material system to allow for a given material to render in multiple mesh passes and abstracts pipelines and descriptors.
* vk_descriptors : Full abstraction for descriptor sets, explained [here]({{ site.baseurl }}{% link docs/extra-chapter/abstracting_descriptors.md %})
* vk_shaders : shader compiling code. It uses spirv-reflect to automatically build Pipeline Layouts from the shaders and grab other info.
* Asset System and baker: Coming from the one explained [here]({{ site.baseurl }}{% link docs/extra-chapter/asset_system.md %}). But it has support for more optimized mesh formats, and supports prefabs and materials. It can now load arbitrary GLTF files and FBX files. A prefab is a list of scenenodes, and gets converted into multiple renderable objects on load.
* Compute Shaders : Logic for compute shaders was added into the main Vulkan Engine class. There is now a ComputePipelineBuilder and more features around memory syncronization.
* Improved buffer handling: Uniform Buffers and Storage Buffers now have a few improvements with things like a Reallocate function for growing buffers. Mostly on vk_engine.cpp

# Render Flow

The main engine render loop is similar to the one after the chapters, but a lot of things were added to it. First, the handling of renderable objects goes through vk_scene, and is loaded from prefabs.

When the engine initializes, it loads some prefabs and spawns them into the world as MeshObjects, which it injects into the RenderScene that will then add the objects into the multiple mesh passes according to materials and configuration. 

There are 3 mesh passes handled. Forward pass handles the "opaque" rendering of objects, Transparent handles the translucent objects, and draws after the opaque objects are finished. Then there is the Shadow pass that will render a sun shadow. MeshObjects will register into these 3 passes according to their setup. Opaque objects will be added to Forward and Shadow passes, while translucent objects will only register into the Transparent pass, as we don't want the translucent objects to cast shadow.

Once the engine is loaded, `RenderScene::build_batches()` and `RenderScene::merge_meshes()` are called at the end of engine initialization. build_batches will process all the mesh passes and prepare their indirect draw commands. merge_meshes will grab the vertex buffers of each of the meshes registered with the RenderScene, and merge them into a huge vertex buffer. This allows us to bind the vertex buffer once per mesh pass and never touch it again.

With the initialization done, we go into the frame loop.

At the start of the frame loop, we flush the descriptor cache and frame deletion queue to make sure dynamic things are reset. 
Then it calls `ready_mesh_draw()` that will process the changes on object data and upload everything to the GPU. This is the main step that uploads the data processed in RenderScene to the GPU.

Once that is finished, we start preparing the data for the compute culling pass. `ready_cull_data` is called for each of the mesh passes in the RenderScene, this will reset the gpu draw state to its "default" state, ready to then be written to by the cull compute shader.

When the multiple ready_cull calls are finished, we execute a pipeline barrier to make sure that all the memory transfers are finished before the compute shaders begin execution.

After that, culling is executed for the 3 passes. Forward and Transparent pass use the same settings as they are both culling from the main camera view, while shadow pass uses different logic.

With the compute shaders for the cull executed, we do another barrier on the GPU to make sure that the gpu finishes the execution of the compute shaders before starting the draw commands.

We then execute the draw commands themselves. First shadow_pass() executes, which renders the scene depth from the sun location into a depth texture. Then the forward pass executes, and begins by rendering all of the opaque meshes, and then following by rendering all of the transparent meshes.

Once the rendering ends, the depth buffer is converted into a depth pyramid, which is what will be used for the culling in the next frame.

Also we copy the rendered image into the swapchain to get it ready for presenting.







{: .fs-6 .fw-300 }
{% include comments.html term="GPU Driven Rendering" %}