---
layout: default
title: Vulkan Initialization Code
parent: Chapter 1
nav_order: 2
---


## Starting point
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

Next thing we are going to do is to add a VK_CHECK macro to the top of vk_engine.cpp . This will just immediately crash whenever a Vulkan error is not handled, useful as errors are likely here.


```cpp
#include <iostream>

//we want to immediately abort when there is an error. In normal engines this would give an error message to the user, or perform a dump of state.
using namespace std;
#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			std::cout <<"Detected Vulkan error: " << err << std::endl; \
			abort();                                                \
		}                                                           \
	} while (0)
```

All Vulkan functions that can error out will return a VkResult. This is really just an integer error code. If the error code isnt 0, something is going badly and we just abort with an error message. 

With those two done, we can go forward with initialization of the basic structures for Vulkan.

# Initializing core Vulkan structures


The first thing we are going to intialize, is the Vulkan instance. For that, lets start by adding a new function and the stored handles to the VulkanEngine class

vk_engine.h
```cpp
class VulkanEngine {
public:

    // --- ommited --- 
    VkInstance _instance; // Vulkan library handle
	VkPhysicalDevice _chosenGPU; // gpu chosen as the default device
	VkDevice _device; // Vulkan device for commands
	VkSurfaceKHR _surface; // Vulkan window surface
private:
		
	void init_vulkan();
```

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

	//load the core Vulkan structures
	init_vulkan();

	//everything went fine
	_isInitialized = true;
}

void VulkanEngine::init_vulkan()
{
    //nothing yet
}
```

We have added 3 handles to the main class, VkDevice, VkPhysicalDevice, and VkInstance.

## Instance

Now that our new init_Vulkan function is added, we can start filling it.


```cpp
	vkb::InstanceBuilder builder;

	//make the Vulkan instance, with basic debug features
	auto inst_ret = builder.set_app_name("Example Vulkan Application")
		.request_validation_layers(true)
		.use_default_debug_messenger()
		.build();

	vkb::Instance vkb_inst = inst_ret.value();

	//store the instance 
	_instance = vkb_inst.instance;
```

We are going to create a vkb::InstanceBuilder, which is from the VkBootstrap library, and simplifies the creation of a Vulkan VkInstance.

For the creation of the instance, we want it to have the name "Example Vulkan Application", have validation layers enabled, and use default debug logger.
The "Example Vulkan Application" name is completely meaningless. If you want to change it to anything, it wont be a problem.
When initializing a VkInstance, the name of the application and engine is supplied. This is so driver vendors can detect the names of AAA games, so they can tweak internal driver logic for them alone. For normal people, its not really important.

We want to enable validation layers by default, hardcoded. With what we are going to do during the guide, there is no need to ever turn them off, as they will catch our errors very nicely. On a more advanced engine, you would only enable the layers in debug mode, or with a specific configuration parameter.

Lastly, we tell the library that we want the debug messenger. This is what catches the log messages that the validation layers will output. Because we have no need for a dedicated one, we will just let the library use one that just directly outputs to console.

We then just grab the actual VkInstance handle from the vkb::Instance object.

## Device

```cpp
	// get the surface of the window we opened with SDL    
	SDL_Vulkan_CreateSurface(_window, _instance, &_surface);

	//use vkbootstrap to select a gpu. 
	//We want a gpu that can write to the SDL surface and supports Vulkan 1.1
	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version(1, 1)
		.set_surface(_surface)
		.select()
		.value();

	//create the final Vulkan device
	vkb::DeviceBuilder deviceBuilder{ physicalDevice };

	vkb::Device vkbDevice = deviceBuilder.build().value();

	// Get the VkDevice handle used in the rest of a Vulkan application
	_device = vkbDevice.device;
	_chosenGPU = physicalDevice.physical_device;
```

To select a gpu to use, we are going to use vkb::PhysicalDeviceSelector.

First of all, we need to create a VkSurfaceKHR object from the SDL window. This is the actual window we will be rendering to, so we need to tell the physical device selector to grab a gpu that can render to said window.

For the gpu selector, we just want Vulkan 1.1 support and the window surface, so there is not much to find. The library will make sure to select the dedicated gpu in the system.

Once we have a VkPhysicalDevice, we can directly build a VkDevice from it. 

And at the end, we store the handles in the class.

Thats it, we have initialized Vulkan. We can now start calling Vulkan commands.

But before we start executing commands, there is one last thing to do.

## Setting up the swapchain

Last thing from the core initialization is to initialize the swapchain, so we can have something to render into.

Begin by adding new members and functions to VulkanEngine

```cpp
class VulkanEngine {
public:
	// --- other code ---
	VkSwapchainKHR _swapchain; // from other articles

