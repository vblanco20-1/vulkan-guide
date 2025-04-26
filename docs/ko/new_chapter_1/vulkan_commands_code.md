---
layout: default
title: Setting up Vulkan commands
parent:  "1. Initializing Vulkan - KO"
nav_order: 21
---

`FrameData` 구조체를 `vk_engine.h`에 작성하겠습니다. 이 구조체는 주어진 프레임을 렌더링하는 데 필요한 구조체와 명령을 저장하게 됩니다. 더블 버퍼링을 사용할 것이므로, GPU가 한쪽 버퍼에서 명령을 실행하는 동안 다른 버퍼에서 명령을 기록할 수 있도록 설계할 것입니다.
```cpp
struct FrameData {

	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;
};

constexpr unsigned int FRAME_OVERLAP = 2;
```
이를 큐를 저장할 멤버 변수와 함께 `VulkanEngine` 클래스에 추가해줍니다.

```cpp
class VulkanEngine{
public:
	FrameData _frames[FRAME_OVERLAP];

	FrameData& get_current_frame() { return _frames[_frameNumber % FRAME_OVERLAP]; };

	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;
}
```

초기화 로직 외부에서 `_frames` 배열에 직접적으로 접근하지는 않을 것입니다. 따라서 `_frameNumber`멤버를 사용하여 해당 배열을 참조하는 getter를 추가하겠습니다. 이를 통해 두 개의 구조체를 번갈아 가며 사용할 수 있습니다.


## 큐 가져오기

이제 유효한 큐 패밀리를 찾아 그로부터 큐를 생성해야 합니다. 모든 종류의 명령을 실행할 수 있는 큐를 생성하여 엔진 전반에서 활용할 것입니다.

다행히 VkBootstrap 라이브러리를 사용하면 큐와 큐 패밀리를 직접 가져올 수 있습니다.

`init_vulkan()`함수의 마지막 부분으로 가봅시다.

맨 마지막에 다음 코드를 추가합니다.


```cpp
void VulkanEngine::init_vulkan(){

// ---- other code, initializing vulkan device ----

	// use vkbootstrap to get a Graphics queue
	_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
}
```

vkbootstrap을 통해 그래픽스 큐와 큐 패밀리를 요청합니다.

## 커맨드 구조체 생성하기

커맨드 풀을 생성하기 위해 `init_commands()` 함수 내부에 이전과는 다른 코드를 추가합니다. 이번에는 VkBootstrap 라이브러리를 사용하지 않을 것이며, 직접 Vulkan 명령을 호출하겠습니다.

```cpp
void VulkanEngine::init_commands()
{
	//create a command pool for commands submitted to the graphics queue.
	//we also want the pool to allow for resetting of individual command buffers
	VkCommandPoolCreateInfo commandPoolInfo =  {};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.pNext = nullptr;
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolInfo.queueFamilyIndex = _graphicsQueueFamily;
	
	for (int i = 0; i < FRAME_OVERLAP; i++) {

		VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_frames[i]._commandPool));

		// allocate the default command buffer that we will use for rendering
		VkCommandBufferAllocateInfo cmdAllocInfo = {};
		cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdAllocInfo.pNext = nullptr;
		cmdAllocInfo.commandPool = _frames[i]._commandPool;
		cmdAllocInfo.commandBufferCount = 1;
		cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_frames[i]._mainCommandBuffer));
	}
}
```
`vkCreateX` 함수에 사용되는 대부분의 Vulkan Info 구조체들과 많은 다양한 Vulkan 구조체들은 `sType`과 `pNext`를 요구합니다. 이는 확장을 지원하기 위한 것으로, 일부 확장은 동일한 `vkCreateX` 함수를 호출하지만, 다른 종류의 구조체를 요구하는 경우가 있습니다. `sType`은 함수에서 사용되는 구조체의 종류를 `Vulkan` 구현체가 인식할 수 있도록 합니다.

Vulkan 구조체를 사용할 때 이를 반드시 수행하는 것이 중요합니다.
```cpp
VkCommandPoolCreateInfo commandPoolInfo = {};
```

` = {}`를 작성하면 컴파일러가 구조체의 모든 값을 0으로 초기화합니다. 대부분의 Vulkan 구조체는 0으로 설정되었을 때 비교적 안전하게 동작하기 때문에 이는 매우 중요합니다. 이를 통해 구조체 내에 초기화되지 않은 값이 남지 않도록 보장할 수 있습니다.

`queueFamilyIndex`에 이전에 가져온 `_graphicsQueueFamily`를 설정합니다. 이는 커맨드 풀이 "그래픽스" 패밀리의 큐에 사용할 수 있는 커맨드 버퍼를 생성하도록 합니다.

또한 `.flags` 인자도 설정해주어야 합니다. 많은 Vulkan 구조체들은 추가 옵션을 설정하기 위해 `.flags` 인지를 갖습니다. 여기서는 `VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT`을 전달하고 있는데, 이는 Vulkan에게 해당 풀에서 생성된 개별 커맨드 버퍼를 독립적으로 리셋할 수 있도록 허용하는 설정입니다. 대안으로는 전체 커맨드 풀을 한번에 리셋하는 방법도 있습니다. 이 경우 풀에 속한 모든 커맨드 버퍼가 함께 초기화되며, 이러한 방식에서는 해당 플래그를 설정할 필요가 없습니다.

