---
layout: default
title: Mainloop Code
parent:  "1. Initializing Vulkan"
nav_order: 41
---

The first thing we need to do is to add the syncronization structures that we are going to need into our FrameData structure. We add the new members into the struct.

```cpp
struct FrameData {
	VkSemaphore _swapchainSemaphore, _renderSemaphore;
	VkFence _renderFence;
};
```

We are going to need 2 semaphores and the main render fence. Let us begin creating them.

The `_swapchainSemaphore` is going to be used so that our render commands wait on the swapchain image request.
The `_renderSemaphore` will be used to control presenting the image to the OS once the drawing finishes
`_renderFence` will lets us wait for the draw commands of a given frame to be finished.

Lets initialize them. Check the functions to make a VkFenceCreateInfo and a VkSemaphoreCreateInfo on our vk_initializers.cpp code.

<!-- codegen from tag init_sync on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_initializers.cpp --> 
```cpp
VkFenceCreateInfo vkinit::fence_create_info(VkFenceCreateFlags flags /*= 0*/)
{
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = nullptr;

    info.flags = flags;

    return info;
}

VkSemaphoreCreateInfo vkinit::semaphore_create_info(VkSemaphoreCreateFlags flags /*= 0*/)
{
    VkSemaphoreCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
}
```

Both of these structures are pretty simple and need almost no options other than to give them some flags. For more info on the structures, here are the spec links [VkFenceCreateInfo](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap7.html#VkFenceCreateInfo), [VkSemaphoreCreateInfo](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap7.html#VkSemaphoreCreateInfo)

Lets write the actual creation now. 

<!-- codegen from tag init_sync on file E:\ProgrammingProjects\vulkan-guide-2\chapter-1/vk_engine.cpp --> 
```cpp
void VulkanEngine::init_sync_structures()
{
	//create syncronization structures
	//one fence to control when the gpu has finished rendering the frame,
	//and 2 semaphores to syncronize rendering with swapchain
	//we want the fence to start signalled so we can wait on it on the first frame
	VkFenceCreateInfo fenceCreateInfo = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphore_create_info();

	for (int i = 0; i < FRAME_OVERLAP; i++) {
		VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_frames[i]._renderFence));

		VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._swapchainSemaphore));
		VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._renderSemaphore));
	}
}
```

On the fence, we are using the flag `VK_FENCE_CREATE_SIGNALED_BIT` . This is very important, as it allows us to wait on a freshly created fence without causing errors. If we did not have that bit, when we call into WaitFences the first frame, before the gpu is doing work, the thread will be blocked. 

We create the 3 structures for each of our frames. Now that we have them, we can write the draw loop.


# Draw loop
Let's start the draw loop by first waiting for the GPU to have finished its work, using the fence.

```cpp
void VulkanEngine::draw()
{
	// wait until the gpu has finished rendering the last frame. Timeout of 1
	// second
	VK_CHECK(vkWaitForFences(_device, 1, &get_current_frame()._renderFence, true, 1000000000));
	VK_CHECK(vkResetFences(_device, 1, &get_current_frame()._renderFence));
}
```

We use `vkWaitForFences()` to wait for the GPU to have finished its work, and after it we reset the fence.
Fences have to be reset between uses, you can't use the same fence on multiple GPU commands without resetting it in the middle.

The timeout of the WaitFences call is of 1 second. It's using nanoseconds for the wait time.
If you call the function with 0 as the timeout, you can use it to know if the GPU is still executing the command or not.

Next, we are going to request an image index from the swapchain.

<!-- codegen from tag draw_2 on file E:\ProgrammingProjects\vulkan-guide-2\chapter-1/vk_engine.cpp --> 
```cpp
	//request image from the swapchain
	uint32_t swapchainImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(_device, _swapchain, 1000000000, get_current_frame()._swapchainSemaphore, nullptr, &swapchainImageIndex));
```

vkAcquireNextImageKHR will request the image index from the swapchain, and if the swapchain doesn't have any image we can use, it will block the thread with a maximum for the timeout set, which will be 1 second.

Check how we are sending the _swapchainSemaphore to it. This is to make sure that we can sync other operations with the swapchain having an image ready to render.

We use the index given from this function to decide which of the swapchain images we are going to use for drawing.

Time to begin the rendering commands. For that, we are going to reset the command buffer for this frame, and begin it again. 
We will need to use another one of the initializer functions. 

<!-- codegen from tag init_cmd_draw on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_initializers.cpp --> 
```cpp
VkCommandBufferBeginInfo vkinit::command_buffer_begin_info(VkCommandBufferUsageFlags flags /*= 0*/)
{
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext = nullptr;

    info.pInheritanceInfo = nullptr;
    info.flags = flags;
    return info;
}
```

When a command buffer is started, we need to give it an info struct with some properties. We will not be using inheritance info so we can keep it nullptr, but we do need the flags. 

Here is the link to the spec for this structure [VkCommandBufferBeginInfo](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap6.html#VkCommandBufferBeginInfo)

Back to `VulkanEngine::draw()`, we start by resetting the command buffer and restarting it.

<!-- codegen from tag draw_3 on file E:\ProgrammingProjects\vulkan-guide-2\chapter-1/vk_engine.cpp --> 
```cpp
	//naming it cmd for shorter writing
	VkCommandBuffer cmd = get_current_frame()._mainCommandBuffer;

	// now that we are sure that the commands finished executing, we can safely
	// reset the command buffer to begin recording again.
	VK_CHECK(vkResetCommandBuffer(cmd, 0));

	//begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//start the command buffer recording
	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));
```

We are going to copy the command buffer handle from our FrameData into a variable named `cmd`. This is to shorten all other references to it. Vulkan handles are just a 64 bit handle/pointer, so its fine to copy them around, but remember that their actual data is handled by vulkan itself.

Now we call `vkResetCommandBuffer` to clear the buffer. This will completly remove all commands and free its memory. We can now start the command buffer again with `vkBeginCommandBuffer`. On the cmdBeginInfo, we will give it the flag `VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT`. This is optional, but we might get a small speedup from our command encoding if we can tell the drivers that this buffer will only be submitted and executed once. We are only doing 1 submit per frame before the command buffer is reset, so this is perfectly good for us.

With the command buffer recording started, let's add commands to it. We will first transition the swapchain image into a drawable layout, then perform a VkCmdClear on it, and finally transition it back for a display optimal layout. 

This means we are going to need a way to transition images as part of a command buffer instead of using a renderpass, so we are going to add it as a function on vk_images.h

```cpp
#pragma once 

#include <vulkan/vulkan.h>

namespace vkutil {

void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);
}
```

Transitioning an image has loads of possible options. We are going to do the absolute simplest way of implementing this, by only using currentLayout + newLayout.

We will be doing a pipeline barrier, using the syncronization 2 feature/extension which is part of vulkan 1.3 . A pipeline barrier can be used for many different things like syncronizing read/write operation between commands and controlling things like one command drawing into a image and other command using that image for reading.  

Add the function to vk_images.cpp. 

<!-- codegen from tag transition on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_images.cpp --> 
```cpp
#include <vk_initializers.h>

void vkutil::transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier2 imageBarrier {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
    imageBarrier.pNext = nullptr;

    imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

    imageBarrier.oldLayout = currentLayout;
    imageBarrier.newLayout = newLayout;

    VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    imageBarrier.subresourceRange = vkinit::image_subresource_range(aspectMask);
    imageBarrier.image = image;

    VkDependencyInfo depInfo {};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.pNext = nullptr;

    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers = &imageBarrier;

    vkCmdPipelineBarrier2(cmd, &depInfo);
}
```

VkImageMemoryBarrier2 contains the information for a given *image* barrier. On here, is where we set the old and new layouts.
In the StageMask, we are doing ALL_COMMANDS. This is inefficient, as it will stall the GPU pipeline a bit. For our needs, its going to be fine as we are only going to do a few transitions per frame. If you are doing many transitions per frame as part of a post-process chain, you want to avoid doing this, and instead use StageMasks more accurate to what you are doing. 

AllCommands stage mask on the barrier means that the barrier will stop the gpu commands completely when it arrives at the barrier. By using more finegrained stage masks, its possible to overlap the GPU pipeline across the barrier a bit. 
AccessMask is similar, it controls how the barrier stops different parts of the GPU. we are going to use `VK_ACCESS_2_MEMORY_WRITE_BIT` for our source, and add `VK_ACCESS_2_MEMORY_READ_BIT` to our destination. Those are generic options that will be fine.

If you want to read about what would be the optimal way of using pipeline barriers for different use cases, you can find a great reference in here [Khronos Vulkan Documentation: Syncronization examples](https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples) 
This layout transition is going to work just fine for the whole tutorial, but if you want, you can add more complicated transition functions that are more accurate/lightweight.

As part of the barrier, we need to use a `VkImageSubresourceRange` too. This lets us target a part of the image with the barrier. Its most useful for things like array images or mipmapped images, where we would only need to barrier on a given layer or mipmap level. We are going to completely default it and have it transition all mipmap levels and layers.

<!-- codegen from tag subresource on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_initializers.cpp --> 
```cpp
VkImageSubresourceRange vkinit::image_subresource_range(VkImageAspectFlags aspectMask)
{
    VkImageSubresourceRange subImage {};
    subImage.aspectMask = aspectMask;
    subImage.baseMipLevel = 0;
    subImage.levelCount = VK_REMAINING_MIP_LEVELS;
    subImage.baseArrayLayer = 0;
    subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

    return subImage;
}
```

An thing we care in that structure is the AspectMask. This is going to be either `VK_IMAGE_ASPECT_COLOR_BIT` or `VK_IMAGE_ASPECT_DEPTH_BIT`. For color and depth images respectively. We wont need any of the other options. We will keep it as Color aspect under all cases except when the target layout is `VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL`, which we will use later when we add depth buffer.

Once we have the range and the barrier, we pack them into a VkDependencyInfo struct and call `VkCmdPipelineBarrier2`. It is possible to layout transitions multiple images at once by sending more imageMemoryBarriers into the dependency info, which is likely to improve performance if we are doing transitions or barriers for multiple things at once. 

With the transition function implemented, we can now draw things, add `vk_images.h` to the includes on top of `vk_engine.cpp` so we can use the function we just wrote.

<!-- codegen from tag draw_4 on file E:\ProgrammingProjects\vulkan-guide-2\chapter-1/vk_engine.cpp --> 
```cpp

	//make the swapchain image into writeable mode before rendering
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	//make a clear-color from frame number. This will flash with a 120 frame period.
	VkClearColorValue clearValue;
	float flash = std::abs(std::sin(_frameNumber / 120.f));
	clearValue = { { 0.0f, 0.0f, flash, 1.0f } };

	VkImageSubresourceRange clearRange = vkinit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);

	//clear image
	vkCmdClearColorImage(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

	//make the swapchain image into presentable mode
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex],VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	//finalize the command buffer (we can no longer add commands, but it can now be executed)
	VK_CHECK(vkEndCommandBuffer(cmd));
```

We begin by transitioning the swapchain image.
`VK_IMAGE_LAYOUT_UNDEFINED` Is the "dont care" layout. Its also the layout newly created images will be at. We use it when we dont care about the data that is already in the image, and we are fine with the GPU destroying it. 

The target layout we want is `VK_IMAGE_LAYOUT_GENERAL` . This is a general purpose layout, which allows reading and writing from the image. Its not the most optimal layout for rendering, but it is the one we want for `vkCmdClearColorImage` . This is the image layout you want to use if you want to write a image from a compute shader. If you want a read-only image or a image to be used with rasterization commands, there are better options.

For a more detailed list on image layouts, you can check the spec here [Vulkan Spec: image layouts](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#resources-image-layouts)

We now calculate a clear color through a basic formula with the _frameNumber. We will be cycling it through a sin function. This will interpolate between black and blue clear color.

`vkCmdClearColorImage` requires 3 main parameters to work. First of them is the image, which is going to be the one from the swapchain. Then it wants a clear color, and then it needs a subresource range for what part of the image to clear, which we are going to use a default ImageSubresourceRange for. 

With the clear command executed, we now need to transition the image to `VK_IMAGE_LAYOUT_PRESENT_SRC_KHR` which is the only image layout that the swapchain allows for presenting to screen. And at the end, we finish by calling `vkEndCommandBuffer`

With this, we now have a fine command buffer that is recorded and ready to be dispatched into the gpu. We could call VkQueueSubmit already, but its going to be of little use right now as we need to also connect the syncronization structures for the logic to interact correctly with the swapchain.

We will be using `vkQueueSubmit2` for submitting our commands. This is part of syncronization-2 and is an updated version of the older `VkQueueSubmit` from vulkan 1.0.
The function call requires a `VkSubmitInfo2` which contains the information on the semaphores used as part of the submit, and we can give it a Fence so that we can check for that submit to be finished executing. 
`VkSubmitInfo2` requires `VkSemaphoreSubmitInfo` for each of the semaphores it uses, and a `VkCommandBufferSubmitInfo` for the command buffers that will be enqueued as part of the submit. Lets check the vkinit functions for those.

<!-- codegen from tag init_submit on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_initializers.cpp --> 
```cpp
VkSemaphoreSubmitInfo vkinit::semaphore_submit_info(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore)
{
	VkSemaphoreSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.semaphore = semaphore;
	submitInfo.stageMask = stageMask;
	submitInfo.deviceIndex = 0;
	submitInfo.value = 1;

	return submitInfo;
}

VkCommandBufferSubmitInfo vkinit::command_buffer_submit_info(VkCommandBuffer cmd)
{
	VkCommandBufferSubmitInfo info{};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	info.pNext = nullptr;
	info.commandBuffer = cmd;
	info.deviceMask = 0;

	return info;
}

VkSubmitInfo2 vkinit::submit_info(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo,
    VkSemaphoreSubmitInfo* waitSemaphoreInfo)
{
    VkSubmitInfo2 info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    info.pNext = nullptr;

    info.waitSemaphoreInfoCount = waitSemaphoreInfo == nullptr ? 0 : 1;
    info.pWaitSemaphoreInfos = waitSemaphoreInfo;

    info.signalSemaphoreInfoCount = signalSemaphoreInfo == nullptr ? 0 : 1;
    info.pSignalSemaphoreInfos = signalSemaphoreInfo;

    info.commandBufferInfoCount = 1;
    info.pCommandBufferInfos = cmd;

    return info;
}
```

command_buffer_submit_info only needs the command buffer handle. We dont need anthing else, and we can leave the deviceMask at 0 as we are not it.

semaphore_submit_info requires a StageMask, which is the same as we saw with the transition_image function. Other than that, it only needs the semaphore handle. 
device index parameter is used for multi-gpu semaphore usage, but we wont do any of it. and value is used for timeline semaphores, which are a special type of semaphore where they work through a counter instead of a binary state. We will also not be using them, so we can default it to 1

submit_info arranges everything together. it needs the command submit info, and then the semaphore wait and signal infos. We are going to only use 1 semaphore each for waiting and signaling, but its possible to signal or wait on multiple semaphores at once for more complicated systems.

Here are the links to spec for those structures: [VkCommandBufferSubmitInfo](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap6.html#VkCommandBufferSubmitInfo), [VkSemaphoreSubmitInfo](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap6.html#VkSemaphoreSubmitInfo), [VkSubmitInfo2](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap6.html#VkSubmitInfo2)

With the initializers made, we can write the submit itself.

<!-- codegen from tag draw_5 on file E:\ProgrammingProjects\vulkan-guide-2\chapter-1/vk_engine.cpp --> 
```cpp
	//prepare the submission to the queue. 
	//we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
	//we will signal the _renderSemaphore, to signal that rendering has finished

	VkCommandBufferSubmitInfo cmdinfo = vkinit::command_buffer_submit_info(cmd);	
	
	VkSemaphoreSubmitInfo waitInfo = vkinit::semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,get_current_frame()._swapchainSemaphore);
	VkSemaphoreSubmitInfo signalInfo = vkinit::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, get_current_frame()._renderSemaphore);	
	
	VkSubmitInfo2 submit = vkinit::submit_info(&cmdinfo,&signalInfo,&waitInfo);	

	//submit command buffer to the queue and execute it.
	// _renderFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit2(_graphicsQueue, 1, &submit, get_current_frame()._renderFence));
```

We first create each of the different info structs needed, and then we call `vkQueueSubmit2`.
For our command info we are just going to send the command we just recorded.
For the wait info, we are going to use the swapchain semaphore of the current frame. When we called `vkAcquireNextImageKHR`, we set this same semaphore to be signaled, so with this, we make sure that the commands executed here wont begin until the swapchain image is ready.

For signal info, we will be using the _renderSemaphore of the current frame, which will lets us syncronize with presenting the image on the screen.

And for the fence, we are going to use the current frame _renderFence. At the start of the draw loop, we waited for that same fence to be ready. This is how we are going to syncronize our gpu to the cpu, as when the cpu goes ahead of the GPU, the fence will stop us so we dont use any of the other structures from this frame until the draw commands are executed.

Last thing we need on the frame is to present the image we have just drawn into the screen

<!-- codegen from tag draw_6 on file E:\ProgrammingProjects\vulkan-guide-2\chapter-1/vk_engine.cpp --> 
```cpp
	//prepare present
	// this will put the image we just rendered to into the visible window.
	// we want to wait on the _renderSemaphore for that, 
	// as its necessary that drawing commands have finished before the image is displayed to the user
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.pSwapchains = &_swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &get_current_frame()._renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	VK_CHECK(vkQueuePresentKHR(_graphicsQueue, &presentInfo));

	//increase the number of frames drawn
	_frameNumber++;

```

`vkQueuePresent` has a very similar info struct as the queue submit. It also has the pointers for the semaphores, but it has image index and swapchain index. 
We will wait on the _renderSemaphore, and connect it to our swapchain. This way, we wont be presenting the image to the screen until it has finished the rendering commands from the submit right before it.

At the end of the function, we increment frame counter.

With this, we have the draw loop done. Only thing left is to clean up the sync structures properly as part of the cleanup function

```cpp
	for (int i = 0; i < FRAME_OVERLAP; i++) {
	
		//already written from before
		vkDestroyCommandPool(_device, _frames[i]._commandPool, nullptr);

		//destroy sync objects
		vkDestroyFence(_device, _frames[i]._renderFence, nullptr);
		vkDestroySemaphore(_device, _frames[i]._renderSemaphore, nullptr);
		vkDestroySemaphore(_device ,_frames[i]._swapchainSemaphore, nullptr);
	}
```

Try running the engine at this moment. If everything is correct, you should have a window with a flashing blue screen. Check the validation layers, as they will catch possible syncronization problems.

This concludes chapter 1, and the next we are going to do is to begin using some compute shaders to draw other things than a simple flashing screen.

Next: [Chapter 2: Improving the render loop]({{ site.baseurl }}{% link docs/new_chapter_2/vulkan_new_rendering.md %})



{% include comments.html term="Vkguide 2 Beta Comments" %}
