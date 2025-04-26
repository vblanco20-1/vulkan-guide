---
layout: default
title: Faster Draw
parent: "5. GLTF loading - KO"
grand_parent: "Korean VKGuide"
nav_order: 10
---

엔진이 정상적으로 작동하긴 하지만, 여전히 비효율적인 부분이 존재합니다. 이제 이러한 부분들을 개선해 보겠습니다.

## 타이머 UI
성능 최적화를 시작하기 전에, 각 작업이 얼마나 빠르게 수행되는지 측정할 수 있는 수단이 필요합니다. 이를 위해 std::chrono와 ImGui를 활용하여 간단한 벤치마킹 타이머 UI를 설정하겠습니다. 원한다면 Trancy와 같은 도구를 사용하는 것도 가능하지만, 여기서는 기본적인 UI 기반 타이머를 구현하는 방식으로 진행하겠습니다. GPU측 성능 프로파일링은 파이프라인 쿼리 등 복잡한 설정이 필요하므로 이번에는 다루지 않습니다. 우리가 필요한 수준에서는 NSight와 같은 GPU 프로파일링 도구를 사용하는 편이 좋습니다.

먼저 시간 관련 정보를 저장할 구조체를 vk_engine.h에 추가해봅시다.

```cpp
struct EngineStats {
    float frametime;
    int triangle_count;
    int drawcall_count;
    float scene_update_time;
    float mesh_draw_time;
};
```

VulkanEngine 클래스에 `EngineStats stats;` 멤버를 추가합니다.

프레임 시간은 전체적인 시간을 측정하는 데 사용되며, VSync를 사용하고 있기 때문에 대부분 모니터 주사율에 맞춰져 있을 것입니다. 하지만 그 외의 값들은 성능 측정에 더 유용하게 쓰일 수 있습니다.

그럼 프레임 시간을 계산해봅시다.

엔진의 메인 루프인 `run()`에 아래 코드를 추가합니다.
```cpp
// main loop
while (!bQuit) {
    //begin clock
    auto start = std::chrono::system_clock::now();

    //everything else

    //get clock again, compare with start clock
    auto end = std::chrono::system_clock::now();
     
     //convert to microseconds (integer), and then come back to miliseconds
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    stats.frametime = elapsed.count() / 1000.f;
}
```

`auto start = std::chrono::system_clock::now();`를 사용하면 현재 시간을 높은 정밀도로 측정할 수 있습니다. 이 값을 이후 다시 호출된 시점의 시간과 비교하면, 특정 구간이 실행되는 데 얼마나 걸렸는지를 알 수 있습니다. 이를 밀리초(ms) 단위의 시간으로 변환하려면, 먼저 마이크로초로 캐스팅한 뒤 1000.f를 곱해야 합니다. 이렇게 하면 소수점 셋째 자리까지 표현되는 값이 됩니다.

draw_geometry 함수에서 해당 코드를 추가하고 동시에 드로우콜 횟수와 삼각형 수를 계산하는 코드도 넣어보겠습니다.

```cpp
void VulkanEngine::draw_geometry(VkCommandBuffer cmd)
{
    //reset counters
    stats.drawcall_count = 0;
    stats.triangle_count = 0;
    //begin clock
    auto start = std::chrono::system_clock::now();

    /* code */

    auto draw = [&](const RenderObject& r) {

        /* drawing code */

        vkCmdDrawIndexed(cmd, draw.indexCount, 1, draw.firstIndex, 0, 0);

        //add counters for triangles and draws
        stats.drawcall_count++;
        stats.triangle_count += draw.indexCount / 3;   
    }

    /* code */


    auto end = std::chrono::system_clock::now();

    //convert to microseconds (integer), and then come back to miliseconds
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    stats.mesh_draw_time = elapsed.count() / 1000.f;
}
```

