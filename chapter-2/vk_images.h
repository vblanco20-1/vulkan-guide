
#include <vk_types.h>
#include <vk_engine.h>

namespace vkutil {
 

	void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);

	void copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent3D imageSize);
}