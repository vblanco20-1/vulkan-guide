---
layout: default
title: Vulkan Initialization Code
parent:  "1. Initializing Vulkan"
nav_order: 2
---


# 핵심 Vulkan 구조체 초기화하기
0장의 코드에서 시작한다고 가정하겠습니다. 프로젝트 설정을 하지 않았다면 0장에서 프로젝트를 설정한 후 읽기를 권장합니다.

우선 초기화 코드를 간단히 하기 위한 vkBootstrap 라이브러리를 `#include`로 가져옵니다. 이를 위해 `vk_engine.cpp` 상단에 `"VkBootstrap.h"`헤더를 추가합니다.

```cpp
// --- other includes ---
#include <vk_types.h>
#include <vk_initializers.h>

//bootstrap library
#include "VkBootstrap.h"

```

VkBootstrap은 엔진의 초기화 코드를 대폭 줄여 수백 줄의 코드를 생략할 수 있도록 합니다. VkBootstrap없이 어떻게 작동하는지 알고 싶다면 [vulkan-tutorial](https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code)을 읽어보는 것을 추천합니다.


가장 먼저 초기화해야되는 것은 Vulkan Instance입니다. 이를 위해 새로운 함수와 핸들을 저장할 변수를 `VulkanEngine`클래스에 추가합니다. 또한, 다른 초기화를 위한 몇가지 함수를 추가합니다.

vk_engine.h
```cpp
class VulkanEngine {
public:

// --- omitted ---

^code inst_init chapter-1/vk_engine.h

private:

	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_sync_structures();
```

이제 이러한 `init` 함수들을 엔진의 `init`함수에서 호출할 것입니다.

vk_engine.cpp
```cpp
^code init_fn chapter-1/vk_engine.cpp

void VulkanEngine::init_vulkan()
{
    //nothing yet
}
void VulkanEngine::init_swapchain()
{
    //nothing yet
}
void VulkanEngine::init_commands()
{
    //nothing yet
}
void VulkanEngine::init_sync_structures()
{
    //nothing yet
}
```

메인 클래스에 `VkDevice`, `VkPhysicalDevice`, `VkInstance`, `VkDebugUtilsMessengerEXT` 4개의 핸들을 추가합니다.

## Instance

이제 새로운 `init_Vulkan` 함수가 추가되었으므로 `VkInstance`를 생성하는 코드를 작성해봅시다.


```cpp
void VulkanEngine::init_vulkan()
{
^code init_instance chapter-1/vk_engine.cpp
}
```

`vkb::InstanceBuilder`를 만들 것입니다. 이는 VkBootstrap 라이브러리의 객체로, `VkInstance`생성을 보다 간단히 처리할 수 있도록 추상화합니다.

인스턴스를 생성할 때 "Example Vulkan Application"이라는 이름을 사용하고, 검증 레이어를 활성화 하며, 기본 디버그 로거를 사용하겠습니다. "Example Vulkan Application"이라는 이름은 원하는 다른 이름으로 대체해도 좋습니다. `VkInstance`를 초기화할 때, 엔진과 애플리케이션의 을 제공해야 합니다. 이는 드라이버 제조사가 게임/엔진의 이름을 쉽게 찾아, 해당 소프트웨어에 맞게 내부 드라이버 로직을 최적화할 수 있도록 하기 위함입니다. 우리에게는 그리 중요한 사항은 아닙니다.

기본적으로 검증 레이어를 활성화해야합니다. 이 가이드를 진행하면서 검증 레이어가 오류를 효과적으로 잡아주기 때문에 이를 비활성화 할 필요는 없습니다. 더 고급 엔진에서는 디버그 모드에서만 활성화하거나, 특정 설정을 통해 조정할 수 있습니다. Vulkan 검증 레이어는 Vulkan 호출 성능을 상당히 저하시킬 수 있으며, 따라서 많은 데이터를 포함하는 복잡한 씬이 불러올 때 실제 코드의 성능을 하려면 이를 비활성화하는 것이 좋습니다.

Vulkan API 1.3버전을 사용할 것입니다. 이는 비교적 최신 GPU에서 지원됩니다. 우리는 Vulkan 1.3이 제공하는 기능들을 활용할 것입니다. 만약 해당 기능을 지원하지 않는 오래된 PC/GPU를 사용중이라면 Vulkan 1.1버전을 사용하는 이전 버전의 vkguide를 보는 것을 권장합니다.

마지막으로, 라이브러리에 디버그 메신저를 사용을 요청합니다. 이는 검증 레이어의 출력을 로그로 수집합니다. 사용자 정의 디버그 메신저가 필요 없으므로 라이브러리에서 기본값(콘솔 창 출력)을 사용하도록 설정합니다.

