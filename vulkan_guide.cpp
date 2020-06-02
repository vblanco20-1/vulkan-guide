// vulkan_guide.cpp : Defines the entry point for the application.
//
#include "volk.h"
#define VK_NO_PROTOTYPES

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include "VkBootstrap.h"

#include "vulkan_guide.h"
#include <vector>
#include <fstream>

#include <SDL.h>
#include <SDL_vulkan.h>

using namespace std;
#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			std::cout <<"Detected Vulkan error: " << err << std::endl; \
			abort();                                                \
		}                                                           \
	} while (0)


namespace VkInit {

	VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule module) {
		VkPipelineShaderStageCreateInfo shaderStageInfo{};
		shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageInfo.pNext = nullptr;
		shaderStageInfo.stage = stage;
		shaderStageInfo.module = module;
		shaderStageInfo.pName = "main";
		return shaderStageInfo;
	}

	VkPipelineLayoutCreateInfo pipeline_layout_create_info() {
		VkPipelineLayoutCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.setLayoutCount = 0;
		info.pSetLayouts = nullptr;
		info.pushConstantRangeCount = 0;
		info.pPushConstantRanges = nullptr;
		return info;
	}

	VkComputePipelineCreateInfo compute_pipeline_create_info(VkPipelineLayout layout, const VkPipelineShaderStageCreateInfo& computeStage) {
		VkComputePipelineCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		info.pNext = nullptr;
		info.layout = layout;
		info.stage = computeStage;
		return info;
	}

	VkCommandPoolCreateInfo command_pool_create_info(uint32_t queueFamilyIndex, VkCommandPoolResetFlags flags = 0) {
		VkCommandPoolCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = flags;
		return info;
	}
	VkCommandBufferAllocateInfo command_allocate_info(VkCommandPool pool, uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) {
		VkCommandBufferAllocateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		info.pNext = nullptr;
		info.commandPool = pool;
		info.commandBufferCount = count;
		info.level = level;
		return info;
	}

	VkCommandBufferBeginInfo command_buffer_begin_info(VkCommandBufferUsageFlags flags = 0) {
		VkCommandBufferBeginInfo info{};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.pNext = nullptr;
		info.pInheritanceInfo = nullptr;
		info.flags = flags;
		return info;
	}

	VkSubmitInfo submit_info(VkCommandBuffer* cmd) {
		VkSubmitInfo info{};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.pNext = nullptr;

		info.waitSemaphoreCount = 0;
		info.pWaitSemaphores = nullptr;
		info.pWaitDstStageMask = nullptr;
		info.commandBufferCount = 1;
		info.pCommandBuffers = cmd;
		info.signalSemaphoreCount = 0;
		info.pSignalSemaphores = nullptr;

		return info;
	}
}
namespace VkUtils {
	
	static std::vector<char> read_file(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	VkShaderModule create_shader_module(VkDevice device,const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}
		return shaderModule;
	}

	VkRenderPass create_render_pass(VkDevice device, VkFormat image_format) {
		VkAttachmentDescription color_attachment = {};
		color_attachment.format = image_format;
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment_ref = {};
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_info.attachmentCount = 1;
		render_pass_info.pAttachments = &color_attachment;
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass;
		render_pass_info.dependencyCount = 1;
		render_pass_info.pDependencies = &dependency;

		VkRenderPass renderPass;
		if (vkCreateRenderPass(device, &render_pass_info, nullptr, &renderPass) != VK_SUCCESS) {
			std::cout << "failed to create render pass\n";
		}
		return renderPass;
	}


