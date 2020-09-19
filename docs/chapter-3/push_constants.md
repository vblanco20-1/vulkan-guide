---
layout: default
title: Push Constants
parent: Chapter 3
nav_order: 15
---


## Push Constants
We can render arbitrary meshes now, but there is not a lot we can do with them. We still dont know how to send data from the CPU to shaders outside of the vertex buffer case.
While its the next chapter the one that will go into detail, there is something simpler we can use for now. Push Constants

Push Constants let us send some small data (it has a limited size) from the command buffer itself, which is a very simple and performant way to send small amounts of data to the shader.

Push constants can send data to any shader stage (both vertex and fragment shaders), and are relatively simple to setup compared to DescriptorSets which will be explored later.

To use push constants, you first need to set their size (in each stage) when you create a VkPipelineLayout. Then you can use the command `vkCmdPushConstants` The data will be embedded into the command buffer, and will be accessible from the shader.
We are going to use them for transformation matrices, to be able to move objects around and do proper 3d rendering. To start, we are going to use it to move the triangle we are rendering.

## New PipelineLayout
We are going to need a new PipelineLayout to hold the push constant sizes. For that, we are going to add a new member to vulkanEngine
```cpp
class VulkanEngine {
public:
///other code...
VkPipelineLayout _meshPipelineLayout;
//other code ...

}
```

We are also going to create a struct to hold it in `vk_engine.h`

```cpp
struct MeshPushConstants {
    glm::vec4 data;
    glm::mat4 render_matrix;
};
```

For now, we will just store a single glm::vec4 into it (4 floats) and a mat4 (16 floats). We will then use it to multiply the triangle.
When you create push constant structs, alignement rules apply. You can read the exact alignement rules for them by reading about the glsl `std140` rules. They can be very complicated, so for simplicity, we are going to strictly only put glm::vec4 and glm::mat4 into the push-constants, as this will be much simpler. PushConstants have enough size for common things like multiple transform matrix and a few extra data.


To initialize it, we create it inside the `init_pipelines()` function

```cpp

    //we start from just the default empty pipeline layout info
    VkPipelineLayoutCreateInfo mesh_pipeline_layout_info = vkinit::pipeline_layout_create_info();
    
	//setup push constants
    VkPushConstantRange push_constant;
    //offset 0
    push_constant.offset = 0;
    //size of a MeshPushConstant struct
    push_constant.size = sizeof(MeshPushConstants);
    //for the vertex shader
    push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	mesh_pipeline_layout_info.pPushConstantRanges = &push_constant;
	mesh_pipeline_layout_info.pushConstantRangeCount = 1;

    VK_CHECK(vkCreatePipelineLayout(_device, &mesh_pipeline_layout_info, nullptr, &_meshPipelineLayout));

```

PushConstants are written in ranges. A important reason for that, is that you can have different pushconstants, at different ranges, in different stages.
For example, you can reserve 64 bytes (1 mat4) size on the vertex shader, and then start the pixel shader pushconstant from offset 64. This way you would have different pushconstants on different stages.
Given that getting the ranges right is tricky, in this tutorial we are going to use a more monolythic approach of using the same pushconstants on both vertex and fragment shader. 

Now that we have the layout, we also need to modify the shaders. Lets go to the tri_mesh.vert shader, and add the push-constant to it.

```cpp

#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;

layout (location = 0) out vec3 outColor;

//push constants block
layout( push_constant ) uniform constants
{
vec4 data;
mat4 render_matrix;
} PushConstants;

void main() 
{	
	gl_Position = PushConstants.render_matrix * vec4(vPosition, 1.0f);
	outColor = vColor;
}
```

the push_constants block has to match to the struct we have on cpp side 1 to 1. If it doesnt match exactly, we will get an enormous amount of problems with it.

Now we can modify the way we create the triangle pipeline to hook the new pipeline layout.


in `init_pipelines`, right before creating the mesh pipeline, we are going to hook the new layout


```cpp

    pipelineBuilder._pipelineLayout = _meshPipelineLayout;

    _meshPipeline = pipelineBuilder.build_pipeline(_device, _renderPass);
```

Now our mesh pipeline has the space for the pushconstants, so we can now execute the command to use them.

in `draw()` function, right before the vkCmdDraw call, we are going to prepare and execute the push-constant

```cpp


//just an small offset on X
glm::vec4 offset = { 0.5f,0.0f,0.f,0 };

//build a translation matrix from that
glm::mat4 mesh_matrix = glm::translate(glm::mat4(1.f), offset);

MeshPushConstants constants;
constants.render_matrix = mesh_matrix;

//upload the matrix to the gpu via pushconstants
vkCmdPushConstants(cmd, _meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);

//we can now draw
vkCmdDraw(cmd, _triangleMesh._vertices.size(), 1, 0, 0);

```

In the push-constant call, we need to set the pointer to the data and its size ( a lot like memcpy), and also VK_PIPELINE_STAGE_VERTEX_BIT. This is because our push constant is on the vertex shader, so we need to let Vulkan know that. If we have the same push-constant on both vertex and fragment shader, we would need to have both of those flags there.

If you now run the program, you will see that the triangle is offset in the X axis. If you change the vec4 offset, it will reflect in the shader, and this will be done at runtime, so you can setup some keybinds to move the triangle with WASD



Next up, loading meshes from OBJ


