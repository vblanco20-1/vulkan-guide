---
layout: default
title: Push Constants and new shaders
parent:  "New 2. Drawing with Compute"
nav_order: 9
---



We have a way to run compute shaders to display, and a way to add debug-UI to the engine. Lets use that to send data to the shaders through the UI, and have an interactive thing.

we will be using PushConstants to send data to the shader. PushConstants are a feature unique to vulkan that allows for some small amount of data to be sent to the GPU. Keeping this data small is important, as most drivers will have a fast-path if the data is below some bytes (consult gpu vendor documentation). Its main use case is to send some per-object indexes or a couple matrices that change for every object. If you have data that is bigger than a handful of floats or ints, you should be using other systems that we will show next chapter.

Push constants are configured when you create a pipeline layout. To keep things simple and not have to change too much code, we are going to default our pushconstants for compute effects to 4 vec4 vectors. 16 floats will be enough for playing around with the shaders.

In the project shader folder, there are multiple compute shaders you will be able to swap around. We will focus on a simple color gradient one, but you can try the other demo shaders the project comes with.

As all of our compute shaders will share the same layout, we are going to also add a drop-down to the UI to select which pipeline to use. This way we can flip between the different compute shaders at runtime to test them.

The shader we are going to use to demonstrate pushconstants is this. It will blend between 2 colors by Y coordinate, making a vertical gradient.

Its found under gradient_color.comp in the shader folder

```c
#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout(rgba16f,set = 0, binding = 0) uniform image2D image;

//push constants block
layout( push_constant ) uniform constants
{
 vec4 data1;
 vec4 data2;
 vec4 data3;
 vec4 data4;
} PushConstants;

void main() 
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

	ivec2 size = imageSize(image);

    vec4 topColor = PushConstants.data1;
    vec4 bottomColor = PushConstants.data2;

    if(texelCoord.x < size.x && texelCoord.y < size.y)
    {
        float blend = float(texelCoord.y)/(size.y); 
    
        imageStore(image, texelCoord, mix(topColor,bottomColor, vertical));
    }
}
```

Its mostly the same as the gradient shader we had from last article. We have added a push constant block containing 4 vec4s, and we are loading top and bottom color from it. data3 and data4 are not used, but we have them in there to avoid the validation layers complaining that we have a push-constants range larger than we have in the shader.

We now need to change the pipeline layout creation to configure the pushconstants range. Lets first create a structure that mirrors those pushconstants directly into vk_engine.h.

```cpp
struct ComputePushConstants {
	glm::vec4 data1;
	glm::vec4 data2;
	glm::vec4 data3;
	glm::vec4 data4;
};
```

To set the push constant ranges, we need to change the code that creates the pipeline layout at the start of init_pipelines. the new version looks like this

^code comp_pipeline_pc chapter-1/vk_engine.cpp

We need to add a VkPushConstantRange to the pipeline layout info. A PushConstantRange holds an offset, which we will keep at 0, and then a size plus the stage flags. For size we will use our cpp version of the structure, as that matches. And for stage flags its going to be compute because its the only stage we have right now.

After that, just change the shader to be compiled to be the new one

```cpp
VkShaderModule computeDrawShader;
if (!vkutil::load_shader_module("../../shaders/gradient_color.comp.spv", _device, &computeDrawShader))
{
	std::cout << "Error when building the colored mesh shader" << std::endl;
}
```

This is all we need to add pushconstants to a shader. lets now use them from the render loop

^code draw_pc chapter-1/vk_engine.cpp

To update pushconstants, we call VkCmdPushConstants. it requires the pipeline layout, an offset for the data to be written to (we use just offset 0), and the size of the data + the pointer to copy. It also requires the shader stage flags as one can update pushconstants for different stages on different commands.

This is all. If you run the program at this moment, you will see a gradient of red to blue.

## Imgui editable parameters

We are hardcoding the colors right now, but we can do better than that by adding a small window using imgui with those as editable colors.

We want to store an array of compute pipelines we will be drawing, alongside one of those ComputePushConstant structs for their value. This way we will be able to switch between different compute shaders.

Lets add a struct to vk_engine.h with that

```cpp
struct ComputeEffect {
    const char* name;

	VkPipeline pipeline;
	VkPipelineLayout layout;

	ComputePushConstants data;
};
```

Now lets add an array of them to the VulkanEngine class, with an integer to hold the index to use when drawing

```cpp
std::vector<ComputeEffect> backgroundEffects;
int currentBackgroundEffect{0};
```

Lets change the code on init_pipelines to create 2 of these effects. One will be the gradient we just did, the other is a pretty star-night sky shader.

The sky shader is too complicated to explain here, but feel free to check the code on sky.comp. Its taken from shadertoy and adapted slightly to run as a compute shader in here. data1 of the pushconstant will contain sky color x/y/z, and then w can be used to control the amount of stars.

With 2 shaders, we need to create 2 different VkShaderModule.

^code comp_pipeline_multi chapter-1/vk_engine.cpp

We have changed the pipelines function. We keep the pipeline layout from before, but now we create 2 different pipelines, and store them into the ComputeEffect vector. We also give the effects some default data.

Now we can add the imgui debug window for this. This goes on run() function. We will replace the demo effect call with the new ui logic

^code imgui_bk chapter-1/vk_engine.cpp

First we grab the selected compute effect by indexing into the array. Then we use Imgui::Text to display the effect name, and then we have int slider and float4 input for the edits.

Last we need to do is to change the render loop to select the shader selected with its data

^code draw_multi chapter-1/vk_engine.cpp

Not much of a change, we are just hooking into the compute effect array and uploading the pushconstants from there.

Try to run the app now, and you will see a debug window where it lets you select the shader, and edit its parameters.