시작할 때 카운터를 0으로 초기화합니다. 그런 다음 draw 람다에서 그리기 로직 이후에 드로우콜 횟수와 삼각형 개수를 누적합니다. 함수의 마지막에는 최종 시간을 측정하여 stats 구조체에 저장합니다.

동일한 작업을 `update_scene()`함수에서도 수행하되, 시작/종료 시간을 측정해 `scene_update_time`에 저장합니다.

이제 ImGui를 사용해 정보를 화면에 표시해야 합니다.

`run()` 함수에서 `ImGui::NewFrame()`과 `ImGui::Render()` 사이에 아래 코드를 추가하여 새로운 ImGui 창을 그립니다.

```cpp
        ImGui::Begin("Stats");

        ImGui::Text("frametime %f ms", stats.frametime);
        ImGui::Text("draw time %f ms", stats.mesh_draw_time);
        ImGui::Text("update time %f ms", stats.scene_update_time);
        ImGui::Text("triangles %i", stats.triangle_count);
        ImGui::Text("draws %i", stats.drawcall_count);
        ImGui::End();
```

여기서 엔진을 실행한다면 각 정보를 확인할 수 있습니다. 현재는 검증 레이어가 활성화되어 있고, 디버그 모드가 켜져있을 가능성이 높습니다. 컴파일러 설정에서 릴리즈 모드를 활성화하고, `constexpr bool bUseValidationLayers = true` 값을 false로 변경해 검증 레이어를 비활성화하세요. 라이젠 5950x CPU 기준으로 검증 레이어를 비활성화하면 draw_time이 약 6.5ms에서 0.3ms로 줄어듭니다. 씬이 올바르게 렌더링되고 있다면 드로우콜 횟수는 1700으로 표시될 것입니다.



## 정렬 후 그리기
현재 우리는 매 드로우콜마다 파이프라인 등을 반복해서 바인딩하고 있기 때문에, 필요 이상으로 많은 Vulkan 호출을 수행하고 있습니다. 바인딩된 상태를 추적하고, 상태가 실제로 변경될 때에만 해당 Vulkan 함수를 호출하도록 해야 합니다. 

이전 글에서 소개했던 draw() 람다를 수정하여 상태 추적 기능을 추가할 것입니다. 파라미터가 변경된 경우에만 Vulkan 함수를 호출하도록 구현하겠습니다.

```cpp
//defined outside of the draw function, this is the state we will try to skip
 MaterialPipeline* lastPipeline = nullptr;
 MaterialInstance* lastMaterial = nullptr;
 VkBuffer lastIndexBuffer = VK_NULL_HANDLE;

 auto draw = [&](const RenderObject& r) {
     if (r.material != lastMaterial) {
         lastMaterial = r.material;
         //rebind pipeline and descriptors if the material changed
         if (r.material->pipeline != lastPipeline) {

             lastPipeline = r.material->pipeline;
             vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, r.material->pipeline->pipeline);
             vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,r.material->pipeline->layout, 0, 1,
                 &globalDescriptor, 0, nullptr);

            VkViewport viewport = {};
            viewport.x = 0;
            viewport.y = 0;
            viewport.width = (float)_windowExtent.width;
            viewport.height = (float)_windowExtent.height;
            viewport.minDepth = 0.f;
            viewport.maxDepth = 1.f;

            vkCmdSetViewport(cmd, 0, 1, &viewport);

            VkRect2D scissor = {};
            scissor.offset.x = 0;
            scissor.offset.y = 0;
            scissor.extent.width = _windowExtent.width;
            scissor.extent.height = _windowExtent.height;

            vkCmdSetScissor(cmd, 0, 1, &scissor);
         }

         vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, r.material->pipeline->layout, 1, 1,
             &r.material->materialSet, 0, nullptr);
     }
    //rebind index buffer if needed
     if (r.indexBuffer != lastIndexBuffer) {
         lastIndexBuffer = r.indexBuffer;
         vkCmdBindIndexBuffer(cmd, r.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
     }
     // calculate final mesh matrix
     GPUDrawPushConstants push_constants;
     push_constants.worldMatrix = r.transform;
     push_constants.vertexBuffer = r.vertexBufferAddress;

     vkCmdPushConstants(cmd, r.material->pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);

     vkCmdDrawIndexed(cmd, r.indexCount, 1, r.firstIndex, 0, 0);
    //stats
    stats.drawcall_count++;
    stats.triangle_count += r.indexCount / 3;
 };
```

