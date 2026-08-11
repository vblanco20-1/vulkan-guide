
// Enable the platform specific parts of the API
#if defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#endif

#if defined(__APPLE__)
#define VK_USE_PLATFORM_IOS_MVK
#define VK_USE_PLATFORM_MACOS_MVK
#define VK_USE_PLATFORM_METAL_EXT
#endif

#if defined(WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#if defined(__linux__) || defined(__FreeBSD__)
#define VK_USE_PLATFORM_WAYLAND_KHR
#define VK_USE_PLATFORM_XCB_KHR
#define VK_USE_PLATFORM_XLIB_KHR
#define VK_USE_PLATFORM_XLIB_XRANDR_EXT
#endif

#define VK_ENABLE_BETA_EXTENSIONS

#include "VkBootstrap.h"
