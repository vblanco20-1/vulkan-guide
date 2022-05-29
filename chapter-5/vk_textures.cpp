#include <vk_textures.h>
#include <iostream>

#include <vk_initializers.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>



bool vkutil::load_image_from_file(VulkanEngine& engine, const char* file, AllocatedImage & outImage)
{
	int texWidth, texHeight, texChannels;

	stbi_uc* pixels = stbi_load(file, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);	

	if (!pixels) {
		std::cout << "Failed to load texture file " << file << std::endl;
		return false;
	}
	
	void* pixel_ptr = pixels;
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	VkFormat image_format = VK_FORMAT_R8G8B8A8_SRGB;

	AllocatedBuffer stagingBuffer = engine.create_buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	void* data;
	vmaMapMemory(engine._allocator, stagingBuffer._allocation, &data);

	memcpy(data, pixel_ptr, static_cast<size_t>(imageSize));

	vmaUnmapMemory(engine._allocator, stagingBuffer._allocation);

	stbi_image_free(pixels);

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
	
	//transition image to transfer-receiver	
	engine.immediate_submit([&](VkCommandBuffer cmd) {

		vkutil::transition_image(cmd, newImage._image,VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	
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

		vkutil::transition_image(cmd, newImage._image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL);
	});


	engine._mainDeletionQueue.push_function([=]() {
	
		vmaDestroyImage(engine._allocator, newImage._image, newImage._allocation);
	});

	vmaDestroyBuffer(engine._allocator, stagingBuffer._buffer, stagingBuffer._allocation);

	std::cout << "Texture loaded succesfully " << file << std::endl;

	outImage = newImage;
	return true;
}


void vkutil::transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout)
{
	VkImageMemoryBarrier2 imageBarrier{};
	imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	imageBarrier.pNext = nullptr;

	imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

	imageBarrier.oldLayout = currentLayout;
	imageBarrier.newLayout = newLayout;

	VkImageSubresourceRange subImage{};
	subImage.aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	subImage.baseMipLevel = 0;
	subImage.levelCount = 1;
	subImage.baseArrayLayer = 0;
	subImage.layerCount = 1;

	imageBarrier.subresourceRange = subImage;
	imageBarrier.image = image;

	VkDependencyInfo depInfo{};
	depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	depInfo.pNext = nullptr;

	depInfo.imageMemoryBarrierCount = 1;
	depInfo.pImageMemoryBarriers = &imageBarrier;

	vkCmdPipelineBarrier2(cmd, &depInfo);
}

void vkutil::copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent3D imageSize)
{
	VkImageSubresourceRange subImage{};
	subImage.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subImage.baseMipLevel = 0;
	subImage.levelCount = 1;
	subImage.baseArrayLayer = 0;
	subImage.layerCount = 1;

	VkImageCopy2 copyregion{};
	copyregion.sType = VK_STRUCTURE_TYPE_IMAGE_COPY_2;
	copyregion.pNext = nullptr;
	copyregion.extent = imageSize;

	copyregion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyregion.srcSubresource.baseArrayLayer = 0;
	copyregion.srcSubresource.layerCount = 1;
	copyregion.srcSubresource.mipLevel = 0;

	copyregion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyregion.dstSubresource.baseArrayLayer = 0;
	copyregion.dstSubresource.layerCount = 1;
	copyregion.dstSubresource.mipLevel = 0;

	VkCopyImageInfo2 copyInfo{};
	copyInfo.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2;
	copyInfo.pNext = nullptr;
	copyInfo.dstImage = destination;
	copyInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	copyInfo.srcImage = source;
	copyInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

	copyInfo.regionCount = 1;
	copyInfo.pRegions = &copyregion;

	vkCmdCopyImage2(cmd, &copyInfo);
}