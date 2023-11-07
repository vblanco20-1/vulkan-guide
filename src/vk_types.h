// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <fmt/core.h>

#define VK_CHECK(x)                                                     \
    do {                                                                \
        VkResult err = x;                                               \
        if (err) {                                                      \
            fmt::println("Detected Vulkan error: {}", string_VkResult(err)); \
            abort();                                                    \
        }                                                               \
    } while (0)