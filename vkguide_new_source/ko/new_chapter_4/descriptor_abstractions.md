---
layout: default
title: Descriptor Abstractions
parent: "4. Textures and Engine Architecture- KO"
grand_parent: "Korean VKGuide"
nav_order: 2
---

이제 텍스쳐를 지원하고 복잡도가 크게 증가함에 따라 엔진 추상화를 확장해야 합니다. 특히 디스크립터 셋에 대해 더 나은 추상화가 필요합니다.

2장에서는 이미 DescriptorAllocator와 DescriptorLayoutBuilder라는 2개의 클래스를 생성했습니다. DescriptorAllocator는 하나의 VkDescriptorPool를 사용해 디스크립터를 할당하는 기본적인 추상화를 제공하고, LayoutBuilder는 디스크립터 셋 레이아웃 생성을 추상화합니다.

# Descriptor Allocator 2
이제 우리는 `DescriptorAllocatorGrowable`이라는 새로운 버전의 디스크립터 할당기를 만들겠습니다. 이전에 생성했던 것은 디스크립터 풀의 공간이 부족하면 그대로 크래시가 발생합니다. 이는 필요한 디스크립터 셋의 수를 사전에 알 수 있는 상황에서는 괜찮지만, 임의의 파일로부터 메시를 불러오는 상황에서는 사용할 수 없습니다. 얼마나 많은 디스크립터가 필요한지 예측할 수 없기 때문입니다. 새로운 클래스는 기존 방식과 거의 동일하게 동작하지만, 단일 풀을 다루는 것 대신 여러 개의 풀을 관리합니다. 할당에 실패할 경우, 새로운 디스크립터 풀을 생성합니다. 이 할당기를 해제하면 관리중인 모든 풀을 정리합니다. 이 방법으로 1개의 DescriptorAllocator를 사용할 수 있으며, 필요에 따라 자동으로 확장됩니다.

아래는 vk_descriptors.h에 들어갈 구현부입니다.

^code descriptor_allocator_grow shared/vk_descriptors.h 

public 인터페이스는 이전의 DescriptorAllocator와 동일합니다. 달라진 점은 PoolSizeRatio의 배열(풀을 재할당 할 때 필요합니다), 풀마다 할당할 디스크립터 셋의 수, 그리고 2개의 배열입니다. `fullPools`는 더이상 할당할 수 없는 풀이 들어가고, 그리고 `readyPools`는 아직 사용할 수 있는 풀이나 새로 생성된 풀이 들어갑니다.

할당 로직은 먼저 readyPools에서 풀을 하나 가져와 할당을 시도합니다. 할당이 성공하면 풀을 다시 readyPools 배열로 넣습니다. 만약 실패한다면 fullPools 배열로 옮긴 뒤 다른 풀을 가져와 다시 시도합니다. `get_pool` 함수는 readyPools로부터 풀을 선택하거나 새로운 풀을 생성합니다.

get_pool과 create_pool 함수를 작성해봅시다.

^code growpool_1 shared/vk_descriptors.cpp 

get_pools에서는 새로운 풀을 생성할 때 setsPerPool을 증가시켜 마치 std::vector의 resize처럼 작동합니다. 하지만 너무 커지는 것을 방지하기 위해 풀마다 디스크립터 셋의 최대 개수는 4092로 제한합니다. 이 제한은 사용자의 상황에 따라 바뀔 수 있습니다.

함수에서 중요한 점은 풀을 가져올 때 readyPools에서 해당 풀을 제거한다는 점입니다. 이렇게 하면 디스크립터 할당 후 해당 풀을 다시 readyPools 또는 다른 배열에 넣을 수 있습니다.

create_pool 함수는 이전의 다른 descriptorAllocator에서 했던 것과 동일합니다.

이제 필요한 다른 함수도 작성해봅시다.

^code growpool_2 shared/vk_descriptors.cpp 

init 함수는 첫 디스크립터 풀을 할당하고 readyPools 배열로 추가하기만 합니다.

풀을 초기화하는 것은 모든 풀을 초기화하고 fullPools 배열의 모든 풀을 readyPools 배열로 복사하는 것을 의미합니다.

파괴 과정에서는 두 배열을 모두 순회하며 모든 풀을 제거하여 할당기를 완전히 정리합니다.

