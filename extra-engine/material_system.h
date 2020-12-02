// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vector>
#include <array>
#include <unordered_map>
#include <material_asset.h>
#include <vk_mesh.h>

class PipelineBuilder {
public:

	std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
	VertexInputDescription vertexDescription;
	VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
	VkViewport _viewport;
	VkRect2D _scissor;
	VkPipelineRasterizationStateCreateInfo _rasterizer;
	VkPipelineColorBlendAttachmentState _colorBlendAttachment;
	VkPipelineMultisampleStateCreateInfo _multisampling;
	VkPipelineLayout _pipelineLayout;
	VkPipelineDepthStencilStateCreateInfo _depthStencil;
	VkPipeline build_pipeline(VkDevice device, VkRenderPass pass);
	void clear_vertex_input();

	void setShaders(struct ShaderEffect* effect);
};

enum class VertexAttributeTemplate {
	DefaultVertex,
	DefaultVertexPosOnly
};

class EffectBuilder {

	VertexAttributeTemplate vertexAttrib;
	struct ShaderEffect* effect{ nullptr };

	VkPrimitiveTopology topology;
	VkPipelineRasterizationStateCreateInfo rasterizerInfo;
	VkPipelineColorBlendAttachmentState colorBlendAttachmentInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
};

class ComputePipelineBuilder {
public:

	VkPipelineShaderStageCreateInfo  _shaderStage;
	VkPipelineLayout _pipelineLayout;
	VkPipeline build_pipeline(VkDevice device);
};
struct ShaderEffect;
class VulkanEngine;
namespace vkutil {
	
	struct ShaderPass {
		ShaderEffect* effect{ nullptr };
		VkPipeline pipeline{ VK_NULL_HANDLE };
		VkPipelineLayout layout{ VK_NULL_HANDLE };
	};

	//struct ShaderPassInfo {
	//	std::vector<std::string> shaders;
	//	PipelineBuilder builder;
	//};
	struct SampledTexture {
		VkSampler sampler;
		VkImageView view;
	};
	struct ShaderParameters
	{
		
	};
	struct EffectTemplate {
		ShaderPass* forwardEffect;
		ShaderPass* shadowEffect;
		ShaderParameters* defaultParameters;
		assets::TransparencyMode transparency;
	};

	struct MaterialInfo {
		std::vector<SampledTexture> textures;
		ShaderParameters* parameters;
		std::string baseTemplate;

		bool operator==(const MaterialInfo& other) const;

		size_t hash() const;
	};

	struct Material {
		EffectTemplate* original;
		VkDescriptorSet forwardSet{ VK_NULL_HANDLE };
		VkDescriptorSet shadowSet{ VK_NULL_HANDLE };
		std::vector<SampledTexture> textures;

		ShaderParameters* parameters;

		Material& operator=(const Material& other) = default;
	};

	class MaterialSystem {
	public:
		void init(VulkanEngine* owner);
		void cleanup();

		void build_default_templates();

		ShaderPass* build_shader(VkRenderPass renderPass,PipelineBuilder& builder, ShaderEffect* effect);

		Material* build_material(const std::string& materialName, const MaterialInfo& info);
		Material* get_material(const std::string& materialName);
		
		void fill_builders();
	private:

		struct MaterialInfoHash
		{

			std::size_t operator()(const MaterialInfo& k) const
			{
				return k.hash();
			}
		};

		PipelineBuilder forwardBuilder;
		PipelineBuilder shadowBuilder;


		std::unordered_map<std::string, EffectTemplate> templateCache;
		std::unordered_map<std::string, Material*> materials;
		std::unordered_map<MaterialInfo, Material*, MaterialInfoHash> materialCache;
		VulkanEngine* engine;
	};
}

