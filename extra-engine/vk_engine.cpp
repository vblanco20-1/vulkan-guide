
#include "vk_engine.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vk_types.h>
#include <vk_initializers.h>
#include <vk_descriptors.h>

#include "VkBootstrap.h"

#include <iostream>
#include <fstream>

#include "vk_textures.h"
#include "vk_shaders.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_vulkan.h"
#include "prefab_asset.h"
#include "material_asset.h"

#include "Tracy.hpp"



constexpr bool bUseValidationLayers = false;

//we want to immediately abort when there is an error. In normal engines this would give an error message to the user, or perform a dump of state.
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


void VulkanEngine::init()
{
	ZoneScopedN("Engine Init");
	// We initialize SDL and create a window with it. 
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

	_window = SDL_CreateWindow(
		"Vulkan Engine",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		_windowExtent.width,
		_windowExtent.height,
		window_flags
	);

	init_vulkan();

	init_swapchain();

	init_default_renderpass();

	init_framebuffers();

	init_commands();

	init_sync_structures();

	init_descriptors();

	init_pipelines();

	load_images();

	load_meshes();

	init_scene();

	init_imgui();
	//everything went fine
	_isInitialized = true;

	_camera = {};
	_camera.position = { 0.f,6.f,10.f };
}
void VulkanEngine::cleanup()
{
	if (_isInitialized) {

		//make sure the gpu has stopped doing its things
		for (auto& frame : _frames)
		{
			vkWaitForFences(_device, 1, &frame._renderFence, true, 1000000000);
		}

		_mainDeletionQueue.flush();

		for (auto& frame : _frames)
		{
			frame.dynamicDescriptorAllocator->cleanup();
		}

		_descriptorAllocator->cleanup();
		_descriptorLayoutCache->cleanup();

		vkDestroySurfaceKHR(_instance, _surface, nullptr);

		vkDestroyDevice(_device, nullptr);
		vkDestroyInstance(_instance, nullptr);

		SDL_DestroyWindow(_window);
	}
}

void VulkanEngine::draw()
{
	ZoneScopedN("Engine Draw");

	ImGui::Render();

	{
		ZoneScopedN("Fence Wait");
		//wait until the gpu has finished rendering the last frame. Timeout of 1 second
		VK_CHECK(vkWaitForFences(_device, 1, &get_current_frame()._renderFence, true, 1000000000));
		VK_CHECK(vkResetFences(_device, 1, &get_current_frame()._renderFence));

	}
	get_current_frame().dynamicDescriptorAllocator->reset_pools();

	//now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
	VK_CHECK(vkResetCommandBuffer(get_current_frame()._mainCommandBuffer, 0));
	uint32_t swapchainImageIndex;
	{
		ZoneScopedN("Aquire Image");
		//request image from the swapchain
		
		VK_CHECK(vkAcquireNextImageKHR(_device, _swapchain, 0, get_current_frame()._presentSemaphore, nullptr, &swapchainImageIndex));

	}
	//naming it cmd for shorter writing
	VkCommandBuffer cmd = get_current_frame()._mainCommandBuffer;

	//begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	//make a clear-color from frame number. This will flash with a 120 frame period.
	VkClearValue clearValue;
	float flash = abs(sin(_frameNumber / 120.f));
	clearValue.color = { { 0.1f, 0.1f, 0.1f, 1.0f } };

	//clear depth at 1
	VkClearValue depthClear;
	depthClear.depthStencil.depth = 1.f;

	//start the main renderpass. 
	//We will use the clear color from above, and the framebuffer of the index the swapchain gave us
	VkRenderPassBeginInfo rpInfo = vkinit::renderpass_begin_info(_renderPass, _windowExtent, _framebuffers[swapchainImageIndex]);

	//connect clear values
	rpInfo.clearValueCount = 2;

	VkClearValue clearValues[] = { clearValue, depthClear };

	rpInfo.pClearValues = &clearValues[0];

	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	draw_objects(cmd, _renderables.data(), _renderables.size());

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

	//finalize the render pass
	vkCmdEndRenderPass(cmd);
	//finalize the command buffer (we can no longer add commands, but it can now be executed)
	VK_CHECK(vkEndCommandBuffer(cmd));

	//prepare the submission to the queue. 
	//we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
	//we will signal the _renderSemaphore, to signal that rendering has finished

	VkSubmitInfo submit = vkinit::submit_info(&cmd);
	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	submit.pWaitDstStageMask = &waitStage;

	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &get_current_frame()._presentSemaphore;

	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &get_current_frame()._renderSemaphore;
	{
		ZoneScopedN("Queue Submit");
		//submit command buffer to the queue and execute it.
		// _renderFence will now block until the graphic commands finish execution
		VK_CHECK(vkQueueSubmit(_graphicsQueue, 1, &submit, get_current_frame()._renderFence));

	}
	//prepare present
	// this will put the image we just rendered to into the visible window.
	// we want to wait on the _renderSemaphore for that, 
	// as its necessary that drawing commands have finished before the image is displayed to the user
	VkPresentInfoKHR presentInfo = vkinit::present_info();

	presentInfo.pSwapchains = &_swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &get_current_frame()._renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	{
		ZoneScopedN("Queue Present");
		VK_CHECK(vkQueuePresentKHR(_graphicsQueue, &presentInfo));

	}
	//increase the number of frames drawn
	_frameNumber++;
}

void VulkanEngine::run()
{
	SDL_Event e;
	bool bQuit = false;

	//main loop
	while (!bQuit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			ImGui_ImplSDL2_ProcessEvent(&e);
			process_input_event(&e);
			//close the window when user alt-f4s or clicks the X button			
			if (e.type == SDL_QUIT)
			{
				bQuit = true;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_SPACE)
				{
					_selectedShader += 1;
					if (_selectedShader > 1)
					{
						_selectedShader = 0;
					}
				}
			}
		}
		//imgui new frame 
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(_window);

		ImGui::NewFrame();


		//imgui commands
		ImGui::ShowDemoWindow();


		update_camera(1.0 / 60.f);

		draw();
	}
}

