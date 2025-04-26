---
layout: default
title: Mesh Loading
parent: "3. Graphics Pipelines - KO"
grand_parent: "Korean VKGuide"
nav_order: 12
---

이후에 씬 로딩을 제대로 수행하기 전까지는, 사각형보다 더 나은 메시가 필요합니다. 이를 위해 GLTF 파일로부터 지오메트리 정보를 제외한 나머지 정보를 무시하는 단순한 편법을 통해 이를 불러오겠습니다.

starting-point 레포지토리에는 basic.glb라 불리는 GLTF 파일이 포함되어 있습니다. 이 파일에는 원점을 중심으로 배치된 큐브와 구, 그리고 원숭이 머리 메시가 들어있습니다. 이처럼 단순한 파일이기 때문에, 실제 GLTF 로딩 설정 없이도 쉽게 불러올 수 있습니다.
 
GLTF 파일은 메시의 목록을 포함하며, 각 메시는 여러 프리미티브로 구성됩니다. 이는 하나의 메시가 여러 머테리얼을 사용하는 경우를 처리하기 위한 구조로, 이를 그리기 위해 여러 번의 드로우콜이 필요합니다. 또한 GLTF 파일에는 씬-트리 형태의 씬 노드도 포함되어 있으며, 일부 노드는 메시를 포함합니다. 현재는 메시 데이터만 로딩하겠지만, 추후 전체 씬트리와 머테리얼도 불러올 예정입니다.
 
파일을 불러오는 것과 관련된 코드는 vk_loader.cpp/h에 작성되어 있습니다.

먼저 메시를 불러오는 데 필요한 두 클래스를 추가합시다.

```cpp
#include <vk_types.h>
#include <unordered_map>
#include <filesystem>

struct GeoSurface {
    uint32_t startIndex;
    uint32_t count;
};

struct MeshAsset {
    std::string name;

    std::vector<GeoSurface> surfaces;
    GPUMeshBuffers meshBuffers;
};

//forward declaration
class VulkanEngine;
```

주어진 MeshAsset은 파일 이름과 메시 버퍼를 갖습니다. 또한 해당 메시의 하위메시를 나타내는 GeoSurface 객체의 배열도 포함합니다. 각 서브메시를 렌더링할 때에는 각각 별도의 드로우콜을 수행하게 됩니다. 우리는 모든 서브메시의 정점 데이터를 하나의 버퍼에 이어붙이기 때문에, 드로우 콜에서는 StartIndex와 count값을 사용하게 됩니다.

이를 vk_loader.cpp에 추가합시다. 이후 필요할 것입니다.

```cpp
#include "stb_image.h"
#include <iostream>
#include <vk_loader.h>

#include "vk_engine.h"
#include "vk_initializers.h"
#include "vk_types.h"
#include <glm/gtx/quaternion.hpp>

#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/parser.hpp>
#include <fastgltf/tools.hpp>
```


파일을 불러오는 함수는 다음과 같습니다.
```cpp
std::optional<std::vector<std::shared_ptr<MeshAsset>>> loadGltfMeshes(VulkanEngine* engine, std::filesystem::path filePath);
```

std::optional이 처음으로 등장하는 부분입니다. std::optional은 타입(여기서는 메시 에셋의 vector)을 래핑하는 표준 클래스로, 값이 없거나 에러가 발생한 상태를 표현할 수 있게 해줍니다. 여러 이유로 파일을 불러오는 것이 실패할 수 있기 때문에, null을 반환할 수 있게 하는 편이 좋습니다. 파일을 불러오는데 새로운 STL 기능을 활용하는 fastGltf 라이브러리를 사용할 것입니다.

이제 파일을 열어봅시다.

<!-- codegen from tag openmesh on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_loader.cpp --> 
```cpp
    std::cout << "Loading GLTF: " << filePath << std::endl;

    fastgltf::GltfDataBuffer data;
    data.loadFromFile(filePath);

    constexpr auto gltfOptions = fastgltf::Options::LoadGLBBuffers
        | fastgltf::Options::LoadExternalBuffers;

    fastgltf::Asset gltf;
    fastgltf::Parser parser {};

    auto load = parser.loadBinaryGLTF(&data, filePath.parent_path(), gltfOptions);
    if (load) {
        gltf = std::move(load.get());
    } else {
        fmt::print("Failed to load glTF: {} \n", fastgltf::to_underlying(load.error()));
        return {};
    }
```

