/*
 * Copyright © 2025 Charles Giessen (charles@lunarg.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the “Software”), to deal in the Software without restriction, including without
 * limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

// This file is a part of VkBootstrap
// https://github.com/charles-lunarg/vk-bootstrap

#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
#if defined(VK_ENABLE_BETA_EXTENSIONS)
#include <vulkan/vulkan_beta.h>
#endif // defined(VK_ENABLE_BETA_EXTENSIONS)
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#include <vulkan/vulkan_android.h>
#endif // defined(VK_USE_PLATFORM_ANDROID_KHR)
#if defined(VK_USE_PLATFORM_SCREEN_QNX)
#include <screen/screen.h>
#include <vulkan/vulkan_screen.h>
#endif // defined(VK_USE_PLATFORM_SCREEN_QNX)

namespace vkb::detail {

void compare_VkPhysicalDeviceFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceFeatures const& supported, VkPhysicalDeviceFeatures const& requested);
void merge_VkPhysicalDeviceFeatures(VkPhysicalDeviceFeatures & current, VkPhysicalDeviceFeatures const& merge_in);
#if (defined(VK_VERSION_1_1))
void compare_VkPhysicalDeviceProtectedMemoryFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceProtectedMemoryFeatures const& supported, VkPhysicalDeviceProtectedMemoryFeatures const& requested);
void merge_VkPhysicalDeviceProtectedMemoryFeatures(VkPhysicalDeviceProtectedMemoryFeatures & current, VkPhysicalDeviceProtectedMemoryFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_1))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_16bit_storage))
void compare_VkPhysicalDevice16BitStorageFeatures(std::vector<std::string> & error_list, VkPhysicalDevice16BitStorageFeatures const& supported, VkPhysicalDevice16BitStorageFeatures const& requested);
void merge_VkPhysicalDevice16BitStorageFeatures(VkPhysicalDevice16BitStorageFeatures & current, VkPhysicalDevice16BitStorageFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_16bit_storage))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_16bit_storage))
void compare_VkPhysicalDevice16BitStorageFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevice16BitStorageFeaturesKHR const& supported, VkPhysicalDevice16BitStorageFeaturesKHR const& requested);
void merge_VkPhysicalDevice16BitStorageFeaturesKHR(VkPhysicalDevice16BitStorageFeaturesKHR & current, VkPhysicalDevice16BitStorageFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_16bit_storage))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
void compare_VkPhysicalDeviceVariablePointersFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceVariablePointersFeatures const& supported, VkPhysicalDeviceVariablePointersFeatures const& requested);
void merge_VkPhysicalDeviceVariablePointersFeatures(VkPhysicalDeviceVariablePointersFeatures & current, VkPhysicalDeviceVariablePointersFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
void compare_VkPhysicalDeviceVariablePointerFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceVariablePointerFeatures const& supported, VkPhysicalDeviceVariablePointerFeatures const& requested);
void merge_VkPhysicalDeviceVariablePointerFeatures(VkPhysicalDeviceVariablePointerFeatures & current, VkPhysicalDeviceVariablePointerFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
void compare_VkPhysicalDeviceVariablePointerFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVariablePointerFeaturesKHR const& supported, VkPhysicalDeviceVariablePointerFeaturesKHR const& requested);
void merge_VkPhysicalDeviceVariablePointerFeaturesKHR(VkPhysicalDeviceVariablePointerFeaturesKHR & current, VkPhysicalDeviceVariablePointerFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
void compare_VkPhysicalDeviceVariablePointersFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVariablePointersFeaturesKHR const& supported, VkPhysicalDeviceVariablePointersFeaturesKHR const& requested);
void merge_VkPhysicalDeviceVariablePointersFeaturesKHR(VkPhysicalDeviceVariablePointersFeaturesKHR & current, VkPhysicalDeviceVariablePointersFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_variable_pointers))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_sampler_ycbcr_conversion))
void compare_VkPhysicalDeviceSamplerYcbcrConversionFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceSamplerYcbcrConversionFeatures const& supported, VkPhysicalDeviceSamplerYcbcrConversionFeatures const& requested);
void merge_VkPhysicalDeviceSamplerYcbcrConversionFeatures(VkPhysicalDeviceSamplerYcbcrConversionFeatures & current, VkPhysicalDeviceSamplerYcbcrConversionFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_sampler_ycbcr_conversion))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_sampler_ycbcr_conversion))
void compare_VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR const& supported, VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR const& requested);
void merge_VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR(VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR & current, VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_sampler_ycbcr_conversion))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_multiview))
void compare_VkPhysicalDeviceMultiviewFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceMultiviewFeatures const& supported, VkPhysicalDeviceMultiviewFeatures const& requested);
void merge_VkPhysicalDeviceMultiviewFeatures(VkPhysicalDeviceMultiviewFeatures & current, VkPhysicalDeviceMultiviewFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_multiview))
#if (defined(VK_VERSION_1_1) || defined(VK_KHR_multiview))
void compare_VkPhysicalDeviceMultiviewFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMultiviewFeaturesKHR const& supported, VkPhysicalDeviceMultiviewFeaturesKHR const& requested);
void merge_VkPhysicalDeviceMultiviewFeaturesKHR(VkPhysicalDeviceMultiviewFeaturesKHR & current, VkPhysicalDeviceMultiviewFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_1) || defined(VK_KHR_multiview))
#if (defined(VK_VERSION_1_1))
void compare_VkPhysicalDeviceShaderDrawParametersFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceShaderDrawParametersFeatures const& supported, VkPhysicalDeviceShaderDrawParametersFeatures const& requested);
void merge_VkPhysicalDeviceShaderDrawParametersFeatures(VkPhysicalDeviceShaderDrawParametersFeatures & current, VkPhysicalDeviceShaderDrawParametersFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_1))
#if (defined(VK_VERSION_1_1))
void compare_VkPhysicalDeviceShaderDrawParameterFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceShaderDrawParameterFeatures const& supported, VkPhysicalDeviceShaderDrawParameterFeatures const& requested);
void merge_VkPhysicalDeviceShaderDrawParameterFeatures(VkPhysicalDeviceShaderDrawParameterFeatures & current, VkPhysicalDeviceShaderDrawParameterFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_1))
#if (defined(VK_VERSION_1_2))
void compare_VkPhysicalDeviceVulkan11Features(std::vector<std::string> & error_list, VkPhysicalDeviceVulkan11Features const& supported, VkPhysicalDeviceVulkan11Features const& requested);
void merge_VkPhysicalDeviceVulkan11Features(VkPhysicalDeviceVulkan11Features & current, VkPhysicalDeviceVulkan11Features const& merge_in);
#endif //(defined(VK_VERSION_1_2))
#if (defined(VK_VERSION_1_2))
void compare_VkPhysicalDeviceVulkan12Features(std::vector<std::string> & error_list, VkPhysicalDeviceVulkan12Features const& supported, VkPhysicalDeviceVulkan12Features const& requested);
void merge_VkPhysicalDeviceVulkan12Features(VkPhysicalDeviceVulkan12Features & current, VkPhysicalDeviceVulkan12Features const& merge_in);
#endif //(defined(VK_VERSION_1_2))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_vulkan_memory_model))
void compare_VkPhysicalDeviceVulkanMemoryModelFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceVulkanMemoryModelFeatures const& supported, VkPhysicalDeviceVulkanMemoryModelFeatures const& requested);
void merge_VkPhysicalDeviceVulkanMemoryModelFeatures(VkPhysicalDeviceVulkanMemoryModelFeatures & current, VkPhysicalDeviceVulkanMemoryModelFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_vulkan_memory_model))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_vulkan_memory_model))
void compare_VkPhysicalDeviceVulkanMemoryModelFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVulkanMemoryModelFeaturesKHR const& supported, VkPhysicalDeviceVulkanMemoryModelFeaturesKHR const& requested);
void merge_VkPhysicalDeviceVulkanMemoryModelFeaturesKHR(VkPhysicalDeviceVulkanMemoryModelFeaturesKHR & current, VkPhysicalDeviceVulkanMemoryModelFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_vulkan_memory_model))
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_host_query_reset))
void compare_VkPhysicalDeviceHostQueryResetFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceHostQueryResetFeatures const& supported, VkPhysicalDeviceHostQueryResetFeatures const& requested);
void merge_VkPhysicalDeviceHostQueryResetFeatures(VkPhysicalDeviceHostQueryResetFeatures & current, VkPhysicalDeviceHostQueryResetFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_EXT_host_query_reset))
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_host_query_reset))
void compare_VkPhysicalDeviceHostQueryResetFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceHostQueryResetFeaturesEXT const& supported, VkPhysicalDeviceHostQueryResetFeaturesEXT const& requested);
void merge_VkPhysicalDeviceHostQueryResetFeaturesEXT(VkPhysicalDeviceHostQueryResetFeaturesEXT & current, VkPhysicalDeviceHostQueryResetFeaturesEXT const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_EXT_host_query_reset))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_timeline_semaphore))
void compare_VkPhysicalDeviceTimelineSemaphoreFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceTimelineSemaphoreFeatures const& supported, VkPhysicalDeviceTimelineSemaphoreFeatures const& requested);
void merge_VkPhysicalDeviceTimelineSemaphoreFeatures(VkPhysicalDeviceTimelineSemaphoreFeatures & current, VkPhysicalDeviceTimelineSemaphoreFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_timeline_semaphore))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_timeline_semaphore))
void compare_VkPhysicalDeviceTimelineSemaphoreFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceTimelineSemaphoreFeaturesKHR const& supported, VkPhysicalDeviceTimelineSemaphoreFeaturesKHR const& requested);
void merge_VkPhysicalDeviceTimelineSemaphoreFeaturesKHR(VkPhysicalDeviceTimelineSemaphoreFeaturesKHR & current, VkPhysicalDeviceTimelineSemaphoreFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_timeline_semaphore))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_buffer_device_address))
void compare_VkPhysicalDeviceBufferDeviceAddressFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceBufferDeviceAddressFeatures const& supported, VkPhysicalDeviceBufferDeviceAddressFeatures const& requested);
void merge_VkPhysicalDeviceBufferDeviceAddressFeatures(VkPhysicalDeviceBufferDeviceAddressFeatures & current, VkPhysicalDeviceBufferDeviceAddressFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_buffer_device_address))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_buffer_device_address))
void compare_VkPhysicalDeviceBufferDeviceAddressFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceBufferDeviceAddressFeaturesKHR const& supported, VkPhysicalDeviceBufferDeviceAddressFeaturesKHR const& requested);
void merge_VkPhysicalDeviceBufferDeviceAddressFeaturesKHR(VkPhysicalDeviceBufferDeviceAddressFeaturesKHR & current, VkPhysicalDeviceBufferDeviceAddressFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_buffer_device_address))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_8bit_storage))
void compare_VkPhysicalDevice8BitStorageFeatures(std::vector<std::string> & error_list, VkPhysicalDevice8BitStorageFeatures const& supported, VkPhysicalDevice8BitStorageFeatures const& requested);
void merge_VkPhysicalDevice8BitStorageFeatures(VkPhysicalDevice8BitStorageFeatures & current, VkPhysicalDevice8BitStorageFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_8bit_storage))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_8bit_storage))
void compare_VkPhysicalDevice8BitStorageFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevice8BitStorageFeaturesKHR const& supported, VkPhysicalDevice8BitStorageFeaturesKHR const& requested);
void merge_VkPhysicalDevice8BitStorageFeaturesKHR(VkPhysicalDevice8BitStorageFeaturesKHR & current, VkPhysicalDevice8BitStorageFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_8bit_storage))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_atomic_int64))
void compare_VkPhysicalDeviceShaderAtomicInt64Features(std::vector<std::string> & error_list, VkPhysicalDeviceShaderAtomicInt64Features const& supported, VkPhysicalDeviceShaderAtomicInt64Features const& requested);
void merge_VkPhysicalDeviceShaderAtomicInt64Features(VkPhysicalDeviceShaderAtomicInt64Features & current, VkPhysicalDeviceShaderAtomicInt64Features const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_shader_atomic_int64))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_atomic_int64))
void compare_VkPhysicalDeviceShaderAtomicInt64FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderAtomicInt64FeaturesKHR const& supported, VkPhysicalDeviceShaderAtomicInt64FeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderAtomicInt64FeaturesKHR(VkPhysicalDeviceShaderAtomicInt64FeaturesKHR & current, VkPhysicalDeviceShaderAtomicInt64FeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_shader_atomic_int64))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
void compare_VkPhysicalDeviceShaderFloat16Int8Features(std::vector<std::string> & error_list, VkPhysicalDeviceShaderFloat16Int8Features const& supported, VkPhysicalDeviceShaderFloat16Int8Features const& requested);
void merge_VkPhysicalDeviceShaderFloat16Int8Features(VkPhysicalDeviceShaderFloat16Int8Features & current, VkPhysicalDeviceShaderFloat16Int8Features const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
void compare_VkPhysicalDeviceShaderFloat16Int8FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderFloat16Int8FeaturesKHR const& supported, VkPhysicalDeviceShaderFloat16Int8FeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderFloat16Int8FeaturesKHR(VkPhysicalDeviceShaderFloat16Int8FeaturesKHR & current, VkPhysicalDeviceShaderFloat16Int8FeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
void compare_VkPhysicalDeviceFloat16Int8FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceFloat16Int8FeaturesKHR const& supported, VkPhysicalDeviceFloat16Int8FeaturesKHR const& requested);
void merge_VkPhysicalDeviceFloat16Int8FeaturesKHR(VkPhysicalDeviceFloat16Int8FeaturesKHR & current, VkPhysicalDeviceFloat16Int8FeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_shader_float16_int8))
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_descriptor_indexing))
void compare_VkPhysicalDeviceDescriptorIndexingFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceDescriptorIndexingFeatures const& supported, VkPhysicalDeviceDescriptorIndexingFeatures const& requested);
void merge_VkPhysicalDeviceDescriptorIndexingFeatures(VkPhysicalDeviceDescriptorIndexingFeatures & current, VkPhysicalDeviceDescriptorIndexingFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_EXT_descriptor_indexing))
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_descriptor_indexing))
void compare_VkPhysicalDeviceDescriptorIndexingFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDescriptorIndexingFeaturesEXT const& supported, VkPhysicalDeviceDescriptorIndexingFeaturesEXT const& requested);
void merge_VkPhysicalDeviceDescriptorIndexingFeaturesEXT(VkPhysicalDeviceDescriptorIndexingFeaturesEXT & current, VkPhysicalDeviceDescriptorIndexingFeaturesEXT const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_EXT_descriptor_indexing))
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_scalar_block_layout))
void compare_VkPhysicalDeviceScalarBlockLayoutFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceScalarBlockLayoutFeatures const& supported, VkPhysicalDeviceScalarBlockLayoutFeatures const& requested);
void merge_VkPhysicalDeviceScalarBlockLayoutFeatures(VkPhysicalDeviceScalarBlockLayoutFeatures & current, VkPhysicalDeviceScalarBlockLayoutFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_EXT_scalar_block_layout))
#if (defined(VK_VERSION_1_2) || defined(VK_EXT_scalar_block_layout))
void compare_VkPhysicalDeviceScalarBlockLayoutFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceScalarBlockLayoutFeaturesEXT const& supported, VkPhysicalDeviceScalarBlockLayoutFeaturesEXT const& requested);
void merge_VkPhysicalDeviceScalarBlockLayoutFeaturesEXT(VkPhysicalDeviceScalarBlockLayoutFeaturesEXT & current, VkPhysicalDeviceScalarBlockLayoutFeaturesEXT const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_EXT_scalar_block_layout))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_uniform_buffer_standard_layout))
void compare_VkPhysicalDeviceUniformBufferStandardLayoutFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceUniformBufferStandardLayoutFeatures const& supported, VkPhysicalDeviceUniformBufferStandardLayoutFeatures const& requested);
void merge_VkPhysicalDeviceUniformBufferStandardLayoutFeatures(VkPhysicalDeviceUniformBufferStandardLayoutFeatures & current, VkPhysicalDeviceUniformBufferStandardLayoutFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_uniform_buffer_standard_layout))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_uniform_buffer_standard_layout))
void compare_VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR const& supported, VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR const& requested);
void merge_VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR(VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR & current, VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_uniform_buffer_standard_layout))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_subgroup_extended_types))
void compare_VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures const& supported, VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures const& requested);
void merge_VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures(VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures & current, VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_shader_subgroup_extended_types))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_shader_subgroup_extended_types))
void compare_VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR const& supported, VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR(VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR & current, VkPhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_shader_subgroup_extended_types))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_imageless_framebuffer))
void compare_VkPhysicalDeviceImagelessFramebufferFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceImagelessFramebufferFeatures const& supported, VkPhysicalDeviceImagelessFramebufferFeatures const& requested);
void merge_VkPhysicalDeviceImagelessFramebufferFeatures(VkPhysicalDeviceImagelessFramebufferFeatures & current, VkPhysicalDeviceImagelessFramebufferFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_imageless_framebuffer))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_imageless_framebuffer))
void compare_VkPhysicalDeviceImagelessFramebufferFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceImagelessFramebufferFeaturesKHR const& supported, VkPhysicalDeviceImagelessFramebufferFeaturesKHR const& requested);
void merge_VkPhysicalDeviceImagelessFramebufferFeaturesKHR(VkPhysicalDeviceImagelessFramebufferFeaturesKHR & current, VkPhysicalDeviceImagelessFramebufferFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_imageless_framebuffer))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_separate_depth_stencil_layouts))
void compare_VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures const& supported, VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures const& requested);
void merge_VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures(VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures & current, VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_separate_depth_stencil_layouts))
#if (defined(VK_VERSION_1_2) || defined(VK_KHR_separate_depth_stencil_layouts))
void compare_VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR const& supported, VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR const& requested);
void merge_VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR(VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR & current, VkPhysicalDeviceSeparateDepthStencilLayoutsFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_2) || defined(VK_KHR_separate_depth_stencil_layouts))
#if (defined(VK_VERSION_1_3))
void compare_VkPhysicalDeviceVulkan13Features(std::vector<std::string> & error_list, VkPhysicalDeviceVulkan13Features const& supported, VkPhysicalDeviceVulkan13Features const& requested);
void merge_VkPhysicalDeviceVulkan13Features(VkPhysicalDeviceVulkan13Features & current, VkPhysicalDeviceVulkan13Features const& merge_in);
#endif //(defined(VK_VERSION_1_3))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_private_data))
void compare_VkPhysicalDevicePrivateDataFeatures(std::vector<std::string> & error_list, VkPhysicalDevicePrivateDataFeatures const& supported, VkPhysicalDevicePrivateDataFeatures const& requested);
void merge_VkPhysicalDevicePrivateDataFeatures(VkPhysicalDevicePrivateDataFeatures & current, VkPhysicalDevicePrivateDataFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_private_data))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_private_data))
void compare_VkPhysicalDevicePrivateDataFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePrivateDataFeaturesEXT const& supported, VkPhysicalDevicePrivateDataFeaturesEXT const& requested);
void merge_VkPhysicalDevicePrivateDataFeaturesEXT(VkPhysicalDevicePrivateDataFeaturesEXT & current, VkPhysicalDevicePrivateDataFeaturesEXT const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_private_data))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2))
void compare_VkPhysicalDeviceSynchronization2Features(std::vector<std::string> & error_list, VkPhysicalDeviceSynchronization2Features const& supported, VkPhysicalDeviceSynchronization2Features const& requested);
void merge_VkPhysicalDeviceSynchronization2Features(VkPhysicalDeviceSynchronization2Features & current, VkPhysicalDeviceSynchronization2Features const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2))
void compare_VkPhysicalDeviceSynchronization2FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceSynchronization2FeaturesKHR const& supported, VkPhysicalDeviceSynchronization2FeaturesKHR const& requested);
void merge_VkPhysicalDeviceSynchronization2FeaturesKHR(VkPhysicalDeviceSynchronization2FeaturesKHR & current, VkPhysicalDeviceSynchronization2FeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_texture_compression_astc_hdr))
void compare_VkPhysicalDeviceTextureCompressionASTCHDRFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceTextureCompressionASTCHDRFeatures const& supported, VkPhysicalDeviceTextureCompressionASTCHDRFeatures const& requested);
void merge_VkPhysicalDeviceTextureCompressionASTCHDRFeatures(VkPhysicalDeviceTextureCompressionASTCHDRFeatures & current, VkPhysicalDeviceTextureCompressionASTCHDRFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_texture_compression_astc_hdr))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_texture_compression_astc_hdr))
void compare_VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT const& supported, VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT const& requested);
void merge_VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT(VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT & current, VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_texture_compression_astc_hdr))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_maintenance4))
void compare_VkPhysicalDeviceMaintenance4Features(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance4Features const& supported, VkPhysicalDeviceMaintenance4Features const& requested);
void merge_VkPhysicalDeviceMaintenance4Features(VkPhysicalDeviceMaintenance4Features & current, VkPhysicalDeviceMaintenance4Features const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_maintenance4))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_maintenance4))
void compare_VkPhysicalDeviceMaintenance4FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance4FeaturesKHR const& supported, VkPhysicalDeviceMaintenance4FeaturesKHR const& requested);
void merge_VkPhysicalDeviceMaintenance4FeaturesKHR(VkPhysicalDeviceMaintenance4FeaturesKHR & current, VkPhysicalDeviceMaintenance4FeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_maintenance4))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_shader_terminate_invocation))
void compare_VkPhysicalDeviceShaderTerminateInvocationFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceShaderTerminateInvocationFeatures const& supported, VkPhysicalDeviceShaderTerminateInvocationFeatures const& requested);
void merge_VkPhysicalDeviceShaderTerminateInvocationFeatures(VkPhysicalDeviceShaderTerminateInvocationFeatures & current, VkPhysicalDeviceShaderTerminateInvocationFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_shader_terminate_invocation))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_shader_terminate_invocation))
void compare_VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR const& supported, VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR(VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR & current, VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_shader_terminate_invocation))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_shader_demote_to_helper_invocation))
void compare_VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures const& supported, VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures const& requested);
void merge_VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures(VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures & current, VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_shader_demote_to_helper_invocation))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_shader_demote_to_helper_invocation))
void compare_VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT const& supported, VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT const& requested);
void merge_VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT(VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT & current, VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_shader_demote_to_helper_invocation))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_pipeline_creation_cache_control))
void compare_VkPhysicalDevicePipelineCreationCacheControlFeatures(std::vector<std::string> & error_list, VkPhysicalDevicePipelineCreationCacheControlFeatures const& supported, VkPhysicalDevicePipelineCreationCacheControlFeatures const& requested);
void merge_VkPhysicalDevicePipelineCreationCacheControlFeatures(VkPhysicalDevicePipelineCreationCacheControlFeatures & current, VkPhysicalDevicePipelineCreationCacheControlFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_pipeline_creation_cache_control))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_pipeline_creation_cache_control))
void compare_VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT const& supported, VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT const& requested);
void merge_VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT(VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT & current, VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_pipeline_creation_cache_control))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_zero_initialize_workgroup_memory))
void compare_VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures const& supported, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures const& requested);
void merge_VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures(VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures & current, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_zero_initialize_workgroup_memory))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_zero_initialize_workgroup_memory))
void compare_VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR const& supported, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR const& requested);
void merge_VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR(VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR & current, VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_zero_initialize_workgroup_memory))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_image_robustness))
void compare_VkPhysicalDeviceImageRobustnessFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceImageRobustnessFeatures const& supported, VkPhysicalDeviceImageRobustnessFeatures const& requested);
void merge_VkPhysicalDeviceImageRobustnessFeatures(VkPhysicalDeviceImageRobustnessFeatures & current, VkPhysicalDeviceImageRobustnessFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_image_robustness))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_image_robustness))
void compare_VkPhysicalDeviceImageRobustnessFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceImageRobustnessFeaturesEXT const& supported, VkPhysicalDeviceImageRobustnessFeaturesEXT const& requested);
void merge_VkPhysicalDeviceImageRobustnessFeaturesEXT(VkPhysicalDeviceImageRobustnessFeaturesEXT & current, VkPhysicalDeviceImageRobustnessFeaturesEXT const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_image_robustness))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_subgroup_size_control))
void compare_VkPhysicalDeviceSubgroupSizeControlFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceSubgroupSizeControlFeatures const& supported, VkPhysicalDeviceSubgroupSizeControlFeatures const& requested);
void merge_VkPhysicalDeviceSubgroupSizeControlFeatures(VkPhysicalDeviceSubgroupSizeControlFeatures & current, VkPhysicalDeviceSubgroupSizeControlFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_subgroup_size_control))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_subgroup_size_control))
void compare_VkPhysicalDeviceSubgroupSizeControlFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceSubgroupSizeControlFeaturesEXT const& supported, VkPhysicalDeviceSubgroupSizeControlFeaturesEXT const& requested);
void merge_VkPhysicalDeviceSubgroupSizeControlFeaturesEXT(VkPhysicalDeviceSubgroupSizeControlFeaturesEXT & current, VkPhysicalDeviceSubgroupSizeControlFeaturesEXT const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_subgroup_size_control))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_inline_uniform_block))
void compare_VkPhysicalDeviceInlineUniformBlockFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceInlineUniformBlockFeatures const& supported, VkPhysicalDeviceInlineUniformBlockFeatures const& requested);
void merge_VkPhysicalDeviceInlineUniformBlockFeatures(VkPhysicalDeviceInlineUniformBlockFeatures & current, VkPhysicalDeviceInlineUniformBlockFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_inline_uniform_block))
#if (defined(VK_VERSION_1_3) || defined(VK_EXT_inline_uniform_block))
void compare_VkPhysicalDeviceInlineUniformBlockFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceInlineUniformBlockFeaturesEXT const& supported, VkPhysicalDeviceInlineUniformBlockFeaturesEXT const& requested);
void merge_VkPhysicalDeviceInlineUniformBlockFeaturesEXT(VkPhysicalDeviceInlineUniformBlockFeaturesEXT & current, VkPhysicalDeviceInlineUniformBlockFeaturesEXT const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_EXT_inline_uniform_block))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_shader_integer_dot_product))
void compare_VkPhysicalDeviceShaderIntegerDotProductFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceShaderIntegerDotProductFeatures const& supported, VkPhysicalDeviceShaderIntegerDotProductFeatures const& requested);
void merge_VkPhysicalDeviceShaderIntegerDotProductFeatures(VkPhysicalDeviceShaderIntegerDotProductFeatures & current, VkPhysicalDeviceShaderIntegerDotProductFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_shader_integer_dot_product))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_shader_integer_dot_product))
void compare_VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR const& supported, VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR(VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR & current, VkPhysicalDeviceShaderIntegerDotProductFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_shader_integer_dot_product))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_dynamic_rendering))
void compare_VkPhysicalDeviceDynamicRenderingFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceDynamicRenderingFeatures const& supported, VkPhysicalDeviceDynamicRenderingFeatures const& requested);
void merge_VkPhysicalDeviceDynamicRenderingFeatures(VkPhysicalDeviceDynamicRenderingFeatures & current, VkPhysicalDeviceDynamicRenderingFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_dynamic_rendering))
#if (defined(VK_VERSION_1_3) || defined(VK_KHR_dynamic_rendering))
void compare_VkPhysicalDeviceDynamicRenderingFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceDynamicRenderingFeaturesKHR const& supported, VkPhysicalDeviceDynamicRenderingFeaturesKHR const& requested);
void merge_VkPhysicalDeviceDynamicRenderingFeaturesKHR(VkPhysicalDeviceDynamicRenderingFeaturesKHR & current, VkPhysicalDeviceDynamicRenderingFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_3) || defined(VK_KHR_dynamic_rendering))
#if (defined(VK_VERSION_1_4))
void compare_VkPhysicalDeviceVulkan14Features(std::vector<std::string> & error_list, VkPhysicalDeviceVulkan14Features const& supported, VkPhysicalDeviceVulkan14Features const& requested);
void merge_VkPhysicalDeviceVulkan14Features(VkPhysicalDeviceVulkan14Features & current, VkPhysicalDeviceVulkan14Features const& merge_in);
#endif //(defined(VK_VERSION_1_4))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_global_priority) || defined(VK_EXT_global_priority_query))
void compare_VkPhysicalDeviceGlobalPriorityQueryFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceGlobalPriorityQueryFeatures const& supported, VkPhysicalDeviceGlobalPriorityQueryFeatures const& requested);
void merge_VkPhysicalDeviceGlobalPriorityQueryFeatures(VkPhysicalDeviceGlobalPriorityQueryFeatures & current, VkPhysicalDeviceGlobalPriorityQueryFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_global_priority) || defined(VK_EXT_global_priority_query))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_global_priority))
void compare_VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR const& supported, VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR const& requested);
void merge_VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR(VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR & current, VkPhysicalDeviceGlobalPriorityQueryFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_global_priority))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_global_priority_query))
void compare_VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT const& supported, VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT const& requested);
void merge_VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT(VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT & current, VkPhysicalDeviceGlobalPriorityQueryFeaturesEXT const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_global_priority_query))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_index_type_uint8) || defined(VK_EXT_index_type_uint8))
void compare_VkPhysicalDeviceIndexTypeUint8Features(std::vector<std::string> & error_list, VkPhysicalDeviceIndexTypeUint8Features const& supported, VkPhysicalDeviceIndexTypeUint8Features const& requested);
void merge_VkPhysicalDeviceIndexTypeUint8Features(VkPhysicalDeviceIndexTypeUint8Features & current, VkPhysicalDeviceIndexTypeUint8Features const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_index_type_uint8) || defined(VK_EXT_index_type_uint8))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_index_type_uint8))
void compare_VkPhysicalDeviceIndexTypeUint8FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceIndexTypeUint8FeaturesKHR const& supported, VkPhysicalDeviceIndexTypeUint8FeaturesKHR const& requested);
void merge_VkPhysicalDeviceIndexTypeUint8FeaturesKHR(VkPhysicalDeviceIndexTypeUint8FeaturesKHR & current, VkPhysicalDeviceIndexTypeUint8FeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_index_type_uint8))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_index_type_uint8))
void compare_VkPhysicalDeviceIndexTypeUint8FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceIndexTypeUint8FeaturesEXT const& supported, VkPhysicalDeviceIndexTypeUint8FeaturesEXT const& requested);
void merge_VkPhysicalDeviceIndexTypeUint8FeaturesEXT(VkPhysicalDeviceIndexTypeUint8FeaturesEXT & current, VkPhysicalDeviceIndexTypeUint8FeaturesEXT const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_index_type_uint8))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance5))
void compare_VkPhysicalDeviceMaintenance5Features(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance5Features const& supported, VkPhysicalDeviceMaintenance5Features const& requested);
void merge_VkPhysicalDeviceMaintenance5Features(VkPhysicalDeviceMaintenance5Features & current, VkPhysicalDeviceMaintenance5Features const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance5))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance5))
void compare_VkPhysicalDeviceMaintenance5FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance5FeaturesKHR const& supported, VkPhysicalDeviceMaintenance5FeaturesKHR const& requested);
void merge_VkPhysicalDeviceMaintenance5FeaturesKHR(VkPhysicalDeviceMaintenance5FeaturesKHR & current, VkPhysicalDeviceMaintenance5FeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance5))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance6))
void compare_VkPhysicalDeviceMaintenance6Features(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance6Features const& supported, VkPhysicalDeviceMaintenance6Features const& requested);
void merge_VkPhysicalDeviceMaintenance6Features(VkPhysicalDeviceMaintenance6Features & current, VkPhysicalDeviceMaintenance6Features const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance6))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance6))
void compare_VkPhysicalDeviceMaintenance6FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance6FeaturesKHR const& supported, VkPhysicalDeviceMaintenance6FeaturesKHR const& requested);
void merge_VkPhysicalDeviceMaintenance6FeaturesKHR(VkPhysicalDeviceMaintenance6FeaturesKHR & current, VkPhysicalDeviceMaintenance6FeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_maintenance6))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_host_image_copy))
void compare_VkPhysicalDeviceHostImageCopyFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceHostImageCopyFeatures const& supported, VkPhysicalDeviceHostImageCopyFeatures const& requested);
void merge_VkPhysicalDeviceHostImageCopyFeatures(VkPhysicalDeviceHostImageCopyFeatures & current, VkPhysicalDeviceHostImageCopyFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_host_image_copy))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_host_image_copy))
void compare_VkPhysicalDeviceHostImageCopyFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceHostImageCopyFeaturesEXT const& supported, VkPhysicalDeviceHostImageCopyFeaturesEXT const& requested);
void merge_VkPhysicalDeviceHostImageCopyFeaturesEXT(VkPhysicalDeviceHostImageCopyFeaturesEXT & current, VkPhysicalDeviceHostImageCopyFeaturesEXT const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_host_image_copy))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_subgroup_rotate))
void compare_VkPhysicalDeviceShaderSubgroupRotateFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceShaderSubgroupRotateFeatures const& supported, VkPhysicalDeviceShaderSubgroupRotateFeatures const& requested);
void merge_VkPhysicalDeviceShaderSubgroupRotateFeatures(VkPhysicalDeviceShaderSubgroupRotateFeatures & current, VkPhysicalDeviceShaderSubgroupRotateFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_shader_subgroup_rotate))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_subgroup_rotate))
void compare_VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR const& supported, VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR(VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR & current, VkPhysicalDeviceShaderSubgroupRotateFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_shader_subgroup_rotate))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_float_controls2))
void compare_VkPhysicalDeviceShaderFloatControls2Features(std::vector<std::string> & error_list, VkPhysicalDeviceShaderFloatControls2Features const& supported, VkPhysicalDeviceShaderFloatControls2Features const& requested);
void merge_VkPhysicalDeviceShaderFloatControls2Features(VkPhysicalDeviceShaderFloatControls2Features & current, VkPhysicalDeviceShaderFloatControls2Features const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_shader_float_controls2))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_float_controls2))
void compare_VkPhysicalDeviceShaderFloatControls2FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderFloatControls2FeaturesKHR const& supported, VkPhysicalDeviceShaderFloatControls2FeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderFloatControls2FeaturesKHR(VkPhysicalDeviceShaderFloatControls2FeaturesKHR & current, VkPhysicalDeviceShaderFloatControls2FeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_shader_float_controls2))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_expect_assume))
void compare_VkPhysicalDeviceShaderExpectAssumeFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceShaderExpectAssumeFeatures const& supported, VkPhysicalDeviceShaderExpectAssumeFeatures const& requested);
void merge_VkPhysicalDeviceShaderExpectAssumeFeatures(VkPhysicalDeviceShaderExpectAssumeFeatures & current, VkPhysicalDeviceShaderExpectAssumeFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_shader_expect_assume))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_shader_expect_assume))
void compare_VkPhysicalDeviceShaderExpectAssumeFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderExpectAssumeFeaturesKHR const& supported, VkPhysicalDeviceShaderExpectAssumeFeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderExpectAssumeFeaturesKHR(VkPhysicalDeviceShaderExpectAssumeFeaturesKHR & current, VkPhysicalDeviceShaderExpectAssumeFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_shader_expect_assume))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_protected_access))
void compare_VkPhysicalDevicePipelineProtectedAccessFeatures(std::vector<std::string> & error_list, VkPhysicalDevicePipelineProtectedAccessFeatures const& supported, VkPhysicalDevicePipelineProtectedAccessFeatures const& requested);
void merge_VkPhysicalDevicePipelineProtectedAccessFeatures(VkPhysicalDevicePipelineProtectedAccessFeatures & current, VkPhysicalDevicePipelineProtectedAccessFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_protected_access))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_protected_access))
void compare_VkPhysicalDevicePipelineProtectedAccessFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePipelineProtectedAccessFeaturesEXT const& supported, VkPhysicalDevicePipelineProtectedAccessFeaturesEXT const& requested);
void merge_VkPhysicalDevicePipelineProtectedAccessFeaturesEXT(VkPhysicalDevicePipelineProtectedAccessFeaturesEXT & current, VkPhysicalDevicePipelineProtectedAccessFeaturesEXT const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_protected_access))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_robustness))
void compare_VkPhysicalDevicePipelineRobustnessFeatures(std::vector<std::string> & error_list, VkPhysicalDevicePipelineRobustnessFeatures const& supported, VkPhysicalDevicePipelineRobustnessFeatures const& requested);
void merge_VkPhysicalDevicePipelineRobustnessFeatures(VkPhysicalDevicePipelineRobustnessFeatures & current, VkPhysicalDevicePipelineRobustnessFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_robustness))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_robustness))
void compare_VkPhysicalDevicePipelineRobustnessFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePipelineRobustnessFeaturesEXT const& supported, VkPhysicalDevicePipelineRobustnessFeaturesEXT const& requested);
void merge_VkPhysicalDevicePipelineRobustnessFeaturesEXT(VkPhysicalDevicePipelineRobustnessFeaturesEXT & current, VkPhysicalDevicePipelineRobustnessFeaturesEXT const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_pipeline_robustness))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_line_rasterization) || defined(VK_EXT_line_rasterization))
void compare_VkPhysicalDeviceLineRasterizationFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceLineRasterizationFeatures const& supported, VkPhysicalDeviceLineRasterizationFeatures const& requested);
void merge_VkPhysicalDeviceLineRasterizationFeatures(VkPhysicalDeviceLineRasterizationFeatures & current, VkPhysicalDeviceLineRasterizationFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_line_rasterization) || defined(VK_EXT_line_rasterization))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_line_rasterization))
void compare_VkPhysicalDeviceLineRasterizationFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceLineRasterizationFeaturesKHR const& supported, VkPhysicalDeviceLineRasterizationFeaturesKHR const& requested);
void merge_VkPhysicalDeviceLineRasterizationFeaturesKHR(VkPhysicalDeviceLineRasterizationFeaturesKHR & current, VkPhysicalDeviceLineRasterizationFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_line_rasterization))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_line_rasterization))
void compare_VkPhysicalDeviceLineRasterizationFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceLineRasterizationFeaturesEXT const& supported, VkPhysicalDeviceLineRasterizationFeaturesEXT const& requested);
void merge_VkPhysicalDeviceLineRasterizationFeaturesEXT(VkPhysicalDeviceLineRasterizationFeaturesEXT & current, VkPhysicalDeviceLineRasterizationFeaturesEXT const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_line_rasterization))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_vertex_attribute_divisor) || defined(VK_EXT_vertex_attribute_divisor))
void compare_VkPhysicalDeviceVertexAttributeDivisorFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceVertexAttributeDivisorFeatures const& supported, VkPhysicalDeviceVertexAttributeDivisorFeatures const& requested);
void merge_VkPhysicalDeviceVertexAttributeDivisorFeatures(VkPhysicalDeviceVertexAttributeDivisorFeatures & current, VkPhysicalDeviceVertexAttributeDivisorFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_vertex_attribute_divisor) || defined(VK_EXT_vertex_attribute_divisor))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_vertex_attribute_divisor))
void compare_VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR const& supported, VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR const& requested);
void merge_VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR(VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR & current, VkPhysicalDeviceVertexAttributeDivisorFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_vertex_attribute_divisor))
#if (defined(VK_VERSION_1_4) || defined(VK_EXT_vertex_attribute_divisor))
void compare_VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT const& supported, VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT const& requested);
void merge_VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT(VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT & current, VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_EXT_vertex_attribute_divisor))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_dynamic_rendering_local_read))
void compare_VkPhysicalDeviceDynamicRenderingLocalReadFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceDynamicRenderingLocalReadFeatures const& supported, VkPhysicalDeviceDynamicRenderingLocalReadFeatures const& requested);
void merge_VkPhysicalDeviceDynamicRenderingLocalReadFeatures(VkPhysicalDeviceDynamicRenderingLocalReadFeatures & current, VkPhysicalDeviceDynamicRenderingLocalReadFeatures const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_dynamic_rendering_local_read))
#if (defined(VK_VERSION_1_4) || defined(VK_KHR_dynamic_rendering_local_read))
void compare_VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR const& supported, VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR const& requested);
void merge_VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR(VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR & current, VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR const& merge_in);
#endif //(defined(VK_VERSION_1_4) || defined(VK_KHR_dynamic_rendering_local_read))
#if (defined(VK_KHR_performance_query))
void compare_VkPhysicalDevicePerformanceQueryFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePerformanceQueryFeaturesKHR const& supported, VkPhysicalDevicePerformanceQueryFeaturesKHR const& requested);
void merge_VkPhysicalDevicePerformanceQueryFeaturesKHR(VkPhysicalDevicePerformanceQueryFeaturesKHR & current, VkPhysicalDevicePerformanceQueryFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_performance_query))
#if (defined(VK_KHR_shader_bfloat16))
void compare_VkPhysicalDeviceShaderBfloat16FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderBfloat16FeaturesKHR const& supported, VkPhysicalDeviceShaderBfloat16FeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderBfloat16FeaturesKHR(VkPhysicalDeviceShaderBfloat16FeaturesKHR & current, VkPhysicalDeviceShaderBfloat16FeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_shader_bfloat16))
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_KHR_portability_subset))
void compare_VkPhysicalDevicePortabilitySubsetFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePortabilitySubsetFeaturesKHR const& supported, VkPhysicalDevicePortabilitySubsetFeaturesKHR const& requested);
void merge_VkPhysicalDevicePortabilitySubsetFeaturesKHR(VkPhysicalDevicePortabilitySubsetFeaturesKHR & current, VkPhysicalDevicePortabilitySubsetFeaturesKHR const& merge_in);
#endif //defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_KHR_portability_subset))
#if (defined(VK_KHR_shader_clock))
void compare_VkPhysicalDeviceShaderClockFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderClockFeaturesKHR const& supported, VkPhysicalDeviceShaderClockFeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderClockFeaturesKHR(VkPhysicalDeviceShaderClockFeaturesKHR & current, VkPhysicalDeviceShaderClockFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_shader_clock))
#if (defined(VK_KHR_fragment_shading_rate))
void compare_VkPhysicalDeviceFragmentShadingRateFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentShadingRateFeaturesKHR const& supported, VkPhysicalDeviceFragmentShadingRateFeaturesKHR const& requested);
void merge_VkPhysicalDeviceFragmentShadingRateFeaturesKHR(VkPhysicalDeviceFragmentShadingRateFeaturesKHR & current, VkPhysicalDeviceFragmentShadingRateFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_fragment_shading_rate))
#if (defined(VK_KHR_shader_constant_data))
void compare_VkPhysicalDeviceShaderConstantDataFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderConstantDataFeaturesKHR const& supported, VkPhysicalDeviceShaderConstantDataFeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderConstantDataFeaturesKHR(VkPhysicalDeviceShaderConstantDataFeaturesKHR & current, VkPhysicalDeviceShaderConstantDataFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_shader_constant_data))
#if (defined(VK_KHR_shader_abort))
void compare_VkPhysicalDeviceShaderAbortFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderAbortFeaturesKHR const& supported, VkPhysicalDeviceShaderAbortFeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderAbortFeaturesKHR(VkPhysicalDeviceShaderAbortFeaturesKHR & current, VkPhysicalDeviceShaderAbortFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_shader_abort))
#if (defined(VK_KHR_shader_quad_control))
void compare_VkPhysicalDeviceShaderQuadControlFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderQuadControlFeaturesKHR const& supported, VkPhysicalDeviceShaderQuadControlFeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderQuadControlFeaturesKHR(VkPhysicalDeviceShaderQuadControlFeaturesKHR & current, VkPhysicalDeviceShaderQuadControlFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_shader_quad_control))
#if (defined(VK_KHR_present_wait))
void compare_VkPhysicalDevicePresentWaitFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePresentWaitFeaturesKHR const& supported, VkPhysicalDevicePresentWaitFeaturesKHR const& requested);
void merge_VkPhysicalDevicePresentWaitFeaturesKHR(VkPhysicalDevicePresentWaitFeaturesKHR & current, VkPhysicalDevicePresentWaitFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_present_wait))
#if (defined(VK_KHR_pipeline_executable_properties))
void compare_VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR const& supported, VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR const& requested);
void merge_VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR(VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR & current, VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_pipeline_executable_properties))
#if (defined(VK_KHR_present_id))
void compare_VkPhysicalDevicePresentIdFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePresentIdFeaturesKHR const& supported, VkPhysicalDevicePresentIdFeaturesKHR const& requested);
void merge_VkPhysicalDevicePresentIdFeaturesKHR(VkPhysicalDevicePresentIdFeaturesKHR & current, VkPhysicalDevicePresentIdFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_present_id))
#if (defined(VK_KHR_device_address_commands))
void compare_VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR const& supported, VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR const& requested);
void merge_VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR(VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR & current, VkPhysicalDeviceDeviceAddressCommandsFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_device_address_commands))
#if (defined(VK_KHR_fragment_shader_barycentric))
void compare_VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR const& supported, VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR const& requested);
void merge_VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR(VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR & current, VkPhysicalDeviceFragmentShaderBarycentricFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_fragment_shader_barycentric))
#if (defined(VK_NV_fragment_shader_barycentric))
void compare_VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV const& supported, VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV const& requested);
void merge_VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV(VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV & current, VkPhysicalDeviceFragmentShaderBarycentricFeaturesNV const& merge_in);
#endif //(defined(VK_NV_fragment_shader_barycentric))
#if (defined(VK_KHR_shader_subgroup_uniform_control_flow))
void compare_VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR const& supported, VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR(VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR & current, VkPhysicalDeviceShaderSubgroupUniformControlFlowFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_shader_subgroup_uniform_control_flow))
#if (defined(VK_KHR_workgroup_memory_explicit_layout))
void compare_VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR const& supported, VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR const& requested);
void merge_VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR(VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR & current, VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_workgroup_memory_explicit_layout))
#if (defined(VK_KHR_ray_tracing_maintenance1))
void compare_VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR const& supported, VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR const& requested);
void merge_VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR(VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR & current, VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_ray_tracing_maintenance1))
#if (defined(VK_KHR_shader_untyped_pointers))
void compare_VkPhysicalDeviceShaderUntypedPointersFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderUntypedPointersFeaturesKHR const& supported, VkPhysicalDeviceShaderUntypedPointersFeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderUntypedPointersFeaturesKHR(VkPhysicalDeviceShaderUntypedPointersFeaturesKHR & current, VkPhysicalDeviceShaderUntypedPointersFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_shader_untyped_pointers))
#if (defined(VK_KHR_shader_maximal_reconvergence))
void compare_VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR const& supported, VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR(VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR & current, VkPhysicalDeviceShaderMaximalReconvergenceFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_shader_maximal_reconvergence))
#if (defined(VK_KHR_present_id2))
void compare_VkPhysicalDevicePresentId2FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePresentId2FeaturesKHR const& supported, VkPhysicalDevicePresentId2FeaturesKHR const& requested);
void merge_VkPhysicalDevicePresentId2FeaturesKHR(VkPhysicalDevicePresentId2FeaturesKHR & current, VkPhysicalDevicePresentId2FeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_present_id2))
#if (defined(VK_KHR_present_wait2))
void compare_VkPhysicalDevicePresentWait2FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePresentWait2FeaturesKHR const& supported, VkPhysicalDevicePresentWait2FeaturesKHR const& requested);
void merge_VkPhysicalDevicePresentWait2FeaturesKHR(VkPhysicalDevicePresentWait2FeaturesKHR & current, VkPhysicalDevicePresentWait2FeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_present_wait2))
#if (defined(VK_KHR_ray_tracing_position_fetch))
void compare_VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR const& supported, VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR const& requested);
void merge_VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR(VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR & current, VkPhysicalDeviceRayTracingPositionFetchFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_ray_tracing_position_fetch))
#if (defined(VK_KHR_pipeline_binary))
void compare_VkPhysicalDevicePipelineBinaryFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePipelineBinaryFeaturesKHR const& supported, VkPhysicalDevicePipelineBinaryFeaturesKHR const& requested);
void merge_VkPhysicalDevicePipelineBinaryFeaturesKHR(VkPhysicalDevicePipelineBinaryFeaturesKHR & current, VkPhysicalDevicePipelineBinaryFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_pipeline_binary))
#if (defined(VK_KHR_swapchain_maintenance1))
void compare_VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR const& supported, VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR const& requested);
void merge_VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR(VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR & current, VkPhysicalDeviceSwapchainMaintenance1FeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_swapchain_maintenance1))
#if (defined(VK_EXT_swapchain_maintenance1))
void compare_VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT const& supported, VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT const& requested);
void merge_VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT(VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT & current, VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_swapchain_maintenance1))
#if (defined(VK_KHR_internally_synchronized_queues))
void compare_VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR const& supported, VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR const& requested);
void merge_VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR(VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR & current, VkPhysicalDeviceInternallySynchronizedQueuesFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_internally_synchronized_queues))
#if (defined(VK_KHR_cooperative_matrix))
void compare_VkPhysicalDeviceCooperativeMatrixFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceCooperativeMatrixFeaturesKHR const& supported, VkPhysicalDeviceCooperativeMatrixFeaturesKHR const& requested);
void merge_VkPhysicalDeviceCooperativeMatrixFeaturesKHR(VkPhysicalDeviceCooperativeMatrixFeaturesKHR & current, VkPhysicalDeviceCooperativeMatrixFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_cooperative_matrix))
#if (defined(VK_KHR_compute_shader_derivatives))
void compare_VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR const& supported, VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR const& requested);
void merge_VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR(VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR & current, VkPhysicalDeviceComputeShaderDerivativesFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_compute_shader_derivatives))
#if (defined(VK_NV_compute_shader_derivatives))
void compare_VkPhysicalDeviceComputeShaderDerivativesFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceComputeShaderDerivativesFeaturesNV const& supported, VkPhysicalDeviceComputeShaderDerivativesFeaturesNV const& requested);
void merge_VkPhysicalDeviceComputeShaderDerivativesFeaturesNV(VkPhysicalDeviceComputeShaderDerivativesFeaturesNV & current, VkPhysicalDeviceComputeShaderDerivativesFeaturesNV const& merge_in);
#endif //(defined(VK_NV_compute_shader_derivatives))
#if (defined(VK_KHR_video_encode_av1))
void compare_VkPhysicalDeviceVideoEncodeAV1FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVideoEncodeAV1FeaturesKHR const& supported, VkPhysicalDeviceVideoEncodeAV1FeaturesKHR const& requested);
void merge_VkPhysicalDeviceVideoEncodeAV1FeaturesKHR(VkPhysicalDeviceVideoEncodeAV1FeaturesKHR & current, VkPhysicalDeviceVideoEncodeAV1FeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_video_encode_av1))
#if (defined(VK_KHR_video_decode_vp9))
void compare_VkPhysicalDeviceVideoDecodeVP9FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVideoDecodeVP9FeaturesKHR const& supported, VkPhysicalDeviceVideoDecodeVP9FeaturesKHR const& requested);
void merge_VkPhysicalDeviceVideoDecodeVP9FeaturesKHR(VkPhysicalDeviceVideoDecodeVP9FeaturesKHR & current, VkPhysicalDeviceVideoDecodeVP9FeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_video_decode_vp9))
#if (defined(VK_KHR_video_maintenance1))
void compare_VkPhysicalDeviceVideoMaintenance1FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVideoMaintenance1FeaturesKHR const& supported, VkPhysicalDeviceVideoMaintenance1FeaturesKHR const& requested);
void merge_VkPhysicalDeviceVideoMaintenance1FeaturesKHR(VkPhysicalDeviceVideoMaintenance1FeaturesKHR & current, VkPhysicalDeviceVideoMaintenance1FeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_video_maintenance1))
#if (defined(VK_KHR_unified_image_layouts))
void compare_VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR const& supported, VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR const& requested);
void merge_VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR(VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR & current, VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_unified_image_layouts))
#if (defined(VK_KHR_copy_memory_indirect))
void compare_VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR const& supported, VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR const& requested);
void merge_VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR(VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR & current, VkPhysicalDeviceCopyMemoryIndirectFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_copy_memory_indirect))
#if (defined(VK_KHR_video_encode_intra_refresh))
void compare_VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR const& supported, VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR const& requested);
void merge_VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR(VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR & current, VkPhysicalDeviceVideoEncodeIntraRefreshFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_video_encode_intra_refresh))
#if (defined(VK_KHR_video_encode_quantization_map))
void compare_VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR const& supported, VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR const& requested);
void merge_VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR(VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR & current, VkPhysicalDeviceVideoEncodeQuantizationMapFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_video_encode_quantization_map))
#if (defined(VK_KHR_shader_relaxed_extended_instruction))
void compare_VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR const& supported, VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR(VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR & current, VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_shader_relaxed_extended_instruction))
#if (defined(VK_KHR_maintenance7))
void compare_VkPhysicalDeviceMaintenance7FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance7FeaturesKHR const& supported, VkPhysicalDeviceMaintenance7FeaturesKHR const& requested);
void merge_VkPhysicalDeviceMaintenance7FeaturesKHR(VkPhysicalDeviceMaintenance7FeaturesKHR & current, VkPhysicalDeviceMaintenance7FeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_maintenance7))
#if (defined(VK_KHR_device_fault))
void compare_VkPhysicalDeviceFaultFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceFaultFeaturesKHR const& supported, VkPhysicalDeviceFaultFeaturesKHR const& requested);
void merge_VkPhysicalDeviceFaultFeaturesKHR(VkPhysicalDeviceFaultFeaturesKHR & current, VkPhysicalDeviceFaultFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_device_fault))
#if (defined(VK_KHR_maintenance8))
void compare_VkPhysicalDeviceMaintenance8FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance8FeaturesKHR const& supported, VkPhysicalDeviceMaintenance8FeaturesKHR const& requested);
void merge_VkPhysicalDeviceMaintenance8FeaturesKHR(VkPhysicalDeviceMaintenance8FeaturesKHR & current, VkPhysicalDeviceMaintenance8FeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_maintenance8))
#if (defined(VK_KHR_shader_fma))
void compare_VkPhysicalDeviceShaderFmaFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceShaderFmaFeaturesKHR const& supported, VkPhysicalDeviceShaderFmaFeaturesKHR const& requested);
void merge_VkPhysicalDeviceShaderFmaFeaturesKHR(VkPhysicalDeviceShaderFmaFeaturesKHR & current, VkPhysicalDeviceShaderFmaFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_shader_fma))
#if (defined(VK_KHR_maintenance9))
void compare_VkPhysicalDeviceMaintenance9FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance9FeaturesKHR const& supported, VkPhysicalDeviceMaintenance9FeaturesKHR const& requested);
void merge_VkPhysicalDeviceMaintenance9FeaturesKHR(VkPhysicalDeviceMaintenance9FeaturesKHR & current, VkPhysicalDeviceMaintenance9FeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_maintenance9))
#if (defined(VK_KHR_video_maintenance2))
void compare_VkPhysicalDeviceVideoMaintenance2FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVideoMaintenance2FeaturesKHR const& supported, VkPhysicalDeviceVideoMaintenance2FeaturesKHR const& requested);
void merge_VkPhysicalDeviceVideoMaintenance2FeaturesKHR(VkPhysicalDeviceVideoMaintenance2FeaturesKHR & current, VkPhysicalDeviceVideoMaintenance2FeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_video_maintenance2))
#if (defined(VK_KHR_video_encode_feedback2))
void compare_VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR const& supported, VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR const& requested);
void merge_VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR(VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR & current, VkPhysicalDeviceVideoEncodeFeedback2FeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_video_encode_feedback2))
#if (defined(VK_KHR_depth_clamp_zero_one))
void compare_VkPhysicalDeviceDepthClampZeroOneFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceDepthClampZeroOneFeaturesKHR const& supported, VkPhysicalDeviceDepthClampZeroOneFeaturesKHR const& requested);
void merge_VkPhysicalDeviceDepthClampZeroOneFeaturesKHR(VkPhysicalDeviceDepthClampZeroOneFeaturesKHR & current, VkPhysicalDeviceDepthClampZeroOneFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_depth_clamp_zero_one))
#if (defined(VK_EXT_depth_clamp_zero_one))
void compare_VkPhysicalDeviceDepthClampZeroOneFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDepthClampZeroOneFeaturesEXT const& supported, VkPhysicalDeviceDepthClampZeroOneFeaturesEXT const& requested);
void merge_VkPhysicalDeviceDepthClampZeroOneFeaturesEXT(VkPhysicalDeviceDepthClampZeroOneFeaturesEXT & current, VkPhysicalDeviceDepthClampZeroOneFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_depth_clamp_zero_one))
#if (defined(VK_KHR_robustness2))
void compare_VkPhysicalDeviceRobustness2FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceRobustness2FeaturesKHR const& supported, VkPhysicalDeviceRobustness2FeaturesKHR const& requested);
void merge_VkPhysicalDeviceRobustness2FeaturesKHR(VkPhysicalDeviceRobustness2FeaturesKHR & current, VkPhysicalDeviceRobustness2FeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_robustness2))
#if (defined(VK_EXT_robustness2))
void compare_VkPhysicalDeviceRobustness2FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceRobustness2FeaturesEXT const& supported, VkPhysicalDeviceRobustness2FeaturesEXT const& requested);
void merge_VkPhysicalDeviceRobustness2FeaturesEXT(VkPhysicalDeviceRobustness2FeaturesEXT & current, VkPhysicalDeviceRobustness2FeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_robustness2))
#if (defined(VK_KHR_present_mode_fifo_latest_ready))
void compare_VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR const& supported, VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR const& requested);
void merge_VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR(VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR & current, VkPhysicalDevicePresentModeFifoLatestReadyFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_present_mode_fifo_latest_ready))
#if (defined(VK_EXT_present_mode_fifo_latest_ready))
void compare_VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT const& supported, VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT const& requested);
void merge_VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT(VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT & current, VkPhysicalDevicePresentModeFifoLatestReadyFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_present_mode_fifo_latest_ready))
#if (defined(VK_KHR_opacity_micromap))
void compare_VkPhysicalDeviceOpacityMicromapFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceOpacityMicromapFeaturesKHR const& supported, VkPhysicalDeviceOpacityMicromapFeaturesKHR const& requested);
void merge_VkPhysicalDeviceOpacityMicromapFeaturesKHR(VkPhysicalDeviceOpacityMicromapFeaturesKHR & current, VkPhysicalDeviceOpacityMicromapFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_opacity_micromap))
#if (defined(VK_KHR_maintenance10))
void compare_VkPhysicalDeviceMaintenance10FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance10FeaturesKHR const& supported, VkPhysicalDeviceMaintenance10FeaturesKHR const& requested);
void merge_VkPhysicalDeviceMaintenance10FeaturesKHR(VkPhysicalDeviceMaintenance10FeaturesKHR & current, VkPhysicalDeviceMaintenance10FeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_maintenance10))
#if (defined(VK_KHR_maintenance11))
void compare_VkPhysicalDeviceMaintenance11FeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceMaintenance11FeaturesKHR const& supported, VkPhysicalDeviceMaintenance11FeaturesKHR const& requested);
void merge_VkPhysicalDeviceMaintenance11FeaturesKHR(VkPhysicalDeviceMaintenance11FeaturesKHR & current, VkPhysicalDeviceMaintenance11FeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_maintenance11))
#if (defined(VK_KHR_extended_flags))
void compare_VkPhysicalDeviceExtendedFlagsFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceExtendedFlagsFeaturesKHR const& supported, VkPhysicalDeviceExtendedFlagsFeaturesKHR const& requested);
void merge_VkPhysicalDeviceExtendedFlagsFeaturesKHR(VkPhysicalDeviceExtendedFlagsFeaturesKHR & current, VkPhysicalDeviceExtendedFlagsFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_extended_flags))
#if (defined(VK_EXT_transform_feedback))
void compare_VkPhysicalDeviceTransformFeedbackFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceTransformFeedbackFeaturesEXT const& supported, VkPhysicalDeviceTransformFeedbackFeaturesEXT const& requested);
void merge_VkPhysicalDeviceTransformFeedbackFeaturesEXT(VkPhysicalDeviceTransformFeedbackFeaturesEXT & current, VkPhysicalDeviceTransformFeedbackFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_transform_feedback))
#if (defined(VK_NV_corner_sampled_image))
void compare_VkPhysicalDeviceCornerSampledImageFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCornerSampledImageFeaturesNV const& supported, VkPhysicalDeviceCornerSampledImageFeaturesNV const& requested);
void merge_VkPhysicalDeviceCornerSampledImageFeaturesNV(VkPhysicalDeviceCornerSampledImageFeaturesNV & current, VkPhysicalDeviceCornerSampledImageFeaturesNV const& merge_in);
#endif //(defined(VK_NV_corner_sampled_image))
#if (defined(VK_EXT_astc_decode_mode))
void compare_VkPhysicalDeviceASTCDecodeFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceASTCDecodeFeaturesEXT const& supported, VkPhysicalDeviceASTCDecodeFeaturesEXT const& requested);
void merge_VkPhysicalDeviceASTCDecodeFeaturesEXT(VkPhysicalDeviceASTCDecodeFeaturesEXT & current, VkPhysicalDeviceASTCDecodeFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_astc_decode_mode))
#if (defined(VK_EXT_conditional_rendering))
void compare_VkPhysicalDeviceConditionalRenderingFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceConditionalRenderingFeaturesEXT const& supported, VkPhysicalDeviceConditionalRenderingFeaturesEXT const& requested);
void merge_VkPhysicalDeviceConditionalRenderingFeaturesEXT(VkPhysicalDeviceConditionalRenderingFeaturesEXT & current, VkPhysicalDeviceConditionalRenderingFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_conditional_rendering))
#if (defined(VK_EXT_depth_clip_enable))
void compare_VkPhysicalDeviceDepthClipEnableFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDepthClipEnableFeaturesEXT const& supported, VkPhysicalDeviceDepthClipEnableFeaturesEXT const& requested);
void merge_VkPhysicalDeviceDepthClipEnableFeaturesEXT(VkPhysicalDeviceDepthClipEnableFeaturesEXT & current, VkPhysicalDeviceDepthClipEnableFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_depth_clip_enable))
#if (defined(VK_IMG_relaxed_line_rasterization))
void compare_VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG(std::vector<std::string> & error_list, VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG const& supported, VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG const& requested);
void merge_VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG(VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG & current, VkPhysicalDeviceRelaxedLineRasterizationFeaturesIMG const& merge_in);
#endif //(defined(VK_IMG_relaxed_line_rasterization))
#if (defined(VK_AMD_gpa_interface))
void compare_VkPhysicalDeviceGpaFeaturesAMD(std::vector<std::string> & error_list, VkPhysicalDeviceGpaFeaturesAMD const& supported, VkPhysicalDeviceGpaFeaturesAMD const& requested);
void merge_VkPhysicalDeviceGpaFeaturesAMD(VkPhysicalDeviceGpaFeaturesAMD & current, VkPhysicalDeviceGpaFeaturesAMD const& merge_in);
#endif //(defined(VK_AMD_gpa_interface))
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_AMDX_shader_enqueue))
void compare_VkPhysicalDeviceShaderEnqueueFeaturesAMDX(std::vector<std::string> & error_list, VkPhysicalDeviceShaderEnqueueFeaturesAMDX const& supported, VkPhysicalDeviceShaderEnqueueFeaturesAMDX const& requested);
void merge_VkPhysicalDeviceShaderEnqueueFeaturesAMDX(VkPhysicalDeviceShaderEnqueueFeaturesAMDX & current, VkPhysicalDeviceShaderEnqueueFeaturesAMDX const& merge_in);
#endif //defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_AMDX_shader_enqueue))
#if (defined(VK_EXT_descriptor_heap))
void compare_VkPhysicalDeviceDescriptorHeapFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDescriptorHeapFeaturesEXT const& supported, VkPhysicalDeviceDescriptorHeapFeaturesEXT const& requested);
void merge_VkPhysicalDeviceDescriptorHeapFeaturesEXT(VkPhysicalDeviceDescriptorHeapFeaturesEXT & current, VkPhysicalDeviceDescriptorHeapFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_descriptor_heap))
#if (defined(VK_EXT_blend_operation_advanced))
void compare_VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT const& supported, VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT const& requested);
void merge_VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT(VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT & current, VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_blend_operation_advanced))
#if (defined(VK_NV_shader_sm_builtins))
void compare_VkPhysicalDeviceShaderSMBuiltinsFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceShaderSMBuiltinsFeaturesNV const& supported, VkPhysicalDeviceShaderSMBuiltinsFeaturesNV const& requested);
void merge_VkPhysicalDeviceShaderSMBuiltinsFeaturesNV(VkPhysicalDeviceShaderSMBuiltinsFeaturesNV & current, VkPhysicalDeviceShaderSMBuiltinsFeaturesNV const& merge_in);
#endif //(defined(VK_NV_shader_sm_builtins))
#if (defined(VK_NV_shading_rate_image))
void compare_VkPhysicalDeviceShadingRateImageFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceShadingRateImageFeaturesNV const& supported, VkPhysicalDeviceShadingRateImageFeaturesNV const& requested);
void merge_VkPhysicalDeviceShadingRateImageFeaturesNV(VkPhysicalDeviceShadingRateImageFeaturesNV & current, VkPhysicalDeviceShadingRateImageFeaturesNV const& merge_in);
#endif //(defined(VK_NV_shading_rate_image))
#if (defined(VK_NV_representative_fragment_test))
void compare_VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV const& supported, VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV const& requested);
void merge_VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV(VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV & current, VkPhysicalDeviceRepresentativeFragmentTestFeaturesNV const& merge_in);
#endif //(defined(VK_NV_representative_fragment_test))
#if (defined(VK_QCOM_cooperative_matrix_conversion))
void compare_VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM const& supported, VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM const& requested);
void merge_VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM(VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM & current, VkPhysicalDeviceCooperativeMatrixConversionFeaturesQCOM const& merge_in);
#endif //(defined(VK_QCOM_cooperative_matrix_conversion))
#if (defined(VK_QCOM_elapsed_timer_query))
void compare_VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM const& supported, VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM const& requested);
void merge_VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM(VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM & current, VkPhysicalDeviceElapsedTimerQueryFeaturesQCOM const& merge_in);
#endif //(defined(VK_QCOM_elapsed_timer_query))
#if (defined(VK_NV_mesh_shader))
void compare_VkPhysicalDeviceMeshShaderFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceMeshShaderFeaturesNV const& supported, VkPhysicalDeviceMeshShaderFeaturesNV const& requested);
void merge_VkPhysicalDeviceMeshShaderFeaturesNV(VkPhysicalDeviceMeshShaderFeaturesNV & current, VkPhysicalDeviceMeshShaderFeaturesNV const& merge_in);
#endif //(defined(VK_NV_mesh_shader))
#if (defined(VK_NV_shader_image_footprint))
void compare_VkPhysicalDeviceShaderImageFootprintFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceShaderImageFootprintFeaturesNV const& supported, VkPhysicalDeviceShaderImageFootprintFeaturesNV const& requested);
void merge_VkPhysicalDeviceShaderImageFootprintFeaturesNV(VkPhysicalDeviceShaderImageFootprintFeaturesNV & current, VkPhysicalDeviceShaderImageFootprintFeaturesNV const& merge_in);
#endif //(defined(VK_NV_shader_image_footprint))
#if (defined(VK_NV_scissor_exclusive))
void compare_VkPhysicalDeviceExclusiveScissorFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceExclusiveScissorFeaturesNV const& supported, VkPhysicalDeviceExclusiveScissorFeaturesNV const& requested);
void merge_VkPhysicalDeviceExclusiveScissorFeaturesNV(VkPhysicalDeviceExclusiveScissorFeaturesNV & current, VkPhysicalDeviceExclusiveScissorFeaturesNV const& merge_in);
#endif //(defined(VK_NV_scissor_exclusive))
#if (defined(VK_EXT_present_timing))
void compare_VkPhysicalDevicePresentTimingFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePresentTimingFeaturesEXT const& supported, VkPhysicalDevicePresentTimingFeaturesEXT const& requested);
void merge_VkPhysicalDevicePresentTimingFeaturesEXT(VkPhysicalDevicePresentTimingFeaturesEXT & current, VkPhysicalDevicePresentTimingFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_present_timing))
#if (defined(VK_INTEL_shader_integer_functions2))
void compare_VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL(std::vector<std::string> & error_list, VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL const& supported, VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL const& requested);
void merge_VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL(VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL & current, VkPhysicalDeviceShaderIntegerFunctions2FeaturesINTEL const& merge_in);
#endif //(defined(VK_INTEL_shader_integer_functions2))
#if (defined(VK_EXT_fragment_density_map))
void compare_VkPhysicalDeviceFragmentDensityMapFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentDensityMapFeaturesEXT const& supported, VkPhysicalDeviceFragmentDensityMapFeaturesEXT const& requested);
void merge_VkPhysicalDeviceFragmentDensityMapFeaturesEXT(VkPhysicalDeviceFragmentDensityMapFeaturesEXT & current, VkPhysicalDeviceFragmentDensityMapFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_fragment_density_map))
#if (defined(VK_AMD_device_coherent_memory))
void compare_VkPhysicalDeviceCoherentMemoryFeaturesAMD(std::vector<std::string> & error_list, VkPhysicalDeviceCoherentMemoryFeaturesAMD const& supported, VkPhysicalDeviceCoherentMemoryFeaturesAMD const& requested);
void merge_VkPhysicalDeviceCoherentMemoryFeaturesAMD(VkPhysicalDeviceCoherentMemoryFeaturesAMD & current, VkPhysicalDeviceCoherentMemoryFeaturesAMD const& merge_in);
#endif //(defined(VK_AMD_device_coherent_memory))
#if (defined(VK_EXT_shader_image_atomic_int64))
void compare_VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT const& supported, VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT const& requested);
void merge_VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT(VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT & current, VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_shader_image_atomic_int64))
#if (defined(VK_EXT_memory_priority))
void compare_VkPhysicalDeviceMemoryPriorityFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceMemoryPriorityFeaturesEXT const& supported, VkPhysicalDeviceMemoryPriorityFeaturesEXT const& requested);
void merge_VkPhysicalDeviceMemoryPriorityFeaturesEXT(VkPhysicalDeviceMemoryPriorityFeaturesEXT & current, VkPhysicalDeviceMemoryPriorityFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_memory_priority))
#if (defined(VK_NV_dedicated_allocation_image_aliasing))
void compare_VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV const& supported, VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV const& requested);
void merge_VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV(VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV & current, VkPhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV const& merge_in);
#endif //(defined(VK_NV_dedicated_allocation_image_aliasing))
#if (defined(VK_EXT_buffer_device_address))
void compare_VkPhysicalDeviceBufferDeviceAddressFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceBufferDeviceAddressFeaturesEXT const& supported, VkPhysicalDeviceBufferDeviceAddressFeaturesEXT const& requested);
void merge_VkPhysicalDeviceBufferDeviceAddressFeaturesEXT(VkPhysicalDeviceBufferDeviceAddressFeaturesEXT & current, VkPhysicalDeviceBufferDeviceAddressFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_buffer_device_address))
#if (defined(VK_EXT_buffer_device_address))
void compare_VkPhysicalDeviceBufferAddressFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceBufferAddressFeaturesEXT const& supported, VkPhysicalDeviceBufferAddressFeaturesEXT const& requested);
void merge_VkPhysicalDeviceBufferAddressFeaturesEXT(VkPhysicalDeviceBufferAddressFeaturesEXT & current, VkPhysicalDeviceBufferAddressFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_buffer_device_address))
#if (defined(VK_NV_cooperative_matrix))
void compare_VkPhysicalDeviceCooperativeMatrixFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCooperativeMatrixFeaturesNV const& supported, VkPhysicalDeviceCooperativeMatrixFeaturesNV const& requested);
void merge_VkPhysicalDeviceCooperativeMatrixFeaturesNV(VkPhysicalDeviceCooperativeMatrixFeaturesNV & current, VkPhysicalDeviceCooperativeMatrixFeaturesNV const& merge_in);
#endif //(defined(VK_NV_cooperative_matrix))
#if (defined(VK_NV_coverage_reduction_mode))
void compare_VkPhysicalDeviceCoverageReductionModeFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCoverageReductionModeFeaturesNV const& supported, VkPhysicalDeviceCoverageReductionModeFeaturesNV const& requested);
void merge_VkPhysicalDeviceCoverageReductionModeFeaturesNV(VkPhysicalDeviceCoverageReductionModeFeaturesNV & current, VkPhysicalDeviceCoverageReductionModeFeaturesNV const& merge_in);
#endif //(defined(VK_NV_coverage_reduction_mode))
#if (defined(VK_EXT_fragment_shader_interlock))
void compare_VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT const& supported, VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT const& requested);
void merge_VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT(VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT & current, VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_fragment_shader_interlock))
#if (defined(VK_EXT_ycbcr_image_arrays))
void compare_VkPhysicalDeviceYcbcrImageArraysFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceYcbcrImageArraysFeaturesEXT const& supported, VkPhysicalDeviceYcbcrImageArraysFeaturesEXT const& requested);
void merge_VkPhysicalDeviceYcbcrImageArraysFeaturesEXT(VkPhysicalDeviceYcbcrImageArraysFeaturesEXT & current, VkPhysicalDeviceYcbcrImageArraysFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_ycbcr_image_arrays))
#if (defined(VK_EXT_provoking_vertex))
void compare_VkPhysicalDeviceProvokingVertexFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceProvokingVertexFeaturesEXT const& supported, VkPhysicalDeviceProvokingVertexFeaturesEXT const& requested);
void merge_VkPhysicalDeviceProvokingVertexFeaturesEXT(VkPhysicalDeviceProvokingVertexFeaturesEXT & current, VkPhysicalDeviceProvokingVertexFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_provoking_vertex))
#if (defined(VK_EXT_shader_atomic_float))
void compare_VkPhysicalDeviceShaderAtomicFloatFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderAtomicFloatFeaturesEXT const& supported, VkPhysicalDeviceShaderAtomicFloatFeaturesEXT const& requested);
void merge_VkPhysicalDeviceShaderAtomicFloatFeaturesEXT(VkPhysicalDeviceShaderAtomicFloatFeaturesEXT & current, VkPhysicalDeviceShaderAtomicFloatFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_shader_atomic_float))
#if (defined(VK_EXT_extended_dynamic_state))
void compare_VkPhysicalDeviceExtendedDynamicStateFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceExtendedDynamicStateFeaturesEXT const& supported, VkPhysicalDeviceExtendedDynamicStateFeaturesEXT const& requested);
void merge_VkPhysicalDeviceExtendedDynamicStateFeaturesEXT(VkPhysicalDeviceExtendedDynamicStateFeaturesEXT & current, VkPhysicalDeviceExtendedDynamicStateFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_extended_dynamic_state))
#if (defined(VK_EXT_map_memory_placed))
void compare_VkPhysicalDeviceMapMemoryPlacedFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceMapMemoryPlacedFeaturesEXT const& supported, VkPhysicalDeviceMapMemoryPlacedFeaturesEXT const& requested);
void merge_VkPhysicalDeviceMapMemoryPlacedFeaturesEXT(VkPhysicalDeviceMapMemoryPlacedFeaturesEXT & current, VkPhysicalDeviceMapMemoryPlacedFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_map_memory_placed))
#if (defined(VK_EXT_shader_atomic_float2))
void compare_VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT const& supported, VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT const& requested);
void merge_VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT(VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT & current, VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_shader_atomic_float2))
#if (defined(VK_NV_device_generated_commands))
void compare_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV const& supported, VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV const& requested);
void merge_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV(VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV & current, VkPhysicalDeviceDeviceGeneratedCommandsFeaturesNV const& merge_in);
#endif //(defined(VK_NV_device_generated_commands))
#if (defined(VK_NV_inherited_viewport_scissor))
void compare_VkPhysicalDeviceInheritedViewportScissorFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceInheritedViewportScissorFeaturesNV const& supported, VkPhysicalDeviceInheritedViewportScissorFeaturesNV const& requested);
void merge_VkPhysicalDeviceInheritedViewportScissorFeaturesNV(VkPhysicalDeviceInheritedViewportScissorFeaturesNV & current, VkPhysicalDeviceInheritedViewportScissorFeaturesNV const& merge_in);
#endif //(defined(VK_NV_inherited_viewport_scissor))
#if (defined(VK_EXT_texel_buffer_alignment))
void compare_VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT const& supported, VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT const& requested);
void merge_VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT(VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT & current, VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_texel_buffer_alignment))
#if (defined(VK_EXT_depth_bias_control))
void compare_VkPhysicalDeviceDepthBiasControlFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDepthBiasControlFeaturesEXT const& supported, VkPhysicalDeviceDepthBiasControlFeaturesEXT const& requested);
void merge_VkPhysicalDeviceDepthBiasControlFeaturesEXT(VkPhysicalDeviceDepthBiasControlFeaturesEXT & current, VkPhysicalDeviceDepthBiasControlFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_depth_bias_control))
#if (defined(VK_EXT_device_memory_report))
void compare_VkPhysicalDeviceDeviceMemoryReportFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDeviceMemoryReportFeaturesEXT const& supported, VkPhysicalDeviceDeviceMemoryReportFeaturesEXT const& requested);
void merge_VkPhysicalDeviceDeviceMemoryReportFeaturesEXT(VkPhysicalDeviceDeviceMemoryReportFeaturesEXT & current, VkPhysicalDeviceDeviceMemoryReportFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_device_memory_report))
#if (defined(VK_EXT_custom_border_color))
void compare_VkPhysicalDeviceCustomBorderColorFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceCustomBorderColorFeaturesEXT const& supported, VkPhysicalDeviceCustomBorderColorFeaturesEXT const& requested);
void merge_VkPhysicalDeviceCustomBorderColorFeaturesEXT(VkPhysicalDeviceCustomBorderColorFeaturesEXT & current, VkPhysicalDeviceCustomBorderColorFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_custom_border_color))
#if (defined(VK_EXT_texture_compression_astc_3d))
void compare_VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT const& supported, VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT const& requested);
void merge_VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT(VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT & current, VkPhysicalDeviceTextureCompressionASTC3DFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_texture_compression_astc_3d))
#if (defined(VK_NV_present_barrier))
void compare_VkPhysicalDevicePresentBarrierFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDevicePresentBarrierFeaturesNV const& supported, VkPhysicalDevicePresentBarrierFeaturesNV const& requested);
void merge_VkPhysicalDevicePresentBarrierFeaturesNV(VkPhysicalDevicePresentBarrierFeaturesNV & current, VkPhysicalDevicePresentBarrierFeaturesNV const& merge_in);
#endif //(defined(VK_NV_present_barrier))
#if (defined(VK_NV_device_diagnostics_config))
void compare_VkPhysicalDeviceDiagnosticsConfigFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceDiagnosticsConfigFeaturesNV const& supported, VkPhysicalDeviceDiagnosticsConfigFeaturesNV const& requested);
void merge_VkPhysicalDeviceDiagnosticsConfigFeaturesNV(VkPhysicalDeviceDiagnosticsConfigFeaturesNV & current, VkPhysicalDeviceDiagnosticsConfigFeaturesNV const& merge_in);
#endif //(defined(VK_NV_device_diagnostics_config))
#if (defined(VK_QCOM_queue_perf_hint))
void compare_VkPhysicalDeviceQueuePerfHintFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceQueuePerfHintFeaturesQCOM const& supported, VkPhysicalDeviceQueuePerfHintFeaturesQCOM const& requested);
void merge_VkPhysicalDeviceQueuePerfHintFeaturesQCOM(VkPhysicalDeviceQueuePerfHintFeaturesQCOM & current, VkPhysicalDeviceQueuePerfHintFeaturesQCOM const& merge_in);
#endif //(defined(VK_QCOM_queue_perf_hint))
#if (defined(VK_QCOM_image_processing3))
void compare_VkPhysicalDeviceImageProcessing3FeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceImageProcessing3FeaturesQCOM const& supported, VkPhysicalDeviceImageProcessing3FeaturesQCOM const& requested);
void merge_VkPhysicalDeviceImageProcessing3FeaturesQCOM(VkPhysicalDeviceImageProcessing3FeaturesQCOM & current, VkPhysicalDeviceImageProcessing3FeaturesQCOM const& merge_in);
#endif //(defined(VK_QCOM_image_processing3))
#if (defined(VK_QCOM_shader_multiple_wait_queues))
void compare_VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM const& supported, VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM const& requested);
void merge_VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM(VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM & current, VkPhysicalDeviceShaderMultipleWaitQueuesFeaturesQCOM const& merge_in);
#endif //(defined(VK_QCOM_shader_multiple_wait_queues))
#if (defined(VK_EXT_shader_split_barrier))
void compare_VkPhysicalDeviceShaderSplitBarrierFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderSplitBarrierFeaturesEXT const& supported, VkPhysicalDeviceShaderSplitBarrierFeaturesEXT const& requested);
void merge_VkPhysicalDeviceShaderSplitBarrierFeaturesEXT(VkPhysicalDeviceShaderSplitBarrierFeaturesEXT & current, VkPhysicalDeviceShaderSplitBarrierFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_shader_split_barrier))
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_NV_cuda_kernel_launch))
void compare_VkPhysicalDeviceCudaKernelLaunchFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCudaKernelLaunchFeaturesNV const& supported, VkPhysicalDeviceCudaKernelLaunchFeaturesNV const& requested);
void merge_VkPhysicalDeviceCudaKernelLaunchFeaturesNV(VkPhysicalDeviceCudaKernelLaunchFeaturesNV & current, VkPhysicalDeviceCudaKernelLaunchFeaturesNV const& merge_in);
#endif //defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_NV_cuda_kernel_launch))
#if (defined(VK_QCOM_tile_shading))
void compare_VkPhysicalDeviceTileShadingFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceTileShadingFeaturesQCOM const& supported, VkPhysicalDeviceTileShadingFeaturesQCOM const& requested);
void merge_VkPhysicalDeviceTileShadingFeaturesQCOM(VkPhysicalDeviceTileShadingFeaturesQCOM & current, VkPhysicalDeviceTileShadingFeaturesQCOM const& merge_in);
#endif //(defined(VK_QCOM_tile_shading))
#if (defined(VK_EXT_descriptor_buffer))
void compare_VkPhysicalDeviceDescriptorBufferFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDescriptorBufferFeaturesEXT const& supported, VkPhysicalDeviceDescriptorBufferFeaturesEXT const& requested);
void merge_VkPhysicalDeviceDescriptorBufferFeaturesEXT(VkPhysicalDeviceDescriptorBufferFeaturesEXT & current, VkPhysicalDeviceDescriptorBufferFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_descriptor_buffer))
#if (defined(VK_EXT_graphics_pipeline_library))
void compare_VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT const& supported, VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT const& requested);
void merge_VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT(VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT & current, VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_graphics_pipeline_library))
#if (defined(VK_AMD_shader_early_and_late_fragment_tests))
void compare_VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD(std::vector<std::string> & error_list, VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD const& supported, VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD const& requested);
void merge_VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD(VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD & current, VkPhysicalDeviceShaderEarlyAndLateFragmentTestsFeaturesAMD const& merge_in);
#endif //(defined(VK_AMD_shader_early_and_late_fragment_tests))
#if (defined(VK_NV_fragment_shading_rate_enums))
void compare_VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV const& supported, VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV const& requested);
void merge_VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV(VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV & current, VkPhysicalDeviceFragmentShadingRateEnumsFeaturesNV const& merge_in);
#endif //(defined(VK_NV_fragment_shading_rate_enums))
#if (defined(VK_NV_ray_tracing_motion_blur))
void compare_VkPhysicalDeviceRayTracingMotionBlurFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceRayTracingMotionBlurFeaturesNV const& supported, VkPhysicalDeviceRayTracingMotionBlurFeaturesNV const& requested);
void merge_VkPhysicalDeviceRayTracingMotionBlurFeaturesNV(VkPhysicalDeviceRayTracingMotionBlurFeaturesNV & current, VkPhysicalDeviceRayTracingMotionBlurFeaturesNV const& merge_in);
#endif //(defined(VK_NV_ray_tracing_motion_blur))
#if (defined(VK_EXT_ycbcr_2plane_444_formats))
void compare_VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT const& supported, VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT const& requested);
void merge_VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT(VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT & current, VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_ycbcr_2plane_444_formats))
#if (defined(VK_EXT_fragment_density_map2))
void compare_VkPhysicalDeviceFragmentDensityMap2FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentDensityMap2FeaturesEXT const& supported, VkPhysicalDeviceFragmentDensityMap2FeaturesEXT const& requested);
void merge_VkPhysicalDeviceFragmentDensityMap2FeaturesEXT(VkPhysicalDeviceFragmentDensityMap2FeaturesEXT & current, VkPhysicalDeviceFragmentDensityMap2FeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_fragment_density_map2))
#if (defined(VK_EXT_image_compression_control))
void compare_VkPhysicalDeviceImageCompressionControlFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceImageCompressionControlFeaturesEXT const& supported, VkPhysicalDeviceImageCompressionControlFeaturesEXT const& requested);
void merge_VkPhysicalDeviceImageCompressionControlFeaturesEXT(VkPhysicalDeviceImageCompressionControlFeaturesEXT & current, VkPhysicalDeviceImageCompressionControlFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_image_compression_control))
#if (defined(VK_EXT_attachment_feedback_loop_layout))
void compare_VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT const& supported, VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT const& requested);
void merge_VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT(VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT & current, VkPhysicalDeviceAttachmentFeedbackLoopLayoutFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_attachment_feedback_loop_layout))
#if (defined(VK_EXT_4444_formats))
void compare_VkPhysicalDevice4444FormatsFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevice4444FormatsFeaturesEXT const& supported, VkPhysicalDevice4444FormatsFeaturesEXT const& requested);
void merge_VkPhysicalDevice4444FormatsFeaturesEXT(VkPhysicalDevice4444FormatsFeaturesEXT & current, VkPhysicalDevice4444FormatsFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_4444_formats))
#if (defined(VK_EXT_device_fault))
void compare_VkPhysicalDeviceFaultFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceFaultFeaturesEXT const& supported, VkPhysicalDeviceFaultFeaturesEXT const& requested);
void merge_VkPhysicalDeviceFaultFeaturesEXT(VkPhysicalDeviceFaultFeaturesEXT & current, VkPhysicalDeviceFaultFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_device_fault))
#if (defined(VK_EXT_rasterization_order_attachment_access))
void compare_VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT const& supported, VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT const& requested);
void merge_VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT(VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT & current, VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_rasterization_order_attachment_access))
#if (defined(VK_ARM_rasterization_order_attachment_access))
void compare_VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM const& supported, VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM const& requested);
void merge_VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM(VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM & current, VkPhysicalDeviceRasterizationOrderAttachmentAccessFeaturesARM const& merge_in);
#endif //(defined(VK_ARM_rasterization_order_attachment_access))
#if (defined(VK_EXT_rgba10x6_formats))
void compare_VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT const& supported, VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT const& requested);
void merge_VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT(VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT & current, VkPhysicalDeviceRGBA10X6FormatsFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_rgba10x6_formats))
#if (defined(VK_EXT_mutable_descriptor_type))
void compare_VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT const& supported, VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT const& requested);
void merge_VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT(VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT & current, VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_mutable_descriptor_type))
#if (defined(VK_VALVE_mutable_descriptor_type))
void compare_VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE(std::vector<std::string> & error_list, VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE const& supported, VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE const& requested);
void merge_VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE(VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE & current, VkPhysicalDeviceMutableDescriptorTypeFeaturesVALVE const& merge_in);
#endif //(defined(VK_VALVE_mutable_descriptor_type))
#if (defined(VK_EXT_vertex_input_dynamic_state))
void compare_VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT const& supported, VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT const& requested);
void merge_VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT(VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT & current, VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_vertex_input_dynamic_state))
#if (defined(VK_EXT_device_address_binding_report))
void compare_VkPhysicalDeviceAddressBindingReportFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceAddressBindingReportFeaturesEXT const& supported, VkPhysicalDeviceAddressBindingReportFeaturesEXT const& requested);
void merge_VkPhysicalDeviceAddressBindingReportFeaturesEXT(VkPhysicalDeviceAddressBindingReportFeaturesEXT & current, VkPhysicalDeviceAddressBindingReportFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_device_address_binding_report))
#if (defined(VK_EXT_depth_clip_control))
void compare_VkPhysicalDeviceDepthClipControlFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDepthClipControlFeaturesEXT const& supported, VkPhysicalDeviceDepthClipControlFeaturesEXT const& requested);
void merge_VkPhysicalDeviceDepthClipControlFeaturesEXT(VkPhysicalDeviceDepthClipControlFeaturesEXT & current, VkPhysicalDeviceDepthClipControlFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_depth_clip_control))
#if (defined(VK_EXT_primitive_topology_list_restart))
void compare_VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT const& supported, VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT const& requested);
void merge_VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT(VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT & current, VkPhysicalDevicePrimitiveTopologyListRestartFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_primitive_topology_list_restart))
#if (defined(VK_HUAWEI_subpass_shading))
void compare_VkPhysicalDeviceSubpassShadingFeaturesHUAWEI(std::vector<std::string> & error_list, VkPhysicalDeviceSubpassShadingFeaturesHUAWEI const& supported, VkPhysicalDeviceSubpassShadingFeaturesHUAWEI const& requested);
void merge_VkPhysicalDeviceSubpassShadingFeaturesHUAWEI(VkPhysicalDeviceSubpassShadingFeaturesHUAWEI & current, VkPhysicalDeviceSubpassShadingFeaturesHUAWEI const& merge_in);
#endif //(defined(VK_HUAWEI_subpass_shading))
#if (defined(VK_HUAWEI_invocation_mask))
void compare_VkPhysicalDeviceInvocationMaskFeaturesHUAWEI(std::vector<std::string> & error_list, VkPhysicalDeviceInvocationMaskFeaturesHUAWEI const& supported, VkPhysicalDeviceInvocationMaskFeaturesHUAWEI const& requested);
void merge_VkPhysicalDeviceInvocationMaskFeaturesHUAWEI(VkPhysicalDeviceInvocationMaskFeaturesHUAWEI & current, VkPhysicalDeviceInvocationMaskFeaturesHUAWEI const& merge_in);
#endif //(defined(VK_HUAWEI_invocation_mask))
#if (defined(VK_NV_external_memory_rdma))
void compare_VkPhysicalDeviceExternalMemoryRDMAFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceExternalMemoryRDMAFeaturesNV const& supported, VkPhysicalDeviceExternalMemoryRDMAFeaturesNV const& requested);
void merge_VkPhysicalDeviceExternalMemoryRDMAFeaturesNV(VkPhysicalDeviceExternalMemoryRDMAFeaturesNV & current, VkPhysicalDeviceExternalMemoryRDMAFeaturesNV const& merge_in);
#endif //(defined(VK_NV_external_memory_rdma))
#if (defined(VK_EXT_pipeline_properties))
void compare_VkPhysicalDevicePipelinePropertiesFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePipelinePropertiesFeaturesEXT const& supported, VkPhysicalDevicePipelinePropertiesFeaturesEXT const& requested);
void merge_VkPhysicalDevicePipelinePropertiesFeaturesEXT(VkPhysicalDevicePipelinePropertiesFeaturesEXT & current, VkPhysicalDevicePipelinePropertiesFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_pipeline_properties))
#if (defined(VK_EXT_frame_boundary))
void compare_VkPhysicalDeviceFrameBoundaryFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceFrameBoundaryFeaturesEXT const& supported, VkPhysicalDeviceFrameBoundaryFeaturesEXT const& requested);
void merge_VkPhysicalDeviceFrameBoundaryFeaturesEXT(VkPhysicalDeviceFrameBoundaryFeaturesEXT & current, VkPhysicalDeviceFrameBoundaryFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_frame_boundary))
#if (defined(VK_EXT_multisampled_render_to_single_sampled))
void compare_VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT const& supported, VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT const& requested);
void merge_VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT(VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT & current, VkPhysicalDeviceMultisampledRenderToSingleSampledFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_multisampled_render_to_single_sampled))
#if (defined(VK_EXT_extended_dynamic_state2))
void compare_VkPhysicalDeviceExtendedDynamicState2FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceExtendedDynamicState2FeaturesEXT const& supported, VkPhysicalDeviceExtendedDynamicState2FeaturesEXT const& requested);
void merge_VkPhysicalDeviceExtendedDynamicState2FeaturesEXT(VkPhysicalDeviceExtendedDynamicState2FeaturesEXT & current, VkPhysicalDeviceExtendedDynamicState2FeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_extended_dynamic_state2))
#if (defined(VK_EXT_color_write_enable))
void compare_VkPhysicalDeviceColorWriteEnableFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceColorWriteEnableFeaturesEXT const& supported, VkPhysicalDeviceColorWriteEnableFeaturesEXT const& requested);
void merge_VkPhysicalDeviceColorWriteEnableFeaturesEXT(VkPhysicalDeviceColorWriteEnableFeaturesEXT & current, VkPhysicalDeviceColorWriteEnableFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_color_write_enable))
#if (defined(VK_EXT_primitives_generated_query))
void compare_VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT const& supported, VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT const& requested);
void merge_VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT(VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT & current, VkPhysicalDevicePrimitivesGeneratedQueryFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_primitives_generated_query))
#if (defined(VK_VALVE_video_encode_rgb_conversion))
void compare_VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE(std::vector<std::string> & error_list, VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE const& supported, VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE const& requested);
void merge_VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE(VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE & current, VkPhysicalDeviceVideoEncodeRgbConversionFeaturesVALVE const& merge_in);
#endif //(defined(VK_VALVE_video_encode_rgb_conversion))
#if (defined(VK_EXT_image_view_min_lod))
void compare_VkPhysicalDeviceImageViewMinLodFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceImageViewMinLodFeaturesEXT const& supported, VkPhysicalDeviceImageViewMinLodFeaturesEXT const& requested);
void merge_VkPhysicalDeviceImageViewMinLodFeaturesEXT(VkPhysicalDeviceImageViewMinLodFeaturesEXT & current, VkPhysicalDeviceImageViewMinLodFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_image_view_min_lod))
#if (defined(VK_EXT_multi_draw))
void compare_VkPhysicalDeviceMultiDrawFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceMultiDrawFeaturesEXT const& supported, VkPhysicalDeviceMultiDrawFeaturesEXT const& requested);
void merge_VkPhysicalDeviceMultiDrawFeaturesEXT(VkPhysicalDeviceMultiDrawFeaturesEXT & current, VkPhysicalDeviceMultiDrawFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_multi_draw))
#if (defined(VK_EXT_image_2d_view_of_3d))
void compare_VkPhysicalDeviceImage2DViewOf3DFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceImage2DViewOf3DFeaturesEXT const& supported, VkPhysicalDeviceImage2DViewOf3DFeaturesEXT const& requested);
void merge_VkPhysicalDeviceImage2DViewOf3DFeaturesEXT(VkPhysicalDeviceImage2DViewOf3DFeaturesEXT & current, VkPhysicalDeviceImage2DViewOf3DFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_image_2d_view_of_3d))
#if (defined(VK_EXT_shader_tile_image))
void compare_VkPhysicalDeviceShaderTileImageFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderTileImageFeaturesEXT const& supported, VkPhysicalDeviceShaderTileImageFeaturesEXT const& requested);
void merge_VkPhysicalDeviceShaderTileImageFeaturesEXT(VkPhysicalDeviceShaderTileImageFeaturesEXT & current, VkPhysicalDeviceShaderTileImageFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_shader_tile_image))
#if (defined(VK_EXT_opacity_micromap))
void compare_VkPhysicalDeviceOpacityMicromapFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceOpacityMicromapFeaturesEXT const& supported, VkPhysicalDeviceOpacityMicromapFeaturesEXT const& requested);
void merge_VkPhysicalDeviceOpacityMicromapFeaturesEXT(VkPhysicalDeviceOpacityMicromapFeaturesEXT & current, VkPhysicalDeviceOpacityMicromapFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_opacity_micromap))
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_NV_displacement_micromap))
void compare_VkPhysicalDeviceDisplacementMicromapFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceDisplacementMicromapFeaturesNV const& supported, VkPhysicalDeviceDisplacementMicromapFeaturesNV const& requested);
void merge_VkPhysicalDeviceDisplacementMicromapFeaturesNV(VkPhysicalDeviceDisplacementMicromapFeaturesNV & current, VkPhysicalDeviceDisplacementMicromapFeaturesNV const& merge_in);
#endif //defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_NV_displacement_micromap))
#if (defined(VK_HUAWEI_cluster_culling_shader))
void compare_VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI(std::vector<std::string> & error_list, VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI const& supported, VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI const& requested);
void merge_VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI(VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI & current, VkPhysicalDeviceClusterCullingShaderFeaturesHUAWEI const& merge_in);
#endif //(defined(VK_HUAWEI_cluster_culling_shader))
#if (defined(VK_EXT_border_color_swizzle))
void compare_VkPhysicalDeviceBorderColorSwizzleFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceBorderColorSwizzleFeaturesEXT const& supported, VkPhysicalDeviceBorderColorSwizzleFeaturesEXT const& requested);
void merge_VkPhysicalDeviceBorderColorSwizzleFeaturesEXT(VkPhysicalDeviceBorderColorSwizzleFeaturesEXT & current, VkPhysicalDeviceBorderColorSwizzleFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_border_color_swizzle))
#if (defined(VK_EXT_pageable_device_local_memory))
void compare_VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT const& supported, VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT const& requested);
void merge_VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT(VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT & current, VkPhysicalDevicePageableDeviceLocalMemoryFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_pageable_device_local_memory))
#if (defined(VK_ARM_scheduling_controls))
void compare_VkPhysicalDeviceSchedulingControlsFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceSchedulingControlsFeaturesARM const& supported, VkPhysicalDeviceSchedulingControlsFeaturesARM const& requested);
void merge_VkPhysicalDeviceSchedulingControlsFeaturesARM(VkPhysicalDeviceSchedulingControlsFeaturesARM & current, VkPhysicalDeviceSchedulingControlsFeaturesARM const& merge_in);
#endif //(defined(VK_ARM_scheduling_controls))
#if (defined(VK_EXT_image_sliced_view_of_3d))
void compare_VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT const& supported, VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT const& requested);
void merge_VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT(VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT & current, VkPhysicalDeviceImageSlicedViewOf3DFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_image_sliced_view_of_3d))
#if (defined(VK_VALVE_descriptor_set_host_mapping))
void compare_VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE(std::vector<std::string> & error_list, VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE const& supported, VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE const& requested);
void merge_VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE(VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE & current, VkPhysicalDeviceDescriptorSetHostMappingFeaturesVALVE const& merge_in);
#endif //(defined(VK_VALVE_descriptor_set_host_mapping))
#if (defined(VK_EXT_non_seamless_cube_map))
void compare_VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT const& supported, VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT const& requested);
void merge_VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT(VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT & current, VkPhysicalDeviceNonSeamlessCubeMapFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_non_seamless_cube_map))
#if (defined(VK_ARM_render_pass_striped))
void compare_VkPhysicalDeviceRenderPassStripedFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceRenderPassStripedFeaturesARM const& supported, VkPhysicalDeviceRenderPassStripedFeaturesARM const& requested);
void merge_VkPhysicalDeviceRenderPassStripedFeaturesARM(VkPhysicalDeviceRenderPassStripedFeaturesARM & current, VkPhysicalDeviceRenderPassStripedFeaturesARM const& merge_in);
#endif //(defined(VK_ARM_render_pass_striped))
#if (defined(VK_EXT_fragment_density_map_offset))
void compare_VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT const& supported, VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT const& requested);
void merge_VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT(VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT & current, VkPhysicalDeviceFragmentDensityMapOffsetFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_fragment_density_map_offset))
#if (defined(VK_QCOM_fragment_density_map_offset))
void compare_VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM const& supported, VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM const& requested);
void merge_VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM(VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM & current, VkPhysicalDeviceFragmentDensityMapOffsetFeaturesQCOM const& merge_in);
#endif //(defined(VK_QCOM_fragment_density_map_offset))
#if (defined(VK_NV_copy_memory_indirect))
void compare_VkPhysicalDeviceCopyMemoryIndirectFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCopyMemoryIndirectFeaturesNV const& supported, VkPhysicalDeviceCopyMemoryIndirectFeaturesNV const& requested);
void merge_VkPhysicalDeviceCopyMemoryIndirectFeaturesNV(VkPhysicalDeviceCopyMemoryIndirectFeaturesNV & current, VkPhysicalDeviceCopyMemoryIndirectFeaturesNV const& merge_in);
#endif //(defined(VK_NV_copy_memory_indirect))
#if (defined(VK_EXT_memory_decompression))
void compare_VkPhysicalDeviceMemoryDecompressionFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceMemoryDecompressionFeaturesEXT const& supported, VkPhysicalDeviceMemoryDecompressionFeaturesEXT const& requested);
void merge_VkPhysicalDeviceMemoryDecompressionFeaturesEXT(VkPhysicalDeviceMemoryDecompressionFeaturesEXT & current, VkPhysicalDeviceMemoryDecompressionFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_memory_decompression))
#if (defined(VK_NV_memory_decompression))
void compare_VkPhysicalDeviceMemoryDecompressionFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceMemoryDecompressionFeaturesNV const& supported, VkPhysicalDeviceMemoryDecompressionFeaturesNV const& requested);
void merge_VkPhysicalDeviceMemoryDecompressionFeaturesNV(VkPhysicalDeviceMemoryDecompressionFeaturesNV & current, VkPhysicalDeviceMemoryDecompressionFeaturesNV const& merge_in);
#endif //(defined(VK_NV_memory_decompression))
#if (defined(VK_NV_device_generated_commands_compute))
void compare_VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV const& supported, VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV const& requested);
void merge_VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV(VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV & current, VkPhysicalDeviceDeviceGeneratedCommandsComputeFeaturesNV const& merge_in);
#endif //(defined(VK_NV_device_generated_commands_compute))
#if (defined(VK_NV_ray_tracing_linear_swept_spheres))
void compare_VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV const& supported, VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV const& requested);
void merge_VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV(VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV & current, VkPhysicalDeviceRayTracingLinearSweptSpheresFeaturesNV const& merge_in);
#endif //(defined(VK_NV_ray_tracing_linear_swept_spheres))
#if (defined(VK_NV_linear_color_attachment))
void compare_VkPhysicalDeviceLinearColorAttachmentFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceLinearColorAttachmentFeaturesNV const& supported, VkPhysicalDeviceLinearColorAttachmentFeaturesNV const& requested);
void merge_VkPhysicalDeviceLinearColorAttachmentFeaturesNV(VkPhysicalDeviceLinearColorAttachmentFeaturesNV & current, VkPhysicalDeviceLinearColorAttachmentFeaturesNV const& merge_in);
#endif //(defined(VK_NV_linear_color_attachment))
#if (defined(VK_EXT_image_compression_control_swapchain))
void compare_VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT const& supported, VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT const& requested);
void merge_VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT(VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT & current, VkPhysicalDeviceImageCompressionControlSwapchainFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_image_compression_control_swapchain))
#if (defined(VK_QCOM_image_processing))
void compare_VkPhysicalDeviceImageProcessingFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceImageProcessingFeaturesQCOM const& supported, VkPhysicalDeviceImageProcessingFeaturesQCOM const& requested);
void merge_VkPhysicalDeviceImageProcessingFeaturesQCOM(VkPhysicalDeviceImageProcessingFeaturesQCOM & current, VkPhysicalDeviceImageProcessingFeaturesQCOM const& merge_in);
#endif //(defined(VK_QCOM_image_processing))
#if (defined(VK_EXT_nested_command_buffer))
void compare_VkPhysicalDeviceNestedCommandBufferFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceNestedCommandBufferFeaturesEXT const& supported, VkPhysicalDeviceNestedCommandBufferFeaturesEXT const& requested);
void merge_VkPhysicalDeviceNestedCommandBufferFeaturesEXT(VkPhysicalDeviceNestedCommandBufferFeaturesEXT & current, VkPhysicalDeviceNestedCommandBufferFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_nested_command_buffer))
#if (defined(VK_EXT_extended_dynamic_state3))
void compare_VkPhysicalDeviceExtendedDynamicState3FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceExtendedDynamicState3FeaturesEXT const& supported, VkPhysicalDeviceExtendedDynamicState3FeaturesEXT const& requested);
void merge_VkPhysicalDeviceExtendedDynamicState3FeaturesEXT(VkPhysicalDeviceExtendedDynamicState3FeaturesEXT & current, VkPhysicalDeviceExtendedDynamicState3FeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_extended_dynamic_state3))
#if (defined(VK_EXT_subpass_merge_feedback))
void compare_VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT const& supported, VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT const& requested);
void merge_VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT(VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT & current, VkPhysicalDeviceSubpassMergeFeedbackFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_subpass_merge_feedback))
#if (defined(VK_ARM_tensors))
void compare_VkPhysicalDeviceTensorFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceTensorFeaturesARM const& supported, VkPhysicalDeviceTensorFeaturesARM const& requested);
void merge_VkPhysicalDeviceTensorFeaturesARM(VkPhysicalDeviceTensorFeaturesARM & current, VkPhysicalDeviceTensorFeaturesARM const& merge_in);
#endif //(defined(VK_ARM_tensors))
#if (defined(VK_ARM_tensors))
void compare_VkPhysicalDeviceDescriptorBufferTensorFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceDescriptorBufferTensorFeaturesARM const& supported, VkPhysicalDeviceDescriptorBufferTensorFeaturesARM const& requested);
void merge_VkPhysicalDeviceDescriptorBufferTensorFeaturesARM(VkPhysicalDeviceDescriptorBufferTensorFeaturesARM & current, VkPhysicalDeviceDescriptorBufferTensorFeaturesARM const& merge_in);
#endif //(defined(VK_ARM_tensors))
#if (defined(VK_EXT_shader_module_identifier))
void compare_VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT const& supported, VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT const& requested);
void merge_VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT(VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT & current, VkPhysicalDeviceShaderModuleIdentifierFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_shader_module_identifier))
#if (defined(VK_NV_optical_flow))
void compare_VkPhysicalDeviceOpticalFlowFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceOpticalFlowFeaturesNV const& supported, VkPhysicalDeviceOpticalFlowFeaturesNV const& requested);
void merge_VkPhysicalDeviceOpticalFlowFeaturesNV(VkPhysicalDeviceOpticalFlowFeaturesNV & current, VkPhysicalDeviceOpticalFlowFeaturesNV const& merge_in);
#endif //(defined(VK_NV_optical_flow))
#if (defined(VK_EXT_legacy_dithering))
void compare_VkPhysicalDeviceLegacyDitheringFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceLegacyDitheringFeaturesEXT const& supported, VkPhysicalDeviceLegacyDitheringFeaturesEXT const& requested);
void merge_VkPhysicalDeviceLegacyDitheringFeaturesEXT(VkPhysicalDeviceLegacyDitheringFeaturesEXT & current, VkPhysicalDeviceLegacyDitheringFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_legacy_dithering))
#if defined(VK_USE_PLATFORM_ANDROID_KHR) && (defined(VK_ANDROID_external_format_resolve))
void compare_VkPhysicalDeviceExternalFormatResolveFeaturesANDROID(std::vector<std::string> & error_list, VkPhysicalDeviceExternalFormatResolveFeaturesANDROID const& supported, VkPhysicalDeviceExternalFormatResolveFeaturesANDROID const& requested);
void merge_VkPhysicalDeviceExternalFormatResolveFeaturesANDROID(VkPhysicalDeviceExternalFormatResolveFeaturesANDROID & current, VkPhysicalDeviceExternalFormatResolveFeaturesANDROID const& merge_in);
#endif //defined(VK_USE_PLATFORM_ANDROID_KHR) && (defined(VK_ANDROID_external_format_resolve))
#if (defined(VK_AMD_anti_lag))
void compare_VkPhysicalDeviceAntiLagFeaturesAMD(std::vector<std::string> & error_list, VkPhysicalDeviceAntiLagFeaturesAMD const& supported, VkPhysicalDeviceAntiLagFeaturesAMD const& requested);
void merge_VkPhysicalDeviceAntiLagFeaturesAMD(VkPhysicalDeviceAntiLagFeaturesAMD & current, VkPhysicalDeviceAntiLagFeaturesAMD const& merge_in);
#endif //(defined(VK_AMD_anti_lag))
#if defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_AMDX_dense_geometry_format))
void compare_VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX(std::vector<std::string> & error_list, VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX const& supported, VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX const& requested);
void merge_VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX(VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX & current, VkPhysicalDeviceDenseGeometryFormatFeaturesAMDX const& merge_in);
#endif //defined(VK_ENABLE_BETA_EXTENSIONS) && (defined(VK_AMDX_dense_geometry_format))
#if (defined(VK_EXT_shader_object))
void compare_VkPhysicalDeviceShaderObjectFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderObjectFeaturesEXT const& supported, VkPhysicalDeviceShaderObjectFeaturesEXT const& requested);
void merge_VkPhysicalDeviceShaderObjectFeaturesEXT(VkPhysicalDeviceShaderObjectFeaturesEXT & current, VkPhysicalDeviceShaderObjectFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_shader_object))
#if (defined(VK_QCOM_tile_properties))
void compare_VkPhysicalDeviceTilePropertiesFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceTilePropertiesFeaturesQCOM const& supported, VkPhysicalDeviceTilePropertiesFeaturesQCOM const& requested);
void merge_VkPhysicalDeviceTilePropertiesFeaturesQCOM(VkPhysicalDeviceTilePropertiesFeaturesQCOM & current, VkPhysicalDeviceTilePropertiesFeaturesQCOM const& merge_in);
#endif //(defined(VK_QCOM_tile_properties))
#if (defined(VK_SEC_amigo_profiling))
void compare_VkPhysicalDeviceAmigoProfilingFeaturesSEC(std::vector<std::string> & error_list, VkPhysicalDeviceAmigoProfilingFeaturesSEC const& supported, VkPhysicalDeviceAmigoProfilingFeaturesSEC const& requested);
void merge_VkPhysicalDeviceAmigoProfilingFeaturesSEC(VkPhysicalDeviceAmigoProfilingFeaturesSEC & current, VkPhysicalDeviceAmigoProfilingFeaturesSEC const& merge_in);
#endif //(defined(VK_SEC_amigo_profiling))
#if (defined(VK_QCOM_multiview_per_view_viewports))
void compare_VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM const& supported, VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM const& requested);
void merge_VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM(VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM & current, VkPhysicalDeviceMultiviewPerViewViewportsFeaturesQCOM const& merge_in);
#endif //(defined(VK_QCOM_multiview_per_view_viewports))
#if (defined(VK_NV_ray_tracing_invocation_reorder))
void compare_VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV const& supported, VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV const& requested);
void merge_VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV(VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV & current, VkPhysicalDeviceRayTracingInvocationReorderFeaturesNV const& merge_in);
#endif //(defined(VK_NV_ray_tracing_invocation_reorder))
#if (defined(VK_NV_cooperative_vector))
void compare_VkPhysicalDeviceCooperativeVectorFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCooperativeVectorFeaturesNV const& supported, VkPhysicalDeviceCooperativeVectorFeaturesNV const& requested);
void merge_VkPhysicalDeviceCooperativeVectorFeaturesNV(VkPhysicalDeviceCooperativeVectorFeaturesNV & current, VkPhysicalDeviceCooperativeVectorFeaturesNV const& merge_in);
#endif //(defined(VK_NV_cooperative_vector))
#if (defined(VK_NV_extended_sparse_address_space))
void compare_VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV const& supported, VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV const& requested);
void merge_VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV(VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV & current, VkPhysicalDeviceExtendedSparseAddressSpaceFeaturesNV const& merge_in);
#endif //(defined(VK_NV_extended_sparse_address_space))
#if (defined(VK_EXT_legacy_vertex_attributes))
void compare_VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT const& supported, VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT const& requested);
void merge_VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT(VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT & current, VkPhysicalDeviceLegacyVertexAttributesFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_legacy_vertex_attributes))
#if (defined(VK_ARM_shader_core_builtins))
void compare_VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM const& supported, VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM const& requested);
void merge_VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM(VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM & current, VkPhysicalDeviceShaderCoreBuiltinsFeaturesARM const& merge_in);
#endif //(defined(VK_ARM_shader_core_builtins))
#if (defined(VK_EXT_pipeline_library_group_handles))
void compare_VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT const& supported, VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT const& requested);
void merge_VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT(VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT & current, VkPhysicalDevicePipelineLibraryGroupHandlesFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_pipeline_library_group_handles))
#if (defined(VK_EXT_dynamic_rendering_unused_attachments))
void compare_VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT const& supported, VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT const& requested);
void merge_VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT(VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT & current, VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_dynamic_rendering_unused_attachments))
#if (defined(VK_ARM_data_graph))
void compare_VkPhysicalDeviceDataGraphFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceDataGraphFeaturesARM const& supported, VkPhysicalDeviceDataGraphFeaturesARM const& requested);
void merge_VkPhysicalDeviceDataGraphFeaturesARM(VkPhysicalDeviceDataGraphFeaturesARM & current, VkPhysicalDeviceDataGraphFeaturesARM const& merge_in);
#endif //(defined(VK_ARM_data_graph))
#if (defined(VK_QCOM_multiview_per_view_render_areas))
void compare_VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM const& supported, VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM const& requested);
void merge_VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM(VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM & current, VkPhysicalDeviceMultiviewPerViewRenderAreasFeaturesQCOM const& merge_in);
#endif //(defined(VK_QCOM_multiview_per_view_render_areas))
#if (defined(VK_NV_per_stage_descriptor_set))
void compare_VkPhysicalDevicePerStageDescriptorSetFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDevicePerStageDescriptorSetFeaturesNV const& supported, VkPhysicalDevicePerStageDescriptorSetFeaturesNV const& requested);
void merge_VkPhysicalDevicePerStageDescriptorSetFeaturesNV(VkPhysicalDevicePerStageDescriptorSetFeaturesNV & current, VkPhysicalDevicePerStageDescriptorSetFeaturesNV const& merge_in);
#endif //(defined(VK_NV_per_stage_descriptor_set))
#if (defined(VK_QCOM_image_processing2))
void compare_VkPhysicalDeviceImageProcessing2FeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceImageProcessing2FeaturesQCOM const& supported, VkPhysicalDeviceImageProcessing2FeaturesQCOM const& requested);
void merge_VkPhysicalDeviceImageProcessing2FeaturesQCOM(VkPhysicalDeviceImageProcessing2FeaturesQCOM & current, VkPhysicalDeviceImageProcessing2FeaturesQCOM const& merge_in);
#endif //(defined(VK_QCOM_image_processing2))
#if (defined(VK_QCOM_filter_cubic_weights))
void compare_VkPhysicalDeviceCubicWeightsFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceCubicWeightsFeaturesQCOM const& supported, VkPhysicalDeviceCubicWeightsFeaturesQCOM const& requested);
void merge_VkPhysicalDeviceCubicWeightsFeaturesQCOM(VkPhysicalDeviceCubicWeightsFeaturesQCOM & current, VkPhysicalDeviceCubicWeightsFeaturesQCOM const& merge_in);
#endif //(defined(VK_QCOM_filter_cubic_weights))
#if (defined(VK_QCOM_ycbcr_degamma))
void compare_VkPhysicalDeviceYcbcrDegammaFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceYcbcrDegammaFeaturesQCOM const& supported, VkPhysicalDeviceYcbcrDegammaFeaturesQCOM const& requested);
void merge_VkPhysicalDeviceYcbcrDegammaFeaturesQCOM(VkPhysicalDeviceYcbcrDegammaFeaturesQCOM & current, VkPhysicalDeviceYcbcrDegammaFeaturesQCOM const& merge_in);
#endif //(defined(VK_QCOM_ycbcr_degamma))
#if (defined(VK_QCOM_filter_cubic_clamp))
void compare_VkPhysicalDeviceCubicClampFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceCubicClampFeaturesQCOM const& supported, VkPhysicalDeviceCubicClampFeaturesQCOM const& requested);
void merge_VkPhysicalDeviceCubicClampFeaturesQCOM(VkPhysicalDeviceCubicClampFeaturesQCOM & current, VkPhysicalDeviceCubicClampFeaturesQCOM const& merge_in);
#endif //(defined(VK_QCOM_filter_cubic_clamp))
#if (defined(VK_EXT_attachment_feedback_loop_dynamic_state))
void compare_VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT const& supported, VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT const& requested);
void merge_VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT(VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT & current, VkPhysicalDeviceAttachmentFeedbackLoopDynamicStateFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_attachment_feedback_loop_dynamic_state))
#if defined(VK_USE_PLATFORM_SCREEN_QNX) && (defined(VK_QNX_external_memory_screen_buffer))
void compare_VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX(std::vector<std::string> & error_list, VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX const& supported, VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX const& requested);
void merge_VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX(VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX & current, VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX const& merge_in);
#endif //defined(VK_USE_PLATFORM_SCREEN_QNX) && (defined(VK_QNX_external_memory_screen_buffer))
#if (defined(VK_NV_descriptor_pool_overallocation))
void compare_VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV const& supported, VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV const& requested);
void merge_VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV(VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV & current, VkPhysicalDeviceDescriptorPoolOverallocationFeaturesNV const& merge_in);
#endif //(defined(VK_NV_descriptor_pool_overallocation))
#if (defined(VK_QCOM_tile_memory_heap))
void compare_VkPhysicalDeviceTileMemoryHeapFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceTileMemoryHeapFeaturesQCOM const& supported, VkPhysicalDeviceTileMemoryHeapFeaturesQCOM const& requested);
void merge_VkPhysicalDeviceTileMemoryHeapFeaturesQCOM(VkPhysicalDeviceTileMemoryHeapFeaturesQCOM & current, VkPhysicalDeviceTileMemoryHeapFeaturesQCOM const& merge_in);
#endif //(defined(VK_QCOM_tile_memory_heap))
#if (defined(VK_NV_raw_access_chains))
void compare_VkPhysicalDeviceRawAccessChainsFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceRawAccessChainsFeaturesNV const& supported, VkPhysicalDeviceRawAccessChainsFeaturesNV const& requested);
void merge_VkPhysicalDeviceRawAccessChainsFeaturesNV(VkPhysicalDeviceRawAccessChainsFeaturesNV & current, VkPhysicalDeviceRawAccessChainsFeaturesNV const& merge_in);
#endif //(defined(VK_NV_raw_access_chains))
#if (defined(VK_NV_command_buffer_inheritance))
void compare_VkPhysicalDeviceCommandBufferInheritanceFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCommandBufferInheritanceFeaturesNV const& supported, VkPhysicalDeviceCommandBufferInheritanceFeaturesNV const& requested);
void merge_VkPhysicalDeviceCommandBufferInheritanceFeaturesNV(VkPhysicalDeviceCommandBufferInheritanceFeaturesNV & current, VkPhysicalDeviceCommandBufferInheritanceFeaturesNV const& merge_in);
#endif //(defined(VK_NV_command_buffer_inheritance))
#if (defined(VK_NV_shader_atomic_float16_vector))
void compare_VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV const& supported, VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV const& requested);
void merge_VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV(VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV & current, VkPhysicalDeviceShaderAtomicFloat16VectorFeaturesNV const& merge_in);
#endif //(defined(VK_NV_shader_atomic_float16_vector))
#if (defined(VK_EXT_shader_replicated_composites))
void compare_VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT const& supported, VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT const& requested);
void merge_VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT(VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT & current, VkPhysicalDeviceShaderReplicatedCompositesFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_shader_replicated_composites))
#if (defined(VK_EXT_shader_float8))
void compare_VkPhysicalDeviceShaderFloat8FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderFloat8FeaturesEXT const& supported, VkPhysicalDeviceShaderFloat8FeaturesEXT const& requested);
void merge_VkPhysicalDeviceShaderFloat8FeaturesEXT(VkPhysicalDeviceShaderFloat8FeaturesEXT & current, VkPhysicalDeviceShaderFloat8FeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_shader_float8))
#if (defined(VK_NV_ray_tracing_validation))
void compare_VkPhysicalDeviceRayTracingValidationFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceRayTracingValidationFeaturesNV const& supported, VkPhysicalDeviceRayTracingValidationFeaturesNV const& requested);
void merge_VkPhysicalDeviceRayTracingValidationFeaturesNV(VkPhysicalDeviceRayTracingValidationFeaturesNV & current, VkPhysicalDeviceRayTracingValidationFeaturesNV const& merge_in);
#endif //(defined(VK_NV_ray_tracing_validation))
#if (defined(VK_NV_cluster_acceleration_structure))
void compare_VkPhysicalDeviceClusterAccelerationStructureFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceClusterAccelerationStructureFeaturesNV const& supported, VkPhysicalDeviceClusterAccelerationStructureFeaturesNV const& requested);
void merge_VkPhysicalDeviceClusterAccelerationStructureFeaturesNV(VkPhysicalDeviceClusterAccelerationStructureFeaturesNV & current, VkPhysicalDeviceClusterAccelerationStructureFeaturesNV const& merge_in);
#endif //(defined(VK_NV_cluster_acceleration_structure))
#if (defined(VK_NV_partitioned_acceleration_structure))
void compare_VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV const& supported, VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV const& requested);
void merge_VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV(VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV & current, VkPhysicalDevicePartitionedAccelerationStructureFeaturesNV const& merge_in);
#endif //(defined(VK_NV_partitioned_acceleration_structure))
#if (defined(VK_EXT_device_generated_commands))
void compare_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT const& supported, VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT const& requested);
void merge_VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT(VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT & current, VkPhysicalDeviceDeviceGeneratedCommandsFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_device_generated_commands))
#if (defined(VK_MESA_image_alignment_control))
void compare_VkPhysicalDeviceImageAlignmentControlFeaturesMESA(std::vector<std::string> & error_list, VkPhysicalDeviceImageAlignmentControlFeaturesMESA const& supported, VkPhysicalDeviceImageAlignmentControlFeaturesMESA const& requested);
void merge_VkPhysicalDeviceImageAlignmentControlFeaturesMESA(VkPhysicalDeviceImageAlignmentControlFeaturesMESA & current, VkPhysicalDeviceImageAlignmentControlFeaturesMESA const& merge_in);
#endif //(defined(VK_MESA_image_alignment_control))
#if (defined(VK_NV_push_constant_bank))
void compare_VkPhysicalDevicePushConstantBankFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDevicePushConstantBankFeaturesNV const& supported, VkPhysicalDevicePushConstantBankFeaturesNV const& requested);
void merge_VkPhysicalDevicePushConstantBankFeaturesNV(VkPhysicalDevicePushConstantBankFeaturesNV & current, VkPhysicalDevicePushConstantBankFeaturesNV const& merge_in);
#endif //(defined(VK_NV_push_constant_bank))
#if (defined(VK_EXT_ray_tracing_invocation_reorder))
void compare_VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT const& supported, VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT const& requested);
void merge_VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT(VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT & current, VkPhysicalDeviceRayTracingInvocationReorderFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_ray_tracing_invocation_reorder))
#if (defined(VK_EXT_depth_clamp_control))
void compare_VkPhysicalDeviceDepthClampControlFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceDepthClampControlFeaturesEXT const& supported, VkPhysicalDeviceDepthClampControlFeaturesEXT const& requested);
void merge_VkPhysicalDeviceDepthClampControlFeaturesEXT(VkPhysicalDeviceDepthClampControlFeaturesEXT & current, VkPhysicalDeviceDepthClampControlFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_depth_clamp_control))
#if (defined(VK_HUAWEI_hdr_vivid))
void compare_VkPhysicalDeviceHdrVividFeaturesHUAWEI(std::vector<std::string> & error_list, VkPhysicalDeviceHdrVividFeaturesHUAWEI const& supported, VkPhysicalDeviceHdrVividFeaturesHUAWEI const& requested);
void merge_VkPhysicalDeviceHdrVividFeaturesHUAWEI(VkPhysicalDeviceHdrVividFeaturesHUAWEI & current, VkPhysicalDeviceHdrVividFeaturesHUAWEI const& merge_in);
#endif //(defined(VK_HUAWEI_hdr_vivid))
#if (defined(VK_NV_cooperative_matrix2))
void compare_VkPhysicalDeviceCooperativeMatrix2FeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCooperativeMatrix2FeaturesNV const& supported, VkPhysicalDeviceCooperativeMatrix2FeaturesNV const& requested);
void merge_VkPhysicalDeviceCooperativeMatrix2FeaturesNV(VkPhysicalDeviceCooperativeMatrix2FeaturesNV & current, VkPhysicalDeviceCooperativeMatrix2FeaturesNV const& merge_in);
#endif //(defined(VK_NV_cooperative_matrix2))
#if (defined(VK_ARM_pipeline_opacity_micromap))
void compare_VkPhysicalDevicePipelineOpacityMicromapFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDevicePipelineOpacityMicromapFeaturesARM const& supported, VkPhysicalDevicePipelineOpacityMicromapFeaturesARM const& requested);
void merge_VkPhysicalDevicePipelineOpacityMicromapFeaturesARM(VkPhysicalDevicePipelineOpacityMicromapFeaturesARM & current, VkPhysicalDevicePipelineOpacityMicromapFeaturesARM const& merge_in);
#endif //(defined(VK_ARM_pipeline_opacity_micromap))
#if (defined(VK_ARM_performance_counters_by_region))
void compare_VkPhysicalDevicePerformanceCountersByRegionFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDevicePerformanceCountersByRegionFeaturesARM const& supported, VkPhysicalDevicePerformanceCountersByRegionFeaturesARM const& requested);
void merge_VkPhysicalDevicePerformanceCountersByRegionFeaturesARM(VkPhysicalDevicePerformanceCountersByRegionFeaturesARM & current, VkPhysicalDevicePerformanceCountersByRegionFeaturesARM const& merge_in);
#endif //(defined(VK_ARM_performance_counters_by_region))
#if (defined(VK_ARM_shader_instrumentation))
void compare_VkPhysicalDeviceShaderInstrumentationFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceShaderInstrumentationFeaturesARM const& supported, VkPhysicalDeviceShaderInstrumentationFeaturesARM const& requested);
void merge_VkPhysicalDeviceShaderInstrumentationFeaturesARM(VkPhysicalDeviceShaderInstrumentationFeaturesARM & current, VkPhysicalDeviceShaderInstrumentationFeaturesARM const& merge_in);
#endif //(defined(VK_ARM_shader_instrumentation))
#if (defined(VK_EXT_vertex_attribute_robustness))
void compare_VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT const& supported, VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT const& requested);
void merge_VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT(VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT & current, VkPhysicalDeviceVertexAttributeRobustnessFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_vertex_attribute_robustness))
#if (defined(VK_ARM_format_pack))
void compare_VkPhysicalDeviceFormatPackFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceFormatPackFeaturesARM const& supported, VkPhysicalDeviceFormatPackFeaturesARM const& requested);
void merge_VkPhysicalDeviceFormatPackFeaturesARM(VkPhysicalDeviceFormatPackFeaturesARM & current, VkPhysicalDeviceFormatPackFeaturesARM const& merge_in);
#endif //(defined(VK_ARM_format_pack))
#if (defined(VK_VALVE_fragment_density_map_layered))
void compare_VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE(std::vector<std::string> & error_list, VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE const& supported, VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE const& requested);
void merge_VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE(VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE & current, VkPhysicalDeviceFragmentDensityMapLayeredFeaturesVALVE const& merge_in);
#endif //(defined(VK_VALVE_fragment_density_map_layered))
#if (VK_HEADER_VERSION >= 345 || defined(VK_ENABLE_BETA_EXTENSIONS)) && (defined(VK_NV_present_metering))
void compare_VkPhysicalDevicePresentMeteringFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDevicePresentMeteringFeaturesNV const& supported, VkPhysicalDevicePresentMeteringFeaturesNV const& requested);
void merge_VkPhysicalDevicePresentMeteringFeaturesNV(VkPhysicalDevicePresentMeteringFeaturesNV & current, VkPhysicalDevicePresentMeteringFeaturesNV const& merge_in);
#endif //(VK_HEADER_VERSION >= 345 || defined(VK_ENABLE_BETA_EXTENSIONS)) && (defined(VK_NV_present_metering))
#if (defined(VK_EXT_multisampled_render_to_swapchain))
void compare_VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT const& supported, VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT const& requested);
void merge_VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT(VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT & current, VkPhysicalDeviceMultisampledRenderToSwapchainFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_multisampled_render_to_swapchain))
#if (defined(VK_EXT_zero_initialize_device_memory))
void compare_VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT const& supported, VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT const& requested);
void merge_VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT(VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT & current, VkPhysicalDeviceZeroInitializeDeviceMemoryFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_zero_initialize_device_memory))
#if (defined(VK_EXT_shader_64bit_indexing))
void compare_VkPhysicalDeviceShader64BitIndexingFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShader64BitIndexingFeaturesEXT const& supported, VkPhysicalDeviceShader64BitIndexingFeaturesEXT const& requested);
void merge_VkPhysicalDeviceShader64BitIndexingFeaturesEXT(VkPhysicalDeviceShader64BitIndexingFeaturesEXT & current, VkPhysicalDeviceShader64BitIndexingFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_shader_64bit_indexing))
#if (defined(VK_EXT_custom_resolve))
void compare_VkPhysicalDeviceCustomResolveFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceCustomResolveFeaturesEXT const& supported, VkPhysicalDeviceCustomResolveFeaturesEXT const& requested);
void merge_VkPhysicalDeviceCustomResolveFeaturesEXT(VkPhysicalDeviceCustomResolveFeaturesEXT & current, VkPhysicalDeviceCustomResolveFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_custom_resolve))
#if (defined(VK_QCOM_data_graph_model))
void compare_VkPhysicalDeviceDataGraphModelFeaturesQCOM(std::vector<std::string> & error_list, VkPhysicalDeviceDataGraphModelFeaturesQCOM const& supported, VkPhysicalDeviceDataGraphModelFeaturesQCOM const& requested);
void merge_VkPhysicalDeviceDataGraphModelFeaturesQCOM(VkPhysicalDeviceDataGraphModelFeaturesQCOM & current, VkPhysicalDeviceDataGraphModelFeaturesQCOM const& merge_in);
#endif //(defined(VK_QCOM_data_graph_model))
#if (defined(VK_ARM_data_graph_optical_flow))
void compare_VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM const& supported, VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM const& requested);
void merge_VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM(VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM & current, VkPhysicalDeviceDataGraphOpticalFlowFeaturesARM const& merge_in);
#endif //(defined(VK_ARM_data_graph_optical_flow))
#if (defined(VK_EXT_shader_long_vector))
void compare_VkPhysicalDeviceShaderLongVectorFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderLongVectorFeaturesEXT const& supported, VkPhysicalDeviceShaderLongVectorFeaturesEXT const& requested);
void merge_VkPhysicalDeviceShaderLongVectorFeaturesEXT(VkPhysicalDeviceShaderLongVectorFeaturesEXT & current, VkPhysicalDeviceShaderLongVectorFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_shader_long_vector))
#if (defined(VK_SEC_pipeline_cache_incremental_mode))
void compare_VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC(std::vector<std::string> & error_list, VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC const& supported, VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC const& requested);
void merge_VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC(VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC & current, VkPhysicalDevicePipelineCacheIncrementalModeFeaturesSEC const& merge_in);
#endif //(defined(VK_SEC_pipeline_cache_incremental_mode))
#if (defined(VK_EXT_shader_uniform_buffer_unsized_array))
void compare_VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT const& supported, VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT const& requested);
void merge_VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT(VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT & current, VkPhysicalDeviceShaderUniformBufferUnsizedArrayFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_shader_uniform_buffer_unsized_array))
#if (defined(VK_NV_compute_occupancy_priority))
void compare_VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV const& supported, VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV const& requested);
void merge_VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV(VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV & current, VkPhysicalDeviceComputeOccupancyPriorityFeaturesNV const& merge_in);
#endif //(defined(VK_NV_compute_occupancy_priority))
#if (defined(VK_EXT_cooperative_matrix_maintenance1))
void compare_VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT const& supported, VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT const& requested);
void merge_VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT(VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT & current, VkPhysicalDeviceCooperativeMatrixMaintenance1FeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_cooperative_matrix_maintenance1))
#if (defined(VK_EXT_shader_subgroup_partitioned))
void compare_VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT const& supported, VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT const& requested);
void merge_VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT(VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT & current, VkPhysicalDeviceShaderSubgroupPartitionedFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_shader_subgroup_partitioned))
#if (defined(VK_EXT_shader_ocp_microscaling_types))
void compare_VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT const& supported, VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT const& requested);
void merge_VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT(VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT & current, VkPhysicalDeviceShaderOCPMicroscalingTypesFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_shader_ocp_microscaling_types))
#if (defined(VK_VALVE_shader_mixed_float_dot_product))
void compare_VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE(std::vector<std::string> & error_list, VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE const& supported, VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE const& requested);
void merge_VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE(VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE & current, VkPhysicalDeviceShaderMixedFloatDotProductFeaturesVALVE const& merge_in);
#endif //(defined(VK_VALVE_shader_mixed_float_dot_product))
#if (defined(VK_SEC_throttle_hint))
void compare_VkPhysicalDeviceThrottleHintFeaturesSEC(std::vector<std::string> & error_list, VkPhysicalDeviceThrottleHintFeaturesSEC const& supported, VkPhysicalDeviceThrottleHintFeaturesSEC const& requested);
void merge_VkPhysicalDeviceThrottleHintFeaturesSEC(VkPhysicalDeviceThrottleHintFeaturesSEC & current, VkPhysicalDeviceThrottleHintFeaturesSEC const& merge_in);
#endif //(defined(VK_SEC_throttle_hint))
#if (defined(VK_ARM_data_graph_neural_accelerator_statistics))
void compare_VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM(std::vector<std::string> & error_list, VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM const& supported, VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM const& requested);
void merge_VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM(VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM & current, VkPhysicalDeviceDataGraphNeuralAcceleratorStatisticsFeaturesARM const& merge_in);
#endif //(defined(VK_ARM_data_graph_neural_accelerator_statistics))
#if (defined(VK_EXT_primitive_restart_index))
void compare_VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT const& supported, VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT const& requested);
void merge_VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT(VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT & current, VkPhysicalDevicePrimitiveRestartIndexFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_primitive_restart_index))
#if (defined(VK_EXT_image_tiling_control))
void compare_VkPhysicalDeviceImageTilingControlFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceImageTilingControlFeaturesEXT const& supported, VkPhysicalDeviceImageTilingControlFeaturesEXT const& requested);
void merge_VkPhysicalDeviceImageTilingControlFeaturesEXT(VkPhysicalDeviceImageTilingControlFeaturesEXT & current, VkPhysicalDeviceImageTilingControlFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_image_tiling_control))
#if (defined(VK_NV_cooperative_matrix_decode_vector))
void compare_VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV(std::vector<std::string> & error_list, VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV const& supported, VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV const& requested);
void merge_VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV(VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV & current, VkPhysicalDeviceCooperativeMatrixDecodeVectorFeaturesNV const& merge_in);
#endif //(defined(VK_NV_cooperative_matrix_decode_vector))
#if (defined(VK_KHR_acceleration_structure))
void compare_VkPhysicalDeviceAccelerationStructureFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceAccelerationStructureFeaturesKHR const& supported, VkPhysicalDeviceAccelerationStructureFeaturesKHR const& requested);
void merge_VkPhysicalDeviceAccelerationStructureFeaturesKHR(VkPhysicalDeviceAccelerationStructureFeaturesKHR & current, VkPhysicalDeviceAccelerationStructureFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_acceleration_structure))
#if (defined(VK_KHR_ray_tracing_pipeline))
void compare_VkPhysicalDeviceRayTracingPipelineFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceRayTracingPipelineFeaturesKHR const& supported, VkPhysicalDeviceRayTracingPipelineFeaturesKHR const& requested);
void merge_VkPhysicalDeviceRayTracingPipelineFeaturesKHR(VkPhysicalDeviceRayTracingPipelineFeaturesKHR & current, VkPhysicalDeviceRayTracingPipelineFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_ray_tracing_pipeline))
#if (defined(VK_KHR_ray_query))
void compare_VkPhysicalDeviceRayQueryFeaturesKHR(std::vector<std::string> & error_list, VkPhysicalDeviceRayQueryFeaturesKHR const& supported, VkPhysicalDeviceRayQueryFeaturesKHR const& requested);
void merge_VkPhysicalDeviceRayQueryFeaturesKHR(VkPhysicalDeviceRayQueryFeaturesKHR & current, VkPhysicalDeviceRayQueryFeaturesKHR const& merge_in);
#endif //(defined(VK_KHR_ray_query))
#if (defined(VK_EXT_mesh_shader))
void compare_VkPhysicalDeviceMeshShaderFeaturesEXT(std::vector<std::string> & error_list, VkPhysicalDeviceMeshShaderFeaturesEXT const& supported, VkPhysicalDeviceMeshShaderFeaturesEXT const& requested);
void merge_VkPhysicalDeviceMeshShaderFeaturesEXT(VkPhysicalDeviceMeshShaderFeaturesEXT & current, VkPhysicalDeviceMeshShaderFeaturesEXT const& merge_in);
#endif //(defined(VK_EXT_mesh_shader))
void compare_feature_struct(VkStructureType sType, std::vector<std::string> & error_list, const void* supported, const void* requested);
void merge_feature_struct(VkStructureType sType, void* current, const void* merge_in);
} // namespace vkb