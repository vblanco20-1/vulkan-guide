---
layout: default
title: Mesh buffers
parent: "3. Graphics Pipelines"
nav_order: 10
---

객체를 제대로 렌더링하려면 위해 정점 데이터를 정점 셰이더로 전달해야 합니다. 지금은 하드코딩된 배열을 사용하고 있기 때문에 단일 삼각형 혹은 단순한 도형외에는 제대로 동작하지 않습니다.

파이프라인에서 고정 함수 방식의 정점 속성 가져오기(fetch) 로직을 사용하지 않기 때문에, 정점 데이터를 셰이더에서 어떻게 정확히 불러올지는 전적으로 자유입니다. 우리는 정점들을 Buffer Device Address를 통해 전달된 대용량 GPU 버퍼로부터 불러올 것입니다. 이는 고성능과 높은 유연성을 부여합니다.

## Vulkan 버퍼
Vulkan에서는 범용 목적의 메모리를 버퍼를 통해 할당할 수 있습니다. 샘플러를 요구하지 않고 일반적인 CPU측의 구조체 혹은 배열처럼 동작하는 면에서 버퍼는 이미지와는 다릅니다. 구조체 혹은 구조체 배열로 이를 셰이더에서 접근할 수 있습니다.

버퍼가 생성될 때 용도에 맞는 Usage 플래그를 설정해야 합니다. 

셰이더에는 범용 읽기/쓰기 연산을 위한 두 종류의 버퍼가 있습니다. 유니폼 버퍼와 스토리지 버퍼입니다.

유니폼 버퍼는 셰이더에서 읽기 전용으로 접근할 수 있으며, 접근 가능한 크기는 제조사마다 다르지만 최소 16KB가 보장됩니다. 반면 파이프라인을 불러올 때 GPU가 사전 캐싱 할 수 있기 때문에 가장 빠른 접근 속도를 제공합니다. 이 크기 제한은 셰이더에 바인딩된 부분에만 적용됩니다. 따라서 하나의 커다란 유니폼 버퍼를 만들어 일부만 셰이더로 바인딩하는 것도 가능합니다. 하드웨어에 따라 푸시상수가 드라이버에 의해 유니폼 버퍼의 한 형태로 구현될 수 있습니다. 

스토리지 버퍼(SSBO)는 대용량의 읽기 쓰기 가능한 버퍼입니다. 최소 크기는 128MB이며, 이 튜토리얼에서 타게팅하는 현대 PC의 GPU는 4GB를 지원합니다. 이 크기는 uint32 크기로 표현할 수 있는 최대 값에 기반한 제한입니다. 스토리지 버퍼는 유니폼 버퍼처럼 사전 로딩되지 않으며 보다 일반적인 형태의 데이터 불러오기/저장(load/store)을 위한 용도로 사용됩니다.

유니폼 버퍼는 크기가 작기 때문에 정점 도형 데이터를 담기에는 적합하지 않습니다. 하지만 머테리얼 인자와 전역 씬 구성을 전달하는 데에는 매우 효과적입니다.

유니폼 버퍼와 스토리지 버퍼의 정확한 속도 차이는 GPU와 셰이더의 작업 내용에 따라 다릅니다. 따라서 대부분의 경우 높은 유연성을 위해 스토리지를 사용하는 것이 보편적입니다. 성능 차이가 프로젝트에 실질적인 영향을 주지 않는 경우가 많기 때문입니다.

이 벤치마크에서 버퍼에 접근하는 다양한 방법을 비교할 수 있습니다.
https://github.com/sebbbi/perftest . 

디스크립터를 생성할 때 동적 버퍼로 설정하는 것도 가능합니다. 동적 버퍼를 사용한다면 명령을 기록할 때 버퍼가 바인딩되는 오프셋을 제어할 수 있습니다. 이 방식을 활용하면 하나의 커다란 버퍼에 여러 오브젝트의 유니폼 데이터를 담고 디스크립터 셋 하나로 다양한 오프셋을 바인딩하여 여러 오브젝트를 렌더링할 수 있습니다. 이는 유니폼 버퍼와 잘 어울리지만 스토리지 버퍼에는 디바이스 어드레스를 쓰는 편이 적합합니다.

