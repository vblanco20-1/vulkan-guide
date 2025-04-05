---
layout: default
lang: ko
title: ko Code Walkthrough
parent: "0. 프로젝트 설정"
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

vk_engine will be our main engine class, and the core of the project. vk_loader will be tied into it as it will need to interface it while loading GLTF files.
The other files are for generic vulkan abstraction layers that will get built as the tutorial needs. Those abstraction files have no dependencies other than vulkan, so you can keep them for your own projects.
vk_engine은 주요 엔진 클래스이자 프로젝트의 핵심이 될 것입니다. vk_loader는 GLTF 파일을 로딩하는 동안 상호작용 해야 하므로 함께 묶일 것입니다. 다른 파일들은 튜토리얼의 필요에 따라 작성된 일반적인 vulkan 추상화 계층입니다. 이러한 추상화 파일들은 vulkan에만 의존하므로, 이를 독자의 프로젝트에도 자유롭게 사용할 수 있습니다.

# 코드

<!-- codegen from tag main on file E:\ProgrammingProjects\vulkan-guide-2\chapter-0/main.cpp --> 
```cpp
#include <vk_engine.h>

int main(int argc, char* argv[])
{
	VulkanEngine engine;

	engine.init();	
	
	engine.run();	

	engine.cleanup();	

	return 0;
}
```

단순한 main.cpp로 시작합니다. Vulkan engine 함수를 호출하는 것외에는 아무것도 하지 않습니다.

향후에는 argc/argv에서 전달된 명령줄 인자나 설정파일을 이용해 파라미터를 구성하는 용도로 사용할 수도 있습니다.

vk_types.h은 다음을 포함합니다.

<!-- codegen from tag intro on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_types.h --> 
```cpp
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <span>
#include <array>
#include <functional>
#include <deque>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vk_mem_alloc.h>

#include <fmt/core.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
```
<!-- codegen from tag intro on file E:\ProgrammingProjects\vulkan-guide-2\shared/vk_types.h --> 
```cpp
#define VK_CHECK(x)                                                     \
    do {                                                                \
        VkResult err = x;                                               \
        if (err) {                                                      \
             fmt::print("Detected Vulkan error: {}", string_VkResult(err)); \
            abort();                                                    \
        }                                                               \
    } while (0)
```

`#pragrma once`는 컴파일러에게 같은 파일을 두 번 포함하지 말라고 알려주는 전처리기 지시문입니다. 이는 헤더가드와 동일한 역할을 하지만 더 간결합니다.

`<vulkan/vulkan.h>`라는 Vulkan 핵심 헤더를 포함합니다. 이는 우리가 필요로 하는 모든 구조체와 Vulkan 함수 정의를 담고 있습니다. 또한 코드 전반에 사용할 fmt 라이브러리의 핵심 헤더를 포함하고 VK_CHECK 매크로를 만들어 vulkan 호출 시 에러를 관리할 것입니다. 튜토리얼에서는 `vk_enum_string_helper.h`를 사용할 것입니다. 이는 Vulkan SDK가 제공하는 헤더로, 주어진 Vulkan 열거형에 대한 문자열을 가져올 수 있게 해줍니다. 이러한 방식으로 로그를 작성할 때 매우 유용합니다.  

이 튜토리얼은 출력을 위해 표준 std::cout을 사용하지 않을 것입니다. 대신 {fmt} 라이브러리를 대신 사용할 것입니다. 이는 문자열 포맷팅과 출력을 위한 고품질 라이브러리입니다. Cpp 20의 std::format은 이 라이브러리를 기반으로 작성되었지만, 더 많은 기능과 지원을 위해 해당 라이브러리를 사용하겠습니다. 여기서는 Vulkan 에러가 발생한 경우 `fmt::println`을 사용해 콘솔창에 에러를 출력하겠습니다.

vk_initializers.h는 사전 작성된 파일입니다. 이는 대부분의 Vulkan info 구조체와 다른 유사한 것들의 이니셜라이저를 담습니다. 이는 이러한 구조체들을 약간 추상화하여 이를 사용할 때 마다 코드와 추상화에 대해 설명할 것입니다.

Vulkan 그 자체를 담는 vk_types 헤더를 포함하고, 이후 이곳에 추가할 함수를 담을 네임스페이스를 선언합니다.

마지막으로 핵심 클래스인 vk_engine.h를 살펴봅시다.

