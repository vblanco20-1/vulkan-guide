// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

struct AllocatedBufferUntyped {
	VkBuffer _buffer{};
	VmaAllocation _allocation{};
	VkDeviceSize _size{0};
	VkDescriptorBufferInfo get_info(VkDeviceSize offset = 0);
};

template<typename T>
struct AllocatedBuffer : public AllocatedBufferUntyped {
	void operator=(const AllocatedBufferUntyped& other) {
		_buffer = other._buffer;
		_allocation = other._allocation;
		_size = other._size;
	}
	AllocatedBuffer(AllocatedBufferUntyped& other) {
		_buffer = other._buffer;
		_allocation = other._allocation;
		_size = other._size;
	}
	AllocatedBuffer() = default;
};

struct AllocatedImage {
	VkImage _image;
	VmaAllocation _allocation;
	VkImageView _defaultView;
	int mipLevels;
};


inline VkDescriptorBufferInfo AllocatedBufferUntyped::get_info(VkDeviceSize offset)
{
	VkDescriptorBufferInfo info;
	info.buffer = _buffer;
	info.offset = offset;
	info.range = _size;
	return info;
}
