---
layout: default
lang: ko
title: Improving the render loop
parent:  "2. 컴퓨트 셰이더 그리기"
nav_order: 1
---

그리기를 시작하기 전에, 몇 가지 추가로 구현해야 할 작업이 있습니다. 먼저, 점점 늘어나는 객체들을 안전하게 정리할 수 있도록 삭제 큐를 만들고, 렌더링 루프를 수정하여 교환사슬 이미지가 아닌 곳에 먼저 그린 후 해당 이미지를 교환사슬로 복사하도록 하겠습니다.


## 삭제 큐
점점 더 많은 Vulkan 구조체를 추가하게 되면서, 파괴를 효율적으로 관리할 수단이 필요합니다. `cleanup()`함수에 항목을 추가할 수도 있지만, 규모가 커질수록 유지보수가 어렵고, 동기화 오류가 발생하기 쉬워집니다. 그래서 VulkanEngine에 삭제 큐라는 새로운 구조체를 추가하겠습니다. 이는 많은 엔진에서 사용하는 일반적인 방식으로, 삭제할 객체들을 큐에 담아 두었다가, 큐를 실행함으로써 객체들을 올바른 순서로 삭제하는 구조입니다. 이 구현에서는 단순함을 우선시하여 std::function 콜백을 덱(deque)에 저장할 것입니다. 덱을 FILO(First In Last Out)방식으로 동작하게 되며, 마지막에 추가된 객체부터 먼저 파괴됩니다.

해당 구현은 다음과 같습니다.

```cpp
struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& function) {
		deletors.push_back(function);
	}

	void flush() {
		// 삭제 큐를 역순으로 순회하며 모든 함수를 실행합니다.
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
			(*it)(); //함수 객체를 호출합니다.
		}

		deletors.clear();
	}
};
```

std::function stores a lambda, and we can use it to store a callback with some data, which is perfect for this case. 
std::function은 람다를 담아 콜백과 데이터를 함께 담을 수 있으며, 이러한 용도에 적합합니다.

하지만 각 객체마다 std::function을 저장하는 방식은 규모가 커질 경우 비효율적입니다. 이 튜토리얼에서 사용할 객체 수에는 큰 문제가 되지 않지만, 수천 개의 객체를 더 빠르게 파괴하고 싶다면 VkImage, VkBuffer과 같은 다양한 타입의 Vulkan 핸들을 저장해 반복문에서 파괴하는 편이 낫습니다.

다양한 생명 주기를 가진 객체를 관리하기 위해 삭제 큐를 여러 위치에 둘 것입니다. 그중 하나는 VulkanEngine 클래스에서 엔진이 종료 시 전역 객체들을 정리하는 역할을 합니다. 또한 각 프레임마다 별도의 삭제 큐를 두어, 사용이 끝난 객체를 다음 프레임에 안전하게 삭제할 수 있도록 하겠습니다. 이 삭제 큐를 VulkanEngine 클래스와 그 내부에 있는 FrameData 구조체에 추가합니다.

이를 VulkanEngine 클래스의 FrameData 구조체 내부에 추가합니다.

```cpp
struct FrameData {
	 //중략
      DeletionQueue _deletionQueue;
};

class VulkanEngine{
    //중략
    DeletionQueue _mainDeletionQueue;
}

```

삭제 큐를 두 위치에서 호출합니다. 하나는 프레임마다 펜스를 대기한 직후, 다른 하나는 WaitIdle 호출 이후 cleanup()함수에서입니다. 펜스 바로 뒤에서 삭제 큐를 비움으로써, GPU가 해당 프레임의 작업을 모두 완료한 후에만 그 프레임에 생성된 객체들을 안전하게 삭제할 수 있도록 합니다. 또한 프레임의 나머지 데이터를 파괴할 때 이러한 프레임별 자원도 함께 해제되도록 보장합니다.