## Buffer Device Adress
일반적으로 버퍼는 디스크립터 셋을 통해 바인딩해야 하며, 이는 CPU에서 특정 버퍼의 크기나 레이아웃을 미리 알고 있어야 하고, 디스크립터 셋의 수명 관리도 함께 고려해야 한다는 의미입니다. 하지만 이 프로젝트에서는 Vulkan 1.3을 대상으로 하기 때문에 버퍼 디바이스 어드레스라는 다른 접근 방식을 활용할 수 있습니다. 이는 int64포인터를 GPU로 전달하여 셰이더에서 접근할 수 있게 하며 포인터 연산도 가능합니다. C++ 포인터와 동일한 매커니즘이며 링크드 리스트나 간접(indirect) 접근과 같은 구조도 구현이 가능합니다.

이를 정점 데이터 전달에사용할 예정입니다. 디바이스 어드레스를 통한 SSBO 접근은 디스크립터 셋을 통한 접근보다 더 빠르며, 정점 데이터를 셰이더에 바인딩할 때 푸시상수를 통해 매우 빠르고 간단하게 전달할 수 있습니다.

## 버퍼 생성하기
메시를 GPU로 업로드 하는 코드를 작성하겠습니다. 먼저 버퍼를 생성할 방법이 필요합니다.

이를 vk_types.h에 추가하겠습니다.
```cpp
struct AllocatedBuffer {
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo info;
};
```

이 구조체는 주어진 버퍼와 관련된 데이터를 담을 때 사용할 것입니다. Vulkan 핸들인 `VkBuffer`와 VmaAllocation, 그리고 버퍼를 해제할 때 필요한 버퍼와 버퍼 할당에 관한 메타데이터를 담는 VmaAllocationInfo를 포함합니다.

VulkanEngine에 이를 생성하기 위한 함수를 추가합니다.

```cpp
AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
```

할당 크기와 usage 플래그, 그리고 VmaMemoryUsage 플래그를 인자로 받아 버퍼 메모리가 어디에 할당될 지를 제어할 수 있도록 합니다

아래는 구현부입니다.

<!-- codegen from tag alloc_buffer on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_engine.cpp --> 
```cpp
AllocatedBuffer VulkanEngine::create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
{
	// allocate buffer
	VkBufferCreateInfo bufferInfo = {.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
	bufferInfo.pNext = nullptr;
	bufferInfo.size = allocSize;

	bufferInfo.usage = usage;

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = memoryUsage;
	vmaallocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
	AllocatedBuffer newBuffer;

	// allocate the buffer
	VK_CHECK(vmaCreateBuffer(_allocator, &bufferInfo, &vmaallocInfo, &newBuffer.buffer, &newBuffer.allocation,
		&newBuffer.info));

	return newBuffer;
}
```

먼저 vVkBuffeCreateInfo 구조체를 채워야 합니다. 이 구조체에는 버퍼의 크기와 Usage 플래그를 지정합니다. 그 후 VMA가 사용할 속성들을 담는  AllocationCreateInfo를 생성합니다. 이때 `VmaMemoryUsage`플래그를 사용하여 VMA가 버퍼를 어떤 메모리 타입에 배치할 지 제어할 수 있습니다. 이미지의 경우 GPU에서 접근할 수 있는 가장 빠른 공간인 디바이스 전용 메모리에 할당했습니다. 하지만 버퍼에서는 CPU에서 직접 쓸수 있도록 만들 지 여부를 결정해야 합니다. 이는 주 사용 목적에 따라 달라집니다.