마지막으로 사용한 파이프라인, 머테리얼, 인덱스 버퍼를 저장합니다. 먼저 파이프라인이 바뀌었는지 확인하고, 바뀌었다면 파이프라인을 다시 바인딩하여 전역 디스크립터 셋도 함께 바인딩합니다. 이때 SetViewport와 SetScissor 명령도 다시 호출해야 합니다.

그 다음, 머테리얼이 변경되면 머테리얼 파라미터와 텍스쳐를 위한 디스크립터 셋을 바인딩합니다. 마지막으로 인덱스 버퍼가 변경되면 해당 인덱스 버퍼도 다시 바인딩합니다.

이제 단 2개의 파이프라인만 사용되므로 vkCmdBindPipeline 호출이 대폭 줄어들어 성능 향상을 기대할 수 있습니다. 하지만 여기서 더 최적화해보겠습니다. 라이젠 5950x 컴퓨터 기준으로 draw_geometry의 실행 시간이 절반으로 단축되었습니다.

호출 수를 최소화하기 위해 렌더링할 객체들을 이러한 파라미터 기준으로 정렬해야 합니다. 이 정렬은 불투명 객체에만 수행하겠습니다. 투명한 객체들은 깊이 정렬이 필요하지만, 아직은 해당 정보를 가지고 있지 않기 때문입니다.

정렬을 구현할 때, 객체 자체가 크기 때문에 draw 배열 그 자체를 정렬하지는 않을 것입니다. 대신, 해당 배열을 참조하는 인덱스 배열을 정렬하겠습니다. 이는 대형 엔진에서도 일반적으로 사용되는 기법입니다.

draw_geometry() 함수의 시작 부분에 다음 코드를 추가하세요.

```cpp
std::vector<uint32_t> opaque_draws;
opaque_draws.reserve(mainDrawContext.OpaqueSurfaces.size());

for (uint32_t i = 0; i < mainDrawContext.OpaqueSurfaces.size(); i++) {
    opaque_draws.push_back(i);
}

// sort the opaque surfaces by material and mesh
std::sort(opaque_draws.begin(), opaque_draws.end(), [&](const auto& iA, const auto& iB) {
    const RenderObject& A = mainDrawContext.OpaqueSurfaces[iA];
    const RenderObject& B = mainDrawContext.OpaqueSurfaces[iB];
    if (A.material == B.material) {
        return A.indexBuffer < B.indexBuffer;
    }
    else {
        return A.material < B.material;
    }
});
```

std::algorithms에는 opaque_draws 벡터를 정렬하는 데 유용한 정렬 함수가 있습니다. 여기에 `<` 연산자를 정의한 람다를 전달하면 효율적으로 정렬할 수 있습니다.

먼저 draw 배열의 인덱스를 기준으로 머테리얼이 같은지 확인하고, 같다면 인덱스 버퍼를 기준으로 정렬합니다. 만약 머테리얼이 다르다면 머테리얼 포인터 그 자체를 비교합니다. 이를 수행하는 또 다른 방법은 정렬 키를 계산하는 것입니다. 이 경우 opaque_draws는 20비트의 인덱스와 44비트의 정렬 키(또는 해시)로 구성될 수 있습니다. 이 방식은 보다 빠른 정렬 알고리즘을 사용할 수 있기 때문에 성능상 더 유리합니다.