마지막은 새로운 할당 함수입니다.

^code growpool_3 shared/vk_descriptors.cpp 

먼저 풀을 가져와 할당합니다. 만약 할당이 실패한다면 해당 풀은 가득 찼다고 판단하여 fullPools 배열에 추가하고 다른 풀을 통해 다시 할당을 시도합니다. 만약 두 번째에도 할당이 실패한다면 심각한 문제가 발생한 것으로 간주하고 assert로 크래시를 일으킵니다. 풀로부터 할당이 성공하면 readyPools 배열에 추가합니다.

# Descriptor Writer
컴퓨트 셰이더에 사용할 디스크립터 셋을 만들 때 `vkUpdateDescriptorSets`를 수동으로 호출해 사용했습니다. 하지만 이 방식은 매우 번거롭고 관리하기 어렵습니다. 따라서 이 부분도 추상화를 진행하겠습니다. 우리의 writer에서는 데이터를 바인딩하기 위해 `write_image`와 `write_buffer`함수를 사용할 것입니다. vk_descriptors.h 파일에 선언된 구조체를 살펴봅시다.

^code writer shared/vk_descriptors.h 

여기서는 std::deque를 사용하여 몇 가지 메모리 트릭을 사용할 것입니다. std::deque은 원소에 대한 포인터가 유효하게 유지된다는 것이 보장되므로, 새로운 `VkWriteDescriptorSet`을 writes 배열로 추가할 때 이 점을 활용할 수 있습니다.

`VkWriteDescriptorSet`의 정의를 살펴봅시다.
```cpp
typedef struct VkWriteDescriptorSet {
    VkStructureType                  sType;
    const void*                      pNext;
    VkDescriptorSet                  dstSet;
    uint32_t                         dstBinding;
    uint32_t                         dstArrayElement;
    uint32_t                         descriptorCount;
    VkDescriptorType                 descriptorType;
    const VkDescriptorImageInfo*     pImageInfo;
    const VkDescriptorBufferInfo*    pBufferInfo;
    const VkBufferView*              pTexelBufferView;
} VkWriteDescriptorSet;
```

타겟 디스크립터 셋과 바인딩 요소가 있으며, 실제 버퍼나 이미지는 포인터로 전달됩니다. 따라서 VkDescriptorBufferInfo 등의 정보를 포인터가 안정적으로 유지되도록 보존하거나, 최종 WriteDescriptorSet 배열을 만들 때 포인터를 수정할 수 있는 방식으로 관리해야 합니다.

write_buffer가 어떤 일을 하는 지 살펴봅시다.

^code write_buffer shared/vk_descriptors.cpp 

먼저 VkDescriptorBufferInfo를 채워야 합니다. 이때 버퍼 자체와 오프셋, 그리고 범위(size)를 설정합니다.

그후, write 객체 자체를 설정해야 합니다. 하나의 디스크립터에 대해 지정된 바인딩 슬롯에 올바른 타입과 함께 VkDescriptorBufferInfo에 대한 포인터를 지정합니다. 해당 info는 std::deque에 emplace_back으로 생성했기 때문에, 포인터를 안정적으로 사용할 수 있습니다.
 
버퍼에 사용할 수 있는 디스크립터 타입은 다음과 같습니다.

```
VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
```

이러한 버퍼 타입들은 지난 챕터에서 이미 설명했습니다. 셰이더에 특정 타입의 버퍼를 바인딩하려면, 여기서 올바른 디스크립터 타입을 지정해야 합니다. VkBuffer를 할당할 때 지정한 Usage 플래그와 일치시켜야한다는 점을 기억하세요.

이미지에는 다른 함수를 사용합니다.
^code write_image shared/vk_descriptors.cpp 

버퍼와 유사하지만 Info 타입이 다릅니다. `VkDescriptorImageInfo`를 대신 사용하고 있습니다. 여기에는 샘플러와 이미지 뷰, 이미지가 사용하는 레이아웃을 전달해야 합니다. 레이아웃은 대부분 셰이더에서 텍스쳐에 접근할 때 최적의 레이아웃인  `VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL`이거나 컴퓨트 셰이더에서 쓰기를 수행할 때 사용하는 `VK_IMAGE_LAYOUT_GENERAL`을 사용합니다.

이 ImageInfo의 세 가지 파라미터는 선택이지만, 상황에 따라 달라집니다.

