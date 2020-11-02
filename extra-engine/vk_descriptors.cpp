﻿#include <vk_descriptors.h>
#include <algorithm>

namespace vkutil {


	VkDescriptorPool createPool(VkDevice device, const DescriptorAllocator::PoolSizes& poolSizes, int count, VkDescriptorPoolCreateFlags flags)
	{
		std::vector<VkDescriptorPoolSize> sizes;
		sizes.reserve(poolSizes.sizes.size());
		for (auto sz : poolSizes.sizes) {
			sizes.push_back({ sz.type, uint32_t(sz.multiplier * count) });
		}
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = flags;
		pool_info.maxSets = count;
		pool_info.poolSizeCount = (uint32_t)sizes.size();
		pool_info.pPoolSizes = sizes.data();

		VkDescriptorPool descriptorPool;
		vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool);

		return descriptorPool;
	}

	void DescriptorAllocator::ResetAll()
	{
		for (auto p : usedPools)
		{
			vkResetDescriptorPool(device, p, 0);
		}

		freePools = usedPools;
		usedPools.clear();
		currentPool = VK_NULL_HANDLE;
	}

	bool DescriptorAllocator::AllocateDescriptor(VkDescriptorSet* set, VkDescriptorSetLayout layout)
	{
		if (currentPool == VK_NULL_HANDLE)
		{
			currentPool = GrabPool();//createPool(device, descriptorSizes, 1000, 0);
			usedPools.push_back(currentPool);
		}

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;

		allocInfo.pSetLayouts = &layout;
		allocInfo.descriptorPool = currentPool;
		allocInfo.descriptorSetCount = 1;		
		

		VkResult allocResult = vkAllocateDescriptorSets(device, &allocInfo, set);
		bool needReallocate = false;

		switch (allocResult) {
		case VK_SUCCESS:
			//all good, return
			return true;

			break;
		case VK_ERROR_FRAGMENTED_POOL:
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			//reallocate pool
			needReallocate = true;
			break;
		default:
			//unrecoverable error
			return false;
		}
		
		if (needReallocate)
		{
			//allocate a new pool and retry

			currentPool = GrabPool();//createPool(device, descriptorSizes, 1000, 0);
			usedPools.push_back(currentPool);

			allocResult = vkAllocateDescriptorSets(device, &allocInfo, set);

			//if it still fails then we have big issues
			if (allocResult != VK_SUCCESS)
			{
				return false;
			}
		}

		return false;
	}

	void DescriptorAllocator::Initialize(VkDevice newDevice)
	{
		device = newDevice;
	}

	VkDescriptorPool DescriptorAllocator::GrabPool()
	{
		if (freePools.size() > 0)
		{
			VkDescriptorPool pool = freePools.back();
			freePools.pop_back();
			return pool;
		}
		else {
			return createPool(device, descriptorSizes, 1000, 0);
		}
	}


	void DescriptorLayoutCache::Initialize(VkDevice newDevice)
	{
		device = newDevice;
	}

	VkDescriptorSetLayout DescriptorLayoutCache::CreateDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* info)
	{
		DescriptorLayoutInfo layoutinfo;
		layoutinfo.bindings.reserve(info->bindingCount);
		bool isSorted = true;
		int lastBinding = -1;
		for (int i = 0; i < info->bindingCount; i++) {
			layoutinfo.bindings.push_back(info->pBindings[i]);

			//check that the bindings are in strict increasing order
			if (info->pBindings[i].binding > lastBinding)
			{
				lastBinding = info->pBindings[i].binding;
			}
			else{
				isSorted = false;
			}
		}
		if (!isSorted)
		{
			std::sort(layoutinfo.bindings.begin(), layoutinfo.bindings.end(), [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b ) {
				return a.binding < b.binding;
			});
		}
		
		auto it = layoutCache.find(layoutinfo);
		if (it != layoutCache.end())
		{
			return (*it).second;
		}
		else {
			VkDescriptorSetLayout layout;
			vkCreateDescriptorSetLayout(device, info, nullptr, &layout);

			//layoutCache.emplace()
			//add to cache
			layoutCache[layoutinfo] = layout;
			return layout;
		}
	}
}