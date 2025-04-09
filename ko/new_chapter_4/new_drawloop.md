---
layout: default
title: Meshes and Camera
parent: "4. Textures and Engine Architecture"
nav_order: 14
---

지난 챕터에서 설명한 RenderObjects를 사용해 새로운 렌더링 루프를 구성하는 것부터 시작하겠습니다. 이전에는 GLTF로부터 불러온 메시 목록을 기반으로 렌더링을 하드코딩했지만, 이제는 해당 리스트를 RenderObjects로 변환한 뒤 이를 통해 렌더링할 것입니다. GLTF로부터 아직은 텍스쳐를 불러오는 기능이 없기 때문에 기본 머테리얼을 사용할 것입니다.

아키텍처 구축은 vk_types.h에 기본 씬 노드 구조를 추가하는 것으로 시작하겠습니다.

<!-- codegen from tag node_types on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_types.h --> 
```cpp
struct DrawContext;

// base class for a renderable dynamic object
class IRenderable {

    virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) = 0;
};

// implementation of a drawable scene node.
// the scene node can hold children and will also keep a transform to propagate
// to them
struct Node : public IRenderable {

    // parent pointer must be a weak pointer to avoid circular dependencies
    std::weak_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;

    glm::mat4 localTransform;
    glm::mat4 worldTransform;

    void refreshTransform(const glm::mat4& parentMatrix)
    {
        worldTransform = parentMatrix * localTransform;
        for (auto c : children) {
            c->refreshTransform(worldTransform);
        }
    }

    virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx)
    {
        // draw children
        for (auto& c : children) {
            c->Draw(topMatrix, ctx);
        }
    }
};
```

Node는 우리가 구현하는 첫 IRenderable이 될 것입니다. 노드 트리를 스마트 포인터를 사용하여 구성하며, 부모 포인터는 순환 참조를 방지하기 위해 weak_ptr로 저장하고, 자식 노드들은 shared_ptr로 관리합니다.

Node 클래스는 변환에 필요한 행렬 정보를 저장합니다. 로컬 변환과 월드 변환 모두 포함되며, 로컬 변환이 변경될 경우 월드 변환을 갱신해야 하므로 `refreshTransform()` 함수를 반드시 호출해야 합니다. 이 함수는 노드 트리를 재귀적으로 순회하며 행렬이 올바르게 갱신되도록 합니다.

draw 함수는 직접적인 렌더링은 수행하지 않고 자식 노드들의 Draw()만 호출합니다.

이 기본 Node 클래스는 실제 렌더링을 수행하지 않기 때문에, 메시를 출력할 수 있는 MeshNode 클래스를 vk_engine.h에 추가하겠습니다.

<!-- codegen from tag meshnode on file E:\ProgrammingProjects\vulkan-guide-2\chapter-4/vk_engine.h --> 
```cpp
struct MeshNode : public Node {

	std::shared_ptr<MeshAsset> mesh;

	virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
};
```

MeshNode는 메시 에셋에 대한 포인터를 가지고 있으며, draw 함수를 오버라이드해 drawContext에 드로우 명령을 추가합니다.

이제 DrawContext도 작성합시다. 구현은 vk_engine.h에서 진행합니다.

<!-- codegen from tag renderobject on file E:\ProgrammingProjects\vulkan-guide-2\chapter-4/vk_engine.h --> 
```cpp
struct RenderObject {
	uint32_t indexCount;
	uint32_t firstIndex;
	VkBuffer indexBuffer;

	MaterialInstance* material;

	glm::mat4 transform;
	VkDeviceAddress vertexBufferAddress;
};

struct DrawContext {
	std::vector<RenderObject> OpaqueSurfaces;
};
```

현재 DrawContext는 단순히 RenderObject 구조체의 목록입니다. RenderObject가 렌더링의 핵심 요소입니다. 엔진 자체는 Node 클래스에서 어떠한 Vulkan 함수도 직접 호출하지 않습니다. 대신 렌더러가 DrawContext로부터 RenderObjects의 배열을 받아 매 프레임을 구성(혹은 캐싱)한 후, 각 객체에 대해 하나의 Vulkan 드로우콜을 실행하게 됩니다.

이것이 정의되었으므로, MeshNode의 Draw() 함수는 다음과 같습니다.

<!-- codegen from tag meshdraw on file E:\ProgrammingProjects\vulkan-guide-2\chapter-4/vk_engine.cpp --> 
```cpp
void MeshNode::Draw(const glm::mat4& topMatrix, DrawContext& ctx)
{
	glm::mat4 nodeMatrix = topMatrix * worldTransform;

	for (auto& s : mesh->surfaces) {
		RenderObject def;
		def.indexCount = s.count;
		def.firstIndex = s.startIndex;
		def.indexBuffer = mesh->meshBuffers.indexBuffer.buffer;
		def.material = &s.material->data;

		def.transform = nodeMatrix;
		def.vertexBufferAddress = mesh->meshBuffers.vertexBufferAddress;
		
		ctx.OpaqueSurfaces.push_back(def);
	}

	// recurse down
	Node::Draw(topMatrix, ctx);
}
```