```cpp
void VulkanEngine::draw()
{
	//지난 프레임의 렌더링을 GPU가 완료할 때 까지 대기합니다. 제한 시간은 1초입니다.
	VK_CHECK(vkWaitForFences(_device, 1, &get_current_frame()._renderFence, true, 1000000000));

	get_current_frame()._deletionQueue.flush();

    //중략
}

void VulkanEngine::cleanup()
{	
	if (_isInitialized) {
		
		//make sure the gpu has stopped doing its things / GPU가 작업을 완료할 때 까지 대기합니다.				
		vkDeviceWaitIdle(_device);
		
		//free per-frame structures and deletion queue / 프레임별 자원과 삭제큐를 해제합니다.
		for (int i = 0; i < FRAME_OVERLAP; i++) {

			vkDestroyCommandPool(_device, _frames[i]._commandPool, nullptr);

			//destroy sync objects
			vkDestroyFence(_device, _frames[i]._renderFence, nullptr);
			vkDestroySemaphore(_device, _frames[i]._renderSemaphore, nullptr);
			vkDestroySemaphore(_device, _frames[i]._swapchainSemaphore, nullptr);

			_frames[i]._deletionQueue.flush();
		}

		//flush the global deletion queue / 전역 삭제 큐를 해제합니다.
		_mainDeletionQueue.flush();

		//후략
	}
}
```

With the deletion queue set, now whenever we create new vulkan objects we can just add them into the queue.
삭제 큐를 설정했으므로 Vulkan 객체를 언제든지 큐에 넣을 수 있습니다.

## 메모리 할당

To improve the render loop, we will need to allocate a image, and this gets us into how to allocate objects in vulkan. We are going to skip that entire chapter, because we will be using Vulkan Memory Allocator library. Dealing with the different memory heaps and object restrictions such as image alignment is very error prone and really hard to get right, specially if you want to get it right at a decent performance. By using VMA, we skip all that, and we get a battle tested way that is guaranteed to work well. There are cases like the PCSX3 emulator project, where they replaced their attempt at allocation to VMA, and won 20% extra framerate. 
렌더링 루프를 개선하기 위해 Vulkan에서 객체를 어떻게 할당하는 지 살펴보고 이미지를 할당할 필요가 있습니다. Vulkan Memory Allocator 라이브러리를 사용하고 있으므로 전반적인 과정은 생략될 것입니다. 이미지 할당과 같은 엄격한 객체와 다양한 메모리 힙을 다루는 것은 오류가 발생하기 쉬우며, 월등한 성능을 원한다면 특히 다루기 어렵습니다. VMA를 사용함으로써 이러한 작업들을 생략할 수 있으며, 잘 작동하도록 보장받는 수단을 얻었습니다. VMA를 사용하는 것만으로 20%의 추가 프레임을 얻은 PCSX3 에뮬레이터 프로젝트와 같은 경우도 있습니다.

vk_types.h already holds the include needed for the VMA library, but we need to do something else too.
vk_types.h는 VMA 라이브러리를 포함하고 있지만 몇 가지 추가할 것이 있습니다.

On vk_engine.cpp we include it too, but with `VMA_IMPLEMENTATION` defined.
vk_engine.cpp에도 포함되어 있지만 `VMA_IMPLEMENTATION`을 정의합니다.

```cpp
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
```

VMA holds both the normal header, and the implementation of the functions into the same header file. We need to define `VMA_IMPLEMENTATION` exactly into only one of the .cpp files of our project, and that will store and compile the definitions for the VMA functions. 
VMA는 일반적인 헤더와 같은 헤더 파일에 구현이 되어있는 경우 2가지 헤더가 있습니다. `VMA_IMPLEMENTATION`을 정의함으로서 프로젝트에서 VMA함수의 정의를 컴파일해 가져올 수 있습니다.

Add the allocator to the VulkanEngine class
할당기를 VulkanEngine 클래스에 추가하겠습니다.
```cpp
class VulkanEngine{

    VmaAllocator _allocator;
}
```

Now we will initialize it from `init_vulkan()` call, at the end of the function.

<!-- codegen from tag vma_init on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
    // initialize the memory allocator / 메모리 할당기를 초기화합니다.
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = _chosenGPU;
    allocatorInfo.device = _device;
    allocatorInfo.instance = _instance;
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    vmaCreateAllocator(&allocatorInfo, &_allocator);

    _mainDeletionQueue.push_function([&]() {
        vmaDestroyAllocator(_allocator);
    });