* `VMA_MEMORY_USAGE_GPU_ONLY`는 GPU 전용 메모리입니다. 이 메모리는 GPU VRAM 공간에 위치하므로 CPU에서 읽고 쓸 수 없지만, 셰이더에서 읽고 쓰기가 가장 빠릅니다.
* `VMA_MEMORY_USAGE_CPU_ONLY`는 CPU RAM에 있는 공간입니다. 이는 CPU에서 쓸 수 있지만 GPU는 읽기만 할 수 있습니다. 이것은 CPU RAM이 GPU 외부에 있어서 그런 것이라는 사실을 명심하세요. GPU에서 RAM에 접근할 때에는 성능 손실이 일어날 수 있습니다. 프레임마다 자주 변경되는 데이터나, 접근 속도가 크게 중요하지 않은 소량의 데이터에 유용합니다.
* `VMA_MEMORY_USAGE_CPU_TO_GPU`는 CPU에서 쓰기가 가능하고, GPU에서도 비교적 빠르게 접근할 수 있는 메모리입니다. Vulkan 1.2버전 이상에서는 GPU가 자체 VRAM 내에 CPU에서 쓰기 가능한 작은 영역을 제공하는데, 이 메모리가 여기에 해당합니다. Resizable BAR를 사용하지 않는 한 용량은 제한적이지만, CPU에서 직접 쓰고 GPU에서도 빠르게 접근할 수 있는 장점이 있습니다.
* `VMA_MEMORY_USAGE_GPU_TO_CPU`는 CPU에서 안전하게 읽어야 하는 데이터를 저장할 때 사용합니다.

`VMA_ALLOCATION_CREATE_MAPPED_BIT`을 모든 버퍼 할당에 사용하고 있습니다. 이는 VMA가 포인터를 자동으로 매핑하여 버퍼가 CPU에서 접근 가능한 경우 바로 메모리에 쓸 수 있습니다. VMA는 이 포인터를 allocationInfo에 저장합니다.

버퍼 생성 함수를 만들었으므로 파괴 함수도 필요합니다. 버퍼를 파괴할 때에는 vmaDestroyBuffer만 호출하면 됩니다.

```cpp
void VulkanEngine::destroy_buffer(const AllocatedBuffer& buffer)
{
    vmaDestroyBuffer(_allocator, buffer.buffer, buffer.allocation);
}
```


이를 통해 메시 구조체를 생성하고 정점 버퍼를 설정할 수 있습니다.

## GPU에서의 메시 버퍼

vk_types.h
<!-- codegen from tag vbuf_types on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_types.h --> 
```cpp
struct Vertex {

	glm::vec3 position;
	float uv_x;
	glm::vec3 normal;
	float uv_y;
	glm::vec4 color;
};

// holds the resources needed for a mesh
struct GPUMeshBuffers {

    AllocatedBuffer indexBuffer;
    AllocatedBuffer vertexBuffer;
    VkDeviceAddress vertexBufferAddress;
};

// push constants for our mesh object draws
struct GPUDrawPushConstants {
    glm::mat4 worldMatrix;
    VkDeviceAddress vertexBuffer;
};
```

우리는 정점 포맷이 하므로 여기서는 이 포맷을 사용하겠습니다. 정점 포맷을 정의할 때는 데이터를 가능한 한 압축하는 것이 매우 중요하지만, 현재 튜토리얼 단계에서는 성능에 큰 영향을 주지 않으므로 우선은 간단하게 진행하겠습니다. 이후 단계에서 이 정점 포맷을 최적화할 예정입니다. UV 변수가 중간에 삽입된 이유는 GPU의 정렬 제한 때문입니다. 이 구조체를 셰이더에서 사용하는 구조와 동일하게 맞추는 것이 중요하기 때문에, 이런 방식의 포맷이 성능이나 호환성 측면에서 더 바람직합니다.

메시 데이터를 GPUMeshBuffers 구조체에 담을 것입니다. 이 구조체는 정점과 인덱스 버퍼 모두를 포함하며, 정점 버퍼의 버퍼 디바이스 어드레스를 포함합니다.

메시를 렌더링할 때 필요한 푸시상수 구조체도 정의할 것이며, 이 구조체에는 객체의 변환 행렬과 메시 버퍼의 디바이스 어드레스를 담을 것입니다.

이러한 버퍼를 생성하고 GPU에 업로드하는 함수를 작성해봅시다.

