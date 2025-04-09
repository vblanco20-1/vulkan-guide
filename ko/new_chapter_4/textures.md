---
layout: default
title: Textures
parent: "4. Textures and Engine Architecture"
nav_order: 6
---

우리는 이미 컴퓨트 기반 렌더링에서 이미지 사용 방법을 다룬 적이 있습니다. 하지만 아직 그래픽 셰이더에서 이미지를 렌더링 및 화면 출력 용도로 사용하는 방법은 다루지 않았습니다. 이제 엔진의 기본 텍스쳐들을 생성하고, 이후에는 파일에서 텍스쳐를 불러오는 과정을 살펴보겠습니다.

먼저 VulkanEngine에 이미지를 관리하고 파괴하는 함수를 추가해야 합니다.

헤더에 다음 함수를 추가합니다.

```cpp
class VulkanEngine {

AllocatedImage create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
AllocatedImage create_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
void destroy_image(const AllocatedImage& img);
}
```

이제 vk_engine.cpp에 다음 함수를 작성합시다.

<!-- codegen from tag create_image on file E:\ProgrammingProjects\vulkan-guide-2\chapter-4/vk_engine.cpp --> 
```cpp
AllocatedImage VulkanEngine::create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped)
{
	AllocatedImage newImage;
	newImage.imageFormat = format;
	newImage.imageExtent = size;

	VkImageCreateInfo img_info = vkinit::image_create_info(format, usage, size);
	if (mipmapped) {
		img_info.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(size.width, size.height)))) + 1;
	}

	// always allocate images on dedicated GPU memory
	VmaAllocationCreateInfo allocinfo = {};
	allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// allocate and create the image
	VK_CHECK(vmaCreateImage(_allocator, &img_info, &allocinfo, &newImage.image, &newImage.allocation, nullptr));

	// if the format is a depth format, we will need to have it use the correct
	// aspect flag
	VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
	if (format == VK_FORMAT_D32_SFLOAT) {
		aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
	}

	// build a image-view for the image
	VkImageViewCreateInfo view_info = vkinit::imageview_create_info(format, newImage.image, aspectFlag);
	view_info.subresourceRange.levelCount = img_info.mipLevels;

	VK_CHECK(vkCreateImageView(_device, &view_info, nullptr, &newImage.imageView));

	return newImage;
}
```

이는 drawImage를 생성했을 때와 동일한 작업이며, 해당 작업을 별도의 함수로 복사한 것입니다. 먼저 이미지의 크기와 포맷 정보를 AllocatedImage 구조체에 저장하고, 그 후 VkImageCreateInfo를 생성합니다. 이후 VMA를 통해 이미지를 할당하고 마지막으로 이미지 뷰를 생성합니다. 이전과 다른 점은 aspect 플래그를 설정한다는 것입니다. 이미지가 D32 float와 같은 깊이 포맷이 아닌 경우 기본적으로 `VK_IMAGE_ASPECT_COLOR_BIT`를 설정합니다. 

이미지에 데이터를 업로드하기 위해 지난 챕터에서 버퍼에 데이터를 썼던 방식과 유사한 절차를 따릅니다. 임시 스테이징 버퍼를 생성하고 해당 버퍼에 픽셀 데이터를 복사한 뒤, VkCmdCopyBufferToImage를 호출하는 immediate submit을 수행합니다. 이 기능을 처리하는 함수를 작성합시다. 기존 create_image 함수의 오버로딩 버전으로 만들고, 픽셀 데이터를 전달받기 위해 void* 인자를 추가할 것입니다. 여기서는 대부분의 이미지 파일이 사용하는 RGBA 8비트 포맷을 기준으로 텍스쳐를 하드코딩할 것입니다.

<!-- codegen from tag upload_image on file E:\ProgrammingProjects\vulkan-guide-2\chapter-4/vk_engine.cpp --> 
```cpp
AllocatedImage VulkanEngine::create_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped)
{
	size_t data_size = size.depth * size.width * size.height * 4;
	AllocatedBuffer uploadbuffer = create_buffer(data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	memcpy(uploadbuffer.info.pMappedData, data, data_size);

	AllocatedImage new_image = create_image(size, format, usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, mipmapped);

	immediate_submit([&](VkCommandBuffer cmd) {
		vkutil::transition_image(cmd, new_image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VkBufferImageCopy copyRegion = {};
		copyRegion.bufferOffset = 0;
		copyRegion.bufferRowLength = 0;
		copyRegion.bufferImageHeight = 0;

		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = 0;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount = 1;
		copyRegion.imageExtent = size;

		// copy the buffer into the image
		vkCmdCopyBufferToImage(cmd, uploadbuffer.buffer, new_image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
			&copyRegion);

		vkutil::transition_image(cmd, new_image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		});

	destroy_buffer(uploadbuffer);

	return new_image;
}
```

