---
layout: default
title: Vulkan Shaders - Code
parent:  "2. Drawing with Compute - KO"
grand_parent: "Korean VKGuide"
nav_order: 5
---

이제 컴퓨트 셰이더에 필요한 코드를 작성하겠습니다. 먼저 이미지를 입력받아 쓰레드 ID 기반으로 색상을 계산하여 그라디언트를 형성하는 매우 단순한 셰이더부터 시작하겠습니다. 이 셰이더는 이미 shaders 폴더에 작성되어 있습니다. 앞으로 추가될 모든 셰이더는 해당 폴더에 추가하여 CMake 스크립트를 통해 자동으로 빌드되도록 구성할 것입니다.

gradient.comp
```c
//GLSL version to use
#version 460

//size of a workgroup for compute
layout (local_size_x = 16, local_size_y = 16) in;

//descriptor bindings for the pipeline
layout(rgba16f,set = 0, binding = 0) uniform image2D image;


void main() 
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	ivec2 size = imageSize(image);

    if(texelCoord.x < size.x && texelCoord.y < size.y)
    {
        vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

        if(gl_LocalInvocationID.x != 0 && gl_LocalInvocationID.y != 0)
        {
            color.x = float(texelCoord.x)/(size.x);
            color.y = float(texelCoord.y)/(size.y);	
        }
    
        imageStore(image, texelCoord, color);
    }
}
```

GLSL 버전을 지정하는 것으로 시작합니다. 460은 GLSL 4.6버전에 대응됩니다.

다음은 작업 그룹의 크기를 정의하는 layout 구문을 작성합니다. 앞서 설명했듯이, 컴퓨트 셰이더는 여러 쓰레드(혹은 레인)로 이루어진 그룹 단위로 실행됩니다. 여기서는 x=16, y=16, z=1(기본값)으로 지정하고 있는데, 이는 각 작업 그룹이 16 * 16 쓰레드 단위로 작업한다는 것을 의미합니다.

다음은 셰이더 입력을 위한 디스크립터 셋을 설정합니다. 여기서는  set 0, binding 0위치에 단일 `image2D`를 바인딩하고 있습니다. Vulkan에서는 하나의 디스크립터 **셋**이 여러 바인딩을 가질 수 있으며, 이는 해당 셋을 바인딩할 때 함께 바인딩되는 자원들입니다. 따라서 디스크립터 셋 하나의 0번 인덱스에 단일 이미지를 0번 바인딩에 포함시키는 구조입니다.

이 셰이더는 global invocation ID의 좌표를 기반으로 그라디언트를 형성하는 매우 단순한 더미 셰이더입니다. 만약 local Invocation ID가 X Y에서 0이라면, 검은 색이 나올 것입니다. 결과적으로 셰이더 작업그룹 호출을 시각적으로 보여주는 그리드를 생성할 것입니다.

셰이더 코드를 수정할 때는 반드시 해당 셰이더를 타겟으로 추가해 컴파일해야 합니다. 또한 새 파일을 추가했다면 CMake를 다시 구성해야만 합니다. 이 과정에서 오류 없이 성공해야 GPU에서 해당 셰이더를 실행하는 데 필요한 spirv 파일들이 정상적으로 생성됩니다. 실패할 경우, 프로젝트에서 셰이더를 실행하기 위해 필요한 spirv파일이 누락될 수 있습니다.

## 디스크립터 셋 레이아웃 설정하기

컴퓨트 파이프라인을 구성하기 위해 파이프라인 레이아웃을 구성해야 합니다. 이 경우, 바인딩 0번에 이미지를 포함하는 단일 디스크립터 셋 만을 담는 레이아웃이 사용됩니다.

디스크립터 셋 레이아웃을 생성하기 위해, 바인딩의 정보를 배열 형태로 저장해야 합니다. 이를 간편하게 다루기 위해 이 과정을 추상화하는 구조체를 정의해보겠습니다. 디스크립터 추상화는 `vk_descriptors.h/cpp`에 작성할 것입니다.

<!-- codegen from tag descriptor_layout on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_descriptors.h --> 
```cpp
struct DescriptorLayoutBuilder {

    std::vector<VkDescriptorSetLayoutBinding> bindings;

    void add_binding(uint32_t binding, VkDescriptorType type);
    void clear();
    VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shaderStages, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
};
```

config/info 구조체인 `VkDescriptorSetLayoutBinding`를 배열로 담을 것입니다. 그리고 이를 기반으로 실제 Vulkan 객체 `VkDescriptorSetLayout`을 만드는 build()함수를 정의하겠습니다.

