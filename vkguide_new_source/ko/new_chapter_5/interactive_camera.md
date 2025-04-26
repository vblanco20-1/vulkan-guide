---
layout: default
title: Interactive Camera
parent: "5. GLTF loading - KO"
grand_parent: "Korean VKGuide"
nav_order: 2
---

렌더링 아키텍처가 전체 씬을 불러올 준비가 되었지만, 카메라가 고정되어 있다면 큰 의미가 없습니다. 마우스를 사용해 주위를 둘러볼 수 있는 인터랙티브 카메라를 구성해 불러온 레벨을 자유롭게 탐색할 수 있도록 해봅시다.

카메라는 본질적으로 게임플레이 계층의 객체입니다. 여기서는 VulkanEngine에 카메라를 추가하겠지만, 실제 아키텍처에서는 입력 처리나 게임 로직을 엔진 내부에서 처리하지 않는 것이 일반적입니다. 대신, 렌더링에 필요한 파라미터만 담은 카메라 구조체를 저장하고, 게임 로직이 업데이트 될 때 해당 행렬들을 갱신해 렌더링에 사용할 수 있도록 하는 방식이 바람직합니다.

카메라 구조체는 다음과 같습니다. 이를 camera.h에 추가해봅시다.

```cpp
#include <vk_types.h>
#include <SDL_events.h>

class Camera {
public:
    glm::vec3 velocity;
    glm::vec3 position;
    // vertical rotation
    float pitch { 0.f };
    // horizontal rotation
    float yaw { 0.f };

    glm::mat4 getViewMatrix();
    glm::mat4 getRotationMatrix();

    void processSDLEvent(SDL_Event& e);

    void update();
};
```

행렬을 저장해두지 않고, 필요할 때마다 계산하는 방식으로 구현할 예정입니다. 이렇게 하면 항상 행렬이 파라미터와 일치하도록 보장할 수 있습니다.

WASD 입력 상태를 추적하기 위해 Velocity 벡터를 유지하며, 이후에는 가속 개념을 추가할 수도 있습니다. Position은 카메라의 월드 위치를 나타냅니다.

전체 회전 행렬을 사용하지 않고 pitch와 raw만으로 회전을 처리할 것입니다. 이는 언리얼 엔진의 카메라 방식과 유사하며, FPS 로직을 보다 간단하게 구현할 수 있게 해줍니다.

함수에서는 Update()를 호출해 velocity에 따라 position을 갱신하고, processSDLEvent는 입력 처리를 담당하는 로직으로 사용할 것입니다.

이제 camera.cpp에 해당 기능을 작성해봅시다.

```cpp
#include <camera.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

void Camera::update()
{
    glm::mat4 cameraRotation = getRotationMatrix();
    position += glm::vec3(cameraRotation * glm::vec4(velocity * 0.5f, 0.f));
}

void Camera::processSDLEvent(SDL_Event& e)
{
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_w) { velocity.z = -1; }
        if (e.key.keysym.sym == SDLK_s) { velocity.z = 1; }
        if (e.key.keysym.sym == SDLK_a) { velocity.x = -1; }
        if (e.key.keysym.sym == SDLK_d) { velocity.x = 1; }
    }

    if (e.type == SDL_KEYUP) {
        if (e.key.keysym.sym == SDLK_w) { velocity.z = 0; }
        if (e.key.keysym.sym == SDLK_s) { velocity.z = 0; }
        if (e.key.keysym.sym == SDLK_a) { velocity.x = 0; }
        if (e.key.keysym.sym == SDLK_d) { velocity.x = 0; }
    }

    if (e.type == SDL_MOUSEMOTION) {
        yaw += (float)e.motion.xrel / 200.f;
        pitch -= (float)e.motion.yrel / 200.f;
    }
}
```

위치를 갱신할 때 회전 행렬을 사용하는 이유는, Velocity가 카메라 기준 좌표계에 있기 때문입니다. 따라서 W키를 누르면 '전방'으로 이동하게 되며, 여기서 '전방'이 의미하는 방향은 카메라의 회전에 따라 달라집니다.

입력 로직에서는 SDL이 키보드 입력(누름/뗌)과 마우스 움직임에 대한 이벤트를 전달해줍니다. 키를 누르면 해당 방향에 맞는 velocity를 설정하고, 키를 떼면 velocity를 0으로 초기화합니다. 다만 이 방식은 입력을 완전히 올바르게 처리하지는 않습니다. 예를 들어 W와 S를 동시에 누른 뒤 하나를 떼면, 움직임이 멈추는 문제가 있어 약간 어색하게 느껴질 수 있습니다. 이 문제를 개선하는 것은 독자에게 맡기겠습니다.

