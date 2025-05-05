---
layout: default
title: Improving the render loop
parent:  "2. Drawing with Compute"
nav_order: 1
---

Before we begin drawing, we need to implement a couple of other things. First we have a deletion queue that will allow us to safely handle the cleanup of a growing amount of objects, and then we will change the render loop to draw into a non-swapchain image and then copy it to the swapchain.


## Deletion queue
As we begin to add more and more vulkan structures, we need a way to handle their destruction. We could keep adding more things into the `cleanup()` function, but that would not scale and would be very annoying to keep synced correctly. 
We are going to add a new structure to the engine, called a `DeletionQueue`. This is a common approach used by lots of engines, where we add the objects we want to delete into some queue, and then run that queue to delete all the objects in the correct order.
In our implementation, we are going to keep it simple, and have it store `std::function` callbacks in a `std::deque`. We will be using that deque as a First In Last Out queue, so that when we flush the deletion queue, it first destroys the objects that were added into it last.

This is the entire implementation.

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
			(*it)(); //call functors
		}

		deletors.clear();
	}
};
```

`std::function` stores a lambda, and we can use it to store a callback with some data, which is perfect for this case. 

Doing callbacks like this is inefficient at scale, because we are storing whole `std::function`s for every object we are deleting, which is not going to be optimal. For the amount of objects we will use in this tutorial, it's going to be fine. If you need to delete thousands of objects and want them deleted faster, a better implementation would be to store arrays of vulkan handles of various types such as `VkImage`, `VkBuffer`, and so on, and then delete those from a loop.

We will have the deletion queue in multiple places, for multiple lifetimes of objects. One of them is on the engine class itself, and will be flushed when the engine gets destroyed. Global objects go into that one. We will also store one deletion queue for each frame in flight, which will allow us to delete objects on the next frame, after they are used. 

Add one to the main `VulkanEngine` class, and one to the `FrameData` struct

```cpp
struct FrameData {
	 //other data
      DeletionQueue _deletionQueue;
};

class VulkanEngine{
    //other data
    DeletionQueue _mainDeletionQueue;
}

```

We then call it from two places, right after we wait on the Fence per frame, and from the `cleanup` function after the `WaitIdle` call. By flushing it right after the fence, we make sure that the GPU has finished executing that frame so we can safely delete objects created for that specific frame only. We also want to make sure we free those per-frame resources when destroying the rest of frame data.

```cpp
void VulkanEngine::draw()
{
	//wait until the gpu has finished rendering the last frame. Timeout of 1 second
	VK_CHECK(vkWaitForFences(_device, 1, &get_current_frame()._renderFence, true, 1000000000));

	get_current_frame()._deletionQueue.flush();

    //other code
}

void VulkanEngine::cleanup()
{	
	if (_isInitialized) {
		
		//make sure the gpu has stopped doing its things				
		vkDeviceWaitIdle(_device);
		
		//free per-frame structures and deletion queue
		for (int i = 0; i < FRAME_OVERLAP; i++) {

			vkDestroyCommandPool(_device, _frames[i]._commandPool, nullptr);

			//destroy sync objects
			vkDestroyFence(_device, _frames[i]._renderFence, nullptr);
			vkDestroySemaphore(_device, _frames[i]._renderSemaphore, nullptr);
			vkDestroySemaphore(_device, _frames[i]._swapchainSemaphore, nullptr);

			_frames[i]._deletionQueue.flush();
		}

		//flush the global deletion queue
		_mainDeletionQueue.flush();

		//rest of cleanup function
	}
}
```

With the deletion queue set, now whenever we create new vulkan objects we can just add them into the queue.

## Memory Allocation

To improve the render loop, we will need to allocate an image, and this gets us into how to allocate objects in Vulkan. We are going to skip that entire chapter, because we will be using the Vulkan Memory Allocator library. Dealing with the different memory heaps and object restrictions such as image alignment is very error prone and really hard to get right, especially if you want to get it right at a decent level of performance. By using VMA, we skip all that, and we get a battle tested way that is guaranteed to work well. There are cases like the RPCS3 emulator project, where they replaced their attempt at allocation with VMA, and won 20% extra framerate. 

`vk_types.h` already holds the include needed for the VMA library, but we also need to include it in `vk_engine.cpp`, this time with `VMA_IMPLEMENTATION` defined.

```cpp
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
```

VMA holds both the normal header and the implementation of the functions in the same file. We need to define `VMA_IMPLEMENTATION` in exactly one of the .cpp files of our project, and that will store and compile the definitions for the VMA functions. 

Add the allocator to the `VulkanEngine` class
```cpp
class VulkanEngine{

