---
layout: default
title: Improving the render loop
parent:  "New 2. Drawing with Compute"
nav_order: 1
---

Before we begin drawing, we need to implement a couple other things. First we have a deletion queue that will allow us to safely handle the cleanup of a growing amount of objects, and then we will change the render loop to draw into a non-swapchain image and then copy it to the swapchain.


## Deletion queue
As we begin to add more and more vulkan structures, we need a way to handle their destruction. We could keep adding more things into the `cleanup()` function, but that would not scale and would be very annoying to keep synced correctly. 
We are going to add a new structure to the engine, called a DeletionQueue. This is a common approach by lots of engines, where we add the objects we want to delete into some queue, and then run that queue to delete all the objects in the correct orders.
In our implementation, we are going to keep it simple, and have it store std::function callbacks in a deque. We will be using that deque as a First In Last Out queue, so that when we flush the deletion queue, it first destroys the objects that were added into it last.

This is the entire implementation.

```cpp
struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& function) {
		deletors.push_back(function);
	}

	void flush() {
		// reverse iterate the deletion queue to execute all the functions
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
			(*it)(); //call functors
		}

		deletors.clear();
	}
};
```

std::function stores a lambda, and we can use it to store a callback with some data, which is perfect for this case. 

Doing callbacks like this is inneficient at scale, because we are storing whole std::functions for every object we are deleting, which is not going to be optimal. For the amount of objects we will use in this tutorial, its going to be fine. but if you need to delete thousands of objects and want them deleted faster, a better implementation would be to store arrays of vulkan handles of various types such as VkImage, VkBuffer, and so on. And then delete those from a loop.

We will have the deletion queue in multiple places, for multiple lifetimes of objects. One of them is on the engine class itself, and will be flushed when the engine gets destroyed. Global objects go into that one. We will also store one deletion queue for each frame in flight, which will allow us to delete objects next frame after they are used. 

Add it into VulkanEngine class inside the main class, and inside the FrameData struct

```cpp
struct FrameData {
	 //other data
      DeletionQueue _deletionQueue;
};

class VulkanEngine{
    //other data
    DeletionQueue _mainDeletionQueue;
}

```

We then call it from 2 places, right after we wait on the Fence per frame, and from the cleanup() function after the WaitIdle call. By flushing it right after the fence, we make sure that the GPU has finished executing that frame so we can safely delete objects create for that specific frame only.

```cpp
void VulkanEngine::draw()
{
	//wait until the gpu has finished rendering the last frame. Timeout of 1 second
	VK_CHECK(vkWaitForFences(_device, 1, &get_current_frame()._renderFence, true, 1000000000));

	get_current_frame()._deletionQueue.flush();

    //other code
}

void VulkanEngine::cleanup()
{	
	if (_isInitialized) {
		
		//make sure the gpu has stopped doing its things
		vkDeviceWaitIdle(_device);
        _mainDeletionQueue.flush();

        //other code
    }

}
```

With the deletion queue set, now whenever we create new vulkan objects we can just add them into the queue.

## Memory Allocation

To improve the render loop, we will need to allocate a image, and this gets us into how to allocate objects in vulkan. We are going to skip that entire chapter, because we will be using Vulkan Memory Allocator library. Dealing with the different memory heaps and object restrictions such as image alignement is very error prone and really hard to get right, specially if you want to get it right at a decent performance. By using VMA, we skip all that, and we get a battle tested way that is guaranteed to work well. There are cases like the PCSX3 emulator project, where they replaced their attempt at allocation to VMA, and won 20% extra framerate. 

Add the allocator to the VulkanEngine class
```cpp
class VulkanEngine{

    VmaAllocator _allocator;
}
```

Now we will initialize it from `init_vulkan()` call, at the end of the function.
```cpp

	//initialize the memory allocator
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = _chosenGPU;
	allocatorInfo.device = _device;
	allocatorInfo.instance = _instance;
	vmaCreateAllocator(&allocatorInfo, &_allocator);

	_mainDeletionQueue.push_function([&]() {
		vmaDestroyAllocator(_allocator);
	});
```

