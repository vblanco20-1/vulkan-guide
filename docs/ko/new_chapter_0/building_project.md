---
layout: default
title: Building Project
parent: "0. Project Setup"
nav_order: 2
auto_comments: Vkguide 2 Beta Comments
---

# 프로젝트 빌드하기
 
<https://github.com/vblanco20-1/vulkan-guide/>에 있는 Git 프로젝트를 가져오는 것으로 시작합니다. all-chapters 브랜치는 전체 프로젝트의 예시 코드를 포함하며, starting-point 브랜치는 초기 빈 프로젝트를 포함합니다. 두 브랜치를 다른 폴더로 가져와 작업하면서 막힐 때 마다 all-chapters 브랜치의 코드를 확인해보는 방법을 권장합니다. 만약 Git을 사용하지 않고 직접 다운로드 받기를 원한다면 아래 링크를 통해 다운로드 할 수 있습니다.

[All Chapters](https://github.com/vblanco20-1/vulkan-guide/archive/all-chapters-2.zip)

[Starting Point](https://github.com/vblanco20-1/vulkan-guide/archive/starting-point-2.zip)

프로젝트를 빌드하기 위해 Visual Studio가 필요합니다. 프로젝트의 코드는 크로스 플랫폼으로 작동하지만 이 가이드에서는 Window 중심으로 설명할 것입니다.

먼저, Vulkan SDK <https://vulkan.lunarg.com/sdk/home>를 설치합니다. 이는 Vulkan 애플리케이션을 만드는 데 필요한 헤더와 라이브러리를 담고 있습니다. 이는 전역 위치에 설치되므로 설치위치를 걱정할 필요는 없습니다.

그 다음, CMake를 설정합니다. 이는 우리가 사용할 빌드 시스템입니다.<https://cmake.org/>.

CMake가 설치되면 CMake-gui를 사용해 프로젝트 루트 경로의 CMakeLists를 엽니다. 그 후, Configure 버튼을 누릅니다.

만약 이 지점에서 Vulkan_INCLUDE_DIR 혹은 Vulkan_LIBRARY를 찾지 못했다는 에러가 발생한다면, Vulkan SDK 설치가 완료되기를 기다리세요. 그리고 CMake-Gui를 재시작 한 후 다시 Configure 버튼을 눌러보세요.

이제 의존성이 설정되었습니다. CMake에서 Configure이 되고 나서 Generate를 눌러 Visual Studio(혹은 다른 IDE)에 맞는 프로젝트 파일을 만들 수 있습니다.

Visual Studio에서 엔진을 시작 프로젝트로 설정한 후, 이제 F5(디버깅 시작)을 눌러 컴파일하고 이를 실행할 수 있습니다.

![vs]({{site.baseurl}}/assets/images/vs_compile.png)


# Third party 라이브러리
third_party 폴더 하위에는 튜토리얼에서 사용할 여러 다양한 라이브러리가 있습니다. 이 라이브러리들은 소스 코드 형태로 폴더에 포함되어 있습니다. 이 중 일부는 공개 저장소의 코드와 다르게 수정되었거나 추가된 부분이 있습니다.

아래는 사용할 라이브러리의 목록입니다.

- vulkan SDK: third_party 폴더에 없는 것이지만 필수적인 것으로, 위에서 설명한대로 설치해야 합니다.
- vkbootstrap: Vulkan 초기화를 단순하게 만들어주는 라이브러리입니다. 우리가 사용할 GPU를 선택하거나 초기화 구조체를 설정하는 것과 같은 작업을 수행해줍니다.
- VMA: Vulkan Memory Allocator입니다. 이는 GPU 구조체를 위한 고성능 메모리 할당기를 구현합니다. 할당자를 우리가 직접 구현하는 것 대신 이를 사용할 것입니다. 이 라이브러리가 우리의 GPU 메모리를 관리해줄 것입니다.
- SDL: 창을 만들고 입력, 오디오, 기타 다양한 것을 수행할 수 있는 고성능 멀티플랫폼 라이브러리입니다. 이 튜토리얼에서는 SDL의 창 생성과 입력 처리 부분을 사용할 것입니다.
- GLM : 수학 라이브러리입니다. 벡터, 행렬 등을 지원합니다.
- {fmt} : 문자열 포맷팅과 콘솔 출력 라이브러리입니다. 성능이 뛰어나며 문자열 출력과 다양한 포맷팅 옵션을 제공하기 때문에 std::cout 대신 사용할 것입니다.
- stb_image: PNG 혹은 jpeg 이미지를 불러오는 데 사용할 라이브러리입니다.
- dear imgui: 디버깅 인터페이스를 위한 매우 유용한 UI 라이브러리입니다. 타이머나 기타 UI 창을 만들 때 사용할 것입니다.
- fastgltf: 고성능 GLTF 로딩 라이브러리입니다. 


Next: [ Code Walkthrough]({{ site.baseurl }}{% link docs/ko/new_chapter_0/code_walkthrough.md %})  
