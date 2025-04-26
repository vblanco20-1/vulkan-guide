---
layout: default
title: Engine Architecture
parent: "4. Textures and Engine Architecture- KO"
grand_parent: "Korean VKGuide"
nav_order: 10
---

# 엔진 아키텍처
엔진을 구성하는 데 필요한 로우 레벨 기법은 이미 갖춰졌습니다. 임의의 메시를 그릴 수 있고, 버퍼나 텍스쳐 같은 데이터를 셰이더에 전달할 수 있습니다. 이제 필요한 것은 객체를 렌더링하기 위한 제대로된 아키텍처를 구축하고 VulkanEngine 클래스에 하드코딩된 구조체 사용을 지양하는 것입니다.

다음 장에서 GLTF 파일로부터 동적으로 전체 씬을 불러올 것이므로, 여기서의 아키텍쳐는 GLTF를 어느 정도 모방하게 됩니다. 우리는 GLTF의 PBR 사양을 기반으로 기본적인 GLTF 파이프라인을 만들고, 첫 번째 메시 클래스를 정의할 것입니다.

이 아키텍처는 두 단계의 구조체를 중심으로 구성됩니다. 그 중 하나는 RenderObject 구조체입니다.

```cpp
struct RenderObject {
    uint32_t indexCount;
    uint32_t firstIndex;
    VkBuffer indexBuffer;
    
    MaterialInstance* material;

    glm::mat4 transform;
    VkDeviceAddress vertexBufferAddress;
};
```

이 구조체는 VkCmdDrawIndexed 호출에 필요한 모든 파라미터를 평탄화한 추상화 구조입니다. 인덱싱에 필요한 정보와 함께, 특정 머테리얼에 해당하는 파이프라인과 디스크립터 셋을 가리키는 MaterialInstance 포인터를 포함합니다. 이어서 3D 렌더링에 필요한 행렬과 정점 버퍼의 포인터가 있으며, 이 두 요소는 객체마다 바뀌는 동적 데이터이므로 푸시 상수를 통해 전달됩니다.

이 구조체는 매 프레임 동적으로 작성되며, 렌더링 로직은 RenderObject 구조체 배열을 순회하면서 그리기 명령을 직접 기록하게 됩니다.

MaterialInstance 구조체는 다음과 같습니다.
```cpp
struct MaterialPipeline {
	VkPipeline pipeline;
	VkPipelineLayout layout;
};

struct MaterialInstance {
    MaterialPipeline* pipeline;
    VkDescriptorSet materialSet;
    MaterialPass passType;
};
```

머테리얼 시스템에는 두 개의 파이프라인, 즉 GLTF PBR 불투명용과 투명용으로 하드코딩될 것입니다. 두 파이프라인은 동일한 정점 및 프래그먼트 셰이더 쌍을 공유하며, 디스크립터 셋은 총 2개만 사용할 예정입니다. 슬롯 0번은 카메라 및 환경 정보와 같은 전역 데이터를 담는 디스크립터 셋으로, 한번 바인딩된 후 모든 드로우 콜에서 재사용됩니다. 이후 조명 정보 등도 추가할 예정입니다. 슬롯 1번은 머테리얼별 디스크립터 셋으로, 텍스쳐 및 머테리얼 파라미터를 바인딩하는 역할을 합니다. GLTF 사양을 그대로 반영하여, PBR GLTF 머테리얼이 요구하는 텍스쳐 외에도 객체 색상 등의 색상 상수를 포함하는 유니폼 버퍼를 사용할 것입니다. GLTF PBR 머테리얼은 특정 텍스쳐가 없어도 유효하지만, 이 경우 용도에 따라 기본 흰색 혹은 검은색 텍스쳐를 바인딩하게 됩니다. MaterialInstance 구조체는 MaterialPass 열거형을 갖는데 이는 불투명 객체와 투명 객체를 구분하는 데 사용됩니다.