이제 Builder를 위한 함수를 작성하겠습니다.

<!-- codegen from tag descriptor_bind on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_descriptors.cpp --> 
```cpp
void DescriptorLayoutBuilder::add_binding(uint32_t binding, VkDescriptorType type)
{
    VkDescriptorSetLayoutBinding newbind {};
    newbind.binding = binding;
    newbind.descriptorCount = 1;
    newbind.descriptorType = type;

    bindings.push_back(newbind);
}

void DescriptorLayoutBuilder::clear()
{
    bindings.clear();
}
```

먼저, `add_binding` 함수를 작성하겠습니다. 이 함수는 `VkDescriptorSetLayoutBinding` 구조체를 구성한 뒤 배열에 추가합니다. 레이아웃 바인딩을 생성할 때 지금은 바인딩 숫자와 디스크립터 타입만 알고 있으면 충분합니다. 위에서 다룬 컴퓨트 셰이더 예제에서는 바인딩 0번에 타입은 `VK_DESCRIPTOR_TYPE_STORAGE_IMAGE`입니다. 이 타입은 쓰기 가능한 이미지를 의미합니다.

이제 실제 레이아웃을 생성해봅시다.

<!-- codegen from tag descriptor_layout on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_descriptors.cpp --> 
```cpp
VkDescriptorSetLayout DescriptorLayoutBuilder::build(VkDevice device, VkShaderStageFlags shaderStages, void* pNext, VkDescriptorSetLayoutCreateFlags flags)
{
    for (auto& b : bindings) {
        b.stageFlags |= shaderStages;
    }

    VkDescriptorSetLayoutCreateInfo info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    info.pNext = pNext;

    info.pBindings = bindings.data();
    info.bindingCount = (uint32_t)bindings.size();
    info.flags = flags;

    VkDescriptorSetLayout set;
    VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &set));

    return set;
}
```

먼저 바인딩을 순회하며 셰이더 실행단계 플래그(stage flag)를 추가합니다. 하나의 디스크립터 셋 내에서 각 디스크립터 바인딩은 프래그먼트 셰이더나 정점 셰이더 등 서로 다른 셰이더 단계에서 사용될 수 있습니다. 그러나 이번 구현에서는 바인딩별 셰이더 단계 플래그는 지원하지 않고 디스크립터 셋 전체에 동일한 셰이더 단계가 적용되도록 강제할 것입니다.

다음은 `VkDescriptorSetLayoutCreateInfo`를 구성하겠습니다. 설정할 것이 많지는 않은데, 디스크립터 바인딩의 배열을 연결하기만 하면 됩니다. 이후 `vkCreateDescriptorSetLayout`을 호출해 디스크립터 셋 레이아웃을 생성합니다.

# 디스크립터 할당기

디스크립터 셋 레이아웃이 구현되었으니 디스크립터 셋을 할당할 수 있습니다. 마찬가지로 코드를 간결하게 작성할 수 있도록 이를 추상화한 할당자 구조체를 작성해보겠습니다.
<!-- codegen from tag descriptor_allocator on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_descriptors.h --> 
```cpp
struct DescriptorAllocator {

    struct PoolSizeRatio{
		VkDescriptorType type;
		float ratio;
    };

    VkDescriptorPool pool;

    void init_pool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
    void clear_descriptors(VkDevice device);
    void destroy_pool(VkDevice device);

    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
};
```

디스크립터 할당은 `VkDescriptorPool`을 통해 이루어집니다. 이 객체는 디스크립터 셋의 크기와 타입을 미리 지정하여 초기화해야 합니다. 특정 디스크립터를 위한 메모리 할당자로 생각할 수도 있습니다. 매우 큰 1개의 디스크립터 셋이 전체 엔진을 관리할 수도 있지만, 이 경우 사용할 모든 디스크립터를 사전에 알아야 한다는 제약이 있습니다. 이는 규모가 커질수록 매우 어렵고 복잡할 수 있습니다. 대신 조금 더 단순한 방식을 구성하겠습니다. 프로젝트의 다양한 부분에 여러 디스크립터 풀을 할당하고 상황에 맞게 더 정확히 설정하는 것입니다.

