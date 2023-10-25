
#pragma once 

#include <vulkan/vulkan.h>

enum class ImageTransitionMode {
    IntoAttachment,
    IntoGeneral,
    GeneralToPresent,
    AttachmentToPresent
};

namespace vkutil {

void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);

void copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent3D imageSize);
} // namespace vkutil
