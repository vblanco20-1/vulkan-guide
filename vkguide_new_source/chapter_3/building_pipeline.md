---
layout: default
title: Setting up render pipeline
parent: "New 3. Graphics Pipelines"
nav_order: 4
---

## VkGraphicsPipelineCreateInfo
Building a graphics pipeline is a far more involved task than building a compute pipeline. With the compute pipeline, we only needed a single shader module and pipeline layout, so there was no need of an abstraction layer. But graphics pipelines contain a considerable amount of options, and without a way to simplify it, creating them can be considerably complicated.

For that reason, we will be creating a PipelineBuilder structure, that keeps track of all those options, and will offer some simpler functions to enable/disable features we want, keeping as much defaulted as possible. A lot of those options are things that we wont be using on the tutorial, so trying to reduce the area will be useful.

Some of the options on a pipeline can be set to be dynamic, which means we will set those options when binding the pipeline and recording draw commands. For example we will put viewport as dynamic, as if we had it "baked in", we would need to create new pipelines if we wanted to change the resolution of our rendering.

Before writing the builder, lets look at what we will need to fill. In the same way creating a compute pipeline required a `VkComputePipelineCreateInfo`, a graphics one is a `VkGraphicsPipelineCreateInfo` structure. 

```cpp
typedef struct VkGraphicsPipelineCreateInfo {
    VkStructureType                                  sType;
    const void*                                      pNext;
    VkPipelineCreateFlags                            flags;
    uint32_t                                         stageCount;
    const VkPipelineShaderStageCreateInfo*           pStages;
    const VkPipelineVertexInputStateCreateInfo*      pVertexInputState;
    const VkPipelineInputAssemblyStateCreateInfo*    pInputAssemblyState;
    const VkPipelineTessellationStateCreateInfo*     pTessellationState;
    const VkPipelineViewportStateCreateInfo*         pViewportState;
    const VkPipelineRasterizationStateCreateInfo*    pRasterizationState;
    const VkPipelineMultisampleStateCreateInfo*      pMultisampleState;
    const VkPipelineDepthStencilStateCreateInfo*     pDepthStencilState;
    const VkPipelineColorBlendStateCreateInfo*       pColorBlendState;
    const VkPipelineDynamicStateCreateInfo*          pDynamicState;
    VkPipelineLayout                                 layout;
    VkRenderPass                                     renderPass;
    uint32_t                                         subpass;
    VkPipeline                                       basePipelineHandle;
    int32_t                                          basePipelineIndex;
} VkGraphicsPipelineCreateInfo;
```

