// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vk_engine.h>

namespace vkutil {

	struct MipmapInfo {
		size_t dataSize;
		size_t dataOffset;
	};

	bool load_image_from_file(VulkanEngine& engine, const char* file, AllocatedImage& outImage);	
	bool load_image_from_asset(VulkanEngine& engine, const char* file, AllocatedImage& outImage);


	AllocatedImage upload_image(int texWidth, int texHeight, VkFormat image_format, VulkanEngine& engine, AllocatedBufferUntyped& stagingBuffer);

	AllocatedImage upload_image_mipmapped(int texWidth, int texHeight, VkFormat image_format, VulkanEngine& engine, AllocatedBufferUntyped& stagingBuffer, std::vector<MipmapInfo> mips);
}