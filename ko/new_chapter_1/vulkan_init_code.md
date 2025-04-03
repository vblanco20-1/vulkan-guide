---
layout: default
title: Vulkan Initialization Code
parent:  "1. Initializing Vulkan"
nav_order: 2
---


# 핵심 Vulkan 구조체 초기화하기
0장의 코드에서 시작한다고 가정하겠습니다. 프로젝트 설정을 하지 않았다면 0장에서 프로젝트를 설정한 후 읽기를 권장합니다.

우선 초기화 코드를 간단히 하기 위한 vkBootstrap 라이브러리를 `#include`로 가져옵니다. 이를 위해 vk_engine.cpp 상단에 "VkBootstrap.h"헤더를 추가합니다.

```cpp
// --- 기타 포함 헤더 ---
#include <vk_types.h>
#include <vk_initializers.h>

//VkBootstrap 라이브러리
#include "VkBootstrap.h"

```

VkBootstrap은 엔진의 초기화 코드를 대폭 줄여 수백 줄의 코드를 생략할 수 있도록 합니다. Vkbootstrap없이 어떻게 작동하는지 알고 싶다면 [vulkan-tutorial](https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code)을 읽어보는 것을 추천합니다.


가장 먼저 초기화해야되는 것은 Vulkan Instance입니다. 이를 위해 새로운 함수와 핸들을 저장할 변수를 VulkanEngine클래스에 추가합니다. 또한, 다른 초기화를 위한 몇가지 함수를 추가합니다.

vk_engine.h
```cpp
class VulkanEngine {
public:

// --- 중략 ---

	VkInstance _instance;// Vulkan 라이브러리 핸들
	VkDebugUtilsMessengerEXT _debug_messenger;// Vulkan 디버그 출력 핸들
	VkPhysicalDevice _chosenGPU;// 기본 장치로 선택한 GPU
	VkDevice _device; // 작업을 위한 Vulkan 디바이스
	VkSurfaceKHR _surface;// Vulkan window surface

private:

	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_sync_structures();
```

이제 이러한 init 함수들을 엔진의 init함수에서 호출할 것입니다.

vk_engine.cpp
```cpp
constexpr bool bUseValidationLayers = false;

void VulkanEngine::init()
{
    // SDL을 초기화하고 창을 생성합니다.
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

	init_commands();

	init_sync_structures();

    // 올바르게 이루어졌다면 초기화 플래그 변수를 설정합니다.
	_isInitialized = true;
}

void VulkanEngine::init_vulkan()
{
    // 이후에 작성합니다.
}
void VulkanEngine::init_swapchain()
{
    // 이후에 작성합니다.
}
void VulkanEngine::init_commands()
{
    // 이후에 작성합니다.
}
void VulkanEngine::init_sync_structures()
{
    // 이후에 작성합니다.
}
```

메인 클래스에 `VkDevice`, `VkPhysicalDevice`, `VkInstance`, `VkDebugUtilsMessengerEXT` 4개의 핸들을 추가합니다.

## Instance

이제 새로운 init_Vulkan 함수가 추가되었으므로 VkInstance를 생성하는 코드를 작성해봅시다.


```cpp
void VulkanEngine::init_vulkan()
{
	vkb::InstanceBuilder builder;

	//기본 디버그 기능을 활성화한 Vulkan Instance를 생성합니다.
	auto inst_ret = builder.set_app_name("Example Vulkan Application")
		.request_validation_layers(bUseValidationLayers)
		.use_default_debug_messenger()
		.require_api_version(1, 3, 0)
		.build();

	vkb::Instance vkb_inst = inst_ret.value();

	//Instance를 저장합니다.
	_instance = vkb_inst.instance;
	_debug_messenger = vkb_inst.debug_messenger;

}
```

`vkb::InstanceBuilder`를 만들 것입니다. 이는 VkBootstrap 라이브러리의 객체로, `VkInstance`생성을 보다 간단히 처리할 수 있도록 추상화합니다.