디스크립터 풀 관련해서 한 가지 매우 중요한 점은 풀을 리셋하면 해당 풀에서 할당된 모든 디스크립터 셋이 파괴된다는 것입니다. 이는 프레임 단위로 생성되는 디스크립터 셋을 다룰 때 매우 유용합니다. 한 프레임 동안만 사용되는 디스크립터 셋을 동적으로 할당하고, 프레임 시작 전 한번에 모두 제거할 수 있습니다. 이 방식은 GPU 제조사에 의해 가장 빠른 경로로 최적화 되었음이 확인되었으며, 프레임별 디스크립터 셋을 구성할 때 사용할 것을 권장합니다.

`DescriptorAllocator`에는 디스크립터 풀 초기화, 해제, 그리고 디스크립터 셋을 할당하는 함수만을 선언하겠습니다.

이제 코드를 작성해봅시다.

<!-- codegen from tag descriptor_pool_init on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_descriptors.cpp --> 
```cpp
void DescriptorAllocator::init_pool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios)
{
    std::vector<VkDescriptorPoolSize> poolSizes;
    for (PoolSizeRatio ratio : poolRatios) {
        poolSizes.push_back(VkDescriptorPoolSize{
            .type = ratio.type,
            .descriptorCount = uint32_t(ratio.ratio * maxSets)
        });
    }

	VkDescriptorPoolCreateInfo pool_info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
	pool_info.flags = 0;
	pool_info.maxSets = maxSets;
	pool_info.poolSizeCount = (uint32_t)poolSizes.size();
	pool_info.pPoolSizes = poolSizes.data();

	vkCreateDescriptorPool(device, &pool_info, nullptr, &pool);
}

void DescriptorAllocator::clear_descriptors(VkDevice device)
{
    vkResetDescriptorPool(device, pool, 0);
}

void DescriptorAllocator::destroy_pool(VkDevice device)
{
    vkDestroyDescriptorPool(device,pool,nullptr);
}
```

생성과 파괴 함수를 추가했습니다. `clear` 함수는 파괴가 아니라 리셋하는 함수입니다. 이 함수는 풀로부터 생성된 모든 디스크립터 셋을 파괴하고 풀을 초기 상태로 되돌리지만, `VkDescriptorPool` 그 자체를 파괴하지는 않습니다.

디스크립터 풀을 초기화하려면 `vkCreateDescriptorPool`을 사용하고, 여기에 `PoolSizeRatio`의 배열을 전달해야 합니다. 이 구조체는 디스크립터 타입(`VkDescriptorType`, 위의 바인딩에서 사용한 것과 동일)과 `maxSets` 파라미터에 곱해질 비율을 함께 포함하고 있습니다. 이를 통해 디스크립터 풀이 얼마나 클지를 직접 제어할 수 있습니다. `maxSets`은 해당 풀에서 생성할 수 있는 `VkDescriptorSets`의 최대 개수를 의미하고, `poolSizes`는 특정 타입의 디스크립터 바인딩이 몇 개 존재할지를 나타냅니다.

마지막으로 `DescriptorAllocator::allocate` 함수를 작성해야 합니다. 아래 코드를 확인해보세요.

<!-- codegen from tag descriptor_alloc on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_descriptors.cpp --> 
```cpp
VkDescriptorSet DescriptorAllocator::allocate(VkDevice device, VkDescriptorSetLayout layout)
{
    VkDescriptorSetAllocateInfo allocInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    VkDescriptorSet ds;
    VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &ds));

    return ds;
}
```

`VkDescriptorSetAllocateInfo`를 채워야 합니다. 이 구조체에는 디스크립터 셋을 할당받을 디스크립터 풀, 할당할 디스크립터 셋의 개수, 그리고 디스크립터 셋 레이아웃이 필요합니다.

# 디스크립터 셋과 레이아웃 초기화하기
`VulkanEngine`에 새로운 함수와 사용할 멤버를 추가합시다.

```cpp
#include <vk_descriptors.h>

struct VulkanEngine{
public:
	DescriptorAllocator globalDescriptorAllocator;

	VkDescriptorSet _drawImageDescriptors;
	VkDescriptorSetLayout _drawImageDescriptorLayout;

private:
	void init_descriptors();
}
```

이러한 `DescriptorAllocator` 중 하나를 전역 할당기로서 엔진에 저장할 것입니다. 그 후 렌더링한 이미지를 바인딩할 디스크립터 셋과, 해당 타입의 디스크립터를 위한 디스크립터 셋 레이아웃을 저장해야 합니다. 이 레이아웃은 이후 파이프라인 생성 시 필요하게 됩니다.

