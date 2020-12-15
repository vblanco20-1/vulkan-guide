---
layout: default
title: Mainloop Code
parent:  "1. Initialization and Render Loop"
nav_order: 41
---

We are going to need a couple more members in our VulkanEngine class, to hold the synchronization structures (Semaphore and Fence) that we will need for the main loop. Plus the function to initalize them.


```cpp
class VulkanEngine {
public:

	//--- other code ---
	VkSemaphore _presentSemaphore, _renderSemaphore;
	VkFence _renderFence;
private:

	//--- other code ---
	void init_sync_structures();
}


//make sure to call the new init function from end of the main init
void VulkanEngine::init()
{
	//--- other code ---
	init_sync_structures();

	//everything went fine
	_isInitialized = true;
}
```



We are going to need 2 semaphores and the main render fence. Let us begin creating them.
```cpp
void VulkanEngine::init_sync_structures()
{
	//create syncronization structures
	
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;

	//we want to create the fence with the Create  Signaled flag, so we can wait on it before using it on a gpu command (for the first frame)
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_renderFence));

	//for the semaphores we don't need any flags
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;

	VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_presentSemaphore));
	VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_renderSemaphore));
}
```

Creating the fences and semaphores is very straightforward as they are relatively simple structures.

# Draw loop
Lets start the draw loop by first waiting for the GPU to have finished its work, using the fence

```cpp
void VulkanEngine::draw()
{
	//wait until the gpu has finished rendering the last frame. Timeout of 1 second
	VK_CHECK(vkWaitForFences(_device, 1, &_renderFence, true, 1000000000));
	VK_CHECK(vkResetFences(_device, 1, &_renderFence));
}
```

We use `vkWaitForFences()` to wait for the GPU to have finished its work, and after it we reset the fence.
Fences have to be reset between uses, you cant use the same fence on multiple gpu commands without resetting it in the middle.

The timeout of the WaitFences call is of 1 second. Its using nanoseconds for the wait time.
If you call the function with 0 as the timeout, you can use it to know if the GPU is still executing the command or not.

Next, we are going to request a image index from the swapchain

```cpp
	//request image from the swapchain, one second timeout
	uint32_t swapchainImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(_device, _swapchain, 1000000000, _presentSemaphore, nullptr, &swapchainImageIndex));
```

vkAcquireNextImageKHR will request the image index from the swapchain, and if the swapchain doesnt have any image we can use, it will block the thread with a maximum for the timeout set, which will be 1 second. This will be our FPS lock.

Check how we are sending the _presentSemaphore to it. This is to make sure that we can sync other operations with the swapchain having an image ready to render.

Time to begin the rendering commands

```cpp

	//now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
	VK_CHECK(vkResetCommandBuffer(_mainCommandBuffer, 0));
```

We need to reset the command buffer first, to empty it and start enqueuing new commands.
Once the command buffer is reset, we can begin it.

```cpp
	//naming it cmd for shorter writing
	VkCommandBuffer cmd = _mainCommandBuffer;

	//begin the command buffer recording. We will use this command buffer exactly once, so we want to let Vulkan know that
	VkCommandBufferBeginInfo cmdBeginInfo= {};
	cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBeginInfo.pNext = nullptr;

	cmdBeginInfo.pInheritanceInfo = nullptr;
	cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));
```

Yet another Vulkan info structure, so typical sType and pNext values.
Inheritance info on a command buffer is used for secondary command buffers, but we arent going to use them, so leave it nullptr.
For flags, we want to let Vulkan know that this command buffer will be submitted once.
As we are going to be recording the command buffer every frame, its best if Vulkan knows that this command will only execute once, as it can allow for great optimization by the driver.

With the command buffer recording started, lets add commands to it. We are going to launch the render pass, with a clear color that flashes over time.

