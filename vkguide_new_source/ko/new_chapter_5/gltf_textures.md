---
layout: default
title: GLTF Textures
parent: "5. GLTF loading - KO"
grand_parent: "Korean VKGuide"
nav_order: 7
---

지난 글에서 텍스쳐에 대한 설명을 이 글에서 조금 더 자세히 다루기 위해 생략했었습니다.

텍스쳐를 불러올 때 stb_image를 사용할 것입니다. 이는 png, jpeg 등을 지원하는 단일 헤더 라이브러리입니다. 아쉽지만 KTX나 DDS와 같은 포맷은 지원하지 않습니다. 이러한 포맷은 GPU에 거의 직접 업로드할 수 있고, GPU가 압축된 상태로 바로 읽을 수 있기 때문에 그래픽 용도로 더 적합하며 VRAM도 절약합니다.

fastgltf는 이미지를 불러올 때 여러 가지 방식을 제공하므로, 이러한 다양한 경우를 처리할 수 있도록 지원해야 합니다.

load_image 함수에 해당 로직을 작성해 봅시다.

^code loadimg chapter-5/vk_loader.cpp

함수는 길지만, 실질적으로 같은 작업을 세 가지 방식으로 처리하는 것입니다.

첫 번째는 텍스쳐가 GLTF/GLB 파일 외부에 저장된 일반적인 경우입니다. 이 경우 텍스쳐 경로를 사용해 stb_load를 호출하고, 성공하면 이미지를 생성합니다.

두 번째는 fastgltf가 텍스쳐를 std::vector으로 불러온 경우 입니다. 텍스쳐가 base64로 인코딩되어 있거나 외부 이미지 파일을 직접 읽도록 설정했을 때 이러한 경우가 발생합니다. 바이트 데이터를 가져와 stbi_load_from_memory로 전달합니다.

세 번째 경우는 이미지 파일이 바이너리 GLB 파일에 임베딩 되어 있을 때 BufferView에서 불러오는 경우입니다. 두 번째와 동일하게 stbi_load_from_memory를 사용합니다.

여기서 엔진을 컴파일하면 STB_image 함수 정의를 찾지 못했다는 오류가 발생합니다. STB는 단일 헤더 라이브러리이므로, 함수 정의를 컴파일 하려면 하나의 소스 파일에 특정 매크로를 정의해주어야 합니다. 이를 vk_images.cpp 혹은 다른 cpp 파일에 추가하면 해당 파일에 함수 정의가 포함되어 링크할 수 있습니다.

```
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
```

이제 GLTF를 불러오는 함수로 되돌아가 거기서 이미지도 함께 불러오도록 하겠습니다.

```cpp
    // load all textures
	for (fastgltf::Image& image : gltf.images) {
		std::optional<AllocatedImage> img = load_image(engine, gltf, image);

		if (img.has_value()) {
			images.push_back(*img);
			file.images[image.name.c_str()] = *img;
		}
		else {
			// we failed to load, so lets give the slot a default white texture to not
			// completely break loading
			images.push_back(engine->_errorCheckerboardImage);
			std::cout << "gltf failed to load texture " << image.name << std::endl;
		}
	}
```

이미지를 불러오고, 성공한다면 이미지를 목록에 저장합니다. 실패한다면 에러 이미지를 사용합니다.

이제 다시 프로젝트를 실행해보면 오브젝트에 텍스쳐가 적용된 것을 볼 수 있습니다.

이제 LoadedGLTF의 clearAll 함수를 구현해 자원들이 올바르게 해제되도록 합니다.

```cpp
void LoadedGLTF::clearAll()
{
    VkDevice dv = creator->_device;

    descriptorPool.destroy_pools(dv);
    creator->destroy_buffer(materialDataBuffer);

    for (auto& [k, v] : meshes) {

		creator->destroy_buffer(v->meshBuffers.indexBuffer);
		creator->destroy_buffer(v->meshBuffers.vertexBuffer);
    }

    for (auto& [k, v] : images) {
        
        if (v.image == creator->_errorCheckerboardImage.image) {
            //dont destroy the default images
            continue;
        }
        creator->destroy_image(v);
    }

	for (auto& sampler : samplers) {
		vkDestroySampler(dv, sampler, nullptr);
    }
}
```

먼저 디스크립터 풀과 머테리얼 버퍼를 파괴합니다. 이후 모든 메시를 순회하며 메시의 인덱스, 정점 버퍼를 파괴합니다. 그런 다음 이미지를 순회하며 에러 이미지가 아닌 모든 이미지를 파괴합니다(앞서 설명했듯, 이미지를 불러오는 데 실패한 경우 에러 이미지를 사용하게 되며, 이를 여러 번 해제하면 안됩니다).