지금은 바이너리 GLTF만을 지원하겠습니다. 따라서 먼저 `loadFromFile`로 파일을 열고 loadBinaryGLTF를 호출해 열겠습니다. 이 과정에서는(아직 사용하지 않더라도) 상대 경로를 계산하기 위해 부모 디렉토리 경로가 필요합니다.

다음으로 각 메시를 순회하여 정점과 인덱스를 임시 std::vector로 복사하고, 이를 엔진의 메시로 업로드 할 것입니다. 이 과정에서 여러 `MeshAsset` 객체를 만들어 배열로 구성할 것입니다.

<!-- codegen from tag loadmesh on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_loader.cpp --> 
```cpp
    std::vector<std::shared_ptr<MeshAsset>> meshes;

    // use the same vectors for all meshes so that the memory doesnt reallocate as
    // often
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;
    for (fastgltf::Mesh& mesh : gltf.meshes) {
        MeshAsset newmesh;

        newmesh.name = mesh.name;

        // clear the mesh arrays each mesh, we dont want to merge them by error
        indices.clear();
        vertices.clear();

        for (auto&& p : mesh.primitives) {
            GeoSurface newSurface;
            newSurface.startIndex = (uint32_t)indices.size();
            newSurface.count = (uint32_t)gltf.accessors[p.indicesAccessor.value()].count;

            size_t initial_vtx = vertices.size();

            // load indexes
            {
                fastgltf::Accessor& indexaccessor = gltf.accessors[p.indicesAccessor.value()];
                indices.reserve(indices.size() + indexaccessor.count);

                fastgltf::iterateAccessor<std::uint32_t>(gltf, indexaccessor,
                    [&](std::uint32_t idx) {
                        indices.push_back(idx + initial_vtx);
                    });
            }

            // load vertex positions
            {
                fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->second];
                vertices.resize(vertices.size() + posAccessor.count);

                fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor,
                    [&](glm::vec3 v, size_t index) {
                        Vertex newvtx;
                        newvtx.position = v;
                        newvtx.normal = { 1, 0, 0 };
                        newvtx.color = glm::vec4 { 1.f };
                        newvtx.uv_x = 0;
                        newvtx.uv_y = 0;
                        vertices[initial_vtx + index] = newvtx;
                    });
            }

            // load vertex normals
            auto normals = p.findAttribute("NORMAL");
            if (normals != p.attributes.end()) {

                fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[(*normals).second],
                    [&](glm::vec3 v, size_t index) {
                        vertices[initial_vtx + index].normal = v;
                    });
            }

            // load UVs
            auto uv = p.findAttribute("TEXCOORD_0");
            if (uv != p.attributes.end()) {

                fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[(*uv).second],
                    [&](glm::vec2 v, size_t index) {
                        vertices[initial_vtx + index].uv_x = v.x;
                        vertices[initial_vtx + index].uv_y = v.y;
                    });
            }

            // load vertex colors
            auto colors = p.findAttribute("COLOR_0");
            if (colors != p.attributes.end()) {

                fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[(*colors).second],
                    [&](glm::vec4 v, size_t index) {
                        vertices[initial_vtx + index].color = v;
                    });
            }
            newmesh.surfaces.push_back(newSurface);
        }

        // display the vertex normals
        constexpr bool OverrideColors = true;
        if (OverrideColors) {
            for (Vertex& vtx : vertices) {
                vtx.color = glm::vec4(vtx.normal, 1.f);
            }
        }
        newmesh.meshBuffers = engine->uploadMesh(indices, vertices);

        meshes.emplace_back(std::make_shared<MeshAsset>(std::move(newmesh)));
    }

    return meshes;

```

메시의 각 프리미티브를 순회하면서 `iterateAccessor`함수를 사용해 원하는 정점 데이터에 접근합니다. 여러 프리미티브를 정점 배열에 추가할 때는 인덱스 버퍼도 올바르게 구성해야 합니다. 마지막으로는 uploadMesh를 호출하여 최종 버퍼를 생성하고, 메시 목록을 반환합니다.

