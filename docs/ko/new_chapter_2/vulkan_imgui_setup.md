---
layout: default
title: Setting up IMGUI
parent:  "2. Drawing with Compute"
nav_order: 7
---

엔진이 점점 커지면서 슬라이더, 버튼을 추가하거나 데이터를 표시할 수 있는 상호작용 가능한 인터페이스가 필요해졌습니다.

이를 위해 프로젝트에 **dear ImGui** 라이브러리를 추가할 것입니다. 이 라이브러리는 ui 설정이나 복잡한 시스템을 다루지 않고도 슬라이더나 버튼, 기타 편집가능한 텍스트와 같은 상호작용 요소와 창을 매우 쉽게 추가할 수 있는 라이브러리입니다.


## Immediate GPU 명령
보류 : 해당 섹션은 나중에 다른 위치로 이동할 예정입니다. 최신 버전의 ImGui에서는 업로드를 위해 immediate 명령이 필요하지 않지만, 아직은 추후 튜토리얼에서 immediate 명령이 필요합니다.

`ImGui`는 일반적인 그리기 루프 외부에서 몇 가지 명령을 실행해야 합니다. 이러한 기능은 엔진에서 다양한 용도로 여러 번 사용할 것입니다. 이를 위해 `immediate_submit`함수를 구현할 것이며, 이 함수는 그리기에 사용하는 커맨드 버퍼와는 다른 버퍼와 펜스를 사용하여 스왑체인이나 렌더링 로직과 동기화하지 않고 GPU에 명령을 전송합니다.

`VulkanEngine` 클래스에 이러한 구조체를 추가합시다.

```cpp
class VulkanEngine{
public:
    // immediate submit structures
    VkFence _immFence;
    VkCommandBuffer _immCommandBuffer;
    VkCommandPool _immCommandPool;

	
	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

private:
	void init_imgui();
}
```
우리는 펜스와 커맨드 버퍼를 가지고 있습니다. `immediate_submit` 함수는 람다와 함께 사용할 수 있도록 `std::function`을 콜백으로 받습니다. init_imgui() 함수도 추가하고, init() 함수 호출 마지막에 호출되도록 합니다. 지금은 비워둡니다.

immediate submit을 위해 이러한 동기화 구조체를 생성해야 하므로, `init_commands()` 함수에 명령 부분을 연결합시다. 

```cpp
void VulkanEngine::init_commands()
{
	VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_immCommandPool));

	// allocate the command buffer for immediate submits
	VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_immCommandPool, 1);

	VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_immCommandBuffer));

	_mainDeletionQueue.push_function([=]() { 
	vkDestroyCommandPool(_device, _immCommandPool, nullptr);
	});

}
```

이는 프레임별 명령을 작성할 때와 같지만, 이번에는 정리를 위해 삭제큐에 직접 추가하고 있습니다.

이제 펜스를 생성해야 합니다. 이는 `init_sync_structures()`에 추가하겠습니다.

```cpp
void VulkanEngine::init_sync_structures()
{
	VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_immFence));
	_mainDeletionQueue.push_function([=]() { vkDestroyFence(_device, _immFence, nullptr); });
}
```

프레임별 펜스에 사용했던 것과 같은 `fenceCreateInfo`를 사용할 것입니다. 커맨드 버퍼와 마찬가지로 이를 삭제 큐에 직접 넣어주겠습니다.