FrameData& VulkanEngine::get_current_frame()
{
	return _frames[_frameNumber % FRAME_OVERLAP];
}


FrameData& VulkanEngine::get_last_frame()
{
	return _frames[(_frameNumber - 1) % 2];
}


void VulkanEngine::process_input_event(SDL_Event* ev)
{
	if (ev->type == SDL_KEYDOWN)
	{
		switch (ev->key.keysym.sym)
		{
		case SDLK_UP:
		case SDLK_w:
			_camera.inputAxis.x += 1.f;
			break;
		case SDLK_DOWN:
		case SDLK_s:
			_camera.inputAxis.x -= 1.f;
			break;
		case SDLK_LEFT:
		case SDLK_a:
			_camera.inputAxis.y -= 1.f;
			break;
		case SDLK_RIGHT:
		case SDLK_d:
			_camera.inputAxis.y += 1.f;
			break;
		}
	}
	else if (ev->type == SDL_KEYUP)
	{
		switch (ev->key.keysym.sym)
		{
		case SDLK_UP:
		case SDLK_w:
			_camera.inputAxis.x -= 1.f;
			break;
		case SDLK_DOWN:
		case SDLK_s:
			_camera.inputAxis.x += 1.f;
			break;
		case SDLK_LEFT:
		case SDLK_a:
			_camera.inputAxis.y += 1.f;
			break;
		case SDLK_RIGHT:
		case SDLK_d:
			_camera.inputAxis.y -= 1.f;
			break;
		}
	}
	else if (ev->type == SDL_MOUSEMOTION) {
		_camera.pitch -= ev->motion.yrel * 0.003;
		_camera.yaw -= ev->motion.xrel * 0.003;
	}

	_camera.inputAxis = glm::clamp(_camera.inputAxis, { -1.0,-1.0,-1.0 }, { 1.0,1.0,1.0 });
}

void VulkanEngine::update_camera(float deltaSeconds)
{
	glm::vec3 forward = { 0,0,-1 };
	glm::vec3 right = { 1,0,0 };

	


	glm::mat4 cam_rot = _camera.get_rotation_matrix();

	forward = cam_rot * glm::vec4(forward, 0.f);
	right =  cam_rot * glm::vec4(right, 0.f);

	_camera.velocity = _camera.inputAxis.x * forward + _camera.inputAxis.y * right;

	_camera.velocity *= 10 * deltaSeconds;

	_camera.position += _camera.velocity;
}

void VulkanEngine::init_vulkan()
{
	vkb::InstanceBuilder builder;

	//make the vulkan instance, with basic debug features
	auto inst_ret = builder.set_app_name("Example Vulkan Application")
		.request_validation_layers(bUseValidationLayers)
		.use_default_debug_messenger()
		.build();

	vkb::Instance vkb_inst = inst_ret.value();

	//grab the instance 
	_instance = vkb_inst.instance;

	SDL_Vulkan_CreateSurface(_window, _instance, &_surface);

	//use vkbootstrap to select a gpu. 
	//We want a gpu that can write to the SDL surface and supports vulkan 1.2
	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version(1, 2)
		.set_surface(_surface)
		.select()
		.value();

	//create the final vulkan device

	vkb::DeviceBuilder deviceBuilder{ physicalDevice };

	vkb::Device vkbDevice = deviceBuilder.build().value();

	// Get the VkDevice handle used in the rest of a vulkan application
	_device = vkbDevice.device;
	_chosenGPU = physicalDevice.physical_device;

	// use vkbootstrap to get a Graphics queue
	_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();

	_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

	//initialize the memory allocator
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = _chosenGPU;
	allocatorInfo.device = _device;
	allocatorInfo.instance = _instance;
	vmaCreateAllocator(&allocatorInfo, &_allocator);



	vkGetPhysicalDeviceProperties(_chosenGPU, &_gpuProperties);

	std::cout << "The gpu has a minimum buffer alignement of " << _gpuProperties.limits.minUniformBufferOffsetAlignment << std::endl;

}

void VulkanEngine::init_swapchain()
{
	vkb::SwapchainBuilder swapchainBuilder{ _chosenGPU,_device,_surface };

	vkb::Swapchain vkbSwapchain = swapchainBuilder
		.use_default_format_selection()
		//use vsync present mode
		.set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
		.set_desired_extent(_windowExtent.width, _windowExtent.height)
		.build()
		.value();

	//store swapchain and its related images
	_swapchain = vkbSwapchain.swapchain;
	_swapchainImages = vkbSwapchain.get_images().value();
	_swapchainImageViews = vkbSwapchain.get_image_views().value();

	_swachainImageFormat = vkbSwapchain.image_format;

	_mainDeletionQueue.push_function([=]() {
		vkDestroySwapchainKHR(_device, _swapchain, nullptr);
		});

	//depth image size will match the window
	VkExtent3D depthImageExtent = {
		_windowExtent.width,
		_windowExtent.height,
		1
	};

	//hardcoding the depth format to 32 bit float
	_depthFormat = VK_FORMAT_D32_SFLOAT;

	//the depth image will be a image with the format we selected and Depth Attachment usage flag
	VkImageCreateInfo dimg_info = vkinit::image_create_info(_depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImageExtent);

	//for the depth image, we want to allocate it from gpu local memory
	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	//allocate and create the image
	vmaCreateImage(_allocator, &dimg_info, &dimg_allocinfo, &_depthImage._image, &_depthImage._allocation, nullptr);

	//build a image-view for the depth image to use for rendering
	VkImageViewCreateInfo dview_info = vkinit::imageview_create_info(_depthFormat, _depthImage._image, VK_IMAGE_ASPECT_DEPTH_BIT);;

	VK_CHECK(vkCreateImageView(_device, &dview_info, nullptr, &_depthImageView));

	//add to deletion queues
	_mainDeletionQueue.push_function([=]() {
		vkDestroyImageView(_device, _depthImageView, nullptr);
		vmaDestroyImage(_allocator, _depthImage._image, _depthImage._allocation);
		});
}

