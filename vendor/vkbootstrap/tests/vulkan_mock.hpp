#pragma once

#include <assert.h>
#include <cstdint>
#include <cstring>

#include <limits>
#include <memory>
#include <utility>
#include <stdexcept>
#include <string>
#include <vector>

#include <vulkan/vulkan_core.h>

using SerializedStruct = std::vector<char>;

template <typename T> SerializedStruct create_serialized_struct_from_object(const T& object) {
    if (object.sType == 0) {
        throw std::runtime_error(
            "create_serialized_struct_from_object being passed in a struct without setting the sType!");
    }
    SerializedStruct new_struct(sizeof(object));
    std::memcpy(new_struct.data(), &object, new_struct.size());
    return new_struct;
}

// Hand written, must be updated to include any used struct.
inline SerializedStruct create_serialized_struct_from_features2_struct(const void* input_data, VkStructureType type) {
    switch (type) {
        case (VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES):
            return create_serialized_struct_from_object(*static_cast<const VkPhysicalDeviceDescriptorIndexingFeatures*>(input_data));
        case (VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES):
            return create_serialized_struct_from_object(*static_cast<const VkPhysicalDeviceVulkan11Features*>(input_data));
        case (VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES):
            return create_serialized_struct_from_object(*static_cast<const VkPhysicalDeviceVulkan12Features*>(input_data));
        case (VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES):
            return create_serialized_struct_from_object(*static_cast<const VkPhysicalDeviceSubgroupSizeControlFeatures*>(input_data));
        case (VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES):
            return create_serialized_struct_from_object(*static_cast<const VkPhysicalDeviceMultiviewFeatures*>(input_data));
        case (VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES):
            return create_serialized_struct_from_object(*static_cast<const VkPhysicalDeviceShaderDrawParameterFeatures*>(input_data));
        default:
            return SerializedStruct{};
    }
}

template <typename T> T get_handle(size_t value) { return reinterpret_cast<T>(value); }

#if INTPTR_MAX == INT32_MAX
template <typename T> uint64_t get_uint64_handle(uint64_t value) { return static_cast<T>(value); }
#elif INTPTR_MAX == INT64_MAX
template <typename T> T get_uint64_handle(uint64_t value) { return reinterpret_cast<T>(value); }
#endif

struct VulkanMock {
    uint32_t instance_api_version = VK_API_VERSION_1_3;
    std::vector<VkExtensionProperties> instance_extensions;
    std::vector<VkLayerProperties> instance_layers;
    std::vector<std::vector<VkExtensionProperties>> per_layer_instance_extension_properties;
    std::vector<std::vector<VkExtensionProperties>> per_layer_device_extension_properties;

    void add_layer(VkLayerProperties layer_properties,
        std::vector<VkExtensionProperties> layer_instance_extensions,
        std::vector<VkExtensionProperties> layer_device_extensions) {
        instance_layers.push_back(layer_properties);
        per_layer_instance_extension_properties.push_back(layer_instance_extensions);
        per_layer_instance_extension_properties.push_back(layer_device_extensions);
    }

    struct SurfaceDetails {
        VkSurfaceCapabilitiesKHR capabilities{};
        std::vector<VkSurfaceFormatKHR> surface_formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    std::vector<VkSurfaceKHR> surface_handles;
    std::vector<SurfaceDetails> surface_details;

    VkSurfaceKHR get_new_surface(SurfaceDetails details) {
        surface_handles.push_back(
            get_uint64_handle<VkSurfaceKHR>(static_cast<uint64_t>(0x123456789ABU + surface_handles.size())));
        surface_details.push_back(details);
        return surface_handles.back();
    }

    struct CreatedDeviceDetails {
        VkPhysicalDeviceFeatures features{};
        std::vector<const char*> extensions;
        std::vector<SerializedStruct> features_pNextChain;
    };

    struct PhysicalDeviceDetails {
        VkPhysicalDeviceProperties properties{};
        VkPhysicalDeviceFeatures features{};
        VkPhysicalDeviceMemoryProperties memory_properties{};
        std::vector<VkExtensionProperties> extensions;
        std::vector<VkQueueFamilyProperties> queue_family_properties;
        std::vector<SerializedStruct> features_pNextChain;

        std::vector<VkDevice> created_device_handles;
        std::vector<CreatedDeviceDetails> created_device_details;
    };

    std::vector<VkPhysicalDevice> physical_device_handles;
    std::vector<PhysicalDeviceDetails> physical_devices_details;

    void add_physical_device(PhysicalDeviceDetails details) {
        physical_device_handles.push_back(get_handle<VkPhysicalDevice>(0x22334455U + physical_device_handles.size()));
        physical_devices_details.emplace_back(std::move(details));
    }

    uint32_t created_image_view_count = 0;
    uint32_t fail_image_creation_on_iteration = std::numeric_limits<uint32_t>::max(); // max int means do not fail instance creation
    // Values set by various Vulkan API calls by the mock. Useful for checking that vk-bootstrap passed the correct
    // information "into" the API.
    // Because thread sanitizer yells when the mock writes to api_version_set_by_vkCreateInstance in
    // multiple threads, only enable writing when needed (aka non-threading tests)
    bool should_save_api_version = false;
    uint32_t api_version_set_by_vkCreateInstance = 0;
};

extern "C" {
VulkanMock* get_vulkan_mock();
}