이제 `vkb::Instance` 객체에서 `VkInstance` 핸들과 `VkDebugUtilsMessengerEXT` 핸들을 가져옵니다. 프로그램 종료 시 이를 안전하게 해제할 수 있도록 `VkDebugUtilsMessengerEXT`를 따로 저장합니다. 그렇지 않으면 메모리 누수가 발생할 수 있습니다.


## Device
```cpp
void VulkanEngine::init_vulkan()
{
	// other code ------

^code init_device chapter-1/vk_engine.cpp
}
```
사용할 GPU를 선택하기 위해 `vkb::PhysicalDeviceSelector`를 사용할 것입니다.

가장 먼저, SDL 창으로부터 `VkSurfaceKHR` 객체를 생성해야 합니다. 이 객체는 실제로 렌더링이 이루어질 창이므로, `PhysicalDeviceSelector`에게 해당 창에 렌더링할 수 있는 GPU를 선택하도록 지시해야 합니다.

몇 가지 기능도 활성화해야 합니다. 우선 Vulkan 1.3 기능으로는 Dynamic Rendering과 Synchronization 2가 있습니다. 이들은 Vulkan 1.3에서 제공하는 선택적인 기능으로, Dynamic rendering을 사용하면 렌더패스와 프레임버퍼를 생략할 수 있습니다(이에 대해 더 알고 싶다면 구버전의 VkGuide를 참고하세요). 또한 새로운 동기화 함수들을 사용할 수 있게 됩니다. 이와 함께 Vulkan1.2 기능인 BufferDeviceAddress와 descriptorIndexing도 사용할 예정입니다. BufferDeviceAddress는 버퍼를 바인딩하지 않고도 GPU 포인터를 사용할 수 있게 해주며, descriptorIndexing은 바인드리스 텍스쳐를 지원합니다.
 
`vkb::PhysicalDeviceSelector`에 `VkPhysicalDeviceVulkan13Features`구조체를 전달함으로써, vkbootstrap에게 해당 기능들을 지원하는 GPU를 찾도록 요청할 수 있습니다.


Vulkan 버전에 따라 사용할 수 있는 다양한 레벨의 기능 구조체가 있으며, 아래 링크에서 정보를 확인할 수 있습니다.

