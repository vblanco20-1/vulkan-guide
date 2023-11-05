---
layout: default
title: Vulkan Shaders
parent:  "New 2. Drawing with Compute"
nav_order: 3
---

## Running code on the GPU
We have the render loop implemented now, so the next step is to draw something.

Instead of drawing geometry just yet, we are going to use Compute Shaders to write data into a image, and then display that image to the screen. Compute shaders to calculate images is a very common use case that you can see from complex postprocessing chains in engines. Its also common to do when the programmer is going to do raytracing or other non-geometry drawing.

## VkPipeline
In vulkan, to execute code on the gpu, we need to setup a pipeline. There are two types of pipelines, Graphics and Compute. The Compute pipelines are much simpler, because they only require the data for the shader code, and the layout for the descriptors used for data bindings. Graphics pipelines, on the other hand, have to configure a considerable amount of state for all of the fixed function hardware in the GPU such as color blending, depth testing, or geometry formats. We will use them next chapter.

Both types of pipelines share the shader modules and the layouts, which are built in the same way. 

# VkShaderModule
A VkShaderModule is a processed shader file. We create it from a pre-compiled SpirV file. 
In vulkan, unlike in opengl, the driver does not accept shader code in GLSL directly. There are extensions that allow it, but its not standard. So we need to compile our GLSL files ahead of time into a compiled spirv file. As part of the vulkan SDK, we have the glslangValidator program, which is used to compile GLSL into spirv. If you look at the main project CMakeLists.txt on the vkguide codebase, you can see the section where it grabs all of the shader files from the /shaders/ folder, and adds them as custom commands into a compile target.

Whenever you want to compile the shaders on this tutorial, you need to build the Shaders target. this will compile all your shader files. Due to cmake limitations, when you add new shader files, you need to re-run the cmake configuration step so that cmake can pick them. On other engines, a common thing to do is to have an executable or a .bat or other similar script that compiles the shaders automatically as the engine opens. 

It is also possible to use HLSL instead of GLSL to code Vulkan shaders. A lot of projects like doing that, but we wont be doing it as part of this tutorial. If you want to look at how to do that, you can check in this official vulkan site [HLSL In Vulkan](https://docs.vulkan.org/guide/latest/hlsl.html)

# VkPipelineLayout
To give data to the shaders, we need to setup some bindings. For example, in this chapter we will connect a draw image to the compute shader so that the shader can write into it. 
In vulkan, shader IO is handled through descriptor sets. A pipeline can have multiple slots to bind a few descriptor sets. The vulkan spec guarantees that we will have at least 4 sets, so that will be what we will target in the tutorial. According to gpu vendors, each descriptor set slot has a cost, so the less we have, the better. 
A given descriptor set has a descriptor set layout, which is what exactly does that descriptor set bind. For example, later in the tutorial we will have Descriptor Set #0 to always bind some global scene data, which will contain some uniform buffers, and some special textures. 

Apart from descriptor set layouts on their binding slots, a pipeline layout can also have some space for PushConstants. PushConstants is a mechanic unique to vulkan which reserves a very small amount of memory to be bound directly to the shader. This one is fast, but its also very limited in space and must be written as you encode the commands. Its main use is that you can give it some per-object data, if your objects dont need much of it, but according to gpu vendors, their best use case is to send some indexes to the shader to be used to access some bigger data buffers. 

PipelineLayouts for a graphics and compute pipeline are made in the same way, and they must be created before the pipeline itself. A pipeline layout needs an array of descriptor set layouts and the push constant ranges. 

## The Compute Pipeline
To build a compute pipeline, we need to create first the pipeline layout for it, and then hook a single shader module for its code. Once its built, we can execute the compute shader by first calling `VkCmdBindPipeline` and then calling `VkCmdDispatch`.

Compute shaders have a specific programming model. When we call vkCmdDispatch, we give vulkan a number of work groups to launch in 3 dimensions in a X * Y * Z fashion. In our case here, we will be using it to draw a image, so we only use 2 of those dimensions, that way we can execute one workgroup per group of pixels in the image.

Inside the shader itself, we can see  `layout (local_size_x = 16, local_size_y = 16) in;` By doing that, we are setting the size of a single workgroup. This means that for every work unit from the vkCmdDispatch, we will have 16x16 lanes of execution, which works well to write into a 16x16 pixel square. 

On the shader code, we can access what the lane index is through `gl_LocalInvocationID` variable. There is also `gl_GlobalInvocationID` and `gl_WorkGroupID`.  By using those variables we can find out what pixel exactly do we write from each lane.

# Writing into images
For now, we have done the VkCmdClear on the swapchain image. But swapchain images have the problem that they are an OS provided construct, and they are generally limited in various ways. There are cases where we wouldnt be able to use a compute shader to directly write into a swapchain image, and their format can vary. We need the format of the target image from the shader itself for it to work (outside of extensions), so directly writing into the swapchain image is going to be a problem. We could do it by setting the correct flags, but the normal thing to do is that we dont draw into the swapchain images. Instead, we create our own image on the format and size we want, and after rendering it we copy it to the swapchain image. This way we can lower latency by decoupling the direct connection between asking the OS for an image and the drawing, and we can use other formats that the ones the OS windowing system wants.

From now on, we will be doing our rendering commands into a image that has the format RGBA 16f. which means that we have 4 channels each of them a half-width float. This format is relatively overkill in precision, but it will be ok for our needs. If you know exactly what you want from your draw shaders and postprocess chain, you can use packed formats which have RGB only at 32 bits, which tend to be enough extra precision but low size. 

Lets now set all of this up in the code and get our first shaders working.


