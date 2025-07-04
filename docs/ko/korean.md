---
layout: default
title: Korean VKGuide
nav_order: 99
has_children: true
permalink: /docs/ko
auto_comments: Korean VKGuide
---

![scene]({{site.baseurl}}/diagrams/fullscene.png)
튜토리얼 이후의 예시 사진

## VulkanGuide에 오신 것을 환영합니다.

새롭게 작성중인 Vulkan Guide에 오신 것을 환영합니다. 이 가이드의 목적은 Vulkan을 올바르게 이해하고 프로젝트에서 일할 수 있는 디딤돌 역할을 하는 것입니다. 렌더링 루프를 하드코딩한 대부분의 예제와 다른 Vulkan 가이드와는 달리 여기서는 동적 렌더링에 집중할 것입니다. 게임 엔진으로서 더 나은 기반으로 작동하도록 하는 것입니다.

이 가이드는 게임 렌더링에 집중되었지만, 개념은 CAD와 시각화에도 적용될 수 있습니다.

여기서는 C++20을 사용할 것이지만, 복잡한 C++ 기능을 사용하지는 않을 것이기 때문에 C 혹은 Rust도 사용할 수 있습니다.

OpenGL 혹은 DirectX에 대해 익숙해 3D 그래픽스에 대해 어느정도 알고 있는 사람이 읽는다고 가정합니다. 이 가이드는 선형 대수학과 같은 3D 렌더링 기초를 가르치지는 않을 것입니다.

저자에게 연락하려면 `contact@vkguide.dev`로 이메일을 보내주세요.

이 코드는 Vulkan 1.3의 기능들을 사용해 엔진 아키텍처와 튜토리얼을 단순화합니다. 만약 구버전을 확인하고 싶다면 [여기]({{ site.baseurl }}{% link docs/old_vkguide.md %})서 확인할 수 있습니다. 

이 가이드는 코드 정리를 위해 여러 장으로 나뉘어 있습니다.

- **소개** : Vulkan API와 프로젝트에 사용되는 라이브러리에 대해 설명합니다.
- **0장** : 초기화 코드를 설정합니다.
- **1장** : Vulkan 초기화와 렌더링 루프를 설정합니다.(번쩍이는 색상을 그립니다.)
- **2장** : Vulkan 컴퓨트 셰이더와 그리기를 수행합니다.(컴퓨트 셰이더를 사용해 그리기를 수행합니다.)
- **3장** : Vulkan 메시 드로잉을 수행합니다.(그래픽스 파이프라인을 사용해 메시를 그립니다.)
- **4장** : 텍스쳐와 더 나은 디스크립터 셋 관리를 설명합니다.
- **5장** : 전체 GLTF 씬을 로드하고 고성능의 렌더링을 수행하는 방법을 설명합니다.

이 책의 부록은 본 책에는 담지 않는 더 많은 정보를 포함합니다. 그 중 일부는 vkguide의 이전 버전도 있으며, 최신 버전을 사용하는 경우 구식일 수 있으며 해당 글에는 상단에 표시가 있습니다.

- **GPU 기반 렌더링** : 컴퓨트 셰이더를 사용해 수십만 개의 메시 렌더링하는 법에 대해 설명합니다.
- **부록** : 튜토리얼에서 구현한 엔진의 성능을 올리는 다양한 내용이 있습니다.


Next: [Introduction to vulkan]({{ site.baseurl }}{% link docs/ko/introduction/vulkan_overview.md %})
{% include comments.html term="Index Comments" %}