```

There isnt much to explain it, we are initializing the _allocator member, and then adding its destruction function into the destruction queue so that it gets cleared when the engine exits. We hook the physical device, instance, and device to the creation function. We give the flag `VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT` that will let us use GPU pointers later when we need them.
Vulkan Memory Allocator library follows similar call conventions as the vulkan api, so it works with similar info structs.
설명할 것이 많지는 않습니다. _allocator 멤버를 초기화 하고, 파괴 함수를 큐에 추가해 엔진이 종료될 때 정리되도록 했습니다. physical device, instance, device도 생성 함수에 연결하였습니다. `VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT`를 전달해 이후 GPU 포인터가 필요할 때 사용할 수 있게 했습니다.

# 새로운 렌더링 루프

Drawing directly into the swapchain is fine for many projects, and it can even be optimal in some cases such as phones. But it comes with a few restrictions. 
The most important of them is that the formats of the image used in the swapchain are not guaranteed. Different OS, drivers, and windowing modes can have different optimal swapchain formats. Things like HDR support also need their own very specific formats.
Another one is that we only get a swapchain image index from the windowing present system. There are low-latency techniques where we could be rendering into another image, and then directly push that image to the swapchain with very low latency. 
교환사슬에 직접 그리는 것은 많은 프로젝트에서 수행하고 있으며, 스마트폰과 같은 환경에서는 최적일 수 있습니다. 하지만 몇 가지 제약이 있습니다. 가장 중요한 점은 교환사슬에 사용되는 이미지의 포맷이 보장되지 않는다는 점입니다. 다양한 OS, 들아ㅣ버, 창 모드가 다양한 포맷을 갖습니다. HDR을 지원하는 경우 다양한 특수 포맷이  필요합니다. 또 다른 하나는 교환사슬 이미지 인덱스를 창 표시 시스템으로부터 가져오는 것입니다. 이는 또 다른 이미지에 렌더링 할 수 있는 저지연 테크닉이며, 그린 이미지를 교환사슬로 매우 적은 지연으로 직접 전달할 수 있습니다.

One very important limitation is that their resolution is fixed to whatever your window size is. If you want to have higher or lower resolution, and then do some scaling logic, you need to draw into a different image.
한 가지 중요한 제한은 해상도가 창 크기가 무엇이든 고정되어 있다는 점입니다. 더 높은, 혹은 더 적은 해상도를 원한다면 확대 축소 로직을 작성해야 하며, 다른 이미지에 작성해야 합니다.

And last, swapchain formats are, for the most part, low precision. Some platforms with High Dynamic Range rendering have higher precision formats, but you will often default to 8 bits per color. So if you want high precision light calculations, system that would prevent banding, or to be able to go past 1.0 on the normalized color range, you will need a separate image for drawing.
ㅇ

For all those reasons, we will do the whole tutorial rendering into a separate image than the one from the swapchain. After we are doing with the drawing, we will then copy that image into the swapchain image and present it to the screen.

The image we will be using is going to be in the  RGBA 16-bit float format. This is slightly overkill, but will provide us with a lot of extra precision that will come in handy when doing lighting calculations and better rendering.


# Vulkan 이미지
We have already dealt superficially with images when setting up the swapchain, but it was handled by VkBootstrap. This time we will create the images ourselves.
ㅇ.


Lets begin by adding the new members we will need to the VulkanEngine class.
VulkanEngine 클래스에 필요한 새로운 멤버를 선언할 필요가 있습니다.

On vk_types.h, add this structure which holds the data needed for an image. We will hold a `VkImage` alongside its default `VkImageView`, then the allocation for the image memory, and last, the image size and its format, which will be useful when dealing with the image. We also add a `_drawExtent` that we can use to decide what size to render.
vk_types.h에 이미지에 필요한 데이터를 담는 구조체를 추가합니다. `VkImage`를 기본 `VkImageView`, 할당한 이미지 메모리, 이미지 크기, 그리고 포맷과 함께 담습니다. 이를 통해 이미지를 다룰 것입니다. 또한 `_drawExtent`를 추가하여 렌더링할 크기를 결정하는 데 사용하겠습니다.

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

Lets check the vk_initializers function for image and imageview create info.
vk_intializers 함수의 이미지와 이미지뷰의 createInfo 함수를 추가합니다.

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

    //for MSAA. we will not be using it by default, so default it to 1 sample per pixel. / MSAA에 사용됩니다. 이를 사용하지 않을 것이므로 픽셀당 1샘플로 둡니다.
    info.samples = VK_SAMPLE_COUNT_1_BIT;

    //optimal tiling, which means the image is stored on the best gpu format / 최적화된 타일링으로, 이는 최적화된 GPU 포맷으로 이미지를 담을 것임을 의미합니다.
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usageFlags;

    return info;
}

VkImageViewCreateInfo vkinit::imageview_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags)
{
    // build a image-view for the depth image to use for rendering / 렌더링에 사용할 깊이 이미지의 이미지 뷰를 구성합니다.
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

We will hardcode the image tiling to OPTIMAL, which means that we allow the gpu to shuffle the data however it sees fit. If we want to read the image data from cpu, we would need to use tiling LINEAR, which makes the gpu data into a simple 2d array. This tiling highly limits what the gpu can do, so the only real use case for LINEAR is CPU readback.
이미지 타일링을 VK_IMAGE_TILING_OPTIMAL로 하드코딩할 것입니다. 이는 GPU에게 데이터를 알아서 섞도록 허용함을 의미합니다. CPU에게 이미지 데이터를 읽게 하고 싶을 때, VK_IMAGE_TILING_LINEAR 타일링을 사용할 필요가 있습니다. 이는 GPU 데이터를 단순한 2차원 배열로 만들어줍니다. 이 타일링은 GPU가 할 수 있는 것을 꽤 제한하므로, LINEAR를 사용하는 실제 경우는 CPU에서 읽는 경우밖에 없습니다.

On the imageview creation, we need to setup the subresource. Thats similar to the one we used in the pipeline barrier.
이미지 뷰가 만들어지면, subresource를 작성해야합니다. 파이프라인 배리어에서 작성했던 것과 유사합니다.

Now, at the end of init_swapchain, lets create it.
이제 init_swapchain의 끝에 이를 추가합니다.

<!-- codegen from tag init_swap on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
	//draw image size will match the window / 그릴 이미지의 크기는 창과 일치해야 합니다.
	VkExtent3D drawImageExtent = {
		_windowExtent.width,
		_windowExtent.height,
		1
	};

	//hardcoding the draw format to 32 bit float / 그리기 포맷을 32비트 float로 하드코딩합니다.
	_drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
	_drawImage.imageExtent = drawImageExtent;

	VkImageUsageFlags drawImageUsages{};
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	VkImageCreateInfo rimg_info = vkinit::image_create_info(_drawImage.imageFormat, drawImageUsages, drawImageExtent);

	//for the draw image, we want to allocate it from gpu local memory / 이미지를 그리기 위해 GPU 전용 메모리에 할당합니다.
	VmaAllocationCreateInfo rimg_allocinfo = {};
	rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	//allocate and create the image / 이미지를 할당하고 생성합니다.
	vmaCreateImage(_allocator, &rimg_info, &rimg_allocinfo, &_drawImage.image, &_drawImage.allocation, nullptr);

	//build a image-view for the draw image to use for rendering / 렌더링에 사용할 이미지를 위한 이미지 뷰를 구성합니다.
	VkImageViewCreateInfo rview_info = vkinit::imageview_create_info(_drawImage.imageFormat, _drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

	VK_CHECK(vkCreateImageView(_device, &rview_info, nullptr, &_drawImage.imageView));

	//add to deletion queues / 삭제큐에 추가합니다.
	_mainDeletionQueue.push_function([=]() {
		vkDestroyImageView(_device, _drawImage.imageView, nullptr);
		vmaDestroyImage(_allocator, _drawImage.image, _drawImage.allocation);
	});
```

