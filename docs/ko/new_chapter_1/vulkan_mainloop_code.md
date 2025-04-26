---
layout: default
title: Mainloop Code
parent:  "1. Initializing Vulkan - KO"
grand_parent: "Korean VKGuide"
nav_order: 41
---

우선 필요한 동기화 구조체를 `FrameData` 구조체에 추가하겠습니다. 구조체에 새로운 멤버를 추가합니다.

```cpp
struct FrameData {
	VkSemaphore _swapchainSemaphore, _renderSemaphore;
	VkFence _renderFence;
};
```

2개의 세마포어와 메인 렌더링 펜스가 필요할 것입니다. 이를 생성하겠습니다.

`_swapchainSemaphore`는 렌더링 명령이 스왑체인 이미지 요청을 대기하도록 하는 데 사용됩니다. `_renderSemaphore`는 그리기가 끝났을 때 이미지를 OS에 표시하는 것을 제어하는 데 사용됩니다.`_renderFence`는 주어진 프레임의 그리기 명령이 끝날 때 까지 대기할 수 있도록 합니다.

이를 초기화해봅시다. `vk_initializers.cpp` 코드에 있는 `VkFenceCreateInfo`와 `VkSemaphoreCreateInfo`를 작성하는 함수를 확인해봅시다.

<!-- codegen from tag init_sync on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_initializers.cpp --> 
```cpp
VkFenceCreateInfo vkinit::fence_create_info(VkFenceCreateFlags flags /*= 0*/)
{
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = nullptr;

    info.flags = flags;

    return info;
}

VkSemaphoreCreateInfo vkinit::semaphore_create_info(VkSemaphoreCreateFlags flags /*= 0*/)
{
    VkSemaphoreCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
}
```