`init_descriptors()` 함수를 `VulkanEngine`의 `init()`함수 내에서 `sync_structures` 이후 추가하는 것을 잊지 마세요.


```cpp
void VulkanEngine::init()
{
	//other code

	init_commands();

	init_sync_structures();

	init_descriptors();	

	//everything went fine
	_isInitialized = true;
}
```

이제 함수를 작성할 수 있습니다.
```cpp
void VulkanEngine::init_descriptors()
{
	//create a descriptor pool that will hold 10 sets with 1 image each
	std::vector<DescriptorAllocator::PoolSizeRatio> sizes =
	{
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }
	};

	globalDescriptorAllocator.init_pool(_device, 10, sizes);

	//make the descriptor set layout for our compute draw
	{
		DescriptorLayoutBuilder builder;
		builder.add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		_drawImageDescriptorLayout = builder.build(_device, VK_SHADER_STAGE_COMPUTE_BIT);
	}
}
```

먼저 `DescriptorAllocator`를 초기화 할 때, 총10개의 디스크립터 셋을 할당 가능하도록 하고, 각 디스크립터 셋에는 `VK_DESCRIPTOR_TYPE_STORAGE_IMAGE`타입의 디스크립터를 1개씩 가지도록 설정합니다. 이는 컴퓨트 셰이더에서 작성할 수 있는 이미지에 사용되는 타입입니다.

그 후, 레이아웃 빌더를 사용해 필요한 디스크립터 셋 레이아웃을 구성할 수 있습니다. 이는 바인딩 0번에 `VK_DESCRIPTOR_TYPE_STORAGE_IMAGE`를 나타내는 레이아웃입니다.(앞서 정의한 디스크립터 풀과 일치합니다.)

이 과정을 통해, 컴퓨트 셰이더 그리기에 사용할 수 있는 `VK_DESCRIPTOR_TYPE_STORAGE_IMAGE` 타입의 디스크립터 셋을 10개까지 할당할 수 있습니다.

이어서 그 중 하나를 실제로 할당한 뒤, 렌더링할 이미지를 참조하도록 디스크립터를 작성합니다.

```cpp
void VulkanEngine::init_descriptors()
{
	// other code
	//allocate a descriptor set for our draw image
	_drawImageDescriptors = globalDescriptorAllocator.allocate(_device,_drawImageDescriptorLayout);	

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

	//make sure both the descriptor allocator and the new layout get cleaned up properly
	_mainDeletionQueue.push_function([&]() {
		globalDescriptorAllocator.destroy_pool(_device);

		vkDestroyDescriptorSetLayout(_device, _drawImageDescriptorLayout, nullptr);
	});
}
```

먼저 앞서 생성한 `DescriptorAllocator`, `_drawImageDescriptorLayout`을 통해 디스크립터 셋 객체를 할당합니다. 그 후 디스크립터 셋을 우리가 사용할 이미지와 연결해주어야 합니다. 이를 위해 `vkUpdateDescriptorSets` 함수를 사용합니다. 이 함수는 개별 업데이트 정보를 담는 `VkWriteDescriptorSets` 배열을 인자로 받습니다. 할당한 디스크립터 셋의 바인딩 0번을 가리키는 단일 write를 생성하여 올바른 디스크립터 타입을 명시합니다. 이는 또한 바인딩할 실제 이미지 데이터를 담는 `VkDescriptorImageInfo`를 가리킵니다. `VkDescriptorImageInfo`는 그릴 이미지의 이미지 뷰를 포함합니다.

이 과정을 마치면, 그릴 이미지에 바인딩할 디스크립터 셋과 필요한 레이아웃이 만들어졌습니다. 마침내 컴퓨트 파이프라인을 생성할 수 있습니다.

## 컴퓨트 파이프라인

디스크립터 셋 레이아웃이 준비되었으므로 파이프라인 레이아웃을 생성할 기반이 마련되었습니다. 파이프라인을 생성하기 전에 마지막으로 해야 할 작업이 남아있습니다. 셰이더 코드를 불러와 드라이버에 연결하는 것입니다. Vulkan 파이프라인에서는 셰이더를 설정하기 위해 `VkShaderModule`을 생성해야 합니다. 이를 불러오는 함수를 `vk_pipelines.h/cpp`에 추가합시다.

다음을 `vk_pipelines.cpp`에 추가합니다.
```cpp
#include <vk_pipelines.h>
#include <fstream>
#include <vk_initializers.h>
```