There isnt much to explain it, we are initializing the _allocator member, and then adding its destruction function into the destruction queue so that it gets cleared when the engine exits. We hook the physical device, instance, and device to the creation function. Vulkan Memory Allocator library follows similar call conventions as the vulkan api, so it works with similar info structs.

# New draw loop.

Drawing directly into the swapchain is fine for many projects, and it can even be optimal in some cases such as phones. But it comes with a few restrictions. 
The most important of them is that the formats of the image used in the swapchain are not guaranteed. Different OS, drivers, and windowing modes can have different optimal swapchain formats. Things like HDR support also need their own very specific formats.
Another one is that we only get a swapchain image index from the windowing present system. There are low-latency techniques where we could be rendering into another image, and then directly push that image to the swapchain with very low latency. 

One very important limitation is that their resolution is fixed to whatever your window size is. If you want to have higher or lower resolution, and then do some scaling logic, you need to draw into a different image.

And last, swapchain formats are, for the most part, low precision. Some platforms with High Dynamic Range rendering have higher precision formats, but you will often default to 8 bits per color. So if you want high precision light calculations, system that would prevent banding, or to be able to go past 1.0 on the normalized color range, you will need a separate image for drawing.

For all those reasons, we will do the whole tutorial rendering into a separate image than the one from the swapchain. After we are doing with the drawing, we will then copy that image into the swapchain image and present it to the screen.

The image we will be using is going to be in the  RGBA 16-bit float format. This is slightly overkill, but will provide us with a lot of extra precision that will come in handy when doing lighting calculations and better rendering.


# Vulkan Images
We have already dealt superficially with images when setting up the swapchain, but it was handled by VkBootstrap. This time we will create the images ourselves.


Lets begin by adding the new members we will need to the VulkanEngine class.
We have an AllocatedImage structure that already holds format, size, imageview, and the image itself

```cpp
class VulkanEngine{

	//draw resources
	AllocatedImage _drawImage;
}
```

Lets check the vk_initializers function for image and imageview create info.

<!-- codegen from tag image_set on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_initializers.cpp --> 
```cpp
VkImageCreateInfo vkinit::image_create_info(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent)
{
    VkImageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;

    info.imageType = VK_IMAGE_TYPE_2D;

    info.format = format;
    info.extent = extent;

    info.mipLevels = 1;
    info.arrayLayers = 1;

    //for MSAA. we will not be using it by default, so default it to 1 sample per pixel.
    info.samples = VK_SAMPLE_COUNT_1_BIT;

    //optimal tiling, which means the image is stored on the best gpu format
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usageFlags;

    return info;
}

VkImageViewCreateInfo vkinit::imageview_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags)
{
    // build a image-view for the depth image to use for rendering
    VkImageViewCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.pNext = nullptr;

    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.image = image;
    info.format = format;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;
    info.subresourceRange.aspectMask = aspectFlags;

    return info;
}
```

We will hardcode the image tiling to OPTIMAL, which means that we allow the gpu to shuffle the data however it sees fit. If we want to read the image data from cpu, we would need to use tiling LINEAR, which makes the gpu data into a simple 2d array. This tiling is highly limited in what can be gpu do with it, so the only real use case for it is CPU readback.

On the imageview creation, we need to setup the subresource. Thats similar to the one we used in the pipeline barrier.

Now, at the end of init_swapchain, lets create it.

<!-- codegen from tag init_swap on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
	//depth image size will match the window
	VkExtent3D drawImageExtent = {
		_windowExtent.width,
		_windowExtent.height,
		1
	};

	//hardcoding the depth format to 32 bit float
	_drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;

	VkImageUsageFlags drawImageUsages{};
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;

	VkImageCreateInfo rimg_info = vkinit::image_create_info(_drawImage.imageFormat, drawImageUsages, drawImageExtent);

	//for the draw image, we want to allocate it from gpu local memory
	VmaAllocationCreateInfo rimg_allocinfo = {};
	rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	//allocate and create the image
	vmaCreateImage(_allocator, &rimg_info, &rimg_allocinfo, &_drawImage.image, &_drawImage.allocation, nullptr);

	//build a image-view for the draw image to use for rendering
	VkImageViewCreateInfo rview_info = vkinit::imageview_create_info(_drawImage.imageFormat, _drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

	VK_CHECK(vkCreateImageView(_device, &rview_info, nullptr, &_drawImage.imageView));

	//add to deletion queues
	_mainDeletionQueue.push_function([=]() {
		vkDestroyImageView(_device, _drawImage.imageView, nullptr);
		vmaDestroyImage(_allocator, _drawImage.image, _drawImage.allocation);
	});
	//< init_swap
}

