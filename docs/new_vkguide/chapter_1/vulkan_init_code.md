---
layout: default
title: Vulkan Initialization Code
parent:  "New 1. Initializing Vulkan"
nav_order: 2
---


# Initializing core Vulkan structures
The explanations assume that you start from the code of chapter-0. If you don't have the project setup, please grab the code of chapter-0 and compile it.

The first thing to do, is to `#include` the vkBootstrap library that we will be using to simplify the initialization code.
For that, go to the top of vk_engine.cpp, and just include the `"VkBootstrap.h"` header

```cpp
// --- other includes ---
#include <vk_types.h>
#include <vk_initializers.h>

//bootstrap library
#include "VkBootstrap.h"

```

VkBootstrap will remove hundreds of lines of code from our engine, simplifying the startup code by a considerable amount. If you want to learn how to do that yourself without vkbootstrap, you can try reading the first chapter of vulkan-tutorial [here](https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Base_code)


The first thing we are need to initialize is the Vulkan instance. For that, let's start by adding a new function and the stored handles to the VulkanEngine class. Lets also add some extra functions to the engine class for the different stages of initialization.

vk_engine.h
```cpp
class VulkanEngine {
public:

    // --- omitted ---

	VkInstance _instance;// Vulkan library handle
	VkDebugUtilsMessengerEXT _debug_messenger;// Vulkan debug output handle
	VkPhysicalDevice _chosenGPU;// GPU chosen as the default device
	VkDevice _device; // Vulkan device for commands
	VkSurfaceKHR _surface;// Vulkan window surface

private:

	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_sync_structures();
```

We will now call those init functions in order from our engine init function

vk_engine.cpp
```cpp
void VulkanEngine::init()
{
	// We initialize SDL and create a window with it. 
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
	
	_window = SDL_CreateWindow(
		"Vulkan Engine",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		_windowExtent.width,
		_windowExtent.height,
		window_flags
	);

	init_vulkan();

	init_swapchain();

	init_commands();

	init_sync_structures();

	//everything went fine
	_isInitialized = true;
}

void VulkanEngine::init_vulkan()
{
    //nothing yet
}
void VulkanEngine::init_swapchain()
{
    //nothing yet
}
void VulkanEngine::init_commands()
{
    //nothing yet
}
void VulkanEngine::init_sync_structures()
{
    //nothing yet
}
```

We have added 4 handles to the main class, VkDevice, VkPhysicalDevice, VkInstance, VkDebugUtilsMessengerEXT.

## Instance

Now that our new init_Vulkan function is added, we can start filling it with the code needed to create the instance.


```cpp
void VulkanEngine::init_vulkan()
{

	vkb::InstanceBuilder builder;

	bool bUseValidationLayers = true;

	//make the vulkan instance, with basic debug features
	auto inst_ret = builder.set_app_name("Example Vulkan Application")
		.request_validation_layers(bUseValidationLayers)
		.use_default_debug_messenger()
		.require_api_version(1, 3, 0)
		.build();

	vkb::Instance vkb_inst = inst_ret.value();

	//grab the instance 
	_instance = vkb_inst.instance;
	_debug_messenger = vkb_inst.debug_messenger;


}
```

We are going to create a vkb::InstanceBuilder, which is from the VkBootstrap library, and abstracts the creation of a Vulkan VkInstance.

For the creation of the instance, we want it to have the name "Example Vulkan Application", have validation layers enabled, and use default debug logger.
The "Example Vulkan Application" name does not matter. You can set the name to whatever you want. 
When initializing a VkInstance, the name of the application and engine is supplied. This is so that driver vendors have a easier time finding the name of the game/engine, so they can tweak internal driver logic for them alone. For us, it's not really important.

