---
layout: default
lang: ko
title: Setting up IMGUI
parent:  "2. 컴퓨트 셰이더 그리기"
nav_order: 7
---

As the engine grows, we need a way to have some interactuable interface we can play with to add sliders, buttons, and display data. 
엔진이 커지면서 슬라이더, 버튼, 그리고 데이터를 표시할 수 있는 상호작용할 수 있는 인터페이스가 필요합니다.

To do that, we are going to add the library "dear Imgui" to the project. This is a library that makes it very easy to add windows and interfaces with sliders, buttons, editable text.. Without having to setup UI files or deal with a complex system. 
이를 위해 프로젝트에 "dear Imgui" 라이브러리를 추가할 것입니다. 이는 슬라이더나 버튼, 기타 편집가능한 텍스트와 같은 상호작용 요소와 창을 ui 설정이나 복잡한 시스템을 다루지 않고 매우 쉽게 추가할 수 있는 라이브러리입니다.


## Immediate GPU commands / 즉시 GPU 명령
EDIT UNTIL FIXED: This section of the article will be moved away, new version of imgui does not need immediate commands to upload. We still need the immediate commands for later in the tutorial.
수정 될 때 까지 편집 : 이 글의 해당 섹션은 새로운 버전의 imgui가 immediate 명령을 업로드할 필요가 없을 때 옮겨질 것입니다. 아직은 튜토리얼에서 immediate 명령이 필요합니다.

Imgui will require us to run some commands outside of the normal draw loop. This is going to be something we will need many times on the engine for different uses. We are going to implement an `immediate_submit` function, which uses a fence and a different command buffer from the one we use on draws to send some commands to the GPU without syncronizing with swapchain or with rendering logic.
Imgui는 일반적인 그리기 루프 외부에 몇 가지 명령을 실행하는 것을 요구합니다. 이는 엔진에서 다양하게 여러번 사용할 것입니다. `immediate_submit`함수를 구현할 것이며, 이는 GPU에게 스왑체인 혹은 렌더링 로직을 동기화하지 않고 명령을 보낼 때 사용하는 다양한 버퍼와 펜스를 사용합니다.

Lets add those structures into the VulkanEngine class
VulkanEngine 클래스에 이러한 구조체를 추가합시다.

```cpp
class VulkanEngine{
public:
    // immediate submit structures / immediate submit 구조체
    VkFence _immFence;
    VkCommandBuffer _immCommandBuffer;
    VkCommandPool _immCommandPool;

	
	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

private:
	void init_imgui();
}
```
We have a fence and a command buffer with its pool. The immediate_submit function takes an std function as callback to use with lambdas. 
Add the init_imgui() function too, and add it at the end of the init() chain of function calls. Leave it empty for now.
펜스와 커맨드 버퍼가 있습니다. immediate_submit 함수는 std::function을 람다를 사용하는 콜백으로서 받습니다.

we need to create those syncronization structures for immediate submit, so lets go into init_commands() function and hook the command part.
immediate submit을 위해 이러한 동기화 구조체를 생성할 필요가 있는데, 따라서 init_commands() 함수에 명령 부분을 연결합시다. 

```cpp
void VulkanEngine::init_commands()
{
	VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_immCommandPool));

	// allocate the command buffer for immediate submits / immediate submit을 위해 커맨드 버퍼를 할당합니다.
	VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_immCommandPool, 1);

	VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_immCommandBuffer));

	_mainDeletionQueue.push_function([=]() { 
	vkDestroyCommandPool(_device, _immCommandPool, nullptr);
	});

}
```

This is the same we were doing with the per-frame commands, but this time we are directly putting it into the deletion queue for cleanup.
이는 프레임별 커맨드를 작성할 때와 같지만, 이번에는  이를 정리를 위해 삭제큐에 직접 넣어주고 있습니다.

Now we need to create the fence, which we are going to add to init_sync_structures(). Add it to the end
이제 펜스를 생성해야 합니다. 이는 init_sync_structures()에 추가하겠습니다.