We begin by creating a VkExtent3d structure with the size of the image we want, which will match our window size. We copy it into the AllocatedImage
VkExtent3d 구조체를 우리가 원하는 이미지의 크기로 생성하믕로서, 창의 크기와 일치시킬 것입니다. 이를 할당된 이미지로 복사합니다.

Then, we need to fill our usage flags. In vulkan, all images and buffers must fill a UsageFlags with what they will be used for. This allows the driver to perform optimizations in the background depending on what that buffer or image is going to do later. In our case, we want TransferSRC and TransferDST so that we can copy from and into the image,  Storage because thats the "compute shader can write to it" layout, and Color Attachment so that we can use graphics pipelines to draw geometry into it.
그리고 usage 플래그를 채워야 합니다. Vulkan에서는 모든 버퍼와 이미지가 어디에 사용될 것인지를 나타내는 Usage 플래그를 채워야만 합니다. 이는 드라이버에게 이미지와 버퍼가 추후 어디에 사용될 것인지에 따라 백그라운드에서 최적화를 수행할 수 있도록 해줍니다. 우리의 경우 TransferSRC와 TransferDST를 사용해 이미지로부터 복사를 수행할 수 있습니다. STORAGE는 컴퓨트 셰이더가 작성할 수 있는 레이아웃을 위해 추가하고, COLOR_ATTACHMENT는 그래픽스 파이프라인에서 도형을 그릴 수 있게 합니다.

