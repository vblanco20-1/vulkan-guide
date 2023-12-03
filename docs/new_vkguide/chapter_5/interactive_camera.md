---
layout: default
title: Interactive Camera
parent: "New 5. GLTF loading"
nav_order: 2
---

The render architecture is ready to load full scenes, but thats not of much use if we have a fixed camera. Lets setup a interactive flying camera with mouse look so that we can explore the levels we load.

The camera is more of a gameplay layer object. We will add it into the VulkanEngine, but in a real architecture, you probably dont want to be doing input events and game logic within the engine itself, instead you would only store a camera struct that contains the render parameters, and when you update game logic, you refresh those matrices so they can be used with rendering.

The Camera struct is going to look like tihs. we will add it into a new camera.h/camera.cpp file.

```cpp
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

We are not actually going to keep the matrix stored, and instead calculate it when needed. this way we guarantee that the matrices are always in sync with the parameters.

We keep a Velocity vector to keep track of the WASD press state and maybe add acceleration later. Position contains where the camera is in the world.

We wont be using a full rotation matrix, but instead just run  pitch + yaw. This mimics how something like an unreal engine camera works, and makes it much easier to deal with FPS logic.

On the functions, we have a Update() call that will modify position by velocity, and a processSDLEvent that acts as the input logic.

Lets write those.

```cpp
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

When calculating the position update, we are using the rotation matrix, this is because Velocity will be in camera relative space, so that pressing W moves forward. what "forward" means depends on the rotation.

On the input logic, SDL gives us input events, for key up/down, and mouse movement. We will set the velocity to the correct values from key down event, and key up sets it to 0. This is not handling the input fully correctly, as if you press W and S at the same time, and then release one of them, the movement stops, which makes it feel a bit awkward. Improving it is an excersise to the reader.

For mouse motion, we acumulate horizontal and vertical movement of the mouse into the pitch and yaw properties. 

Movement in this code is frame-dependant, as we arent taking the speed of the engine into account. This is done for simplicity in the case, if you want to improve it, you would need to pass deltaTime (time between frames) to the update() function, and multiply the velocity by that. In the tutorial, we are more or less FPS locked to monitor speed due to the options we have used in the swapchain, and we arent rendering enough data to slow down the engine. 

The matrix functions look like this

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

For rotation matrix, we are calculating 2 quaternions. One will be horizontal rotation, and the other vertical rotation. We use pitch and yaw properties for that, and use it to compose the matrices we want for rotation.

On the view matrix, we calculate the final transform matrix of the camera by combining the rotation matrix and a translation matrix. Then we invert the matrix. This is because to have a real camera matrix, what you are doing is not really move the camera, but move the world in the opposite way the camera moves,as the matrices are accumulated in the shaders.

This is really all of it, so lets hook it into VulkanEngine class

First we need to add the camera as a member

```cpp
#include <camera.h>

class VulkanEngine{

     Camera mainCamera;
}
```

Now we hook it into the renderer. Begin by going into the `run()` function, where we will hook the SDL events.

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

Next, in the update_scene() function, we add the camera logic for update and copying the camera matrices into the renderer sceneData struct. 

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


From the end of the init() function, we will set the camera initial variables.

```cpp
mainCamera.velocity = glm::vec3(0.f);
mainCamera.position = glm::vec3( 0,0,-5 );

mainCamera.pitch = 0;
mainCamera.yaw = 0;
```

Thats it, we now have a basic interactive camera. Try to use it and look around, you will be able to see the meshes we had before in the world, using WASD to move the camera, and mouse to look around.




