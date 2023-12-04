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
	VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_immCommandPool));

	// allocate the default command buffer that we will use for rendering
	VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_immCommandPool, 1);

	VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_immCommandBuffer));

	_mainDeletionQueue.push_function([=]() { 
	vkDestroyCommandPool(_device, _immCommandPool, nullptr);
	});

}
```

This is the same we were doing with the per-frame commands, but this time we are directly putting it into the deletion queue for cleanup.

Now we need to create the fence, which we are going to add to init_sync_structures(). Add it to the end

```cpp
void VulkanEngine::init_sync_structures()
{
	VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_immFence));
	_mainDeletionQueue.push_function([=]() { vkDestroyFence(_device, _immFence, nullptr); });
}
```

We will use the same fenceCreateInfo we were using for the per-frame fences. Same as with the commands, we are directly adding its destroy function to the deletion queue too.

Now implement the immediate_submit function

<!-- codegen from tag imm_submit on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
void VulkanEngine::immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function)
{
	VK_CHECK(vkResetFences(_device, 1, &_immFence));
	VK_CHECK(vkResetCommandBuffer(_immCommandBuffer, 0));

	VkCommandBuffer cmd = _immCommandBuffer;

	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	function(cmd);

	VK_CHECK(vkEndCommandBuffer(cmd));

	VkCommandBufferSubmitInfo cmdinfo = vkinit::command_buffer_submit_info(cmd);
	VkSubmitInfo2 submit = vkinit::submit_info(&cmdinfo, nullptr, nullptr);

	// submit command buffer to the queue and execute it.
	//  _renderFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit2(_graphicsQueue, 1, &submit, _immFence));

	VK_CHECK(vkWaitForFences(_device, 1, &_immFence, true, 9999999999));
}
```

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

<!-- codegen from tag imgui_init on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
void VulkanEngine::init_imgui()
{
	// 1: create descriptor pool for IMGUI
	//  the size of the pool is very oversize, but it's copied from imgui demo
	//  itself.
	VkDescriptorPoolSize pool_sizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	VkDescriptorPool imguiPool;
	VK_CHECK(vkCreateDescriptorPool(_device, &pool_info, nullptr, &imguiPool));

	// 2: initialize imgui library

	// this initializes the core structures of imgui
	ImGui::CreateContext();

	// this initializes imgui for SDL
	ImGui_ImplSDL2_InitForVulkan(_window);

	// this initializes imgui for Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = _instance;
	init_info.PhysicalDevice = _chosenGPU;
	init_info.Device = _device;
	init_info.Queue = _graphicsQueue;
	init_info.DescriptorPool = imguiPool;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	init_info.UseDynamicRendering = true;
	init_info.ColorAttachmentFormat = _swapchainImageFormat;

	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	ImGui_ImplVulkan_Init(&init_info, VK_NULL_HANDLE);

	// execute a gpu command to upload imgui font textures
	immediate_submit([&](VkCommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(cmd); });

	// clear font textures from cpu data
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	// add the destroy the imgui created structures
	_mainDeletionQueue.push_function([=]() {
		vkDestroyDescriptorPool(_device, imguiPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
	});
}
```

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

<!-- codegen from tag color_info on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_initializers.cpp --> 
```cpp
VkRenderingAttachmentInfo vkinit::attachment_info(
    VkImageView view, VkClearValue* clear ,VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/)
{
    VkRenderingAttachmentInfo colorAttachment {};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.pNext = nullptr;

    colorAttachment.imageView = view;
    colorAttachment.imageLayout = layout;
    colorAttachment.loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    if (clear) {
        colorAttachment.clearValue = *clear;
    }

    return colorAttachment;
}
```

For our attachment info, we will have clear value as an optional pointer, that way we can either do a clear or skip it and load the image.

We need to hook imageview and layout as usual with all these rendering commands. The important part is the loadOP and storeOP. This controls what happens to the render target in this attachment when its used within a renderpass (both dynamic one and class renderpass). For load options, we have LOAD, which will keep the data in that image. Clear which will set it to our clear value at the start, and dont-care where we plan to replace every pixel and thus the gpu can skip loading it from memory. 

For our store op, we are going to use store hardcoded, as we will want our draw commands to be saved.

With the attachment info done, we can make the VkRenderingInfo.

<!-- codegen from tag color_info on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_initializers.cpp --> 
```cpp
VkRenderingAttachmentInfo vkinit::attachment_info(
    VkImageView view, VkClearValue* clear ,VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/)
{
    VkRenderingAttachmentInfo colorAttachment {};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.pNext = nullptr;

    colorAttachment.imageView = view;
    colorAttachment.imageLayout = layout;
    colorAttachment.loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    if (clear) {
        colorAttachment.clearValue = *clear;
    }

    return colorAttachment;
}
```

We are going to take a render extent to setup a rectangle of pixels to draw, and we will send a color attachment and a depth attachment. We dont need the depth attachment right now, thats for later.

We now go change the render loop so that we can call the imgui draw commands. We will begin by adding a new function draw_imgui() to the VulkanEngine class, and putting our imgui drawing code inside it.

<!-- codegen from tag imgui_draw_fn on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
void VulkanEngine::draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView)
{
	VkRenderingAttachmentInfo colorAttachment = vkinit::attachment_info(targetImageView, nullptr, VK_IMAGE_LAYOUT_GENERAL);
	VkRenderingInfo renderInfo = vkinit::rendering_info(_windowExtent, &colorAttachment, nullptr);

	vkCmdBeginRendering(cmd, &renderInfo);

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

	vkCmdEndRendering(cmd);
}
```

Then we need to call it from our draw() function.

<!-- codegen from tag imgui_draw on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
	// execute a copy from the draw image into the swapchain
	vkutil::copy_image_to_image(cmd, _drawImage.image, _swapchainImages[swapchainImageIndex], extent);

	// set swapchain image layout to Attachment Optimal so we can draw it
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	//draw imgui into the swapchain image
	draw_imgui(cmd,  _swapchainImageViews[swapchainImageIndex]);

	// set swapchain image layout to Present so we can draw it
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	//finalize the command buffer (we can no longer add commands, but it can now be executed)
	VK_CHECK(vkEndCommandBuffer(cmd));
```

That copy_image command is the same as before, we are replacing the later commands until the end of the VkEndCommandBuffer call.

Before, we were transitioning the swapchain image from transfer layout into present layout, but now we are going to change it into `VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL` instead. This is the layout you should be using whenever you are calling rendering commands like in here.

After that, we build the VkRenderingInfo and send a single color attachment to it. That color attachment will point to the swapchain image we target.

Now that we have a VkRenderInfo, we can call `vkCmdBeginRendering`, this begins a render pass, and we can now execute draw commands. We call into the imgui vulkan backend with our command buffer, which will make imgui record its draw commands into the buffer. Once that is done, we can call `vkCmdEndRendering` to end the render pass. 

After that, we transition the swapchain image from attachment-optimal into present mode, and can finally end the command buffer.

If you run the application at this point, you will have the imgui demo window you can play around with.

Lets continue and hook our new debug UI to the shaders.

Next: [ Push Constants and new shaders]({{ site.baseurl }}{% link docs/new_vkguide/chapter_2/vulkan_pushconstants.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}
 
