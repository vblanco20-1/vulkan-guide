---
layout: default
title: Blending
grand_parent: "Korean VKGuide"
parent: "3. Graphics Pipelines - KO"
nav_order: 20
---

파이프라인 빌더를 생성할 때, 블렌딩 로직은 완전히 생략했었습니다. 블렌딩은 투명한 객체나 몇몇 그래픽 효과에 필수적으로 사용되기 때문에 중요한 기능입니다. 따라서 이번에는 지난 글에서 렌더링한 사각형을 투명하게 만들겠습니다.

## 파이프라인 블렌딩
셰이더에서 블렌딩을 제어할 수는 없으며, 이는 파이프라인의 속성입니다. GPU 하드웨어 자체가 블렌딩 연산을 처리하며, 다양한 옵션을 제공합니다. PipelineBuilder에 2가지 새로운 블렌딩 모드를 추가해 보겠습니다. 하나는 단순히 색상을 더하는 addtive blending, 다른 하나는 색상과 알파 값을 섞는 alpha blending 입니다.

PipelineBuilder에 다음 두 함수를 추가합니다.

^code alphablend shared/vk_pipelines.cpp

Vulkan에서 블렌딩 옵션을 설정할 때 색상과 알파값에 각각에 대해 블렌딩 식을 지정해주어야 합니다. 색상과 알파에 매개변수는 동일하게 동작하며, 블렌딩 식은 다음과 같은 방식으로 작동합니다.

```
outColor = srcColor * srcColorBlendFactor <op> dstColor * dstColorBlendFactor;
```
블렌딩에는 여러 연산자가 있지만, 대부분은 가장 기본적이고 안정적으로 작동하는 `VK_BLEND_OP_ADD`를 사용합니다. 그 외에도 다양한 고급 연산자들이 있지만, 대부분 확장이 필요하며 여기서는 다루지 않겠습니다. Source는 파이프라인에서 현재 처리중인 색상을 의미하고, Destination은 우리가 렌더링 중인 이미지의 현재 픽셀 값입니다.

위의 식을 바탕으로 덧셈 블렌딩이 어떻게 작동하는지 설명하겠습니다.

`VK_BLEND_FACTOR_ONE`은 blend factor를 1.0으로 설정하므로, 곱셈 연산 없이 그대로 더해지게 됩니다. `VK_BLEND_FACTOR_SRC_ALPHA`은 반면 Source의 alpha 값을 곱합니다. 이 경우 우리의 블렌딩 공식은 다음과 같은 형태가 됩니다.
```c
outColor = srcColor.rgb * srcColor.a + dstColor.rgb * 1.0
```

알파 블렌딩을 사용할 경우 대신 다음과 같은 모습이 될 것입니다.

```c
outColor = srcColor.rgb * srcColor.a + dstColor.rgb * (1.0 - srcColor.a)
```
본질적으로 srcColor의 alpha값에 의해 제어되는 보간을 수행하는 셈이며, 이 alpha 값은 셰이더에서 전달됩니다.

이걸 사용해 어떤 결과가 나오는지 확인해 봅시다. 셰이더에서는 알파 값을 따로 설정하지 않았으니, addtive를 사용해 봅시다. `init_mesh_pipeline()`함수에서 블렌딩 설정을 변경해 주세요.`

```cpp
//pipelineBuilder.disable_blending();
pipelineBuilder.enable_blending_additive();
```

이제 원숭이 메시가 반투명해진 것을 볼 수 있습니다. 다양한 블렌딩 모드를 시도해보면서 각각이 어떤 효과를 주는지 확인해 보세요.

4장으로 넘어가기 전에 창 크기 조정 기능을 구현해봅시다.

^nextlink

{% include comments.html term="Vkguide 2 Korean Comments" %}