[Vulkan Spec: 1.0 physical device features](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap47.html#VkPhysicalDeviceFeatures)
[Vulkan Spec: 1.1 physical device features](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap47.html#VkPhysicalDeviceVulkan11Features)
[Vulkan Spec: 1.2 physical device features](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap47.html#VkPhysicalDeviceVulkan12Features)
[Vulkan Spec: 1.3 physical device features](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap47.html#VkPhysicalDeviceVulkan13Features)


`VkPhysicalDevice`를 얻었으면 이로부터 `VkDevice`를 직접 생성할 수 있습니다.

마지막으로 생성된 핸들을 클래스에 저장합니다.

이것으로 끝났습니다. Vulkan이 초기화되었습니다. 이제 Vulkan 명령을 호출할 수 있습니다.

이 시점에서 프로젝트를 실행하면, 필요한 기능을 지원하지 않거나 Vulkan 드라이버가 설치되어 있지 않은 GPU를 사용할 경우 크래시가 발생할 수 있습니다. 그런 경우, 드라이버가 최신인지 확인하세요. 만약 GPU 자체가 해당 기능을 지원하지 않는 경우에는 이 튜토리얼을 따라갈 수 없으므로 구버전의 VkGuide를 참고해주세요.

## 스왑체인 설정하기

핵심 초기화 과정의 마지막 단계는 스왑체인을 초기화하는 것입니다. 그래야 렌더링할 수 있는 대상이 생깁니다.

먼저 `VulkanEngine`에 새로운 멤버와 함수를 추가합니다.

```cpp
class VulkanEngine {
public:
	// --- other code ---

^code swap_init chapter-1/vk_engine.h

private: 
	void create_swapchain(uint32_t width, uint32_t height);
	void destroy_swapchain();
}
```

`VkSwapchainKHR` 자체와 함께, 스왑체인 이미지들이 렌더링시 사용할 포맷도 저장합니다.

또한 두 개의 배열을 저장하는데, 하나는 `VkImage`의 배열이고 다른 하나는 `VkImageView`의 배열입니다.

`VkImage`는 텍스쳐로 사용하거나 렌더링 대상이 되는 실제 이미지 객체를 가리키는 핸들입니다.
`VkImageView`는 해당 이미지를 감싸는 래퍼로, 색상 포맷을 변경하는 등의 작업을 가능하게 합니다. 이에 대해서는 이후에 더 자세히 다룰 예정입니다.

또한 스왑체인을 위한 생성 및 제거 함수도 추가합니다.

다른 초기화 함수들과 마찬가지로, `vkb` 라이브러리를 사용해 스왑체인을 생성할 것입니다. 이는 `VkInstance`와 `VkDevice`를 만들 때 사용한 것과 유사할 것입니다.

^code init_swap chapter-1/vk_engine.cpp

`create_swapchain()`에서 스왑체인 구조체를 생성한 뒤, `init_swapchain()`에서 해당 함수를 호출합니다.

여기서 가장 중요한 부분은 표시 모드(Present Mode)로, `VK_PRESENT_MODE_FIFO_KHR`을 설정한 부분입니다. 이 설정은 엄격한 VSync를 의미하며, 전체 엔진의 FPS를 모니터 주사율에 맞춰 제한하게 됩니다.

또한 스왑체인 생성 시 창의 크기를 전달합니다. 스왑체인을 생성하면 동시에 해당 크기에 맞는 이미지들도 생성되므로, 이미지 크기는 고정됩니다. 이후 튜토리얼에서는 창 크기 변경에 따라 스왑체인을 다시 생성해야 하므로, 초기화 흐름과 분리해 관리할 예정입니다. 하지만 초기화 시점에서는 창의 크기를 기본값으로 설정합니다.

스왑체인이 생성되면, 그와 관련된 모든 정보를 `VulkanEngine` 클래스의 멤버 변수에 저장합니다.

`destroy_swapchain()`함수도 작성해봅시다.

^code destroy_sc chapter-1/vk_engine.cpp

우선 스왑체인 객체를 삭제하는데, 이 작업은 내부적으로 포함된 이미지들도 함께 삭제합니다. 그런 다음, 이 이미지들에 해당하는 `ImageView`들도 따로 제거해주어야 합니다.

## 자원 정리하기
애플리케이션이 종료될 때 우리가 생성한 모든 Vulkan 자원들이 올바르게 해제되도록 해야 합니다.

이를 위해 `VulkanEngine::cleanup()`함수로 이동합시다.

```cpp
void VulkanEngine::cleanup()
{
	if (_isInitialized) {

		destroy_swapchain();

		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		vkDestroyDevice(_device, nullptr);
		
		vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
		vkDestroyInstance(_instance, nullptr);
		SDL_DestroyWindow(_window);
	}
}
```

객체들 간에는 의존성이 있기 때문에, 올바른 순서로 삭제해야 합니다. 일반적으로 생성한 순서의 역순으로 삭제하는 것이 좋은 방법입니다. 어떤 경우에는, 만약 구조를 잘 이해하고 있다면 순서를 약간 바꾸어도 문제가 없을 수 있습니다.

`VkPhysicalDevice`는 Vulkan 자원 그 자체가 아니기 때문에 삭제할 수 없습니다. 이는 시스템 내 GPU를 가리키는 핸들일 뿐입니다.

우리는 Window → Instance → Surface → Device → Swapchain 순서로 초기화했기 때문에, 삭제는 정확히 그 반대 순서로 수행됩니다.

지금 프로그램을 실행해보면 아무것도 하지 않을 것입니다. 하지만 그 "아무것도 하지 않음"에는 에러도 발생하지 않는다는 점이 포함됩니다.

이번 경우에는 `VkImage`들을 별도로 삭제할 필요는 없습니다. 왜냐하면 이미지들은 스왑체인에 소유되어 있고, 스왑체인을 삭제할 때 함께 삭제되기 때문입니다.

## 검증 레이어 에러
검증 레이어가 제대로 작동하는지 확인하기 위해, 이번에는 의도적으로 잘못된 순서로 파괴 함수를 호출해보겠습니다.

```cpp
void VulkanEngine::cleanup()
{
	if (_isInitialized) {
		//ERROR - Instance destroyed before others
		vkDestroyInstance(_instance, nullptr);

		destroy_swapchain();

		vkDestroyDevice(_device, nullptr);
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
		SDL_DestroyWindow(_window);
	}
}
```

지금 우리는 `VkInstance`를 `VkDeivce`와 `VkInstance`로부터 생성된 `VkSurfaceKHR`보다 먼저 삭제하고 있습니다. 이 경우 검증 레이어는 다음과 같은 오류 메시지로 경고를 출력할 것입니다.

```
[ERROR: Validation]
Validation Error: [ VUID-vkDestroyInstance-instance-00629 ] Object 0: handle = 0x24ff02340c0, type = VK_OBJECT_TYPE_INSTANCE; Object 1: handle = 0xf8ce070000000002, type = VK_OBJECT_TYPE_SURFACE_KHR; | MessageID = 0x8b3d8e18 | OBJ ERROR : For VkInstance 0x24ff02340c0[], VkSurfaceKHR 0xf8ce070000000002[] has not been destroyed. The Vulkan spec states: All child objects created using instance must have been destroyed prior to destroying instance (https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#VUID-vkDestroyInstance-instance-00629)
```

Vulkan 초기화가 완료되었고 검증 레이어도 정상적으로 작동하므로, 이제 GPU가 무언가를 수행할 수 있도록 명령 구조체를 준비하겠습니다.

^nextlink

{% include comments.html term="Vkguide 2 Beta Comments" %}