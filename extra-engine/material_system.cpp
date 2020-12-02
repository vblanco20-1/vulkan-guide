#include "material_system.h"
#include <vk_initializers.h>
#include <vk_shaders.h>
#include "logger.h"
#include "vk_engine.h"

VkPipeline ComputePipelineBuilder::build_pipeline(VkDevice device)
{
	VkComputePipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = nullptr;

	pipelineInfo.stage = _shaderStage;
	pipelineInfo.layout = _pipelineLayout;


	VkPipeline newPipeline;
	if (vkCreateComputePipelines(
		device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS) {
		LOG_FATAL("Failed to build compute pipeline");
		return VK_NULL_HANDLE;
	}
	else
	{
		return newPipeline;
	}
}
VkPipeline PipelineBuilder::build_pipeline(VkDevice device, VkRenderPass pass)
{
	_vertexInputInfo = vkinit::vertex_input_state_create_info();
//connect the pipeline builder vertex input info to the one we get from Vertex
	_vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes.data();
	_vertexInputInfo.vertexAttributeDescriptionCount = vertexDescription.attributes.size();

	_vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings.data();
	_vertexInputInfo.vertexBindingDescriptionCount = vertexDescription.bindings.size();


	//make viewport state from our stored viewport and scissor.
		//at the moment we wont support multiple viewports or scissors
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.pNext = nullptr;

	viewportState.viewportCount = 1;
	viewportState.pViewports = &_viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &_scissor;

	//setup dummy color blending. We arent using transparent objects yet
	//the blending is just "no blend", but we do write to the color attachment
	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.pNext = nullptr;

	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &_colorBlendAttachment;

	//build the actual pipeline
	//we now use all of the info structs we have been writing into into this one to create the pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = nullptr;

	pipelineInfo.stageCount = _shaderStages.size();
	pipelineInfo.pStages = _shaderStages.data();
	pipelineInfo.pVertexInputState = &_vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &_inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &_rasterizer;
	pipelineInfo.pMultisampleState = &_multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDepthStencilState = &_depthStencil;
	pipelineInfo.layout = _pipelineLayout;
	pipelineInfo.renderPass = pass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;


	std::vector<VkDynamicState> dynamicStates;
	dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
	dynamicStates.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
	dynamicState.pDynamicStates = dynamicStates.data();
	dynamicState.dynamicStateCount = dynamicStates.size();

	pipelineInfo.pDynamicState = &dynamicState;

	//its easy to error out on create graphics pipeline, so we handle it a bit better than the common VK_CHECK case
	VkPipeline newPipeline;
	if (vkCreateGraphicsPipelines(
		device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS) {
		LOG_FATAL("Failed to build graphics pipeline");
		return VK_NULL_HANDLE;
	}
	else
	{
		return newPipeline;
	}
}


void PipelineBuilder::clear_vertex_input()
{
	_vertexInputInfo.pVertexAttributeDescriptions = nullptr;
	_vertexInputInfo.vertexAttributeDescriptionCount = 0;

	_vertexInputInfo.pVertexBindingDescriptions = nullptr;
	_vertexInputInfo.vertexBindingDescriptionCount = 0;
}

void PipelineBuilder::setShaders(ShaderEffect* effect)
{
	_shaderStages.clear();
	effect->fill_stages(_shaderStages);

	_pipelineLayout = effect->builtLayout;
}

void vkutil::MaterialSystem::init(VulkanEngine* owner)
{
	engine = owner;
	build_default_templates();
}

void vkutil::MaterialSystem::cleanup()
{

}

ShaderEffect* build_effect(VulkanEngine* eng,std::string_view vertexShader, std::string_view fragmentShader) {
	ShaderEffect::ReflectionOverrides overrides[] = {
		{"sceneData", VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC},
		{"cameraData", VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC}
	};
	//textured defaultlit shader
	ShaderEffect* effect = new ShaderEffect();
	
	effect->add_stage(eng->_shaderCache.get_shader(VulkanEngine::shader_path(vertexShader)), VK_SHADER_STAGE_VERTEX_BIT);
	if (fragmentShader.size() > 2)
	{
		effect->add_stage(eng->_shaderCache.get_shader(VulkanEngine::shader_path(fragmentShader)), VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	

	effect->reflect_layout(eng, overrides, 2);

	return effect;
}

void vkutil::MaterialSystem::build_default_templates()
{
	fill_builders();

	//default effects	
	ShaderEffect* texturedLit = build_effect(engine,  "tri_mesh_ssbo_instanced.vert.spv" ,"textured_lit.frag.spv" );
	ShaderEffect* defaultLit = build_effect(engine, "tri_mesh_ssbo_instanced.vert.spv" , "default_lit.frag.spv" );
	ShaderEffect* opaqueShadowcast = build_effect(engine, "tri_mesh_ssbo_instanced_shadowcast.vert.spv","");

	//passes
	ShaderPass* texturedLitPass = build_shader(engine->_renderPass,forwardBuilder, texturedLit);
	ShaderPass* defaultLitPass = build_shader(engine->_renderPass, forwardBuilder, defaultLit);
	ShaderPass* opaqueShadowcastPass = build_shader(engine->_shadowPass,shadowBuilder, opaqueShadowcast);


	{
		EffectTemplate defaultTextured;
		defaultTextured.forwardEffect = texturedLitPass;
		defaultTextured.shadowEffect = opaqueShadowcastPass;
		defaultTextured.defaultParameters = nullptr;
		defaultTextured.transparency = assets::TransparencyMode::Opaque;

		templateCache["texturedPBR_opaque"] = defaultTextured;
	}

	{
		EffectTemplate defaultColored;
		defaultColored.forwardEffect = defaultLitPass;
		defaultColored.shadowEffect = opaqueShadowcastPass;
		defaultColored.defaultParameters = nullptr;
		defaultColored.transparency = assets::TransparencyMode::Opaque;
		templateCache["colored_opaque"] = defaultColored;
	}
	
}

vkutil::ShaderPass* vkutil::MaterialSystem::build_shader(VkRenderPass renderPass, PipelineBuilder& builder, ShaderEffect* effect)
{
	ShaderPass* pass = new ShaderPass();

	pass->effect = effect;
	pass->layout = effect->builtLayout;

	PipelineBuilder pipbuilder = builder;

	pipbuilder.setShaders(effect);

	pass->pipeline = pipbuilder.build_pipeline(engine->_device, renderPass);

	return pass;
}


vkutil::Material* vkutil::MaterialSystem::build_material(const std::string& materialName, const MaterialInfo& info)
{
	Material* mat;
	//search material in the cache first in case its already built
	auto it = materialCache.find(info);
	if (it != materialCache.end())
	{
		mat = (*it).second;
		materials[materialName] = mat;
	}
	else {

		//need to build the material
		Material *newMat = new Material();
		newMat->original = &templateCache[ info.baseTemplate];
		newMat->parameters = info.parameters;
		//not handled yet
		newMat->shadowSet = VK_NULL_HANDLE;
		newMat->textures = info.textures;


	
		auto& db = vkutil::DescriptorBuilder::begin(engine->_descriptorLayoutCache, engine->_descriptorAllocator);

		for (int i = 0; i < info.textures.size(); i++)
		{
			VkDescriptorImageInfo imageBufferInfo;
			imageBufferInfo.sampler = info.textures[i].sampler;
			imageBufferInfo.imageView = info.textures[i].view;
			imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			db.bind_image(i, &imageBufferInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		}

		
			
		db.build(newMat->forwardSet);
		LOG_INFO("Built New Material {}", materialName);
		//add material to cache
		materialCache[info] = (newMat);
		mat = newMat;
		materials[materialName] = mat;
	}

	return mat;
}

vkutil::Material* vkutil::MaterialSystem::get_material(const std::string& materialName)
{
	auto it = materials.find(materialName);
	if (it != materials.end())
	{
		return(*it).second;
	}
	else {
		return nullptr;
	}
}

void vkutil::MaterialSystem::fill_builders()
{
	{
		shadowBuilder.vertexDescription = Vertex::get_vertex_description();

		shadowBuilder._inputAssembly = vkinit::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		
		shadowBuilder._rasterizer = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_FILL);
		shadowBuilder._rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
		shadowBuilder._rasterizer.depthBiasEnable = VK_TRUE;
		
		shadowBuilder._multisampling = vkinit::multisampling_state_create_info();
		shadowBuilder._colorBlendAttachment = vkinit::color_blend_attachment_state();

		//default depthtesting
		shadowBuilder._depthStencil = vkinit::depth_stencil_create_info(true, true, VK_COMPARE_OP_LESS);
	}
	{
		forwardBuilder.vertexDescription = Vertex::get_vertex_description();
		
		forwardBuilder._inputAssembly = vkinit::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

		
		forwardBuilder._rasterizer = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_FILL);
		forwardBuilder._rasterizer.cullMode = VK_CULL_MODE_NONE;
		
		forwardBuilder._multisampling = vkinit::multisampling_state_create_info();
		
		forwardBuilder._colorBlendAttachment = vkinit::color_blend_attachment_state();

		//default depthtesting
		forwardBuilder._depthStencil = vkinit::depth_stencil_create_info(true, true, VK_COMPARE_OP_GREATER_OR_EQUAL);
	}	
}

bool vkutil::MaterialInfo::operator==(const MaterialInfo& other) const
{
	if (other.baseTemplate.compare(baseTemplate) != 0 || other.parameters != parameters || other.textures.size() != textures.size())
	{
		return false;
	}
	else {
		//binary compare textures
		bool comp = memcmp(other.textures.data(), textures.data(), textures.size() * sizeof(textures[0])) == 0;
		return comp;
	}
}

size_t vkutil::MaterialInfo::hash() const
{
	using std::size_t;
	using std::hash;

	size_t result = hash<std::string>()(baseTemplate);

	for (const auto& b : textures)
	{
		//pack the binding data into a single int64. Not fully correct but its ok
		size_t texture_hash = (std::hash<size_t>()((size_t)b.sampler) << 3) &&(std::hash<size_t>()((size_t)b.view) >> 7);

		//shuffle the packed binding data and xor it with the main hash
		result ^= std::hash<size_t>()(texture_hash);
	}

	return result;
}