이제 정렬된 배열을 기반으로 드로우를 수행합니다. 기존의 불투명 객체 드로우 반복문을 아래 코드로 대체합시다.

```cpp
for (auto& r : opaque_draws) {
    draw(mainDrawContext.OpaqueSurfaces[r]);
}
```


이렇게 하면 렌더러가 디스크립터 셋 바인딩 수를 최소화할 수 있습니다. 머테리얼 단위로 처리하기 때문입니다. 인덱스 버퍼 바인딩은 여전히 처리해야 하지만, 이는 비교적 빠르게 전환됩니다.

이렇게 하면 엔진의 성능이 약간 향상될 수 있습니다. 하지만 하나의 씬만 렌더링하는 경우 정렬 비용 때문에 성능 향상 효과가 거의 없을 수 있습니다. 현재 우리는 비효율적인 단일 쓰레드 정렬을 수행하고 있기 때문에, 정렬 비용이 Vulkan 호출 감소로 얻는 성능 향상을 상쇄할 수도 있습니다. 반드시 씬을 프로파일링하여 이 기능을 활성화할지 여부를 결정하세요.

성능이 개선되었지만 아직 개선할 부분이 남아있습니다. 현재 우리는 카메라 뒤에 있는 객체들까지 렌더링하고 있어, CPU와 GPU 모두에서 불필요하게 많은 객체를 처리하고 있습니다. 이를 프러스텀 컬링을 통해 개선할 수 있습니다.



## 프러스텀 컬링
현재는 맵에 있는 모든 객체를 렌더링하고 있지만, 시야 밖에 있는 것들까지 그릴 필요는 없습니다. 우리는 이미 그릴 객체의 목록을 가지고 있으므로, 이를 필터링하여 시야 안에 들어오는 객체만 렌더링하고, 그렇지 않은 객체는 건너뛰겠습니다. 필터링 비용이 객체를 렌더링하는 비용보다 적기만 하다면, 성능상의 이득을 얻을 수 있습니다.

프러스텀 컬링을 수행하는 방법에는 여러 가지가 있지만, 현재 우리가 가진 데이터와 아키텍처에 적합한 방식은 OBB(Oriented Bounding Box)를 사용하는 것입니다. 각 GeoSurface마다 바운딩 박스를 계산하고, 그것이 시야에 들어오는지 확인할 것입니다.

vk_loaders.h의 구조체에 바운딩 정보를 추가하세요.
```cpp
struct Bounds {
    glm::vec3 origin;
    float sphereRadius;
    glm::vec3 extents;
};

struct GeoSurface {
    uint32_t startIndex;
    uint32_t count;
    Bounds bounds;
	std::shared_ptr<GLTFMaterial> material;
};
```

RenderObject에 Bounds 구조체를 추가합니다.

```cpp
struct RenderObject {
    uint32_t indexCount;
    uint32_t firstIndex;
    VkBuffer indexBuffer;
    
    MaterialInstance* material;
    Bounds bounds;
    glm::mat4 transform;
    VkDeviceAddress vertexBufferAddress;
};
```

Bounds 구조체는 중심점, 크기, 구의 반지름을 포함합니다. 구의 반지름은 다른 프러스텀 컬링 알고리즘이나 그 외 다른 용도로도 사용될 수 있습니다.

이를 계산하기 위해, 해당 로직을 loader 코드에 추가해야 합니다.

이 코드는 메시 데이터를 불러오는 반복문의 끝부분에 있는 loadGLTF 함수 내부에 작성할 것입니다.

```cpp
//code that writes vertex buffers

//loop the vertices of this surface, find min/max bounds
glm::vec3 minpos = vertices[initial_vtx].position;
glm::vec3 maxpos = vertices[initial_vtx].position;
for (int i = initial_vtx; i < vertices.size(); i++) {
    minpos = glm::min(minpos, vertices[i].position);
    maxpos = glm::max(maxpos, vertices[i].position);
}
// calculate origin and extents from the min/max, use extent lenght for radius
newSurface.bounds.origin = (maxpos + minpos) / 2.f;
newSurface.bounds.extents = (maxpos - minpos) / 2.f;
newSurface.bounds.sphereRadius = glm::length(newSurface.bounds.extents);

newmesh->surfaces.push_back(newSurface);
```

