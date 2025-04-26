---
layout: default
title: "Code Walkthrough - KO"
parent: "0. Project Setup - KO"
grand_parent: "Korean VKGuide"
nav_order: 3
auto_comments: Vkguide 2 Beta Comments
---

# 둘러보기
이번 챕터에서는 이미 작성된 스켈레톤 코드를 살펴보며 해당 코드가 실제로 어떤 역할을 하는 지 확인해보겠습니다.

파일은 project/src/ 폴더에 있습니다.

- vk_engine.h/cpp : 이는 엔진의 핵심 클래스입니다. 튜토리얼에서 다룰 대부분의 코드가 있는 곳입니다.
- main.cpp : 코드의 진입점입니다. vk_engine 호출 외에는 아무 코드도 없습니다. 
- vk_initializers.h/cpp : Vulkan 구조체를 생성하는 것을 돕는 함수가 포함되어 있습니다.
- vk_images.h/cpp : 이미지와 관련된 함수들을 포함되어 있습니다.
- vk_pipelines.h/cpp : 파이프라인을 위한 추상화가 포함되어 있습니다.
- vk_descriptors.h/cpp : 디스크립터 셋(descriptor set) 추상화가 포함되어 있습니다.
- vk_loader.h/cpp : GLTF 파일을 불러오기 위한 함수가 포함되어 있습니다.
- vk_types.h : 전체 코드 베이스는 이 헤더를 포함합니다. 이는 널리 사용되는 기본 구조체와 포함 파일을 제공합니다.

`vk_engine`은 프로젝트의 중심이자 주요 엔진 클래스가 될 것입니다. `vk_loader`는 GLTF 파일을 불러올 때 `vk_engine`과 상호작용해야 하므로, 이에 통합될 예정입니다. 그 외의 다른 파일들은 튜토리얼 진행에 따라 만들어지는 일반적인 Vulkan 추상화 계층으로, Vulkan 외에는 별다른 의존성이 없습니다. 따라서 이러한 추상화 파일들은 여러분의 다른 프로젝트에서도 자유롭게 활용할 수 있습니다.

# 코드

^code main chapter-0/main.cpp


단순한 `main.cpp`로 시작합니다. `VulkanEngine` 함수를 호출하는 것외에는 아무것도 하지 않습니다.

향후에는 `argc/argv`에서 전달된 명령줄 인자나 설정파일을 이용해 파라미터를 구성하는 용도로 사용할 수도 있습니다.

`vk_types.h`은 다음을 포함합니다.

^code intro shared/vk_types.h

`#pragrma once`는 컴파일러에게 같은 파일을 두 번 포함하지 말라고 알려주는 전처리기 지시문입니다. 이는 헤더가드와 동일한 역할을 하지만 더 간결합니다.

`<vulkan/vulkan.h>`라는 Vulkan 핵심 헤더를 포함합니다. 이는 우리가 필요로 하는 모든 구조체와 Vulkan 함수 정의를 담고 있습니다. 또한 코드 전반에 사용할 fmt 라이브러리의 핵심 헤더를 포함하고 `VK_CHECK` 매크로를 만들어 Vulkan 호출 시 에러를 관리할 것입니다. 튜토리얼에서는 `vk_enum_string_helper.h`를 사용할 것입니다. 이는 Vulkan SDK가 제공하는 헤더로, 주어진 Vulkan 열거형에 대한 문자열을 가져올 수 있게 해줍니다. 이러한 방식으로 로그를 작성할 때 매우 유용합니다.  

이 튜토리얼은 출력을 위해 표준 `std::cout`을 사용하지 않을 것입니다. 대신 `{fmt}` 라이브러리를 대신 사용할 것입니다. 이는 문자열 포맷팅과 출력을 위한 고품질 라이브러리입니다. C++20의 `std::format`은 이 라이브러리를 기반으로 작성되었지만, 더 많은 기능과 지원을 위해 해당 라이브러리를 사용하겠습니다. 여기서는 Vulkan 에러가 발생한 경우 `fmt::println`을 사용해 콘솔창에 에러를 출력하겠습니다.

`vk_initializers.h`는 사전 작성된 파일입니다. 이는 대부분의 Vulkan info 구조체와 다른 유사한 것들의 이니셜라이저를 담습니다. 이는 이러한 구조체들을 약간 추상화하여 이를 사용할 때 마다 코드와 추상화에 대해 설명할 것입니다.

Vulkan 그 자체를 담는 `vk_types` 헤더를 포함하고, 이후 이곳에 추가할 함수를 담을 네임스페이스를 선언합니다.

마지막으로 핵심 클래스인 `vk_engine.h`를 살펴봅시다.

^code intro chapter-0/vk_engine.h 

`vk_initializers`과 마찬가지로 `vk_types`를 포함합니다. Vulkan 타입인 `VkExtent2D`가 필요하기 때문입니다. VulkanEngine은 우리가 작업할 핵심입니다. 엔진이 수행하는 대부분의 작업을 이 클래스를 중심으로 작업할 것입니다. 이렇게 하면 프로젝트의 아키텍처를 단순하게 만들 수 있습니다.

엔진이 초기화 되었는지를 확인하는 플래그, 프레임을 나타내는 정수, 그리고 창의 크기를 픽셀 단위로 저장하는 변수를 갖습니다.

