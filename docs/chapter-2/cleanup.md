---
layout: default
title: Cleanup and deletion queue
parent:  "2. The graphics pipeline, Hello Triangle"
grand_parent: Legacy VkGuide, Vulkan 1.1
nav_order: 20
---

If you have run the triangle with layers enabled, you have probably seen that when you close the application, the layers complain about pipelines and other objects not being deleted. Given how far the amount of Vulkan objects we have is growing, it is time to do a small refactor, and implement a better system to take care of those deletions.

## Deletion queue
In Vulkan, we can't delete any object until we are sure that the GPU isn't using it. Deleting the objects out of order is also a big issue, as it will make the layers complain and might crash the drivers. For those reasons, using normal Cpp destructors is out of the question, and not doable. We need a better system to delete objects.

A very common implementation is to create a deletion queue. When creating objects, we will also add the objects to the queue, and then at some point in the application, once we are sure that the GPU is finished, we go through the queue deleting everything.

One way of implementing said deletion queue is by having arrays with the Vulkan objects, and then deleting those in order. But we need something far more simple for now.

The way we are going to implement it is by having a queue of std::function lambdas, that will get called in order FIFO (first in, first out). This way is not the most efficient, but it is good enough for a small engine.

`vulkan_engine.h`
```cpp
struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& function) {
		deletors.push_back(function);
	}

	void flush() {
		// reverse iterate the deletion queue to execute all the functions
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
			(*it)(); //call the function
		}

		deletors.clear();
	}
};

//add the deletion queue as a variable of vulkanengine
class VulkanEngine{
// other code .....
	DeletionQueue _mainDeletionQueue;
}
```

Make sure to `#include <functional>` so that you can use std::function, and `#include <deque>` so that you can use deque. You can include them on vk_engine.h or on vk_types.h

This is the implementation of the deletion queue we will use from now on. The usage is this

```cpp
VkSomething something;
VkCreateSomething(&something);

_mainDeletionQueue.push_function([=](){
VkDeleteSomething(something);
});
```

And then, as part of our cleanup function, we `flush()` the deletion queue, which will call the lambdas in the order they were enqueued.
It's very important to keep in mind how cpp lambdas capture data. In here we are using `[=]` capture, which means that it will create a *copy* of the objects. Be very careful with this, and never capture anything by reference unless you know what you are doing.
In the example above, it will call the DestroyFence first, and then the DestroySemaphore calls, as it keeps order.

Now that we have this, let's implement it over the current codebase so we can go back to a state where the validation layers don't complain.
If you want to skip this step, you can look at the tutorial code, which already uses it.

## Refactoring the code
We will begin by changing the `VulkanEngine::Cleanup()` to use the deletion queue.

```cpp
void VulkanEngine::cleanup()
{
	if (_isInitialized) {

		//make sure the GPU has stopped doing its things
		vkWaitForFences(_device, 1, &_renderFence, true, 1000000000);

		_mainDeletionQueue.flush();

		vkDestroyDevice(_device, nullptr);
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
		vkDestroyInstance(_instance, nullptr);
		SDL_DestroyWindow(_window);
	}
}
```

We are not going to put everything in the deletion queue, so we will keep the Surface, Device, Instance, and the SDL window.
All the others we are going to convert to use the deletion queue.


Let's add the deletion queue code to the swapchain code
```cpp
void VulkanEngine::init_swapchain()
{
	//other code ....

	//store swapchain and its related images
	_swapchain = vkbSwapchain.swapchain;
	_swapchainImages = vkbSwapchain.get_images().value();
	_swapchainImageViews = vkbSwapchain.get_image_views().value();

	_swapchainImageFormat = vkbSwapchain.image_format;

	_mainDeletionQueue.push_function([=]() {
		vkDestroySwapchainKHR(_device, _swapchain, nullptr);
	});
}
```

Now delete the renderpass properly

```cpp
void VulkanEngine::init_default_renderpass()
{
	// other code.....

	VK_CHECK(vkCreateRenderPass(_device, &render_pass_info, nullptr, &_renderPass));

	_mainDeletionQueue.push_function([=]() {
		vkDestroyRenderPass(_device, _renderPass, nullptr);
    });
}
```

