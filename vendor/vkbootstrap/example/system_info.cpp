#include <VkBootstrap.h>
#include <iostream>

int main() {
    vkb::InstanceBuilder instance_builder;

    auto system_info_ret = vkb::SystemInfo::get_system_info();
    if (!system_info_ret) {
        std::cerr << system_info_ret.error().message().c_str() << "\n";
        return -1;
    }
    auto system_info = system_info_ret.value();

    // check for a layer
    if (system_info.is_layer_available("VK_LAYER_LUNARG_api_dump")) {
        instance_builder.enable_layer("VK_LAYER_LUNARG_api_dump");
    }

    // of course dedicated variable for validation
    if (system_info.validation_layers_available) {
        instance_builder
            .enable_validation_layers()
            // Validation needs to send errors via a callback, have vk-bootstrap do it
            .use_default_debug_messenger();
    }

    // if you need an instance level extension
    if (system_info.is_extension_available("VK_KHR_get_physical_device_properties2")) {
        instance_builder.enable_extension("VK_KHR_get_physical_device_properties2");
    }

    // Build instance now!
    auto inst_ret = instance_builder.build();
    if (!inst_ret) {
        std::cerr << "Failed to create Vulkan instance. Error: " << inst_ret.error().message() << "\n";
        return -1;
    }
    vkb::Instance vkb_inst = inst_ret.value();

    // fancy app logic

    vkb::destroy_instance(vkb_inst);
    return 0;
}
