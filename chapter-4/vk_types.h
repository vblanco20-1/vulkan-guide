// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

//we will add our main reusable types here
struct AllocatedImage {
	VkImage _image;
	VkImageView _imageView;
	VmaAllocation _allocation;
	VkExtent3D _imageExtent;
	VkFormat _imageFormat;
};

struct AllocatedBuffer {
	VkBuffer _buffer;
	VmaAllocation _allocation;
	VmaAllocationInfo _info;
};