Next is deleting the framebuffers


```cpp
void VulkanEngine::init_framebuffers()
{
	//create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
	VkFramebufferCreateInfo fb_info = vkinit::framebuffer_create_info(_renderPass, _windowExtent);

	const uint32_t swapchain_imagecount = _swapchainImages.size();
	_framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

	for (int i = 0; i < swapchain_imagecount; i++) {

		fb_info.pAttachments = &_swapchainImageViews[i];
		VK_CHECK(vkCreateFramebuffer(_device, &fb_info, nullptr, &_framebuffers[i]));

		_mainDeletionQueue.push_function([=]() {
			vkDestroyFramebuffer(_device, _framebuffers[i], nullptr);
			vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
    	});
	}
}
```

Command pools next

```cpp
void VulkanEngine::init_commands()
{
	//create a command pool for commands submitted to the graphics queue.
	//we also want the pool to allow for resetting of individual command buffers
	VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_commandPool));

	//allocate the default command buffer that we will use for rendering
	VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_commandPool, 1);

	VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_mainCommandBuffer));

	_mainDeletionQueue.push_function([=]() {
		vkDestroyCommandPool(_device, _commandPool, nullptr);
	});
}
```


And sync structures
```cpp
void VulkanEngine::init_sync_structures()
{
	VkFenceCreateInfo fenceCreateInfo = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);

	VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_renderFence));

    //enqueue the destruction of the fence
    _mainDeletionQueue.push_function([=]() {
        vkDestroyFence(_device, _renderFence, nullptr);
    });

	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphore_create_info();

	VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_presentSemaphore));
	VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_renderSemaphore));

    //enqueue the destruction of semaphores
    _mainDeletionQueue.push_function([=]() {
        vkDestroySemaphore(_device, _presentSemaphore, nullptr);
        vkDestroySemaphore(_device, _renderSemaphore, nullptr);
    });
}
```

We add new initializers to vk_initializers, for fence and semaphore creation
```cpp
//header
VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0);
VkSemaphoreCreateInfo semaphore_create_info(VkSemaphoreCreateFlags flags = 0);

//implementation
VkFenceCreateInfo vkinit::fence_create_info(VkFenceCreateFlags flags)
{
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;
    fenceCreateInfo.flags = flags;
    return fenceCreateInfo;
}

VkSemaphoreCreateInfo vkinit::semaphore_create_info(VkSemaphoreCreateFlags flags)
{
    VkSemaphoreCreateInfo semCreateInfo = {};
    semCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semCreateInfo.pNext = nullptr;
    semCreateInfo.flags = flags;
    return semCreateInfo;
}
```

Lastly, we add the deletion for the new objects from this chapter. For the shader modules, we aren't going to use the queue.
When a pipeline is created from a shader module, you can then delete the shader module and it will be ok.
```cpp

void VulkanEngine::init_pipelines()
{
// other code .....

	//destroy all shader modules, outside of the queue
	vkDestroyShaderModule(_device, redTriangleVertShader, nullptr);
	vkDestroyShaderModule(_device, redTriangleFragShader, nullptr);
	vkDestroyShaderModule(_device, triangleFragShader, nullptr);
	vkDestroyShaderModule(_device, triangleVertexShader, nullptr);

 	_mainDeletionQueue.push_function([=]() {
		//destroy the 2 pipelines we have created
		vkDestroyPipeline(_device, _redTrianglePipeline, nullptr);
        vkDestroyPipeline(_device, _trianglePipeline, nullptr);

		//destroy the pipeline layout that they use
		vkDestroyPipelineLayout(_device, _trianglePipelineLayout, nullptr);
    });
}

```


At this point, if you run the engine, and close it, the debug layers should only complain about a missing `VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT;` As this object is something implicitly created by vk-bootstrap library, we aren't going to care about it.

With this, chapter 2 is completed. Next step is to start rendering meshes in chapter 3.

Next: [Vertex Buffers]({{ site.baseurl }}{% link docs/chapter-3/triangle_mesh.md %})

{% include comments.html term="Chapter 2 Comments" %}