* `VK_DESCRIPTOR_TYPE_SAMPLER`는 샘플러 뿐입니다. 따라서 ImageView나 레이아웃을 설정할 필요가 없습니다.
* `VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE`는 셰이더에서 다른 샘플러에 접근될 것이기 때문에 샘플러를 요구하지 않습니다. 따라서 이 디스크립터 타입은 이미지의 포인터일 뿐입니다.
* `VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER`는 모든 것을 설정해야 합니다. 샘플러와 샘플링할 이미지에 대한 정보 모두를 담아야 합니다. 이는 텍스쳐에 접근하기 위해 1개의 디스크립터만 바인딩하면 되기 때문에 유용한 타입입니다.
* `VK_DESCRIPTOR_TYPE_STORAGE_IMAGE`는 2장에서 사용했습니다. 이는 샘플러를 요구하지 않으며 픽셀 데이터에 직접 접근하는 컴퓨트셰이더에서 사용됩니다.

write_image와 write_buffer 함수는 다소 일반적으로 구현되어 있습니다. 이는 단순함을 위한 것이지만, 필요하다면 `VK_DESCRIPTOR_TYPE_SAMPLER`를 사용하고 imageView와 layout을 null로 설정하는 `write_sampler()`와 같은 함수를 추가하여 더 구체적인 추상화를 만들 수도 있습니다

이제 이를 바탕으로 실제 write 작업을 수행할 수 있습니다.


^code writer_end shared/vk_descriptors.cpp 

clear()함수는 모든 상태를 초기화합니다. update_set 함수는 디바이스와 디스크립터 셋을 입력받아 해당 셋을 writes의 배열에 연결한 뒤, `vkUpdateDescriptorSets`를 호출해 디스크립터 셋에 새로운 바인딩을 적용합니다.

이 추상화를 통해 기존 `init_descriptors` 함수에서 사용하던 코드를 어떻게 대체할수 있는지 살펴봅시다.


before: 
```cpp
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
```

after:
```cpp
DescriptorWriter writer;
writer.write_image(0, _drawImage.imageView, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);

writer.update_set(_device,_drawImageDescriptors);
```

이 추상화는 allocator와 layout builder를 결합하여 좀 더 복잡한 디스크립터 셋을 관리할 때 더 유용할 것입니다.

# 동적 디스크립터 할당
이제 이 추상화를 사용하여 매 프레임마다 전역 씬 데이터 디스크립터를 생성해 봅시다. 이 디스크립터 셋은 모든 드로우콜마다 사용되며, 카메라 행렬을 포함하여 3D 렌더링이 가능하도록 해줄 것입니다.

디스크립터 셋을 런타임에 할당하기 위해서 FrameData 구조체에 DescriptorAllocator를 담습니다. 이 방식은 삭제 큐처럼 작동하여, 프레임 렌더링을 시작할 때 자원을 정리하여 파괴합니다. 개별 디스크립터 셋의 수명을 추적하는 것보다 전체 디스크립터 풀을 한 번에 리셋하는 방식이 훨씬 빠릅니다.

이를 FrameData 구조체에 추가합니다.

```cpp
struct FrameData {
	VkSemaphore _swapchainSemaphore, _renderSemaphore;
	VkFence _renderFence;

	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

	DeletionQueue _deletionQueue;
	DescriptorAllocatorGrowable _frameDescriptors;
};
```

이제 스왑체인과 해당 구조체를 초기화할 때 함께 초기화합시다. init_descriptors() 함수의 끝에 다음 코드를 추가합니다.

^code frame_desc chapter-4/vk_engine.cpp

그후, 매 프레임마다 삭제 큐를 flush 할 때 이를 정리할 수 있습니다. 이 코드는 draw() 함수의 시작 부분에 위치합니다.

^code frame_clear chapter-4/vk_engine.cpp

이제 디스크립터 셋을 동적으로 할당할 수 있습니다. 씬 데이터를 담을 버퍼를 할당하고 해당 디스크립터 셋을 생성하겠습니다.

씬 데이터의 유니폼 버퍼에 사용하는 새로운 구조체를 추가합시다. 뷰 행렬과 투영 행렬을 개별적으로 저장하고, 미리 곱한 view-projection 행렬도 포함합니다. 또한 이후에 구성할 아주 기본적인 조명 모델에 필요한 몇 가지 vec4도 추가합니다.