```cpp
GPUMeshBuffers VulkanEngine::uploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices)
{
	const size_t vertexBufferSize = vertices.size() * sizeof(Vertex);
	const size_t indexBufferSize = indices.size() * sizeof(uint32_t);

	GPUMeshBuffers newSurface;

	//create vertex buffer
	newSurface.vertexBuffer = create_buffer(vertexBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY);

	//find the adress of the vertex buffer
	VkBufferDeviceAddressInfo deviceAdressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,.buffer = newSurface.vertexBuffer.buffer };
	newSurface.vertexBufferAddress = vkGetBufferDeviceAddress(_device, &deviceAdressInfo);

	//create index buffer
	newSurface.indexBuffer = create_buffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY);

}
```
이를 VulkanEngine 클래스 선언에 추가합니다.

이 함수는 정점 데이터를 위한 Vertex 타입의 std::span과 인덱스를 위한 int 타입의 std::span을 인자로 받습니다. std::span은 포인터와 크기로 구성된 구조로, C 스타일의 배열이나 std::vector에서 변환할 수도 있습니다. 이렇게 하면 불필요한 데이터 복사를 방지할 수 있습니다. 

먼저 필요한 버퍼가 얼마나 큰지 계산하고, GPU 전용 메모리에 버퍼를 생성합니다.

 정점 버퍼에는 다음과 같은 Usage 플래그를 사용합니다. SSBO를 위해`VK_BUFFER_USAGE_STORAGE_BUFFER_BIT`를, 버퍼 디바이스 어드레스를 받아오기 위해 `VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT`를 사용했습니다.
 
 인덱스 버퍼에는 `VK_BUFFER_USAGE_INDEX_BUFFER_BIT`를 사용해 해당 버퍼를 인덱스 드로우 호출에 사용할 것이라고 Vulkan에게 알려줍니다.

 그리고 두 버퍼 모두에 `VK_BUFFER_USAGE_TRANSFER_DST_BIT`를 사용했는데 이는 메모리 복사 명령을 수행하기 위해서입니다.

 버퍼 디바이스 어드레스를 얻기 위해 `vkGetBufferDeviceAddress`를 호출해야 합니다.주소를 조회하고자 하는 VkBuffer를 전달해야 합니다. VkDeviceAddress를 얻고 나면, 포인터 연산도 가능하며, 이는 커다란 버퍼에 하위할당을 할 때 유용합니다.

 버퍼를 할당한 이후에는 버퍼에 데이터를 작성해야 합니다. 이를 위해 Vulkan에서 흔히 사용되는 방식인 스테이징 버퍼를 사용할 것입니다. GPU_ONLY 메모리는 CPU에서 직접 접근할 수 없기 때문에 먼저 CPU 쓰기 가능한 임시 스테이징 버퍼의 메모리에 작성하고, 이후 복사 명령을 통해 GPU 전용 버퍼로 복사합니다. mesh에 꼭 GPU_ONLY 메모리를 사용할 필요는 없지만, CPU에서 파티클 시스템이나 다른 동적 효과가 아닌 한 GPU_ONLY 버퍼를 권장합니다.

<!-- codegen from tag mesh_create_2 on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_engine.cpp --> 
```cpp
	AllocatedBuffer staging = create_buffer(vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	void* data = staging.allocation->GetMappedData();

	// copy vertex buffer
	memcpy(data, vertices.data(), vertexBufferSize);
	// copy index buffer
	memcpy((char*)data + vertexBufferSize, indices.data(), indexBufferSize);

	immediate_submit([&](VkCommandBuffer cmd) {
		VkBufferCopy vertexCopy{ 0 };
		vertexCopy.dstOffset = 0;
		vertexCopy.srcOffset = 0;
		vertexCopy.size = vertexBufferSize;

		vkCmdCopyBuffer(cmd, staging.buffer, newSurface.vertexBuffer.buffer, 1, &vertexCopy);

		VkBufferCopy indexCopy{ 0 };
		indexCopy.dstOffset = 0;
		indexCopy.srcOffset = vertexBufferSize;
		indexCopy.size = indexBufferSize;

		vkCmdCopyBuffer(cmd, staging.buffer, newSurface.indexBuffer.buffer, 1, &indexCopy);
	});

	destroy_buffer(staging);

	return newSurface;
```

