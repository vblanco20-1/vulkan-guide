---
layout: default
title: Vulkan Shaders
parent:  "2. Drawing with Compute"
nav_order: 3
---

## Running code on the GPU
We have the render loop implemented now, so the next step is to draw something.

Instead of drawing geometry just yet, we are going to use Compute Shaders to write data into a image, and then display that image to the screen. Compute shaders to calculate images is a very common use case that you can see from complex postprocessing chains in engines. Its also common to do when the programmer is going to do raytracing or other non-geometry drawing.

## VkPipeline
In vulkan, to execute code on the gpu, we need to setup a pipeline. There are two types of pipelines, Graphics and Compute. The Compute pipelines are much simpler, because they only require the data for the shader code, and the layout for the descriptors used for data bindings. Graphics pipelines, on the other hand, have to configure a considerable amount of state for all of the fixed function hardware in the GPU such as color blending, depth testing, or geometry formats. We will use them next chapter.

Both types of pipelines share the shader modules and the layouts, which are built in the same way. 

## VkShaderModule
A VkShaderModule is a processed shader file. We create it from a pre-compiled SpirV file. 
In vulkan, unlike in opengl, the driver does not accept shader code in GLSL directly. There are extensions that allow it, but its not standard. So we need to compile our GLSL files ahead of time into a compiled spirv file. As part of the vulkan SDK, we have the glslangValidator program, which is used to compile GLSL into spirv. If you look at the main project CMakeLists.txt on the vkguide codebase, you can see the section where it grabs all of the shader files from the /shaders/ folder, and adds them as custom commands into a compile target.

Whenever you want to compile the shaders on this tutorial, you need to build the Shaders target. this will compile all your shader files. Due to cmake limitations, when you add new shader files, you need to re-run the cmake configuration step so that cmake can pick them. On other engines, a common thing to do is to have an executable or a .bat or other similar script that compiles the shaders automatically as the engine opens. 