마지막으로 `VkCreateCommandPool`를 호출할 때, `VkDevice`, 생성 인자로 사용할 `commandPoolInfo`, 그리고 `_commandPool`멤버의 포인터를 전달합니다. 생성이 성공하면 `_commandPool` 멤버가 새로운 핸들로 덮어씌워집니다.

명령이 성공했는지 확인하기 위해 `VK_CHECK()` 매크로를 사용합니다. 이는 예외가 발생하면 즉시 프로그램을 중단하여 문제를 감지할 수 있도록 합니다

이제 `VkCommandPool`이 생성되어 `_commandPool`멤버에 저장되었으므로, 이를 통해 커맨드 버퍼를 할당할 수 있습니다.

커맨드 풀과 마찬가지로, `sType`과 `pNext`를 설정하고 나머지 Info 구조체의 필드를 채워야 합니다.

Vulkan에게 우리가 방금 생성한 `_commandPool`을 커맨드 버퍼의 부모로 지정하며, 하나의 커맨드 버퍼만을 생성하겠다고 알려줍니다.

`.commandBufferCount` 인자를 사용하면 여러 커맨드 버퍼를 한 번에 할당할 수 있습니다. `VkAllocateCommandBuffer`로 전달하는 포인터가 해당 개수 만큼의 공간이 확보하고 있어야 합니다.

`.level`은 Primary로 설정합니다. 커맨드 버퍼의 레벨은 Primary 혹은 Secondary가 될 수 있습니다. Primary 레벨은 VkQueue로 전달되어 모든 작업을 수행합니다. 이 가이드에서는 Primary를 사용할 것입니다. Secondary 레벨은 Primary 버퍼의 "하위명령" 역할을 할 수 있습니다. 멀티쓰레드 단일 패스 환경에서 명령을 기록할 때 흔히 사용됩니다. 그러나 우리가 구축할 아키텍처에서는 멀티쓰레드로 처리할 필요가 없으므로 Secondary 커맨드 버퍼는 사용하지 않을 것입니다.

이러한 info 구조체에 사용되는 인자와 세부사항에 관한 정보는 아래 링크에서 확인할 수 있습니다.
* [VkCommandPoolCreateInfo](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap6.html#VkCommandPoolCreateInfo)
* [VkCommandBufferAllocateInfo](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap6.html#VkCommandBufferAllocateInfo)

## VkInit 모듈

프로젝트 파일을 다룬 글을 기억한다면, `vk_initializers` 모듈이 다양한 Vulkan 구조체 초기화 과정의 추상화를 포함한다고 언급했었습니다. 다음 두 구조체의 구현을 살펴봅시다.

<!-- codegen from tag init_cmd on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_initializers.cpp --> 
```cpp
VkCommandPoolCreateInfo vkinit::command_pool_create_info(uint32_t queueFamilyIndex,
    VkCommandPoolCreateFlags flags /*= 0*/)
{
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.queueFamilyIndex = queueFamilyIndex;
    info.flags = flags;
    return info;
}


VkCommandBufferAllocateInfo vkinit::command_buffer_allocate_info(
    VkCommandPool pool, uint32_t count /*= 1*/)
{
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;

    info.commandPool = pool;
    info.commandBufferCount = count;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    return info;
}
```


커맨드 버퍼의 레벨을 `VK_COMMAND_BUFFER_LEVEL_PRIMARY`로 하드코딩할 것입니다. 세컨더리 커맨드 버퍼를 사용하지 않을 것이기 때문에 이를 무시할 수 있습니다. 엔진에 맞는 기본값을 설정함으로서 전체적인 구조를 단순화할 수 있습니다.

<!-- codegen from tag init_cmd on file E:\ProgrammingProjects\vulkan-guide-2\chapter-1/vk_engine.cpp --> 
```cpp
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
}
```

더 낫고 더 짧아졌습니다. 가이드를 거치며 `vkinit` 네임스페이스를 사용하겠습니다. 이 네임스페이스는 매우 간단하므로, 다른 프로젝트에서도 안전하게 재사용할 수 있습니다. 0장에서 설명한대로 starting_point 브랜치에 작성되었음을 유의하세요.

## 정리
이전과 같이 생성한 객체를 파괴해야 합니다.

```cpp
void VulkanEngine::cleanup()
{
	if (_isInitialized) {
		//make sure the gpu has stopped doing its things
		vkDeviceWaitIdle(_device);

		for (int i = 0; i < FRAME_OVERLAP; i++) {
			vkDestroyCommandPool(_device, _frames[i]._commandPool, nullptr);
		}

		// --- rest of code
	}
}
```

커맨드 풀이 가장 최근에 생성한 Vulkan 객체이기 때문에 다른 객체보다 먼저 파괴해야 합니다. `VkCommandBuffer`를 개별적으로 파괴할 수는 없습니다. 대신 커맨드 풀이 파괴되면 해당 풀에서 할당된 모든 커맨드 버퍼가 함께 파괴됩니다.

또한 `VkQueue`들은 `VkPhysicalDevice`처럼 파괴될 수 없습니다. 이들은 실제로 생성된 객체가 아니라 `VkInstance`의 일부에 대한 핸들이기 때문입니다.

이제 GPU로 명령을 전송할 방법을 마련했지만, CPU와 GPU간 실행을 동기화할 구조체를 구현해야 합니다.

Next: [ Rendering Loop]({{ site.baseurl }}{% link docs/ko/new_chapter_1/vulkan_mainloop.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}