```cpp
void VulkanEngine::init_sync_structures()
{
	VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_immFence));
	_mainDeletionQueue.push_function([=]() { vkDestroyFence(_device, _immFence, nullptr); });
}
```

We will use the same fenceCreateInfo we were using for the per-frame fences. Same as with the commands, we are directly adding its destroy function to the deletion queue too.
같은 프레임별 펜스에 사용했던 fenceCreateInfo를 사용할 것입니다. 커맨드 버퍼와 같이 이를 삭제 큐에 직접 넣어주겠습니다.

Now implement the immediate_submit function
이제 immediate_submit 함수를 구현하겠습니다.

<!-- codegen from tag imm_submit on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
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

	// submit command buffer to the queue and execute it. / 커맨드 버퍼를 큐에 제출하고 실행합니다.
	//  _renderFence will now block until the graphic commands finish execution / _renderFence가 그래픽 명령이 완료될 때 까지 블록합니다.
	VK_CHECK(vkQueueSubmit2(_graphicsQueue, 1, &submit, _immFence));

	VK_CHECK(vkWaitForFences(_device, 1, &_immFence, true, 9999999999));
}
```

Note how this function is very similar and almost the same as the way we are executing commands on the gpu. 
이 함수가 GPU에서 명령을 실행하는 방법과 매우 유사한 것을 유의해야 합니다.

Its close to the same thing, except we are not syncronizing the submit with the swapchain. 
거의 유사하지만 교환사슬에 제출할 때 동기화는 수행하지 않고 있습니다.

We will be using this function for data uploads and other "instant" operations outside of the render loop. One way to improve it would be to run it on a different queue than the graphics queue, and that way we could overlap the execution from this with the main render loop. 
데이터 업로드와 렌더링 루프 외부에서 다른 "즉각적인" 명령을 위해 함수를 사용할 것입니다. 이를 개선하는 방법 중 하나는 그래픽스 큐와는 다른 큐에서 실행하여 메인 렌더링 루프 실행을 겹쳐 사용하는 것입니다.


## IMGUI 설정
Lets now go with the imgui initialization.
이제 imgui를 초기화해보겠습니다.

We need to add some includes first to vk_engine.cpp
몇가지 헤더를 vk_engine.cpp에 포함하겠습니다.

```cpp
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"
```

Its the main imgui header, and then the implementation headers for the SDL 2 and the vulkan backends.
이는 핵심 imgui 헤더로, SDL 2와 Vulkan 백엔드 구현 헤더입니다.

Now to the initialization function
이제 초기화 함수를 작성해봅시다.

<!-- codegen from tag imgui_init on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
void VulkanEngine::init_imgui()
{
	// 1: create descriptor pool for IMGUI / IMGUI를 위한 디스크립터 풀을 생성
	//  the size of the pool is very oversize, but it's copied from imgui demo 디스크립터 풀의 크기는 매우 넉넉히 잡았지만, imgui demo에서부터 복사됩니다.
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

	// 2: initialize imgui library / 2: imgui 라이브러리 초기화

	// this initializes the core structures of imgui / 이는 imgui의 핵심 구조체를 초기화합니다.
	ImGui::CreateContext();

	// this initializes imgui for SDL / SDL을 위한 IMGUI 초기화입니다.
	ImGui_ImplSDL2_InitForVulkan(_window);

	// this initializes imgui for Vulkan / Vulkan을 위한 IMGUI 초기화입니다.
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = _instance;
	init_info.PhysicalDevice = _chosenGPU;
	init_info.Device = _device;
	init_info.Queue = _graphicsQueue;
	init_info.DescriptorPool = imguiPool;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	init_info.UseDynamicRendering = true;

	//dynamic rendering parameters for imgui to use / imgui가 사용할 동적 렌더링 파라미터입니다.
	init_info.PipelineRenderingCreateInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
	init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
	init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &_swapchainImageFormat;
	

	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	ImGui_ImplVulkan_Init(&init_info);

	ImGui_ImplVulkan_CreateFontsTexture();

	// add the destroy the imgui created structures / imgui가 생성한 구조체를 삭제큐에 추가해 파괴합니다.
	_mainDeletionQueue.push_function([=]() {
		ImGui_ImplVulkan_Shutdown();
		vkDestroyDescriptorPool(_device, imguiPool, nullptr);
	});
}
```

