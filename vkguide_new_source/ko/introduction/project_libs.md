---
layout: default
title: Project layout and libraries
parent: introduction
nav_order: 99
---

## 프로젝트 레이아웃

Vulkan 엔진에서는 다음과 같은 폴더 레이아웃을 사용할 것입니다.

- `/assets`에는 이 가이드에서 사용할 텍스쳐와 3D 모델이 들어있습니다.
- `/bin`은 실행 가능한 파일이 빌드될 곳입니다. 에셋과 셰이더의 경로를 단순하게 하기 위해 일반적인 CMake 빌드 폴더를 사용하지 않습니다.
- `/shaders`는 셰이더와 컴파일된 셰이더가 출력될 곳입니다.
- `/chapter-N`에는 이 가이드의 각 챕터의 코드가 들어있습니다.
- `/third_party`에는 우리가 사용할 모든 라이브러리가 들어있으며, 소스코드에서 바로 사용가능할 수 있습니다. 

## 라이브러리

엔진에서 대부분의 라이브러리는 `/third_part`에 들어가 있습니다. 유일하게 내장되지 않는 예외는 SDL입니다.

모든 써드파티 라이브러리가 내장되어 있으며, CMake가 자동으로 빌드합니다. SDL은 프로젝트와 별도로 빌드해야 하며, CMake에게 그 위치를 알려주어야 합니다. 

사용할 라이브러리의 목록은 다음과 같습니다.
- [GLM (openGL Mathematics)](https://github.com/g-truc/glm) Header only 수학 라이브러리입니다. GLM을 사용해 행렬과 벡터 연산을 수행할 것입니다. 대부분의 상황에서 GLSL 타입과 직접적으로 호환되는 타입들을 갖고 있습니다. 예를 들어, `glm::mat4`는 셰이더에서의 `mat4`와 직접적으로 호환되며 유사한 연산을 할 수 있습니다. 이름에 OpenGL이 들어가 있지만 Vulkan에서도 잘 작동합니다.

- [SDL](https://www.libsdl.org/) 창(window)와 입력을 받는 라이브러리입니다. 개별적으로 빌드해야 합니다. SDL은 창을 만들어 입력 장치에 접근하는 가장 널리 쓰이는 것 중 하나입니다. SDL은 대부분의 플랫폼을 지원하며 작동합니다. 언리얼 엔진, 유니티, 소스, 그 이외의 다양한 곳에서 사용됩니다. 이 프로젝트에서는 창을 만들고 키보드 입력과 같은 세부사항을 쉽고 빠르게 관리하는 방법으로 SDL을 사용할 것입니다.

- [dear IMGUI](https://github.com/ocornut/imgui) 사용하기 쉬운 GUI 라이브러리입니다. 이는 슬라이더와 창과 같은 편집 가능한 위젯을 생성할 수 있게 해줍니다. 게임 산업에서 디버그 툴로 널리 사용됩니다. 이 프로젝트에서는 렌더링에 상호작용할 옵션들을 만드는 데 사용됩니다.

- [STB Image](https://github.com/nothings/stb) 이미지를 로딩하는 header only 라이브러리입니다. 이미지 파일을 로딩하는 데 사용되는 작고 빠른 라이브러리입니다. BMP, JPEG, 그리고 기타 다른 흔한 이미지 포맷들을 로딩할 수 있습니다. 이는 다른 유사한 단일 파일 라이브러리 집합의 일부입니다.

- [Tiny Obj Loader](https://github.com/tinyobjloader/tinyobjloader) .Obj 모델을 로딩하는 header only 라이브러리입니다. 3D 모델을 불러오는 데 사용할 작고 빠른 라이브러리입니다. 

- [Vk Bootstrap](https://github.com/charles-lunarg/vk-bootstrap/blob/master/src/VkBootstrap.cpp) Vulkan을 초기화할 때 필요한 많은 양의 보일러플레이트(boilerplate)를 추상화해줍니다. 대부분의 코드는 한번만 작성되고 이후 수정할 일이 없기 때문에 이 라이브러리를 사용해 대부분의 과정을 생략할 것입니다. 이 라이브러리는 인스턴스 생성, 스왑체인 생성, 그리고 확장 로딩과 같은 작업을 단순하게 만들어줍니다. 이후 Vulkan을 수동적으로 초기화하는 방법을 설명하는 부록에서는 제거될 것입니다.

- [VMA (vulkan memory allocator)](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) Vulkan을 위한 메모리 할당자가 구현되어있는 header only 라이브러리입니다. Vulkan에서는 버퍼, 이미지, 그리고 다른 자원들의 메모리 관리를 사용자가 직접 다뤄야 합니다. 이는 성능과 안정성을 동시에 고려하면서 처리하기 매우 어려울 수 있습니다. Vulkan Memory Allocator는 이를 대신 처리하여 이미지나 다른 자원의 생성을 쉽게 수행할 수 있습니다. 개인 Vulkan 엔진이나 에뮬레이터와 같은 소규모 프로젝트에서 널리 사용됩니다. 언리얼 엔진이나 기타 AAA 엔진과 같은 대규모 프로젝트에서는 각자의 메모리 할당자가 구현되어있습니다.

Next: [Building Project]({{ site.baseurl }}{% link ko/docs/new_chapter_0/building_project.md %})


{% include comments.html term="Introduction Comments" %}