    VmaAllocator _allocator;
}
```

Now we will initialize it from inside `init_vulkan`, at the end of the function.

<!-- codegen from tag vma_init on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
    // initialize the memory allocator
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = _chosenGPU;
    allocatorInfo.device = _device;
    allocatorInfo.instance = _instance;
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    vmaCreateAllocator(&allocatorInfo, &_allocator);

    _mainDeletionQueue.push_function([&]() {
        vmaDestroyAllocator(_allocator);
    });
```

There isn't much to explain, we are initializing the `_allocator` member, and then adding its destruction function into the destruction queue so that it gets cleared when the engine exits. We hook the physical device, instance, and device to the creation function. We give the flag `VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT` that will let us use GPU pointers later when we need them.
The Vulkan Memory Allocator library follows similar call conventions to the Vulkan API, so it works with similar info structs.

# New draw loop.

Drawing directly into the swapchain is fine for many projects, and it can even be optimal in some cases such as phones, but it comes with a few restrictions. 
The most important of them is that the formats of the image used in the swapchain are not guaranteed. Different OS, drivers, and windowing modes can have different optimal swapchain formats. Things like HDR support also need their own very specific formats.
Another restriction is that we only get a swapchain image index from the windowing present system. There are techniques where we could be rendering into another image, and then directly push that image to the swapchain with very low latency. 

One very important limitation is that their resolution is fixed to whatever your window size is. If you want to have higher or lower resolution, and then do some scaling logic, you need to draw into a different image.

And last, swapchain formats are, for the most part, low precision. Some platforms with High Dynamic Range rendering have higher precision formats, but you will often default to 8 bits per color. So if you want high precision light calculations, a system that would prevent banding, or to be able to go past 1.0 on the normalized color range, you will need a separate image for drawing.

For all those reasons, we will do the whole tutorial rendering into an image separate from the swapchain one. After we are done drawing, we will copy that image into the swapchain image and present it to the screen.

The image we will be using is going to be in the RGBA 16-bit float format. This is slightly overkill, but will provide us with a lot of extra precision that will come in handy when doing lighting calculations and better rendering.


# Vulkan Images
We have already dealt superficially with images when setting up the swapchain, but it was handled by VkBootstrap. This time we will create the images ourselves.


Let's begin by adding the new members we will need to the `VulkanEngine` class.

On `vk_types.h`, add this structure which holds the data needed for an image. We will hold a `VkImage` alongside its default `VkImageView`, then the allocation for the image memory, and last, the image's size and its format, which will be useful when working with it. We also add a `_drawExtent` member that we can use to decide what size to render.

```cpp
struct AllocatedImage {
    VkImage image;
    VkImageView imageView;
    VmaAllocation allocation;
    VkExtent3D imageExtent;
    VkFormat imageFormat;
};
```

```cpp
class VulkanEngine{

	//draw resources
	AllocatedImage _drawImage;
	VkExtent2D _drawExtent;
}
```

Let's check the vk_initializers function for image and imageview create info.

<!-- codegen from tag image_set on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_initializers.cpp --> 
```cpp
VkImageCreateInfo vkinit::image_create_info(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent)
{
    VkImageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;

    info.imageType = VK_IMAGE_TYPE_2D;

    info.format = format;
    info.extent = extent;

    info.mipLevels = 1;
    info.arrayLayers = 1;

    //for MSAA. we will not be using it by default, so default it to 1 sample per pixel.
    info.samples = VK_SAMPLE_COUNT_1_BIT;

    //optimal tiling, which means the image is stored on the best gpu format
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usageFlags;

    return info;
}

VkImageViewCreateInfo vkinit::imageview_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags)
{
    // build a image-view for the depth image to use for rendering
    VkImageViewCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.pNext = nullptr;

    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.image = image;
    info.format = format;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;
    info.subresourceRange.aspectMask = aspectFlags;

    return info;
}
```

