---
layout: default
title: Implementing DearImgui to a Vulkan engine
parent: Extra Chapter
nav_order: 30
---

Dear Imgui is one of the best debug user interface libraries around. Using it makes it very easy to create debug windows with widgets of various kinds.
This guide will use a few things from chapter 5 code, but can be done standalone just fine.

Imgui itself is a portable library, but it doesnt do any user interaction or rendering by itself, you need to hook it into your renderer or input system. The library comes with a set of example implementations that hook those into imgui.
We are using Vulkan for rendering, and SDL for user input events. Those 2 are covered by the example implementations, so we are going to use those.

## Compiling
If you are using the tutorial code, there is already an imgui Cmake target that will compile imgui. If you arent, you need to add it to your build system.
The files that we are going to need to compile are:
```cmake
target_sources(imgui PRIVATE 
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_vulkan.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_impl_sdl.cpp"
)
```

And also adding the imgui directory to the include path. In here, we are adding the core imgui library code (imgui.cpp, demo.cpp, draw.cpp, widgets.cpp), and the implementations for both Vulkan and SDL.

Make sure that you link to that imgui target from your cmake or build system. You can also just add those files to your build for the main executable.

## Initializing Imgui
There are a few things that we need to initialize before being able to use it.
For that, create a `init_imgui()` function, and make sure to call it as part of your initialization. Has to be after vulkan is fully initialized.

```cpp

void VulkanEngine::init_imgui()
{
	//1: create descriptor pool for IMGUI
	// the size of the pool is very oversize, but its copied from imgui demo itself.
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	VkDescriptorPool imguiPool;
	VK_CHECK(vkCreateDescriptorPool(_device, &pool_info, nullptr, &imguiPool));


	// 2: initialize imgui library

	//this initializes the core structures of imgui
	ImGui::CreateContext();

	//this initializes imgui for SDL
	ImGui_ImplSDL2_InitForVulkan(_window);

	//this initializes imgui for Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = _instance;
	init_info.PhysicalDevice = _chosenGPU;
	init_info.Device = _device;
	init_info.Queue = _graphicsQueue;
	init_info.DescriptorPool = imguiPool;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;

	ImGui_ImplVulkan_Init(&init_info, _renderPass);

	//execute a gpu command to upload imgui font textures
	immediate_submit([&](VkCommandBuffer cmd) {
		ImGui_ImplVulkan_CreateFontsTexture(cmd);
		});

	//clear font textures from cpu data
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	//add the destroy the imgui created structures
	_mainDeletionQueue.push_function([=]() {

		vkDestroyDescriptorPool(_device, imguiPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
		});
}

```

We begin by creating a descriptor pool that imgui needs. Having a descriptor pool just for imgui is the easiest. While this descriptor pool with 1000 of everything is likely going to be oversized, it wont really matter. 

Then, we need to call the functions that initialize imgui itself, and the implementations for vulkan and for SDL
On the Vulkan implementation, there are a few things that we have to hook. `VkInstance`, `VkPhysicalDevice`, `VkDevice`,the  `VkQueue` for graphics, and the descriptor pool we just created. Image count is for the overlapping of the commands. Use the same variables that you use when creating your swapchain.

Once imgui is initialized, the last thing is to execute a command to upload the fonts. In here we are using the immediate submit lambda that we did on chapter 5, but you can replace that with whatever way you have in your engine to execute a command in a blocking way. Once the command has fully finished execution (immediate submit blocks on fence), you can destroy the font upload objects to clear the staging buffers. Then you add the deletion of the descriptor pool and imgui vulkan itself.

## Hooking imgui
With imgui initialized, we now hook it into the main loop of the engine. 

```cpp
    //main loop
	while (!bQuit)
	{
        //imgui new frame 
        ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(_window);

		ImGui::NewFrame();        

		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
            ImGui_ImplSDL2_ProcessEvent(&e);

			//other event handling
		}


        //imgui commands
        ImGui::ShowDemoWindow();

        //your draw function
		draw();
	}
```

We begin by calling the NewFrame functions for Vulkan, SDL, and base library, after that, we can start executing imgui things. You can call imgui functions at any point between `Imgui::NewFrame()`, and when the render function is called.

Last thing is to render the imgui objects.

On the `draw()` function, we call `ImGui::Render();` at the start. 

As part of your main renderpass, call `ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);` right before you end it. 
This will make imgui render as part as your main pass. If you have an UI pass of some kind, thats a good place to put it.

Thats really all that was needed, so enjoy using imgui!






