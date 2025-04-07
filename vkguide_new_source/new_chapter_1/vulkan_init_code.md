---
layout: default
title: Vulkan Initialization Code
parent:  "1. Initializing Vulkan"
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

^code inst_init chapter-1/vk_engine.h

private:

	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_sync_structures();
```

We will now call those init functions in order from our engine init function

vk_engine.cpp
```cpp
^code init_fn chapter-1/vk_engine.cpp

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

We have added 4 handles to the main class, `VkDevice`, `VkPhysicalDevice`, `VkInstance`, `VkDebugUtilsMessengerEXT`.

## Instance

Now that our new init_Vulkan function is added, we can start filling it with the code needed to create the instance.


```cpp
void VulkanEngine::init_vulkan()
{
^code init_instance chapter-1/vk_engine.cpp
}
```

We are going to create a `vkb::InstanceBuilder`, which is from the VkBootstrap library, and abstracts the creation of a Vulkan `VkInstance`.

For the creation of the instance, we want it to have the name "Example Vulkan Application", have validation layers enabled, and use default debug logger.
The "Example Vulkan Application" name does not matter. You can set the name to whatever you want. 
When initializing a `VkInstance`, the name of the application and engine is supplied. This is so that driver vendors have a easier time finding the name of the game/engine, so they can tweak internal driver logic for them alone. For us, it's not really important.

We want to enable validation layers by default. With what we are going to do during the guide, there is no need to ever turn them off, as they will catch our errors very nicely. On a more advanced engine, you would only enable the layers in debug mode, or with a specific configuration parameter. Vulkan validation layers can slow down the performance of the vulkan calls significantly, so once we begin loading complex scenes with lots of data, we will want to disable them to see what the real performance of the code is.

We also require the Vulkan api version 1.3. This should be supported on gpus that are relatively modern. We will be taking advantage of the features given by that vulkan version. If you are on a old PC/gpu that does not support those features, then you will have to follow the older version of this guide, which targets 1.1.

Lastly, we tell the library that we want the debug messenger. This is what catches the log messages that the validation layers will output. Because we have no need for a dedicated one, we will just let the library use the default one, which outputs to console window.

Then we just grab the actual VkInstance handle and the `VkDebugUtilsMessengerEXT` handle from the `vkb::Instance` object.
We store the `VkDebugUtilsMessengerEXT` so we can destroy it at program exit, otherwise we would leak it.


## Device
```cpp
void VulkanEngine::init_vulkan()
{
	// other code ------

^code init_device chapter-1/vk_engine.cpp
}
```
To select a GPU to use, we are going to use `vkb::PhysicalDeviceSelector`.

First of all, we need to create a `VkSurfaceKHR` object from the SDL window. This is the actual window we will be rendering to, so we need to tell the physical device selector to grab a GPU that can render to said window.

We need to enable some features. First some vulkan 1.3 features, those are dynamic rendering, and syncronization 2. Those are optional features provided in vulkan 1.3 that change a few things. dynamic rendering allows us to completely skip renderpasses/framebuffers (if you want to learn about them, they are explained in the old version of vkguide), and also use a new upgraded version of the syncronization functions. 
We are also going to use the vulkan 1.2 features `bufferDeviceAddress` and `descriptorIndexing`. Buffer device address will let us use GPU pointers without binding buffers, and descriptorIndexing gives us bindless textures.

By giving the `vkb::PhysicalDeviceSelector` the `VkPhysicalDeviceVulkan13Features` structure , we can tell vkbootstrap to find a gpu that has those features. 


There are multiple levels of feature structs you can use depending on your vulkan version, you can find their info here: 

[Vulkan Spec: 1.0 physical device features](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap47.html#VkPhysicalDeviceFeatures)
[Vulkan Spec: 1.1 physical device features](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap47.html#VkPhysicalDeviceVulkan11Features)
[Vulkan Spec: 1.2 physical device features](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap47.html#VkPhysicalDeviceVulkan12Features)
[Vulkan Spec: 1.3 physical device features](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap47.html#VkPhysicalDeviceVulkan13Features)


Once we have a `VkPhysicalDevice`, we can directly build a VkDevice from it.

And at the end, we store the handles in the class.

That's it, we have initialized Vulkan. We can now start calling Vulkan commands.

If you run the project right now, it will crash if you dont have a gpu with the required features or vulkan drivers that dont support them. If that happens, make sure your drivers are updated. If its just something your GPU doesnt support, go with the older version of vkguide as you wont be able to follow this tutorial.

## Setting up the swapchain

Last thing from the core initialization is to initialize the swapchain, so we can have something to render into.

Begin by adding new members and functions to VulkanEngine

```cpp
class VulkanEngine {
public:
	// --- other code ---

^code swap_init chapter-1/vk_engine.h

private: 
	void create_swapchain(uint32_t width, uint32_t height);
	void destroy_swapchain();
}
```

We are storing the `VkSwapchainKHR` itself, alongside the format that the swapchain images use when rendering to them.

We also store 2 arrays, one of Images, and another of ImageViews.

A `VkImage` is a handle to the actual image object to use as texture or to render into. A `VkImageView` is a wrapper for that image. It allows to do things like swap the colors. We will go into detail about it later.

We are also adding create and destroy functions for the swapchain. 

Like with the other initialization functions, we are going to use the vkb library to create a swapchain. It uses a builder similar to the ones we used for instance and device.

^code init_swap chapter-1/vk_engine.cpp

From create_swapchain, we make the swapchain structures, and then we call the function from `init_swapchain()`

The most important detail here is the present mode, which we have set to `VK_PRESENT_MODE_FIFO_KHR`. This way we are doing a hard VSync, which will limit the FPS of the entire engine to the speed of the monitor.

We also send the window sizes to the swapchain. This is important as creating a swapchain will also create the images for it, so the size is locked. Later in the tutorial we will need to rebuild the swapchain as the window resizes, so we have them separated from the init flow, but in the init flow we default that size to the window size.

Once the swapchain is built, we just store all of its stuff into the members of VulkanEngine class.

Lets write the `destroy_swapchain()` function too.

^code destroy_sc chapter-1/vk_engine.cpp

We first delete the swapchain object, which will delete the images it holds internally. We then have to destroy the ImageViews for those images.

## Cleaning up resources
We need to make sure that all of the Vulkan resources we create are correctly deleted, when the app exists.

For that, go to the `VulkanEngine::cleanup()` function

```cpp
void VulkanEngine::cleanup()
{
	if (_isInitialized) {

		destroy_swapchain();

		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		vkDestroyDevice(_device, nullptr);
		
		vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
		vkDestroyInstance(_instance, nullptr);
		SDL_DestroyWindow(_window);
	}
}
```

Objects have dependencies on each other, and we need to delete them in the correct order. Deleting them in the opposite order they were created is a good way of doing it. In some cases, if you know what you are doing, the order can be changed a bit and it will be fine.

`VkPhysicalDevice` can't be destroyed, as it's not a Vulkan resource per-se, it's more like just a handle to a GPU in the system.

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

		destroy_swapchain();

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

^nextlink

{% include comments.html term="Vkguide 2 Beta Comments" %}