Call this function at the end of `VulkanEngine::init()`, after `init_pipelines();`       
This code is adapted from the imgui demos. We first need to create some structures that imgui wants, like its own descriptor pool. The descriptor pool here is storing data for 1000 of a lot of different types of descriptors, so its a bit overkill. It wont be a problem, just slightly less efficient space-wise.
이 함수를 `init_pipelines();` 이후에 `VulkanEngine::init()`의 끝에서 호출합니다. 이 코드는 imgui 데모로부터 채택됩니다. 먼저 imgui가 원하는 구조체, 예를 들어 imgui만의 디스크립터 풀과 같은 구조체를 추가해야 합니다.
여기서의 디스크립터 풀은 1000개의 다양한 디스크립터를 담는데, 이는 약간 과합니다. 하지만 이는 문제가 되지 않는데, 공간적으로 약간 덜 효율적일 뿐입니다.

We then call `CreateContext()` , `ImGui_ImplSDL2_InitForVulkan`, and `ImGui_ImplVulkan_Init`. These functions will initialize the different parts of imgui we need. 
On the vulkan one, we need to hook a few things, like our device, instance, queue.
그리고 `CreateContext()`, `Imgui_ImplSDL2_InitForVulkan`과 `ImGUI_ImplVulkan_Init`을 호출합니다. 이 함수들은 우리가 필요한 imgui의 다양한 부분을 초기화할 것입니다. Vulkan에서는 디바이스, 인스턴스, 큐와 같은 것들을 연결할 것입니다.

One important one is that we need to set UseDynamicRendering to true, and set ColorAttachmentFormat to our swapchain format, this is because we wont be using vulkan render-passes but Dynamic Rendering instead. And unlike with the compute shader, we are going to draw dear imgui directly into the swapchain. 


After calling `ImGui_ImplVulkan_Init`, we need to do an immediate submit to upload the font texture. Once that is executed, we call DestroyFontUploadObjects so that imgui deletes those temporal structures. 
Last, we add cleanup code into the destruction queue.

# Imgui render loop
Imgui is initialized now, but we need to hook it into the rendering loop.

First thing we have to do is to add its code into the run() function

```cpp
//Handle events on queue
while (SDL_PollEvent(&e) != 0) {
    //close the window when user alt-f4s or clicks the X button			
    if (e.type == SDL_QUIT) bQuit = true;

    if (e.type == SDL_WINDOWEVENT) {

        if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
            stop_rendering = true;
        }
        if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
            stop_rendering = false;
        }
    }

    //send SDL event to imgui for handling
    ImGui_ImplSDL2_ProcessEvent(&e);
}

//do not draw if we are minimized
if (stop_rendering) {
    //throttle the speed to avoid the endless spinning
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    continue;
}		

// imgui new frame
ImGui_ImplVulkan_NewFrame();
ImGui_ImplSDL2_NewFrame();
ImGui::NewFrame();

//some imgui UI to test
ImGui::ShowDemoWindow();

//make imgui calculate internal draw structures
ImGui::Render();

//our draw function
draw();
```

We have to pass the SDL events into imgui for handling from our pollEvent loop.
After that, we need to call the 3 functions for a new frame on imgui. 
Once that is done, we can now do our UI commands. We are going to leave it on the demo window for now. 
When we call `ImGui::Render()`, that calculates the vertices/draws/etc that imgui requires to draw the frame, but it does not do any drawing on its own. To draw it we will continue it from within our draw() function.

