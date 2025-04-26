---
layout: default
title: Window Resizing
parent: "3. Graphics Pipelines"
nav_order: 25
---

## 창 크기 재조정하기
Vulkan에서는 창 크기 조정을 직접 처리해 주어야 합니다. 0장에서 이미 창이 최소화되었을 때를 처리하는 코드를 작성했지만, 창 크기 조정은 훨씬 더 복잡한 작업입니다. 

창 크기가 조정될 때 스왑체인은 더 이상 유효하지 않게 되며, `vkAcquireNextImageKHR`, `vkQueuePresentKHR`과 같은 Vulkan의 스왑체인 명령은 `VK_ERROR_OUT_OF_DATE_KHR` 오류를 반환할 수 있습니다. 이를 올바르게 처리하여 스왑체인을 새로운 크기로 재생성하게 해야 합니다.

효율성을 위해 drawImage를 재할당하지는 않을 것입니다. 지금 당장은 drawImage와 depthImage만을 갖지만 조금 더 고도화된 엔진에서는 상당히 많은 이미지가 있을 것이며 이 모든 것을 재생성하는 것은 번거롭고 비용이 큽니다. 대신, drawImage와 depthImage를 시작 시 프리셋된 크기로 생성해 창이 작을 경우 해당 이미지의 일부분에 렌더링하고, 창이 커지면 더 넓은 영역에 렌더링하도록 합니다. 이미지를 재할당하지 않고 구석에만 렌더링하기 때문에 이 로직은 동적 해상도 구현에도 응용할 수 있습니다. 동적 해상도는 성능을 유연하게 조정할 수 있는 좋은 방법이며, 디버깅 시에도 유용하게 활용할 수 있습니다. 현재 drawImage에서 스왑체인 이미지로 VkCmdBlit을 사용해 복사하고 있으며, 이 연산은 확대 축소를 수행하기 때문에 이러한 방식에서도 잘 작동할 것입니다. 물론 이러한 방식의 확대 축소는 최고 품질은 아닙니다. 일반적으로는 확대 시 sharpening같은 추가 필터를 적요하거나, 안티앨리어싱 효과를 흉내내는 등의 복잡한 처리가 더해져야 좋은 품질을 얻을 수 있습니다. ImGui UI는 여전히 스왑체인 이미지에 직접 그릴 것이므로, 항상 네이티브 해상도로 렌더링할 것입니다.

이제 창 생성 시 창 크기 조정 가능(resizable) 플래그를 활성화해봅시다. 그후 실제로 창 크기를 조정해 보면 어떤 일이 일어나는지 확인해볼 수 있습니다.

`VulkanEngine::init`의 상단에 window_flags를 바꾸어 resizable 플래그를 갖도록 합니다.

```cpp
SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
```
SDL은 창 크기 조정과 관련된 운영체제 레벨의 처리를 담당하므로, 지금 당장 창 크기 조정을 시도해볼 수 있습니다. 엔진을 실행해 창을 재조정해봅시다.

이 작업을 하면 `vkAcquireNextImageKHR` 혹은 `vkQueuePresentKHR` 중 하나에서 VK_CHECK 매크로가 에러를 감지하고 크래시가 발생할 것입니다. 발생하는 에러는 `VK_ERROR_OUT_OF_DATE_KHR`일 가능성이 큽니다. 따라서 이 에러를 제대로 처리하려면, 해당 에러가 발생했을 때 렌더링을 중단하고 스왑체인을 다시 생성해 주어야 합니다.

먼저, VulkanEngine 클래스에 `resize_requested`라는 이름의 bool 변수를 추가합니다.

draw()함수에 `vkAcquireNextImageKHR` 호출을 에러 코드를 확인하는 코드로 대체합니다.

```cpp
	VkResult e = vkAcquireNextImageKHR(_device, _swapchain, 1000000000, get_current_frame()._swapchainSemaphore, nullptr, &swapchainImageIndex);
	if (e == VK_ERROR_OUT_OF_DATE_KHR) {
        resize_requested = true;       
		return ;
	}
```

또한 `vkQueuePresentKHR`도 동일하게 대체합니다. 하지만 이미 함수의 끝이기 때문에 `return`은 하지 않습니다.
```cpp
VkResult presentResult = vkQueuePresentKHR(_graphicsQueue, &presentInfo);
if (presentResult == VK_ERROR_OUT_OF_DATE_KHR) {
    resize_requested = true;
}
```

