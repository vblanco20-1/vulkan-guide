
#include "vulkan_mock_setup.hpp"

vkb::Instance get_instance(uint32_t minor_version) {
    auto instance_ret = vkb::InstanceBuilder().request_validation_layers().require_api_version(1, minor_version).build();
    REQUIRE(instance_ret.has_value());
    return instance_ret.value();
}
vkb::Instance get_headless_instance(uint32_t minor_version) {
    auto instance_ret =
        vkb::InstanceBuilder().request_validation_layers().require_api_version(1, minor_version).set_headless().build();
    REQUIRE(instance_ret.has_value());
    return instance_ret.value();
}

VkExtensionProperties get_extension_properties(const char* extName) {
    VkExtensionProperties ext_props{};
    std::copy_n(extName, strlen(extName) + 1, ext_props.extensionName);
    return ext_props;
}

VulkanMock& get_and_setup_default() {
    VulkanMock& mock = *get_vulkan_mock();
    mock.instance_api_version = VK_API_VERSION_1_0;
    mock.instance_extensions.push_back(get_extension_properties(VK_KHR_SURFACE_EXTENSION_NAME));
#if defined(_WIN32)
    mock.instance_extensions.push_back(get_extension_properties("VK_KHR_win32_surface"));
#elif defined(__ANDROID__)
    mock.instance_extensions.push_back(get_extension_properties("VK_KHR_android_surface"));
#elif defined(_DIRECT2DISPLAY)
    mock.instance_extensions.push_back(get_extension_properties("VK_KHR_display"));
#elif defined(__linux__) || defined(__FreeBSD__)
    mock.instance_extensions.push_back(get_extension_properties("VK_KHR_xcb_surface"));
    mock.instance_extensions.push_back(get_extension_properties("VK_KHR_xlib_surface"));
    mock.instance_extensions.push_back(get_extension_properties("VK_KHR_wayland_surface"));
#elif defined(__APPLE__)
    mock.instance_extensions.push_back(get_extension_properties("VK_EXT_metal_surface"));
#endif
    mock.instance_extensions.push_back(get_extension_properties(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));
    add_basic_physical_device(mock);
    return mock;
}

VulkanMock::PhysicalDeviceDetails& add_basic_physical_device(VulkanMock& mock) {
    VulkanMock::PhysicalDeviceDetails physical_device_details{};
    physical_device_details.extensions.push_back(get_extension_properties(VK_KHR_SWAPCHAIN_EXTENSION_NAME));
    physical_device_details.properties.apiVersion = VK_API_VERSION_1_0;
    VkQueueFamilyProperties queue_family_properties{};
    queue_family_properties.queueCount = 1;
    queue_family_properties.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
    queue_family_properties.minImageTransferGranularity = { 1, 1, 1 };
    physical_device_details.queue_family_properties.push_back(queue_family_properties);
    mock.add_physical_device(std::move(physical_device_details));
    return mock.physical_devices_details.back();
}

VulkanMock::SurfaceDetails get_basic_surface_details() {
    VulkanMock::SurfaceDetails details;
    details.present_modes.push_back(VK_PRESENT_MODE_FIFO_KHR);
    details.surface_formats.push_back(VkSurfaceFormatKHR{ VK_FORMAT_R8G8B8_SRGB, VK_COLORSPACE_SRGB_NONLINEAR_KHR });
    details.capabilities.minImageCount = 2;
    details.capabilities.minImageExtent = { 600, 800 };
    details.capabilities.currentExtent = { 600, 800 };
    details.capabilities.supportedUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    return details;
}