We will hardcode the image tiling to OPTIMAL, which means that we allow the GPU to shuffle the data however it sees fit. If we want to read the image data from the CPU, we would need to use LINEAR tiling, which turns the GPU data into a simple 2D array. This tiling mode highly limits what the GPU can do, so the only real use case for LINEAR is CPU readback.

On the imageview creation, we need to set up the subresource. That's similar to the one we used in the pipeline barrier.

Now, at the end of init_swapchain, lets create it.

<!-- codegen from tag init_swap on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
	//draw image size will match the window
	VkExtent3D drawImageExtent = {
		_windowExtent.width,
		_windowExtent.height,
		1
	};

	//hardcoding the draw format to 32 bit float
	_drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
	_drawImage.imageExtent = drawImageExtent;

	VkImageUsageFlags drawImageUsages{};
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	VkImageCreateInfo rimg_info = vkinit::image_create_info(_drawImage.imageFormat, drawImageUsages, drawImageExtent);

	//for the draw image, we want to allocate it from gpu local memory
	VmaAllocationCreateInfo rimg_allocinfo = {};
	rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	//allocate and create the image
	vmaCreateImage(_allocator, &rimg_info, &rimg_allocinfo, &_drawImage.image, &_drawImage.allocation, nullptr);

	//build a image-view for the draw image to use for rendering
	VkImageViewCreateInfo rview_info = vkinit::imageview_create_info(_drawImage.imageFormat, _drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

	VK_CHECK(vkCreateImageView(_device, &rview_info, nullptr, &_drawImage.imageView));

	//add to deletion queues
	_mainDeletionQueue.push_function([=]() {
		vkDestroyImageView(_device, _drawImage.imageView, nullptr);
		vmaDestroyImage(_allocator, _drawImage.image, _drawImage.allocation);
	});
```

We begin by creating a `VkExtent3d` structure with the size of the image we want, which will match our window size. We copy it into the `AllocatedImage`.

Then, we need to fill our usage flags. In Vulkan, all images and buffers must have UsageFlags describing what they will be used for, as this allows the driver to perform optimizations in the background. In our case, we want TransferSRC and TransferDST so that we can copy from and into the image, Storage because that's the "compute shader can write to it" layout, and Color Attachment so that we can use graphics pipelines to draw geometry into it.

The format is going to be `VK_FORMAT_R16G16B16A16_SFLOAT`. This is 16 bit floats for all 4 channels, and will use 64 bits per pixel. That's a fair amount of data, two times what an 8 bit color image uses, but it's going to be useful.

When creating the image itself, we need to send the image info and an alloc info to VMA. VMA will do the Vulkan create calls for us and directly give us the Vulkan image. 
The interesting thing in here is Usage and the required memory flags.
With `VMA_MEMORY_USAGE_GPU_ONLY` usage, we are letting VMA know that this is a GPU texture that won't ever be accessed from the CPU, which lets it put it into GPU VRAM. To make extra sure of that, we are also setting `VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT` as a memory flag. This is a flag that only GPU-side VRAM has, and guarantees the fastest access.

In Vulkan, there are multiple memory regions we can allocate images and buffers from. PC implementations with dedicated GPUs will generally have a CPU RAM region, a GPU VRAM region, and an "upload heap" which is a special region of GPU VRAM that allows CPU writes. If you have resizable BAR enabled, the upload heap can be the entire GPU VRAM. Otherwise, it will be much smaller, generally only 256 megabytes. We tell VMA to put it on `GPU_ONLY` which will prioritize it to be on the GPU VRAM, but outside of that upload heap region.

With the image allocated, we create an imageview to pair it with. In Vulkan, you need an imageview to access images. This is generally a thin wrapper over the image itself that lets you do things like limit access to only one mipmap. We will always be pairing `VkImage`s with their "default" imageview in this tutorial.

# New draw loop

Now that we have a new draw image, let's add it to the render loop.

We will need a way to copy images, so add this into `vk_images.cpp`

<!-- codegen from tag copyimg on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_images.cpp --> 
```cpp
void vkutil::copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize)
{
	VkImageBlit2 blitRegion{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

	blitRegion.srcOffsets[1].x = srcSize.width;
	blitRegion.srcOffsets[1].y = srcSize.height;
	blitRegion.srcOffsets[1].z = 1;

	blitRegion.dstOffsets[1].x = dstSize.width;
	blitRegion.dstOffsets[1].y = dstSize.height;
	blitRegion.dstOffsets[1].z = 1;

	blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.srcSubresource.baseArrayLayer = 0;
	blitRegion.srcSubresource.layerCount = 1;
	blitRegion.srcSubresource.mipLevel = 0;

	blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.dstSubresource.baseArrayLayer = 0;
	blitRegion.dstSubresource.layerCount = 1;
	blitRegion.dstSubresource.mipLevel = 0;

	VkBlitImageInfo2 blitInfo{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr };
	blitInfo.dstImage = destination;
	blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	blitInfo.srcImage = source;
	blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	blitInfo.filter = VK_FILTER_LINEAR;
	blitInfo.regionCount = 1;
	blitInfo.pRegions = &blitRegion;

	vkCmdBlitImage2(cmd, &blitInfo);
}
```
 
Also add the corresponding declaration to `vk_images.h`
```cpp
	namespace vkutil {

	    // Existing:
	    void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);

		void copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize);	
	}