Spec page for graphics pipeline can be found here, which can be used to check things in detail.
* [VkGraphicsPipelineCreateInfo](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap10.html#VkGraphicsPipelineCreateInfo)

`stageCount` and `pStages` contains the ShaderStageCreateInfo that will contain the shader modules for the different stages on the pipeline. We will be sending here our fragment shader and vertex shader.

`VkPipelineVertexInputStateCreateInfo` contains the configuration for vertex attribute input with vertex buffers. If we configure this correctly, our vertex shader will get vertex properties as input in an optimal way. But we will not be using this, as we are just going to send a data array to the shader and index it ourselves, which allows techniques that improve performance and allows more complicated vertex formats that compress data. This is generally known as "vertex pulling", and even if you are doing equivalent thing as the fixed-hardware vertex input, on modern gpus it will perform about the same.

`VkPipelineInputAssemblyStateCreateInfo` contains the configuration for triangle topology. We use this to set the pipeline to draw triangles, points, or lines.

`VkPipelineTessellationStateCreateInfo` is configuration for fixed tesellation. We will not be using this and will leave it as null.

`VkPipelineViewportStateCreateInfo` contains information about the viewport the pixels will be rendered into. This lets you set what region of pixels will the pipeline draw. We will default it, because we will be using dynamic state for this. 

`VkPipelineRasterizationStateCreateInfo` has the information on how exactly do the triangles get rasterized between the vertex shader and the fragment shader. It has options for depth bias (used when rendering shadows), toggling between wireframe and solid rendering, and the configuration for drawing or skipping backfaces.

`VkPipelineMultisampleStateCreateInfo` lets us configure Multi Sample antialiasing. Thats a way of improving the antialiasing of our rendering by rasterizing the fragments more times at triangle edges. We will default it to no antialiasing, but we will look into using it later.

`VkPipelineDepthStencilStateCreateInfo` contains the depth-testing and stencil configuration. 

`VkPipelineColorBlendStateCreateInfo` has the color blending and attachment write information. Its used to make triangles transparent or other blending configurations.

`VkPipelineDynamicStateCreateInfo` configures dynamic state. One great downside that vulkan pipelines have is that their configuration is "hardcoded" at creation. So if we want to do things like toggle depth-testing on and off, we will need 2 pipelines. It even hardcodes viewport, so if we want to change the size of our render targets, we will also need to rebuild all pipelines. Building pipelines is a very expensive operation, and we want to minimize the number of pipelines used as its critical for performance. For that reason, some of the state of a vulkan pipeline can be set as dynamic, and then the configuration option can be modified at runtime when recording commands. What dynamic state is supported by a given gpu depends on gpu vendor, driver version, and other variables. We will be using dynamic state for our viewport and scissor configuration, as almost all GPUs support that one, and it removes the need to hardcode the draw image resolution when building the pipelines.

The VkGraphicsPipelineCreateInfo takes a VkPipelineLayout that is the same one used when building compute pipelines. 

It also takes a VkRenderPass and subpass index. We will not be using that because we use dynamic rendering, so all systems related to VkRenderPass will be completely skipped. Instead, we need to extend the VkGraphicsPipelineCreateInfo with a `VkPipelineRenderingCreateInfo` added into its pNext chain. This structure holds a list of the attachment formats the pipeline will use.


Lets begin writing the builder. All pipeline code will be on vk_pipelines.h/cpp. You can find it on the shared folder if you are checking the chapter code.

```cpp
class PipelineBuilder {
^code pipeline shared/vk_pipelines.h
}
```

The pipeline builder will hold most of the state we need to track of. and an array of color attachment formats and shader stages. The actual CreateInfo structure will be fully filled from the build_pipeline() function. We have a clear() function that will set everything into empty/default properties. The constructor for the pipeline builder will call it, but its useful to have the clear function so we can call it manually when wanted.

Lets write that clear() function first.

^code pipe_clear shared/vk_pipelines.cpp

We will set the .sType of every structure here, and leave everything else as 0. This is using cpp20 initializers, so the parameters we dont write from within the brackets will be defaulted to 0. Most of the Info structures in vulkan are designed so that 0 is valid clear/default option, so this works great here.

Lets begin writing the build_pipeline function. first we will begin by setting some of the Info structures we are missing because they wont be configured.

^code build_pipeline_1 shared/vk_pipelines.cpp

We first fill `VkPipelineViewportStateCreateInfo` with just viewport count and nothing else. With dynamic viewport state we dont need to fill the viewport or stencil options here.

Then we fill `VkPipelineColorBlendStateCreateInfo` with some default options for logic blending (we wont use it), and hook the `VkPipelineColorBlendAttachmentState` for the blending options for a single attachment. We only support rendering to one attachment here, so this is fine. It can be made into an array of `VkPipelineColorBlendAttachmentState` if drawing to multiple attachments is needed.

Lets continue with the function, and begin filling the VkGraphicsPipelineCreateInfo

^code build_pipeline_2 shared/vk_pipelines.cpp

We connect all of the configuration structures we have on the builder, and add _renderInfo into the pNext of the graphics pipeline info itself.

next is setting up dynamic state

^code build_pipeline_3 shared/vk_pipelines.cpp

Setting up dynamic state is just filling a `VkPipelineDynamicStateCreateInfo` with an array of VkDynamicState enums. We will use these 2 for now.

This is all we needed for the pipeline, so we can finally call the create function.

^code build_pipeline_4 shared/vk_pipelines.cpp

And thats it with the main creation function. We now need to actually set the options properly, as right now the entire pipeline is essentially null, which will error as-is due to missing options.

^code set_shaders shared/vk_pipelines.cpp

We begin by adding a function to set the vertex and fragment shaders. We add them into the _shaderStages array with the proper info creation, which we already had from building the compute pipeline.

Next we add a function to set input topology

^code set_topo shared/vk_pipelines.cpp

VkPrimitiveTopology has the options for VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_PRIMITIVE_TOPOLOGY_POINT_LIST, and so on. PrimitiveRestart is used for triangle strips and line strips, but we dont use it.

The rasterizer state is a big one so we will split it on a few options.

^code set_poly shared/vk_pipelines.cpp

We need to have lineWidth as 1.f as default, then we set the polygon mode, which controls wireframe vs solid rendering and point rendering.

^code set_cull shared/vk_pipelines.cpp

Cull mode will set the front face and the cull mode for backface culling. 

Next is setting the multisample state. We will default the structure to multisampling disabled. Later we can add other functions for enabling different multisampling levels for antialiasing

^code set_multisample shared/vk_pipelines.cpp

Next we will add a function for blending mode

^code set_noblend shared/vk_pipelines.cpp

We will have our disable_blending() function that sets blendEnable to false but sets the correct write mask. We will add functions for more blending modes later. We need to setup a proper colorWriteMask here so that our pixel output will write to the attachment correctly.

Now we hook our formats, lets add the functions for both depth testing and color attachment.

^code set_formats shared/vk_pipelines.cpp

On the color attachment, the pipeline needs it by pointer because it wants an array of color attachments. This is useful for things like deferred rendering where you draw to multiple images at once, but we dont need this yet so we can default it to just 1 color format.

The last one we need is a function to disable the depth testing logic.

^code depth_disable shared/vk_pipelines.cpp

With all the basic features for the pipeline builder filled, we can now draw a triangle.
For our triangle, we are going to use hardcoded vertex positions in the vertex shader, and the output will be a pure color. 

These are the shaders: 

colored_triangle.vert
^code all shaders/colored_triangle.vert

colored_triangle.frag
^code all shaders/colored_triangle.frag



In our vertex shader, we have a hardcoded array of positions, and we index into it from `gl_VertexIndex`. This works in a similar way to LocalThreadID on compute shaders worked. For every invocation of the vertex shader, this will be a different index, and we can use it to process out vertex, which will write into the fixed function gl_Position variable. As the array is only of lenght 3, if we tried to render more than 3 vertices (1 triangle) this will error.

In our fragment shader, we will declare an output at layout = 0 (this connects to the render attachments of the render pass), and we have a simple hardcoded red output.

Lets now create the pipeline and layout we need to draw this triangle.
We are adding new shader files, so make sure you rebuild the CMake project and build the Shaders target.

On VulkanEngine class, we will add a `init_triangle_pipeline()` function, and a couple of members to hold the pipeline and its layout

```cpp
VkPipelineLayout _trianglePipelineLayout;
VkPipeline _trianglePipeline;

void init_triangle_pipeline();
```

We will call this `init_triangle_pipeline()` from `init_pipelines()` function. 

Lets write that function We will start by loading the 2 shaders into VkShaderModules, like we did with the compute shader, but this time more shaders.

^code triangle_shaders chapter-3/vk_engine.cpp

We also create the pipeline layout. Unlike with the compute shader before, this time we have no push constants and no descriptor bindings on here, so its really just a completely empty layout.

Now we create the pipeline, using the Pipeline Builder created before.

^code triangle_pip_1 chapter-3/vk_engine.cpp

With the pipeline built, we can draw our triangle as part of the command buffer we create every frame.

The compute shader we run for the background needed to draw into GENERAL image layout, but when doing geometry rendering, we need to use COLOR_ATTACHMENT_OPTIMAL. It is possible to draw into GENERAL layout with graphics pipelines, but its lower performance and the validation layers will complain. We will create a new function, `draw_geometry()`, to hold these graphics commands. Lets update the draw loop first.

^code draw_barriers chapter-3/vk_engine.cpp

Now fill the draw_geometry function

^code draw_geo chapter-3/vk_engine.cpp

To draw our triangle we need to begin a renderpass with cmdBeginRendering. This is the same we were doing for imgui last chapter, but this time we are pointing it into our _drawImage instead of the swapchain image.

We do a CmdBindPipeline, but instead of using the BIND_POINT_COMPUTE, we now use `VK_PIPELINE_BIND_POINT_GRAPHICS`. Then, we have to set our viewport and scissor. This is required before we left them undefined when creating the pipeline as we were using dynamic pipeline state. With that set, we can do a vkCmdDraw() to draw the triangle. With that done, we can finish the render pass to end our drawing.

If you run the program at this point, you should see a triangle being rendered on top of the compute based background

![triangle]({{site.baseurl}}/diagrams/ColorTri2.png)

^nextlink

{% include comments.html term="Vkguide 2 Beta Comments" %}