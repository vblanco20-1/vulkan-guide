---
layout: default
title: Window Resizing
parent: "New 3. Graphics Pipelines"
nav_order: 25
---

## Window Resizing
In vulkan, we have to handle window resizing ourselves. As part of chapter 0 we already had the code for minimzation, but resizing the window is a lot more involved.

When the window resizes, the swapchain becomes invalid, and the vulkan operations with the swapchain like `vkAcquireNextImageKHR` and `vkQueuePresentKHR` can fail with a `VK_ERROR_OUT_OF_DATE_KHR` error. We must handle those correctly, and make sure that we can re-create the swapchain with a new size. 

For efficiency, we will not be reallocating the draw image. Right now we only have one draw image and depth image, but on a more developed engine it could be significantly more, and re-creating all that can be a considerable hassle. Instead, we create the draw and depth image at startup with a preset size, and then draw into a section of it if the window is small, or scale it up if the window is bigger. As we arent reallocating but just rendering into a corner, we can also use this same logic to perform dynamic resolution, which is a useful way of scaling performance, and can be handy for debugging. We are copying the rendering from the draw image into the swapchain with VkCmdBlit, and that one performs scaling so it will work well here. This sort of scaling is not the highest quality, as normally you would want to perform some more complicated logic for the upscaling like applying some sharpening, or doing fake antialiasing as part of that scaling. The Imgui UI will still render into the swapchain image directly, so it will always render at native resolution.

Lets begin by enabling the resizable flag when creating the window. Then we can see what happens if we try to resize.

At the top of `VulkanEngine::init`, change the window_flags so that it has the resizable flag.

```cpp
SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
```
SDL handles the OS part of resizing the window so we can do it now. Run the engine and try to resize the window.

It should give an error and crash on the VK_CHECK macros we have on either `vkAcquireNextImageKHR` or `vkQueuePresentKHR`. The error will be `VK_ERROR_OUT_OF_DATE_KHR`. So to handle the resize, we need to stop the rendering if we see that error, and rebuild the swapchain when that happens.

First, add a `resize_requested` boolean to the VulkanEngine class.

On the draw() function, replace the call to `vkAcquireNextImageKHR` to check the error code.

```cpp
	VkResult e = vkAcquireNextImageKHR(_device, _swapchain, 1000000000, get_current_frame()._swapchainSemaphore, nullptr, &swapchainImageIndex);
	if (e == VK_ERROR_OUT_OF_DATE_KHR) {
        resize_requested = true;       
		return ;
	}
```

Also replace the call to `vkQueuePresentKHR` at the end in the same way. But we dont add the `return` as we are already at the end of the function.
```cpp
VkResult presentResult = vkQueuePresentKHR(_graphicsQueue, &presentInfo);
if (presentResult == VK_ERROR_OUT_OF_DATE_KHR) {
    resize_requested = true;
}
```

Now, as we will stop rendering when it happens, you can resize the image, and it will not crash, but it will also not draw again and just freeze the image.

Lets add a `resize_swapchain()` function to VulkanEngine to re-create the swapchain.

<!-- codegen from tag resize_swap on file E:\ProgrammingProjects\vulkan-guide-2\chapter-3/vk_engine.cpp --> 
```cpp
void VulkanEngine::resize_swapchain()
{
	vkDeviceWaitIdle(_device);

	destroy_swapchain();

	int w, h;
	SDL_GetWindowSize(_window, &w, &h);
	_windowExtent.width = w;
	_windowExtent.height = h;

	create_swapchain(_windowExtent.width, _windowExtent.height);

	resize_requested = false;
}
```

To resize the swapchain, we first begin by waiting until the GPU has finished all rendering commands. We dont want to change the images and views while the gpu is still handling them. Then we destroy the swapchain, and then we query the window size from SDL and create it again. 

Now we need to call this function from the main run() loop when the image resizes.

Right after the SDL event loop and the freeze_rendering check, add this before it also calls any NewFrame imgui functions.
```cpp
if (resize_requested) {
	resize_swapchain();
}
```

Now we have the resizing implemented so try it. You should be able to resize the image down without running into errors. But if you make the window bigger it will fail. We are going past the size of our draw-image and its try to render out of bounds. We can fix that by implementing a _drawExtent variable and making sure that it gets maxed at the size of the draw image.

Add `_drawExtent` to VulkanEngine class and a `renderScale` float that we will use for dynamic resolution.

```cpp
VkExtent2D _drawExtent;
float renderScale = 1.f;
```

Back to the draw() function, we calculate the draw extent at the start of it, instead of using the draw image extent for it.

```cpp
_drawExtent.height = std::min(_swapchainExtent.height, _drawImage.imageExtent.height) * renderScale;
_drawExtent.width= std::min(_swapchainExtent.width, _drawImage.imageExtent.width) * renderScale;
```

Now we are going to add a slider to imgui to control this draw scale parameter.

In the `run()` function, inside the imgui window that calculates background parameters, add this to the top

```cpp
if (ImGui::Begin("background")) {
ImGui::SliderFloat("Render Scale",&renderScale, 0.3f, 1.f);
//other code
}
```

This will give us a render scale editable slider, that will go from 0.3 to 1.f. We dont want to go past 1 because it will break the resolution. 

Run it, and try to resize the window and play with the render scale. You will see that now you can maximize or move around the window and change its resolution dynamically.

We are setting up the draw image a bit small, but if you want, try to increase the size of the draw image from the place its created in init_swapchain(). Set the `drawImageExtent` to your monitor resolution instead of the _windowExtent, which is hardcoded to a small size.

With this we have chapter 3 done, and can move forward to the next chapter.

Next: [Chapter 4: New Descriptor Abstractions]({{ site.baseurl }}{% link docs/new_chapter_4/descriptor_abstractions.md %})

{% include comments.html term="Vkguide 2 Beta Comments" %}
