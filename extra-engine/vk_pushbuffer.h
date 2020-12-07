// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>


namespace vkutil {
	struct PushBuffer {

		template<typename T>
		uint32_t push(T& data);

		uint32_t push(void* data, size_t size);

		void init(VmaAllocator& allocator,AllocatedBufferUntyped sourceBuffer, uint32_t alignement);
		void reset();

		uint32_t pad_uniform_buffer_size(uint32_t originalSize);
		AllocatedBufferUntyped source;
		uint32_t align;
		uint32_t currentOffset;
		void* mapped;
	};

	template<typename T>
	uint32_t vkutil::PushBuffer::push(T& data)
	{
		return push(&data, sizeof(T));
	}

}