먼저 스테이징 버퍼를 생성합니다. 이 버퍼 하나로 인덱스 버퍼와 정점 버퍼로의 복사를 모두 처리할 것입니다. 스테이징 버퍼의 메모리 타입은 CPU_ONLY 이며 Usage 플래그는 복사만 수행하기 때문에 `VK_BUFFER_USAGE_TRANSFER_SRC_BIT`입니다.

버퍼가 생성되면 GetMappedData()를 통해 매핑된 주소를 가져올 수 있습니다. 이는 void* 형태의 포인터로, 직접 데이터를 쓸 수 있습니다. memcpy를 두 번 사용하여 각 span의 데이터를 복사합니다.

스테이징 버퍼가 작성되면 `immediate_submit`을 실행해 GPU 측에서의 데이터를 복사하는 명령을 수행합니다. 이 명령은 2개의 VkCmdCopyBuffer 명령을 수행합니다. 이는 GPU에서 수행되는 memcpy역할입니다. 사용된 VkBufferCopy 구조체는 스테이징 버퍼에 데이터를 쓸 때 수행했던 memcpy를 그대로 반영하는 형태입니다.

immediate submit이 수행되면 안전하게 스테이징 버퍼를 해제할 수 있습니다.

GPU 명령을 완전히 실행될 때 까지 CPU 측이 대기해야 하기 때문에 이 패턴이 효율적만은 않다는 점을 명심하세요. 이러한 데이터 업로드는 일반적으로 백그라운드 쓰레드에서 수행하는 편이 좋습니다. 

## 메시 그리기
지금까지 만든 함수들을 사용해 메시를 생성하고 실제로 렌더링을 수행해 봅시다. 삼각형을 조합하여 인덱싱된 사각형을 그릴 것입니다.

정점 버퍼를 사용하게 되었으므로, 셰이더도 이에 맞게 수정해야 합니다. 프래그먼트 셰이더는 기존과 동일하게 `colored_triangle.frag`를 사용할 것이며, 정점 셰이더만 데이터를 푸시 상수에서 불러오도록 변경해야 합니다. 이 새로운 정점 셰이더는 기존의 하드코딩된 삼각형 셰이더와 유사하므로, `colored_triangle_mesh.vert`라는 이름으로 새로 생성하겠습니다.

<!-- codegen from tag all on file E:\ProgrammingProjects\vulkan-guide-2\shaders/colored_triangle_mesh.vert --> 
```cpp
#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outUV;

struct Vertex {

	vec3 position;
	float uv_x;
	vec3 normal;
	float uv_y;
	vec4 color;
}; 

layout(buffer_reference, std430) readonly buffer VertexBuffer{ 
	Vertex vertices[];
};

//push constants block
layout( push_constant ) uniform constants
{	
	mat4 render_matrix;
	VertexBuffer vertexBuffer;
} PushConstants;

void main() 
{	
	//load vertex data from device adress
	Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

	//output data
	gl_Position = PushConstants.render_matrix *vec4(v.position, 1.0f);
	outColor = v.color.xyz;
	outUV.x = v.uv_x;
	outUV.y = v.uv_y;
}
```

`GL_EXT_buffer_reference` 확장을 활성화해야 셰이더 컴파일러가 버퍼 레퍼런스를 올바르게 처리할 수 있습니다.

그 후 정점 구조체를 정의합니다. 이는 CPU에서의 구조체와 동일합니다.

이후 정점 버퍼를 선언합니다. 이 버퍼는 정점 구조체 (unsized) 배열을 갖는 읽기 전용 버퍼입니다. `layout`에 `buffer_reference`를 선언함으로서 셰이더에게 해당 구조체가 버퍼 주소 기반으로 사용됨을 알려줍니다. `std430`은 GPU에서 사용하는 메모리 정렬 규칙입니다.