void VulkanEngine::init_default_renderpass()
{
	//we define an attachment description for our main color image
	//the attachment is loaded as "clear" when renderpass start
	//the attachment is stored when renderpass ends
	//the attachment layout starts as "undefined", and transitions to "Present" so its possible to display it
	//we dont care about stencil, and dont use multisampling

	VkAttachmentDescription color_attachment = {};
	color_attachment.format = _swachainImageFormat;
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

	VkAttachmentDescription depth_attachment = {};
	// Depth attachment
	depth_attachment.flags = 0;
	depth_attachment.format = _depthFormat;
	depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_attachment_ref = {};
	depth_attachment_ref.attachment = 1;
	depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//we are going to create 1 subpass, which is the minimum you can do
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;
	//hook the depth attachment into the subpass
	subpass.pDepthStencilAttachment = &depth_attachment_ref;

	//1 dependency, which is from "outside" into the subpass. And we can read or write color
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


	//array of 2 attachments, one for the color, and other for depth
	VkAttachmentDescription attachments[2] = { color_attachment,depth_attachment };

	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	//2 attachments from said array
	render_pass_info.attachmentCount = 2;
	render_pass_info.pAttachments = &attachments[0];
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;
	//render_pass_info.dependencyCount = 1;
	//render_pass_info.pDependencies = &dependency;

	VK_CHECK(vkCreateRenderPass(_device, &render_pass_info, nullptr, &_renderPass));

	_mainDeletionQueue.push_function([=]() {
		vkDestroyRenderPass(_device, _renderPass, nullptr);
		});
}

void VulkanEngine::init_framebuffers()
{
	//create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
	VkFramebufferCreateInfo fb_info = vkinit::framebuffer_create_info(_renderPass, _windowExtent);

	const uint32_t swapchain_imagecount = _swapchainImages.size();
	_framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

	for (int i = 0; i < swapchain_imagecount; i++) {

		VkImageView attachments[2];
		attachments[0] = _swapchainImageViews[i];
		attachments[1] = _depthImageView;

		fb_info.pAttachments = attachments;
		fb_info.attachmentCount = 2;
		VK_CHECK(vkCreateFramebuffer(_device, &fb_info, nullptr, &_framebuffers[i]));

		_mainDeletionQueue.push_function([=]() {
			vkDestroyFramebuffer(_device, _framebuffers[i], nullptr);
			vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
			});
	}
}

void VulkanEngine::init_commands()
{
	//create a command pool for commands submitted to the graphics queue.
	//we also want the pool to allow for resetting of individual command buffers
	VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);


	for (int i = 0; i < FRAME_OVERLAP; i++) {


		VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_frames[i]._commandPool));

		//allocate the default command buffer that we will use for rendering
		VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_frames[i]._commandPool, 1);

		VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_frames[i]._mainCommandBuffer));

		_mainDeletionQueue.push_function([=]() {
			vkDestroyCommandPool(_device, _frames[i]._commandPool, nullptr);
			});
	}


	VkCommandPoolCreateInfo uploadCommandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily);
	//create pool for upload context
	VK_CHECK(vkCreateCommandPool(_device, &uploadCommandPoolInfo, nullptr, &_uploadContext._commandPool));

	_mainDeletionQueue.push_function([=]() {
		vkDestroyCommandPool(_device, _uploadContext._commandPool, nullptr);
		});
}

void VulkanEngine::init_sync_structures()
{
	//create syncronization structures
	//one fence to control when the gpu has finished rendering the frame,
	//and 2 semaphores to syncronize rendering with swapchain
	//we want the fence to start signalled so we can wait on it on the first frame
	VkFenceCreateInfo fenceCreateInfo = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);

	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphore_create_info();

	for (int i = 0; i < FRAME_OVERLAP; i++) {

		VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_frames[i]._renderFence));

		//enqueue the destruction of the fence
		_mainDeletionQueue.push_function([=]() {
			vkDestroyFence(_device, _frames[i]._renderFence, nullptr);
			});


		VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._presentSemaphore));
		VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._renderSemaphore));

		//enqueue the destruction of semaphores
		_mainDeletionQueue.push_function([=]() {
			vkDestroySemaphore(_device, _frames[i]._presentSemaphore, nullptr);
			vkDestroySemaphore(_device, _frames[i]._renderSemaphore, nullptr);
			});
	}


	VkFenceCreateInfo uploadFenceCreateInfo = vkinit::fence_create_info();

	VK_CHECK(vkCreateFence(_device, &uploadFenceCreateInfo, nullptr, &_uploadContext._uploadFence));
	_mainDeletionQueue.push_function([=]() {
		vkDestroyFence(_device, _uploadContext._uploadFence, nullptr);
		});
}