이제 `immediate_submit` 함수를 구현하겠습니다.

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

	// submit command buffer to the queue and execute it.
	//  _renderFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit2(_graphicsQueue, 1, &submit, _immFence));

	VK_CHECK(vkWaitForFences(_device, 1, &_immFence, true, 9999999999));
}
```

이 함수가 GPU에서 명령을 실행하는 방법과 거의 동일하다는 점에 주목하세요.

거의 동일하지만 제출할 때 스왑체인과 동기화는 수행하지 않고 있습니다.

데이터 업로드와 렌더링 루프 외부에서 기타 "즉각적인" 작업에 이 함수를 사용할 것입니다. 이를 개선하는 방법 중 하나는 그래픽스 큐와는 다른 큐에서 실행하여 함수의 실행을 메인 렌더링 루프와 겹쳐 사용하는 것입니다.


## ImGui 설정
이제 ImGui를 초기화해보겠습니다.

먼저 몇가지 헤더를 `vk_engine.cpp`에 포함하겠습니다.

```cpp
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"
```

이는 주요 imgui 헤더와, SDL 2 및 Vulkan 백엔드 구현 헤더입니다.

이제 초기화 함수를 작성해봅시다.

<!-- codegen from tag imgui_init on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
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

	//dynamic rendering parameters for imgui to use
	init_info.PipelineRenderingCreateInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
	init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
	init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &_swapchainImageFormat;
	

	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	ImGui_ImplVulkan_Init(&init_info);

	ImGui_ImplVulkan_CreateFontsTexture();

	// add the destroy the imgui created structures
	_mainDeletionQueue.push_function([=]() {
		ImGui_ImplVulkan_Shutdown();
		vkDestroyDescriptorPool(_device, imguiPool, nullptr);
	});
}
```

이 함수를 `init_pipelines();` 이후에 `VulkanEngine::init()`의 마지막에 호출합니다. 이 코드는 ImGui 데모에서 가져온 예제를 기반으로 작성되었습니다. 먼저 ImGui에서 요구하는 구조체, 예를 들어 ImGui 전용 디스크립터 풀 등을 생성해야 합니다.
여기서의 디스크립터 풀은 1000개의 다양한 타입의 디스크립터를 담는데, 이는 약간 과하지만, 공간 활용 면에서 약간 비효율적일 뿐 문제는 발생하지 않습니다.

그리고 `CreateContext()`, `Imgui_ImplSDL2_InitForVulkan`과 `ImGUI_ImplVulkan_Init`을 호출합니다. 이 함수들은 우리가 필요한 ImGui의 여러 구성요소를 초기화할 것입니다. Vulkan에서는 디바이스, 인스턴스, 큐와 같은 것들을 연결해야 합니다.

한 가지 중요한 점은 `UseDynamicRendering`을 `true`로 설정하고, `ColorAttachmentFormat`을 스왑체인 포맷으로 설정해야 한다는 것입니다. 왜냐하면 Vulkan 렌더 패스를 사용하지 않고 동적렌더링을 사용할 것이기 때문입니다. 그리고 컴퓨트 셰이더와는 달리 dear imgui는 스왑체인에 직접 렌더링될 것입니다.


`ImGui_ImplVulkan_Init`을 호출한 후, 폰트 텍스쳐를 업로드 하기 위해 immediate submit을 해야합니다. 이 작업이 실행 되면 DestroyFontUploadObjects를 호출하여 ImGui가 임시 구조체를 정리하도록 합니다. 마지막으로 정리하는 코드를 삭제 큐에 추가합니다.

# Imgui 렌더링 루프
Imgui가 초기화되었지만 이를 렌더링 루프에 연결해주어야 합니다.

먼저 ImGui 코드를 `run()`함수에 추가합니다.

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

`pollEvent` 루프에서 발생한 SDL 이벤트를 ImGui로 전달하여 처리하게 해야 합니다. 그 다음, ImGui의 새로운 프레임을 시작하기 위해 세 가지 함수를 호출해야 합니다. 이 작업이 수행되면 UI 명령을 실행할 수 있습니다. 지금은 데모 창만을 표시해보겠습니다. `ImGui::Render()`를 호출할 때, ImGui가 프레임을 그리기에 필요한 정점/드로우콜 등을 계산하지만, 실제로 화면에 그리지는 않습니다. 실제 렌더링은 `draw()`함수 내부에서 계속하겠습니다.

