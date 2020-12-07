---
layout: default
title: Memory transfers
parent: "5. Textures"
nav_order: 11
---

Before we go to implement textures, there is something we need to prepare first.
Right now, we are implementing vertex buffers by storing them in CPU_TO_GPU memory type. This is doable, but its not the recommended way of dealing with mesh data.
For vertex buffers, you want to have them accessible in the fastest memory type possible, which is GPU_ONLY memory type. The problem is that you cant write to it directly from the CPU.

To send data into GPU only memory, you need to first copy your data into a CPU writeable buffer, encode a copy command in a `VkCommandBuffer`, and then submit that command buffer to a queue. This will enqueue the transfer of the memory from your CPU writeable buffer into another buffer, which can be a GPU allocated buffer.

Because this is necessary when dealing with textures, we will implement this copy logic in our upload_mesh function.
This will likely cause an immediate rendering speed-up if you have been trying to load heavy meshes.

## Upload Context
As copying meshes from CPU buffer to GPU buffer wont be the only thing we are going to do, we are going to create a small abstraction for this sort of short-lived commands. 

Lets begin by adding a new struct to VulkanEngine, and a function for immediate command execution.

```cpp
struct UploadContext {
	VkFence _uploadFence;
	VkCommandPool _commandPool;	
};
```
```cpp
class VulkanEngine {
public:
    UploadContext _uploadContext;

    void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);
}
```

We are going to store the upload related structures in the struct, to keep the amount of objects in VulkanEngine class better organized. 
The `immediate_submit()` function uses an `std::function` in a very similar way as we do in the deletion queue.

Eventually we will add more instant-submit functions, but this one will be the default one. 

We need to initialize that command pool and fence in the upload context.
In `init_structures()`, we will initialize the fence alongside the rendering fences that we have.

```cpp

    VkFenceCreateInfo uploadFenceCreateInfo = vkinit::fence_create_info();

	VK_CHECK(vkCreateFence(_device, &uploadFenceCreateInfo, nullptr, &_uploadContext._uploadFence));
	_mainDeletionQueue.push_function([=]() {
		vkDestroyFence(_device, _uploadContext._uploadFence, nullptr);
	});
```

On this fence we wont set the `VK_FENCE_CREATE_SIGNALED_BIT` flag, as we will not try to wait on it before sending commands like we do in the render loop.

The other thing is the command pool, which we create in `init_commands`

```cpp
	VkCommandPoolCreateInfo uploadCommandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily);
	//create pool for upload context
	VK_CHECK(vkCreateCommandPool(_device, &uploadCommandPoolInfo, nullptr, &_uploadContext._commandPool));

	_mainDeletionQueue.push_function([=]() {
		vkDestroyCommandPool(_device, _uploadContext._commandPool, nullptr);
	});
```

Right now we are creating the pool using the graphics queue family. This is because we will submit the commands to the same queue as the graphics one.

Now that the structure is initialized, lets write the code for the `immediate_submit` function

```cpp

void VulkanEngine::immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function)
{
	//allocate the default command buffer that we will use for the instant commands
	VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_uploadContext._commandPool, 1);

    VkCommandBuffer cmd;
	VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &cmd));

	//begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    //execute the function
	function(cmd);

	VK_CHECK(vkEndCommandBuffer(cmd));

	VkSubmitInfo submit = vkinit::submit_info(&cmd);


	//submit command buffer to the queue and execute it.
	// _uploadFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit(_graphicsQueue, 1, &submit, _uploadContext._uploadFence));

	vkWaitForFences(_device, 1, &_uploadContext._uploadFence, true, 9999999999);
	vkResetFences(_device, 1, &_uploadContext._uploadFence);

    //clear the command pool. This will free the command buffer too
	vkResetCommandPool(_device, _uploadContext._commandPool, 0);
}
```

This is very similar to the logic that we do in the render loop, but with some key differences.
The most important one is that we aren't reusing the same command buffer from frame to frame. Instead, we are allocating it from the pool, and then resetting the pool.
This is done because eventually we will want to be able to upload multiple command buffers per submit, so resetting the entire pool will work well. It also shows that its perfectly fine to allocate and then free the pool every time.

We first allocate command buffer, we then call the function between begin/end command buffer, and then we submit it. Then we wait for the submit to be finished, and reset the command pool.

With this, we now have a way of instantly executing some commands to the GPU, without dealing with the render loop and other syncronization. This is great for compute calculations, and, if it submitted into a different queue, you could use this from a background thread, separated from the render loop.

