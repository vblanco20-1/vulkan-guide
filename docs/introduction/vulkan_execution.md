---
layout: default
title: Vulkan Usage
nav_order: 2
parent: introduction
---

# Vulkan main objects and their use

- VkInstance : The Vulkan driver
- VkPhysicalDevice : A GPU. mostly used to query physical gpu details
- VkDevice : A "virtual" GPU, which is where you actually execute things on.
- VkBuffer : A chunk of GPU visible memory.
- VkImage : A texture you can wrote and read from. 
- VkPipeline : Holds the state of the gpu needed to perform a draw (shaders, rasterization options)
- VkRenderPass : Holds information about the images you are rendering into. All rendering has to be done inside a renderpass.
- VkFrameBuffer : Holds the target images for a renderpass.
- VkCommandBuffer : Encodes GPU commands. All execution that is performed on the GPU itself (not in the driver) has to be encoded in a VkCommandBuffer
- VkQueue : Execution "port" for commands. GPUs will have a set of queues with different properties (some allow only graphics commands, other only allow mememory commands.. etc). Command buffers are executed by submitting them into a queue, which will copy the rendering commands to the actual GPU for execution.
- VkDescriptorSet : Holds the binding information to connect shader inputs to data such as buffers and images. Think of it as a set of gpu-side pointers that you bind at once.
- VkSwapchain : Holds the images for the actual screen. It allows you to render things into a visible window.

- VkSemaphore : syncronizes GPU to Gpu execution of commands (used for executing multiple submits one after other or similar)
- VkFence : Syncronizes Gpu to CPU execution of commands (used to know if an execution has finished)

# High level Vulkan application flow

## Engine initialization
First, everything is initialized. To initialize Vulkan, you start by creating a VkInstance. From the VkInstance, you query the VkPhysicalDevice available in the machine (there is a VkPhysicalDevice for dedicated GPU, and another for integrated GPU). Once you query the limits and features of the available VkPhysicalDevice, you create a VkDevice from it.
Once you have a VkDevice, you create some VkQueues that will allow you to execute commands on it, and initialize the VkSwapchain. Alongside the VkQueues, you create some VkCommandPool that will allow you to allocate VkCommandBuffer from them. 

## Asset initialization
Once the core structures are initialized, you initialize the resources you need for whatever you will be rendering. The materials are loaded, and you create a set of VkPipeline objects for the shader combinations and parameters needed to render the materials. For the meshes, you upload their vertex data into VkBuffers, and you upload their textures into VkImages, making sure that the images are in "readable" layout. You also create VkRenderPass for whatever your main rendering passes you have. For example one for the main rendering, and another for a shadow pass. On a real engine, you might want to do this from a background thread.

## Render Loop
Now that everything is ready for the rendering, you first ask the VkSwapchain for an image to render to, and start allocating a VkCommandBuffer from the pools (or reusing an already allocated command buffer that finished execution), and "starting" it. 
Next, you start rendering by starting a VkRenderPass (that renders to the image requested from swapchain), and then a loop where you bind a VkPipeline, bind some VkDescriptorSets (for the shader parameters), Bind the vertex buffers, and then execute a draw.
Once you are finished with the draws for a pass, you end the VkRenderPass . If there is nothing more to do, you also end the VkCommandBuffer.
Finally, you submit the command buffer into the queue for rendering. This will start all rendering on the gpu. 
If you want to display the result of the rendering, you then Present the image you have rendered to to the screen, with a semaphore that controls that the rendering has finished before the present is done.

Pseudocode of a render-loop in Vulkan:

```cpp
//ask the swapchain for the index of the image we can render into
int image_index = request_image(mySwapchain);

//create a new command buffer
VkCommandBuffer cmd = allocate_command_buffer();

//initialize the command buffer
VkBeginCommandBuffer(cmd, ... );

//start a new renderpass with the image index from swapchain as target to render into.
VkCmdBeginRenderPass(cmd, main_render_pass, framebuffers[image_index] );

//rendering all objects
for(object in PassObjects){

    //bind the shaders and configuration used to render the object
    VkCmdBindPipeline(cmd, object.pipeline);
    
    //bind the vertex and index buffers for rendering the object
    VkCmdBindVertexBuffer(cmd, object.VertexBuffer,...);
    VkCmdBindIndexBuffer(cmd, object.IndexBuffer,...);

    //bind the descriptor sets for the object (shader inputs)
    VkCmdBindDescriptorSets(cmd, object.textureDescriptorSet);
    VkCmdBindDescriptorSets(cmd, object.parametersDescriptorSet);

    //execute draw
    VkCmdDraw(cmd,...);
}

//finalize the render pass and command buffer
VkCmdEndRenderPass(cmd);
VkEndCommandBuffer(cmd);


//submit the command buffer to begin execution on GPU
VkQueueSubmit(graphicsQueue, cmd, ...);

// display the image we just rendered on the screen
VkQueuePresent(graphicsQueue, renderSemaphore);
```

Next: [Project files and libraries]({{ site.baseurl }}{% link docs/introduction/project_libs.md %})