컴파일 시 플래그인 OverrideColors가 설정되어 있다면, 정점 색상을 디버깅에 유용한 법선 벡터로 덮어씁니다.

위치 배열은 항상 존재하므로, Vertex 구조체를 초기화합니다. 다른 모든 속성은 존재 여부를 확인 후 초기화해야 합니다.

이제 이를 그려봅시다.

```
	std::vector<std::shared_ptr<MeshAsset>> testMeshes;
```

먼저 이를 VulkanEngine 클래스에 추가합니다. `init_default_data()`에서 불러와 vk_engine의 include 목록에 `#include <vk_loader.h>`를 추가합니다.

```
testMeshes = loadGltfMeshes(this,"..\\..\\assets\\basicmesh.glb").value();
```

제공된 파일에는 인덱스 0번에 큐브, 인덱스 1번에 구, 그리고 인덱스 2번에는 원숭이 머리가 포함되어 있습니다. 마지막 메시 것을 그릴 것이며, 이전의 그린 사각형 바로 다음에 그리도록 하겠습니다.

<!-- codegen from tag meshdraw on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_engine.cpp --> 
```cpp
	push_constants.vertexBuffer = testMeshes[2]->meshBuffers.vertexBufferAddress;

	vkCmdPushConstants(cmd, _meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);
	vkCmdBindIndexBuffer(cmd, testMeshes[2]->meshBuffers.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(cmd, testMeshes[2]->surfaces[0].count, 1, testMeshes[2]->surfaces[0].startIndex, 0, 0);
```

이제 원숭이 머리가 색상과 함께 렌더링 된 것을 볼 수 있습니다. `OverrideColors`를 지금은 true로 설정했기 때문에 법선 벡터값이 색상으로 나타납니다. 현재 셰이더에서는 조명이 적용되지 않았기 때문에, 이 옵션을 끄면 원숭이 머리는 순수한 흰색으로 표시됩니다.

이제 원숭이 머리가 보이지만, 거꾸로 뒤집혀 있습니다. 변환 행렬을 조정해봅시다.

GLTF에서 축은 OpenGL에서와 같습니다. OpenGL에서는 Y축이 위 방향을 가리키지만 Vulkan에서는 Y축이 아래 방향이기 때문에 결과적으로 화면이 뒤집혀 보이게 됩니다. 이를 해결하는 방법에는 2가지 방법이 있습니다. 하나는 뷰포트를 뒤집어 전체 렌더링 결과를 뒤집는 방식이며, 이는 DirectX와 유사합니다. 두 번째는 투영 행렬에 Y축 반전을 포함하는 방식입니다. 우리는 후자를 수행하겠습니다.
 
렌더링 코드에서 더 적절한 변환 행렬을 적용하겠습니다. 아래 코드를 `draw_geometry()`의 푸시상수 호출 직전에 추가합니다.

<!-- codegen from tag matview on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_engine.cpp --> 
```cpp
	glm::mat4 view = glm::translate(glm::vec3{ 0,0,-5 });
	// camera projection
	glm::mat4 projection = glm::perspective(glm::radians(70.f), (float)_drawExtent.width / (float)_drawExtent.height, 10000.f, 0.1f);

	// invert the Y direction on projection matrix so that we are more similar
	// to opengl and gltf axis
	projection[1][1] *= -1;

	push_constants.worldMatrix = projection * view;
```

`#include <glm/gtx/transform.hpp>`를 vk_engine의 상단에 추가합니다. 그러면 cpp에서 변환 함수들을 사용할 수 있게 됩니다.

먼저 카메라 시점의 뷰 행렬을 계산합니다. 현재는 뒤로 움직이는 변환 행렬로 충분합니다.

투영 행렬에는 약간의 꼼수를 사용했습니다. near 값에는 10000을, far 값에는 0.1을 전달함을 주목하세요. 깊이를 뒤집어 1이 near plane, 깊이 0이 far plane이 되도록 하기 위함입니다. 이 방식은 깊이 테스트의 품질을 높이는 기법 중 하나입니다.

