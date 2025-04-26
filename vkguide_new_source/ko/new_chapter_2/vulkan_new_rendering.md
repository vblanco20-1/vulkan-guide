---
layout: default
title: Improving the render loop
parent:  "2. Drawing with Compute - KO"
grand_parent: "Korean VKGuide"
nav_order: 1
---

그리기를 시작하기 전에, 몇 가지 추가로 구현해야 할 작업이 있습니다. 먼저, 점점 늘어나는 객체들을 안전하게 정리할 수 있도록 삭제 큐를 만들고, 렌더링 루프를 수정하여 스왑체인 이미지가 아닌 곳에 먼저 그린 후 해당 이미지를 스왑체인로 복사하도록 하겠습니다.


## 삭제 큐
점점 더 많은 Vulkan 구조체를 추가하게 되면서, 파괴를 효율적으로 관리할 수단이 필요합니다. `cleanup()`함수에 항목을 추가할 수도 있지만, 규모가 커질수록 유지보수가 어렵고, 동기화 오류가 발생하기 쉬워집니다. 그래서 `VulkanEngine`에 삭제 큐라는 새로운 구조체를 추가하겠습니다. 이는 많은 엔진에서 사용하는 일반적인 방식으로, 삭제할 객체들을 큐에 담아 두었다가, 큐를 실행함으로써 객체들을 올바른 순서로 삭제하는 구조입니다. 이 구현에서는 단순함을 우선시하여 `std::function` 콜백을 덱(deque)에 저장할 것입니다. 덱을 FILO(First In Last Out)큐 처럼 사용할 것이므로, 마지막에 추가된 객체부터 먼저 파괴됩니다.

해당 구현은 다음과 같습니다.

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

`std::function`은 람다를 담아 콜백과 데이터를 함께 담을 수 있으며, 이러한 용도에 적합합니다.

하지만 각 객체마다 `std::function`을 저장하는 방식은 규모가 커질 경우 비효율적입니다. 이 튜토리얼에서 사용할 객체 수에는 큰 문제가 되지 않지만, 수천 개의 객체를 더 빠르게 파괴하고 싶다면 `VkImage`, `VkBuffer`과 같은 다양한 타입의 Vulkan 핸들을 저장해 반복문에서 파괴하는 편이 낫습니다.

다양한 생명 주기를 가진 객체를 관리하기 위해 삭제 큐를 여러 위치에 둘 것입니다. 그중 하나는 `VulkanEngine` 클래스에서 엔진이 종료 시 전역 객체들을 정리하는 역할을 합니다. 또한 각 프레임마다 별도의 삭제 큐를 두어, 사용이 끝난 객체를 다음 프레임에 안전하게 삭제할 수 있도록 하겠습니다. 이 삭제 큐를 `VulkanEngine` 클래스와 그 내부에 있는 `FrameData` 구조체에 추가합니다.

이를 `VulkanEngine` 클래스의 `FrameData` 구조체 내부에 추가합니다.

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

삭제 큐를 두 위치에서 호출합니다. 하나는 프레임마다 펜스를 대기한 직후, 다른 하나는 WaitIdle 호출 이후 `cleanup()`함수에서입니다. 펜스 바로 뒤에서 삭제 큐를 비움으로써, GPU가 해당 프레임의 작업을 모두 완료한 후에만 그 프레임에 생성된 객체들을 안전하게 삭제할 수 있도록 합니다. 또한 프레임의 나머지 데이터를 파괴할 때 이러한 프레임별 자원도 함께 해제되도록 보장합니다.

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
		
		//free per-frame structures and deletion queue
		for (int i = 0; i < FRAME_OVERLAP; i++) {

			vkDestroyCommandPool(_device, _frames[i]._commandPool, nullptr);

			//destroy sync objects
			vkDestroyFence(_device, _frames[i]._renderFence, nullptr);
			vkDestroySemaphore(_device, _frames[i]._renderSemaphore, nullptr);
			vkDestroySemaphore(_device, _frames[i]._swapchainSemaphore, nullptr);

			_frames[i]._deletionQueue.flush();
		}

		//flush the global deletion queue
		_mainDeletionQueue.flush();

		//rest of cleanup function
	}
}
```

삭제 큐를 설정했으므로 이제 Vulkan 객체를 생성할 때 마다 언제든지 큐에 넣을 수 있습니다.

## 메모리 할당

렌더링 루프를 개선하려면 이미지를 할당해야 하며, 이는 Vulkan에서 객체가 어떻게 할당되는지 알아야 한다는 뜻입니다. 하지만 우리는 Vulkan Memory Allocator 라이브러리를 사용하고 있으므로 전반적인 과정은 생략할 것입니다. 이미지 정렬과 같은 엄격한 제약 조건을 갖는 객체와 다양한 메모리 힙을 다루는 것은 오류가 발생하기 쉽고, 특히 성능까지 고려한다면 구현하기 매우 어렵습니다. VMA를 사용하면 이러한 작업들을 생략할 수 있으며, 검증된 방식으로 안정적인 동작을 보장받을 수 있습니다. 실제로 pcsx3 에뮬레이터 프로젝트에서도 할당 방식을 VMA로 교체한 결과 프레임 레이트가 20% 향상되었습니다.

`vk_types.h`에는 이미 VMA 라이브러리를 포함하는 코드가 있지만, 몇 가지 추가할 작업이 있습니다.

`vk_engine.cpp`에서는 `VMA_IMPLEMENTATION`를 정의한 상태로 VMA를 포함해야 합니다
```cpp
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
```

VMA는 일반적인 헤더와 구현이 같은 헤더 파일에 들어 있는 방식입니다. 프로젝트의 .cpp 파일 중 한 곳에 `VMA_IMPLEMENTATION`을 정의해야 하며, 그 파일에 VMA 함수들의 정의가 컴파일 될 것입니다.

VMA를 `VulkanEngine` 클래스에 추가하겠습니다.
```cpp
class VulkanEngine{

