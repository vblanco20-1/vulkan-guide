#pragma once

#include <vector>
#include <vk_types.h>
#include <deque>

struct DescriptorLayoutBuilder {

    std::vector<VkDescriptorSetLayoutBinding> bindings;

    void add_binding(uint32_t binding, VkDescriptorType type);
    void clear();
    VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shaderStages);
};

struct DescriptorWriter {
    std::deque<VkDescriptorImageInfo> imageInfos;
    std::deque<VkDescriptorBufferInfo> bufferInfos;
    std::vector<VkWriteDescriptorSet> writes;

    void write_image(int binding,VkImageView image,VkSampler sampler , VkImageLayout layout, VkDescriptorType type);
    void write_buffer(int binding,VkBuffer buffer,size_t size, size_t offset,VkDescriptorType type); 

    void clear();
    void build(VkDevice device, VkDescriptorSet set);
};

struct DescriptorAllocator {

    VkDescriptorPool pool;

    void init_pool(VkDevice device, uint32_t maxSets, std::vector<VkDescriptorPoolSize> poolSizes);
    void clear_descriptors(VkDevice device);

    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
};
