---
layout: default
title: Loading Images
parent: "5. Textures"
nav_order: 14
---

Let's start the code to upload and handle textures on the engine.

We will begin by creating another pair of files. Like we did with the mesh. vk_textures.h and .cpp. We will store the main code for loading textures in there.

vk_textures.h
```cpp
#pragma once

#include <vk_types.h>
#include <vk_engine.h>

namespace vkutil {

	bool load_image_from_file(VulkanEngine& engine, const char* file, AllocatedImage& outImage);

}
```
We are going to have a single load-image function, that will do the whole load operation from file in disk into GPU memory.

We will use the stb_image library to load the texture.


On vk_textures.cpp, we begin filling the function.

```cpp
#include <vk_textures.h>
#include <iostream>

#include <vk_initializers.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

bool vkutil::load_image_from_file(VulkanEngine& engine, const char* file, AllocatedImage& outImage)
{
	int texWidth, texHeight, texChannels;

	stbi_uc* pixels = stbi_load(file, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	if (!pixels) {
		std::cout << "Failed to load texture file " << file << std::endl;
		return false;
	}

    return true;
}
```

At the start, we use `stbi_load()` to load a texture directly from file into a CPU array of pixels. The function will return nullptr if it doesn't find the file, or if there are errors.
When loading the function, we also send `STBI_rgb_alpha` to the function, which will make the library always load the pixels as RGBA 4 channels. This is useful as it will match with the format we will use for Vulkan.

With the texture file loaded into the pixels array, we can create a staging buffer and store the pixels there. This is almost the same as what we did in the last article when copying meshes to the GPU.


```cpp
    void* pixel_ptr = pixels;
	VkDeviceSize imageSize = texWidth * texHeight * 4;

    //the format R8G8B8A8 matches exactly with the pixels loaded from stb_image lib
	VkFormat image_format = VK_FORMAT_R8G8B8A8_SRGB;

    //allocate temporary buffer for holding texture data to upload
	AllocatedBuffer stagingBuffer = engine.create_buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

    //copy data to buffer
	void* data;
	vmaMapMemory(engine._allocator, stagingBuffer._allocation, &data);

	memcpy(data, pixel_ptr, static_cast<size_t>(imageSize));

	vmaUnmapMemory(engine._allocator, stagingBuffer._allocation);
    //we no longer need the loaded data, so we can free the pixels as they are now in the staging buffer
	stbi_image_free(pixels);
```

We calculate image sizes by doing 4 bytes per pixel, and `texWidth * texHeight` number of pixels.
For the format, we match it with the stb format `STBI_rgb_alpha`. And then we create a buffer to hold that data and copy it there.
Once the data is copied into the buffer, we no longer need the original library loaded pixels, so we can free it.

We now continue creating the image

```cpp
VkExtent3D imageExtent;
	imageExtent.width = static_cast<uint32_t>(texWidth);
	imageExtent.height = static_cast<uint32_t>(texHeight);
	imageExtent.depth = 1;

	VkImageCreateInfo dimg_info = vkinit::image_create_info(image_format, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageExtent);

	AllocatedImage newImage;

	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	//allocate and create the image
	vmaCreateImage(engine._allocator, &dimg_info, &dimg_allocinfo, &newImage._image, &newImage._allocation, nullptr);

```

This is similar to the time where we created the depth image. The main difference is the usage flags of the image, which will be Sampled and Transfer Destination, as we will use this as just a texture for the shaders.
We use VMA with GPU_ONLY memory type so that the image is allocated on VRAM

With the image created and the buffer ready, we now can start the commands to copy the data into it.

With images, you cant just copy the data from the buffer into the image directly. Right now, the image is not initialized in any specific layout, so we need to do a layout transition so that the driver puts the texture into Linear layout, which is the best for copying data from a buffer into a texture.

To do the layout transition, we do this

```cpp
engine.immediate_submit([&](VkCommandBuffer cmd) {
		VkImageSubresourceRange range;
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		range.baseArrayLayer = 0;
		range.layerCount = 1;

		VkImageMemoryBarrier imageBarrier_toTransfer = {};
		imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

		imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier_toTransfer.image = newImage._image;
		imageBarrier_toTransfer.subresourceRange = range;

		imageBarrier_toTransfer.srcAccessMask = 0;
		imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		//barrier the image into the transfer-receive layout
		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);
});
```