```cpp
	//make a clear-color from frame number. This will flash with a 120 frame period.
	VkClearValue clearValue;
	float flash = abs(sin(_frameNumber / 120.f));
	clearValue.color = { { 0.0f, 0.0f, flash, 1.0f } };

	//start the main renderpass. 
	//We will use the clear color from above, and the framebuffer of the index the swapchain gave us
	VkRenderPassBeginInfo rpInfo = {};
	rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rpInfo.pNext = nullptr;

	rpInfo.renderPass = _renderPass;
	rpInfo.renderArea.offset.x = 0;
	rpInfo.renderArea.offset.y = 0;
	rpInfo.renderArea.extent = _windowExtent;
	rpInfo.framebuffer = _framebuffers[swapchainImageIndex];	

	//connect clear values
	rpInfo.clearValueCount = 1;
	rpInfo.pClearValues = &clearValue;

	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
```

Beginning a render pass needs us to start writing another info structure, as beginning a renderpass needs a lot of parameters.
Set sType and pNext as usual
`.renderPass` is whatever render pass we want to begin
`.renderArea.offset` and `.renderArea.extent` is what area will be rendered, in case we want to render a small renderpass into a bigger image. We will just set the offset to 0 (no offset) and the extent to our main window size.

`.framebuffer` is what image will we render into for this renderpass, so we are going to index into the cached _framebuffers with the image index we got from the swapchain.

Lastly, we are going to create a VkClearValue of a flashing blue color, and connect it to the info. We are using the `_frameNumber` variable to get the numbers of frames rendered and use it for the flashing. This variable was in the engine from the starting-point code.

The `vkCmdBeginRenderPass()` function will bind the framebuffers, clear the image, and put the images in the layout we specified when creating the renderpass. We can now start rendering commands... but we don't have anything to render yet. That will be on next chapter.

We can now end the render pass, and also end the command buffer
```cpp
	//finalize the render pass
	vkCmdEndRenderPass(cmd);
	//finalize the command buffer (we can no longer add commands, but it can now be executed)
	VK_CHECK(vkEndCommandBuffer(cmd));
```

Calling `vkCmdEndRenderPass()` finishes the rendering, and transitions the image to what we specified, which is "ready to be displayed". As the work is now done, we can `vkEndCommandBuffer()` to finish the command buffer.

With the buffer finished, we can execute it by submitting it into the GPU

```cpp

	//prepare the submission to the queue. 
	//we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
	//we will signal the _renderSemaphore, to signal that rendering has finished

	VkSubmitInfo submit = {};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.pNext = nullptr;

	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	submit.pWaitDstStageMask = &waitStage;

	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &_presentSemaphore;

	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &_renderSemaphore;

	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &cmd;

	//submit command buffer to the queue and execute it.
	// _renderFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit(_graphicsQueue, 1, &submit, _renderFence));
```

To execute a `vkQueueSubmit()` we need to setup the info structure. we are going to configure it to wait on the `_presentSemaphore`, and signal the `_renderSemaphore`. 
By waiting on the `_presentSemaphore` that we were signaling from `vkAcquireNextImageKHR`, we make sure that the image for rendering is completely ready in the GPU.

We then also set the command buffer we are going to submit.

the `.pWaitDstStageMask` is a complex parameter. We are not going to explain it until we go into details of synchronization.

After the commands are submitted, we now display the image to the screen
```cpp
	// this will put the image we just rendered to into the visible window.
	// we want to wait on the _renderSemaphore for that, 
	// as its necessary that drawing commands have finished before the image is displayed to the user
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;

	presentInfo.pSwapchains = &_swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &_renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	VK_CHECK(vkQueuePresentKHR(_graphicsQueue, &presentInfo));

	//increase the number of frames drawn
	_frameNumber++;
```

The `vkQueuePresentKHR` function displays an image to the screen. We have to tell it what swapchain we are using for the call, and what is the image index.
We also need to set the WaitSemaphore correctly with the `_renderSemaphore` we signal from the VkQueueSubmit of the main rendering. This will tell the GPU to only display the image to the screen once the main render work has finished execution. As our rendering frame is done now, we can increment the `_frameNumber` variable to increase engine time.

We finally have something rendering! You should be seeing a flashing blue screen.

Next: [Chapter 2: Vulkan Pipelines]({{ site.baseurl }}{% link docs/chapter-2/vulkan_render_pipeline.md %})

{% include comments.html term="1. Comments" %}