void VulkanEngine::init_pipelines()
{
	VkShaderModule colorMeshShader;

	ShaderModule colorModule;

	if (!vkutil::load_shader_module(_device,"../../shaders/default_lit.frag.spv", &colorModule))
	{
		std::cout << "Error when building the colored mesh shader" << std::endl;
	}
	colorMeshShader = colorModule.module;



	VkShaderModule texturedMeshShader;
	ShaderModule textureModule;
	if (!vkutil::load_shader_module(_device ,"../../shaders/textured_lit.frag.spv", &textureModule))
	{
		std::cout << "Error when building the colored mesh shader" << std::endl;
	}
	texturedMeshShader = textureModule.module;
	VkShaderModule meshVertShader;
	ShaderModule meshModule;
	if (!vkutil::load_shader_module(_device,"../../shaders/tri_mesh_ssbo.vert.spv", &meshModule))
	{
		std::cout << "Error when building the mesh vertex shader module" << std::endl;
	}
	meshVertShader = meshModule.module;

	ShaderEffect* mainEffect = new ShaderEffect();
	mainEffect->add_stage(&meshModule, VK_SHADER_STAGE_VERTEX_BIT);
	mainEffect->add_stage(&colorModule, VK_SHADER_STAGE_FRAGMENT_BIT);


	ShaderEffect::ReflectionOverrides overrides[] = { 
		{"sceneData", VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC},
		{"cameraData", VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC}	
	};
	mainEffect->reflect_layout(this, overrides, 2);

	//build the stage-create-info for both vertex and fragment stages. This lets the pipeline know the shader modules per stage
	PipelineBuilder pipelineBuilder;

	pipelineBuilder._shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, meshVertShader));

	pipelineBuilder._shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, colorMeshShader));


	VkPipelineLayout meshPipLayout = mainEffect->builtLayout;

	
	ShaderEffect* texturedEffect = new ShaderEffect();;
	texturedEffect->add_stage(&meshModule, VK_SHADER_STAGE_VERTEX_BIT);
	texturedEffect->add_stage(&textureModule, VK_SHADER_STAGE_FRAGMENT_BIT);

	texturedEffect->reflect_layout(this, overrides, 2);

	VkPipelineLayout texturedPipeLayout = texturedEffect->builtLayout;
	

	//hook the push constants layout
	pipelineBuilder._pipelineLayout = meshPipLayout;

	//vertex input controls how to read vertices from vertex buffers. We arent using it yet
	pipelineBuilder._vertexInputInfo = vkinit::vertex_input_state_create_info();

	//input assembly is the configuration for drawing triangle lists, strips, or individual points.
	//we are just going to draw triangle list
	pipelineBuilder._inputAssembly = vkinit::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

	//build viewport and scissor from the swapchain extents
	pipelineBuilder._viewport.x = 0.0f;
	pipelineBuilder._viewport.y = 0.0f;
	pipelineBuilder._viewport.width = (float)_windowExtent.width;
	pipelineBuilder._viewport.height = (float)_windowExtent.height;
	pipelineBuilder._viewport.minDepth = 0.0f;
	pipelineBuilder._viewport.maxDepth = 1.0f;

	pipelineBuilder._scissor.offset = { 0, 0 };
	pipelineBuilder._scissor.extent = _windowExtent;

	//configure the rasterizer to draw filled triangles
	pipelineBuilder._rasterizer = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_FILL);

	//we dont use multisampling, so just run the default one
	pipelineBuilder._multisampling = vkinit::multisampling_state_create_info();

	//a single blend attachment with no blending and writing to RGBA
	pipelineBuilder._colorBlendAttachment = vkinit::color_blend_attachment_state();


	//default depthtesting
	pipelineBuilder._depthStencil = vkinit::depth_stencil_create_info(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);

	//build the mesh pipeline

	VertexInputDescription vertexDescription = Vertex::get_vertex_description();

	//connect the pipeline builder vertex input info to the one we get from Vertex
	pipelineBuilder._vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes.data();
	pipelineBuilder._vertexInputInfo.vertexAttributeDescriptionCount = vertexDescription.attributes.size();

	pipelineBuilder._vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings.data();
	pipelineBuilder._vertexInputInfo.vertexBindingDescriptionCount = vertexDescription.bindings.size();


	//build the mesh triangle pipeline
	VkPipeline meshPipeline = pipelineBuilder.build_pipeline(_device, _renderPass);

	create_material(meshPipeline, mainEffect, "defaultmesh");

	pipelineBuilder._shaderStages.clear();
	pipelineBuilder._shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, meshVertShader));

	pipelineBuilder._shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, texturedMeshShader));

	pipelineBuilder._pipelineLayout = texturedPipeLayout;
	VkPipeline texPipeline = pipelineBuilder.build_pipeline(_device, _renderPass);
	create_material(texPipeline, texturedEffect, "texturedmesh");


	vkDestroyShaderModule(_device, meshVertShader, nullptr);
	vkDestroyShaderModule(_device, colorMeshShader, nullptr);

	_mainDeletionQueue.push_function([=]() {
		vkDestroyPipeline(_device, meshPipeline, nullptr);

		vkDestroyPipelineLayout(_device, meshPipLayout, nullptr);
		});
}


