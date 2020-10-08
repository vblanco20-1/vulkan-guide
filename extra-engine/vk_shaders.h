// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vector>
#include <array>
struct ShaderModule {
	std::vector<uint32_t> code;
	VkShaderModule module;
};
namespace vkutil {
	

	//loads a shader module from a spir-v file. Returns false if it errors	
	bool load_shader_module(VkDevice device, const char* filePath, ShaderModule* outShaderModule);
}



class VulkanEngine;
//holds all information for a given shader set for pipeline
struct ShaderEffect {

	struct ReflectionOverrides {
		const char* name;
		VkDescriptorType overridenType;
	};


	void add_stage(ShaderModule* shaderModule, VkShaderStageFlagBits stage);

	void reflect_layout(VulkanEngine* engine, ReflectionOverrides* overrides, int overrideCount);
	VkPipelineLayout builtLayout;
private:
	struct ShaderStage {
		ShaderModule* shaderModule;
		VkShaderStageFlagBits stage;
	};

	std::vector<ShaderStage> stages;

	
	std::array<VkDescriptorSetLayout,4> setLayouts;
};