메시는 서로 다른 머테리얼은 갖는 여러 표면을 가질 수 있으므로, 메시의 표면을 순회하면서 각각에 대해 RenderObject를 생성해 목록에 추가합니다. 행렬을 어떻게 처리하는 지에 주목하세요. 객체를 노드의 WorldTransform만으로 삽입하는 것이 아니라, TopMatrix와 곱한 값을 사용하고 있습니다. 이는 Draw() 함수가 여러 번 호출되더라도 동일한 객체를 서로 다른 변환으로 여러 번 렌더링할 수 있다는 뜻이며, 동일한 객체를 여러 위치에 렌더링해야 할 때 매우 유용한 방식입니다.

마지막으로 해야 할 일은 VulkanEngine 클래스에 객체를 그리는 루프를 추가하여 DrawContext를 처리하고 실제 Vulkan 호출로 변환하는 것입니다.

이를 위해 기존에 하드코딩되어 있던 사각형 메시와 원숭이 머리를 그리던 코드를 제거합니다. draw_geometry() 함수에서 첫 번째 삼각형을 그리는 이후의 모든 코드를 제거합니다.

렌더링 목록을 저장하기 위해 DrawContext 구조체를 VulkanEngine 클래스에 추가하고, Vulkan 렌더링 루프 외부에서 노드들의 Draw() 함수를 호출할 `update_scene()` 함수도 추가합니다. 또한 불러온 메시들을 담기 위한 Node 객체의 해시 맵도 추가합니다. 이 함수는 카메라 설정 같은 씬 관련 로직도 함께 처리할 것입니다.

```cpp
class VulkanEngine{
    DrawContext mainDrawContext;
    std::unordered_map<std::string, std::shared_ptr<Node>> loadedNodes;

    void update_scene();
}
```

이 코드를 draw_geometry에 GPUSceneData 디스크립터 셋을 생성한 직후에 추가하여 바인딩할 수 있도록 합니다. 기존에 원숭이 머리를 하드코딩으로 그렸던 코드를 이 코드로 대체하세요. 단, 씬 데이터 디스크립터 셋 할당 부분은 이 코드에서 사용되므로 그대로 남겨둡니다.

```cpp
	for (const RenderObject& draw : mainDrawContext.OpaqueSurfaces) {

		vkCmdBindPipeline(cmd,VK_PIPELINE_BIND_POINT_GRAPHICS, draw.material->pipeline->pipeline);
		vkCmdBindDescriptorSets(cmd,VK_PIPELINE_BIND_POINT_GRAPHICS,draw.material->pipeline->layout, 0,1, &globalDescriptor,0,nullptr );
		vkCmdBindDescriptorSets(cmd,VK_PIPELINE_BIND_POINT_GRAPHICS,draw.material->pipeline->layout, 1,1, &draw.material->materialSet,0,nullptr );

		vkCmdBindIndexBuffer(cmd, draw.indexBuffer,0,VK_INDEX_TYPE_UINT32);

		GPUDrawPushConstants pushConstants;
		pushConstants.vertexBuffer = draw.vertexBufferAddress;
		pushConstants.worldMatrix = draw.transform;
		vkCmdPushConstants(cmd,draw.material->pipeline->layout ,VK_SHADER_STAGE_VERTEX_BIT,0, sizeof(GPUDrawPushConstants), &pushConstants);

		vkCmdDrawIndexed(cmd,draw.indexCount,1,draw.firstIndex,0,0);
	}
```

RenderObject는 설계 당시 Vulkan의 단일 그리기 명령으로 직접 변환되도록 의도되었습니다. 따라서 자원을 바인딩하고 vkCmdDraw를 호출하는 것 외에는 별다른 로직이 없습니다. 현재는 매 그리기 마다 데이터를 바인딩하고 있어 비효율적이지만, 이는 추후에 개선할 예정입니다.

마지막으로 지난 챕터에서 불러온 메시 데이터를 사용해 몇 개의 Node를 생성하고, 이를 그려 메시를 DrawContext에 추가하는 작업이 남아있습니다. loadGLTFMeshes 함수는 머테리얼을 제대로 불러오지 않지만, 기본 머테리얼을 적용해줄 수 있습니다.

우선 vk_loader.h의 GeoSurface 구조체를 수정하여 머테리얼 정보를 담도록 합시다.

```cpp
struct GLTFMaterial {
	MaterialInstance data;
};

struct GeoSurface {
	uint32_t startIndex;
	uint32_t count;
	std::shared_ptr<GLTFMaterial> material;
};
```

다음은 vk_engine.cpp의 `init_default_data`함수에서 기본 머테리얼을 생성한 이후의 마지막 부분을 수정합니다.