인스턴스를 생성할 때 "Example Vulkan Application"이라는 이름을 사용하고, 검증 레이어를 활성화 하며, 기본 디버그 로거를 사용하겠습니다. "Example Vulkan Application"이라는 이름은 원하는 다른 이름으로 대체해도 좋습니다. `VkInstance`를 초기화할 때, 엔진과 애플리케이션의 을 제공해야 합니다. 이는 드라이버 제조사가 게임/엔진의 이름을 쉽게 찾아, 해당 소프트웨어에 맞게 내부 드라이버 로직을 최적화할 수 있도록 하기 위함입니다. 우리에게는 그리 중요한 사항은 아닙니다.

기본적으로 검증 레이어를 활성화해야합니다. 이 가이드를 진행하면서 검증 레이어가 오류를 효과적으로 잡아주기 때문에 이를 비활성화 할 필요는 없습니다. 더 고급 엔진에서는 디버그 모드에서만 활성화하거나, 특정 설정을 통해 조정할 수 있습니다. Vulkan 검증 레이어는 vulkan 호출 성능을 상당히 저하시킬 수 있으며, 따라서 많은 데이터를 포함하는 복잡한 씬이 불러올 때 실제 코드의 성능을 하려면 이를 비활성화하는 것이 좋습니다.

Vulkan API 1.3버전을 사용할 것입니다. 이는 비교적 최신 GPU에서 지원됩니다. 우리는 Vulkan 1.3이 제공하는 기능들을 활용할 것입니다. 만약 해당 기능을 지원하지 않는 오래된 PC/GPU를 사용중이라면 Vulkan 1.1버전을 사용하는 이전 버전의 vkguide를 보는 것을 권장합니다.

마지막으로, 라이브러리에 디버그 메신저를 사용을 요청합니다. 이는 검증 레이어의 출력을 로그로 수집합니다. 사용자 정의 디버그 메신저가 필요 없으므로 라이브러리에서 기본값(콘솔 창 출력)을 사용하도록 설정합니다.

이제 `vkb::Instance` 객체에서 `VkInstance` 핸들과 `VkDebugUtilsMessengerEXT` 핸들을 가져옵니다. 프로그램 종료 시 이를 안전하게 해제할 수 있도록 `VkDebugUtilsMessengerEXT`를 따로 저장합니다. 그렇지 않으면 메모리 누수가 발생할 수 있습니다.


## Device
```cpp
void VulkanEngine::init_vulkan()
{
	// 중략 ------

	SDL_Vulkan_CreateSurface(_window, _instance, &_surface);

	//vulkan 1.3 기능
	VkPhysicalDeviceVulkan13Features features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
	features.dynamicRendering = true;
	features.synchronization2 = true;

	//vulkan 1.2 기능
	VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
	features12.bufferDeviceAddress = true;
	features12.descriptorIndexing = true;


	//use vkbootstrap to select a gpu. /GPU를 선택하기 위해 vkbootstrap을 사용합니다. 
	//We want a gpu that can write to the SDL surface and supports vulkan 1.3 with the correct features / SDL surface에 작성할 수 있고 지정한 vulkan 1.3 기능을 지원하는 GPU를 선택합니다.
	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version(1, 3)
		.set_required_features_13(features)
		.set_required_features_12(features12)
		.set_surface(_surface)
		.select()
		.value();


	//create the final vulkan device /최종적으로 vulkan device를 생성합니다.
	vkb::DeviceBuilder deviceBuilder{ physicalDevice };

	vkb::Device vkbDevice = deviceBuilder.build().value();

	// Get the VkDevice handle used in the rest of a vulkan application / 남은 Vulkan 애플리케이션에 사용할 VkDevice 핸들을 저장합니다.
	_device = vkbDevice.device;
	_chosenGPU = physicalDevice.physical_device;
}
```
To select a GPU to use, we are going to use `vkb::PhysicalDeviceSelector`. 
사용할 GPU를 고르기 위해 `vkb::PhysicalDeviceSelector`를 사용할 것입니다.

First of all, we need to create a `VkSurfaceKHR` object from the SDL window. This is the actual window we will be rendering to, so we need to tell the physical device selector to grab a GPU that can render to said window.
가장 먼저 SDL로부터 `VkSurfaceKHR` 객체를 만들어야 합니다. 이는 실제로 렌더링할 창을 나타냅니다. 따라서 `vkb:PhysicalDeviceSelector`에게 해당 창에 렌더링을 할 수 있는 GPU를 가져오라고 말해야 합니다.

