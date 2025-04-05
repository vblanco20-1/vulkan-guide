---
layout: default
lang: ko
title: Vulkan Shaders - Code
parent:  "2. 컴퓨트 셰이더 그리기"
nav_order: 5
---

Lets now write the code necesary for our compute shader. We will begin with a very simple shader that has an image as input, and writes a color to it, based on the thread ID, forming a gradient. This one is already on the code, in the shaders folder. From now on, all the shaders we add will go into that folder as the CMake script will build them.
이제 우리의 컴퓨트 셰이더에 필요한 코드를 작성하겠습니다. 이미지를 입력으로 받아 쓰레드 ID 기반으로 색상을 작성하여 그라디언트를 형성하는 매우 단순한 셰이더로 시작할 것입니다. 이는 이미 코드로 작성되어 있으며, shaders 폴더에서 확인할 수 있습니다. 이제 앞으로 추가할 모든 셰이더는 해당 폴더에 추가하여 CMake 스크립트가 구성할 것입니다.

gradient.comp
```c
//GLSL version to use / 사용할 GLSL 버전입니다.
#version 460

//size of a workgroup for compute / 계산에 사용할 작업그룹의 크기입니다.
layout (local_size_x = 16, local_size_y = 16) in;

//descriptor bindings for the pipeline / 파이프라인을 위한 디스크립터 바인딩입니다.
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

We begin by specifying a default glsl version, 460 which maps to GLSL 4.6 will do just fine.
GLSL 버전을 사용하는 것으로 시작하겠습니다. 460은 GLSL 4.6버전에 대응됩니다.

Next we have the layout statement that defines the workgroup size. As explained in the article before, when executing compute shaders, they will get executed in groups of N lanes/threads. We are specifying sizes x=16, y=16, z=1 (default). Which means that in the shader, each group will be of 16x16 lanes working together. 
다음은 작업그룹의 상태를 정의하는 레이아웃 상태를 설정합니다. 이전 글에서 설명했던 대로, 컴퓨트 셰이더를 실행할 때 N개의 라인을 쓰레드로 나눈 그룹을 실행합니다. 여기서는 x=16, y=16, z=1(기본값)으로 지정합니다. 이는 셰이더에서 각 그룹이 16 * 16라인 단위로 작업한다는 것을 의미합니다.

The next layout statement is for the shader input through descriptor sets. We are setting a single image2D as set 0 and binding 0 within that set. With vulkan, each descriptor *set* can have a number of bindings, which are the things bound by that set when you bind that set. So this is 1 set, at index 0, which will contain a single image at binding #0.
다음은 셰이더 입력을 위한 디스크립터 셋을 설정합니다. 단일 이미지2D를 디스크립터 셋 0에 바인딩 0번에 설정하고 있습니다. Vulkan에서는 각 디스크립터 *셋*이 여러 바인딩을 가질 수 있습니다. 이는 해당 디스크립터 셋을 바인딩할 때 디스크립터 셋에 의해 바인딩됩니다. 따라서 디스크립터 셋 1개를 사용하며 0번 인덱스에는 단일 이미지를 0번 바인딩에 포함합니다.

The shader code is a very simple dummy shader that will create a gradient from the coordinates of the global invocation ID. If local invocation ID is 0 on either X or Y, we will just default to black. This is going to create a grid that will directly display our shader workgroup invocations.
셰이더 코드는 global invocation ID의 좌표에 따라 그라디언트를 형성하는 매우 단순한 더미 셰이더입니다. 만약 local Invocation ID가 X Y에서 0이라면, 검은 색이 나올 것입니다. 이는 셰이더 작업그룹 호출을 직접적으로 표시할 그리드를 생성할 것입니다.

Any time you touch the shaders, make sure you compile the shaders target from the build, and if you add new files, you must re-configure the cmake. This process has to succeed with no errors, or the project will be missing the spirv files necesary to run this shaders on the gpu.
언제든 셰이더를 다룰 때 셰이더를 타겟으로 추가해 컴파일하는 것을 분명히 하세요. 새로운 파일을 추가하면 cmake를 재구성해야만 합니다. 이 과정은 성공해서 오류를 발생시키지 않거나, 프로젝트가 GPU에서 셰이더를 실행하기 위해 필요한 spirv파일을 놓칠 것입니다.

## 디스크립터 셋 레이아웃 설정하기

To build the compute pipeline, we need to create its layout. In this case, the layout will only contain a single descriptor set that then has a image as its binding 0. 
컴퓨트 파이프라인을 구성하기 위해 레이아웃을 구성해야 합니다. 이 경우, 레이아웃은 바인딩 0번에 이미지를 갖는 단일 디스크립터 셋 만을 담고있습니다.

To build a descriptor layout, we need to store an array of bindings. Lets create a structure to abstract this so that handling those is simpler. Our descriptor abstractions will go into vk_descriptors.h/cpp
디스크립터 셋 레이아웃을 구성하기 위해, 바인딩의 배열을 저장해야 합니다. 이를 추상화하는 구조체를 추가하여 조금 더 단순하게 다뤄보도록 합시다. 우리의 디스크립터 추상화는 vk_descriptors.h/cpp에 작성하겠습니다.

<!-- codegen from tag descriptor_layout on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_descriptors.h --> 
```cpp
struct DescriptorLayoutBuilder {