void VulkanEngine::init_commands()
{
	//create a command pool for commands submitted to the graphics queue.
	//we also want the pool to allow for resetting of individual command buffers
	VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	for (int i = 0; i < FRAME_OVERLAP; i++) {

		VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_frames[i]._commandPool));

		// allocate the default command buffer that we will use for rendering
		VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_frames[i]._commandPool, 1);

		VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_frames[i]._mainCommandBuffer));
	}

//> imm_cmd
	VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_immCommandPool));

	// allocate the default command buffer that we will use for rendering
	VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_immCommandPool, 1);

	VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_immCommandBuffer));

	_mainDeletionQueue.push_function([=]() { 
	vkDestroyCommandPool(_device, _immCommandPool, nullptr);
	});

//< imm_cmd
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

		VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._swapchainSemaphore));
		VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._renderSemaphore));
	}

//> imm_fence
	VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_immFence));
	_mainDeletionQueue.push_function([=]() { vkDestroyFence(_device, _immFence, nullptr); });
//< imm_fence
}

//> imm_submit
void VulkanEngine::immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function)
{
	VK_CHECK(vkResetFences(_device, 1, &_immFence));
	VK_CHECK(vkResetCommandBuffer(_immCommandBuffer, 0));

	VkCommandBuffer cmd = _immCommandBuffer;

	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	function(cmd);

	VK_CHECK(vkEndCommandBuffer(cmd));

	VkCommandBufferSubmitInfo cmdinfo = vkinit::command_buffer_submit_info(cmd);
	VkSubmitInfo2 submit = vkinit::submit_info(&cmdinfo, nullptr, nullptr);

	// submit command buffer to the queue and execute it.
	//  _renderFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit2(_graphicsQueue, 1, &submit, _immFence));

	VK_CHECK(vkWaitForFences(_device, 1, &_immFence, true, 9999999999));
}
//< imm_submit