여기서 엔진을 실행한다면 원숭이 모델이 약간 깨진 것처럼 보일 수 있습니다. 깊이 테스트를 수행하지 않았기 때문입니다. 따라서 머리 뒤쪽의 삼각형이 앞쪽의 삼각형을 덮어써 잘못된 이미지를 생성한 것입니다. 깊이 테스트를 구현해봅시다.

이제 렌더링 시 drawimage와 함께 사용할 새로운 이미지를 VulkanEngine클래스에 추가해봅시다.

```cpp
AllocatedImage _drawImage;
AllocatedImage _depthImage;
```

이제 drawImage를 따라 init_swapchain함수에서 초기화합니다.

<!-- codegen from tag depthimg on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_engine.cpp --> 
```cpp
	_depthImage.imageFormat = VK_FORMAT_D32_SFLOAT;
	_depthImage.imageExtent = drawImageExtent;
	VkImageUsageFlags depthImageUsages{};
	depthImageUsages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	VkImageCreateInfo dimg_info = vkinit::image_create_info(_depthImage.imageFormat, depthImageUsages, drawImageExtent);

	//allocate and create the image
	vmaCreateImage(_allocator, &dimg_info, &rimg_allocinfo, &_depthImage.image, &_depthImage.allocation, nullptr);

	//build a image-view for the draw image to use for rendering
	VkImageViewCreateInfo dview_info = vkinit::imageview_create_info(_depthImage.imageFormat, _depthImage.image, VK_IMAGE_ASPECT_DEPTH_BIT);

	VK_CHECK(vkCreateImageView(_device, &dview_info, nullptr, &_depthImage.imageView));
```

깊이 이미지는 drawImage와 동일한 방식으로 초기화되지만, Usage플래그에 `VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT`를 전달하고 있으며, 깊이 포맷으로 `VK_FORMAT_D32_SFLOAT`를 사용하고 있습니다.

또한 깊이 이미지를 삭제 큐에 넣는 것을 잊지 마세요.

```cpp
_mainDeletionQueue.push_function([=]() {
	vkDestroyImageView(_device, _drawImage.imageView, nullptr);
	vmaDestroyImage(_allocator, _drawImage.image, _drawImage.allocation);

	vkDestroyImageView(_device, _depthImage.imageView, nullptr);
	vmaDestroyImage(_allocator, _depthImage.image, _depthImage.allocation);
});
```

렌더링 루프에서 깊이 이미지를 drawImage에서와 동일한 방식으로 `VK_IMAGE_LAYOUT_UNDEFINED`에서 `VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL` 모드로 전환할 것입니다. 이는 `draw_geometry()`호출 직전에 수행됩니다.

```
vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
vkutil::transition_image(cmd, _depthImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
```

이제 renderPassBeginInfo를 이 깊이 어태치먼트를 사용하도록 설정하고, 올바르게 초기화해야 합니다. `draw_geometry()` 상단을 아래와 같이 바꿔줍니다.

```cpp
	VkRenderingAttachmentInfo colorAttachment = vkinit::attachment_info(_drawImage.imageView, nullptr, VK_IMAGE_LAYOUT_GENERAL);
	VkRenderingAttachmentInfo depthAttachment = vkinit::depth_attachment_info(_depthImage.imageView, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

	VkRenderingInfo renderInfo = vkinit::rendering_info(_windowExtent, &colorAttachment, &depthAttachment);
```

rendering_info를 위해 vkinit 구조체에 깊이 어태치먼트를 위한 공간은 마련해두었지만, 깊이 값을 올바르게 초기화하기 위한 clear 값도 설정해야 합니다. 이제 depth_attachment_info를 위해 vkinit 구현부를 살펴봅시다.

<!-- codegen from tag depth_info on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_initializers.cpp --> 
```cpp
VkRenderingAttachmentInfo vkinit::depth_attachment_info(
    VkImageView view, VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/)
{
    VkRenderingAttachmentInfo depthAttachment {};
    depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depthAttachment.pNext = nullptr;

    depthAttachment.imageView = view;
    depthAttachment.imageLayout = layout;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.clearValue.depthStencil.depth = 0.f;

    return depthAttachment;
}
```

색상 어태치먼트에서 수행했던 작업과 유사하지만, loadOP를 clear로, 그리고 깊이 초기화 값을 0.f로 설정했습니다. 깊이 0을 far 값으로, 1을 near 값으로 사용할 것입니다.