    std::vector<VkDescriptorSetLayoutBinding> bindings;

    void add_binding(uint32_t binding, VkDescriptorType type);
    void clear();
    VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shaderStages, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
};
```

We will be storing `VkDescriptorSetLayoutBinding`, a configuration/info struct, into an array, and then have a build() function which creates the `VkDescriptorSetLayout`, which is a vulkan object, not a info/config structure.
`VkDescriptorSetLayoutBinding`, config/info 구조체를 배열로 담을 것입니다. 그리고 info/config 구조체가 아니라 Vulkan 객체인 `VkDescriptorSetLayout`을 만드는 build()함수를 만들겠습니다.

Lets write the functions for that builder
Builder를 위한 함수를 작성하겠습니다.

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

First, we have the add_binding function. This will just write a `VkDescriptorSetLayoutBinding` and push it into the array.
When creating a layout binding, for now we only need to know the binding number and the descriptor type. With the example of the compute shader above, thats binding = 0, and type is `VK_DESCRIPTOR_TYPE_STORAGE_IMAGE`, which is a writeable image.
먼저, dad_binding 함수를 작성합니다. 이는 `VkDescriptorSetLayoutBinding`을 작성해 바열로 추가합니다. 레이아웃 바인딩을 생성할 때 지금은 바인딩 숫자와 디스크립터 타입만을 알면 됩니다. 위의서의 컴퓨트 셰이더 예제에서는 바인딩 0번에 타입은 `VK_DESCRIPTOR_TYPE_STORAGE_IMAGE`입니다. 이는 쓰기 가능한 이미지를 의미합니다.

Next is creating the layout itself
다음은 레이아웃을 생성해봅시다.

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

We are first looping through the bindings and adding the stage-flags. For each of the descriptor bindings within a descriptor set, they can be different between the fragment shader and vertex shader. We wont be supporting per-binding stage flags, and instead force it to be for the whole descriptor set.
먼저 바인딩을 순회하며 실행단계 플래그를 추가합니다. 디스크립터 셋 안의 각 디스크립터 바인딩은 프래그먼트 셰이더와 정점 셰이더 간 다를 수 있습니다. 바인딩당 실행 단계 플래그를 지원하지는 않을 것이므로 전체 디스크립ㅌ ㅓ셋에 같은 실행 단계를 설정합니다.

Next we need to build the `VkDescriptorSetLayoutCreateInfo`, which we dont do much with, just hook it into the array of descriptor bindings. We then call `vkCreateDescriptorSetLayout` and return the set layout.
다음은 `VkDescriptorSetLayoutCreateInfo`를 구성하겠습니다. 설정할 것이 많지는 않은데, 디스크립터 바인딩의 배열을 연결하기만 할 것입니다. 그 후 `vkCreateDescriptorSetLayout`을 호출해 디스크립터 셋 레이아웃을 얻을 것입니다.

# 디스크립터 할당자

With the layout, we can now allocate the descriptor sets. Lets also write a allocator struct that will abstract it so that we can keep using it through the codebase.
디스크립터 셋 레이아웃이 구현되었으니 디스크립터 셋을 할당할 수 있습니다. 마찬가지로 할당자 구조체를 작성해 이후 코드를 간결히 사용할 수 있도록 추상화합시다.
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

Descriptor allocation happens through `VkDescriptorPool`. Those are objects that need to be pre-initialized with some size and types of descriptors for it. Think of it like a memory allocator for some specific descriptors. Its possible to have 1 very big descriptor pool that handles the entire engine, but that means we need to know what descriptors we will be using for everything ahead of time. That can be very tricky to do at scale. Instead, we will keep it simpler, and we will have multiple descriptor pools for different parts of the project, and try to be more accurate with them.
디스크립터 할당은 `VkDescriptorPool`을 통해 일어납니다. 이는 디스크립터 셋의 크기와 타입을 사전초기화할 필요가 있는 객체입니다. 몇몇 특수 디스크립터 셋의 메모리 할당자라고 생각해도 무방합니다. 매우 큰 1개의 디스크립터 셋이 전체 엔진을 관리할 수도 있지만, 이는 사용할 모든 디스크립터를 사전에 알아야된다는 것을 의미합니다. 이는 꽤 복잡하고 어려울 수 있습니다. 대신 조금 더 단순하게 구성하겠습니다. 프로젝트의 다양한 부분을 위해 여러 디스크립터 풀을 할당하고 더 정확히 하겠습니다.

One very important thing to do with pools is that when you reset a pool, it destroys all of the descriptor sets allocated from it. This is very useful for things like per-frame descriptors. That way we can have descriptors that are used just for one frame, allocated dynamically, and then before we start the frame we completely delete all of them in one go. This is confirmed to be a fast path by GPU vendors, and recommended to use when you need to handle per-frame descriptor sets. 
디스크립터 풀 관련해서 한 가지 매우 중요한 점은 풀을 리셋할 때 풀로부터 할당된 모든 디스크립터 셋이 파괴된다는 점입니다. 이는 프레임별 디스크립터 셋을 구성할 때 매우 유용합니다. 이 방법으로 한 프레임에만 사용되며 동적으로 할당되는 디스크립터 셋을 구성할 수 있습니다. 그리고 프레임을 시작하기 전 이를 완전히 해제할 수 있습니다. 이는 GPU 제조사에 의해 가장 빠른 경로로 확인되었으며, 프레임별 디스크립터 셋을 구성할 때 사용할 것을 권장합니다.

The DescriptorAllocator we have just declared has functions to initialize a pool, clear the pool, and allocate a descriptor set from it.
DescriptorAllocator에는 디스크립터 풀 초기화, 해제, 그리고 디스크립터 셋을 할당하는 수만을 선언하겟습니다.

Lets write the code now
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

We add creation and destruction functions. The clear function is not a delete, but a reset. It will destroy all of the descriptors created from the pool and put it back to initial state, but wont delete the VkDescriptorPool itself.
생성과 파괴 함수를 추가했습니다. clear 함수는 파괴가 아니라 리셋일 뿐입니다. 풀로부터 생성된 모든 디스크립터 셋을 파괴하고 초기 상태로 되돌리지만, VkDescriptorPool 그 자체를 파괴하지는 않습니다.

To initialize a pool, we use `vkCreateDescriptorPool` and give it an array of PoolSizeRatio. Thats a structure that contains a type of descriptor (same VkDescriptorType as on the bindings above ), alongside a ratio to multiply the maxSets parameter is. This lets us directly control how big the pool is going to be. 
maxSets controls how many VkDescriptorSets we can create from the pool in total, and the pool sizes give how many individual bindings of a given type are owned.
디스크립터 풀을 초기화하기 위해 `vkCreateDescriptorPool`을 사용합니다. 해당 함수에 PoolSizeRatio의 배열을 전달합니다. 이는 maxSets 파라미터를 곱한 비율에 따라 디스크립터의 타입(위에서 바인딩에 설정한 VkDescriptorType과 같습니다.) 을 담는 구조체입니다. 이는 디스크립터 풀을 얼마나 크게 할지를 직접적으로 제어할 수 있게 합니다. maxSets은 얼마나 많은 VkDescriptorSets이 생성될 수 있을 지를 나타내고, poolSizes는 주어진 타입에 얼마나 많은 개별 바인딩ㅇ ㅣ있을지를 나타냅니다.

Now we need the last function, `DescriptorAllocator::allocate`. Here it is.
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

We need to fill the `VkDescriptorSetAllocateInfo`. It needs the descriptor pool we will allocate from, how many descriptor sets to allocate, and the set layout. 
`VkDescriptorSetAllocateInfo`를 채울 필요가 잇습니다. 이는 할당받을 디스크립터 풀과 얼마나 많은 디스크립터 셋을 할당할 지, 그리고 디스크립터 셋레이아웃을 요구합니다.

# 디스크립터 셋과 레이아웃 초기화하기
Lets add a new function to VulkanEngine and some new members we will use.
VulkanEngine에 새로운 함수와 사용할 멤버를 추가합시다.

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

We will be storing one of those descriptor allocators in our engine as the global allocator. Then we need to store the descriptor set that will bind our render image, and the descriptor layout for that type of descriptor, which we will need later for creating the pipeline.
전역 할당기로서 엔진의 이러한 DescriptorAllocator 중 하나를 담을 것입니다. 그 후 디스크립터 셋에 렌더링할 이미지, 이후 파이프라인 생성에 사용할 디스크립터 셋 레이아웃을 담아야 합니다.

Remember to add the `init_descriptors()` function to the init() function of the engine, after sync_structures.
`init_descriptors()` 함수를 VulkanEngine의 `init()`함수에 sync_structures 이후 추가하는 것을 잊지 마세요.


```cpp
void VulkanEngine::init()
{
	//중략

	init_commands();

	init_sync_structures();

	init_descriptors();	

	//everything went fine
	_isInitialized = true;
}
```

We can now begin writing the function
이제 함수를 작성할 수 있습니다.
```cpp
void VulkanEngine::init_descriptors()
{
	//create a descriptor pool that will hold 10 sets with 1 image each / 각각이 1개의 이미지를 담는 10개의 디스크립터 셋을 담는 디스크립터 풀을 생성합니다.
	std::vector<DescriptorAllocator::PoolSizeRatio> sizes =
	{
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }
	};

	globalDescriptorAllocator.init_pool(_device, 10, sizes);

	//make the descriptor set layout for our compute draw / 컴퓨트 계싼에 사용할 디스크립터 셋 레이아웃을 생성합니다.
	{
		DescriptorLayoutBuilder builder;
		builder.add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		_drawImageDescriptorLayout = builder.build(_device, VK_SHADER_STAGE_COMPUTE_BIT);
	}
}
```

We will first initialize the descriptor allocator with 10 sets, and we 1 descriptor per set of type `VK_DESCRIPTOR_TYPE_STORAGE_IMAGE`. Thats the type used for a image that can be written to from a compute shader.
먼저 DescriptorAllocator를 10개의 디스크립터 셋과 각 디스크립터 셋을 `VK_DESCRIPTOR_TYPE_STORAGE_IMAGE`로 초기화합니다. 이는 컴퓨트 셰이더에서 작성할 수 있는 이미지에 사용되는 타입입니다.

Then, we use the layout builder to build the descriptor set layout we need, which is a layout with only 1 single binding at binding number 0, of type `VK_DESCRIPTOR_TYPE_STORAGE_IMAGE` too (matching the pool). 
그 후, 레이아웃 빌더를 사용해 필요한 디스크립터 셋 레이아웃을 구성할 수 있습니다. 이는 바인딩 0번에 `VK_DESCRIPTOR_TYPE_STORAGE_IMAGE`를 나타내는 레이아웃입니다.(디스크립터 풀과 일치시킵니다.)

With this, we would be able to allocate up to 10 descriptors of this type for use with compute drawing.
이를 통해, 컴퓨트 그리기에 사용할 이러한 타입의 10개 디스크립터 셋을 할당할 수 있습니다.

We continue the function by allocating one of them, and writing it so that it points to our draw image.
함수를 계속해 그 중 하나를 할당하고 작성하여 우리가 그릴 이미지를 가리키도록 합니다.

```cpp
void VulkanEngine::init_descriptors()
{
	// 중략
	//allocate a descriptor set for our draw image / 그릴 이미지에 사용할 디스크립터 셋을 할당합니다.
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

	//make sure both the descriptor allocator and the new layout get cleaned up properly / DescriptorAllocator와 새로운 디스크립터 셋 레이아웃이 올바르게 정리되도록 합니다.
	_mainDeletionQueue.push_function([&]() {
		globalDescriptorAllocator.destroy_pool(_device);

		vkDestroyDescriptorSetLayout(_device, _drawImageDescriptorLayout, nullptr);
	});
}
```

First we allocate a descriptor set object with the help of the allocator, of layout _drawImageDescriptorLayout which we created above. Then we need to update that descriptor set with our draw image. To do that, we need to use the `vkUpdateDescriptorSets` function. This function takes an array of `VkWriteDescriptorSet` which are the individual updates to perform.
We create a single write, which points to binding 0 on the set we just allocated, and it has the correct type. It also points to a `VkDescriptorImageInfo` holding the actual image data we want to bind, which is going to be the imageview for our draw image.
먼저 이전에 생성한 DescriptorAllocator, _drawImageDescriptorLayout을 통해 디스크립터 셋 객체를 할당합니다. 그 후 디스크립터 셋을 그릴 이미지와 연결해주어야 합니다. 이를 위해 `vkUpdateDescriptorSets` 함수를 사용합니다. 이 함수는 `VkWriteDescriptorSets` 배열을 받아 개별 업데이트를 수행합니다. 할당한 디스크립터 셋의 바인딩 0번을 가리키는 단일 write를 생성하며 올바른 타입을 갖습니다. 이는 또한 바인딩할 실제 이미지 데이터를 담는 `VkDescriptorImageInfo`를 가리킵니다. 이는 그릴 이미지의 이미지 뷰일 것입니다.

With this done, we now have a descriptor set we can use to bind our draw image, and the layout we needed. We can finally proceed to creating the compute pipeline
이것이 수행되면 그릴 이미지에 바인딩할 디스크립터 셋과 필요한 레이아웃이 만들어졌습니다. 마침내 컴퓨트 파이프라인을 생성할 수 있습니다.

## 컴퓨트 파이프라인

With the descriptor set layout, we now have a way of creating the pipeline layout. There is one last thing we have to do before creating the pipeline, which is to load the shader code to the driver.
In vulkan pipelines, to set shaders you need to build a `VkShaderModule`. We are going to add a function to load those as part of vk_pipelines.h/cpp
디스크립터 셋 레이아웃으로 파이프라인 레이아웃을 생성할 수단을 얻었습니다. 파이프라인을 생성하기 전에 해야 할 한가지 작업이 남았는데, 셰이더 코드를 불러와 드라이버로 연결하는 것입니다. Vulkan 파이프라인에서는 `VkShaderModule`을 통해 셰이더를 설정할 수 있습니다. 이를 불러오는 함수를 vk_pipelines.h/cpp에 추가합시다.

Add these includes to vk_pipelines.cpp
이를 vk_pipelines.cpp에 추가합니다.
```cpp
#include <vk_pipelines.h>
#include <fstream>
#include <vk_initializers.h>
```

Now add this function. Add it also to the header.
<!-- codegen from tag load_shader on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_pipelines.cpp --> 
```cpp
bool vkutil::load_shader_module(const char* filePath,
    VkDevice device,
    VkShaderModule* outShaderModule)
{
    // open the file. With cursor at the end / 파일을 읽어 커서를 끝으로 위치시킵니다.
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    // find what the size of the file is by looking up the location of the cursor / 커서의 위치를 찾아 파일의 크기를 계산합니다.
    // because the cursor is at the end, it gives the size directly in bytes / 커서가 맨 끝에 있으므로 바이트로 바로 계산할 수 있습니다.
    size_t fileSize = (size_t)file.tellg();

    // spirv expects the buffer to be on uint32, so make sure to reserve a int / spirv는 버퍼가 uin32일 것이라 가정하므로 정수를 보존합니다.
    // vector big enough for the entire file / 전체 파일을 담기에 벡터면 충분히 큽니다.
    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

    // put file cursor at beginning / 커서를 파일 앞으로 위치시킵니다.
    file.seekg(0);

    // load the entire file into the buffer / 버퍼로 전체 파일을 불러옵니다.
    file.read((char*)buffer.data(), fileSize);

    // now that the file is loaded into the buffer, we can close it / 파일을 버퍼로 옮겼으므로 닫을 수 있습니다.
    file.close();

    // create a new shader module, using the buffer we loaded / 불러온 버퍼를 사용해 셰이더 모듈을 생성합니다.
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;

    // codeSize has to be in bytes, so multply the ints in the buffer by size of / 코드 크기는 바이트 단위이므로 실제 버퍼 크기를 알기 위해 정수 크기를 곱합니다.
    // int to know the real size of the buffer 
    createInfo.codeSize = buffer.size() * sizeof(uint32_t);
    createInfo.pCode = buffer.data();

    // check that the creation goes well. / 생성이 올바르게 수행되었는지 확인합니다.
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        return false;
    }
    *outShaderModule = shaderModule;
    return true;
}
```

With this function, we first load the file into a `std::vector<uint32_t>`. This will store the compiled shader data, which we can then use on the call to `vkCreateShaderModule`. The create-info for a shader module needs nothing except the shader data as an int array. Shader modules are only needed when building a pipeline, and once the pipeline is built they can be safely destroyed, so we wont be storing them in the VulkanEngine class.
이 함수를 통해 먼저 파일을 `std::vector<uint32_t>`로 불러옵니다. 이는 컴파일된 셰이더 데이터를 담을 것이며, `vkCreateShaderModule`을 사용할 수 있습니다. 셰이더 모듈을 위한 createInfo는 셰이더 데이터를 담는 배열 외에는 아무것도 필요하지 않습니다. 셰이더 모듈은 파이프라인을 구성할 때에만 필요하며, 파이프라인이 안전하게 파괴되므로 이를 VulkanEngine에 담을 필요는 없습니다.

Back to VulkanEngine, lets add the new members we will need, and the init_pipelines() function alongside a init_background_pipelines() function. init_pipelines() will call the other pipeline initialization functions that we will add as the tutorial progresses.
VulkanEngine으로 돌아와 필요한 새로운 멤버와 init_pipelines()함수를 추가합시다.init_pipelines()는 튜토리얼이 진행되며 추가되는 다른 파이프라인 초기화 함수도 호출할 것입니다.

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

Add it  to the init function, and add the vk_pipelines.h include to the top of the file. The `init_pipelines()` function will call `init_background_pipelines()`
이를 함수에 추가하고 vk_pipelines.h를 상단에 포함시킵니다. `init_pipelines()` 함수가 `init_background_pipelines()`를 호출할 것입니다.

```cpp
#include <vk_pipelines.h>