The format is going to be `VK_FORMAT_R16G16B16A16_SFLOAT`. This is 16 bit floats for all 4 channels, and will use 64 bits per pixel. Thats a fair amount of data, 2x what a 8 bit color image uses, but its going to be useful.
포맷은 `VK_FORMAT_R16G16B16A16_SFLOAT`를 사용하겠습니다. 이는 4채널의 16비트 float이며, 픽셀당 64비트를 사용할 것입니다. 이는 8비트 이미지를 사용하는 것보다 2배나 많은 양의 데이터를 사용하지만, 유용할 것입니다.

When creating the image itself, we need to send the image info and an alloc info to VMA. VMA will do the vulkan create calls for us and directly give us the vulkan image. 
The interesting thing in here is Usage and the required memory flags.
With VMA_MEMORY_USAGE_GPU_ONLY usage, we are letting VMA know that this is a gpu texture that wont ever be accessed from CPU, which lets it put it into gpu VRAM. To make extra sure of that, we are also setting `VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT` as a memory flag. This is a flag that only gpu-side VRAM has, and guarantees the fastest access.
이미지를 생성할 때, imageInfo 구조체와 allocInfo 구조체를 VMA로 전달해야 합니다. VMA는 Vulkan 생성 호출을 수행해 Vulkan 이미지를 가져올 것입니다. 여기서 흥미로운 점은 Usage와 RequiredMemory 플래그입니다. VMA_MEMORY_USAGE_GPU_ONLY 플래그를 통해 VMA에게 해당 이미지가 CPU에서 접근되지 않는 GPU 텍스쳐 임을 알려주어 GPU VRAM에 넣을 수 있게 합니다. 이를 분명히 하기 위해 `VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT`를 메모리 플래그로 설정했습니다. 이 플래그는 GPU VRAM만을 위한 플래그로, 가장 빠른 접근을 보장합니다.

In vulkan, there are multiple memory regions we can allocate images and buffers from. PC implementations with dedicated GPUs will generally have a cpu ram region, a GPU Vram region, and a "upload heap" which is a special region of gpu vram that allows cpu writes. If you have resizable bar enabled, the upload heap can be the entire gpu vram. Else it will be much smaller, generally only 256 megabytes. We tell VMA to put it on GPU_ONLY which will prioritize it to be on the gpu vram but outside of that upload heap region.
Vulkan에서는 이미지와 버퍼를 할당할 수 있는 여러 메모리 영역이 있습니다. 전용 GPU를 갖는 PC 구현에서는 일반적으로 CPU RAM 영역, GPU VRAM 영역, 그리고 CPU가 작성할 수 있는 특수한 GPU VRAM 영역인 "업로드 힙" 이 있습니다. 크기를 변경할 수 있는 바가 활성화되어 있다면, 업로드 힙이 전체 GPU VRAM이 될 수 있습니다. 더 작을 수도 있지만 일반적으로 256MB입니다. VMA에게 이미지를 업로드 힙 영역 외부에 있는 GPU VRAM에 GPU_ONLY에 할당하라고 말합니다.