# Dynamic Rendering
Imgui will draw using actual gpu draws with meshes and shaders, it will not do a compute draw like we are doing at the moment.
To draw geometry, it needs to be done withing a renderpass. But we are not using renderpasses as we will use dynamic rendering, a vulkan 1.3 feature. Instead of calling VkCmdBeginRenderpass, and giving it a VkRenderPass object, we call VkBeginRendering, with a VkRenderingInfo that contains the settings needed for the images to draw into.

The VkRenderingInfo points into multiple VkRenderingAttachmentInfo for our target images to draw into, so lets begin writing that one into the initializers.

<!-- codegen from tag color_info on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_initializers.cpp --> 
```cpp
VkRenderingAttachmentInfo vkinit::attachment_info(
    VkImageView view, VkClearValue* clear ,VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/)
{
    VkRenderingAttachmentInfo colorAttachment {};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.pNext = nullptr;

    colorAttachment.imageView = view;
    colorAttachment.imageLayout = layout;
    colorAttachment.loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    if (clear) {
        colorAttachment.clearValue = *clear;
    }

    return colorAttachment;
}
```

For our attachment info, we will have clear value as an optional pointer, that way we can either do a clear or skip it and load the image.

We need to hook imageview and layout as usual with all these rendering commands. The important part is the loadOP and storeOP. This controls what happens to the render target in this attachment when its used within a renderpass (both dynamic one and class renderpass). For load options, we have LOAD, which will keep the data in that image. Clear which will set it to our clear value at the start, and dont-care where we plan to replace every pixel and thus the gpu can skip loading it from memory. 

For our store op, we are going to use store hardcoded, as we will want our draw commands to be saved.

With the attachment info done, we can make the VkRenderingInfo. Add a new function `draw_imgui()` to the VulkanEngine class, to draw a renderpass that renders imgui. 

<!-- codegen from tag imgui_draw_fn on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
void VulkanEngine::draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView)
{
	VkRenderingAttachmentInfo colorAttachment = vkinit::attachment_info(targetImageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	VkRenderingInfo renderInfo = vkinit::rendering_info(_swapchainExtent, &colorAttachment, nullptr);

	vkCmdBeginRendering(cmd, &renderInfo);

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

	vkCmdEndRendering(cmd);
}
```

We are going to take a render extent to setup a rectangle of pixels to draw, and we will send a color attachment and a depth attachment. We dont need the depth attachment right now, thats for later.

Then we need to call it from our draw() function.

<!-- codegen from tag imgui_draw on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
	// execute a copy from the draw image into the swapchain
	vkutil::copy_image_to_image(cmd, _drawImage.image, _swapchainImages[swapchainImageIndex], _drawExtent, _swapchainExtent);

	// set swapchain image layout to Attachment Optimal so we can draw it
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	//draw imgui into the swapchain image
	draw_imgui(cmd,  _swapchainImageViews[swapchainImageIndex]);

	// set swapchain image layout to Present so we can draw it
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	//finalize the command buffer (we can no longer add commands, but it can now be executed)
	VK_CHECK(vkEndCommandBuffer(cmd));
```

That copy_image command is the same as before, we are replacing the later commands until the end of the VkEndCommandBuffer call.

Before, we were transitioning the swapchain image from transfer layout into present layout, but now we are going to change it into `VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL` instead. This is the layout you should be using whenever you are calling rendering commands like in here.

After that, we build the VkRenderingInfo and send a single color attachment to it. That color attachment will point to the swapchain image we target.

Now that we have a VkRenderInfo, we can call `vkCmdBeginRendering`, this begins a render pass, and we can now execute draw commands. We call into the imgui vulkan backend with our command buffer, which will make imgui record its draw commands into the buffer. Once that is done, we can call `vkCmdEndRendering` to end the render pass. 

After that, we transition the swapchain image from attachment-optimal into present mode, and can finally end the command buffer.

If you run the application at this point, you will have the imgui demo window you can play around with.

Lets continue and hook our new debug UI to the shaders.

Next: [ Push Constants and new shaders]({{ site.baseurl }}{% link docs/new_chapter_2/vulkan_pushconstants.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}
 