VkPipeline PipelineBuilder::build_pipeline(VkDevice device, VkRenderPass pass)
{
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

	//its easy to error out on create graphics pipeline, so we handle it a bit better than the common VK_CHECK case
	VkPipeline newPipeline;
	if (vkCreateGraphicsPipelines(
		device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS) {
		std::cout << "failed to create pipline\n";
		return VK_NULL_HANDLE; // failed to create graphics pipeline
	}
	else
	{
		return newPipeline;
	}
}

void VulkanEngine::load_meshes()
{
	Mesh triMesh{};
	//make the array 3 vertices long
	triMesh._vertices.resize(3);

	//vertex positions
	triMesh._vertices[0].position = { 1.f,1.f, 0.0f };
	triMesh._vertices[1].position = { -1.f,1.f, 0.0f };
	triMesh._vertices[2].position = { 0.f,-1.f, 0.0f };

	//vertex colors, all green
	triMesh._vertices[0].color = { 0.f,1.f, 0.0f }; //pure green
	triMesh._vertices[1].color = { 0.f,1.f, 0.0f }; //pure green
	triMesh._vertices[2].color = { 0.f,1.f, 0.0f }; //pure green
	//we dont care about the vertex normals

	//load the monkey
	Mesh monkeyMesh{};
	//monkeyMesh.load_from_obj("../../assets/monkey_smooth.obj");
	monkeyMesh.load_from_meshasset("../../assets/monkey_smooth.mesh");

	Mesh lostEmpire{};
	//lostEmpire.load_from_obj("../../assets/lost_empire.obj");
	lostEmpire.load_from_meshasset("../../assets/lost_empire.mesh");

	Mesh helmet1{};
	//lostEmpire.load_from_obj("../../assets/lost_empire.obj");
	helmet1.load_from_meshasset("../../assets/FlightHelmet/FlightHelmet/LeatherParts_low.mesh");




	upload_mesh(triMesh);
	upload_mesh(monkeyMesh);
	upload_mesh(lostEmpire);
	upload_mesh(helmet1);


	_meshes["monkey"] = monkeyMesh;
	_meshes["helmet1"] = helmet1;
	_meshes["triangle"] = triMesh;
	_meshes["empire"] = lostEmpire;

	{
		Mesh helmet{};
		//lostEmpire.load_from_obj("../../assets/lost_empire.obj");
		helmet.load_from_meshasset("../../assets/FlightHelmet/FlightHelmet/GlassPlastic_low.mesh");
		upload_mesh(helmet);
		_meshes["helmet2"] = helmet;
	}
	{
		Mesh helmet{};
		//lostEmpire.load_from_obj("../../assets/lost_empire.obj");
		helmet.load_from_meshasset("../../assets/FlightHelmet/FlightHelmet/Hose_low.mesh");
		upload_mesh(helmet);
		_meshes["helmet3"] = helmet;
	}
	{
		Mesh helmet{};
		//lostEmpire.load_from_obj("../../assets/lost_empire.obj");
		helmet.load_from_meshasset("../../assets/FlightHelmet/FlightHelmet/Lenses_low.mesh");
		upload_mesh(helmet);
		_meshes["helmet4"] = helmet;
	}
	{
		Mesh helmet{};
		//lostEmpire.load_from_obj("../../assets/lost_empire.obj");
		helmet.load_from_meshasset("../../assets/FlightHelmet/FlightHelmet/MetalParts_low.mesh");
		upload_mesh(helmet);
		_meshes["helmet5"] = helmet;
	}
	{
		Mesh helmet{};
		
		helmet.load_from_meshasset("../../assets/FlightHelmet/FlightHelmet/RubberWood_low.mesh");
		upload_mesh(helmet);
		_meshes["helmet6"] = helmet;
	}
}


void VulkanEngine::load_images()
{
	load_image_to_cache("empire_diffuse", "../../assets/lost_empire-RGBA.tx");

	load_image_to_cache("GlassPlastic_diffuse", "../../assets/FlightHelmet/FlightHelmet_Materials_GlassPlasticMat_BaseColor.tx");
	load_image_to_cache("LeatherParts_diffuse", "../../assets/FlightHelmet/FlightHelmet_Materials_LeatherPartsMat_BaseColor.tx");

	load_image_to_cache("MetalPartsMat_diffuse", "../../assets/FlightHelmet/FlightHelmet_Materials_MetalPartsMat_BaseColor.tx");
	load_image_to_cache("LensesMat_diffuse", "../../assets/FlightHelmet/FlightHelmet_Materials_LensesMat_BaseColor.tx");

	load_image_to_cache("RubberWoodMat_diffuse", "../../assets/FlightHelmet/FlightHelmet_Materials_RubberWoodMat_BaseColor.tx");
}


void VulkanEngine::load_image_to_cache(const char* name, const char* path)
{
	ZoneScopedNC("Load Texture", tracy::Color::Yellow);
	Texture newtex;

	vkutil::load_image_from_asset(*this, path, newtex.image);

	VkImageViewCreateInfo imageinfo = vkinit::imageview_create_info(VK_FORMAT_R8G8B8A8_UNORM, newtex.image._image, VK_IMAGE_ASPECT_COLOR_BIT);
	vkCreateImageView(_device, &imageinfo, nullptr, &newtex.imageView);

	_loadedTextures[name] = newtex;
}

void VulkanEngine::upload_mesh(Mesh& mesh)
{
	ZoneScopedNC("Upload Mesh", tracy::Color::Orange);


	const size_t vertex_buffer_size = mesh._vertices.size() * sizeof(Vertex);
	const size_t index_buffer_size = mesh._indices.size() * sizeof(uint32_t);
	const size_t bufferSize = vertex_buffer_size + index_buffer_size;
	//allocate vertex buffer
	VkBufferCreateInfo stagingBufferInfo = {};
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.pNext = nullptr;
	//this is the total size, in bytes, of the buffer we are allocating
	stagingBufferInfo.size = bufferSize;
	//this buffer is going to be used as a Vertex Buffer
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;


	//let the VMA library know that this data should be writeable by CPU, but also readable by GPU
	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

	AllocatedBuffer stagingBuffer;

	//allocate the buffer
	VK_CHECK(vmaCreateBuffer(_allocator, &stagingBufferInfo, &vmaallocInfo,
		&stagingBuffer._buffer,
		&stagingBuffer._allocation,
		nullptr));

	//copy vertex data
	char* data;
	vmaMapMemory(_allocator, stagingBuffer._allocation, (void**)&data);

	memcpy(data, mesh._vertices.data(), vertex_buffer_size);
	memcpy(data + vertex_buffer_size, mesh._indices.data(), index_buffer_size);

	vmaUnmapMemory(_allocator, stagingBuffer._allocation);


	//allocate vertex buffer
	VkBufferCreateInfo vertexBufferInfo = {};
	vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vertexBufferInfo.pNext = nullptr;
	//this is the total size, in bytes, of the buffer we are allocating
	vertexBufferInfo.size = vertex_buffer_size;
	//this buffer is going to be used as a Vertex Buffer
	vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	//let the VMA library know that this data should be gpu native	
	vmaallocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	//allocate the buffer
	VK_CHECK(vmaCreateBuffer(_allocator, &vertexBufferInfo, &vmaallocInfo,
		&mesh._vertexBuffer._buffer,
		&mesh._vertexBuffer._allocation,
		nullptr));

	if (index_buffer_size > 0) {
		//allocate index buffer
		VkBufferCreateInfo indexBufferInfo = {};
		indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		indexBufferInfo.pNext = nullptr;
		//this is the total size, in bytes, of the buffer we are allocating
		indexBufferInfo.size = index_buffer_size;
		//this buffer is going to be used as a index Buffer
		indexBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		//allocate the buffer
		VK_CHECK(vmaCreateBuffer(_allocator, &indexBufferInfo, &vmaallocInfo,
			&mesh._indexBuffer._buffer,
			&mesh._indexBuffer._allocation,
			nullptr));
	}
	


	//add the destruction of triangle mesh buffer to the deletion queue
	_mainDeletionQueue.push_function([=]() {
	
		vmaDestroyBuffer(_allocator, mesh._vertexBuffer._buffer, mesh._vertexBuffer._allocation);
		if (index_buffer_size > 0) {
			vmaDestroyBuffer(_allocator, mesh._indexBuffer._buffer, mesh._indexBuffer._allocation);
		}
	});

	immediate_submit([=](VkCommandBuffer cmd) {
		VkBufferCopy copy;
		copy.dstOffset = 0;
		copy.srcOffset = 0;
		copy.size = vertex_buffer_size;
		vkCmdCopyBuffer(cmd, stagingBuffer._buffer, mesh._vertexBuffer._buffer, 1, &copy);

		if (index_buffer_size > 0) {
			copy.dstOffset = 0;
			copy.srcOffset = vertex_buffer_size;
			copy.size = index_buffer_size;
			vkCmdCopyBuffer(cmd, stagingBuffer._buffer, mesh._indexBuffer._buffer, 1, &copy);
		}
	});

	vmaDestroyBuffer(_allocator, stagingBuffer._buffer, stagingBuffer._allocation);
}


Material* VulkanEngine::create_material(VkPipeline pipeline, ShaderEffect* effect, const std::string& name)
{
	Material mat;
	mat.pipeline = pipeline;
	mat.effect = effect;
	_materials[name] = mat;
	return &_materials[name];
}

Material* VulkanEngine::clone_material(const std::string& originalname, const std::string& copyname)
{
	Material* m = get_material(originalname);

	Material mat;
	mat.pipeline = m->pipeline;
	mat.effect = m->effect;
	_materials[copyname] = mat;
	return &_materials[copyname];
	
}

Material* VulkanEngine::get_material(const std::string& name)
{
	//search for the object, and return nullpointer if not found
	auto it = _materials.find(name);
	if (it == _materials.end()) {
		return nullptr;
	}
	else {
		return &(*it).second;
	}
}


Mesh* VulkanEngine::get_mesh(const std::string& name)
{
	auto it = _meshes.find(name);
	if (it == _meshes.end()) {
		return nullptr;
	}
	else {
		return &(*it).second;
	}
}


void VulkanEngine::draw_objects(VkCommandBuffer cmd, RenderObject* first, int count)
{
	ZoneScopedNC("DrawObjects", tracy::Color::Blue);
	//make a model view matrix for rendering the object
	//camera view
	glm::vec3 camPos = _camera.position;
	
	glm::mat4 cam_rot = _camera.get_rotation_matrix();

	glm::mat4 view = glm::translate(glm::mat4{ 1 }, camPos) * cam_rot;
	
	//we need to invert the camera matrix
	view = glm::inverse(view);

	//camera projection
	glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
	projection[1][1] *= -1;

	GPUCameraData camData;
	camData.proj = projection;
	camData.view = view;
	camData.viewproj = projection * view;

	float framed = (_frameNumber / 120.f);
	_sceneParameters.ambientColor = { sin(framed),0,cos(framed),1 };

	void* objectData;
	vmaMapMemory(_allocator, get_current_frame().objectBuffer._allocation, &objectData);

	GPUObjectData* objectSSBO = (GPUObjectData*)objectData;

	for (int i = 0; i < count; i++)
	{
		RenderObject& object = first[i];
		objectSSBO[i].modelMatrix = object.transformMatrix;
	}

	vmaUnmapMemory(_allocator, get_current_frame().objectBuffer._allocation);

	//push data to dynmem
	uint32_t camera_data_offsets[3];
	uint32_t scene_data_offset;

	uint32_t dyn_offset = 0;

	char* dynData;
	vmaMapMemory(_allocator, get_current_frame().dynamicDataBuffer._allocation, (void**)&dynData);

	for (int i = 0; i < 3; i++) {
		camera_data_offsets[i] = dyn_offset;
		memcpy(dynData, &camData, sizeof(GPUCameraData));
		dyn_offset += sizeof(GPUCameraData);
		dyn_offset = pad_uniform_buffer_size(dyn_offset);

		dynData += dyn_offset;
	}
	

	scene_data_offset = dyn_offset;
	memcpy(dynData, &_sceneParameters, sizeof(GPUSceneData));

	vmaUnmapMemory(_allocator, get_current_frame().dynamicDataBuffer._allocation);

	Mesh* lastMesh = nullptr;
	Material* lastMaterial = nullptr;
	VkPipeline lastPipeline = VK_NULL_HANDLE;
	ShaderDescriptorBinder binder{};

	VkDescriptorBufferInfo objectBufferInfo;
	objectBufferInfo.buffer = get_current_frame().objectBuffer._buffer;
	objectBufferInfo.offset = 0;
	objectBufferInfo.range = sizeof(GPUObjectData) * 10000;

	VkDescriptorBufferInfo dynamicInfo;
	dynamicInfo.buffer = get_current_frame().dynamicDataBuffer._buffer;;
	dynamicInfo.offset = 0;
	dynamicInfo.range = 100;

	VkDescriptorSet GlobalSet;
	vkutil::DescriptorBuilder::begin(_descriptorLayoutCache, get_current_frame().dynamicDescriptorAllocator)
		.bind_buffer(0, &dynamicInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT )
		.bind_buffer(1, &dynamicInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build(GlobalSet);

	VkDescriptorSet ObjectDataSet;
	vkutil::DescriptorBuilder::begin(_descriptorLayoutCache, get_current_frame().dynamicDescriptorAllocator)
		.bind_buffer(0, &objectBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.build(ObjectDataSet);

	{
		ZoneScopedNC("Draw Loop", tracy::Color::Blue2);

		struct RenderBatch {
			RenderObject* object;
			uint64_t sortKey;
			uint64_t objectIndex;
		};

		std::vector<RenderBatch> batch;
		batch.resize(count);
		for (int i = 0; i < count; i++) {
			RenderObject* object = &first[i];

			batch[i].object = object;
			batch[i].objectIndex = i;
			uint64_t material_hash = std::hash<void*>()(object->material) & UINT32_MAX;
			uint64_t mesh_hash = std::hash<void*>()(object->mesh) & UINT32_MAX;

			batch[i].sortKey = material_hash << 32 | mesh_hash;
		}

		std::sort(batch.begin(), batch.end(), [](const RenderBatch& A,const RenderBatch& B) {
			return A.sortKey < B.sortKey;
		});

		for (int i = 0; i < count; i++)
		{
			RenderObject& object = *batch[i].object;

			//only bind the pipeline if it doesnt match with the already bound one
			if (object.material != lastMaterial) {

				if (object.material->pipeline != lastPipeline) {
					lastPipeline = object.material->pipeline;
					vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipeline);
					vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->effect->builtLayout, 1, 1, &ObjectDataSet, 0, nullptr);
				}
				
				lastMaterial = object.material;

				
			}

			//update dynamic binds
			uint32_t dynamicBinds[] = { camera_data_offsets[0],scene_data_offset };
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->effect->builtLayout, 0, 1, &GlobalSet, 2, dynamicBinds);

			if (object.material->textureSet != VK_NULL_HANDLE) {
				//texture descriptor
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->effect->builtLayout, 2, 1, &object.material->textureSet, 0, nullptr);
			}

			glm::mat4 model = object.transformMatrix;
			//final render matrix, that we are calculating on the cpu
			glm::mat4 mesh_matrix = model;

			MeshPushConstants constants;
			constants.render_matrix = mesh_matrix;

			//upload the mesh to the gpu via pushconstants
			vkCmdPushConstants(cmd, object.material->effect->builtLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);

			//only bind the mesh if its a different one from last bind
			if (object.mesh != lastMesh) {
				//bind the mesh vertex buffer with offset 0
				VkDeviceSize offset = 0;
				vkCmdBindVertexBuffers(cmd, 0, 1, &object.mesh->_vertexBuffer._buffer, &offset);
				lastMesh = object.mesh;

				if (object.mesh->_indexBuffer._buffer != VK_NULL_HANDLE) {
					vkCmdBindIndexBuffer(cmd, object.mesh->_indexBuffer._buffer, 0, VK_INDEX_TYPE_UINT32);
				}

			}
			//we can now draw

			bool bHasIndices = object.mesh->_indices.size() > 0;
			if (!bHasIndices) {
				vkCmdDraw(cmd, object.mesh->_vertices.size(), 1, 0, batch[i].objectIndex);
			}
			else {
				vkCmdDrawIndexed(cmd, object.mesh->_indices.size(), 1, 0, 0, batch[i].objectIndex);
			}
		}
	}
}

