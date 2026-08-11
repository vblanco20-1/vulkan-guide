#include <VkBootstrap.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vulkan/vulkan_core.h>

bool init_vulkan() {
    vkb::InstanceBuilder builder;
    auto inst_ret =
        builder.set_app_name("Example Vulkan Application").request_validation_layers().use_default_debug_messenger().build();
    if (!inst_ret) {
        std::cerr << "Failed to create Vulkan instance. Error: " << inst_ret.error().message() << "\n";
        return false;
    }
    vkb::Instance vkb_inst = inst_ret.value();

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1024, 1024, "Vulkan Triangle", NULL, NULL);

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkResult glfw_result = glfwCreateWindowSurface(vkb_inst, window, nullptr, &surface);
    if (glfw_result != VK_SUCCESS) {
        std::cerr << "Failed to select create window surface. Error: " << std::to_string(glfw_result) << "\n";
        return false;
    }

    vkb::PhysicalDeviceSelector selector{ vkb_inst };
    auto phys_ret = selector.set_surface(surface).select();
    if (!phys_ret) {
        std::cerr << "Failed to select Vulkan Physical Device. Error: " << phys_ret.error().message() << "\n";
        if (phys_ret.error() == vkb::PhysicalDeviceError::no_suitable_device) {
            const auto& detailed_reasons = phys_ret.detailed_failure_reasons();
            if (!detailed_reasons.empty()) {
                std::cerr << "GPU Selection failure reasons:\n";
                for (const std::string& reason : detailed_reasons) {
                    std::cerr << reason << "\n";
                }
            }
        }
        return false;
    }

    vkb::DeviceBuilder device_builder{ phys_ret.value() };
    // automatically propagate needed data from instance & physical device
    auto dev_ret = device_builder.build();
    if (!dev_ret) {
        std::cerr << "Failed to create Vulkan device. Error: " << dev_ret.error().message() << "\n";
        return false;
    }
    vkb::Device vkb_device = dev_ret.value();

    // Get the VkDevice handle used in the rest of a vulkan application
    VkDevice device = vkb_device.device;

    // Get the graphics queue with a helper function
    auto graphics_queue_ret = vkb_device.get_queue(vkb::QueueType::graphics);
    if (!graphics_queue_ret) {
        std::cerr << "Failed to get graphics queue. Error: " << graphics_queue_ret.error().message() << "\n";
        return false;
    }
    VkQueue graphics_queue = graphics_queue_ret.value();

    vkb::SwapchainBuilder swapchain_builder{ vkb_device };
    auto swap_ret = swapchain_builder.build();
    if (!swap_ret) {
        std::cout << swap_ret.error().message() << "\n";
        return false;
    }
    vkb::Swapchain vkb_swapchain = swap_ret.value();

    // We did it!
    // Turned 400-500 lines of boilerplate into a fraction of that.

    // Time to cleanup
    vkb::destroy_swapchain(vkb_swapchain);
    vkb::destroy_device(vkb_device);
    vkb::destroy_surface(vkb_inst, surface);
    vkb::destroy_instance(vkb_inst);
    glfwDestroyWindow(window);
    glfwTerminate();
    (void)device; // silence unused warning
    (void)graphics_queue;
    return true;
}

int main() {
    if (init_vulkan()) {
        std::cout << "Boilerplate done, time to write your application!\n";
    }
}
