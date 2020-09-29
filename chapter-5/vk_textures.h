// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vk_engine.h>

namespace vkutil {

	bool load_image_from_file(VulkanEngine& engine, const char* file, AllocatedImage& outImage);

}