void VulkanEngine::init_scene()
{
	VkSamplerCreateInfo samplerInfo = vkinit::sampler_create_info(VK_FILTER_NEAREST);

	VkSampler blockySampler;
	vkCreateSampler(_device, &samplerInfo, nullptr, &blockySampler);

	samplerInfo = vkinit::sampler_create_info(VK_FILTER_LINEAR);

	VkSampler smoothSampler;
	vkCreateSampler(_device, &samplerInfo, nullptr, &smoothSampler);


	RenderObject monkey;
	monkey.mesh = get_mesh("monkey");
	monkey.material = get_material("defaultmesh");
	monkey.transformMatrix = glm::mat4{ 1.0f };

	for (int x = -10; x <= 10; x++) {
		for (int y = -10; y <= 10; y++) {

			glm::mat4 translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(x * 5, 0, y * 5));
			glm::mat4 scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(10));

			load_prefab("K:/Programming/vulkan-guide-otherbranch/assets/FlightHelmet/FlightHelmet.pfb", translation * scale);
		}
	}

	glm::mat4 sponzaMatrix = glm::scale(glm::mat4{ 1.0 }, glm::vec3(0.1, 0.1, 0.1));;

	load_prefab("K:/Programming/vulkan-guide-otherbranch/assets/Sponza/Sponza.pfb", sponzaMatrix);

	for (int x = -20; x <= 20; x++) {
		for (int y = -20; y <= 20; y++) {

			RenderObject tri;
			tri.mesh = get_mesh("triangle");
			tri.material = get_material("defaultmesh");
			glm::mat4 translation = glm::translate(glm::mat4{ 1.0 }, glm::vec3(x, 0, y));
			glm::mat4 scale = glm::scale(glm::mat4{ 1.0 }, glm::vec3(0.2, 0.2, 0.2));
			tri.transformMatrix = translation * scale;

			_renderables.push_back(tri);
		}
	}

	

	

	build_texture_set(blockySampler, get_material("texturedmesh"), "empire_diffuse");



}