이 함수를 추가하고 헤더에도 선언부를 추가해줍니다.
<!-- codegen from tag load_shader on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_pipelines.cpp --> 
```cpp
bool vkutil::load_shader_module(const char* filePath,
    VkDevice device,
    VkShaderModule* outShaderModule)
{
    // open the file. With cursor at the end
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    // find what the size of the file is by looking up the location of the cursor
    // because the cursor is at the end, it gives the size directly in bytes
    size_t fileSize = (size_t)file.tellg();

    // spirv expects the buffer to be on uint32, so make sure to reserve a int
    // vector big enough for the entire file
    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

    // put file cursor at beginning
    file.seekg(0);

    // load the entire file into the buffer
    file.read((char*)buffer.data(), fileSize);

    // now that the file is loaded into the buffer, we can close it
    file.close();

    // create a new shader module, using the buffer we loaded
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;

    // codeSize has to be in bytes, so multply the ints in the buffer by size of
    // int to know the real size of the buffer
    createInfo.codeSize = buffer.size() * sizeof(uint32_t);
    createInfo.pCode = buffer.data();

    // check that the creation goes well.
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        return false;
    }
    *outShaderModule = shaderModule;
    return true;
}
```

이 함수는 먼저 파일을 `std::vector<uint32_t>`형식으로 불러옵니다. 이는 컴파일된 셰이더 데이터를 담을 것이며, `vkCreateShaderModule`을 호출할 때 사용됩니다. 셰이더 모듈의 `createInfo`는 셰이더 데이터를 담는 배열 외에는 특별히 필요한 정보는 없습니다. 셰이더 모듈은 파이프라인을 생성할 때에만 필요하며, 파이프라인이 생성된 이후에는 안전하게 파괴해도 되므로 이를 `VulkanEngine`에 담을 필요는 없습니다.

이제 `VulkanEngine`으로 돌아와 필요한 새로운 멤버를 추가하고 `init_pipelines()`함수와 `init_background_pipelines()` 함수를 추가합시다. `init_pipelines()`는 추후 튜토리얼이 진행되며 추가할 다른 파이프라인 초기화 함수도 호출할 것입니다.

```cpp
class VulkanEngine{
public:
	VkPipeline _gradientPipeline;
	VkPipelineLayout _gradientPipelineLayout;

private:
	void init_pipelines();
	void init_background_pipelines();
}
```

이를 `init`함수에 추가하고 `vk_pipelines.h`를 파일 상단에 포함시킵니다. `init_pipelines()` 함수는 `init_background_pipelines()`를 호출할 것입니다.

```cpp
#include <vk_pipelines.h>

void VulkanEngine::init()
{
	//other code

	init_commands();

	init_sync_structures();

	init_descriptors();	

	init_pipelines();

	//everything went fine
	_isInitialized = true;
}

void VulkanEngine::init_pipelines()
{
	init_background_pipelines();
}
```

이제 파이프라인을 생성하겠습니다. 먼저 파이프라인 레이아웃을 만들어야 합니다.
```cpp
void VulkanEngine::init_background_pipelines()
{
	VkPipelineLayoutCreateInfo computeLayout{};
	computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	computeLayout.pNext = nullptr;
	computeLayout.pSetLayouts = &_drawImageDescriptorLayout;
	computeLayout.setLayoutCount = 1;

	VK_CHECK(vkCreatePipelineLayout(_device, &computeLayout, nullptr, &_gradientPipelineLayout));
}
```

파이프라인을 생성하려면 사용할 디스크립터 셋 레이아웃 배열과 푸시상수와 같은 추가 구성이 필요합니다. 이 셰이더에서는 이러한 구성이 필요 없기 때문에, 디스크립터 셋 레이아웃만 사용하면 됩니다.

이제 셰이더 모듈을 불러와 다른 옵션들을 `VkComputePipelineCreateInfo`에 추가함으로서 파이프라인 객체 자체를 생성합니다.
```cpp
void VulkanEngine::init_background_pipelines()
{
	//layout code
	VkShaderModule computeDrawShader;
	if (!vkutil::load_shader_module("../../shaders/gradient.comp.spv", _device, &computeDrawShader))
	{
		fmt::print("Error when building the compute shader \n");
	}

	VkPipelineShaderStageCreateInfo stageinfo{};
	stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageinfo.pNext = nullptr;
	stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	stageinfo.module = computeDrawShader;
	stageinfo.pName = "main";

	VkComputePipelineCreateInfo computePipelineCreateInfo{};
	computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	computePipelineCreateInfo.pNext = nullptr;
	computePipelineCreateInfo.layout = _gradientPipelineLayout;
	computePipelineCreateInfo.stage = stageinfo;
	
	VK_CHECK(vkCreateComputePipelines(_device,VK_NULL_HANDLE,1,&computePipelineCreateInfo, nullptr, &_gradientPipeline));
}
```

