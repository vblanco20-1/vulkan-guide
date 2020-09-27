---
layout: default
title: Double buffering
parent: Chapter 4
nav_order: 10
---

Before we start implementing descriptor sets to improve sending data to the gpu, there is something we have to do. Right now, the engine will only execute one frame at a time, which is not optimal. While the GPU is busy drawing a frame, the CPU is waiting for that frame to end. Performance takes a huge hit here as the CPU will spend a lot of time waiting for the GPU.
We are going to refactor a few things in the engine to implement double buffering for the rendering work. While the GPU is busy drawing frame N, the cpu will be preparing the work for frame N+1. This way, the CPU will be doing work while the GPU is running, instead of waiting. This will not add extra latency, and will improve performance a lot.
It is possible to make the CPU render ahead more frames, which can be useful if your cpu work is wildly varying, but in general, just overlapping the next frame will be enough and work well.

## Object lifetime
Most Vulkan objects are used while the Gpu is performing its rendering work, so it is not possible to modify or delete them while they are in use.
An example of this is command buffers. Once you submit a command buffer into a queue, that buffer cant be reset or modified until the GPU has finished executing its commands.
You can control this using Fences. If you submit a command buffer that will signal a fence, and then you wait until that fence is signaled, you can be sure that the command buffer can now be reused or modified. Its the same for the other objects related used in those commands.


## The Frame struct
We are going to move a few of the rendering related structures from the core vulkanEngine class into a "Frame" struct. This way we can control their lifetime a bit better.

<vk_engine.h>
```cpp
struct FrameData {
	VkSemaphore _presentSemaphore, _renderSemaphore;
	VkFence _renderFence;	

	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;
};
```

We are moving those structures (semaphores, fence, command pool and command buffer), from the core class into the struct. Delete them from the class too.

On its place, we add a fixed array of FrameData structs.

```cpp
//number of frames to overlap when rendering
constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanEngine {
public:


//other code ....
//frame storage
FrameData _frames[FRAME_OVERLAP];

//getter for the frame we are rendering to right now.
FrameData& get_current_frame();

//other code ....
}
```

The implementation for `get_current_frame()` is going to be this.

```cpp
FrameData& VulkanEngine::get_current_frame()
{
	return _frames[_frameNumber % FRAME_OVERLAP];
}
```
Every time we render a frame, the _frameNumber gets bumped by 1. This will be very useful here. With a frame overlap of 2 (the default), it means that even frames will use `_frames[0]`, while odd frames will use `_frames[1]`, While the GPU is busy executing the rendering commands from frame 0, the CPU will be writing the buffers of frame 1, and reverse.

Now we need to modify the sync structures and the command buffer structures on the engine so that they use this _frames structs.

In `init_commands()` function, we change it into a loop that initializes the commands for both frames
```cpp
void VulkanEngine::init_commands()
{
	//create a command pool for commands submitted to the graphics queue.
	//we also want the pool to allow for resetting of individual command buffers
	VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	for (int i = 0; i < FRAME_OVERLAP; i++) {

	
		VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_frames[i]._commandPool));

		//allocate the default command buffer that we will use for rendering
		VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_frames[i]._commandPool, 1);

		VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_frames[i]._mainCommandBuffer));

		_mainDeletionQueue.push_function([=]() {
			vkDestroyCommandPool(_device, _frames[i]._commandPool, nullptr);
		});
	}
}
```

Note that we are creating 2 separated command pools. This is not strictly necessary right now, but its much more necessary if you create multiple command buffers per frame and want to delete them at once. (Resetting a command pool will reset all the command buffers created from it)

In the `init_sync_structures()` function, we also create a set of semaphores and fences for each frame

```cpp
void VulkanEngine::init_sync_structures()
{	
	VkFenceCreateInfo fenceCreateInfo = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);

	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphore_create_info();

	for (int i = 0; i < FRAME_OVERLAP; i++) {     

        VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_frames[i]._renderFence));

        //enqueue the destruction of the fence
        _mainDeletionQueue.push_function([=]() {
            vkDestroyFence(_device, _frames[i]._renderFence, nullptr);
            });


        VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._presentSemaphore));
        VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._renderSemaphore));

        //enqueue the destruction of semaphores
        _mainDeletionQueue.push_function([=]() {
            vkDestroySemaphore(_device, _frames[i]._presentSemaphore, nullptr);
            vkDestroySemaphore(_device, _frames[i]._renderSemaphore, nullptr);
            });
	}
}
```

With this, we have created the structures we need for the multiple frames, so now we need to change the render loop to use them

In the `draw()` function, change every instance of _renderFence usage with `get_current_frame()._renderFence`. Do exactly the same for:
`_mainCommandBuffer`
`_presentSemaphore`
`_renderSemaphore`

Example:
```cpp
    //wait until the gpu has finished rendering the last frame. Timeout of 1 second
	VK_CHECK(vkWaitForFences(_device, 1, &get_current_frame()._renderFence, true, 1000000000));
	VK_CHECK(vkResetFences(_device, 1, &get_current_frame()._renderFence));

    //now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
	VK_CHECK(vkResetCommandBuffer(get_current_frame()._mainCommandBuffer, 0));

```