남은 일은 파이프라인에서 깊이테스트를 활성화하는 것입니다. 작성한 pipelineBuilder에서 깊이 옵션을 만들어 두었지만 활성화하지는 않았습니다. 해당 기능을 채우는 함수를 PipelineBuilder에 추가합니다.

<!-- codegen from tag depth_enable on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_pipelines.cpp --> 
```cpp
void PipelineBuilder::enable_depthtest(bool depthWriteEnable, VkCompareOp op)
{
    _depthStencil.depthTestEnable = VK_TRUE;
    _depthStencil.depthWriteEnable = depthWriteEnable;
    _depthStencil.depthCompareOp = op;
    _depthStencil.depthBoundsTestEnable = VK_FALSE;
    _depthStencil.stencilTestEnable = VK_FALSE;
    _depthStencil.front = {};
    _depthStencil.back = {};
    _depthStencil.minDepthBounds = 0.f;
    _depthStencil.maxDepthBounds = 1.f;
}
```

스텐실 부분은 아직 비워뒀지만, 깊이 테스트를 활성화 한 후 depthOp를 구조체에 전달합니다.

이제 이 설정을 실제 파이프라인 생성에 반영할 차례입니다. `init_mesh_pipeline`을 바꿔봅시다.

```cpp
	//pipelineBuilder.disable_depthtest();
	pipelineBuilder.enable_depthtest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);

	//connect the image format we will draw into, from draw image
	pipelineBuilder.set_color_attachment_format(_drawImage.imageFormat);
	pipelineBuilder.set_depth_format(_depthImage.imageFormat);
```

파이프라인 빌더에서 enable_depthtest 함수를 호출하고, depthWrite를 활성화하며, 연산자로 GREATER_OR_EQUAL을 전달합니다. 앞서 설명한 것처럼 깊이 0이 far이고 1이 near이므로 현재 픽셀이 깊이 이미지의 값보다 더 큰 경우에만 렌더링되도록 설정하는 것입니다.

`init_triangle_pipeline` 함수의 `set_depth_format` 호출 부분도 수정합니다. 비록 깊이 테스트가 비활성화되었더라도 렌더 패스가 올바르게 작동하고 검증레이어가 경고를 출력하지 않도록 정확한 포맷을 설정해야 합니다.

이제 엔진을 실행하면 원숭이 모델이 올바르게 렌더링 될 것입니다. 삼각형과 사각형에는 깊이 테스트 설정이 되지 않았기 때문에 깊이 어태치먼트에서 읽거나 쓰지 않으며, 그 결과 도형들이 원숭이 모델 뒤에 있음에도 앞에 렌더링됩니다.


새롭게 생성된 메시들을 정리하기 위해, cleanup 함수에서 메인 삭제 큐를 정리하기 전에 메시 배열에 포함된 버퍼들을 명시적으로 파괴해 줍니다.

```cpp
for (auto& mesh : testMeshes) {
	destroy_buffer(mesh->meshBuffers.indexBuffer);
	destroy_buffer(mesh->meshBuffers.vertexBuffer);
}

_mainDeletionQueue.flush();
```

다음 작업을 진행하기 전에, 사각형 메시와 배경의 삼각형 코드는 지우도록 하겠습니다. 이제 더이상 필요하지 않습니다. `init_triangle_pipeline` 함수와 관련된 객체들, 그리고 `init_default_data`에 있는 사각형과 관련 코드를 제거합니다. `draw_geometry`에서는 `vkCmdSetViewport`와 `vkCmdSetScissor` 호출을 `vkCmdBindPipeline(cmd,VK_PIPELINE_BIND_POINT_GRAPHICS, _meshPipeline);`호출 이후로 옮겨야 합니다. 삼각형 파이프라인을 바인딩하는 코드가 삭제되었기 때문에, 해당 상태 설정은 파이프라인 바인딩 이후에 수행되어야 합니다.

다음은 투명한 객체와 블렌딩을 설정하겠습니다.

Next: [ Blending]({{ site.baseurl }}{% link docs/ko/new_chapter_3/blending.md %})  

{% include comments.html term="Vkguide 2 Beta Comments" %}