우선 픽셀 데이터를 저장할 수 있을 만큼의 공간을 갖는 CPU_TO_GPU 메모리 타입의 스테이징 버퍼를 할당하고, memcpy를 수행해 해당 버퍼에 픽셀 데이터를 복사합니다.

그 다음 기존의 create_image 함수를 호출하되 `VK_IMAGE_USAGE_TRANSFER_DST_BIT`와 `VK_IMAGE_TRANSFER_SRC_BIT`를 추가하여 데이터를 복사하거나 이미지에 데이터를 복사할 수 있도록 합니다.

이미지와 스테이징 버퍼가 준비되었으면, immediate submit을 수행해 스테이징 버퍼의 픽셀 데이터를 이미지로 복사합니다.

스왑체인 이미지에서 작업했던 것과 마찬가지로, 먼저 이미지 레이아웃을 `VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL`로 전환합니다. 그후 복사 명령 파라미터를 담을 copyRegion 구조체를 생성합니다. 이 구조체에는 이미지 크기와 복사 대상 이미지 레이어, 그리고 밉맵 레벨 정보가 필요합니다. 이미지 레이어는 여러 레이어를 갖는 텍스쳐에 사용되며, 대표적인 예시는 큐브맵 텍스쳐입니다. 큐브맵은 여섯 개의 면을 가지므로 레이어 수가 6이 됩니다. 이후 반사 큐브맵을 구성할 때 다루게 될 예정입니다. 

밉맵의 경우, 최상위 레벨인 0번 레벨에 데이터를 복사하며, 현재 이미지는 추가적인 밉맵 레벨은 갖고 있지 않습니다. 지금은 create_image 함수에 mipmapped bool 값만 전달하고 있으며, 별도의 처리는 하지 않고 있습니다. 밉맵 생성은 이후 단계에서 다룰 예정입니다.

마지막으로 이미지 메모리를 해제하기 위해 destroy_image() 함수를 구현하겠습니다.
```cpp
void VulkanEngine::destroy_image(const AllocatedImage& img)
{
    vkDestroyImageView(_device, img.imageView, nullptr);
    vmaDestroyImage(_allocator, img.image, img.allocation);
}
```
먼저 이미지 뷰를 파괴하고 VMA를 사용해 이미지를 파괴합니다. 이 과정을 통해 이미지와 그에 할당된 메모리가 올바르게 파괴됩니다.

이러한 함수들을 사용해 기본 텍스쳐들을 설정할 수 있습니다. 기본 흰색, 검은색, 회색, 체크보드 텍스쳐를 생성하겠습니다. 이렇게 하면 텍스쳐 로딩에 실패했을 때 사용할 수 있는 기본 텍스쳐들을 미리 준비해둘 수 있습니다.

이러한 테스트 이미지들을 VulkanEngine 클래스에 추가합니다. 이 텍스쳐들과 기타 이미지에서 사용할 수 있도록 샘플러들도 몇개 함께 생성할 예정입니다. 

```cpp
	AllocatedImage _whiteImage;
	AllocatedImage _blackImage;
	AllocatedImage _greyImage;
	AllocatedImage _errorCheckerboardImage;

    VkSampler _defaultSamplerLinear;
	VkSampler _defaultSamplerNearest;
```

사각형 메시를 생성하는 코드 이후에, `init_default_data()` 함수의 내부에서 이 기본 텍스쳐들을 생성해 봅시다.

<!-- codegen from tag default_img on file E:\ProgrammingProjects\vulkan-guide-2\chapter-4/vk_engine.cpp --> 
```cpp
	//3 default textures, white, grey, black. 1 pixel each
	uint32_t white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
	_whiteImage = create_image((void*)&white, VkExtent3D{ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_SAMPLED_BIT);

	uint32_t grey = glm::packUnorm4x8(glm::vec4(0.66f, 0.66f, 0.66f, 1));
	_greyImage = create_image((void*)&grey, VkExtent3D{ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_SAMPLED_BIT);

	uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 0));
	_blackImage = create_image((void*)&black, VkExtent3D{ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_SAMPLED_BIT);

	//checkerboard image
	uint32_t magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
	std::array<uint32_t, 16 *16 > pixels; //for 16x16 checkerboard texture
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			pixels[y*16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
		}
	}
	_errorCheckerboardImage = create_image(pixels.data(), VkExtent3D{16, 16, 1}, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_SAMPLED_BIT);

	VkSamplerCreateInfo sampl = {.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};

	sampl.magFilter = VK_FILTER_NEAREST;
	sampl.minFilter = VK_FILTER_NEAREST;

	vkCreateSampler(_device, &sampl, nullptr, &_defaultSamplerNearest);

	sampl.magFilter = VK_FILTER_LINEAR;
	sampl.minFilter = VK_FILTER_LINEAR;
	vkCreateSampler(_device, &sampl, nullptr, &_defaultSamplerLinear);

	_mainDeletionQueue.push_function([&](){
		vkDestroySampler(_device,_defaultSamplerNearest,nullptr);
		vkDestroySampler(_device,_defaultSamplerLinear,nullptr);

		destroy_image(_whiteImage);
		destroy_image(_greyImage);
		destroy_image(_blackImage);
		destroy_image(_errorCheckerboardImage);
	});
```