//> imgui_init
void VulkanEngine::init_imgui()
{
	// 1: create descriptor pool for IMGUI
	//  the size of the pool is very oversize, but it's copied from imgui demo
	//  itself.
	VkDescriptorPoolSize pool_sizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	VkDescriptorPool imguiPool;
	VK_CHECK(vkCreateDescriptorPool(_device, &pool_info, nullptr, &imguiPool));

	// 2: initialize imgui library

	// this initializes the core structures of imgui
	ImGui::CreateContext();

	// this initializes imgui for SDL
	ImGui_ImplSDL2_InitForVulkan(_window);

	// this initializes imgui for Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = _instance;
	init_info.PhysicalDevice = _chosenGPU;
	init_info.Device = _device;
	init_info.Queue = _graphicsQueue;
	init_info.DescriptorPool = imguiPool;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	init_info.UseDynamicRendering = true;
	init_info.ColorAttachmentFormat = _swapchainImageFormat;

	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	ImGui_ImplVulkan_Init(&init_info, VK_NULL_HANDLE);

	// execute a gpu command to upload imgui font textures
	immediate_submit([&](VkCommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(cmd); });

	// clear font textures from cpu data
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	// add the destroy the imgui created structures
	_mainDeletionQueue.push_function([=]() {
		vkDestroyDescriptorPool(_device, imguiPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
	});
}
//< imgui_init

void VulkanEngine::init_pipelines()
{

#if CHAPTER_STAGE < 2
//> comp_pipeline_1
	VkPipelineLayoutCreateInfo computeLayout{};
	computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	computeLayout.pNext = nullptr;
	computeLayout.pSetLayouts = &_drawImageDescriptorLayout;
	computeLayout.setLayoutCount = 1;

	VK_CHECK(vkCreatePipelineLayout(_device, &computeLayout, nullptr, &_gradientPipelineLayout));
//< comp_pipeline_1
#else
//> comp_pipeline_pc
VkPipelineLayoutCreateInfo computeLayout{};
computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
computeLayout.pNext = nullptr;
computeLayout.pSetLayouts = &_drawImageDescriptorLayout;
computeLayout.setLayoutCount = 1;

VkPushConstantRange pushConstant{};
pushConstant.offset = 0;
pushConstant.size = sizeof(ComputePushConstants) ;
pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

computeLayout.pPushConstantRanges = &pushConstant;
computeLayout.pushConstantRangeCount = 1;

VK_CHECK(vkCreatePipelineLayout(_device, &computeLayout, nullptr, &_gradientPipelineLayout));
//< comp_pipeline_pc
#endif

#if CHAPTER_STAGE < 2
//> comp_pipeline_2
	VkShaderModule computeDrawShader;
	if (!vkutil::load_shader_module("../../shaders/gradient_color.comp.spv", _device, &computeDrawShader))
	{
		std::cout << "Error when building the compute shader" << std::endl;
	}

	VkPipelineShaderStageCreateInfo stageinfo{};
	stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageinfo.pNext = nullptr;
	stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	stageinfo.module = computeDrawShader;
	stageinfo.pName = "main";

	VkComputePipelineCreateInfo computePipelineCreateInfo{};
	computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	computePipelineCreateInfo.pNext = nullptr;
	computePipelineCreateInfo.layout = _gradientPipelineLayout;
	computePipelineCreateInfo.stage = stageinfo;
	
	VK_CHECK(vkCreateComputePipelines(_device,VK_NULL_HANDLE,1,&computePipelineCreateInfo, nullptr, &_gradientPipeline));
//< comp_pipeline_2
/
//> comp_pipeline_3
	vkDestroyShaderModule(_device, computeDrawShader, nullptr);

	_mainDeletionQueue.push_function([&]() {
		vkDestroyPipelineLayout(_device, _gradientPipelineLayout, nullptr);
		vkDestroyPipeline(_device, _gradientPipeline, nullptr);
		});
//< comp_pipeline_3

#elif CHAPTER_STAGE == 2
VkShaderModule computeDrawShader;
if (!vkutil::load_shader_module("../../shaders/gradient_color.comp.spv", _device, &computeDrawShader)) {
	std::cout << "Error when building the compute shader" << std::endl;
}

VkPipelineShaderStageCreateInfo stageinfo{};
stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
stageinfo.pNext = nullptr;
stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
stageinfo.module = computeDrawShader;
stageinfo.pName = "main";

VkComputePipelineCreateInfo computePipelineCreateInfo{};
computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
computePipelineCreateInfo.pNext = nullptr;
computePipelineCreateInfo.layout = _gradientPipelineLayout;
computePipelineCreateInfo.stage = stageinfo;

VK_CHECK(vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &_gradientPipeline));


vkDestroyShaderModule(_device, computeDrawShader, nullptr);
_mainDeletionQueue.push_function([&]() {
	vkDestroyPipelineLayout(_device, _gradientPipelineLayout, nullptr);
	vkDestroyPipeline(_device, _gradientPipeline, nullptr);
	});
#else
//> comp_pipeline_multi
VkShaderModule gradientShader;
if (!vkutil::load_shader_module("../../shaders/gradient_color.comp.spv", _device, &gradientShader)) {
	std::cout << "Error when building the compute shader" << std::endl;
}

VkShaderModule skyShader;
if (!vkutil::load_shader_module("../../shaders/sky.comp.spv", _device, &skyShader)) {
	std::cout << "Error when building the compute shader" << std::endl;
}

VkPipelineShaderStageCreateInfo stageinfo{};
stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
stageinfo.pNext = nullptr;
stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
stageinfo.module = gradientShader;
stageinfo.pName = "main";

VkComputePipelineCreateInfo computePipelineCreateInfo{};
computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
computePipelineCreateInfo.pNext = nullptr;
computePipelineCreateInfo.layout = _gradientPipelineLayout;
computePipelineCreateInfo.stage = stageinfo;

ComputeEffect gradient;
gradient.layout = _gradientPipelineLayout;
gradient.name = "gradient";
gradient.data = {};

//default colors
gradient.data.data1 = glm::vec4(1, 0, 0, 1);
gradient.data.data2 = glm::vec4(0, 0, 1, 1);

VK_CHECK(vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &gradient.pipeline));