	// image format expected by the windowing system
	VkFormat _swachainImageFormat; 	
	
	//array of images from the swapchain
	std::vector<VkImage> _swapchainImages;

	//array of image-views from the swapchain
	std::vector<VkImageView> _swapchainImageViews;

	// --- other code ---
private:
	// --- other code ---
	void init_swapchain();	
}

```

We are storing the VkSwapchainKHR itself, alongside the format that the swapchain images use when rendering to them.

We also store 2 arrays, one of Images, and another of ImageViews. 

A VkImage is a handle to the actual image object to use as texture or to render into. A VkImageView is a wrapper for that image. It allows to do things like swap the colors. We will go into detail about it on chapter 5 when explaining textures.

We call `init_swapchain()` on the main init function, right after calling `init_vulkan()`


vk_engine.cpp
```cpp
void VulkanEngine::init()
{
	// --- SDL stuff ----

	//load the core Vulkan structures
	init_vulkan();

	//create the swapchain
	init_swapchain();

	//everything went fine
	_isInitialized = true;
}

void VulkanEngine::init_swapchain()
{
    //nothing yet
}
```

Like with the other initialization functions, we are going to use the vkb library to create a swapchain.

```cpp
void VulkanEngine::init_swapchain()
{
	vkb::SwapchainBuilder swapchainBuilder{_chosenGPU,_device,_surface };

	vkb::Swapchain vkbSwapchain = swapchainBuilder
		.use_default_format_selection()
		//use vsync present mode
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(_windowExtent.width, _windowExtent.height)
		.build()
		.value();

	//store swapchain and its related images
	_swapchain = vkbSwapchain.swapchain;
	_swapchainImages = vkbSwapchain.get_images().value();
	_swapchainImageViews = vkbSwapchain.get_image_views().value();

	_swachainImageFormat = vkbSwapchain.image_format;

}
```

The most important detail here is the present mode, which we have set to `VK_PRESENT_MODE_FIFO_KHR`. This way we are doing a hard VSync, which will limit the FPS of the entire engine to the speed of the monitor. Its a good way to have a FPS limit for now.

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
		vkDestroyInstance(_instance, nullptr);
		SDL_DestroyWindow(_window);
	}
}
```

It is imperative that objects are destroyed in the opposite order that they are created. In some cases, if you know what you are doing, the order can be changed a bit and it will be fine, but destroying the objects in reverse order is an easy way to have it work.

VkPhysicalDevice cant be destroyed, as its not a Vulkan resource per-se, its more like just a handle to a gpu in the system.

Because our initialization order was SDL Window -> Instance -> Surface -> Device -> Swapchain, we are doing exactly the opposite order for destruction.

If you try to run the program now, it should do nothing, but that nothing also includes not emitting errors.

There is no need to destroy the Images in this specific case, because the images are owned and destroyed with the swapchain. 


## Validation layer errors
Just to check that our validation layers are working, lets try to call the destruction functions in the wrong order

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
		SDL_DestroyWindow(_window);
	}
}
```

We are now destroying the Instance before the Device and the Surface (which was created from the Instance) is also deleted. The validation layers should complain with a errors like this.

```
[ERROR: Validation]
Validation Error: [ VUID-vkDestroyInstance-instance-00629 ] Object 0: handle = 0x24ff02340c0, type = VK_OBJECT_TYPE_INSTANCE; Object 1: handle = 0xf8ce070000000002, type = VK_OBJECT_TYPE_SURFACE_KHR; | MessageID = 0x8b3d8e18 | OBJ ERROR : For VkInstance 0x24ff02340c0[], VkSurfaceKHR 0xf8ce070000000002[] has not been destroyed. The Vulkan spec states: All child objects created using instance must have been destroyed prior to destroying instance (https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#VUID-vkDestroyInstance-instance-00629)
```

If you get one of those errors about a VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT not being deleted, don't worry, that's just the debug logger, and its not important.



With the Vulkan initialization completed and the layers working, we can begin to prepare the actual render loop and command execution.


Next: [Vulkan Render Passes]({{ site.baseurl }}{% link docs/chapter-1/vulkan_renderpass.md %})