We want to enable validation layers by default. With what we are going to do during the guide, there is no need to ever turn them off, as they will catch our errors very nicely. On a more advanced engine, you would only enable the layers in debug mode, or with a specific configuration parameter. Vulkan validation layers can slow down the performance of the vulkan calls significantly, so once we begin loading complex scenes with lots of data, we will want to disable them to see what the real performance of the code is.

We also require the Vulkan api version 1.3. This should be supported on gpus that are relatively modern. We will be taking advantage of the features given by that vulkan version. If you are on a old PC/gpu that does not support those features, then you will have to follow the older version of this guide, which targets 1.1.

Lastly, we tell the library that we want the debug messenger. This is what catches the log messages that the validation layers will output. Because we have no need for a dedicated one, we will just let the library use the default one, which outputs to console window.

Then we just grab the actual VkInstance handle and the VkDebugUtilsMessengerEXT handle from the vkb::Instance object.
We store the VkDebugUtilsMessengerEXT so we can destroy it at program exit, otherwise we would leak it.


## Device
```cpp
void VulkanEngine::init_vulkan()
{
	// other code ------

	SDL_Vulkan_CreateSurface(_window, _instance, &_surface);

	VkPhysicalDeviceVulkan13Features features{};
	features.dynamicRendering = true;
	features.synchronization2 = true;

	//use vkbootstrap to select a gpu. 
	//We want a gpu that can write to the SDL surface and supports vulkan 1.3
	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version(1, 3)
		.set_required_features_13(features)
		.set_surface(_surface)
		.select()
		.value();


	//create the final vulkan device
	vkb::DeviceBuilder deviceBuilder{ physicalDevice };

	vkb::Device vkbDevice = deviceBuilder.build().value();

	// Get the VkDevice handle used in the rest of a vulkan application
	_device = vkbDevice.device;
	_chosenGPU = physicalDevice.physical_device;

}
```
To select a GPU to use, we are going to use vkb::PhysicalDeviceSelector.

First of all, we need to create a VkSurfaceKHR object from the SDL window. This is the actual window we will be rendering to, so we need to tell the physical device selector to grab a GPU that can render to said window.

We need to enable some features. For now, those are dynamic rendering, and syncronization 2. Those are optional features provided in vulkan 1.3 that change a few things. dynamic rendering allows us to completely skip renderpasses/framebuffers (if you want to learn about them, they are explained in the old version of vkguide), and also use a new upgraded version of the syncronization functions. By giving the vkb::PhysicalDeviceSelector the `VkPhysicalDeviceVulkan13Features` structure, we can tell vkbootstrap to find a gpu that has those features.



Once we have a VkPhysicalDevice, we can directly build a VkDevice from it.

And at the end, we store the handles in the class.

That's it, we have initialized Vulkan. We can now start calling Vulkan commands.

If you run the project right now, it will crash if you dont have a gpu with the required features or vulkan drivers that dont support them. If that happens, make sure your drivers are updated. If its just something your GPU doesnt support, go with the older version of vkguide as you wont be able to follow this tutorial.

But before we start executing commands, there is one last thing to do.

## Setting up the swapchain

Last thing from the core initialization is to initialize the swapchain, so we can have something to render into.

Begin by adding new members and functions to VulkanEngine

```cpp
class VulkanEngine {
public:
	// --- other code ---

	VkSwapchainKHR _swapchain;
	VkFormat _swachainImageFormat;

	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;
}

```

We are storing the VkSwapchainKHR itself, alongside the format that the swapchain images use when rendering to them.

We also store 2 arrays, one of Images, and another of ImageViews.

A VkImage is a handle to the actual image object to use as texture or to render into. A VkImageView is a wrapper for that image. It allows to do things like swap the colors. We will go into detail about it later.

Like with the other initialization functions, we are going to use the vkb library to create a swapchain. It uses a builder similar to the ones we used for instance and device.

