#pragma once

#include <vk_types.h>
#include <vector>

struct DescriptorLayoutBuilder {

	std::vector<VkDescriptorSetLayoutBinding> bindings;

	void add_binding(uint32_t binding, VkDescriptorType type);
	void clear();
	VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shaderStages);
};

struct DescriptorAllocator {

	VkDescriptorPool pool;

	void init_pool(VkDevice device,uint32_t maxSets,std::vector<VkDescriptorPoolSize> poolSizes);
	void clear_descriptors(VkDevice device);

	VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
};