	int create_graphics_pipeline(VkDevice device,VkExtent2D swapchainExtent, VkRenderPass renderPass,VkPipelineLayout* outPipelineLayout,VkPipeline* outPipeline) {
		auto vert_code = read_file("vert.spv");
		auto frag_code = read_file("frag.spv");

		VkShaderModule vert_module = create_shader_module(device, vert_code);
		VkShaderModule frag_module = create_shader_module(device, frag_code);
		if (vert_module == VK_NULL_HANDLE || frag_module == VK_NULL_HANDLE) {
			std::cout << "failed to create shader module\n";
			return -1; // failed to create shader modules
		}

		VkPipelineShaderStageCreateInfo vert_stage_info = {};
		vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vert_stage_info.module = vert_module;
		vert_stage_info.pName = "main";

		VkPipelineShaderStageCreateInfo frag_stage_info = {};
		frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		frag_stage_info.module = frag_module;
		frag_stage_info.pName = "main";

		VkPipelineShaderStageCreateInfo shader_stages[] = { vert_stage_info, frag_stage_info };

		VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
		vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_info.vertexBindingDescriptionCount = 0;
		vertex_input_info.vertexAttributeDescriptionCount = 0;

		VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
		input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		input_assembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapchainExtent.width;
		viewport.height = (float)swapchainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchainExtent;

		VkPipelineViewportStateCreateInfo viewport_state = {};
		viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state.viewportCount = 1;
		viewport_state.pViewports = &viewport;
		viewport_state.scissorCount = 1;
		viewport_state.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo color_blending = {};
		color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blending.logicOpEnable = VK_FALSE;
		color_blending.logicOp = VK_LOGIC_OP_COPY;
		color_blending.attachmentCount = 1;
		color_blending.pAttachments = &colorBlendAttachment;
		color_blending.blendConstants[0] = 0.0f;
		color_blending.blendConstants[1] = 0.0f;
		color_blending.blendConstants[2] = 0.0f;
		color_blending.blendConstants[3] = 0.0f;

		VkPipelineLayoutCreateInfo pipeline_layout_info = {};
		pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_info.setLayoutCount = 0;
		pipeline_layout_info.pushConstantRangeCount = 0;

		if (vkCreatePipelineLayout(
			device, &pipeline_layout_info, nullptr, outPipelineLayout) != VK_SUCCESS) {
			std::cout << "failed to create pipeline layout\n";
			return -1; // failed to create pipeline layout
		}

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shader_stages;
		pipelineInfo.pVertexInputState = &vertex_input_info;
		pipelineInfo.pInputAssemblyState = &input_assembly;
		pipelineInfo.pViewportState = &viewport_state;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &color_blending;
		pipelineInfo.layout = *outPipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(
			device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, outPipeline) != VK_SUCCESS) {
			std::cout << "failed to create pipline\n";
			return -1; // failed to create graphics pipeline
		}

		vkDestroyShaderModule(device, frag_module, nullptr);
		vkDestroyShaderModule(device, vert_module, nullptr);
		return 0;
	}

}

class VulkanEngine {
public:
	
	VkInstance instance;
	VkDevice device;
	VkQueue graphicsQueue;
	VkSwapchainKHR swapchain;
	VkSemaphore sema_present, sema_render;
	VkFence render_fence;
	VkCommandPool commandPool;
	VkRenderPass renderPass;
	std::vector<VkFramebuffer> fbuffers;
	VkExtent2D windowExtent;
	std::vector<VkImage> sc_images;
	std::vector<VkImageView> sc_imageviews;