이제 해당 에러가 발생하면 렌더링을 중단하게 되므로, 창 크기를 조정해도 크래시는 발생하지 않겠지만, 화면이 다시 그려지지 않고 이미지가 멈춘 상태로 유지될 것입니다.

`resize_swapchain()`함수를 추가하여 VulkanEngine이 스왑체인을 재생성하도록 합시다.

<!-- codegen from tag resize_swap on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_engine.cpp --> 
```cpp
void VulkanEngine::resize_swapchain()
{
	vkDeviceWaitIdle(_device);

	destroy_swapchain();

	int w, h;
	SDL_GetWindowSize(_window, &w, &h);
	_windowExtent.width = w;
	_windowExtent.height = h;

	create_swapchain(_windowExtent.width, _windowExtent.height);

	resize_requested = false;
}
```

스왑체인을 재생성하기 위해 GPU가 모든 렌더링 작업을 완료할 때 까지 대기해야 합니다. GPU가 여전히 이미지와 이미지 뷰를 사용중일 수 있기 때문에, 그 상태에서 변경해서는 안됩니다. 그후 스왑체인을 파괴하고 SDL로부터 창크기를 조회하여 그에 맞게 새로 생성합니다.

이제 run() 반복문에서 이미지가 재조정될 때 이 함수를 호출해야 합니다.

SDL 이벤트 루프와 freeze_rendering 확인 이후, 새로운 ImGui 프레임을 시작하는 함수들보다 먼저 이 함수를 호출하도록 코드를 추가합니다. 
```cpp
if (resize_requested) {
	resize_swapchain();
}
```

이제 재조정 기능이 구현되었으니 한번 시도해보세요. 이제 에러 없이 이미지 축소가 가능합니다. 하지만 창을 더 크게 만든다면 실패할 것입니다. drawImage 영역 밖으로 렌더링하려고 시도했기 때문입니다. 이 문제는 _drawExtent 변수를 도입하고 해당 변수가 drawImage 크기를 초과하지 않도록 설정함으로써 해결할 수 있습니다.

`_drawExtent`와 동적 해상도 조절을 위해 사용할 `renderScale` float를 VulkanEngine 클래스에 추가해줍니다.

```cpp
VkExtent2D _drawExtent;
float renderScale = 1.f;
```

draw() 함수로 돌아와 시작할 때 기존에 drawImage Extent를 사용하는 부분 대신 drawExtent를 계산하도록 합니다.

```cpp
_drawExtent.height = std::min(_swapchainExtent.height, _drawImage.imageExtent.height) * renderScale;
_drawExtent.width= std::min(_swapchainExtent.width, _drawImage.imageExtent.width) * renderScale;
```

이제 ImGui에 슬라이더를 추가해 drawScale 파라미터를 제어할 수 있도록 하겠습니다.

`run()`함수에서 배경 관련 파라미터를 설정하는 ImGui 창 내부의 상단에 이 슬라이더 코드를 추가해주세요.

```cpp
if (ImGui::Begin("background")) {
ImGui::SliderFloat("Render Scale",&renderScale, 0.3f, 1.f);
//other code
}
```

이는 0.3에서 1.0까지의 렌더링 배율을 조정할 수 있는 슬라이더입니다. 1을 초과하면 해상도를 깨트릴 수 있기 때문에 제한합니다.

프로그램을 실행해 창 크기를 재조정하고 렌더링 배율을 조정해보세요. 이제 동적으로 해상도를 변경하고 창을 최대화하거나 이동할 수 있습니다.

현재 drawImage를 다소 작은 크기로 설정했지만 원한다면 init_swapchain()에서 생성되는 drawImage의 크기를 키워볼 수도 있습니다. _windowExntent 대신 모니터 해상도를 `drawImageExtent`에 직접 지정해보세요.

이것으로 3장이 끝났습니다. 이제 다음 장으로 넘어가겠습니다.

Next: [Chapter 4: New Descriptor Abstractions]({{ site.baseurl }}{% link docs/ko/new_chapter_4/descriptor_abstractions.md %})

{% include comments.html term="Vkguide 2 Beta Comments" %}
