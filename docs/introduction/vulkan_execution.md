---
layout: default
title: Vulkan Usage
nav_order: 2
parent: introduction
---

# Vulkan main objects and their use

- `VkInstance` : The Vulkan context, used to access drivers.
- `VkPhysicalDevice` : A GPU. Used to query physical GPU details, like features, capabilities, memory size, etc.
- `VkDevice` : The "logical" GPU context that you actually execute things on.
- `VkBuffer` : A chunk of GPU visible memory.
- `VkImage` : A texture you can write to and read from. 
- `VkPipeline` : Holds the state of the gpu needed to draw. For example: shaders, rasterization options, depth settings.
- `VkRenderPass` : Holds information about the images you are rendering into. All drawing commands have to be done inside a renderpass. Only used in legacy vkguide
- `VkFrameBuffer` : Holds the target images for a renderpass. Only used in legacy vkguide
- `VkCommandBuffer` : Encodes GPU commands. All execution that is performed on the GPU itself (not in the driver) has to be encoded in a `VkCommandBuffer`.
- `VkQueue` : Execution "port" for commands. GPUs will have a set of queues with different properties. Some allow only graphics commands, others only allow memory commands, etc. Command buffers are executed by submitting them into a queue, which will copy the rendering commands onto the GPU for execution.
- `VkDescriptorSet` : Holds the binding information that connects shader inputs to data such as `VkBuffer` resources and `VkImage` textures. Think of it as a set of gpu-side pointers that you bind once.
- `VkSwapchainKHR` : Holds the images for the screen. It allows you to render things into a visible window. The `KHR` suffix shows that it comes from an extension, which in this case is `VK_KHR_swapchain`
- `VkSemaphore` : Synchronizes GPU to GPU execution of commands. Used for syncing multiple command buffer submissions one after other.
- `VkFence` : Synchronizes GPU to CPU execution of commands. Used to know if a command buffer has finished being executed on the GPU.

# High level Vulkan application flow

## Engine initialization
First, everything is initialized. To initialize Vulkan, you start by creating a `VkInstance`. From the `VkInstance`, you query the list of `VkPhysicalDevice` handles available in the machine. For example, if the computer had both a dedicated GPU and integrated graphics, there would be a `VkPhysicalDevice` for each. After querying the limits and features of the available `VkPhysicalDevice` handles, you create a `VkDevice` from it.
With a `VkDevice`, you then get `VkQueue` handles from it, allowing you to execute commands. Then you initialize the `VkSwapchainKHR`. Alongside the `VkQueue` handles, you create `VkCommandPool` objects that enable you to allocate command buffers from them. 

## Asset initialization
Once the core structures are initialized, you initialize the resources you need for whatever you will be rendering. The materials are loaded, and you create a set of `VkPipeline` objects for the shader combinations and parameters needed to render the materials. For the meshes, you upload their vertex data into `VkBuffer` resources, and you upload their textures into `VkImage` resources, making sure that the images are in "readable" layout. You also create `VkRenderPass` objects for whatever your main rendering passes you have. For example, there may be a `VkRenderPass` for the main rendering, and another for a shadow pass. On a real engine, much of this can be parallelized and done in background threads, especially since pipeline creation can be quite expensive.

## Render Loop
Now that everything is ready for rendering, you first ask the `VkSwapchainKHR` for an image to render to. Then you allocate a `VkCommandBuffer` from a `VkCommandBufferPool` or reuse an already allocated command buffer that has finished execution, and "start" the command buffer, which allows you to write commands into it.
Next, you begin rendering by starting a `VkRenderPass`, this can be done with a normal `VkRenderPass`, or using dynamic rendering. The render pass specifies that you are rendering to the image requested from swapchain. Then create a loop where you bind a `VkPipeline`, bind some `VkDescriptorSet` resources (for the shader parameters), bind the vertex buffers, and then execute a draw call.
Once you are finished with the drawing for a pass, you end the `VkRenderPass`. If there is nothing more to render, you end the `VkCommandBuffer`.
Finally, you submit the command buffer into the queue for rendering. This will begin execution of the commands in the command buffer on the gpu.
If you want to display the result of the rendering, you "present" the image you have rendered to to the screen. Because the execution may not have finished yet, you use a semaphore to make the presentation of the image to the screen wait until rendering is finished.

Pseudocode of a render-loop in Vulkan:

```cpp
// Ask the swapchain for the index of the swapchain image we can render onto
int image_index = request_image(mySwapchain);

// Create a new command buffer
VkCommandBuffer cmd = allocate_command_buffer();

// Initialize the command buffer
vkBeginCommandBuffer(cmd, ... );

// Start a new renderpass with the image index from swapchain as target to render onto
// Each framebuffer refers to a image in the swapchain
vkCmdBeginRenderPass(cmd, main_render_pass, framebuffers[image_index] );

// Rendering all objects
for(object in PassObjects){

    // Bind the shaders and configuration used to render the object
    vkCmdBindPipeline(cmd, object.pipeline);
    
    // Bind the vertex and index buffers for rendering the object
    vkCmdBindVertexBuffers(cmd, object.VertexBuffer,...);
    vkCmdBindIndexBuffer(cmd, object.IndexBuffer,...);

    // Bind the descriptor sets for the object (shader inputs)
    vkCmdBindDescriptorSets(cmd, object.textureDescriptorSet);
    vkCmdBindDescriptorSets(cmd, object.parametersDescriptorSet);

    // Execute drawing
    vkCmdDraw(cmd,...);
}

// Finalize the render pass and command buffer
vkCmdEndRenderPass(cmd);
vkEndCommandBuffer(cmd);


// Submit the command buffer to begin execution on GPU
vkQueueSubmit(graphicsQueue, cmd, ...);

// Display the image we just rendered on the screen
// renderSemaphore makes sure the image isn't presented until `cmd` is finished executing
vkQueuePresent(graphicsQueue, renderSemaphore);
```

Next: [Project files and libraries]({{ site.baseurl }}{% link docs/introduction/project_libs.md %})


{% include comments.html term="Introduction Comments" %}