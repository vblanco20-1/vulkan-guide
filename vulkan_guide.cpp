// vulkan_guide.cpp : Defines the entry point for the application.
//
#include "volk.h"
#define VK_NO_PROTOTYPES

//#define VMA_IMPLEMENTATION
//#include "vk_mem_alloc.h"

#include "VkBootstrap.h"

#include "vulkan_guide.h"
#include <vector>
#include <fstream>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <chrono>
#include <thread>

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


namespace vkinit {

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
	VkFramebufferCreateInfo framebuffer_create_info(VkRenderPass renderPass,VkExtent2D extent) {
		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.pNext = nullptr;

		info.renderPass = renderPass;
		info.attachmentCount = 1;
		info.width = extent.width;
		info.height = extent.height;
		info.layers = 1;

		return info;
	}

	VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0) {

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.flags = flags;

		return fenceCreateInfo;
	}
	VkSemaphoreCreateInfo semaphore_create_info(VkSemaphoreCreateFlags flags = 0) {
		VkSemaphoreCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		info.pNext = nullptr;
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
	VkPresentInfoKHR present_info() {
		VkPresentInfoKHR info = {  };
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.pNext = nullptr;

		info.swapchainCount = 0;
		info.pSwapchains = nullptr;
		info.pWaitSemaphores = nullptr;
		info.waitSemaphoreCount = 0;
		info.pImageIndices = nullptr;

		return info;
	}
	
}
namespace vkutil {

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
		VK_CHECK(vkCreateRenderPass(device, &render_pass_info, nullptr, &renderPass));
		return renderPass;
	}
}

class VulkanEngine {
public:
	
	VkInstance _instance;
	VkDevice _device;
	VkQueue _graphicsQueue;
	VkSwapchainKHR _swapchain;
	VkSemaphore _presentSemaphore, _renderSemaphore;
	VkFence _renderFence;
	VkCommandPool _commandPool;
	VkRenderPass _renderPass;
	std::vector<VkFramebuffer> _framebuffers;
	VkExtent2D _windowExtent;
	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;
	VkCommandBuffer _mainCommandBuffer;

	uint64_t _frameNumber;