It is also possible to use HLSL instead of GLSL to code Vulkan shaders. A lot of projects like doing that, but we wont be doing it as part of this tutorial. If you want to look at how to do that, you can check in this official vulkan site [HLSL In Vulkan](https://docs.vulkan.org/guide/latest/hlsl.html)

## Descriptor sets
To give data to the shaders, we need to setup some bindings. In vulkan, binding objects like images and buffers to the shader requires Descriptor Sets.
Think of a single descriptor as a handle or pointer into a resource. That resource being a Buffer or a Image, and also holds other information, such as the size of the buffer, or the type of sampler if it's for an image.
A `VkDescriptorSet` is a pack of those pointers that are bound together. Vulkan does not allow you to bind individual resources in shaders.
Descriptor sets are allocated from a `VkDescriptorPool`, using a `VkDescriptorSetLayout` that contains the information about what that descriptor set holds (for example, 2 images). Once a descriptor set is allocated, you can update its data using `vkUpdateDescriptorSets`, which takes an array of `VkWriteDescriptorSet`. Once you have a fully configured descriptor set, you can bind it to your pipeline using `VkBindDescriptorSets`, and its data will be available in the shader. 

<div class="mxgraph" style="max-width:100%;border:1px solid transparent;" data-mxgraph="{&quot;highlight&quot;:&quot;#0000ff&quot;,&quot;nav&quot;:true,&quot;resize&quot;:true,&quot;toolbar&quot;:&quot;zoom layers lightbox&quot;,&quot;edit&quot;:&quot;_blank&quot;,&quot;xml&quot;:&quot;&lt;mxfile host=\&quot;app.diagrams.net\&quot; modified=\&quot;2020-10-02T16:02:40.267Z\&quot; agent=\&quot;5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.121 Safari/537.36\&quot; etag=\&quot;G0Tl161wDfz5oobdgIFU\&quot; version=\&quot;13.7.7\&quot; type=\&quot;device\&quot;&gt;&lt;diagram id=\&quot;6hGFLwfOUW9BJ-s0fimq\&quot; name=\&quot;Page-1\&quot;&gt;7Vpdk5owFP01PG4HCCA+6qptZ7Yzne706zFKRLrR0BBX6a9vAkEgQdc6wLRs9UFyE0hyzj03N0ED3G+PbymMNx9IgLBhm8HRADPDti0bjPiPsKS5xfNAbghpFMhGpeEx+oWk0ZTWfRSgpNaQEYJZFNeNK7LboRWr2SCl5FBvtia43msMQ6QZHlcQ69avUcA2udW3R6X9HYrCTdGz5Y3zmiVcPYWU7HeyP8MGzkx88+otLJ4lJ5psYEAOlU7B3AD3lBCWX22P9wgLbAvY8vsWZ2pP46Zox665AXhyHCwt5o4CDoUsEso2JCQ7iOeldZrND4knmLxUtnkgJOZGixt/IMZSySvcM8JNG7bFsnZNduyeYEKzHsHcE19uTxglT6hSMxuLL6/R5yWnmpA9XcmRS3bEBFREuaciskWMprwBRRiy6LnONZQuE57anW79SCLeq21K93ZdSZ50bsc0649gkIaIybtK8CeUwrTSLBYNkgv9mM39LG5rzy/yERSlCialKfOPZl+R8D5DvJfQfXmaoWRFo5gR+oiY5kqlowjWD5uIoccYZnQdeOBQnCLCuEI9tE3TdpucwluY/KO70Tr7XHKWZ0QZOl70DVkLnDqWYCTLhzIcWIWIN5VQ4Cm+UHWnChMXgbZvESWPJLGoXWN0nIgQ2IZQOYA0/Sbuf+MWxe/ycVlhdqyVUlnqSOC+XAoyfXWk+atJ8jU5TDAmK8iQYXuYD2+6pDUSvZ97EdYzcO6SDPIJb2D58TEDoKjnVyE7QaI4wQNc8rW2RhLEUbjj1ysOHeK4ToWbR3wxm8iKbRQEeeBGvFu4zJ4naJIRiD/cnRruTI5NeoPlN9JSOKempdNqLDso1p6zGrsz3wBzbNV0dmfnxVtDdNGErNcJYhqffxbwdIarAe8jT0gGHPFO0a2PiAf8AaUhwO4nD/HGdcY0JlrKQ0ZKXgHcy3mI2r4YV1t5SIFvRZdmHkqn+/UaiaBbqnRACnU9hQfnSoXaLSi06GsYCgX9KNT3blPoLaIAmiisVymKaxP1VkRhaUj+w6JwehKFun12uhOFo4nCzkXxfitOgGwPboVz75aJ+HkVAjmdVvQiEDAkgbj9CGQE6oy5oDuBuJpAwGtcNRzQoyiAfpD2juAg0fDlk2HdbfWFbhK51W9XNNfz4Izc+urt6zy4XdEw1miYfl4s5p+qxzdnTmJe8PUWkNE2dw0eCrqCxtF3WO8/TN7O/wpkfHAmf6gg4zUA08Y5haMnFH+Rz2gLR9MGsQkadYG5CRpPg2aZirNQkW+8CM7wQ93IUgTdsFHpTM/6ijPNuXH/s5MHldHL7DQppw123CElyY48ye06SQbq0YrbWZLsXH4PkL34VCX0AFOy11+IvrAGqCJigss23NtSgo81vjLftVRYbwo/eqalHNgOZmNhmeO6W/a5sSi4On8GOByc1dXU6xNnS8O5fqw0XJivjRutwKzvQsArcWdw7cvXVnDWNzXZsolo8j83NPSteNMhRUu5IS+W//3LU5XyD5Zg/hs=&lt;/diagram&gt;&lt;/mxfile&gt;&quot;}"></div>
<script type="text/javascript" src="https://viewer.diagrams.net/js/viewer-static.min.js"></script>

In this chapter we will connect a draw image to the compute shader so that the shader can write into it. Over the tutorial, we will be writing some abstractions that simplify this flow.
A pipeline can have multiple slots to bind a few descriptor sets. The vulkan spec guarantees that we will have at least 4 sets, so that will be what we will target in the tutorial. According to gpu vendors, each descriptor set slot has a cost, so the less we have, the better. Later in the tutorial we will have Descriptor Set #0 to always bind some global scene data, which will contain some uniform buffers, and some special textures. Descriptor Set #1 will be used for per-object data.

## Push Constants
Apart from Descriptor Sets, which are used to connect the shaders to buffers and images, vulkan offers an option to directly write a few bytes of data while recording the command buffer. PushConstants is a mechanic unique to vulkan which reserves a very small amount of memory to be bound directly to the shader. This one is fast, but its also very limited in space and must be written as you encode the commands. Its main use is that you can give it some per-object data, if your objects dont need much of it, but according to gpu vendors, their best use case is to send some indexes to the shader to be used to access some bigger data buffers. 

## Pipeline Layout
A shader has some inputs it needs, and this is what a `VkPipelineLayout` is made of. To create one, we must give it the `VkDescriptorSetLayout` of the descriptor slots it needs, and PushConstantRange defining its push constant usage.
PipelineLayouts for a graphics and compute pipeline are made in the same way, and they must be created before the pipeline itself. 

## The Compute Pipeline
To build a compute pipeline, we need to create first the pipeline layout for it, and then hook a single shader module for its code. Once its built, we can execute the compute shader by first calling `VkCmdBindPipeline` and then calling `VkCmdDispatch`.

Compute shaders have a specific programming model. When we call vkCmdDispatch, we give vulkan a number of work groups to launch in 3 dimensions in a X * Y * Z fashion. In our case here, we will be using it to draw a image, so we only use 2 of those dimensions, that way we can execute one workgroup per group of pixels in the image.

Inside the shader itself, we can see  `layout (local_size_x = 16, local_size_y = 16) in;` By doing that, we are setting the size of a single workgroup. This means that for every work unit from the vkCmdDispatch, we will have 16x16 lanes of execution, which works well to write into a 16x16 pixel square. 

On the shader code, we can access what the lane index is through `gl_LocalInvocationID` variable. There is also `gl_GlobalInvocationID` and `gl_WorkGroupID`.  By using those variables we can find out what pixel exactly do we write from each lane.

Lets now set all of this up in the code and get our first shaders working.

Next: [ Vulkan Shaders - Code]({{ site.baseurl }}{% link docs/new_chapter_2/vulkan_shader_code.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}
