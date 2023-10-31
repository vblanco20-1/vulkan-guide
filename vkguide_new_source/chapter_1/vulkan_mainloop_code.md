---
layout: default
title: Mainloop Code
parent:  "New 1. Initializing Vulkan"
nav_order: 41
---

The first thing we need to do, is to add the syncronization structures that we are going to need into our FrameData structure




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

Lets initialize them. We will begin by adding the functions to make a VkFenceCreateInfo and a VkSemaphoreCreateInfo on our vk_initializers.cpp/h code.

^code init_sync shared/vk_initializers.cpp

Both of these structures are pretty simple and need almost no options other than to give them some flags. Lets write the actual creation now.

^code init_sync chapter-1/vk_engine.cpp

On the fence, we are using the flag `VK_FENCE_CREATE_SIGNALED_BIT` . This is very important, as it allows us to wait on a freshly created fence without causing errors. If we did not have that bit, when we call into WaitFences, the cpu will hang, as there is no code that will signal that fence, so it just stops. 

We create the 3 structures for each of our doublebuffered frames. Now that we have them, we can write the draw loop.


# Draw loop
Let's start the draw loop by first waiting for the GPU to have finished its work, using the fence.

```cpp
void VulkanEngine::draw()
{
^code draw_1 chapter-1/vk_engine.cpp
}
```

We use `vkWaitForFences()` to wait for the GPU to have finished its work, and after it we reset the fence.
Fences have to be reset between uses, you can't use the same fence on multiple GPU commands without resetting it in the middle.

The timeout of the WaitFences call is of 1 second. It's using nanoseconds for the wait time.
If you call the function with 0 as the timeout, you can use it to know if the GPU is still executing the command or not.

Next, we are going to request an image index from the swapchain.


^code draw_2 chapter-1/vk_engine.cpp


vkAcquireNextImageKHR will request the image index from the swapchain, and if the swapchain doesn't have any image we can use, it will block the thread with a maximum for the timeout set, which will be 1 second. This will be our FPS lock.

Check how we are sending the _swapchainSemaphore to it. This is to make sure that we can sync other operations with the swapchain having an image ready to render.

We use the index given from this function to decide which of the swapchain images we are going to use for drawing.

Time to begin the rendering commands. For that, we are going to reset the command buffer for this frame, and begin it again. 
We will need 2 extra functions added into the initializers code. 

^code init_cmd_draw shared/vk_initializers.cpp

when a command buffer is started, we need to give it an info struct with some properties. We will not be using inheritance info so we can keep it nullptr, but we do need the flags. 

Back to VulkanEngine::draw(), we start by resetting the command buffer and restarting it.

^code draw_2 chapter-1/vk_engine.cpp

We are going to copy the command buffer handle from our FrameData into a variable named `cmd`. this is to shorten all other references to it. Vulkan handles are just a 64 bit handle/pointer, so its fine to copy them around, but remember that their actual data is handled by vulkan itself.

Now we call `vkResetCommandBuffer` to clear the buffer. This will generally completly remove the commands and likely free its memory. We can now start the command buffer again with `vkBeginCommandBuffer`. On the cmdBeginInfo, we will give it the flag `VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT`. This is optional, but we might get a small speedup from our command encoding if we can tell the drivers that this buffer will only be submitted and executed once. We are only doing 1 submit per frame before the command buffer is reset, so this is perfectly good for us.

With the command buffer recording started, let's add commands to it. We will first transition the swapchain image into a drawable layout, then perform a VkCmdClear on it, and then transition it back for a display optimal layout. 

This means we are going to need a way to transition images as part of a command buffer instead of using a renderpass, so we are going to add it as a function on vk_images.h

```cpp
#pragma once 

#include <vulkan/vulkan.h>

namespace vkutil {

void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);
}
```

transitioning a image has loads of possible options. We are going to do the absolute simplest way of implementing this, by only using currentLayout + newLayout.


We will be doing a pipeline barrier, using the syncronization 2 feature/extension which is part of vulkan 1.3 . A pipeline barrier can be used for many different things like syncronizing read/write operation between commands and controlling things like one command drawing into a image and other command using that image for reading.  

^code transition shared/vk_images.cpp

VkImageMemoryBarrier2 contains the information for a given *image* barrier. On here, is where we set the old and new layouts.
In the StageMask, we are doing ALL_COMMANDs. This is inneficient, as it will stall the GPU pipeline a bit. For our needs, its going to be fine as we are only going to do a few transitions per frame. If you are doing many transitions per frame as part of a post-process chain, you want to avoid doing this, and instead use much better StageMasks. 
AllCommands stage mask on the barrier means that the barrier will stop the gpu commands completely when it arrives at the barrier. By using more finegrained stage masks, its possible to overlap the GPU pipeline across the barrier a bit. 
AccessMask is similar, it controls how the barrier stops different parts of the GPU. we are going to use `VK_ACCESS_2_MEMORY_WRITE_BIT` for our source, and add `VK_ACCESS_2_MEMORY_READ_BIT` to our destination. Those are generic options that will be fine.

