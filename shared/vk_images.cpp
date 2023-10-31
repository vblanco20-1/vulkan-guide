#include <vk_images.h>
#include <vk_initializers.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//> transition
void vkutil::transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier2 imageBarrier {};
    imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    imageBarrier.pNext = nullptr;

    imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

    imageBarrier.oldLayout = currentLayout;
    imageBarrier.newLayout = newLayout;

    VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    imageBarrier.subresourceRange = vkinit::image_subresource_range(aspectMask);
    imageBarrier.image = image;

    VkDependencyInfo depInfo {};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.pNext = nullptr;

    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers = &imageBarrier;

    vkCmdPipelineBarrier2(cmd, &depInfo);
}
//< transition

void vkutil::copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent3D imageSize)
{
    VkImageSubresourceRange subImage {};
    subImage.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subImage.baseMipLevel = 0;
    subImage.levelCount = 1;
    subImage.baseArrayLayer = 0;
    subImage.layerCount = 1;

    VkImageCopy2 copyregion {};
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

    VkCopyImageInfo2 copyInfo {};
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