<!-- codegen from tag intro on file E:\ProgrammingProjects\vulkan-guide-2\chapter-0/vk_engine.h --> 
```cpp
#pragma once

#include <vk_types.h>

class VulkanEngine {
public:

	bool _isInitialized{ false };
	int _frameNumber {0};
	bool stop_rendering{ false };
	VkExtent2D _windowExtent{ 1700 , 900 };

	struct SDL_Window* _window{ nullptr };

	static VulkanEngine& Get();

	//엔진의 모든 요소를 초기화합니다.
	void init();

	//엔진을 작동 중지합니다.
	void cleanup();

	//그리기를 수행합니다.
	void draw();

	//주요 반복문을 수행합니다.
	void run();
};
```

vk_init과 마찬가지로 vk_types를 포함합니다. Vulkan 타입인 VkExtent2D가 필요하기 때문입니다. VulkanEngine은 우리가 작업할 핵심입니다. 엔진이 수행하는 대부분의 작업을 이 클래스를 중심으로 작업할 것입니다. 이렇게 하면 프로젝트의 아키텍처를 단순하게 만들 수 있습니다.

엔진이 초기화 되었는지를 확인하는 플래그, 프레임을 나타내는 정수, 그리고 창의 크기를 픽셀 단위로 저장하는 변수를 갖습니다.

선언 `struct SDL_Window* _window;`는 특히 중요합니다. `struct`가 맨 앞에 온 점을 유의합시다. 이는 전방 선언이라 불리는 것으로, VulkanEngine 헤더에 SDL을 포함시키지 않고도 클래스 내부에서 `SDL_Window` 포인터를 사용 가능하게 해줍니다. 이 변수는 애플리케이션에서 만들 창을 담고 있습니다.

또한 Get() 함수를 전역 싱클톤 패턴으로 추가했습니다.

헤더에 대해 다뤄보았으므로, cpp 파일을 살펴 봅시다.

vk_engine.cpp line 1

<!-- codegen from tag includes on file E:\ProgrammingProjects\vulkan-guide-2\chapter-0/vk_engine.cpp --> 
```cpp
#include "vk_engine.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vk_initializers.h>
#include <vk_types.h>

#include <chrono>
#include <thread>
```

다른 파일들과는 달리, 여기서는 더 많은 것들을 포함하고 있습니다. `<SDL.h>`와 `<SDL_vulkan.h>` 모두를 포함하고 있습니다. SDL.h는 SDL 라이브러리의 창을 열고 입력을 받는 핵심 데이터를 담고 있습니다. 반면 SDL_vulkan.h는 Vulkan 특수 플래그와 Vulkan과 호환되는 창을 열기 위한 기능, 그 외에도 다양한 Vulkan 특수 기능을 담고 있습니다. 또한 사용할 몇몇 STL 컨테이너도 추가합니다.

vk_engine.cpp, line 10

<!-- codegen from tag init on file E:\ProgrammingProjects\vulkan-guide-2\chapter-0/vk_engine.cpp --> 
```cpp
constexpr bool bUseValidationLayers = false;

VulkanEngine* loadedEngine = nullptr;

VulkanEngine& VulkanEngine::Get() { return *loadedEngine; }
void VulkanEngine::init()
{
    // 애플리케이션 당 하나의 엔진 초기화만을 허용합니다.
    assert(loadedEngine == nullptr);
    loadedEngine = this;

    // SDL을 초기화하고 창을 생성합니다.
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

    _window = SDL_CreateWindow(
        "Vulkan Engine",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        _windowExtent.width,
        _windowExtent.height,
        window_flags);

    // 올바르게 이루어졌다면 초기화 플래그 변수를 설정합니다.
    _isInitialized = true;
}
```

SDL 창을 생성하는 첫 번재 코드를 살펴봅시다. 
첫 번째로 하는 것은 SDL 라이브러리 초기화입니다. SDL 라이브러리는 다양한 기능을 포함하고 있기 때문에 우리가 어떤 기능을 사용할 것인지에 대한 플래그를 전달해야 합니다. SDL_INIT_VIDEO는 SDL에게 기본 창 기능을 사용하고 싶다는 것을 알려줍니다. 이는 또한 키보드와 마우스 입력을 받는 기본 입력 이벤트도 포함합니다.

또한 vulkan 엔진을 싱글톤으로 참조하기 위한 전역 포인터를 설정합니다. 전형적인 싱글톤 대신 이처럼 작업하는 이유는 클래스가 초기화되고 파괴되는 시점을 명시적으로 제어하기 위해서입니다. 일반적인 Cpp 싱글톤 패턴은 이러한 제어를 할 수 없습니다.