If you want to read about what would be the optimal way of using pipeline barriers for different use cases, you can find a great reference in here [Khronos Vulkan Documentation: Syncronization examples](https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples) 
This layout transition is going to work just fine for the whole tutorial, but if you want, you can add more complicated transition functions that are more accurate/lightweight.

As part of the barrier, we need to use a VkImageSubresourceRange too. This lets us target a part of the image with the barrier. Its most useful for things like array images or mipmapped images, where we would only need to barrier on a given layer or mipmap level. We are going to completely default it as we only need to transition mip level 0 here. Lets add it into vk_initializers

^code subresource shared/vk_initializers.cpp

An thing we care in that structure is the AspectMask. This is going to be either `VK_IMAGE_ASPECT_COLOR_BIT` or `VK_IMAGE_ASPECT_DEPTH_BIT`. For color and depth images respectively. We wont need any of the other options. We will keep it as Color aspect under all cases except when the target layout is `VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL`, which we will use later when we add depth buffer.

Once we have the range and the barrier, we pack them into a VkDependencyInfo struct and call `VkCmdPipelineBarrier2`. It is possible to layout transitions multiple images at once by sending more imageMemoryBarriers into the dependency info, which is likely to improve performance if we are doing transitions or barriers for multiple things at once. 

With the transition function implemented, we can now actually draw things.

^code draw_3 chapter-1/vk_engine.cpp

We begin by transitioning the swapchain image.
`VK_IMAGE_LAYOUT_UNDEFINED` Is the "dont care" layout. Its also the layout newly created images will be at. We use it when we dont care about the data that is already in the image, and we are fine with the GPU destroying it. 

The target layout we want is `VK_IMAGE_LAYOUT_GENERAL` . This is a general purpose layout, which allows reading and writing from the image. Its not the most optimal layout for rendering, but it is the one we want for `vkCmdClearColorImage` . This is the image layout you want to use if you want to write a image from a compute shader. If you want a read-only image or a image to be used with rasterization commands, there are better options.

We now calculate a clear color through a basic formula with the _frameNumber. We will be cycling it through a sin function. This will interpolate between black and blue clear color.

`vkCmdClearColorImage` requires 3 main parameters to work. First of them is the image, which is going to be the one from the swapchain. Then it wants a clear color, and then it needs a subresource range for what part of the image to clear, which we are going to use a default ImageSubresourceRange for. 

With the clear command executed, we now need to transition the image to `VK_IMAGE_LAYOUT_PRESENT_SRC_KHR` which is the only image layout that the swapchain allows for presenting to screen. And at the end, we finish by calling `vkEndCommandBuffer`

With this, we now have a fine command buffer that is recorded and ready to be dispatched into the gpu. We could call VkQueueSubmit already, but its going to be of little use right now as we need to also connect the syncronization structures for the logic to interact correctly with the swapchain.


We will be using `vkQueueSubmit2` for submitting our commands. This is part of syncronization-2 and is an updated version of the older `VkQueueSubmit` from vulkan 1.0.
The function call requires a `VkSubmitInfo2` which contains the information on the semaphores used as part of the submit, and we can give it a Fence so that we can check for that submit to be finished executing. 
`VkSubmitInfo2` requires `VkSemaphoreSubmitInfo` for each of the semaphores it uses, and a `VkCommandBufferSubmitInfo` for the command buffers that will be enqueued as part of the submit. Lets add all those into vk_initializers so that we can use them.

^code init_submit shared/vk_initializers.cpp

command_buffer_submit_info only needs the command buffer handle. We dont need anthing else, and we can leave the deviceMask at 0 as we are not it.

semaphore_submit_info requires a StageMask, which is the same as we saw with the transition_image function. Other than that, it only needs the semaphore handle. 
device index parameter is used for multi-gpu semaphore usage, but we wont do any of it. and value is used for timeline semaphores, which are a special type of semaphore where they work through a counter intead of a binary state. We will also not be using them, so we can default it to 1

submit_info arranges everything together. it needs the command submit info, and then the semaphore wait and signal infos. We are going to only use 1 semaphore each for waiting and signaling, but its possible to signal or wait on multiple semaphores at once for more complicated systems.

With the initializers made, we can write the submit itself.

^code draw_4 chapter-1/vk_engine.cpp

We first create each of the different info structs needed, and then we call `vkQueueSubmit2`.
For our command info we are just going to send the command we just recorded.
For the wait info, we are going to use the swapchain semaphore of the current frame. When we called `vkAcquireNextImageKHR`, we set this same semaphore to be signaled, so with this, we make sure that the commands executed here wont begin until the swapchain image is ready.

For signal info, we will be using the _renderSemaphore of the current frame, which will lets us syncronize with presenting the image on the screen.

And for the fence, we are going to use the current frame _renderFence. At the start of the draw loop, we waited for that same fence to be ready. This is how we are going to syncronize our gpu to the cpu, as when the cpu goes ahead of the GPU, the fence will stop us so we dont use any of the other structures from this frame until the draw commands are executed.


Last thing we need on the frame is to present the image we have just drawn into the screen

^code draw_5 chapter-1/vk_engine.cpp

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