    VmaAllocator _allocator;
}
```

이제 `init_vulkan()`함수의 끝 부분에서 할당기 초기화를 수행하겠습니다.

^code vma_init chapter-2/vk_engine.cpp

설명할 것이 많지는 않습니다. `_allocator` 멤버를 초기화한 뒤, 해당 파괴 함수를 삭제 큐에 추가해 엔진이 종료될 때 자동으로 정리되도록 했습니다. 물리 디바이스, 인스턴스, 디바이스도 생성 함수에 연결하였으며, `VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT`를 전달해 이후 GPU 포인터가 필요할 때 사용할 수 있도록 설정했습니다.
VMA 라이브러리는 Vulkan API와 유사한 호출 방식을 따르므로, 유사한 info 구조체를 사용해 동작합니다.

# 새로운 렌더링 루프

스왑체인에 직접 렌더링하는 방식은 많은 프로젝트에서 무난하게 사용되며, 스마트폰과 같은 환경에서는 오히려 최적의 방식일 수 있습니다. 그러나 몇 가지 제약이 따릅니다. 가장 중요한 제약은 스왑체인에 사용되는 이미지의 포맷이 보장되지 않는다는 점입니다. OS, 드라이버, 창 모드에 따라 최적의 스왑체인 포맷이 달라질 수 있습니다. HDR을 지원하려면 특정한 포맷이 요구되기도 합니다. 또 다른 제약은 우리는 창 표시 시스템으로부터 스왑체인 이미지 인덱스만을 얻어온다는 점입니다. 일부 저지연 렌더링 기술에서는 다른 이미지에 렌더링 한 뒤, 그 이미지를 스왑체인에 매우 짧은 지연으로 전송하는 방식도 사용할 수 있습니다.

한 가지 중요한 제한은 해상도가 창 크기에 고정되어 있다는 점입니다. 더 높은, 혹은 더 낮은 해상도로 렌더링하고 싶다면, 확대/축소 로직을 구현해야 하며, 그리기 작업을 별도의 수행해야 합니다.

또한, 대부분의 스왑체인 포맷은 낮은 정밀도를 가지고 있습니다. HDR 렌더링을 지원하는 일부 플랫폼은 더 높은 정밀도의 포맷을 제공하지만, 일반적으로는 색상당 8비트가 기본입니다. 따라서 높은 정밀도의 조명 계산이나 밴딩 현상을 방지하고, 정규화된 색상 범위(1.0)을 초과하는 표현을 하고 싶다면, 별도의 이미지에 렌더링을 해야 합니다.

이러한 이유로, 이 튜토리얼에서는 스왑체인 이미지가 아닌 별도의 이미지에 렌더링 한 후 그 결과 이미지를 스왑체인에 복사하여 화면에 출력하는 방식을 사용하겠습니다.

우리가 사용할 이미지는 RGBA 16비트 float 포맷을 사용할 것입니다. 이는 다소 과할 수 있지만 조명 계산과 고품질 렌더링 시 유용한 높은 정밀도를 제공합니다.


# Vulkan 이미지
스왑체인을 설정할 때 이미지에 대해 간단히 다룬 적이 있지만, 그 과정은 VkBootstrap이 처리해주었습니다. 이번에는 이미지를 직접 생성해보겠습니다.


먼저, `VulkanEngine` 클래스에 필요한 새로운 멤버를 추가해보겠습니다.

`vk_types.h`에 이미지 생성을 위해 필요한 데이터를 담을 구조체를 추가합니다. 이 구조체에는 `VkImage`를 기본 `VkImageView`, 이미지 메모리 할당 객체, 그리고 이미지 크기와 포맷과 함께 저장합니다. 이러한 정보들은 이미지를 다룰 때 유용하게 사용할 수 있습니다. 또한 렌더링할 해상도를 결정하는 데 사용할 `_drawExtent`도 추가하겠습니다.

```cpp
struct AllocatedImage {
    VkImage image;
    VkImageView imageView;
    VmaAllocation allocation;
    VkExtent3D imageExtent;
    VkFormat imageFormat;
};
```

```cpp
class VulkanEngine{