```

Vulkan has two main ways of copying one image into another: `VkCmdCopyImage` or `VkCmdBlitImage`.
CopyImage is faster, but it's much more restricted, for example the resolution on both images must match.
Meanwhile, blit image lets you copy images of different formats and different sizes into one another. You have a source rectangle and a target rectangle, and the system copies it into position. Those two functions are useful when setting up the engine, but later it's best to ignore them and write your own version that can perform extra logic in a fullscreen fragment shader.

With it, we can now update the render loop. As `draw` is getting too big, we are going to leave the synchronization, command buffer management, and transitions in the `draw` function, and we are going to add the draw commands themselves in a `draw_background` function.

```cpp
void VulkanEngine::draw_background(VkCommandBuffer cmd)
{
	//make a clear-color from frame number. This will flash with a 120 frame period.
	VkClearColorValue clearValue;
	float flash = std::abs(std::sin(_frameNumber / 120.f));
	clearValue = { { 0.0f, 0.0f, flash, 1.0f } };

	VkImageSubresourceRange clearRange = vkinit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);

	//clear image
	vkCmdClearColorImage(cmd, _drawImage.image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
}
```

Add the function to the header too.

We will be changing the code that records the command buffer. You can now delete the older one. 
The new code is this.
```cpp
	_drawExtent.width = _drawImage.imageExtent.width;
	_drawExtent.height = _drawImage.imageExtent.height;

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));	

	// transition our main draw image into general layout so we can write into it
	// we will overwrite it all so we dont care about what was the older layout
	vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	draw_background(cmd);

	//transition the draw image and the swapchain image into their correct transfer layouts
	vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// execute a copy from the draw image into the swapchain
	vkutil::copy_image_to_image(cmd, _drawImage.image, _swapchainImages[swapchainImageIndex], _drawExtent, _swapchainExtent);

	// set swapchain image layout to Present so we can show it on the screen
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	//finalize the command buffer (we can no longer add commands, but it can now be executed)
	VK_CHECK(vkEndCommandBuffer(cmd));
```

The main difference we have in the render loop is that we no longer clear the swapchain image. Instead, we clear the draw image. Once we have cleared the image, we transition both the swapchain and the draw image into their layouts for transfer, and we execute the copy command. Once we are done with the copy command, we transition the swapchain image into present layout for display. As we are always drawing on the same image, our `draw_background` function does not need to access a swapchain image index, it just clears the draw image. We are also setting the `_drawExtent` that we will use for our draw region.

This will now provide us with a way to render images outside of the swapchain itself. We now get significantly higher pixel precision, and we unlock some other techniques.

With that done, we can now move to the actual compute shader execution steps.

Next: [ Vulkan Shaders]({{ site.baseurl }}{% link docs/new_chapter_2/vulkan_shader_drawing.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}