With the image allocated, we create an imageview to pair with it. In vulkan, you need a imageview to access images. This is generally a thin wrapper over the image itself that lets you do things like limit access to only 1 mipmap. We will always be pairing vkimages with their "default" imageview in this tutorial.
이미지를 할당했으므로 이미지 뷰도 함께 생성해야 합니다. Vulkan에서는 이미지에 접근하기 위해 이미지 뷰가 필요합니다. 이는 일반적으로 이미지에 대한 얇은 제한된 래퍼입니다. 예를 들어 밉맵 하나에만 접근하도록 할 수도 있습니다. 이 튜토리얼에서는 이미지를 기본 이미지뷰와 함께 묶을 것입니다.

# 새로운 렌더링 루프

Now that we have a new draw image, lets add it into the render loop.
이제 이미지를 그릴 수 있는 공간이 마련되었습니다. 렌더링 루프를 추가하겠습니다.

We will need a way to copy images, so add this into vk_images.cpp
먼저 이미지를 복사할 필요가 있습니다. 이를 vk_images.cpp에 추가합니다.

<!-- codegen from tag copyimg on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_images.cpp --> 
```cpp
void vkutil::copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize)
{
	VkImageBlit2 blitRegion{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

	blitRegion.srcOffsets[1].x = srcSize.width;
	blitRegion.srcOffsets[1].y = srcSize.height;
	blitRegion.srcOffsets[1].z = 1;

	blitRegion.dstOffsets[1].x = dstSize.width;
	blitRegion.dstOffsets[1].y = dstSize.height;
	blitRegion.dstOffsets[1].z = 1;

	blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.srcSubresource.baseArrayLayer = 0;
	blitRegion.srcSubresource.layerCount = 1;
	blitRegion.srcSubresource.mipLevel = 0;

	blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.dstSubresource.baseArrayLayer = 0;
	blitRegion.dstSubresource.layerCount = 1;
	blitRegion.dstSubresource.mipLevel = 0;

	VkBlitImageInfo2 blitInfo{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr };
	blitInfo.dstImage = destination;
	blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	blitInfo.srcImage = source;
	blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	blitInfo.filter = VK_FILTER_LINEAR;
	blitInfo.regionCount = 1;
	blitInfo.pRegions = &blitRegion;

	vkCmdBlitImage2(cmd, &blitInfo);
}
```
 
Also add the corresponding declaration to `vk_images.h`
대응되는 선언을 `vk_images.h`에 추가합니다.
```cpp
	namespace vkutil {

	    // Existing:
	    void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);

		void copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize);	
	}
```

Vulkan has 2 main ways of copying one image to another. you can use VkCmdCopyImage or VkCmdBlitImage.
CopyImage is faster, but its much more restricted, for example the resolution on both images must match.
Meanwhile, blit image lets you copy images of different formats and different sizes into one another. You have a source rectangle and a target rectangle, and the system copies it into its position. Those two functions are useful when setting up the engine, but later its best to ignore them and write your own version that can do extra logic on a fullscreen fragment shader.
Vulkan은 이미지를 복사하는 2가지 방법을 제시합니다. VkCmdCopyImage와 VkCmdBlitImage입니다. CopyImage는 빠르지만 제약이 있습니다. 예를 들어 두 이미지의 해상도가 같아야 합니다. 반면 blitImage는 다양한 포맷과 다양한 사이즈 간 복사를 수행할 수 있습니다. 소스 사각형과 타겟 사각형이 있을 때, 시스템은 대응되는 위치에 복사를 수행합니다. 이 두 함수는 엔진을 설정할 때 유용하지만, 이후에 이를 무시하고 전체 프래그먼트 셰이더에 사용할 추가 로직을 작성하는 편이 좋습니다.

With it, we can now update the render loop. As draw() is getting too big, we are going to leave the syncronization, command buffer management, and transitions in the draw() function, but we are going to add the draw commands themselves into a draw_background() function.
이를 통해 렌더링 루프를 업데이트할 수 있습니다. draw()가 점점 더 커지고 있으므로 동기화, 커맨드 버퍼 관리, 이미지 변환을 draw()함수에 남기고 그리기 명령 그 자체를 draw_background()함수에 작성하겠습니다.