//change the shader module only to create the sky shader
computePipelineCreateInfo.stage.module = skyShader;

ComputeEffect sky;
sky.layout = _gradientPipelineLayout;
sky.name = "sky";
sky.data = {};
//default sky parameters
sky.data.data1 = glm::vec4(0.1, 0.2, 0.4 ,0.97);

VK_CHECK(vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &sky.pipeline));

//add the 2 background effects into the array
backgroundEffects.push_back(gradient);
backgroundEffects.push_back(sky);

//destroy structures properly
vkDestroyShaderModule(_device, gradientShader, nullptr);
vkDestroyShaderModule(_device, skyShader, nullptr);
_mainDeletionQueue.push_function([&]() {
	vkDestroyPipelineLayout(_device, _gradientPipelineLayout, nullptr);
	vkDestroyPipeline(_device, sky.pipeline, nullptr);
	vkDestroyPipeline(_device, gradient.pipeline, nullptr);
});

//< comp_pipeline_multi
#endif



}

void VulkanEngine::init_descriptors()
{
//> init_desc_1
	//create a descriptor pool that will hold 10 sets with 1 image each
	std::vector<DescriptorAllocator::PoolSizeRatio> sizes =
	{
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }
	};

	globalDescriptorAllocator.init_pool(_device, 10, sizes);

	//make the descriptor set layout for our compute draw
	{
		DescriptorLayoutBuilder builder;
		builder.add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		_drawImageDescriptorLayout = builder.build(_device, VK_SHADER_STAGE_COMPUTE_BIT);
	}
//< init_desc_1
// 
//> init_desc_2	
	//allocate a descriptor set for our draw image
	_drawImageDescriptors = globalDescriptorAllocator.allocate(_device,_drawImageDescriptorLayout);	

	VkDescriptorImageInfo imgInfo{};
	imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	imgInfo.imageView = _drawImage.imageView;
	
	VkWriteDescriptorSet drawImageWrite = {};
	drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	drawImageWrite.pNext = nullptr;
	
	drawImageWrite.dstBinding = 0;
	drawImageWrite.dstSet = _drawImageDescriptors;
	drawImageWrite.descriptorCount = 1;
	drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	drawImageWrite.pImageInfo = &imgInfo;

	vkUpdateDescriptorSets(_device, 1, &drawImageWrite, 0, nullptr);
// init_desc_2