## Transferring memory.
Now that we have the system for instant commands, we will rewrite the function for `upload_mesh()` so that it uploads the mesh to a GPU local buffer, for best speed.

First, we need to allocate a CPU side buffer to hold the mesh data before uploading it to the GPU buffer.

```cpp
void VulkanEngine::upload_mesh(Mesh& mesh)
{
	const size_t bufferSize= mesh._vertices.size() * sizeof(Vertex);
	//allocate staging buffer
	VkBufferCreateInfo stagingBufferInfo = {};
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.pNext = nullptr;
	
	stagingBufferInfo.size = bufferSize;	
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	//let the VMA library know that this data should be on CPU RAM
	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

	AllocatedBuffer stagingBuffer;

	//allocate the buffer
	VK_CHECK(vmaCreateBuffer(_allocator, &stagingBufferInfo, &vmaallocInfo,
		&stagingBuffer._buffer,
		&stagingBuffer._allocation,
		nullptr));
}
```

We are creating the stagingBuffer with enough size to hold the mesh data, and giving it the `VK_BUFFER_USAGE_TRANSFER_SRC_BIT` usage flag. This flag tells Vulkan that this buffer will only be used as source for transfer commands. We wont be using the staging buffer for rendering.

We can now copy the mesh data to this buffer

```cpp
//copy vertex data
	void* data;
	vmaMapMemory(_allocator, stagingBuffer._allocation, &data);

	memcpy(data, mesh._vertices.data(), mesh._vertices.size() * sizeof(Vertex));

	vmaUnmapMemory(_allocator, stagingBuffer._allocation);
```

Similar map/unmap buffer logic as always. This is unchanged from the last version of `upload_mesh()`.

With the vertex buffer now in a Vulkan CPU side buffer, we need to create the actual GPU-side buffer.

```cpp
    //allocate vertex buffer
	VkBufferCreateInfo vertexBufferInfo = {};
	vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vertexBufferInfo.pNext = nullptr;
	//this is the total size, in bytes, of the buffer we are allocating
	vertexBufferInfo.size = bufferSize;
	//this buffer is going to be used as a Vertex Buffer
	vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	//let the VMA library know that this data should be gpu native	
	vmaallocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	//allocate the buffer
	VK_CHECK(vmaCreateBuffer(_allocator, &vertexBufferInfo, &vmaallocInfo,
		&mesh._vertexBuffer._buffer,
		&mesh._vertexBuffer._allocation,
		nullptr));
```
We give the buffer the `VK_BUFFER_USAGE_VERTEX_BUFFER_BIT` and `VK_BUFFER_USAGE_TRANSFER_DST_BIT` usage flags, so that the drivers knows that we will use this to render meshes, and to copy data into. We also make sure that VMA allocates it on GPU VRAM by using the `VMA_MEMORY_USAGE_GPU_ONLY` memory type.

With the buffers created, we can now execute the copy command.

```cpp
immediate_submit([=](VkCommandBuffer cmd) {
		VkBufferCopy copy;
		copy.dstOffset = 0;
		copy.srcOffset = 0;
		copy.size = bufferSize;
		vkCmdCopyBuffer(cmd, stagingBuffer._buffer, mesh._vertexBuffer._buffer, 1, & copy);
	});
```

We use the immediate_submit function to enqueue a `vkCmdCopyBuffer()` command. this command will copy regions of one buffer into another buffer, using `VkBufferCopy` for each region detail. In here, we are copying the entire staging buffer into the vertex buffer.

With the memory now uploaded, we can clean up.

```cpp
//add the destruction of mesh buffer to the deletion queue
_mainDeletionQueue.push_function([=]() {
	vmaDestroyBuffer(_allocator, mesh._vertexBuffer._buffer, mesh._vertexBuffer._allocation);
	});

vmaDestroyBuffer(_allocator, stagingBuffer._buffer, stagingBuffer._allocation);
```

For the GPU vertex buffer, we will add it to the deletion queue, but for the staging buffer, we will instantly delete it as we are done with it.

Try to run the engine now, everything should work completely fine.

With this new mesh loading code, you will have great performance even uploading meshes that are tens of millions of triangles.

All the logic for copying data is done for meshes, so its time to continue with textures.



Next: [Vulkan Images]({{ site.baseurl }}{% link docs/chapter-5/vulkan_images.md %})


{% include comments.html term="Chapter 5 Comments" %}