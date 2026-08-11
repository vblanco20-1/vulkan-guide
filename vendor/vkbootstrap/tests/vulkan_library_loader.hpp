#pragma once

#include <stdio.h>

#include <memory>
#include <iostream>

#if defined(_WIN32)
#include <fcntl.h>
#define NOMINMAX
#include <windows.h>
#endif // _WIN32

#include "../src/VkBootstrap.h"

#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
#include <dlfcn.h>
#endif

struct VulkanLibrary {
#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
    void* library;
#elif defined(_WIN32)

    HMODULE library;
#endif

    VulkanLibrary() {
#if defined(__linux__) || defined(__FreeBSD__)
        library = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
        if (!library) library = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
#elif defined(__APPLE__)
        library = dlopen("libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
        if (!library) library = dlopen("libvulkan.1.dylib", RTLD_NOW | RTLD_LOCAL);
#elif defined(_WIN32)
        library = LoadLibrary(TEXT("vulkan-1.dll"));
#else
        assert(false && "Unsupported platform");
#endif
        if (!library) return;
#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
        vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(library, "vkGetInstanceProcAddr"));
#elif defined(_WIN32)
        vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetProcAddress(library, "vkGetInstanceProcAddr"));
#endif
    }

    void close() {
#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
        dlclose(library);
#elif defined(_WIN32)
        FreeLibrary(library);
#endif
        library = 0;
    }

    void init(VkInstance instance) {
        vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(instance, "vkGetDeviceProcAddr");
    }

    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = VK_NULL_HANDLE;
    PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = VK_NULL_HANDLE;
};