먼저 앞서 만든 함수를 사용해 `VkShaderModule`을 불러와 파일에 오류가 있는지 확인합니다. 여기서 경로는 기본 Windows + msvc 빌드 폴더를 기준으로 설정되어 있다는 점을 유의하세요. 만약 다른 옵션을 사용한다면 파일 경로가 제대로 되었는지 확인하세요. 구성 파일에 설정된 폴더를 기준으로 경로를 추상화하는 것도 고려해보세요

그 다음으로 셰이더를 `VkPipelineShaderStageCreateInfo`로 연결합니다. 여기서 유의할 점은 사용할 셰이더의 함수의 이름을 전달한다는 점입니다. 여기서는 `main()`입니다. 이는 동일한 셰이더 파일 안에 다른 진입점을 설정함으로써 여러 컴퓨트 셰이더를 담을 수 있다는 의미입니다.

마지막으로 `VkComputePipelineCreateInfo`를 설정합니다. 이는 컴퓨트 셰이더의 단계 정보와 레이아웃이 필요합니다. 그 후 `vkCreateComputePipelines`를 호출할 수 있습니다.

함수의 끝에서 삭제 큐에 등록하여 프로그램 종료 시 구조체들이 정리되도록 합니다.

<!-- codegen from tag comp_pipeline_3 on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
	vkDestroyShaderModule(_device, computeDrawShader, nullptr);

	_mainDeletionQueue.push_function([&]() {
		vkDestroyPipelineLayout(_device, _gradientPipelineLayout, nullptr);
		vkDestroyPipeline(_device, _gradientPipeline, nullptr);
		});
```

셰이더 모듈은 해당 함수내에서 바로 파괴할 수 있습니다. 파이프라인이 이미 생성되었기 때문에 더 이상 필요하지 않습니다. 반면, 파이프라인과 파이프라인 레이아웃은 계속 필요하므로 프로그램이 종료 시 파괴합니다.

이제 그릴 준비가 끝났습니다.

# 컴퓨트 셰이더 그리기

`draw_background()`함수로 돌아와 `vkCmdClear`를 컴퓨트 셰이더 호출로 대체합니다.

<!-- codegen from tag draw_comp on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
	// bind the gradient drawing compute pipeline
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _gradientPipeline);

	// bind the descriptor set containing the draw image for the compute pipeline
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _gradientPipelineLayout, 0, 1, &_drawImageDescriptors, 0, nullptr);

	// execute the compute pipeline dispatch. We are using 16x16 workgroup size so we need to divide by it
	vkCmdDispatch(cmd, std::ceil(_drawExtent.width / 16.0), std::ceil(_drawExtent.height / 16.0), 1);
```

먼저 `vkCmdBindPipeline`을 사용하여 파이프라인을 바인딩합니다. 컴퓨트 셰이더를 사용하기 때문에 `VK_PIPELINE_BIND_POINT_COMPUTE`를 사용합니다. 그 후, 그릴 이미지를 담고있는 디스크립터 셋을 바인딩하여 셰이더가 접근할 수 있도록 합니다. 마지막으로 `vkCmdDispatch`를 사용하여 컴퓨트 셰이더를 실행합니다. 얼마나 셰이더가 몇 번 실행 될지는, 작업 그룹당 16 * 16 쓰레드가 실행된다는 점을 고려하여, 그릴 이미지의 해상도를 16으로 나누고 반올림함으로써 결정합니다.

이 시점에서 프로그램을 실행하면 이미지가 표시될 것입니다. 만약 셰이더를 불러오는 과정에서 오류가 발생했다면 CMake를 재실행하고 셰이더 타겟을 재설정하여 셰이더를 불러오세요. 이는 엔진을 구성할 때 자동으로 구성되지 않기 때문에 셰이더가 바뀔 때 마다 수동으로 재빌드해야합니다.

![chapter2]({{site.baseurl}}/diagrams/compute_grid.png)

Next: [ Setting up IMGUI]({{ site.baseurl }}{% link docs/ko/new_chapter_2/vulkan_imgui_setup.md %})  

{% include comments.html term="Vkguide 2 Korean Comments" %}