	//draw resources
	AllocatedImage _drawImage;
	VkExtent2D _drawExtent;
}
```

`vk_intializers` 함수에 이미지와 이미지뷰의 `createInfo` 함수를 추가합니다.

^code image_set shared/vk_initializers.cpp

이미지 타일링은 `VK_IMAGE_TILING_OPTIMAL`로 하드코딩할 것입니다. 이는 GPU가 데이터를 내부적으로 최적화하여 자유롭게 재배치할 수 있도록 허용한다는 의미입니다. 만약 CPU에서 이미지 데이터를 직접 읽고 싶을 때, `VK_IMAGE_TILING_LINEAR`을 사용해야 하며, 이는 GPU 데이터를 단순한 2차원 배열로 만들어줍니다. 하지만 이 방식은 GPU의 활용에 큰 제약을 주기 때문에, LINEAR 타일링의 실제 용례는 CPU에서 읽는 경우밖에 없습니다.

이미지 뷰를 생성할 때에는 `subresource`를 설정해야 합니다. 이는 파이프라인 배리어에서 작성했던 것과 유사합니다.

이제 `init_swapchain`의 마지막 부분에 이미지 생성 코드를 추가해보겠습니다.

^code init_swap chapter-2/vk_engine.cpp

먼저 원하는 이미지 크기의 `VkExtent3d` 구조체를 생성하겠습니다. 이는 창의 크기와 일치하도록 할 것이며, 해당 값을 `AllocatedImage` 구조체에 복사합니다.

그 다음으로는 `usage` 플래그를 설정해야 합니다. Vulkan에서는 모든 버퍼와 이미지에 대해 사용 목적을 나타내는 `usage` 플래그를 지정해야 하며, 이는 드라이버가 해당 리소스의 사용 방식을 기반으로 내부적으로 최적화를 수행할 수 있도록 도와줍니다. 우리의 경우 이미지 간 복사를 위해 `TransferSRC`와 `TransferDST`를 설정하고, 컴퓨트 셰이더가 쓰기를 할 수 있도록 `STORAGE`를, 그리고 그래픽스 파이프라인으로 도형을 그릴 수 있도록 `COLOR_ATTACHMENT`를 추가합니다.

이미지 포맷은 `VK_FORMAT_R16G16B16A16_SFLOAT`를 사용하겠습니다. 이는 4채널의 16비트 float이며, 픽셀당 64비트를 차지합니다. 이는 일반적인 8비트 색상 포맷의 두 배 용량이지만, 조명 계산 등에서 유용하게 사용될 것입니다.

이미지를 실제로 생성할 때는 `imageInfo` 구조체와 `allocInfo` 구조체를 VMA에 전달해야 합니다. 그러면 VMA는 내부적으로 Vulkan 생성 함수를 호출하여 이미지를 생성해줍니다. 여기서 중요한 점은 `usage`와 `required` 플래그입니다. `VMA_MEMORY_USAGE_GPU_ONLY`를 사용해 VMA에게 해당 이미지가 CPU에서 접근되지 않는 GPU 전용 텍스쳐 임을 알려주어 GPU VRAM에 배치하게 합니다. 추가로 `VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT`도 설정하여 GPU 전용 메모리에 할당되도록 명시합니다. 이 플래그는 GPU VRAM만을 위한 플래그로, 가장 빠른 접근 속도를 보장합니다.

Vulkan에서는 이미지와 버퍼를 할당할 수 있는 여러 메모리 영역이 있습니다. 전용 GPU를 갖는 PC에서는 일반적으로 CPU RAM 영역, GPU VRAM 영역, 그리고 CPU가 접근할 수 있는 특수한 GPU VRAM 영역인 "업로드 힙" 이 있습니다. 만약 resizable BAR가 활성화되어 있다면, 업로드 힙이 전체 VRAM이 될 수 있습니다. 그렇지 않다면 일반적으로 256MB입니다. VMA에게 GPU_ONLY로 할당하라고 지시하여, 업로드 힙 외부의 순수 VRAM에 자원을 배치하도록 합니다.

이미지를 할당한 후에는 대응되는 이미지 뷰도 생성해야 합니다. Vulkan에서는 이미지에 접근하기 위해 반드시 이미지 뷰가 필요합니다. 이는 이미지 위에 존재하는 얇은 래퍼입니다. 예를 들어 하나의 밉맵 레벨에만 접근하도록 제한할 수도 있습니다. 이 튜토리얼에서는 항상 VkImage와 그에 해당하는 기본 이미지 뷰를 쌍으로 묶어 사용합니다.

# 새로운 렌더링 루프

이제 이미지를 그릴 수 있는 공간이 마련되었습니다. 렌더링 루프에 해당 이미지를 추가하겠습니다.

그 전에 이미지간 복사가 필요하므로, `vk_images.cpp`에 이미지 복사 함수를 추가합니다.

^code copyimg shared/vk_images.cpp
 
대응되는 선언을 `vk_images.h`에 추가합니다.
```cpp
	namespace vkutil {

	    // Existing:
	    void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);

		void copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize);	
	}