	void init();
	void cleanup();
	void draw();

};
SDL_Renderer* gRenderer;
SDL_Window* gWindow;
void VulkanEngine::init()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);// | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

	windowExtent.height = 900;
	windowExtent.width = 1700;

	gWindow = SDL_CreateWindow(
		"Vulkan Engine",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		1700,
		900,
		window_flags
	);
	assert(gWindow != nullptr);
	
	volkInitialize();

	vkb::InstanceBuilder builder;

	auto inst_ret = builder.set_app_name("Example Vulkan Application")
		//.set_headless(true)
		.request_validation_layers()		
		.use_default_debug_messenger()
		.add_validation_feature_enable(VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT)
		.add_debug_messenger_severity(VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT)
		.add_debug_messenger_type(VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT)
		.build();
	
	vkb::Instance vkb_inst = inst_ret.value();

	instance = vkb_inst.instance;
	
	volkLoadInstance(instance);

	VkPhysicalDevice dedicatedGPU;

	VkSurfaceKHR vksurface;
	if (!SDL_Vulkan_CreateSurface(gWindow, instance, &vksurface)) {
		throw std::runtime_error("Failed to create surface");
		// failed to create a surface!
	}

	
	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	auto phys_ret = selector//.set_surface(/* from user created window*/)
		.set_minimum_version(1, 2) // require a vulkan 1.1 capable device		
		.add_required_extension(VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME)
		.set_surface(vksurface)
		.select();

	vkb::DeviceBuilder device_builder{ phys_ret.value() };
	// automatically propagate needed data from instance & physical device
	auto dev_ret = device_builder.build();

	vkb::Device vkb_device = dev_ret.value();

	// Get the VkDevice handle used in the rest of a vulkan application
	device = vkb_device.device;

	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = phys_ret.value().physical_device;
	allocatorInfo.device = device;
	allocatorInfo.instance = instance;

	VmaAllocator allocator;
	vmaCreateAllocator(&allocatorInfo, &allocator);

	

	vkb::SwapchainBuilder swapchain_builder{ vkb_device };
	auto swap_ret =
		swapchain_builder.use_default_format_selection().use_default_present_mode_selection()
		.set_desired_extent(windowExtent.width, windowExtent.height)
		.build();
	
	swapchain = swap_ret.value().swapchain;

	uint32_t swapchain_imagecount = swap_ret.value().image_count;


	sc_images = swap_ret.value().get_images().value();
	sc_imageviews= swap_ret.value().get_image_views().value();
	



	renderPass = VkUtils::create_render_pass(device, swap_ret.value().image_format);


	VkFramebufferCreateInfo fb_info = {};
	fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fb_info.renderPass = renderPass;
	fb_info.attachmentCount =1;
	
	fb_info.width = windowExtent.width;
	fb_info.height = windowExtent.height;
	fb_info.layers = 1;

	fbuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

	for (int i = 0; i < swapchain_imagecount; i++) {
		
		fb_info.pAttachments = &sc_imageviews[i];
		VK_CHECK( vkCreateFramebuffer(device, &fb_info, NULL, &fbuffers[i]));		
	}

	// Get the graphics queue with a helper function
	auto graphics_queue_ret = vkb_device.get_queue(vkb::QueueType::graphics);

	graphicsQueue = graphics_queue_ret.value();
	

	uint32_t graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

	auto compute_shader_code = VkUtils::read_file("K:/Programming/vulkan_guide/shaders/hellogpu.comp.spv");

	VkShaderModule computeShader = VkUtils::create_shader_module(device, compute_shader_code);

	VkPipelineShaderStageCreateInfo compShaderStageInfo = VkInit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_COMPUTE_BIT, computeShader);

	VkPipelineLayout computeLayout;
	VkPipelineLayoutCreateInfo computeLayoutInfo = VkInit::pipeline_layout_create_info();

	VK_CHECK(vkCreatePipelineLayout(device, &computeLayoutInfo, nullptr, &computeLayout));

	VkComputePipelineCreateInfo computePipelineInfo = VkInit::compute_pipeline_create_info(computeLayout, compShaderStageInfo);

	VkPipeline computePipeline;
	VK_CHECK(vkCreateComputePipelines(device,VK_NULL_HANDLE,1,&computePipelineInfo,nullptr,&computePipeline));

	VkCommandPoolCreateInfo commandPoolInfo = VkInit::command_pool_create_info(graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	

	VK_CHECK(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &commandPool));

	VkCommandBufferAllocateInfo cmdAllocInfo = VkInit::command_allocate_info(commandPool, 1);
	VkCommandBuffer cmd;

	VK_CHECK(vkAllocateCommandBuffers(device, &cmdAllocInfo, &cmd));

	VkCommandBufferBeginInfo cmdBeginInfo = VkInit::command_buffer_begin_info();

	VkClearValue clear_value;
		
		
	
	clear_value.color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
	

	VkRenderPassBeginInfo rp_info = {  };
	rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rp_info.renderPass = renderPass;
	rp_info.renderArea.offset.x = 0;
	rp_info.renderArea.offset.y = 0;
	rp_info.renderArea.extent = windowExtent;
	rp_info.clearValueCount = 2;
	rp_info.pClearValues = &clear_value;
	rp_info.framebuffer = fbuffers[0];


	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
	vkCmdDispatch(cmd, 10, 1, 1);

	vkCmdBeginRenderPass(cmd, &rp_info, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdEndRenderPass(cmd);
	VK_CHECK(vkEndCommandBuffer(cmd));

	VkSubmitInfo submit = VkInit::submit_info(&cmd);

	VkFence compute_fence;
	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;
	fenceCreateInfo.flags = 0;

	vkCreateFence(device, &fenceCreateInfo, nullptr, &compute_fence);
	vkCreateFence(device, &fenceCreateInfo, nullptr, &render_fence);
	vkQueueSubmit(graphicsQueue, 1, &submit, compute_fence);

	vkWaitForFences(device, 1, &compute_fence, true, 0);

	vkDestroyPipelineLayout(device, computeLayout, nullptr);
	vkDestroyShaderModule(device, computeShader, nullptr);
	
	vkDestroyPipeline(device, computePipeline, nullptr);
	vkDestroyFence(device, compute_fence, nullptr);

	vkDestroyDebugUtilsMessengerEXT(instance, vkb_inst.debug_messenger, nullptr);

	VkSemaphoreCreateInfo bake_sema = {};
	bake_sema.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	bake_sema.pNext = nullptr;
	bake_sema.flags = VK_FENCE_CREATE_SIGNALED_BIT;//0;

	VK_CHECK(vkCreateSemaphore(device, &bake_sema, NULL, &sema_present));
	VK_CHECK(vkCreateSemaphore(device, &bake_sema, NULL, &sema_render));
	
}
void VulkanEngine::draw() {
	uint32_t idx;
	
	vkWaitForFences(device, 1, &render_fence, true, 999999999);
	vkResetFences(device, 1, &render_fence);

	vkResetCommandPool(device, commandPool, 0);
		
		
	VK_CHECK( vkAcquireNextImageKHR(device, swapchain, 0, sema_present, NULL, &idx));	


	
	VkCommandBufferAllocateInfo cmdAllocInfo = VkInit::command_allocate_info(commandPool, 1);
	VkCommandBuffer cmd;

	VK_CHECK(vkAllocateCommandBuffers(device, &cmdAllocInfo, &cmd));

	VkCommandBufferBeginInfo cmdBeginInfo = VkInit::command_buffer_begin_info();

	VkClearValue clear_value;

	clear_value.color = { { 0.0f, 0.0f, 0.2f, 1.0f } };


	VkRenderPassBeginInfo rp_info = {  };
	rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rp_info.renderPass = renderPass;
	rp_info.renderArea.offset.x = 0;
	rp_info.renderArea.offset.y = 0;
	rp_info.renderArea.extent = windowExtent;
	rp_info.clearValueCount = 2;
	rp_info.pClearValues = &clear_value;
	rp_info.framebuffer = fbuffers[idx];


	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	

	vkCmdBeginRenderPass(cmd, &rp_info, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdEndRenderPass(cmd);
	VK_CHECK(vkEndCommandBuffer(cmd));

	VkSubmitInfo submit = VkInit::submit_info(&cmd);
	VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	
	submit.pWaitDstStageMask = &wait_stage;

	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &sema_present;


	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &sema_render;

	vkQueueSubmit(graphicsQueue, 1, &submit, render_fence);
	
	

	VkPresentInfoKHR present_info = {  };
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &swapchain;
	present_info.pWaitSemaphores = &sema_render;
	present_info.waitSemaphoreCount = 1;
	present_info.pImageIndices = &idx;
	VK_CHECK (vkQueuePresentKHR(graphicsQueue, &present_info));

	
	
}
void VulkanEngine::cleanup()
{
	vkDestroyCommandPool(device, commandPool, nullptr);
	vkDestroyDevice(device, nullptr);
	vkDestroyInstance(instance,nullptr);
}

int main(int argc, char* argv[])
{
	VulkanEngine engine;
	engine.init();
	SDL_Event e;
	bool bQuit = false;
	while (!bQuit)
	{		
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT) bQuit = true;
		}

		engine.draw();
	}

	engine.cleanup();

	return 0;
}