	void init();
	void cleanup();
	void draw();
};
SDL_Renderer* gRenderer;
SDL_Window* gWindow;
void VulkanEngine::init()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

	_frameNumber = 0;
	_windowExtent.height = 900;
	_windowExtent.width = 1700;

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
		.request_validation_layers()		
		.use_default_debug_messenger()
		.add_validation_feature_enable(VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT)
		.add_debug_messenger_severity(VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT)
		.add_debug_messenger_type(VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT)
		.build();
	
	vkb::Instance vkb_inst = inst_ret.value();

	_instance = vkb_inst.instance;
	
	volkLoadInstance(_instance);

	VkPhysicalDevice dedicatedGPU;

	VkSurfaceKHR vksurface;
	if (!SDL_Vulkan_CreateSurface(gWindow, _instance, &vksurface)) {
		throw std::runtime_error("Failed to create surface");
		// failed to create a surface!
	}

	
	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	auto phys_ret = selector//.set_surface(/* from user created window*/)
		.set_minimum_version(1, 2) // require a vulkan 1.1 capable device		
		.add_required_extension(VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME)
		.set_surface(vksurface)
		.select();

	vkb::DeviceBuilder deviceBuilder{ phys_ret.value() };
	// automatically propagate needed data from instance & physical device

	vkb::Device vkbDevice = deviceBuilder.build().value();

	// Get the VkDevice handle used in the rest of a vulkan application
	_device = vkbDevice.device;

	vkb::SwapchainBuilder swapchainBuilder{ vkbDevice };

	vkb::Swapchain vkbSwapchain = swapchainBuilder
		.use_default_format_selection()
		.use_default_present_mode_selection()
		.set_desired_extent(_windowExtent.width, _windowExtent.height)
		.build()
		.value();

	
	_swapchain = vkbSwapchain.swapchain;

	uint32_t swapchain_imagecount = vkbSwapchain.image_count;

	_swapchainImages = vkbSwapchain.get_images().value();
	_swapchainImageViews= vkbSwapchain.get_image_views().value();

	_renderPass = vkutil::create_render_pass(_device, vkbSwapchain.image_format);


	VkFramebufferCreateInfo fb_info = vkinit::framebuffer_create_info(_renderPass,_windowExtent);

	_framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

	for (int i = 0; i < swapchain_imagecount; i++) {
		
		fb_info.pAttachments = &_swapchainImageViews[i];
		VK_CHECK( vkCreateFramebuffer(_device, &fb_info, NULL, &_framebuffers[i]));		
	}

	// Get the graphics queue with a helper function
	_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();

	uint32_t graphics_queue_family = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

	VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_commandPool));

	VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_allocate_info(_commandPool, 1);

	VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_mainCommandBuffer));

	VkFenceCreateInfo fenceCreateInfo = vkinit::fence_create_info();

	vkCreateFence(_device, &fenceCreateInfo, nullptr, &_renderFence);

	VkSemaphoreCreateInfo sephoreCreateInfo = vkinit::semaphore_create_info();

	VK_CHECK(vkCreateSemaphore(_device, &sephoreCreateInfo, NULL, &_presentSemaphore));
	VK_CHECK(vkCreateSemaphore(_device, &sephoreCreateInfo, NULL, &_renderSemaphore));	
}
void VulkanEngine::draw() {	
	
	vkWaitForFences(_device, 1, &_renderFence, true, 999999999);
	vkResetFences(_device, 1, &_renderFence);

	vkResetCommandBuffer(_mainCommandBuffer, 0);	
		
	uint32_t imageIndex;
	VK_CHECK( vkAcquireNextImageKHR(_device, _swapchain, 0, _presentSemaphore, NULL, &imageIndex));	
	
	VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_allocate_info(_commandPool, 1);
	
	VkCommandBuffer cmd = _mainCommandBuffer;

	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkClearValue clearValue;

	float flash = abs(sin(_frameNumber / 120.f));
	clearValue.color = { { 0.0f, 0.0f, flash, 1.0f } };

	VkRenderPassBeginInfo rpInfo = {  };
	rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rpInfo.renderPass = _renderPass;
	rpInfo.renderArea.offset.x = 0;
	rpInfo.renderArea.offset.y = 0;
	rpInfo.renderArea.extent = _windowExtent;
	rpInfo.clearValueCount = 2;
	rpInfo.pClearValues = &clearValue;
	rpInfo.framebuffer = _framebuffers[imageIndex];

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdEndRenderPass(cmd);
	VK_CHECK(vkEndCommandBuffer(cmd));

	VkSubmitInfo submit = vkinit::submit_info(&cmd);
	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	
	submit.pWaitDstStageMask = &waitStage;

	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &_presentSemaphore;

	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &_renderSemaphore;

	VK_CHECK(vkQueueSubmit(_graphicsQueue, 1, &submit, _renderFence));

	VkPresentInfoKHR presentInfo = vkinit::present_info();

	presentInfo.pSwapchains = &_swapchain;
	presentInfo.swapchainCount = 1;
	
	presentInfo.pWaitSemaphores = &_renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &imageIndex;

	VK_CHECK (vkQueuePresentKHR(_graphicsQueue, &presentInfo));

	_frameNumber++;
}
void VulkanEngine::cleanup()
{
	vkDestroyCommandPool(_device, _commandPool, nullptr);
	vkDestroyDevice(_device, nullptr);
	vkDestroyInstance(_instance,nullptr);
}

int main(int argc, char* argv[])
{
	using namespace std::chrono_literals;
	using std::chrono::system_clock;

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

		
		//wait a few ms because it will go into the thousands otherwise
		std::this_thread::sleep_for(10ms);
	}

	engine.cleanup();

	return 0;
}