MeshNode::Draw() 함수에서 Bounds가 RenderOBject로 복사되도록 해야 합니다. 이는 다음과 같이 보일 것입니다.

```cpp
void MeshNode::Draw(const glm::mat4& topMatrix, DrawContext& ctx) {
    glm::mat4 nodeMatrix = topMatrix * worldTransform;

    for (auto& s : mesh->surfaces) {
        RenderObject def;
        def.indexCount = s.count;
        def.firstIndex = s.startIndex;
        def.indexBuffer = mesh->meshBuffers.indexBuffer.buffer;
        def.material = &s.material->data;
        def.bounds = s.bounds;
        def.transform = nodeMatrix;
        def.vertexBufferAddress = mesh->meshBuffers.vertexBufferAddress;

        if (s.material->data.passType == MaterialPass::Transparent) {
            ctx.TransparentSurfaces.push_back(def);
        } else {
            ctx.OpaqueSurfaces.push_back(def);
        }
    }

    // recurse down
    Node::Draw(topMatrix, ctx);
}
```

이제 GeoSurface에 Bounds가 설정되었으므로, RenderObject가 시야에 들어오는 지 확인해야 합니다. 아래 함수를 vk_engine.cpp에 전역 함수로 추가하세요.

^code visfn chapter-5/vk_engine.cpp

이는 프러스텀 컬링을 수행할 수 있는 여러 방법 중 하나입니다. 이 방식은 메시 공간의 바운딩 박스 8개의 꼭짓점을 객체 행렬과 뷰 프로젝션 행렬을 사용해 스크린 공간으로 변환하여 작동합니다. 변환된 꼭짓점으로부터 스크린 공간의 바운딩 박스를 계산한 뒤, 해당 박스가 클립 공간 뷰 안에 있는지 확인합니다. 이러한 경계 계산 방식은 다른 공식들에 비해 다소 느린 편이며, 실제로는 보이지 않는 객체를 보이는 것으로 판단하는 false positive가 발생할 수 있습니다. 각 방식마다 트레이드오프가 존재하며, 이 방식은 단순성과 정점 셰이더에서 수행하는 작업과의 유사성 때문에 선택하였습니다.

이 함수를 사용하려면, opaque_draws 배열을 채우는 루프를 수정해야 합니다.

```cpp
for (int i = 0; i < mainDrawContext.OpaqueSurfaces.size(); i++) {
	if (is_visible(mainDrawContext.OpaqueSurfaces[i], sceneData.viewproj)) {
		opaque_draws.push_back(i);
	}
}
```

이제 `i`를 추가하는 대신, 객체가 시야에 들어오는 지 확인합니다.

이제 렌더러는 시야 외부의 객체들을 건너뛸 것입니다. 화면에 보이는 결과는 동일하지만, 드로우콜 수가 줄어들고 성능은 더 빨라질 것입니다. 만약 시각적 이상이 발생한다면, `GeoSurface`의 바운딩 박스 생성 과정과 is_visible 함수에 오타가 없는지 다시 확인해보세요

투명한 객체에 대해서도 동일한 컬링과 정렬을 적용하는 코드는 건너뛰었습니다. 불투명 객체에 적용한 방식과 동일하므로 직접 구현해보세요.

투명한 객체의 경우, 경계와 카메라 간의 거리를 기준으로 정렬하는 방식으로 정렬 코드를 변경해야 더 정확하게 렌더링할 수 있습니다. 다만 깊이 기준 정렬은 파이프라인 기준 정렬과 호환되지 않기 때문에, 어떤 방식이 자신의 상황에 더 적합한지 결정해야 합니다.