마지막으로 샘플러를 순회하며 각 샘플러를 파괴합니다.

여기서 중요한 점이 있습니다. 사용 중인 프레임에서 LoadedGLTF를 즉시 삭제할 수는 없다는 것입니다. 해당 구조체가 여전히 사용중일 수 있기 때문입니다. 런타임에 LoadedGLTF를 삭제하려면 cleanup에서 했던 것처럼 VkQueueWait을 호출하거나, 프레임별 삭제큐에 추가하여 삭제를 지연시켜야합니다. LoadedGLTF는 shared_ptr로 관리되고 있으므로, 람다 캡쳐 기능을 적절히 활용해 이 작업을 처리할 수 있습니다.


# 투명한 객체

우리는 이전에 이를 생략했지만, GLTF파일에는 불투명한 객체뿐만 아니라 투명한 객체도 포함되어 있습니다. GLTF 기본 머테리얼을 생성하고 해당 파이프라인을 컴파일할 때 블렌딩을 활성화 했었습니다. 패스를 투명 모드로 설정하는 코드가 이미 포함되어 있지만, 현재는 투명한 객체를 올바르게 렌더링하지 않고 있습니다.

투명한 객체들은 깊이 버퍼에 값을 기록하지 않기 때문에, 투명한 객체를 먼저 그린 뒤 불투명 객체가 그려진다면 시각적인 결함이 발생할 수 있습니다. 이러한 문제를 방지하기 위해, 투명 객체는 프레임의 마지막에 렌더링되도록 처리해야 합니다.

이를 위해 RenderObject를 정렬하는 방법도 있지만, 투명 객체들은 불투명 객체와는 다르게 정렬되어야 합니다. 따라서 DrawContext에 두 개의 RenderObject 배열을 두는 것이 더 나은 방법입니다. 하나는 불투명, 다른 하나는 투명 객체용입니다. 이처럼 객체들을 분리하면, 불투명 객체에만 깊이 패스를 적용하거나 특정 셰이더 로직을 적용하는 등의 다양한 작업에 유용합니다. 또한, 투명 객체를 별도의 이미지에 렌더링한 후 최종 이미지에 합성하는 방식도 흔히 사용됩니다. 

```cpp
struct DrawContext {
    std::vector<RenderObject> OpaqueSurfaces;
    std::vector<RenderObject> TransparentSurfaces;
};
```

이제 `draw_geometry`함수를 수정하겠습니다. 두 개의 반복문에서 Vulkan 호출이 필요하기 때문에, 내부 반복문을 draw() 람다로 분리하고, 해당 람다를 반복문 내에서 호출하겠습니다.

```cpp
auto draw = [&](const RenderObject& draw) {
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, draw.material->pipeline->pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, draw.material->pipeline->layout, 0, 1, &globalDescriptor, 0, nullptr);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, draw.material->pipeline->layout, 1, 1, &draw.material->materialSet, 0, nullptr);

    vkCmdBindIndexBuffer(cmd, draw.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    GPUDrawPushConstants pushConstants;
    pushConstants.vertexBuffer = draw.vertexBufferAddress;
    pushConstants.worldMatrix = draw.transform;
    vkCmdPushConstants(cmd, draw.material->pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &pushConstants);

    vkCmdDrawIndexed(cmd, draw.indexCount, 1, draw.firstIndex, 0, 0);
};

for (auto& r : mainDrawContext.OpaqueSurfaces) {
    draw(r);
}

for (auto& r : mainDrawContext.TransparentSurfaces) {
    draw(r);
}
```

이제 올바르게 동작하는 투명 객체를 처리할 수 있게 되었습니다. GLTF 구조체를 불러온다면 조명 헤일로가 올바르게 보이는 것을 확인할 수 있습니다.

함수의 끝에서 투명 객체 배열도 반드시 초기화 해주세요.

```
// we delete the draw commands now that we processed them
mainDrawContext.OpaqueSurfaces.clear();
mainDrawContext.TransparentSurfaces.clear();
```

이로써 엔진의 기본적인 기능이 끝났습니다. 이를 기반으로 게임을 개발할 수 있습니다. 하지만 선택적으로 몇 가지 성능 및 품질 향상을 위한 개선 작업을 추가로 진행하겠습니다.

^nextlink

{% include comments.html term="Vkguide 2 Beta Comments" %}