마우스가 움직이면 수평 및 수직 이동량을 pitch와 yaw 값에 누적시켜 회전을 처리합니다.

이 코드에서의 움직임은 프레임에 영향을 받습니다. 엔진의 실행 속도를 고려하지 않기 때문입니다. 이는 구현을 단순화하기 위한 선택이며, 이를 개선하고 싶다면 프레임 간 시간(deltaTime)을 update()에 전달하고, velocity에 해당 값을 곱해줘야 합니다. 이 튜토리얼에서는 스왑체인 설정으로 인해 모니터 주사율에 맞춰 FPS가 고정되어 있으며, 엔진을 느리게 할 정도로 많은 데이터를 렌더링하지 않기 때문에 문제가 되지 않습니다.

행렬 함수는 다음과 같습니다.

```cpp
glm::mat4 Camera::getViewMatrix()
{
    // to create a correct model view, we need to move the world in opposite
    // direction to the camera
    //  so we will create the camera model matrix and invert
    glm::mat4 cameraTranslation = glm::translate(glm::mat4(1.f), position);
    glm::mat4 cameraRotation = getRotationMatrix();
    return glm::inverse(cameraTranslation * cameraRotation);
}

glm::mat4 Camera::getRotationMatrix()
{
    // fairly typical FPS style camera. we join the pitch and yaw rotations into
    // the final rotation matrix

    glm::quat pitchRotation = glm::angleAxis(pitch, glm::vec3 { 1.f, 0.f, 0.f });
    glm::quat yawRotation = glm::angleAxis(yaw, glm::vec3 { 0.f, -1.f, 0.f });

    return glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
}
```

회전 행렬을 계산하기 위해 두 개의 사원수를 생성합니다. 하나는 수평 회전, 다른 하나는 수직 회전용입니다. 이를 위해 pitch와 yaw 속성을 사용하며, 이를 조합해 원하는 회전 행렬을 계산합니다.

뷰 행렬은 회전 행렬과 위치를 나타내는 변환 행렬을 결합하여 카메라의 최종 변환 행렬을 계산합니다. 이후, 이 행렬을 역행렬로 변환합니다. 이는 실제로 카메라가 움직이는 것이 아니라 셰이더에서는 카메라의 움직임과 반대 방향으로 월드를 이동시키는 방식으로 동작하기 때문입니다. 즉, 실제 카메라의 이동은 월드 좌표계를 반대로 이동시키는 것과 같습니다.

이제 모든 구현이 끝났으니, VulkanEngine 클래스에 연결해보겠습니다.

먼저 카메라를 클래스 멤버로 추가해봅시다.

```cpp
#include <camera.h>

class VulkanEngine{

     Camera mainCamera;
}
```

이제 이를 렌더러에 연결해봅시다. 먼저 `run()`함수로 이동하여 SDL 이벤트와 연결해주겠습니다.

```cpp
 // Handle events on queue
 while (SDL_PollEvent(&e) != 0) {
     // close the window when user alt-f4s or clicks the X button
     if (e.type == SDL_QUIT)
         bQuit = true;

     mainCamera.processSDLEvent(e);
     ImGui_ImplSDL2_ProcessEvent(&e);
 }
```

다음은 update_scene() 함수에서 카메라를 업데이트하는 로직을 추가하고, 카메라의 행렬을 렌더러에 sceneData 구조체에 복사합니다.

```cpp
void VulkanEngine::update_scene()
{
    mainCamera.update();

    glm::mat4 view = mainCamera.getViewMatrix();

    // camera projection
    glm::mat4 projection = glm::perspective(glm::radians(70.f), (float)_windowExtent.width / (float)_windowExtent.height, 10000.f, 0.1f);

    // invert the Y direction on projection matrix so that we are more similar
    // to opengl and gltf axis
    projection[1][1] *= -1;

    sceneData.view = view;
    sceneData.proj = projection;
    sceneData.viewproj = projection * view;
}
```


init()함수의 마지막에는 카메라 초기화 변수를 설정합니다. 카메라가 원점을 바라보도록 설정되므로, 좌표 (0,0,0)에 있는 메시가 화면에 보일 것입니다.

```cpp
mainCamera.velocity = glm::vec3(0.f);
	mainCamera.position = glm::vec3(0, 0, 5);

mainCamera.pitch = 0;
mainCamera.yaw = 0;
```

이것으로 끝났습니다. 기본적인 인터랙티브 카메라가 준비되었습니다. 직접 사용해보며 주위를 둘러보세요. WASD키로 카메라를 이동하고 마우스로 시야를 움직이면, 이전에 배치해 두었던 월드 내 메시들을 확인할 수 있을 것입니다.

^nextlink

{% include comments.html term="Vkguide 2 Beta Comments" %}