카메라를 회전시키면서 스탯 창에서 드로우콜 수와 삼각형 개수가 변화하는 것을 확인할 수 있을 것입니다. 특히 객체를 바라보지 않는 상황에서는 성능이 눈에 띄게 향상될 것입니다.

## 밉맵 생성하기

텍스쳐를 불러올 때 밉맵을 생성하지는 않았습니다. OpenGL과는 달리, Vulkan에서는 밉맵을 자동으로 생성해주는 단일 호출 함수가 존재하지 않기 때문에, 우리가 직접 생성해주어야 합니다.

`create_image`는 이미 밉맵을 지원하도록 작성되어 있지만, 실제 데이터를 업로드 하는 것을 수정하여 밉맵을 생성하도록 해야 합니다. 이를 위해 해당 함수를 변경하겠습니다.

^code create_mip_2 chapter-5/vk_engine.cpp

`immediate_submit`는 이제 해당 이미지에 밉맵을 사용하고 싶을 경우 `vkutil::generate_mipmaps()`함수를 호출할 수 있습니다. 이 함수를 vk_images.h에 추가합시다.

```cpp
namespace vkutil {
void generate_mipmaps(VkCommandBuffer cmd, VkImage image, VkExtent2D imageSize);
}
```

밉맵을 생성하는 방법에는 여러 가지가 있습니다. 반드시 불러오는 시점에 생성할 필요는 없으며, 밉맵이 사전 생성되어 있는 KTX 혹은 DDS 같은 포맷을 사용하는 것도 가능합니다. 가장 많이 사용되는 방법 중 하나는 컴퓨트 셰이더를 이용해 여러 레벨을 한 번에 생성하는 방식이며, 이는 성능 향상에 도움이 될 수 있습니다. 우리가 사용하는 방식은 VkCmdImageBlit 호출을 연속으로 수행하여 밉맵을 생성하는 것입니다

각 밉맵 레벨에 대해, 이전 레벨의 이미지를 다음 레벨로 복사하면서 해상도를 절반으로 줄여야 합니다. 이 과정에서 복사 원본인 밉맵 레벨의 레이아웃을 `VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL`로 전환해야 합니다. 모든 복사가 완료되면 전체 밉맵 레벨에 대해 또 다른 배리어를 추가하고, 이미지의 레이아웃을 `VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL`로 전환해야 합니다.

의사 코드는 다음과 같습니다.

```cpp
//image already comes with layout VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL on all mipmap levels from image creation

int miplevels = calculate_mip_levels(imageSize);
for (int mip = 0; mip < mipLevels; mip++) {

    barrier( image.mips[mip] , VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)

    //not the last level
    if (mip < mipLevels - 1)
    {
        copy_image(image.mips[mip], image.mips[mip+1];)
    }
}

barrier( image , VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
```

실제 코드를 살펴봅시다.

^code mipgen shared/vk_images.cpp

배리어는 `transition_image`에서와 유사하며, blit 또한 `copy_image_to_image`에서 했던 것과 유사하지만, 밉맵 레벨을 처리한다는 점만 다릅니다. 이 함수는 두 기능을 결합한 형태라고 볼 수 있습니다
 
각 반복문에서는 이미지 크기를 절반으로 나누어 가며, 복사할 밉맵 레벨을 전환하고, 현재 밉맵 레벨에서 다음 레벨로 VkCmdBlit을 수행합니다.

vk_loader.cpp의 `load_image`에 있는 `create_image` 호출 시 마지막 인자를 true로 설정했는지 확인하여, 밉맵이 실제로 생성되도록 해주세요.
 
이렇게 하면 필요한 밉맵이 자동으로 생성됩니다. 이미 샘플러는 올바른 설정으로 생성되어 있으므로, 별도의 수정 없이 잘 작동할 것입니다.

{% include comments.html term="Vkguide 2 Korean Comments" %}