# 동적 렌더링
ImGui는 메시와 셰이더를 포함하는 실제 GPU 드로우콜을 사용해 그립니다. 현재 우리가 진행중인 컴퓨트 셰이더 방식의 드로우는 아닙니다. 도형을 렌더링하기 위해 렌더 패스를 설정해야 하지만, Vulkan 1.3기능인 동적 렌더링을 사용할 것이기 때문에 렌더 패스를 사용하지 않을 것입니다. `VkRenderPass` 객체를 전달해 `VkCmdBeginRenderPass`를 호출하는 것 대신 이미지를 그릴 때 필요한 정보를 담는 `VkRenderingInfo`와 함께 `VkBeginRendering`을 호출할 것입니다.

`VkRenderingInfo`는 우리의 그릴 이미지인 여러 `VkRenderingAttachmentInfo`를 참조합니다. 이를 `intializers`에 작성해봅시다.

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

attachment info를 위해 선택적인 포인터로 clear value를 설정해야 합니다. 이를 통해 초기화하거나, 혹은 생략하고 기존 이미지를 불러올 수 있습니다.

이미지 뷰와 레이아웃은 평소처럼 모든 렌더링 명령과 연결해주어야 합니다. 여기서 중요한 부분은 `loadOP`와 `storeOP`입니다. 이는 해당 어태치먼트가 렌더패스(동적 렌더링과 일반 렌더패스 모두)에서 사용될 때 렌더 타겟에 어떤 일이 일어나는지를 제어합니다. load 선택지에서는 LOAD를 설정했습니다. 이는 기존 이미지 데이터를 유지하는 것을 의미합니다. Clear는 시작 시 설정된 clear value로 초기화하며, 모든 픽셀을 교체할 때에는 dont-care로 설정해 GPU가 메모리에서 값을 읽지 않게 하여 성능을 최적화합니다.

우리의 store op는 그리기 명령을 저장하기 위해 store로 하드코딩할 것입니다. 

attachment info가 작성되었으므로 `VkRenderingInfo`를 작성할 수 있습니다. ImGui를 렌더링하는 렌더패스를 실행하기 위해 새로운 함수 `draw_imgui()`를 `VulkanEngine` 클래스에 추가합니다.

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

그릴 사각형의 영역을 설정하기 위해 render extent를 받아야 하며, 색상 어태치먼트와 깊이 어태치먼트를 전달해야 합니다. 깊이 어태치먼트는 당장은 필요 없지만 이후에 다룰 것입니다.

이후에는 `draw()` 함수에서 호출하기만 하면 됩니다.

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

`copy_image` 명령은 이전과 같습니다. 이후의 명령들을 `VkEndCommandBuffer` 호출까지의 구간으로 대체합니다.

이전에는 스왑체인의 이미지를 전송 레이아웃에서 표시 레이아웃으로 전환했지만, `VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL`로 대신 바꿀 것입니다. 이는 여기처럼 렌더링 명령을 호출할 때 사용하는 레이아웃입니다.

그 다음, `VkRenderingInfo`를 구성하고 단일 색상 어태치먼트를 전달합니다. 이 색상 어태치먼트는 우리가 타게팅한 스왑체인 이미지를 가리킵니다.

이제 `VkRenderInfo`를 준비했으므로 `vkCmdBeginRendering`을 호출할 수 있습니다. 이는 렌더 패스를 시작하며 이제 그리기 명령을 실행할 수 있습니다. ImGui Vulkan 백엔드에서 우리의 커맨드 버퍼를 전달하면, ImGui가 자신의 그리기 명령을 해당 버퍼에 기록합니다. 이것이 완료되면 `vkCmdEndRendering`을 호출해 렌더 패스를 끝낼 수 있습니다.

이후, 스왑체인 이미지를 어태치먼트 최적에서 표시 모드로 전환하고 커맨드 버퍼를끝냅니다.

이 시점에서 애플리케이션을 실행해본다면 ImGui 데모창을 확인할 수 있습니다.

이제 디버깅 UI를 셰이더와 연결해보겠습니다.

Next: [ Push Constants and new shaders]({{ site.baseurl }}{% link docs/ko/new_chapter_2/vulkan_pushconstants.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}
 