선언 `struct SDL_Window* _window;`는 특히 중요합니다. `struct`가 맨 앞에 온 점을 유의합시다. 이는 전방 선언이라 불리는 것으로, VulkanEngine 헤더에 SDL을 포함시키지 않고도 클래스 내부에서 `SDL_Window` 포인터를 사용 가능하게 해줍니다. 이 변수는 애플리케이션에서 만들 창을 담고 있습니다.

또한 `Get()` 함수를 전역 싱클톤 패턴으로 추가했습니다.

헤더에 대해 다뤄보았으므로, cpp 파일을 살펴 봅시다.

vk_engine.cpp line 1

^code includes chapter-0/vk_engine.cpp

다른 파일들과는 달리, 여기서는 더 많은 것들을 포함하고 있습니다. `<SDL.h>`와 `<SDL_vulkan.h>` 모두를 포함하고 있습니다. `SDL.h`는 SDL 라이브러리의 창을 열고 입력을 받는 핵심 데이터를 담고 있습니다. 반면 `SDL_vulkan.h`는 Vulkan 특수 플래그와 Vulkan과 호환되는 창을 열기 위한 기능, 그 외에도 다양한 Vulkan 특수 기능을 담고 있습니다. 또한 사용할 몇몇 STL 컨테이너도 추가합니다.

vk_engine.cpp, line 10

^code init chapter-0/vk_engine.cpp

SDL 창을 생성하는 첫 번재 코드를 살펴봅시다. 
첫 번째로 하는 것은 SDL 라이브러리 초기화입니다. SDL 라이브러리는 다양한 기능을 포함하고 있기 때문에 우리가 어떤 기능을 사용할 것인지에 대한 플래그를 전달해야 합니다. `SDL_INIT_VIDEO`는 SDL에게 기본 창 기능을 사용하고 싶다는 것을 알려줍니다. 이는 또한 키보드와 마우스 입력을 받는 기본 입력 이벤트도 포함합니다.

또한 Vulkan 엔진을 싱글톤으로 참조하기 위한 전역 포인터를 설정합니다. 전형적인 싱글톤 대신 이처럼 작업하는 이유는 클래스가 초기화되고 파괴되는 시점을 명시적으로 제어하기 위해서입니다. 일반적인 C++ 싱글톤 패턴은 이러한 제어를 할 수 없습니다.

SDL이 초기화되면, 이를 사용해 창을 생성할 것입니다. 창은 이후에 사용할 수 있도록 `_window` 멤버에 담깁니다.

SDL이 C 라이브러리이기 때문에, 생성자와 파괴자를 지원하지 않습니다. 따라서 수동적으로 파괴해주어야 합니다.

창이 만들어지면 파괴도 수행해야 합니다.
^code extras chapter-0/vk_engine.cpp


`SDL_CreateWindow`에서 했던 것과 유사하게, `SDL_DestroyWindow`도 수행할 필요가 있습니다. 이는 프로그램의 창을 제거합니다. 이제 엔진이 완전히 정리되었으므로, 이 지점에서 엔진을 가리키는 싱글톤 포인터도 초기화합니다. 

이후에 더 많은 함수를 `cleanup` 함수에 추가할 것입니다.

`draw` 함수가 지금은 비어있지만, 이후에 렌더링 코드를 추가할 것입니다.

^code drawloop chapter-0/vk_engine.cpp


애플리케이션의 메인 루프입니다. `while()`에 무한 반복문이 있습니다. 이는 SDL이 `SDL_QUIT`를 받았을 때에만 멈춥니다.

매 반복마다 `SDL_PollEvent`를 수행합니다. 이는 SDL에게 지난 프레임동안 OS가 애플리케이션으로 보낸 모든 이벤트를 요청합니다. 여기서 키보드 입력, 마우스 이동, 창 움직이기, 최소화 등을 확인할 수 있습니다. 지금은 `SDL_QUIT`과 창 최소화/복구에만 신경쓰면 됩니다. 창을 최소화하는 이벤트를 받으면, `stop_rendering` 불 값을 `true`로 설정해 최소화 되었을 때 그리는 것을 방지합니다. 창을 복구하면 이를 다시 `false`로 설정해 그리기를 계속하도록 합니다.

마지막으로, 메인 루프의 각 반복마다 `draw()`를 호출하거나 그리기가 비활성화된 경우에는 `std::this_thread::sleep_for`를 호출합니다. 이렇게 하면 창이 최소화 되었을 때 실행되는 것을 방지함으로서 성능을 절약할 수 있습니다.

이제 SDL이 어떻게 창을 만드는지를 살펴봤습니다. 사실 그 외에는 큰 변화가 없습니다.

이제 SDL 이벤트를 실험해보는 것만이 남았습니다.

연습삼아 SDL2의 문서를 읽어 `fmt::print`를 사용해 키보드 이벤트를 로그로 출력해보는 것도 좋습니다.

이제 첫 챕터를 진행할 수 있습니다. 렌더링 루프를 구현해보겠습니다.

Next: [Initializing Vulkan]({{ site.baseurl }}{% link docs/ko/new_chapter_1/vulkan_init_flow.md %})

{% include comments.html term="Vkguide 2 Korean Comments" %}