void VulkanEngine::build_texture_set(VkSampler blockySampler, Material* texturedMat, const char * textureName)
{
	VkDescriptorImageInfo imageBufferInfo;
	imageBufferInfo.sampler = blockySampler;
	imageBufferInfo.imageView = _loadedTextures[textureName].imageView;
	imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	vkutil::DescriptorBuilder::begin(_descriptorLayoutCache, _descriptorAllocator)
		.bind_image(0, &imageBufferInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build(texturedMat->textureSet);
}

AllocatedBuffer VulkanEngine::create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
{
	//allocate vertex buffer
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = nullptr;
	bufferInfo.size = allocSize;

	bufferInfo.usage = usage;


	//let the VMA library know that this data should be writeable by CPU, but also readable by GPU
	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = memoryUsage;

	AllocatedBuffer newBuffer;

	//allocate the buffer
	VK_CHECK(vmaCreateBuffer(_allocator, &bufferInfo, &vmaallocInfo,
		&newBuffer._buffer,
		&newBuffer._allocation,
		nullptr));

	return newBuffer;
}

size_t VulkanEngine::pad_uniform_buffer_size(size_t originalSize)
{
	// Calculate required alignment based on minimum device offset alignment
	size_t minUboAlignment = _gpuProperties.limits.minUniformBufferOffsetAlignment;
	size_t alignedSize = originalSize;
	if (minUboAlignment > 0) {
		alignedSize = (alignedSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
	}
	return alignedSize;
}


void VulkanEngine::immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function)
{
	ZoneScopedNC("Inmediate Submit", tracy::Color::White);

	VkCommandBuffer cmd;

	//allocate the default command buffer that we will use for rendering
	VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_uploadContext._commandPool, 1);

	VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &cmd));

	//begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));


	function(cmd);


	VK_CHECK(vkEndCommandBuffer(cmd));

	VkSubmitInfo submit = vkinit::submit_info(&cmd);


	//submit command buffer to the queue and execute it.
	// _renderFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit(_graphicsQueue, 1, &submit, _uploadContext._uploadFence));

	vkWaitForFences(_device, 1, &_uploadContext._uploadFence, true, 9999999999);
	vkResetFences(_device, 1, &_uploadContext._uploadFence);

	vkResetCommandPool(_device, _uploadContext._commandPool, 0);
}


