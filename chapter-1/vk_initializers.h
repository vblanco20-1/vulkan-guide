// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>

namespace vkinit {

	VkCommandPoolCreateInfo command_pool_create_info(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);

	VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool pool, uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	VkCommandBufferBeginInfo command_buffer_begin_info(VkCommandBufferUsageFlags flags = 0);

	VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0);

	VkSemaphoreCreateInfo semaphore_create_info(VkSemaphoreCreateFlags flags = 0);

	VkSubmitInfo submit_info(VkCommandBuffer* cmd);

	VkPresentInfoKHR present_info();

	VkRenderingAttachmentInfo color_attachment_info(VkImageView view, VkClearValue clearValue, VkImageLayout layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	VkRenderingInfo rendering_info(VkExtent2D viewport, VkRenderingAttachmentInfo* colorAttachment);
}