<!-- codegen from tag default_meshes on file E:\ProgrammingProjects\vulkan-guide-2\chapter-4/vk_engine.cpp --> 
```cpp
	for (auto& m : testMeshes) {
		std::shared_ptr<MeshNode> newNode = std::make_shared<MeshNode>();
		newNode->mesh = m;

		newNode->localTransform = glm::mat4{ 1.f };
		newNode->worldTransform = glm::mat4{ 1.f };

		for (auto& s : newNode->mesh->surfaces) {
			s.material = std::make_shared<GLTFMaterial>(defaultData);
		}

		loadedNodes[m->name] = std::move(newNode);
	}
```

각 테스트 메시마다 새로운 MeshNode를 생성하고, 해당 메시 에셋을 해당 노드의 shared_ptr로 복사합니다. 기본 머테리얼도 비슷하게 처리합니다.

이는 일반적으로 우리가 객체를 이러한 방식으로 불러오는 것이 아니라, GLTF로부터 노드, 메시, 머테리얼을 올바르게 직접 불러오기 때문입니다. 실제 GLTF 데이터에서는 여러 노드가 하나의 메시를 참조하거나, 여러 메시가 동일한 머테리얼을 참조할 수 있으므로 비록 지금은 불필요해 보일 수 있지만 이런 경우에는 shared_ptr이 필요합니다.

이제 update_scene() 함수를 만들어 봅시다. 지난 챕터에서 원숭이 머리에 적용했던 카메라 로직도 이 함수로 옮겨올 것입니다.

```cpp
void VulkanEngine::update_scene()
{
	mainDrawContext.OpaqueSurfaces.clear();

	loadedNodes["Suzanne"]->Draw(glm::mat4{1.f}, mainDrawContext);	

	sceneData.view = glm::translate(glm::vec3{ 0,0,-5 });
	// camera projection
	sceneData.proj = glm::perspective(glm::radians(70.f), (float)_windowExtent.width / (float)_windowExtent.height, 10000.f, 0.1f);

	// invert the Y direction on projection matrix so that we are more similar
	// to opengl and gltf axis
	sceneData.proj[1][1] *= -1;
	sceneData.viewproj = sceneData.proj * sceneData.view;

	//some default lighting parameters
	sceneData.ambientColor = glm::vec4(.1f);
	sceneData.sunlightColor = glm::vec4(1.f);
	sceneData.sunlightDirection = glm::vec4(0,1,0.5,1.f);
}
```

먼저 DrawContext에서 RenderObject들을 초기화한 후, loadedNode를 순회하면서 메시 이름이 `Suzanne`인 원숭이 메시에 대해 Draw를 호출합니다.

이 함수는 darw() 함수의 가장 처음, 프레임 펜스를 대기하기 전에 호출됩니다.

```cpp
void VulkanEngine::draw()
{
	update_scene();

	//wait until the gpu has finished rendering the last frame. Timeout of 1 second
	VK_CHECK(vkWaitForFences(_device, 1, &get_current_frame()._renderFence, true, 1000000000));
}
```

이제 엔진을 실행해보면, 원숭이 머리가 위에서 비추는 드라마틱한 조명 아래에 렌더링 되는 것을 볼 수 있습니다. 만약 원숭이 머리가 흰색이 아니라 여러 색으로 보인다면, `vk_loader.cpp`의 `OverrideColors`가 false로 설정되어 있는지 확인해보세요.

이제 이를 시연하기 위해 Node를 조작하고 렌더링 동작을 조금 변경해 보겠습니다.

먼저, 큐브를 그리는 Node를 가져와 여러 번 그려 큐브로 이루어진 선을 만들 것입니다. 노드들은 해시 맵에 저장되어 있으므로, 원하는 방식대로 개별적으로 접근하고 렌더링할 수 있습니다.

이 작업은 update_scene 함수에 추가합니다.
```cpp
	for (int x = -3; x < 3; x++) {

		glm::mat4 scale = glm::scale(glm::vec3{0.2});
		glm::mat4 translation =  glm::translate(glm::vec3{x, 1, 0});

		loadedNodes["Cube"]->Draw(translation * scale, mainDrawContext);
	}
```

큐브를 크기를 작게 줄인 뒤, 화면의 왼쪽에서 오른쪽으로 이동시키는 변환을 적용합니다. 그런 다음 Draw를 호출합니다. Draw가 호출될 때 마다 서로 다른 행렬을 가진 RenderObject가 DrawContext에 추가되므로, 같은 객체를 여러 위치에 렌더링할 수 있습니다.

이것으로 4장이 끝났습니다. 다음 장에서는 GLTF 로더를 업그레이드하여 텍스쳐와 여러 객체가포함된 씬을 불러오고, 상호작용 가능한 FPS 카메라를 설정해보겠습니다.

다음 글 : [ 5장 : 상호작용 카메라 ]({{ site.baseurl }}{% link docs/new_chapter_5/interactive_camera.md %})


{% include comments.html term="Vkguide 2 Beta Comments" %}