기본 색상 이미지 3개는 각각 하나의 색상을 가진 단일 픽셀 이미지로 생성합니다. 체크보드 텍스쳐는 16 * 16 크기의 픽셀 배열을 작성하고, 간단한 수학 연산을 통해 검정색과 마젠타 색이 번갈아 나타나는 패턴으로 만듭니다.

샘플러에는 min/mag filters를 제외한 모든 파라미터를 기본값으로 두었습니다. 필터는 Linear 혹은 Nearest로 설정할 수 있으며, Nearest는 픽셀 형태가 그대로 보이는 뚜렷한 느낌을 주고, Linear는 픽셀 사이를 블러 처리하여 부드럽게 보이게 합니다.

## 셰이더에 이미지 바인딩하기 
컴퓨트 기반 렌더링을 수행할 때, 샘플링 로직 없이 텍스쳐에 읽기 쓰기 하기 위해 `VK_DESCRIPTOR_TYPE_STORAGE_IMAGE`를 사용해 이미지를 바인딩했었습니다. 이는 다차원 구조와 다른 메모리 레이아웃을 갖는 버퍼를 바인딩하는 것과 유사합니다. 하지만 그래픽스 렌더링을 할 때에는 텍스쳐에 접근할 때 GPU의 고정 하드웨어를 사용해야 하며, 이를 위해 샘플러가 필요합니다. 샘플러를 사용하는 방법에는 이미지와 샘플러를 하나로 묶는 `VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER`를 사용하는 방법과, 2개의 디스크립터를 통해 `VK_DESCRIPTOR_TYPE_SAMPLER`와 `VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE`를 개별적으로 사용하는 방법이 있습니다. GPU 제조사에 따르면 개별 디스크립터를 사용하는 방식이 데이터 중복이 적어 더 빠를 수 있다고 합니다. 하지만 이 방식은 다루기 복잡하므로, 지금은 사용하지 않을 예정입니다. 대신, 셰이더를 간단하게 만들기 위해 combined 디스크립터를 사용하겠습니다.

이전에 그렸던 직사각형 도형을 해당 도형 안에 이미지를 표시하는 방식으로 수정할 예정입니다.이를 위해 이미지를 표시하는 새로운 프래그먼트 셰이더가 필요하며, 이름은 `text_image.frag`로 하겠습니다.

```c
//glsl version 4.5
#version 450

//shader input
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inUV;
//output write
layout (location = 0) out vec4 outFragColor;

//texture to access
layout(set =0, binding = 0) uniform sampler2D displayTexture;

void main() 
{
	outFragColor = texture(displayTexture,inUV);
}
```

프래그먼트 셰이더에 color와 UV라는 2가지 입력이 있습니다. 셰이더가 color를 사용하지는 않지만 이전과 동일한 정점 셰이더를 사용하기 때문에 그대로 유지합니다.

텍스쳐를 샘플링하기 위해 `texture(textureSampler, coordinates)`를 사용합니다. 특정 픽셀에 직접 접근할 수 있는 다른 방법도 있습니다. 텍스쳐 객체는 `uniform sampler2D`로 선언됩니다.

이렇게 하려면 파이프라인 레이아웃을 수정해야 하므로, 그것도 함께 업데이트하겠습니다.

VulkanEngine에 레이아웃을 추가합시다. 이후에도 계속 사용할 예정입니다.

```cpp
class VulkanEngine {
VkDescriptorSetLayout _singleImageDescriptorLayout;
}
```

On init_descriptors(), lets create it alongside the rest

```cpp
{
	DescriptorLayoutBuilder builder;
	builder.add_binding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	_singleImageDescriptorLayout = builder.build(_device, VK_SHADER_STAGE_FRAGMENT_BIT);
}
```