푸시 상수 블럭에는 정점 셰이더의 단일 인스턴스와 행렬을 담고 있습니다. 정점 버퍼가 buffer_reference로 선언되었으므로 이는 실제로는 uint64 핸들이며, 행렬은 일반적인 행렬입니다.
 
main()에는 `gl_VertexIndex`를 사용해 정점 배열을 인덱싱합니다. 하드코딩된 배열에서 수행했던 것과 동일합니다. GLSL에서는 버퍼 참조도 일반 변수처럼 다루기 때문에 C++ 처럼 ->가 아니라 `.`을 사용해 멤버에 접근합니다. 정점을 얻은 후에는 해당 정점의 색상과 위치 정보를 추력하면 됩니다. 위치는 렌더링 행렬과 곱해져 변환됩니다.

이제 파이프라인을 생성하겠습니다. 기존의 `init_triangle_pipeline()`과는 별도로 파이프라인을 만들지만 구조는 거의 동일합니다. VulkanEngine에 추가합니다.

```cpp
VkPipelineLayout _meshPipelineLayout;
VkPipeline _meshPipeline;

GPUMeshBuffers rectangle;

void init_mesh_pipeline();
```

`init_triangle_pipeline()`의 거의 복사본이 될 것입니다.

<!-- codegen from tag rectangle_shaders on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_engine.cpp --> 
```cpp
	VkShaderModule triangleFragShader;
	if (!vkutil::load_shader_module("../../shaders/colored_triangle.frag.spv", _device, &triangleFragShader)) {
		fmt::print("Error when building the triangle fragment shader module");
	}
	else {
		fmt::print("Triangle fragment shader succesfully loaded");
	}

	VkShaderModule triangleVertexShader;
	if (!vkutil::load_shader_module("../../shaders/colored_triangle_mesh.vert.spv", _device, &triangleVertexShader)) {
		fmt::print("Error when building the triangle vertex shader module");
	}
	else {
		fmt::print("Triangle vertex shader succesfully loaded");
	}

	VkPushConstantRange bufferRange{};
	bufferRange.offset = 0;
	bufferRange.size = sizeof(GPUDrawPushConstants);
	bufferRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::pipeline_layout_create_info();
	pipeline_layout_info.pPushConstantRanges = &bufferRange;
	pipeline_layout_info.pushConstantRangeCount = 1;

	VK_CHECK(vkCreatePipelineLayout(_device, &pipeline_layout_info, nullptr, &_meshPipelineLayout));

```

정점 셰이더를 `colored_triangle_mesh.vert.spv`로 교체하고, 위에서 정의한 푸시 상수 구조체를 사용할 수 있도록 파이프라인 레이아웃을 수정합니다.

나머지 함수 내용은 triangle pipeline 함수와 동일하지만, 파이프라인 레이아웃과 파이프라인 객체 이름을 새로운 것으로 변경합니다.

<!-- codegen from tag rectangle_shaders2 on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_engine.cpp --> 
```cpp
	PipelineBuilder pipelineBuilder;

	//use the triangle layout we created
	pipelineBuilder._pipelineLayout = _meshPipelineLayout;
	//connecting the vertex and pixel shaders to the pipeline
	pipelineBuilder.set_shaders(triangleVertexShader, triangleFragShader);
	//it will draw triangles
	pipelineBuilder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	//filled triangles
	pipelineBuilder.set_polygon_mode(VK_POLYGON_MODE_FILL);
	//no backface culling
	pipelineBuilder.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
	//no multisampling
	pipelineBuilder.set_multisampling_none();
	//no blending
	pipelineBuilder.disable_blending();

	pipelineBuilder.disable_depthtest();

	//connect the image format we will draw into, from draw image
	pipelineBuilder.set_color_attachment_format(_drawImage.imageFormat);
	pipelineBuilder.set_depth_format(VK_FORMAT_UNDEFINED);

	//finally build the pipeline
	_meshPipeline = pipelineBuilder.build_pipeline(_device);

	//clean structures
	vkDestroyShaderModule(_device, triangleFragShader, nullptr);
	vkDestroyShaderModule(_device, triangleVertexShader, nullptr);

	_mainDeletionQueue.push_function([&]() {
		vkDestroyPipelineLayout(_device, _meshPipelineLayout, nullptr);
		vkDestroyPipeline(_device, _meshPipeline, nullptr);
	});
```