```

We begin by creating a VkExtent3d structure with the size of the image we want, which will match our window size.

Then, we need to fill our usage flags. In vulkan, all images and buffers must fill a UsageFlags with what they will be used for. This allows the driver to perform optimizations in the background depending on what that buffer or image is going to do later. In our case, we want TransferSRC because we will have it as a copy source to the swapchain, Storage because thats the "compute shader can write to it" layout, and Color Attachment so that we can use graphics pipelines to draw geometry into it.

The format is going to be VK_FORMAT_R16G16B16A16_SFLOAT. This is 16 bit floats for all 4 channels, and will use 64 bits per pixel. Thats a fair amount of data, 2x what a 8 bit color image uses, but its going to be useful.

When creating the image itself, we need to send the image info and an alloc info to VMA. VMA will do the vulkan create calls for us and directly give us the vulkan image. 
The interesting thing in here is Usage and the required memory flags.
With VMA_MEMORY_USAGE_GPU_ONLY usage, we are letting VMA know that this is a gpu texture that wont ever be accessed from CPU, which lets it put it into gpu VRAM. To make extra sure of that, we are also setting VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT as a memory flag. This is a flag that only gpu-side VRAM has, and guarantees the fastest access.

In vulkan, there are multiple memory regions we can allocate images and buffers from. PC implementations with dedicated GPUs will generally have a cpu ram region, a GPU Vram region, and a "upload heap" which is a special region of gpu vram that allows cpu writes. If you have resizable bar enabled, the upload heap can be the entire gpu vram. Else it will be much smaller, generally only 256 megabytes. We tell VMA to put it on GPU_ONLY which will prioritize it to be on the gpu vram but outside of that upload heap region.

With the image allocated, we create an imageview to pair with it. In vulkan, you need a imageview to access images. This is generally a thin wrapper over the image itself that lets you do things like limit access to only 1 mipmap. We will always be pairing vkimages with their "default" imageview in this tutorial.

# new draw loop

Now that we have a new draw image, lets add it into the render loop.

We will need a way to copy images, so add this into vk_images.cpp

<!-- codegen from tag copyimg on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_images.cpp --> 
```cpp
void vkutil::copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent3D imageSize)
{
    VkImageBlit2 blitRegion{};
	blitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
	blitRegion.pNext = nullptr;

    blitRegion.srcOffsets[1].x = imageSize.width;
    blitRegion.srcOffsets[1].y = imageSize.height;
    blitRegion.srcOffsets[1].z = 1;

    blitRegion.dstOffsets[1].x = imageSize.width;
    blitRegion.dstOffsets[1].y = imageSize.height;
	blitRegion.dstOffsets[1].z = 1;

	blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.srcSubresource.baseArrayLayer = 0;
	blitRegion.srcSubresource.layerCount = 1;
	blitRegion.srcSubresource.mipLevel = 0;

	blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.dstSubresource.baseArrayLayer = 0;
	blitRegion.dstSubresource.layerCount = 1;
	blitRegion.dstSubresource.mipLevel = 0;

    VkBlitImageInfo2 blitInfo{};
	blitInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
	blitInfo.pNext = nullptr;
	blitInfo.dstImage = destination;
	blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	blitInfo.srcImage = source;
	blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    blitInfo.filter = VK_FILTER_NEAREST;
    blitInfo.regionCount = 1;
    blitInfo.pRegions = &blitRegion;

    vkCmdBlitImage2(cmd, &blitInfo);
}
```
 
Vulkan has 2 main ways of copying one image to another. you can use VkCmdCopyImage or VkCmdBlitImage
CopyImage is faster, but its much more restricted, for example the resolution on both images must match.
Meanwhile, blit image lets you copy images of different formats and different sizes into one another. You have a source rectangle and a target rectangle, and the system copies it into its position.

With it, we can now update the render loop. As draw() is getting too big, we are going to leave the syncronization, command buffer management, and transitions in the draw() function, but we are going to add the draw commands themselves into a draw_main() function.

```cpp
void VulkanEngine::draw_main(VkCommandBuffer cmd)
{
	//make a clear-color from frame number. This will flash with a 120 frame period.
	VkClearColorValue clearValue;
	float flash = abs(sin(_frameNumber / 120.f));
	clearValue = { { 0.0f, 0.0f, flash, 1.0f } };

	VkImageSubresourceRange clearRange = vkinit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);

	//clear image
	vkCmdClearColorImage(cmd, _drawImage.image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
}
```
We will be changing the code that records the command buffer. You can now delete the older one. 
The new code is this.
```cpp
	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));	

	// transition our main draw image into general layout so we can write into it
	// we will overwrite it all so we dont care about what was the older layout
	vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	draw_main(cmd);

	//transtion the draw image and the swapchain image into their correct transfer layouts
	vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VkExtent3D extent;
	extent.height = _windowExtent.height;
	extent.width = _windowExtent.width;
	extent.depth = 1;

	// execute a copy from the draw image into the swapchain
	vkutil::copy_image_to_image(cmd, _drawImage.image, _swapchainImages[swapchainImageIndex], extent);

	// set swapchain image layout to Present so we can show it on the screen
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	//finalize the command buffer (we can no longer add commands, but it can now be executed)
	VK_CHECK(vkEndCommandBuffer(cmd));
```

The main diference we have in the render loop is that we no longer do the clear on the swapchain image. Instead, we do it on the `_drawImage.image`. Once we have cleared the image, we transition both the swapchain and the draw image into their layouts for transfer, and we execute the copy command. Once we are done with the copy command, we transition the swapchain image into present layout for display. As we are always drawing on the same image, our draw_image does not need to access swapchain index, it just clears the draw image.

This will now provide us a way to render images outside of the swapchain itself. We now get significantly higher pixel precision, and we unlock some other techniques.

With that done, we can now move into the actual compute shader execution steps.