하나의 이미지-샘플러 디스크립터만 갖는 디스크립터 셋을 설정합니다. 이제 `init_mesh_pipeline()` 함수를 수정하여 이를 반영할 수 있습니다. 함수의 시작 부분을 수정하면서, 프래그먼트 셰이더를 교체하고, 디스크립터 셋 레이아웃을 파이프라인 레이아웃 생성에 연결합니다.

```cpp
void VulkanEngine::init_mesh_pipeline()
{
	VkShaderModule triangleFragShader;
	if (!vkutil::load_shader_module("../../shaders/tex_image.frag.spv", _device, &triangleFragShader)) {
		fmt::print("Error when building the fragment shader \n");
	}
	else {
		fmt::print("Triangle fragment shader succesfully loaded \n");
	}

	VkShaderModule triangleVertexShader;
	if (!vkutil::load_shader_module("../../shaders/colored_triangle_mesh.vert.spv", _device, &triangleVertexShader)) {
		fmt::print("Error when building the vertex shader \n");
	}
	else {
		fmt::print("Triangle vertex shader succesfully loaded \n");
	}

	VkPushConstantRange bufferRange{};
	bufferRange.offset = 0;
	bufferRange.size = sizeof(GPUDrawPushConstants);
	bufferRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::pipeline_layout_create_info();
	pipeline_layout_info.pPushConstantRanges = &bufferRange;
	pipeline_layout_info.pushConstantRangeCount = 1;
	pipeline_layout_info.pSetLayouts = &_singleImageDescriptorLayout;
	pipeline_layout_info.setLayoutCount = 1;
	VK_CHECK(vkCreatePipelineLayout(_device, &pipeline_layout_info, nullptr, &_meshPipelineLayout));

}
```

이제 draw 함수에서 파이프라인을 바인딩할 때 필요한 디스크립터 셋을 동적으로 생성하여 원하는 텍스쳐를 화면에 출력하는 데 사용할 수 있습니다.

이를 `draw_geometry()` 함수에 작성하여 원숭이 메시를 텍스쳐가 적용된 메시로 바꿔봅시다.

<!-- codegen from tag draw_tex on file E:\ProgrammingProjects\vulkan-guide-2\chapter-4/vk_engine.cpp --> 
```cpp
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _meshPipeline);

	//bind a texture
	VkDescriptorSet imageSet = get_current_frame()._frameDescriptors.allocate(_device, _singleImageDescriptorLayout);
	{
		DescriptorWriter writer;
		writer.write_image(0, _errorCheckerboardImage.imageView, _defaultSamplerNearest, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

		writer.update_set(_device, imageSet);
	}

	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _meshPipelineLayout, 0, 1, &imageSet, 0, nullptr);

	glm::mat4 view = glm::translate(glm::vec3{ 0,0,-5 });
	// camera projection
	glm::mat4 projection = glm::perspective(glm::radians(70.f), (float)_drawExtent.width / (float)_drawExtent.height, 10000.f, 0.1f);

	// invert the Y direction on projection matrix so that we are more similar
	// to opengl and gltf axis
	projection[1][1] *= -1;

	GPUDrawPushConstants push_constants;
	push_constants.worldMatrix = projection * view;
	push_constants.vertexBuffer = testMeshes[2]->meshBuffers.vertexBufferAddress;

	vkCmdPushConstants(cmd, _meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);
	vkCmdBindIndexBuffer(cmd, testMeshes[2]->meshBuffers.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(cmd, testMeshes[2]->surfaces[0].count, 1, testMeshes[2]->surfaces[0].startIndex, 0, 0);
```

셰이더에서 사용하는 _singleImageDescriptorLayout을 이용해 프레임별 디스크립터 셋 할당기에서 새로운 디스크립터 셋을 할당합니다.

그런 다음 DescriptorWrite를 사용해 바인딩 0번에 이미지 디스크립터를 작성합니다. 이 디스크립터에는 _errorCheckerboardImage를 설정하고, 픽셀 간 블렌딩이 일어나지 않도록 nearest-sampler를 전달합니다. 이후 작성된 내용을 기반으로 디스크립터 셋을 업데이트하고 바인딩한 뒤, 그리기를 진행합니다.

결과적으로 마젠타 체크 패턴이 적용된 원숭이 머리를 확인할 수 있습니다.


![chapter2]({{site.baseurl}}/diagrams/texmonkey.png)

다음 글 : [ 엔진 아키텍처 ]({{ site.baseurl }}{% link docs/new_chapter_4/engine_arch.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}