SDL이 초기화되면, 이를 사용해 창을 생성할 것입니다. 창은 이후에 사용할 수 있도록 `_window` 멤버에 담깁니다.

SDL이 C 라이브러리이기 때문에, 생성자와 파괴자를 지원하지 않습니다. 따라서 수동적으로 파괴해주어야 합니다.

창이 만들어지면 파괴도 수행해야 합니다.
<!-- codegen from tag extras on file E:\ProgrammingProjects\vulkan-guide-2\chapter-0/vk_engine.cpp --> 
```cpp
void VulkanEngine::cleanup()
{
    if (_isInitialized) {

        SDL_DestroyWindow(_window);
    }

    // 엔진 포인터를 초기화합니다.
    loadedEngine = nullptr;
}

void VulkanEngine::draw()
{
    // 이후에 작성합니다.
}
```

`SDL_CreateWindow`에서 했던 것과 유사하게, `SDL_DestroyWindow`도 수행할 필요가 있습니다. This will destroy the window for the program. We also clear the singleton pointer for the engine from here, now that the engine is fully cleared. 이는 프로그램의 창을 파괴합니다. 이제 엔진이 완전히 해제되었으므로 엔진을 가리키는 싱글톤 포인터도 초기화합니다. 

이후에 더 많은 함수를 cleanup 함수에 추가할 것입니다.

draw 함수가 지금은 비어있지만, 이후에 렌더링 코드를 추가할 것입니다.

<!-- codegen from tag drawloop on file E:\ProgrammingProjects\vulkan-guide-2\chapter-0/vk_engine.cpp --> 
```cpp
void VulkanEngine::run()
{
    SDL_Event e;
    bool bQuit = false;

    // 메인 루프
    while (!bQuit) {
        // 큐의 이벤트를 관리합니다.
        while (SDL_PollEvent(&e) != 0) {
            // 사용자가 alt-f4 혹은 X버튼을 눌렀을 때 창을 닫습니다.
            if (e.type == SDL_QUIT)
                bQuit = true;

            if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                    stop_rendering = true;
                }
                if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
                    stop_rendering = false;
                }
            }
        }

        // 최소화되었을 때 아무것도 그리지 않습니다.
        if (stop_rendering) {
            // 무한 반복하지 않도록 방지합니다.
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        draw();
    }
}
```

애플리케이션의 메인 루프입니다. `while()`에 무한 반복문이 있습니다. 이는 SDL이 SDL_QUIT를 받았을 때에만 멈춥니다.

매 반복마다 SDL_PollEvent를 수행합니다. 이는 SDL에게 지난 프레임동안 OS가 애플리케이션으로 보낸 모든 이벤트를 요청합니다. 여기서 키보드 입력, 마우스 이동, 창 움직이기, 최소화 등을 확인할 수 있습니다. 지금은 SDL_QUIT과 창 최소화/복구에만 신경쓰면 됩니다. 창을 최소화하는 이벤트를 받으면, stop_rendering 불 값을 true로 설정해 최소화 되었을 때 그리는 것을 방지합니다. 창을 복구하면 이를 다시 false로 설정해 그리기를 계속하도록 합니다.

And finally, every iteration of the main loop we call either `draw();`, or `std::this_thread::sleep_for` if drawing is disabled. This way we save performance as we dont want the application spinning at full speed if the user has it minimized.
마지막으로, 메인 루프의 반복마다 `draw()` 혹은 그리기가 비활성화되있다면 `std::this_thread::sleep_for`를 호출합니다. 이렇게 하면 창이 최소화 되었을 때 계속 실행되는 것을 방지함으로서 성능을 절약할 수 있습니다.

이제 SDL이 어떻게 창을 만드는지를 살펴봤습니다. 사실 그 외에는 큰 변화가 없습니다.

이제 SDL 이벤트를 실험해보는 것만이 남았습니다.

연습삼아 SDL2의 문서를 읽어 `fmt::print`를 사용해 키보드 이벤트를 로그로 출력해보는 것도 좋습니다.

이제 첫 챕터를 진행할 수 있습니다. 렌더링 루프를 구현해보겠습니다.

다음 글: [Vulkan 초기화]({{ site.baseurl }}{% link docs/new_chapter_1/vulkan_init_flow.md %})

{% include comments.html term="Vkguide 2 Beta Comments" %}
