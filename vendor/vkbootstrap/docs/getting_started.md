# Getting Started

`vk-bootstrap` reduces the complexity of dealing with `VkInstance`, `VkPhysicalDevice`, and `VkDevice`, the three things every Vulkan app needs to start.

- [Getting Started](#getting-started)
- [Build Pattern](#build-pattern)
- [Instance Creation](#instance-creation)
  - [Versions](#versions)
  - [Enabling Instance Level Extensions and Layers](#enabling-instance-level-extensions-and-layers)
- [Physical Device Selection](#physical-device-selection)
- [Physical Device Object](#physical-device-object)
- [Device Creation](#device-creation)
  - [Queues](#queues)
    - [Custom queue setup](#custom-queue-setup)
- [Dispatch Table](#dispatch-table)
- [WSI](#wsi)
  - [For those who don't want a surface](#for-those-who-dont-want-a-surface)
  - [Surface Creation](#surface-creation)
  - [Swapchain](#swapchain)
- [Cleanup](#cleanup)
- [Creating your own debug callback](#creating-your-own-debug-callback)


# Build Pattern

`vk-bootstrap` uses a "build" pattern to creat the objects.

```cpp
vkb::Result<vkb::Wrapper> result = vkb::WrapperBuilder()
                                            .set_thing()
                                            .more_things()
                                            .build();

if (!result) { /* handle error */ }

// The result also holds the vk-bootstrap wrapper
vkb::Wrapper wrapper = result.value();

// The underlying Vulkan handle can easily be acquired
VkObject object = wrapper.object;
```

# Instance Creation

Taking a look at a simple example

```cpp
vkb::InstanceBuilder instance_builder;
auto instance_ret = instance_builder
                            .set_app_name("Awesome Vulkan Application")
                            .set_engine_name("Excellent Game Engine")
                            .require_api_version(1,0,0)
                            .build(); // build is always called last

// simple error checking and helpful error messages
if (!instance_ret) {
    std::cerr << "Failed to create Vulkan instance. Error: " << instance_ret.error().message() << "\n";
    return -1;
}

// Get handle and use however you want!
VkInstance instance = instance_ret.value();
```

## Versions

There are many "versions" in the world of Vulkan and `InstanceBuilder` lets you set them all!

```cpp
vkb::InstanceBuilder instance_builder;

// Sets VkApplicationInfo::applicationVersion
instance_builder.set_app_version(3, 0, 0);
// Sets VkApplicationInfo::engineVersion
instance_builder.set_engine_version(5, 0, 2);

// Set the required API version of your Vulkan app
// This will make sure you have a 1.3 Loader and 1.3 Physical Devices
instance_builder.require_api_version(1, 3, 0);

// Lower the required Instance version - used when you need a higher Physical Device version than Instance version
instance_builder.set_minimum_instance_version(1, 2, 0)
```

## Enabling Instance Level Extensions and Layers

The `vkb::SystemInfo` provides a way to work with the instance build. (see [System Info Example](../example/system_info.cpp))

```cpp
vkb::InstanceBuilder instance_builder;

auto system_info_ret = vkb::SystemInfo::get_system_info();
if (!system_info_ret) { /* report error */ }
auto system_info = system_info_ret.value();

// check for a layer
if (system_info.is_layer_available("VK_LAYER_LUNARG_api_dump")) {
    instance_builder.enable_layer("VK_LAYER_LUNARG_api_dump");
}

// of course dedicated variable for validation
if (system_info.validation_layers_available){
    instance_builder.enable_validation_layers()
    // Validation needs to send errors via a callback, have vk-bootstrap do it
                    .use_default_debug_messenger();

}

// if you need an instance level extension
if (system_info.is_extension_available("VK_KHR_get_physical_device_properties2")) {
    instance_builder.enable_extension("VK_KHR_get_physical_device_properties2");
}
```

# Physical Device Selection

Once a Vulkan instance has been created, the next step is to find a suitable GPU for the application to use. `vk-bootstrap` provide the `vkb::PhysicalDeviceSelector` class to streamline this process.

Creating a `vkb::PhysicalDeviceSelector` requires a valid `vkb::Instance` to construct.

```cpp
vkb::PhysicalDeviceSelector phys_device_selector(vkb_instance);
// select() grabs a PhysicalDevice
// By default, this will prefer a discrete GPU.
auto physical_device_selector_return = phys_device_selector.require_things().select();
if (!physical_device_selector_return) {

    // If no suitable devices were found, detailed_failure_reasons() will contain a list of reasons why.
    if (physical_device_selector_return.error() == vkb::PhysicalDeviceError::no_suitable_device) {
        const auto& detailed_reasons = physical_device_selector_return.detailed_failure_reasons();
        if (!detailed_reasons.empty()) {
            std::cerr << "GPU Selection failure reasons:\n";
            for (const std::string& reason : detailed_reasons) {
                std::cerr << reason << "\n";
            }
        }
    }
    /* handle error */
}
```

The `vkb::PhysicalDeviceSelector` will look for the first device in the list that satisfied all the specified criteria, and if none is found, will return the first device that partially satisfies the criteria.

The various "require" functions indicate to `vk-bootstrap` what features and capabilities are necessary for an application. A "require" function will fail any `VkPhysicalDevice` that doesn't satisfy the constraint.

For example, "requiring" certain device extensions to be supported is done as follows:

```cpp
// Application cannot function without this extension
phys_device_selector.add_required_extension("VK_KHR_timeline_semaphore");

// Application can deal with the lack of this extension
phys_device_selector.add_desired_extension("VK_KHR_imageless_framebuffer");
```

While requiring that certain features are available is as follows:

```cpp
VkPhysicalDeviceFeatures required_features{};
required_features.multiViewport = true;

phys_device_selector.set_required_features(required_features);
```

To enable features for newer versions of Vulkan, use `set_required_features_11()`, `set_required_features_12()`, etc, and follow the same pattern as `set_required_features()` of passing in the features struct, corresponding to the version.

> Note that `set_required_features_11()` was released with 1.2, so it cannot be used for 1.1 only capable Vulkan devices.

Features only available through extensions need to use `add_required_extension_features()`. For example:

```cpp
VkPhysicalDeviceDescriptorIndexingFeatures descriptor_indexing_features{};
descriptor_indexing_features.<features_used> = true;

phys_device_selector.add_required_extension_features(descriptor_indexing_features);
```

The features and extensions used as selection criteria in `vkb::PhysicalDeviceSelector` automatically propagate into `vkb::DeviceBuilder`. That means the application only needs to state the feature requirement once, and `vk-bootstrap` will handle enabling it on the resulting device.

Note:

Because `vk-bootstrap` does not manage creating a `VkSurfaceKHR` handle, it is explicitly passed into the `vkb::PhysicalDeviceSelector` for proper querying of surface support details. (See [the WSI section](#wsi))


# Physical Device Object

The `vkb::PhysicalDevice` represents a chosen physical device, along with all the necessary details about how to create a `VkDevice` from it with the requested features and extensions. While most use cases will simply give the  `vkb::PhysicalDevice` to `vkb::DeviceBuilder`, there are a handful of useful things that can be done with it.

Adding optional extensions. It is occasionally useful to enable features if they are present but not require that they be available on the physical device.

This is done using `enable_extension_if_present()` as follows.

```cpp
bool supported = phys_device.enable_extension_if_present("VK_KHR_timeline_semaphore");
if (supported){
    // allows easy feedback whether an extension is supported or not.
}
```

Use `enable_extensions_if_present()` to check if a group of extensions are available, and enable all of them if they are all present. This will *not* enable any extension unless they are all present, useful for handling dependencies between extensions, where one extension requires another one to be enabled.

# Device Creation

Once a `VkPhysicalDevice` has been selected, a `VkDevice` can be created. Facilitating that is the `vkb::DeviceBuilder`. Creation and usage follows the forms laid out by `vkb::InstanceBuilder`.

```cpp
vkb::DeviceBuilder device_builder{ phys_device};
auto dev_ret = device_builder.build ();
if (!dev_ret) {
    // error
}
vkb::Device vkb_device = dev_ret.value();
 ```

## Queues

By default, `vkb::DeviceBuilder` will enable one queue from each queue family available on the `VkPhysicalDevice`. This is done because in practice, most use cases only need a single queue from each family.

To get a `VkQueue` or the index of a `VkQueue`, use the `get_queue(QueueType type)` and `get_queue_index(QueueType type)` functions of `vkb::Device`. These will return the appropriate `VkQueue` or `uint32_t` if they exist and were enabled, else they will return an error.

```cpp
auto queue_ret = vkb_device.get_queue (vkb::QueueType::graphics);
if (!queue_ret) {
    // handle error
}
graphics_queue = queue_ret.value ();
```

Queue families represent a set of queues with similar operations, such as graphics, transfer, and compute. Because not all Vulkan hardware has queue families for each operation category, an application should be able to handle the presence or lack of certain queue families. For this reason the `get_dedicated_queue`  and `get_dedicated_queue_index` functions of `vkb::Device` exist to allow applications to easily know if there is a queue dedicated to a particular operation, such as compute or transfer operations.

### Custom queue setup

If an application wishes to have more fine grained control over their queue setup, they should create a `std::vector` of `vkb::CustomQueueDescription` which describe the index, count and a `std::vector<float>` of priorities. To build up such a vector, use the `get_queue_families` function in `vkb::PhysicalDevice` to get a `std::vector<VkQueueFamilyProperties>`

For example
```cpp
std::vector<vkb::CustomQueueDescription> queue_descriptions;
auto queue_families = phys_device.get_queue_families ();
for (uint32_t i = 0; i < static_cast<uint32_t>(queue_families.size ()); i++) {
    if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        // Find the first queue family with graphics operations supported
        queue_descriptions.push_back (vkb::CustomQueueDescription (
            i, std::vector<float> (queue_families[i].queueCount, 1.0f)));
    }
}
```

# Dispatch Table

`vk-bootstrap` will create the dispatch table for you!

```cpp
// Assume you built a valid instance/device
vkb::Instance instance;
vkb::Device device;

// Table are not built and ready to use!
// (Instance dispatch likely not needed as vk-bootstrap queries most things already)
vkb::InstanceDispatchTable inst_disp = instance.make_table();
vkb::DispatchTable disp = device.make_table();

VkBuffer my_buffer;
VkBufferCreateInfo buffer_info = {};
buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
buffer_info.size = 1024;
buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

// Calls vkCreateBuffer() for you, automatically can infer the first VkDevice parameter
disp.createBuffer(&buffer_info, nullptr, &my_buffer);
```

# WSI

## For those who don't want a surface

In the `vkb::InstanceBuilder` just call `.set_headless()` and that is it!

```cpp
vkb::InstanceBuilder instance_builder;
// Skip vk-bootstrap trying to create WSI for you
instance_builder.set_headless()
auto instance_ret = instance_builder.build();
```

> If this is not set, you might see a `no_surface_provided` error.

## Surface Creation

Presenting images to the screen Vulkan requires creating a surface, encapsulated in a `VkSurfaceKHR` handle. Creating a surface is the responsibility of the windowing system, thus is out of scope for `vk-bootstrap`. However, `vk-bootstrap` does try to make the process as painless as possible by automatically enabling the correct windowing extensions in `VkInstance` creation.

> If an application does intend to present but cannot create a `VkSurfaceKHR` handle before physical device selection, use `defer_surface_initialization()` to disable the `no_surface_provided` error.

Windowing libraries which support Vulkan usually provide a way of getting the `VkSurfaceKHR` handle for the window. These methods require a valid Vulkan instance, thus must be done after instance creation.

Examples for GLFW and SDL2 are listed below.
```cpp
vkb::Instance vkb_instance; //valid vkb::Instance
VkSurfaceKHR surface = nullptr;
// window is a valid library specific Window handle

// GLFW
VkResult err = glfwCreateWindowSurface (vkb_instance.instance, window, NULL, &surface);
if (err != VK_SUCCESS) { /* handle error */ }

// SDL2
SDL_bool err = SDL_Vulkan_CreateSurface(window, vkb_instance.instance, &surface);
if (!err){ /* handle error */ }
```

## Swapchain

Creating a swapchain follows the same form outlined by `vkb::InstanceBuilder` and `vkb::DeviceBuilder`. Create the `vkb::SwapchainBuilder`, provide `vkb::Device`, call the appropriate builder functions, and call `build()`.

```cpp
vkb::SwapchainBuilder swapchain_builder{ device };
auto swap_ret = swapchain_builder.build ();
if (!swap_ret){

}
vkb::Swapchain swapchain = swap_ret.value();
```

By default, the swapchain will use the VK_FORMAT_B8G8R8A8_SRGB or VK_FORMAT_R8G8B8A8_SRGB image format with the color space VK_COLOR_SPACE_SRGB_NONLINEAR_KHR. The present mode will default to VK_PRESENT_MODE_MAILBOX_KHR if available and fallback to VK_PRESENT_MODE_FIFO_KHR. The image usage default flag is VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT.

Recreating the swapchain is equivalent to creating a new swapchain but providing the old swapchain as a source. Be sure to not use the same `VkSwapchainKHR` again as it expires when it is recycled after trying to create a new swapchain.
```cpp
vkb::SwapchainBuilder swapchain_builder{ device };
auto swap_ret = swapchain_builder.set_old_swapchain (vkb_swapchain)
                                 .build ();
if (!swap_ret){
    // If it failed to create a swapchain, the old swapchain handle is invalid.
    vkb_swapchain.swapchain = VK_NULL_HANDLE;
}
// Even though we recycled the previous swapchain, we need to free its resources.
vkb::destroy_swapchain(vkb_swapchain);
// Get the new swapchain and place it in our variable
vkb_swapchain = swap_ret.value();
```

# Cleanup

The cleanup at the end of your program is simple

```cpp
vkb::Instance instance;
VkSurfaceKHR surface;
vkb::Device device;
vkb::Swapchain swapchain;

// crazy fast Vulkan renderer logic

vkb::destroy_swapchain(swapchain);
vkb::destroy_device(device);
vkb::destroy_surface(instance, surface);
vkb::destroy_instance(instance);
```

# Creating your own debug callback

When setting up the Validation Layers there is a default `Debug Messenger Callback`:

```cpp
instance_builder.request_validation_layers().use_default_debug_messenger()
```

For those who want to control what and where the "Validation Layers" log its output

```cpp
instance_builder.set_debug_callback (
    [] (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	    VkDebugUtilsMessageTypeFlagsEXT messageType,
	    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	    void *pUserData)
        -> VkBool32 {
			auto severity = vkb::to_string_message_severity(messageSeverity);
			auto type = vkb::to_string_message_type(messageType);
			printf ("[%s: %s] %s\n", severity, type, pCallbackData->pMessage);
			return VK_FALSE;
		}
    );
```

See [custom_debug_callback.cpp](../example/custom_debug_callback.cpp) for a working example.