이제 `init_pipelines()`함수에서 이를 호출합니다.

```cpp
void VulkanEngine::init_pipelines()
{
	//COMPUTE PIPELINES	
	init_background_pipelines();

	// GRAPHICS PIPELINES
	init_triangle_pipeline();
	init_mesh_pipeline();
}
```

다음은 메시를 생성하고 업로드 하는 것입니다. 엔진의 기본 데이터를 담을 새로운 초기화 함수 `init_default_data()`를 작성합니다. 이를 init 함수의 마지막에 추가합니다.

```cpp
void VulkanEngine::init_default_data() {
	std::array<Vertex,4> rect_vertices;

	rect_vertices[0].position = {0.5,-0.5, 0};
	rect_vertices[1].position = {0.5,0.5, 0};
	rect_vertices[2].position = {-0.5,-0.5, 0};
	rect_vertices[3].position = {-0.5,0.5, 0};

	rect_vertices[0].color = {0,0, 0,1};
	rect_vertices[1].color = { 0.5,0.5,0.5 ,1};
	rect_vertices[2].color = { 1,0, 0,1 };
	rect_vertices[3].color = { 0,1, 0,1 };

	std::array<uint32_t,6> rect_indices;

	rect_indices[0] = 0;
	rect_indices[1] = 1;
	rect_indices[2] = 2;

	rect_indices[3] = 2;
	rect_indices[4] = 1;
	rect_indices[5] = 3;

	rectangle = uploadMesh(rect_indices,rect_vertices);

	//delete the rectangle data on engine shutdown
	_mainDeletionQueue.push_function([&](){
		destroy_buffer(rectangle.indexBuffer);
		destroy_buffer(rectangle.vertexBuffer);
	});

}
```
정점과 인덱스를 위한 2개의 배열을 만들고 uploadMesh 함수를 호출하여 버퍼로 업로드합니다.

이제 렌더링을 수행할 수 있습니다. `draw_geometry()` 함수에서 기존의 삼각형을 그린 뒤, 그 아래에 새로운 그리기 명령을 추가합니다.

```cpp
	//launch a draw command to draw 3 vertices
	vkCmdDraw(cmd, 3, 1, 0, 0);

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _meshPipeline);

	GPUDrawPushConstants push_constants;
	push_constants.worldMatrix = glm::mat4{ 1.f };
	push_constants.vertexBuffer = rectangle.vertexBufferAddress;

	vkCmdPushConstants(cmd, _meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);
	vkCmdBindIndexBuffer(cmd, rectangle.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);

	vkCmdEndRendering(cmd);
```

이번에는 사각형 메시를 위한 파이프라인을 바인딩합니다.

그 후, 푸시 상수를 사용해 vertexBufferAddress를 GPU로 업로드 합니다. 행렬은 메시 변환을 수행할 때 까지는 기본 행렬을 사용하겠습니다.

그 후 vkCmdBindIndexBuffer를 사용해 인덱스 버퍼를 바인딩합니다. 아쉽게도 여기서는 디바이스 어드레스를 사용할 수 없고, 대신 VkBuffer와 오프셋을 전달해야 합니다.

마지막으로 `vkCmdDrawIndexed`를 사용해 2개의 삼각형(6개의 인덱스)를 그립니다. 이는 vkCmdDraw와 같지만 현재 바인딩된 인덱스 버퍼를 사용하여 메시를 그린다는 점이 다릅니다.

이것으로 끝났습니다. 이제 어떤 메시도 렌더링할 수 있는 일반적인 수단이 마련되었습니다.

다음은 가장 기본적인 방법으로 GLTF로부터 메시 파일을 불러와 사각형보다는 더 복잡하고 멋있는 것을 다뤄보겠습니다.

다음 글 : [ 메시 불러오기]({{ site.baseurl }}{% link docs/new_chapter_3/loading_meshes.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}