bool VulkanEngine::load_prefab(const char* path, glm::mat4 root)
{
	ZoneScopedNC("Load Prefab", tracy::Color::Red);
	
	auto pf = _prefabCache.find(path);
	if (pf == _prefabCache.end())
	{
		assets::AssetFile file;
		bool loaded = assets::load_binaryfile(path, file);

		if (!loaded) {
			std::cout << "Error when loading prefab ";
			return false;
		}

		_prefabCache[path] = new assets::PrefabInfo;
		
		*_prefabCache[path] = assets::read_prefab_info(&file);
	}

	assets::PrefabInfo* prefab = _prefabCache[path];

	VkSamplerCreateInfo samplerInfo = vkinit::sampler_create_info(VK_FILTER_LINEAR);

	

	VkSampler smoothSampler;
	vkCreateSampler(_device, &samplerInfo, nullptr, &smoothSampler);

	for (auto& [k, v] : prefab->node_meshes)
	{
		//load mesh

		if (!get_mesh(v.mesh_path.c_str()))
		{
			Mesh mesh{};
			mesh.load_from_meshasset(v.mesh_path.c_str());

			upload_mesh(mesh);

			_meshes[v.mesh_path.c_str()] = mesh;
		}		

		//load material

		if (!get_material(v.material_path.c_str()))
		{
			assets::AssetFile materialFile;
			bool loaded = assets::load_binaryfile(v.material_path.c_str(), materialFile);

			assets::MaterialInfo material = assets::read_material_info(&materialFile);

			auto texture = material.textures["baseColor"];
			load_image_to_cache(texture.c_str(), texture.c_str());

			Material* mat = clone_material("texturedmesh", v.material_path.c_str());

			build_texture_set(smoothSampler, mat, texture.c_str());
		}
		

		RenderObject loadmesh;
		loadmesh.mesh = get_mesh(v.mesh_path.c_str());
		loadmesh.transformMatrix = root;		
		loadmesh.material = get_material(v.material_path.c_str());

		_renderables.push_back(loadmesh);
	}

	return true;
}

void VulkanEngine::init_descriptors()
{
	_descriptorAllocator = new vkutil::DescriptorAllocator{};
	_descriptorAllocator->init(_device);

	_descriptorLayoutCache = new vkutil::DescriptorLayoutCache{};
	_descriptorLayoutCache->init(_device);


	VkDescriptorSetLayoutBinding textureBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0);
	
	VkDescriptorSetLayoutCreateInfo set3info = {};
	set3info.bindingCount = 1;
	set3info.flags = 0;
	set3info.pNext = nullptr;
	set3info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	set3info.pBindings = &textureBind;
	
	_singleTextureSetLayout = _descriptorLayoutCache->create_descriptor_layout(&set3info);
	
	
	const size_t sceneParamBufferSize = FRAME_OVERLAP * pad_uniform_buffer_size(sizeof(GPUSceneData));	
	
	for (int i = 0; i < FRAME_OVERLAP; i++)
	{
		_frames[i].dynamicDescriptorAllocator = new vkutil::DescriptorAllocator{};
		_frames[i].dynamicDescriptorAllocator->init(_device);
		
		const int MAX_OBJECTS = 10000;
		_frames[i].objectBuffer = create_buffer(sizeof(GPUObjectData) * MAX_OBJECTS, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		//1 megabyte of dynamic data buffer
		_frames[i].dynamicDataBuffer = create_buffer( 10000000 , VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	}
}

void VulkanEngine::init_imgui()
{
	//1: create descriptor pool for IMGUI
	// the size of the pool is very oversize, but its copied from imgui demo itself.
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	VkDescriptorPool imguiPool;
	VK_CHECK(vkCreateDescriptorPool(_device, &pool_info, nullptr, &imguiPool));


	// 2: initialize imgui library

	//this initializes the core structures of imgui
	ImGui::CreateContext();

	//this initializes imgui for SDL
	ImGui_ImplSDL2_InitForVulkan(_window);

	//this initializes imgui for Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = _instance;
	init_info.PhysicalDevice = _chosenGPU;
	init_info.Device = _device;
	init_info.Queue = _graphicsQueue;
	init_info.DescriptorPool = imguiPool;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;

	ImGui_ImplVulkan_Init(&init_info, _renderPass);

	//execute a gpu command to upload imgui font textures
	immediate_submit([&](VkCommandBuffer cmd) {
		ImGui_ImplVulkan_CreateFontsTexture(cmd);
		});

	//clear font textures from cpu data
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	//add the destroy the imgui created structures
	_mainDeletionQueue.push_function([=]() {

		vkDestroyDescriptorPool(_device, imguiPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
		});
}

glm::mat4 PlayerCamera::get_rotation_matrix()
{
	glm::mat4 yaw_rot = glm::rotate(glm::mat4{ 1 }, yaw, { 0,1,0 });
	glm::mat4 pitch_rot = glm::rotate(glm::mat4{ yaw_rot }, pitch, { 1,0,0 });

	return pitch_rot;
}