```cpp
void VulkanEngine::draw_background(VkCommandBuffer cmd)
{
	//프레임 수로 색상을 초기화합니다. 120프레임 주기로 깜빡일 것입니다.
	VkClearColorValue clearValue;
	float flash = std::abs(std::sin(_frameNumber / 120.f));
	clearValue = { { 0.0f, 0.0f, flash, 1.0f } };

	VkImageSubresourceRange clearRange = vkinit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);

	//이미지를 초기화합니다.
	vkCmdClearColorImage(cmd, _drawImage.image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
}
```

Add the function to the header too.
헤더에 함수를 추가합니다.

We will be changing the code that records the command buffer. You can now delete the older one. 
The new code is this.
커맨드 버퍼에 기록하는 코드를 바꿀 것입니다. 지금은 구버전을 지우고 새로운 코드를 작성합니다.
```cpp
	_drawExtent.width = _drawImage.imageExtent.width;
	_drawExtent.height = _drawImage.imageExtent.height;

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));	

	// transition our main draw image into general layout so we can write into it / 이미지의 레이아웃을 쓸 수 있는 레이아웃으로 변경합니다.
	// we will overwrite it all so we dont care about what was the older layout / 모두 덮어씌워 이전 레이아웃이 무엇인지 신경쓰지 않습니다.
	vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	draw_background(cmd);

	//transition the draw image and the swapchain image into their correct transfer layouts / 그릴 이미지와 교환사슬 이미지를 전송 최적화 레이아웃으로 변환합니다.
	vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// execute a copy from the draw image into the swapchain / 그린 이미지를 교환사슬로 복사합니다.
	vkutil::copy_image_to_image(cmd, _drawImage.image, _swapchainImages[swapchainImageIndex], _drawExtent, _swapchainExtent);

	// set swapchain image layout to Present so we can show it on the screen / 교환사슬 이미지 레이아웃을 표시할 수 있는 레이아웃으로 설정해 화면에 표시합니다.
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	//finalize the command buffer (we can no longer add commands, but it can now be executed) / 커맨드 버퍼를 끝냅니다.(명령을 더이상 추가할 수 없으며 이제 실행할 수 있습니다.)
	VK_CHECK(vkEndCommandBuffer(cmd));
```

The main difference we have in the render loop is that we no longer do the clear on the swapchain image. Instead, we do it on the `_drawImage.image`. Once we have cleared the image, we transition both the swapchain and the draw image into their layouts for transfer, and we execute the copy command. Once we are done with the copy command, we transition the swapchain image into present layout for display. As we are always drawing on the same image, our draw_image does not need to access swapchain index, it just clears the draw image. We are also writing the _drawExtent that we will use for our draw region.
렌더링 루프의 주요 차이점은 더이상 교환사슬 그 자체에 초기화를 수행하지 않는다는 점입니다. 대신 `_drawImage.image`에 초기화를 수행합니다. 이미지가 색상이 그려지면 스왑체인과 그려진 이미지를 전송 전용 레이아웃으로 변환하고 복사합니다. 복사가 수행되면 교환사슬 이미지를 표시 레이아웃으로 전환합니다. 계속 같은 이미지에 그리므로 draw_image가 더이상 교환사슬 인덱스에 접근할 필요가 없습니다. draw image를 초기화할 뿐입니다. 또한 _drawExtent를 작성하여 그리기 영역을 사용할 것입니다.

This will now provide us a way to render images outside of the swapchain itself. We now get significantly higher pixel precision, and we unlock some other techniques.
교환사슬 외부에서 이미지를 렌더링하는 방법을 제공할 것입니다. 상당히 높은 픽셀 정밀도를 사용하며 다른 기법을 사용하겠습니다.

With that done, we can now move into the actual compute shader execution steps.
이를 통해 실제 컴퓨트 셰이더 실행 단계로 넘어갈 수 있습니다.


다음 글 : [ Vulkan 셰이더]({{ site.baseurl }}{% link docs/new_chapter_2/vulkan_shader_drawing.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}
