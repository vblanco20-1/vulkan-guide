// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vector>
#include <array>
#include <unordered_map>

namespace vkutil {
	

	class DescriptorAllocator {
	public:
		struct PoolSize {
			VkDescriptorType type;
			float multiplier;
		};
		struct PoolSizes {
			std::vector<PoolSize> sizes =
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.f },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
			};
		};



		void ResetAll();
		bool AllocateDescriptor(VkDescriptorSet* set, VkDescriptorSetLayout layout);
		
		void Initialize(VkDevice newDevice);

		VkDevice device;
	private:
		VkDescriptorPool GrabPool();

		VkDescriptorPool currentPool{VK_NULL_HANDLE};
		PoolSizes descriptorSizes;
		std::vector<VkDescriptorPool> usedPools;
		std::vector<VkDescriptorPool> freePools;		
	};


	class DescriptorLayoutCache {
	public:
		void Initialize(VkDevice newDevice);


		VkDescriptorSetLayout CreateDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* info);

		struct DescriptorLayoutInfo {
			//good idea to turn this into a inlined array
			std::vector<VkDescriptorSetLayoutBinding> bindings;

			bool operator==(const DescriptorLayoutInfo& other) const {
				if (other.bindings.size() != bindings.size())
				{
					return false;
				}
				else {
					for (int i = 0; i < bindings.size(); i++) {
						if (other.bindings[i].binding != bindings[i].binding)
						{
							return false;
						}
						if (other.bindings[i].descriptorType != bindings[i].descriptorType)
						{
							return false;
						}
						if (other.bindings[i].descriptorCount != bindings[i].descriptorCount)
						{
							return false;
						}
						if (other.bindings[i].stageFlags != bindings[i].stageFlags)
						{
							return false;
						}
					}
					return true;
				}
			}
		};
	private:

		struct DescriptorLayoutHash
		{

			std::size_t operator()(const DescriptorLayoutInfo& k) const
			{
				using std::size_t;
				using std::hash;

				size_t result = hash<size_t>()(k.bindings.size());

				for (const VkDescriptorSetLayoutBinding& b : k.bindings)
				{
					//pack the binding data into a single int64. Not fully correct but its ok
					size_t binding_hash = b.binding | b.descriptorType << 8 |  b.descriptorCount << 16 | b.stageFlags << 24;
					
					//shuffle the packed binding data and xor it with the main hash
					result ^= hash<size_t>()(binding_hash);
				}

				return result;
			}
		};		

		std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash> layoutCache;
		VkDevice device;
	};


	class DescriptorBuilder {
	public:

		static DescriptorBuilder begin(DescriptorLayoutCache* layoutCache, DescriptorAllocator* allocator );

		DescriptorBuilder& bind_buffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo, VkDescriptorType type, VkShaderStageFlags stageFlags);

		DescriptorBuilder& bind_image(uint32_t binding, VkDescriptorImageInfo* imageInfo, VkDescriptorType type, VkShaderStageFlags stageFlags);

		bool build(VkDescriptorSet& set, VkDescriptorSetLayout& layout);
		bool build(VkDescriptorSet& set);
	private:
		
		std::vector<VkWriteDescriptorSet> writes;
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		

		DescriptorLayoutCache* cache;
		DescriptorAllocator* alloc;
	};
}