<!-- codegen from tag init_swap on file E:\ProgrammingProjects\vulkan-guide-2\chapter-1/vk_engine.cpp --> 
```cpp
void VulkanEngine::init_swapchain()
{
	vkb::SwapchainBuilder swapchainBuilder{_chosenGPU,_device,_surface };

	vkb::Swapchain vkbSwapchain = swapchainBuilder
		//.set_desired_format({ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		//use vsync present mode
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(_windowExtent.width, _windowExtent.height)
		.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.build()
		.value();

	//store swapchain and its related images
	_swapchain = vkbSwapchain.swapchain;
	_swapchainImages = vkbSwapchain.get_images().value();
	_swapchainImageViews = vkbSwapchain.get_image_views().value();

	_swachainImageFormat = vkbSwapchain.image_format;
}
```

The most important detail here is the present mode, which we have set to `VK_PRESENT_MODE_FIFO_KHR`. This way we are doing a hard VSync, which will limit the FPS of the entire engine to the speed of the monitor. It's a good way to have a FPS limit for now.

We also send the window sizes to the swapchain. This is important as creating a swapchain will also create the images for it, so the size is locked. If you need to resize the window, the swapchain will need to be rebuild.

Once the swapchain is built, we just store all of its stuff into the members of VulkanEngine class.


## Cleaning up resources
We need to make sure that all of the Vulkan resources we create are correctly deleted, when the app exists.

For that, go to the `VulkanEngine::cleanup()` function

```cpp
void VulkanEngine::cleanup()
{
	if (_isInitialized) {

		vkDestroySwapchainKHR(_device, _swapchain, nullptr);

		//destroy swapchain resources
		for (int i = 0; i < _swapchainImageViews.size(); i++) {

			vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
		}

		vkDestroyDevice(_device, nullptr);
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
		vkDestroyInstance(_instance, nullptr);
		SDL_DestroyWindow(_window);
	}
}
```

It is imperative that objects are destroyed in the opposite order that they are created. In some cases, if you know what you are doing, the order can be changed a bit and it will be fine, but destroying the objects in reverse order is an easy way to have it work.

VkPhysicalDevice can't be destroyed, as it's not a Vulkan resource per-se, it's more like just a handle to a GPU in the system.

Because our initialization order was SDL Window -> Instance -> Surface -> Device -> Swapchain, we are doing exactly the opposite order for destruction.

If you try to run the program now, it should do nothing, but that nothing also includes not emitting errors.

There is no need to destroy the Images in this specific case, because the images are owned and destroyed with the swapchain.


## Validation layer errors
Just to check that our validation layers are working, let's try to call the destruction functions in the wrong order

```cpp
void VulkanEngine::cleanup()
{
	if (_isInitialized) {
		//ERROR - Instance destroyed before others
		vkDestroyInstance(_instance, nullptr);

		vkDestroySwapchainKHR(_device, _swapchain, nullptr);

		//destroy swapchain resources
		for (int i = 0; i < _swapchainImageViews.size(); i++) {

			vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
		}

		vkDestroyDevice(_device, nullptr);
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
		SDL_DestroyWindow(_window);
	}
}
```

We are now destroying the Instance before the Device and the Surface (which was created from the Instance) is also deleted. The validation layers should complain with an error like this.

```
[ERROR: Validation]
Validation Error: [ VUID-vkDestroyInstance-instance-00629 ] Object 0: handle = 0x24ff02340c0, type = VK_OBJECT_TYPE_INSTANCE; Object 1: handle = 0xf8ce070000000002, type = VK_OBJECT_TYPE_SURFACE_KHR; | MessageID = 0x8b3d8e18 | OBJ ERROR : For VkInstance 0x24ff02340c0[], VkSurfaceKHR 0xf8ce070000000002[] has not been destroyed. The Vulkan spec states: All child objects created using instance must have been destroyed prior to destroying instance (https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#VUID-vkDestroyInstance-instance-00629)
```

With the Vulkan initialization completed and the layers working, we can begin to prepare the command structures so that we can make the gpu do something.