두 구조체 모두 꽤 단순하며, 특별한 옵션 없이 몇 가지 플래그만 설정하면 됩니다. 구조체에 대한 더 많은 정보를 보려면 다음 링크를 참고하세요. [VkFenceCreateInfo](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap7.html#VkFenceCreateInfo), [VkSemaphoreCreateInfo](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap7.html#VkSemaphoreCreateInfo)

이제 실제로 생성해봅시다.

<!-- codegen from tag init_sync on file E:\ProgrammingProjects\vulkan-guide-2\chapter-1/vk_engine.cpp --> 
```cpp
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
}
```

펜스에서 `VK_FENCE_CREATE_SIGNALED_BIT`를 사용하고 있습니다. 이는 매우 중요한 합니다. 펜스가 생성되자 마자 대기하게 하여 오류를 방지할 수 있습니다. 만약 이를 설정하지 않는다면, 첫 프레임에서 WaitFences를 호출 할 때 GPU가 작업을 수행하기도 전에 호출 되므로 쓰레드가 잠깁니다.

각 프레임마다 3개의 구조체를 생성했습니다. 이를 사용해 렌더링 루프를 작성하겠습니다.


# 렌더링 루프
펜스를 사용하여 GPU의 작업 완료를 기다리는 것으로 렌더링 루프를 시작합니다.

```cpp
void VulkanEngine::draw()
{
	// wait until the gpu has finished rendering the last frame. Timeout of 1
	// second
	VK_CHECK(vkWaitForFences(_device, 1, &get_current_frame()._renderFence, true, 1000000000));
	VK_CHECK(vkResetFences(_device, 1, &get_current_frame()._renderFence));
}
```

`vkWaitForFences()`를 사용하여 GPU가 작업을 끝낼 때 까지 대기한 후, 펜스를 리셋합니다. 펜스는 사용 후 반드시 리셋해야 합니다. 같은 펜스를 리셋하지 않고 여러 GPU 명령에서 사용할 수는 없습니다.

WaitFences 호출의 시간 제한은 1초로 설정했습니다. 이 함수는 나노초 단위를 사용합니다. 시간 제한을 0으로 설정한다면 GPU가 명령을 실행중인지 확인하는 용도로 사용할 수 있습니다.

다음으로는 스왑체인에게 이미지 인덱스를 요청하겠습니다.

<!-- codegen from tag draw_2 on file E:\ProgrammingProjects\vulkan-guide-2\chapter-1/vk_engine.cpp --> 
```cpp
	//request image from the swapchain
	uint32_t swapchainImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(_device, _swapchain, 1000000000, get_current_frame()._swapchainSemaphore, nullptr, &swapchainImageIndex));
```

`vkAcquireNextImageKHR`을 사용해 스왑체인에서 이미지 인덱스를 요청합니다. 사용 가능한 이미지가 없다면 설정한 시간 제한까지 쓰레드가 대기 상태가 됩니다. 여기서 제한 시간은 1초입니다.

`_swapchainSemaphore`를 어떻게 전달하는지 확인해보세요. 이는 스왑체인에서 렌더링할 이미지를 준비하는 작업과 다른 연산들을 동기화하기 위한 것입니다.

함수에서 반환된 인덱스를 사용해 그릴 스왑체인의 이미지를 결정합니다.

이제 렌더링 명령을 작성하겠습니다. 해당 프레임의 커맨드 버퍼를 리셋하고 시작(begin)하겠습니다. 여기에 사용할 또 다른 초기화 함수가 필요합니다.

<!-- codegen from tag init_cmd_draw on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_initializers.cpp --> 
```cpp
VkCommandBufferBeginInfo vkinit::command_buffer_begin_info(VkCommandBufferUsageFlags flags /*= 0*/)
{
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext = nullptr;

    info.pInheritanceInfo = nullptr;
    info.flags = flags;
    return info;
}
```

커맨드 버퍼가 시작되면, 몇 가지 속성을 담은 info 구조체를 전달해야 합니다. 여기서는 상속 정보를 사용하지 않으므로 `nullptr`로 설정하지만, `flags` 속성은 필요합니다.

 [VkCommandBufferBeginInfo](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap6.html#VkCommandBufferBeginInfo)에서 더 자세한 내용을 확인할 수 있습니다.

`VulkanEngine::draw()`로 돌아가, 커맨드 버퍼를 리셋하고 다시 시작하겠습니다.

<!-- codegen from tag draw_3 on file E:\ProgrammingProjects\vulkan-guide-2\chapter-1/vk_engine.cpp --> 
```cpp
	//naming it cmd for shorter writing
	VkCommandBuffer cmd = get_current_frame()._mainCommandBuffer;

	// now that we are sure that the commands finished executing, we can safely
	// reset the command buffer to begin recording again.
	VK_CHECK(vkResetCommandBuffer(cmd, 0));

	//begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//start the command buffer recording
	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));
```

`FrameData` 구조체에서 커맨드 버퍼를 `cmd`라는 변수로 복사하겠습니다. 이는 이후 코드에서 참조를 간결하게 하기 위함입니다. Vulkan 핸들은 64비트 핸들 혹은 포인터일 뿐이므로 복사해도 문제가 없습니다. 다만 실제 데이터는 Vulkan이 관리함을 기억하세요.

이제 `vkResetCommandBuffer`를 호출해 커맨드 버퍼를 리셋합니다. 이 호출은 커맨드 버퍼의 모든 명령을 완전히 제거하고 사용된 메모리를 해제합니다. 이제 `vkBeginCommandBuffer`를 사용해 커맨드 버퍼를 다시 시작할 수 있습니다. cmdBeginInfo에서 `VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT`플래그를 사용합니다. 이는 선택이지만 드라이버에게 해당 버퍼가 단 한 번만 제출되어 실행될 것임을 알 경우, 약간의 성능 향상을 기대할 수 있습니다. 프레임마다 커맨드 버퍼를 리셋해서 한번만 제출할 것이므로, 이 설정이 적절합니다. 

커맨드 버퍼 기록이 시작되었으므로, 명령을 추가하겠습니다. 먼저 스왑체인의 이미지를 그릴 수 있는 레이아웃으로 변환하고, `VkCmdClear`를 수행한 뒤, 최종적으로 다시 표시 최적화 레이아웃으로 변환하겠습니다.

즉, 렌더패스를 사용하지 않고도 커맨드 버퍼 내부에서 이미지를 변환할 수단이 필요하다는 뜻입니다. 이를 수행하는 함수를 `vk_images.h`에 추가하겠습니다.

```cpp
#pragma once 

#include <vulkan/vulkan.h>

namespace vkutil {

void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);
}
```

이미지를 변환하는 데에는 여러 방법이 있습니다. 기존의 레이아웃과 새로운 레이아웃만을 사용하여 이를 수행하는 가장 단순한 방법을 선택하겠습니다.

Vulkan 1.3에 포함된 Synchronization 2 기능 확장을 사용해 파이프라인 배리어를 설정하겠습니다. 파이프라인 배리어는 명령 간 읽기/쓰기 작업을 동기화하거나, 한 명령이 이미지에 그리는 동안 다른 명령이 해당 이미지를 읽도록 제어하는 등의 다양한 작업을 수행할 수 있습니다.

함수를 `vk_images.cpp`에 작성하겠습니다.

<!-- codegen from tag transition on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_images.cpp --> 
```cpp
#include <vk_initializers.h>

void vkutil::transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier2 imageBarrier {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
    imageBarrier.pNext = nullptr;

    imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

    imageBarrier.oldLayout = currentLayout;
    imageBarrier.newLayout = newLayout;

    VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    imageBarrier.subresourceRange = vkinit::image_subresource_range(aspectMask);
    imageBarrier.image = image;

    VkDependencyInfo depInfo {};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.pNext = nullptr;

    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers = &imageBarrier;

    vkCmdPipelineBarrier2(cmd, &depInfo);
}
```

`VkImageMemoryBarrier2`는 주어진 **이미지** 배리어의 정보를 담고 있습니다. 여기서는 변경 전 레이아웃과 변경 후 레이아웃을 설정합니다. `StageMask`는 `VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT`를 사용합니다. 이는 GPU 파이프라인이 약간 멈추게 하므로 비효율적일 수 있습니다. 하지만 우리는 프레임마다 몇 번만 변환을 수행하므로 문제가 되지 않습니다. 만약 후처리 과정에서 프레임마다 여러 번 변환을 수행한다면 이렇게 설정하는 것은 피하는 것이 좋습니다. 대신 수행하려는 작업에 더 적절한 `StageMask`를 사용하는 것이 바람직합니다.

배리어의 `VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT`설정은  GPU의 모든 명령이 배리어에서 완전히 멈춘다는 것을 의미합니다. 조금 더 세밀한 `StageMask`를 사용하면, GPU 파이프라인을 부분적으로 겹쳐서 실행할 수 있습니다. `AccessMask`도 유사하게 동작합니다. 이는 GPU의 특정 자원 접근을 제한하는 방식으로 동작합니다. 여기서는 `VK_ACCESS_2_MEMORY_WRITE_BIT`를 소스로 설정하고, `VK_ACCESS_2_MEMORY_READ_BIT`를 대상으로 추가하겠습니다. 이는 일반적으로 적절한 옵션입니다.

만약 파이프라인 배리어가 쓰이는 최적화 방법에 대해 더 알고 싶다면 [Khronos Vulkan Documentation: Syncronization examples](https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples)에서 찾아볼 수 있습니다. 이번 튜토리얼에서는 이 방식이 잘 작동하지만, 필요하다면 더 정밀하고 가벼운 변환 함수를 추가할 수 있습니다.

배리어 설정의 일부로 `VkImageSubresourceRange`도 사용해야 합니다. 이를 통해 특정 이미지의 일부만 배리어를 적용할 수 있습니다. 이는 배열 이미지나 밉맵 이미지에서 특정 레이어나 밉맵 단계에만 배리어를 적용할 때 유용합니다. 이번 예제에서는 기본값을 사용하여 모든 밉맵 단계와 모든 레이어를 변환하겠습니다.

<!-- codegen from tag subresource on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_initializers.cpp --> 
```cpp
VkImageSubresourceRange vkinit::image_subresource_range(VkImageAspectFlags aspectMask)
{
    VkImageSubresourceRange subImage {};
    subImage.aspectMask = aspectMask;
    subImage.baseMipLevel = 0;
    subImage.levelCount = VK_REMAINING_MIP_LEVELS;
    subImage.baseArrayLayer = 0;
    subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

    return subImage;
}
```

이 구조체에서 중요한 요소는 `AspectMask`입니다. 이는 색상 이미지의 경우 `VK_IMAGE_ASPECT_COLOR_BIT`, 깊이 이미지의 경우 `VK_IMAGE_ASPECT_DEPTH_BIT`로 설정됩니다. 깊이 버퍼를 추가할 때 사용할 `VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL`을 제외하고 모든 경우에 `VK_IMAGE_ASPECT_COLOR_BIT`를 사용하겠습니다.

범위(`VkImageSubresourceRange`)와 배리어(`VkImageMemoryBarrier2`)를 설정한 후, 이를 `VkDependencyInfo` 구조체에 담아 `VkCmdPipelineBarrier2`를 호출합니다. `VkImageMemoryBarrier2` 여러개를 `VkDependencyInfo`에 추가하면 여러 이미지의 레이아웃을 동시에 변환할 수 있습니다. 여러 이미지를 동시에 변환하거나 배리어를 적용할 경우 성능이 향상될 수 있습니다.

변환 함수가 구현되었으므로, 이제 렌더링을 수행할 수 있습니다. 작성한 함수를 사용하기 위해 `vk_images.h`를 `vk_engine.cpp` 상단에 포함하겠습니다.

<!-- codegen from tag draw_4 on file E:\ProgrammingProjects\vulkan-guide-2\chapter-1/vk_engine.cpp --> 
```cpp

	//make the swapchain image into writeable mode before rendering
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	//make a clear-color from frame number. This will flash with a 120 frame period.
	VkClearColorValue clearValue;
	float flash = std::abs(std::sin(_frameNumber / 120.f));
	clearValue = { { 0.0f, 0.0f, flash, 1.0f } };

	VkImageSubresourceRange clearRange = vkinit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);

	//clear image
	vkCmdClearColorImage(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

	//make the swapchain image into presentable mode
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex],VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	//finalize the command buffer (we can no longer add commands, but it can now be executed)
	VK_CHECK(vkEndCommandBuffer(cmd));
```

먼저 스왑체인의 이미지를 변환합니다. `VK_IMAGE_LAYOUT_UNDEFINED`는 초기화되지 않은 레이아웃입니다. 새로 생성된 이미지는 기본적으로 해당 레이아웃을 갖습니다. 이미지의 기존 데이터가 중요하지 않으며, GPU가 이를 덮어 써도 상관없을 때 사용합니다.

우리가 원하는 목표 레이아웃은 `VK_IMAGE_LAYOUT_GENERAL`입니다. 이는 범용 목적의 레이아웃으로, 이미지로부터 읽기쓰기를 할 수 있습니다. 렌더링에 최적화된 레이아웃은 아니지만, `vkCmdClearColorImage`를 사용할 때는 적합합니다. 컴퓨트 셰이더에서 이미지를 쓰기할 때 사용하는 이미지 레이아웃입니다. 만약 읽기 전용 이미지 혹은 래스터화 명령에 사용되는 이미지를 원한다면 더 나은 선택지가 있습니다.

이미지 레이아웃에 관해 세부사항을 확인해보고 싶다면 [Vulkan Spec: image layouts](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#resources-image-layouts)을 확인해보세요.


이제 `_frameNumber`를 사용한 기본적인 공식을 통해 초기화 색상을 계산합니다. sin함수를 사용해 파란색과 검은색 간 보간하며 주기적으로 변화합니다.

`vkCmdClearColorImage`는 세가지 주요 인자를 받습니다. 첫 번째는 이미지이며, 스왑체인에서 가져온 이미지를 사용합니다. 두 번째는 초기화 색상입니다. 마지막은 이미지의 어느 부분을 초기화할 지를 나타내는 `SubresourceRange`를 요구합니다. 여기서는 기본 `ImageSubresourceRange`를 사용합니다.

초기화 명령이 실행한 후, 이미지를 화면에 표시할 수 있도록 `VK_IMAGE_LAYOUT_PRESENT_SRC_KHR` 레이아웃으로 바꾸어야 합니다. 마지막으로는 `vkEndCommandBuffer`를 호출해 커맨드 버퍼를 종료합니다.

이제 GPU에 제출할 준비가 된 커맨드 버퍼가 완성되었습니다. 하지만 `VkQueueSubmit`을 호출하기 전에 동기화 구조를 설정하지 않으면 스왑체인과 올바르게 상호작용할 수 없습니다.

명령을 제출할 때는 `vkQueueSubmit2`를 사용합니다. 이는 synchronization-2의 일부로, Vulkan 1.0 버전에서의 `VkQueueSubmit`이 개선된 버전입니다. 이 함수는 제출에 필요한 세마포어의 정보를 담는 `VkSubmitInfo2`가 필요하며, 실행 완료를 확인하기 위해 펜스를 설정할 수 있습니다.
`VkSubmitInfo2`는 사용되는 각각의 세마포어에 대해 `VkSemaphoreSubmitInfo`가 필요하며, 제출될 커맨드 버퍼들에 대해서는 `VkCommandBufferSubmitInfo`가 필요합니다. 이러한 구조체들을 생성하는 `vkinit`함수들을 살펴보겠습니다.

<!-- codegen from tag init_submit on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_initializers.cpp --> 
```cpp
VkSemaphoreSubmitInfo vkinit::semaphore_submit_info(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore)
{
	VkSemaphoreSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.semaphore = semaphore;
	submitInfo.stageMask = stageMask;
	submitInfo.deviceIndex = 0;
	submitInfo.value = 1;

	return submitInfo;
}

VkCommandBufferSubmitInfo vkinit::command_buffer_submit_info(VkCommandBuffer cmd)
{
	VkCommandBufferSubmitInfo info{};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	info.pNext = nullptr;
	info.commandBuffer = cmd;
	info.deviceMask = 0;

	return info;
}

VkSubmitInfo2 vkinit::submit_info(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo,
    VkSemaphoreSubmitInfo* waitSemaphoreInfo)
{
    VkSubmitInfo2 info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    info.pNext = nullptr;

    info.waitSemaphoreInfoCount = waitSemaphoreInfo == nullptr ? 0 : 1;
    info.pWaitSemaphoreInfos = waitSemaphoreInfo;

    info.signalSemaphoreInfoCount = signalSemaphoreInfo == nullptr ? 0 : 1;
    info.pSignalSemaphoreInfos = signalSemaphoreInfo;

    info.commandBufferInfoCount = 1;
    info.pCommandBufferInfos = cmd;

    return info;
}
```

`command_buffer_submit_info`는 커맨드 버퍼만을 요구합니다. 다른 정보는 필요하지 않으며, `deviceMask`는 사용하지 않으므로 0으로 남겨둘 수 있습니다.

`semaphore_submit_info`는 `StageMask`를 요구합니다. 이는 `transition_image` 함수에서 보았던 것과 같습니다. 이외에는 세마포어 핸들만이 필요합니다. 디바이스 인덱스 인자는 여러 GPU간 세마포어를 사용할 때 필요하지만, 여기서는 사용하지 않을것입니다. 그리고 `value`는 타임라인 세마포어에 사용되며, 이는 이진 상태 대신 카운터 값을 증가시키며 동작하는 특수한 세마포어입니다. 여기서는 사용하지 않을 것이므로 기본적으로 1로 설정할 수 있습니다.

`submit_info`는 모든 것을 하나로 묶습니다. 이는 `VkCommandSubmitInfo`와 `signalSemaphoreInfo`,`waitSemaphoreInfo`를 필요로 합니다. 대기와 신호에 각각 1개의 세마포어씩을 사용할 것이지만, 복잡한 시스템에서는 여러 세마포어를 동시에 사용할 수도 있습니다.

구조체에 대한 더 많은 정보는 다음 링크에서 확인할 수 있습니다. [VkCommandBufferSubmitInfo](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap6.html#VkCommandBufferSubmitInfo), [VkSemaphoreSubmitInfo](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap6.html#VkSemaphoreSubmitInfo), [VkSubmitInfo2](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap6.html#VkSubmitInfo2)

이제 초기화할 준비가 되었으니 제출을 수행해봅시다.

<!-- codegen from tag draw_5 on file E:\ProgrammingProjects\vulkan-guide-2\chapter-1/vk_engine.cpp --> 
```cpp
	//prepare the submission to the queue. 
	//we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
	//we will signal the _renderSemaphore, to signal that rendering has finished

	VkCommandBufferSubmitInfo cmdinfo = vkinit::command_buffer_submit_info(cmd);	
	
	VkSemaphoreSubmitInfo waitInfo = vkinit::semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,get_current_frame()._swapchainSemaphore);
	VkSemaphoreSubmitInfo signalInfo = vkinit::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, get_current_frame()._renderSemaphore);	
	
	VkSubmitInfo2 submit = vkinit::submit_info(&cmdinfo,&signalInfo,&waitInfo);	

	//submit command buffer to the queue and execute it.
	// _renderFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit2(_graphicsQueue, 1, &submit, get_current_frame()._renderFence));
```

먼저 필요한 info 구조체를 만들어 `vkQueueSubmit2`를 호출합니다.`cmdInfo`에는 작성한 커맨드 버퍼를 전달할 것입니다. `waitInfo`에는 현재 프레임의 스왑체인 세마포어를 사용할 것입니다. `vkAcquireNextImageKHR`을 호출할 때,  같은 세마포어를 신호하도록 설정했으며, 이를 통해 스왑체인 이미지가 준비될 때 까지 해당 명령이 실행되지 않게 보장합니다. 

`signalInfo`에는 현재 프레임의 `_renderSemaphore`를 설정합니다. 이는 화면에 이미지를 표시하는 작업과 동기화합니다.

`fence`에는 현재 프레임의 `_renderFence`를 사용합니다. 렌더링 루프 시작지점에서 해당 펜스가 준비되기를 기다립니다. 이것이 CPU와 GPU를 동기화하는 방법입니다. CPU의 진행을 멈추어 GPU가 작업을 완료할 때 까지 현재 프레임의 다른 자원을 사용하지 않도록 합니다.

마지막으로 할 일은 그린 이미지를 화면에 표시하는 것입니다.

<!-- codegen from tag draw_6 on file E:\ProgrammingProjects\vulkan-guide-2\chapter-1/vk_engine.cpp --> 
```cpp
	//prepare present
	// this will put the image we just rendered to into the visible window.
	// we want to wait on the _renderSemaphore for that, 
	// as its necessary that drawing commands have finished before the image is displayed to the user
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.pSwapchains = &_swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &get_current_frame()._renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	VK_CHECK(vkQueuePresentKHR(_graphicsQueue, &presentInfo));

	//increase the number of frames drawn
	_frameNumber++;

```

`vkQueuePresent`는 info 구조체는 큐 제출 구조체와 매우 유사합니다. 이 구조체에는 세마포어의 포인터 뿐만 아니라 이미지 인덱스와 스왑체인의 인덱스를 갖습니다. `_renderSemaphore`를 대기하고 이를 스왑체인과 연결하겠습니다. 이렇게 하면 이전에 제출된 렌더링 명령이 완료될 때 까지 이미지를 화면에 표시하지 않을 것입니다.

함수의 끝에서 프레임 카운터를 증가시켜줍니다.

이로써 렌더링 루프를 작성했습니다. 남은 것은 동기화 자원을 올바르게 해제하는 것입니다.

```cpp
	for (int i = 0; i < FRAME_OVERLAP; i++) {
	
		//already written from before
		vkDestroyCommandPool(_device, _frames[i]._commandPool, nullptr);

		//destroy sync objects
		vkDestroyFence(_device, _frames[i]._renderFence, nullptr);
		vkDestroySemaphore(_device, _frames[i]._renderSemaphore, nullptr);
		vkDestroySemaphore(_device ,_frames[i]._swapchainSemaphore, nullptr);
	}
```

이제 엔진을 실행해 보세요. 정상적으로 작동한다면 파란 화면이 깜빡일 것입니다. 동기화 문제를 감지할 수 있도록 검증 레이어를 확인해보세요.

이것으로 1장이 끝났습니다. 다음은 컴퓨트 셰이더를 사용해 단순한 깜빡이는 화면보다 조금 더 복잡한 것을 그려보겠습니다.

Next: [Chapter 2: Improving the render loop]({{ site.baseurl }}{% link docs/ko/new_chapter_2/vulkan_new_rendering.md %})



{% include comments.html term="Vkguide 2 Beta Comments" %}