2개의 파이프라인만 사용하는 이유는 파이프라인 수를 가능한 적게 유지하기 위함입니다. 파이프라인 수가 적으면 시작 시 사전 로딩이 가능하고, 특히 바인드리스 렌더링이나 draw-indirect 렌더링 로직을 도입했을 때 렌더링 속도가 향상됩니다. 우리의 목표는 GLTF PBR 머테리얼과 같은 각 머테리얼 타입에 대해 소수의 파이프라인만을 사용하는 것입니다. 렌더링 엔진이 필요로 하는 파이프라인 수는 성능에 큰 영향을 미칩니다. 예를 들어 둠 이터널 엔진과 같은 경우는 게임에 약 200여개의 파이프라인만 사용하지만, 언리얼 엔진 기반의 프로젝트인 경우 종종 10만 개 이상의 파이프라인을 사용합니다. 이처럼 지나치게 많은 파이프라인을 컴파일 하는 것은 렌더링 중 스터터링을 유발하고, 많은 저장 공간을 차지하며, draw-indirect와 같은 고급 기능의 구현에도 제약을 줍니다. 
 
RenderObject는 매우 로우 레벨의 구조이므로, 이를 생성하고 관리하기 위한 구조가 필요합니다. 여기에는 씬 그래프를 도입하겠습니다. 이렇게 하면 메시 간의 부모-자식 관계를 표현할 수 있으며, 메시가 없는 빈 노드도 가질 수 있습니다. 이는 게임 엔진에서 레벨을 구성하기 위한 전형적인 방식입니다.

이번에 설계할 씬 그래프는 중간 또는 다소 낮은 성능 구조이지만, 추후에 개선할 예정이며 매우 유연하고 확장성이 뛰어납니다. 현재 설계만으로도 수만 개의 객체 렌더링에는 충분한 성능을 제공합니다.

```cpp
// base class for a renderable dynamic object
class IRenderable {

    virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) = 0;
};
```

단일 Draw() 함수만을 정의하는 IRenderable 인터페이스가 있습니다. 이 함수는 부모로 사용할 행렬과 RenderContext를 인자로 받습니다. RenderContext는 현재로서는 렌더링할 객체들의 배열일 뿐입니다. 이 구조의 아이디어는 Draw() 함수가 호출될 때, 해당 객체가 이번 프레임에 렌더링할 항목에 Renderables을 추가하는 것입니다. 이 방식은 흔히 Immediate Design이라 불리며, 가장 큰 장점은 동일한 객체를 여러 번 다른 행렬을 사용하여 한 프레임 안에서 중복 렌더링할 수 있다는 점입니다. 또한 특정 로직에 따라 어떤 프레임에서는 Draw()를 호출하지 않음으로써 해당 객체를 렌더링하지 않을 수도 있습니다. 이 접근법은 자원 관리 및 객체 수명 관리가 훨씬 단순해지기 때문에 동적인 객체를 다룰 때 매우 유용하며, 코드 작성 또한 비교적 간단합니다. 단점으로는, 프레임마다 여러 객체를 순회하며 가상 함수를 호출하게 되는데, 객체 수가 많아질수록 이러한 호출 비용이 누적된다는 점입니다.

Node 클래스는 IRenderable을 상속받을 것이며, 고유의 변환 행렬과 자식 노드의 배열을 가질 것입니다. Draw()가 호출될 때 자식 노드들의 Draw()도 호출하게 됩니다.

MeshNode 클래스를 만들 것이며, 이는 Node를 상속받을 것입니다. 이 클래스는 렌더링에 필요한 자원들을 보유하고 있으며, Draw()가 호출되면 RenderObject를 생성해 DrawContext에 추가하게 됩니다.

조명과 같은 다른 렌더링 요소를 추가할 때도 동일한 구조로 동작합니다. DrawContext는 조명들의 목록을 유지하고, LightNode는 조명이 활성화되어 있을 경우 자신의 파라미터를 해당 목록에 추가합니다. 지형, 파티클 등 다른 렌더링 객체도 동일한 방식으로 처리할 수 있습니다.

우리가 사용할 한 가지 트릭은 GLTF를 추가할 때 LoadedGLTF라는 클래스를 만드는 것입니다. 이 클래스는 Node가 아닌 IRenderable을 직접 상속하며, 특정 GLTF 파일에 포함된 전체 상태와 텍스쳐, 메시같은 모든 자원을 포함합니다. 그리고 Draw()가 호출되면 해당 GLTF의 내용을 렌더링합니다. 비슷한 방식으로 OBJ와 같은 다른 포맷도 처리할 수 있어 유용합니다.


GLTF를 불러오는 것은 다음 장에서 할 예정이지만, 지금은 RenderObject와 GLTF 머테리얼 처리 방식을 준비할 것입니다.

^nextlink

{% include comments.html term="Vkguide 2 Beta Comments" %}