```

Vulkan은 이미지를 복사하는 2가지 주요 방법을 제시합니다. `VkCmdCopyImage`와 `VkCmdBlitImage`입니다. CopyImage는 속도는 빠르지만 제약이 있습니다. 예를 들어 두 이미지의 해상도가 반드시 같아야 합니다. 반면 blitImage는 서로 다른 포맷과 크기의 이미지도 복사할 수 있습니다. 복사에 사용할 소스 사각형과 타겟 사각형을 지정하면, 시스템이 해당 위치로 이미지를 복사합니다. 이 두 함수는 엔진 초기 설정 단계에서는 유용하지만, 이후에는 전체 화면 프래그먼트 셰이더에 커스텀 로직을 추가하는 방식으로 대체하는 편이 좋습니다.

이제 이를 바탕으로 렌더링 루프를 업데이트할 수 있습니다. `draw()`가 점점 더 커지고 있으므로 동기화, 커맨드 버퍼 관리, 이미지 변환 처리를 draw()함수에 그대로 두고 실제 그리기 명령만 분리하여 `draw_background()`함수로 옮기겠습니다.

```cpp
void VulkanEngine::draw_background(VkCommandBuffer cmd)
{
^code draw_clear chapter-2/vk_engine.cpp
}
```

헤더에 함수를 추가합니다.

커맨드 버퍼에 기록하는 코드를 수정할 것입니다. 기존 코드는 삭제하고 아래 새로운 코드로 대체합니다.
```cpp
^code draw_first chapter-2/vk_engine.cpp

	// execute a copy from the draw image into the swapchain
	vkutil::copy_image_to_image(cmd, _drawImage.image, _swapchainImages[swapchainImageIndex], _drawExtent, _swapchainExtent);

	// set swapchain image layout to Present so we can show it on the screen
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	//finalize the command buffer (we can no longer add commands, but it can now be executed)
	VK_CHECK(vkEndCommandBuffer(cmd));
```

렌더링 루프의 주요 차이점은 더 이상 스왑체인 이미지 그 자체에 `clear`를 수행하지 않는다는 점입니다. 대신 `_drawImage.image`에 `clear`를 수행합니다. 이미지가 `clear`되면 스왑체인 이미지와 렌더링된 이미지를 전송 레이아웃으로 변환하고 복사 명령을 실행합니다. 복사가 수행되면 스왑체인 이미지를 표시 레이아웃으로 전환합니다. 항상 같은 이미지에 렌더링하기 때문에 `draw_image`가 더이상 스왑체인 인덱스에 접근할 필요가 없고, 단순히 `draw_image`를 초기화만 하면 됩니다. 또한 `_drawExtent`값을 설정하여 이후 그리기 영역에 사용할 것입니다.

이제 스왑체인 외부에서 이미지를 렌더링하는 방법을 사용할 수 있게 되었습니다. 상당히 픽셀 정밀도를 얻었으며 다양한 추가 기법도 사용할 수 있습니다

이제 실제 컴퓨트 셰이더 실행 단계로 넘어갈 준비가 끝났습니다.

^nextlink

{% include comments.html term="Vkguide 2 Korean Comments" %}