We need to enable some features. First some vulkan 1.3 features, those are dynamic rendering, and syncronization 2. Those are optional features provided in vulkan 1.3 that change a few things. dynamic rendering allows us to completely skip renderpasses/framebuffers (if you want to learn about them, they are explained in the old version of vkguide), and also use a new upgraded version of the syncronization functions. 
We are also going to use the vulkan 1.2 features `bufferDeviceAddress` and `descriptorIndexing`. Buffer device adress will let us use GPU pointers without binding buffers, and descriptorIndexing gives us bindless textures.

By giving the `vkb::PhysicalDeviceSelector` the `VkPhysicalDeviceVulkan13Features` structure , we can tell vkbootstrap to find a gpu that has those features. 
`vkb::PhysicalDeviceSelector`에게 `VkPhysicalDeviceVulkan13Features` 구조체를 전달함으로서 vkbootstrap이 해당 기능을 갖는 GPU를 찾도록 지시합니다.


There are multiple levels of feature structs you can use depending on your vulkan version, you can find their info here: 
Vulkan 버전에 따라 다양한 수준의 기능 구조체들이 있습니다. 해당 정보는 아래 링크에서 확인할 수 있습니다.

[Vulkan Spec: 1.0 physical device features](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap47.html#VkPhysicalDeviceFeatures)
[Vulkan Spec: 1.1 physical device features](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap47.html#VkPhysicalDeviceVulkan11Features)
[Vulkan Spec: 1.2 physical device features](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap47.html#VkPhysicalDeviceVulkan12Features)
[Vulkan Spec: 1.3 physical device features](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap47.html#VkPhysicalDeviceVulkan13Features)


Once we have a `VkPhysicalDevice`, we can directly build a VkDevice from it.
`VkPhysicalDevice`를 구하고 나면, 이로부터 VkDevice를 직접 생성할 수 있습니다.

And at the end, we store the handles in the class.
마지막에는 클래스에 핸들을 저장합니다.

That's it, we have initialized Vulkan. We can now start calling Vulkan commands.
이것으로 Vulkan 초기화가 끝났습니다. 이제 Vulkan 명령 호출을 시작하겠습니다.

If you run the project right now, it will crash if you dont have a gpu with the required features or vulkan drivers that dont support them. If that happens, make sure your drivers are updated. If its just something your GPU doesnt support, go with the older version of vkguide as you wont be able to follow this tutorial.
만약 요구하는 기능을 충족하는 GPU가 없거나 Vulkan 드라이버가 이를 지원하지 않는다면 프로젝트를 실행했을 때 크래시가 발생할 것입니다. 이것이 발생한다면 드라이버가 업데이트 되었는지 확인해보세요. 만약 GPU가 지원하지 않는다면 구버전의 vkguide를 따르는 것이 도움이 될 수 있습니다.

## 교환사슬 설정하기

Last thing from the core initialization is to initialize the swapchain, so we can have something to render into.

Vulkan Engine에 새로운 멤버와 함수를 추가합니다.

```cpp
class VulkanEngine {
public:
	// --- 중략 ---

	VkSwapchainKHR _swapchain;
	VkFormat _swapchainImageFormat;

	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;
	VkExtent2D _swapchainExtent;

private: 
	void create_swapchain(uint32_t width, uint32_t height);
	void destroy_swapchain();
}
```

We are storing the `VkSwapchainKHR` itself, alongside the format that the swapchain images use when rendering to them.

We also store 2 arrays, one of Images, and another of ImageViews.

A `VkImage` is a handle to the actual image object to use as texture or to render into. A `VkImageView` is a wrapper for that image. It allows to do things like swap the colors. We will go into detail about it later.

We are also adding create and destroy functions for the swapchain. 

Like with the other initialization functions, we are going to use the vkb library to create a swapchain. It uses a builder similar to the ones we used for instance and device.

<!-- codegen from tag init_swap on file E:\ProgrammingProjects\vulkan-guide-2\chapter-1/vk_engine.cpp --> 
```cpp
void VulkanEngine::create_swapchain(uint32_t width, uint32_t height)
{
	vkb::SwapchainBuilder swapchainBuilder{ _chosenGPU,_device,_surface };

	_swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

	vkb::Swapchain vkbSwapchain = swapchainBuilder
		//.use_default_format_selection()
		.set_desired_format(VkSurfaceFormatKHR{ .format = _swapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		//use vsync present mode
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(width, height)
		.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.build()
		.value();

	_swapchainExtent = vkbSwapchain.extent;
	//store swapchain and its related images / 스왑체인과 관련된 이미지를 저장합니다.
	_swapchain = vkbSwapchain.swapchain;
	_swapchainImages = vkbSwapchain.get_images().value();
	_swapchainImageViews = vkbSwapchain.get_image_views().value();
}

void VulkanEngine::init_swapchain()
{
	create_swapchain(_windowExtent.width, _windowExtent.height);
}
```

From create_swapchain, we make the swapchain structures, and then we call the function from `init_swapchain()`

The most important detail here is the present mode, which we have set to `VK_PRESENT_MODE_FIFO_KHR`. This way we are doing a hard VSync, which will limit the FPS of the entire engine to the speed of the monitor.

We also send the window sizes to the swapchain. This is important as creating a swapchain will also create the images for it, so the size is locked. Later in the tutorial we will need to rebuild the swapchain as the window resizes, so we have them separated from the init flow, but in the init flow we default that size to the window size.

Once the swapchain is built, we just store all of its stuff into the members of VulkanEngine class.

Lets write the `destroy_swapchain()` function too.

<!-- codegen from tag destroy_sc on file E:\ProgrammingProjects\vulkan-guide-2\chapter-1/vk_engine.cpp --> 
```cpp
void VulkanEngine::destroy_swapchain()
{
	vkDestroySwapchainKHR(_device, _swapchain, nullptr);

	// destroy swapchain resources
	for (int i = 0; i < _swapchainImageViews.size(); i++) {

		vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
	}
}
```

We first delete the swapchain object, which will delete the images it holds internally. We then have to destroy the ImageViews for those images.

## 자원 정리하기
We need to make sure that all of the Vulkan resources we create are correctly deleted, when the app exists.

For that, go to the `VulkanEngine::cleanup()` function

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

Objects have dependencies on each other, and we need to delete them in the correct order. Deleting them in the opposite order they were created is a good way of doing it. In some cases, if you know what you are doing, the order can be changed a bit and it will be fine.

`VkPhysicalDevice` can't be destroyed, as it's not a Vulkan resource per-se, it's more like just a handle to a GPU in the system.

Because our initialization order was SDL Window -> Instance -> Surface -> Device -> Swapchain, we are doing exactly the opposite order for destruction.

If you try to run the program now, it should do nothing, but that nothing also includes not emitting errors.

There is no need to destroy the Images in this specific case, because the images are owned and destroyed with the swapchain.

## 검증 레이어 에러
Just to check that our validation layers are working, let's try to call the destruction functions in the wrong order

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

We are now destroying the Instance before the Device and the Surface (which was created from the Instance) is also deleted. The validation layers should complain with an error like this.

```
[ERROR: Validation]
Validation Error: [ VUID-vkDestroyInstance-instance-00629 ] Object 0: handle = 0x24ff02340c0, type = VK_OBJECT_TYPE_INSTANCE; Object 1: handle = 0xf8ce070000000002, type = VK_OBJECT_TYPE_SURFACE_KHR; | MessageID = 0x8b3d8e18 | OBJ ERROR : For VkInstance 0x24ff02340c0[], VkSurfaceKHR 0xf8ce070000000002[] has not been destroyed. The Vulkan spec states: All child objects created using instance must have been destroyed prior to destroying instance (https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#VUID-vkDestroyInstance-instance-00629)
```

With the Vulkan initialization completed and the layers working, we can begin to prepare the command structures so that we can make the gpu do something.

Next: [ Executing Vulkan Commands]({{ site.baseurl }}{% link docs/new_chapter_1/vulkan_command_flow.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}