```cpp
struct GPUSceneData {
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewproj;
    glm::vec4 ambientColor;
    glm::vec4 sunlightDirection; // w for sun power
    glm::vec4 sunlightColor;
};
```

VulkanEngine 클래스에 새로운 디스크립터 레이아웃을 추가합니다.
```
GPUSceneData sceneData;

VkDescriptorSetLayout _gpuSceneDataDescriptorLayout;
```

init_descriptors함수에서 디스크립터 셋 레이아웃을 생성합니다. 이 디스크립터 셋은 하나의 유니폼 버퍼만을 바인딩합니다. 버퍼가 작기 때문에 여기서는 SSBO 대신 유니폼 버퍼를 사용하며, 버퍼 디바이스 어드레스를 사용하지 않는 이유는 모든 객체가 하나의 디스크립터 셋을 공유하기 때문에 별도로 관리할 오버헤드가 없기 때문입니다.

```cpp
{
	DescriptorLayoutBuilder builder;
	builder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	_gpuSceneDataDescriptorLayout = builder.build(_device, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
}
```

이제 `draw_geometry()` 함수 내부에서 매 프레임마다 디스크립터 셋을 생성할 것입니다. 프레임마다 동적으로 생성되는 임시 데이터를 처리하는 예제로 유니폼 버퍼도 동적으로 할당할 예정입니다. 실제로는 이 버퍼들을 FrameData 구조체에 캐싱하는 것이 더 바람직하지만, 여기서는 그 과정을 보여주기 위한 목적입니다. 동적 드로우나 렌더 패스를 처리할 때는 이와 같은 방식이 필요할 수 있습니다.

```cpp	
	//allocate a new uniform buffer for the scene data
	AllocatedBuffer gpuSceneDataBuffer = create_buffer(sizeof(GPUSceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	//add it to the deletion queue of this frame so it gets deleted once its been used
	get_current_frame()._deletionQueue.push_function([=, this]() {
		destroy_buffer(gpuSceneDataBuffer);
		});

	//write the buffer
	GPUSceneData* sceneUniformData = (GPUSceneData*)gpuSceneDataBuffer.allocation->GetMappedData();
	*sceneUniformData = sceneData;

	//create a descriptor set that binds that buffer and update it
	VkDescriptorSet globalDescriptor = get_current_frame()._frameDescriptors.allocate(_device, _gpuSceneDataDescriptorLayout);

	DescriptorWriter writer;
	writer.write_buffer(0, gpuSceneDataBuffer.buffer, sizeof(GPUSceneData), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	writer.update_set(_device, globalDescriptor);
```

먼저 CPU_TO_GPU 플래그를 사용해 CPU에서 읽고 쓸 수 있는 타입의 유니폼 버퍼를 할당합니다. 실제로는 CPU RAM에 위치할 수 있지만, 소량의 데이터이기 때문에 GPU가 캐시로 데이터를 불러올 때 문제가 없습니다. 따라서 이러한 경우 스테이징 버퍼를 통해 GPU 전용 메모리로 업로드하는 과정은 생략할 수 있습니다.

그후 버퍼를 현재 프레임의 삭제 큐에 추가합니다. 이는 버퍼를 다음 프레임이 렌더링 된 후 파괴되도록 하여 GPU가 해당 버퍼에 접근을 마칠 충분한 시간을 확보할 수 있게 합니다. 한 프레임에 동적으로 생성된 모든 자원은 이 삭제 큐를 통해 정리되어야 합니다.

디스크립터 셋은 _frameDescriptors로부터 할당합니다. 해당 풀은 매 프레임 리셋되므로 삭제 큐와 비슷하게 동작하며, GPU가 자원 사용을 끝내는 시점은 2프레임 뒤에 자동으로 삭제됩니다.

그 다음 새로 만든 버퍼를 디스크립터 셋에 작성합니다. 이제 전역 디스크립터가 렌더링에 사용할 준비가 되었습니다. 지금은 씬 데이터 버퍼를 사용하고 있지 않지만 이후 필요할 것입니다.

렌더링을 계속하기 전에 텍스쳐를 설정합시다.

^nextlink

{% include comments.html term="Vkguide 2 Korean Comments" %}