To perform layout transitions, we need to use pipeline barriers. Pipeline barriers can control how the GPU overlaps commands before and after the barrier, but if you do pipeline barriers with image barriers, the driver can also transform the image to the correct formats and layouts.

In here, We start with a `VkImageSubresourceRange`, to tell it what part of the image we will transform. In here we don't have mipmaps or layered textures, so we have 1 for level count and for layer count.

Next, we fill `VkImageMemoryBarrier` with the layout transition. We begin with layout `VK_IMAGE_LAYOUT_UNDEFINED`, and then go to `VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL`. This prepares the image into the layout ready to be the destination of memory transfers.

We point the image and the subresource range too.

With that filled, we can do the pipeline barrier. It will be a barrier that blocks from `VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT` to `VK_PIPELINE_STAGE_TRANSFER_BIT`. If you want to learn more about the exact specific of pipeline barrier stages, read this. https://gpuopen.com/learn/vulkan-barriers-explained/
We will not need to know the specific details here for what we are doing.

With the image ready to receive pixel data, we can now transfer with a command. We continue filling the `immediate_submit()` call

```cpp
    VkBufferImageCopy copyRegion = {};
	copyRegion.bufferOffset = 0;
	copyRegion.bufferRowLength = 0;
	copyRegion.bufferImageHeight = 0;

	copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.imageSubresource.mipLevel = 0;
	copyRegion.imageSubresource.baseArrayLayer = 0;
	copyRegion.imageSubresource.layerCount = 1;
	copyRegion.imageExtent = imageExtent;

	//copy the buffer into the image
	vkCmdCopyBufferToImage(cmd, stagingBuffer._buffer, newImage._image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
```

Like when we copied the buffers, we need to fill a struct containing the information of what to copy. We have the buffer just at offset 0, and then what exact layer and mipmap we want to copy into, which is just level 0 and 1 layer.
We also need to send imageExtent for the image size.

We now execute the `VkCmdCopyBufferToImage()` command, where we also need to specify whats the layout of the image, which is `TRANSFER_DST_OPTIMAL`

The image now has the correct pixel data, so we can change its layout one more time to make it into a shader readable layout.


```cpp
    VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

	imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	//barrier the image into the shader readable layout
	vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toReadable);
```

We are transferring the image into `VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL`, to let the driver know to shuffle the image into whatever internal format works best for reading from shaders.

We now finish the load function by releasing the resources we don't need and adding the image to deletion queue.

```cpp
bool vkutil::load_image_from_file(VulkanEngine& engine, const char* file, AllocatedImage & outImage)
{
    // file load code

    // staging buffer copy

    engine.immediate_submit([&](VkCommandBuffer cmd) {
        //transitions and copy commands
    });


    engine._mainDeletionQueue.push_function([=]() {

		vmaDestroyImage(engine._allocator, newImage._image, newImage._allocation);
	});

	vmaDestroyBuffer(engine._allocator, stagingBuffer._buffer, stagingBuffer._allocation);

	std::cout << "Texture loaded successfully " << file << std::endl;

	outImage = newImage;
	return true;
}
```

We can now load many file formats like .jpeg and .png into a texture. Let's try it with one from the assets folder to make sure that it works.

Let's add a way to load and store the images on VulkanEngine

vulkan_engine.h
```cpp
struct Texture {
	AllocatedImage image;
	VkImageView imageView;
};

class VulkanEngine {
public:
//texture hashmap
std::unordered_map<std::string, Texture> _loadedTextures;

void load_images();
}
```
vk_engine.cpp
```cpp
//add the textures.h include
#include "vk_textures.h"

void VulkanEngine::load_images()
{
	Texture lostEmpire;

	vkutil::load_image_from_file(*this, "../../assets/lost_empire-RGBA.png", lostEmpire.image);

	VkImageViewCreateInfo imageinfo = vkinit::imageview_create_info(VK_FORMAT_R8G8B8A8_SRGB, lostEmpire.image._image, VK_IMAGE_ASPECT_COLOR_BIT);
	vkCreateImageView(_device, &imageinfo, nullptr, &lostEmpire.imageView);

	_loadedTextures["empire_diffuse"] = lostEmpire;
}
```
Make sure you call load_images before setup_scene in the `init` function

With the texture now loaded, we need to display it into the shader, refactoring a few things.



Next: [Drawing images]({{ site.baseurl }}{% link docs/chapter-5/drawing_images.md %})


{% include comments.html term="Chapter 5 Comments" %}