void VulkanEngine::init()
{
	//중략

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

Lets now begin creating the pipeline. The first thing we will do is create the pipeline layout.
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

To create a pipeline, we need an array of descriptor set layouts to use, and other configuration such as push-constants. On this shader we wont need those so we can skip them, leaving only the DescriptorSetLayout.
파이프라인을 생성하기 위해 사용할 디스크립터 셋 레이아웃의 배열과 푸시상수와 같은 다른 구성이 필요합니다. 이 셰이더에서는 이러한 과정을 생략하고 디스크립터 셋 레이아웃만이 필요합니다.

Now, we are going to create the pipeline object itself by loading the shader module and adding it plus other options into a VkComputePipelineCreateInfo.
이제 파이프라인 객체 그 자체를 셰이더 모듈을 가져오고 다른 옵션들을 VkComputePipelineCreateInfo에 추가함으로서 생성합니다.
```cpp
void VulkanEngine::init_background_pipelines()
{
	//layout code / 레이아웃 코드
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

First we load the computeDrawShader VkShaderModule by using the function we created just now. We will check for error as its very common for it to fail if the file is wrong. Keep in mind that the paths here are configured to work for the default windows + msvc build folders. If you are using any other option, check that the file paths are correct. Consider abstracting the path yourself to work from a folder set in a config file.
먼저 생성한 함수를 사용해 VkShaderModule을 불러옵니다. 파일에 오류가 있는지 확인합니다. 여기서 경로는 기본 창 + msvc 빌드 폴더를 기준으로 작업함을 유의하세요. 만약 다른 옵션을 사용한다면 파일 경로가 제대로 되었는지 확인하세요. 구성 파일에 설정된 폴더에서 작업할 경로를 직접 추상화하는 것을 고려해보세요.

Then, we need to connect the shader into a `VkPipelineShaderStageCreateInfo`. The thing to note here is that we are giving it the name of the function we want the shader to use, which is going to be main(). It is possible to store multiple compute shader variants on the same shader file, by using different entry point functions and then setting them up here.
그러면 셰이더를 `VkPipelineShaderStageCreateInfo`로 연결합니다. 여기서 유의할 점은 사용할 셰이더의 함수의 이름을 전달한다는 점입니다. 여기서는 main()입니다. 이는 같은 셰이더 파일 안에 다른 진입점을 설정함으로써 여러 컴퓨트 셰이더를 담을 수 있습니다.

Last, we fill the `VkComputePipelineCreateInfo`. We will need the stage info for the compute shader, and the layout. We can  then call `vkCreateComputePipelines`. 
마지막으로 `VkComputePipelineCreateInfo`를 채웁니다. 이는 컴퓨트 셰이더의 단계 정보와 레이아웃이 필요합니다. 그 후 `vkCreateComputePipelines`를 호출할 수 있습니다.

At the end of the function, we will do proper cleanup of the structures so that they get deleted at the end of the program through the deletion queue.
함수의 끝에서 구조체를 제대로 정리함으로써 프로그램의 끝에서 삭제 큐를 통해 삭제되도록 합니다.

<!-- codegen from tag comp_pipeline_3 on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
	vkDestroyShaderModule(_device, computeDrawShader, nullptr);

	_mainDeletionQueue.push_function([&]() {
		vkDestroyPipelineLayout(_device, _gradientPipelineLayout, nullptr);
		vkDestroyPipeline(_device, _gradientPipeline, nullptr);
		});
```

We can destroy the shader module directly on the function, we created the pipeline so we have no need for it anymore. With the pipeline and its layout, we need to wait until the end of the program.
셰이더 모듈을 함수에서 바로 파괴할 수 있습니다. 파이프라인을 생성했으므로 더이상 필요가 없기 때문입니다. 파이프라인과 파이프라인 레이아웃은 필요하므로 프로그램이 끝난 후 파괴합니다.

We are now ready to draw with it.
이제 그릴 준비가 끝났습니다.

# Drawing with compute / 컴퓨트 셰이더로 그리기

Go back to the draw_background() function, we will replace the vkCmdClear with a compute shader invocation.
draw_background()함수로 돌아와 vkCmdClear를 컴퓨트 셰이더 호출로 대체합니다.

<!-- codegen from tag draw_comp on file E:\ProgrammingProjects\vulkan-guide-2\chapter-2/vk_engine.cpp --> 
```cpp
	// bind the gradient drawing compute pipeline
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _gradientPipeline);

	// bind the descriptor set containing the draw image for the compute pipeline
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _gradientPipelineLayout, 0, 1, &_drawImageDescriptors, 0, nullptr);

	// execute the compute pipeline dispatch. We are using 16x16 workgroup size so we need to divide by it
	vkCmdDispatch(cmd, std::ceil(_drawExtent.width / 16.0), std::ceil(_drawExtent.height / 16.0), 1);
```

First we need to bind the pipeline using `vkCmdBindPipeline`. As the pipeline is a compute shader, we use `VK_PIPELINE_BIND_POINT_COMPUTE`. Then, we need to bind the descriptor set that holds the draw image so that the shader can access it.
Finally, we use `vkCmdDispatch` to launch the compute shader. We will decide how many invocations of the shader to launch (remember, the shader is 16x16 per workgroup) by dividing the resolution of our draw image by 16, and rounding up.
먼저 `vkCmdBindPipeline`을 사용하여 파이프라인을 바인딩합니다. 컴퓨트 셰이더를 사용하기 때문에 `VK_PIPELINE_BIND_POINT_COMPUTE`를 사용합니다. 그 후, 그릴 이미지를 담고있는 디스크립터 셋을 바인딩하여 셰이더가 접근할 수 있도록 합니다. 마지막으로 `vkCmdDispatch`를 사용하여 컴퓨트 셰이더를 시작합니다. 얼마나 많은 셰이더 호출이 일어날지를 그릴 이미지의 해상도를 16으로 나누고 반올림함으로써 결정해야합니다.(여기서는 작업 그룹당 16 * 16입니다.)

If you run the program at this point, it should display this image. If you have an error when loading the shader, make sure you have the shaders built by re-running CMake, and rebuilding the Shaders target. This target will not build automatically when building engine, so you must rebuild it every time shaders change.
이 시점에서 프로그램을 실행하면 이미지가 표시될 것입니다. 만약 셰이더를 불러오는 과정에서 오류가 발생했다면 CMake를 재실행하고 셰이더 타겟을 재설정하여 셰이더를 불러오세요. 이는 엔진을 구성할 때 자동으로 구성되지 않기 때문에 셰이더가 바뀔 때 마다 재빌드해야합니다.

![chapter2]({{site.baseurl}}/diagrams/compute_grid.png)

다음 글 : [ IMGUI 설정하기]({{ site.baseurl }}{% link docs/new_chapter_2/vulkan_imgui_setup.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}
