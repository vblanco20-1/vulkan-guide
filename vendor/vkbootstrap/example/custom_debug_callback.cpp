#include <VkBootstrap.h>
#include <iostream>
#include <vulkan/vulkan_core.h>

int main() {
    vkb::InstanceBuilder builder;
    builder.set_app_name("Example Vulkan Application").request_validation_layers().set_headless();

    builder.set_debug_callback([](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                   VkDebugUtilsMessageTypeFlagsEXT messageType,
                                   const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                   void*) -> VkBool32 {
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            const char* severity = vkb::to_string_message_severity(messageSeverity);
            const char* type = vkb::to_string_message_type(messageType);
            printf("[%s: %s] %s\n", severity, type, pCallbackData->pMessage);
        }
        // Return false to move on, but return true for validation to skip passing down the call to the driver
        return VK_TRUE;
    });

    auto inst_ret = builder.build();
    if (!inst_ret) {
        std::cerr << "Failed to create Vulkan instance. Error: " << inst_ret.error().message() << "\n";
        return -1;
    }
    vkb::Instance vkb_inst = inst_ret.value();

    vkb::PhysicalDeviceSelector selector{ vkb_inst };
    auto phys_ret = selector.select();
    if (!phys_ret) {
        std::cerr << "Failed to select Vulkan Physical Device. Error: " << phys_ret.error().message() << "\n";
        return -1;
    }

    vkb::DeviceBuilder device_builder{ phys_ret.value() };
    auto dev_ret = device_builder.build();
    if (!dev_ret) {
        std::cerr << "Failed to create Vulkan device. Error: " << dev_ret.error().message() << "\n";
        return -1;
    }
    vkb::Device vkb_device = dev_ret.value();
    vkb::DispatchTable disp = vkb_device.make_table();

    VkBuffer my_buffer;
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    // Size must be greater than zero!!!
    // We might crash, I hope the validation will come to save us!
    buffer_info.size = 0;
    VkResult result = disp.createBuffer(&buffer_info, nullptr, &my_buffer);
    if (result == VK_ERROR_VALIDATION_FAILED_EXT) {
        // If we return VK_TRUE in our callback, the validation will block the function from
        // calling the driver and return back this VkResult.
    }

    vkb::destroy_device(vkb_device);
    vkb::destroy_instance(vkb_inst);
    return 0;
}
