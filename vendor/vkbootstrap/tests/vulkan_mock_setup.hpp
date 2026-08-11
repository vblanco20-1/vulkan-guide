#pragma once

#include <algorithm>

#include "vulkan_mock.hpp"

#include <catch2/catch_test_macros.hpp>

#include <VkBootstrap.h>

vkb::Instance get_instance(uint32_t minor_version = 0);

vkb::Instance get_headless_instance(uint32_t minor_version = 0);

VkExtensionProperties get_extension_properties(const char* extName);

VulkanMock& get_and_setup_default();

VulkanMock::PhysicalDeviceDetails& add_basic_physical_device(VulkanMock& mock);

VulkanMock::SurfaceDetails get_basic_surface_details();
