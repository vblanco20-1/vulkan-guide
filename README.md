# `vk-bootstrap`

A utility library that jump starts initialization of Vulkan

This library simplifies the tedious process of:

* Instance creation
* Physical Device selection
* Device creation
* Getting queues
* Swapchain creation

It also adds several conveniences for:

* Enabling validation layers
* Adding a debug callback messenger
* Enabling extensions on a physical device
* Select a gpu based on a set of criteria like features, extensions, memory, etc

Read the [Getting Started](docs/getting_started.md) guide for a quick start on using `vk-bootstrap`

## Basic Usage

See [basic_usage.cpp](./example/basic_usage.cpp) to see 500 lines slimmed down to about 50

```cpp
#include "VkBootstrap.h"

void init_vulkan () {
    vkb::InstanceBuilder builder;
    auto inst_ret = builder.set_app_name ("Example Vulkan Application")
                        .request_validation_layers ()
                        .use_default_debug_messenger ()
                        .build ();
    if (!inst_ret) { /* report */ }
    vkb::Instance vkb_inst = inst_ret.value ();

    vkb::PhysicalDeviceSelector selector{ vkb_inst };
    auto phys_ret = selector.set_surface (surface)
                        .set_minimum_version (1, 1)
                        .require_dedicated_transfer_queue ()
                        .select ();
    if (!phys_ret) { /* report */ }

    vkb::DeviceBuilder device_builder{ phys_ret.value () };
    auto dev_ret = device_builder.build ();
    if (!dev_ret) { /* report */ }
    vkb::Device vkb_device = dev_ret.value ();

    auto graphics_queue_ret = vkb_device.get_queue (vkb::QueueType::graphics);
    if (!graphics_queue_ret)  { /* report */ }
    VkQueue graphics_queue = graphics_queue_ret.value ();
}
```

### More fun quick examples

- [triangle.cpp](./example/triangle.cpp) - renders a triangle to the screen.
- [simple_compute.cpp](./example/simple_compute.cpp) - Does a simple add with a compute shader and no surface in less than 300 lines.

## Setting up `vk-bootstrap`

This library has no external dependencies beyond C++17, its standard library, and at least the 1.1 version of the Vulkan Headers.

Note: on Unix platforms, `vk-bootstrap` will require the dynamic linker in order to compile as the library doesn't link against `vulkan-1.dll`/`libvulkan.so` directly.

### Copy-Paste

Copy the `src/VkBootstrap.h`, `src/VkBootstrapDispatch.h`, and `src/VkBootstrap.cpp` files into your project, include them into your build, then compile as you normally would.

`vk-bootstrap` is *not* a header only library, so no need to worry about macros in the header.

#### Linux specific

vk-bootstrap will load the required symbols at runtime, which requires that the application is linked to the system dynamic link.
How the dynamic linker is linked into the project depends on the build system in question.
If CMake is being used, link vk-bootstrap with `${CMAKE_DL_LIBS}`.

### git-submodule + CMake

Add this project as a git-submodule into the root directory. Suggested is using a subdirectory to hold all submodules.

```bash
git submodule add https://github.com/charles-lunarg/vk-bootstrap
```

With CMake, add the subdirectory to include the project

```cmake
add_subdirectory(vk-bootstrap)
```

Then use `target_link_libraries` to use the library in whichever target needs it.

```cmake
target_link_libraries(your_application_name vk-bootstrap::vk-bootstrap)
```

### CMake Fetch Content
If cmake 3.12 is available, use the FetchContent capability of cmake to directly download and build the library for you.

```cmake
include(FetchContent)
FetchContent_Declare(
    fetch_vk_bootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
    GIT_TAG        BRANCH_OR_TAG #suggest using a tag so the library doesn't update whenever new commits are pushed to a branch
)
FetchContent_MakeAvailable(fetch_vk_bootstrap)
target_link_libraries(your_application_name vk-bootstrap::vk-bootstrap)
```

### Manually Building

```bash
git clone https://github.com/charles-lunarg/vk-bootstrap
cd vk-bootstrap
mkdir build
cd build
cmake ..
```

### Vulkan-Headers dependency

By default, when using vk-bootstrap through CMake, it will attempt to locate the Vulkan-Headers on the system and fall back to downloading them directly if they aren't present. If the `VK_BOOTSTRAP_VULKAN_HEADER_DIR` option is specified, it will use that directory instead.


### Testing

Tests will be enabled if you open this project standalone. If you include this project as a subdirectory or sub-project, you can force enable tests by setting the option `VK_BOOTSTRAP_TEST` to `ON`. Testing requires GLFW and Catch2 but are acquired automatically using cmake fetch content.

```bash
cmake ../path/to/your_project/ -DVK_BOOTSTRAP_TEST=ON
```

### Build Options
| Name                             | Type   | Default Value | Description                                                                                                                                                               |
| -------------------------------- | ------ | ------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `VK_BOOTSTRAP_WERROR`            | bool   | `OFF`         | Enable warnings as errors during compilation.                                                                                                                             |
| `VK_BOOTSTRAP_TEST`              | bool   | `OFF`         | Enable building of the tests in this project. Will download GLFW and Catch2 automatically if enabled.                                                                     |
| `VK_BOOTSTRAP_VULKAN_HEADER_DIR` | string | `""`          | Optional. Specify the directory that contains the Vulkan Headers. Useful if you are downloading the headers manually and don't want vk-bootstrap to download them itself. |
