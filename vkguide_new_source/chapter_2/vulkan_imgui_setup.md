---
layout: default
title: Setting up IMGUI
parent:  "New 2. Drawing with Compute"
nav_order: 7
---

As the engine grows, we need a way to have some interactuable interface we can play with to add sliders, buttons, and display data. 

To do that, we are going to add the library "dear Imgui" to the project. This is a library that makes it very easy to add windows and interfaces with sliders, buttons, editable text.. Without having to setup UI files or deal with a complex system. 


## Immediate GPU commands
Imgui will require us to run some commands outside of the normal draw loop. This is going to be something we will need many times on the engine for different uses. We are going to implement an `immediate_submit` function, which uses a fence and a different command buffer from the one we use on draws to send some commands to the GPU without syncronizing with swapchain or with rendering logic.

Lets add those structures into the VulkanEngine class

```cpp
class VulkanEngine{
public:
    // immediate submit structures
    VkFence _immFence;
    VkCommandBuffer _immCommandBuffer;
    VkCommandPool _immCommandPool;

	
	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

private:
	void init_imgui();
}
```
We have a fence and a command buffer with its pool. The immediate_submit function takes an std function as callback to use with lambdas. 
Add the init_imgui() function too, and add it at the end of the init() chain of function calls. Leave it empty for now.

we need to create those syncronization structures for immediate submit, so lets go into init_commands() function and hook the command part.

```cpp
void VulkanEngine::init_commands()
{
^code imm_cmd chapter-2/vk_engine.cpp
}
```

This is the same we were doing with the per-frame commands, but this time we are directly putting it into the deletion queue for cleanup.

Now we need to create the fence, which we are going to add to init_sync_structures(). Add it to the end

```cpp
void VulkanEngine::init_sync_structures()
{
^code imm_fence chapter-2/vk_engine.cpp
}
```

We will use the same fenceCreateInfo we were using for the per-frame fences. Same as with the commands, we are directly adding its destroy function to the deletion queue too.

Now implement the immediate_submit function

^code imm_submit chapter-2/vk_engine.cpp

Note how this function is very similar and almost the same as the way we are executing commands on the gpu. 

Its close to the same thing, except we are not syncronizing the submit with the swapchain. 

We will be using this function for data uploads and other "instant" operations outside of the render loop. One way to improve it would be to run it on a different queue than the graphics queue, and that way we could overlap the execution from this with the main render loop. 


## IMGUI Setup
Lets now go with the imgui initialization.

We need to add some includes first to vk_engine.cpp

```cpp
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"
```

Its the main imgui header, and then the implementation headers for the SDL 2 and the vulkan backends.

Now to the initialization function

^code imgui_init chapter-2/vk_engine.cpp

This code is adapted from the imgui demos. We first need to create some structures that imgui wants, like its own descriptor pool. The descriptor pool here is storing data for 1000 of a lot of different types of descriptors, so its a bit overkill. It wont be a problem, just slightly less efficient space-wise.

We then call `CreateContext()` , `ImGui_ImplSDL2_InitForVulkan`, and `ImGui_ImplVulkan_Init`. These functions will initialize the different parts of imgui we need. 
On the vulkan one, we need to hook a few things, like our device, instance, queue.

One important one is that we need to set UseDynamicRendering to true, and set ColorAttachmentFormat to our swapchain format, this is because we wont be using vulkan render-passes but Dynamic Rendering instead. And unlike with the compute shader, we are going to draw dear imgui directly into the swapchain. 

After calling `ImGui_ImplVulkan_Init`, we need to do an immediate submit to upload the font texture. Once that is executed, we call DestroyFontUploadObjects so that imgui deletes those temporal structures. 
Last, we add cleanup code into the destruction queue.

# Imgui render loop
Imgui is initialized now, but we need to hook it into the rendering loop.

First thing we have to do is to add its code into the run() function

```cpp
//Handle events on queue
while (SDL_PollEvent(&e) != 0)
{
    //close the window when user alt-f4s or clicks the X button			
    if (e.type == SDL_QUIT) bQuit = true;
    
    //send SDL event to imgui for handling
    ImGui_ImplSDL2_ProcessEvent(&e);
}

// imgui new frame
ImGui_ImplVulkan_NewFrame();
ImGui_ImplSDL2_NewFrame(_window);
ImGui::NewFrame();

//some imgui UI to test
ImGui::ShowDemoWindow();

//make imgui calculate internal draw structures
ImGui::Render();

//our draw function
draw();
```

We have to pass the SDL events into imgui for handling from our pollEvent loop.
After that, we need to call the 3 functions for a new frame on imgui. 
Once that is done, we can now do our UI commands. We are going to leave it on the demo window for now. 
When we call `ImGui::Render()`, that calculates the vertices/draws/etc that imgui requires to draw the frame, but it does not do any drawing on its own. To draw it we will continue it from within our draw() function.

# Dynamic Rendering
Imgui will draw using actual gpu draws with meshes and shaders, it will not do a compute draw like we are doing at te moment.
To draw geometry, it needs to be done withing a renderpass. But we are not using renderpasses as we will use dynamic rendering, a vulkan 1.3 feature. Instead of calling VkCmdBeginRenderpass, and giving it a VkRenderPass object, we call VkBeginRendering, with a VkRenderingInfo that contains the settings needed for the images to draw into.

The VkRenderingInfo points into multiple VkRenderingAttachmentInfo for our target images to draw into, so lets begin writing that one into the initializers.

^code color_info shared/vk_initializers.cpp

For our attachment info, we will have clear value as an optional pointer, that way we can either do a clear or skip it and load the image.

We need to hook imageview and layout as usual with all these rendering commands. The important part is the loadOP and storeOP. This controls what happens to the render target in this attachment when its used within a renderpass (both dynamic one and class renderpass). For load options, we have LOAD, which will keep the data in that image. Clear which will set it to our clear value at the start, and dont-care where we plan to replace every pixel and thus the gpu can skip loading it from memory. 

For our store op, we are going to use store hardcoded, as we will want our draw commands to be saved.

With the attachment info done, we can make the VkRenderingInfo.

^code color_info shared/vk_initializers.cpp

We are going to take a render extent to setup a rectangle of pixels to draw, and we will send a color attachment and a depth attachment. We dont need the depth attachment right now, thats for later.

We now go change the render loop so that we can call the imgui draw commands

^code imgui_draw chapter-2/vk_engine.cpp

That copy_image command is the same as before, we are replacing the later commands until the end of the VkEndCommandBuffer call.

Before, we were transitioning the swapchain image from transfer layout into present layout, but now we are going to change it into `VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL` instead. This is the layout you should be using whenever you are calling rendering commands like in here.

After that, we build the VkRenderingInfo and send a single color attachment to it. That color attachment will point to the swapchain image we target.

Now that we have a VkRenderInfo, we can call `vkCmdBeginRendering`, this begins a render pass, and we can now execute draw commands. We call into the imgui vulkan backend with our command buffer, which will make imgui record its draw commands into the buffer. Once that is done, we can call `vkCmdEndRendering` to end the render pass. 

After that, we transition the swapchain image from attachment-optimal into present mode, and can finally end the command buffer.

If you run the application at this point, you will have the imgui demo window you can play around with.
 