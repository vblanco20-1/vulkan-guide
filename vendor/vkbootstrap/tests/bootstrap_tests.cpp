#include "vulkan_library_loader.hpp"

#include "vulkan_mock.hpp"
#include "vulkan_mock_setup.hpp"

#include <array>
#include <thread>

// TODO
// changing present modes and/or image formats

TEST_CASE("Instance with surface", "[VkBootstrap.bootstrap]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_api_version = VK_API_VERSION_1_1;
    mock.physical_devices_details[0].properties.apiVersion = VK_API_VERSION_1_1;
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("VK_KHR_multiview"));
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("VK_KHR_driver_properties"));
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("VK_EXT_robustness2"));
    auto surface = mock.get_new_surface(get_basic_surface_details());
    GIVEN("A window and a vulkan instance") {

        auto sys_info_ret = vkb::SystemInfo::get_system_info();
        REQUIRE(sys_info_ret);

        vkb::InstanceBuilder instance_builder;
        auto instance_ret = instance_builder.require_api_version(1, 1, 0).use_default_debug_messenger().build();
        REQUIRE(instance_ret);
        vkb::Instance instance = instance_ret.value();

        GIVEN("A default selected physical device") {
            vkb::PhysicalDeviceSelector phys_device_selector(instance);
            auto phys_device_ret = phys_device_selector.set_surface(surface).select();
            REQUIRE(phys_device_ret);
            vkb::PhysicalDevice physical_device = phys_device_ret.value();

            GIVEN("A device created with default parameters") {
                vkb::DeviceBuilder device_builder(physical_device);
                auto device_ret = device_builder.build();
                REQUIRE(device_ret);
                vkb::Device device = device_ret.value();

                // possible swapchain creation...

                vkb::destroy_device(device);
            }
        }

        THEN("Can select physical device with customized requirements") {
            vkb::PhysicalDeviceSelector selector(instance);
            auto phys_dev_ret = selector.set_surface(surface)
                                    .add_required_extension(VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME)
                                    .set_minimum_version(1, 0)
                                    .select();
            REQUIRE(phys_dev_ret.has_value());

            REQUIRE(phys_dev_ret->is_extension_present(VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME));
            REQUIRE(!phys_dev_ret->is_extension_present(VK_KHR_16BIT_STORAGE_EXTENSION_NAME));

            REQUIRE(phys_dev_ret->enable_extension_if_present(VK_EXT_ROBUSTNESS_2_EXTENSION_NAME));
            REQUIRE(!phys_dev_ret->enable_extension_if_present(VK_KHR_16BIT_STORAGE_EXTENSION_NAME));

            const std::vector<const char*> extension_set_1 = { VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME,
                VK_EXT_ROBUSTNESS_2_EXTENSION_NAME };
            const std::vector<const char*> extension_set_2 = { VK_KHR_16BIT_STORAGE_EXTENSION_NAME,
                VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME };

            REQUIRE(phys_dev_ret->enable_extensions_if_present(extension_set_1));
            REQUIRE(!phys_dev_ret->enable_extensions_if_present(extension_set_2));

            auto device_ret = vkb::DeviceBuilder(phys_dev_ret.value()).build();
            REQUIRE(device_ret.has_value());
            vkb::destroy_device(device_ret.value());
        }

        vkb::destroy_surface(instance, surface);
        vkb::destroy_instance(instance);
    }
    GIVEN("Two Instances") {
        vkb::InstanceBuilder instance_builder1;
        auto instance_ret1 = instance_builder1.use_default_debug_messenger().build();
        REQUIRE(instance_ret1);
        vkb::InstanceBuilder instance_builder2;
        auto instance_ret2 = instance_builder2.use_default_debug_messenger().build();
        REQUIRE(instance_ret2);

        vkb::destroy_instance(instance_ret1.value());
        vkb::destroy_instance(instance_ret2.value());
    }
}

TEST_CASE("instance configuration", "[VkBootstrap.bootstrap]") {
    [[maybe_unused]] VulkanMock& mock = get_and_setup_default();
    SECTION("custom debug callback") {
        vkb::InstanceBuilder builder;

        auto instance_ret = builder.request_validation_layers()
                                .set_app_name("test app")
                                .set_app_version(1, 0, 0)
                                .set_engine_name("engine_name")
                                .set_engine_version(9, 9, 9)
                                .set_debug_callback([](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void*
                                                        /*pUserData*/) -> VkBool32 {
                                    auto ms = vkb::to_string_message_severity(messageSeverity);
                                    auto mt = vkb::to_string_message_type(messageType);
                                    printf("[%s: %s](user defined)\n%s\n", ms, mt, pCallbackData->pMessage);
                                    return VK_FALSE;
                                })
                                .build();

        REQUIRE(instance_ret.has_value());

        vkb::destroy_instance(instance_ret.value());
    }
    SECTION("Validation configuration") {
        vkb::InstanceBuilder builder;

        auto instance_ret =
            builder.request_validation_layers()
                .require_api_version(1, 0)
                .use_default_debug_messenger()
                .add_validation_feature_enable(VkValidationFeatureEnableEXT::VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT)
                .add_validation_feature_disable(VkValidationFeatureDisableEXT::VK_VALIDATION_FEATURE_DISABLE_OBJECT_LIFETIMES_EXT)
                .add_validation_disable(VkValidationCheckEXT::VK_VALIDATION_CHECK_SHADERS_EXT)
                .build();
        REQUIRE(instance_ret.has_value());
        vkb::destroy_instance(instance_ret.value());
    }
}

TEST_CASE("Headless Vulkan", "[VkBootstrap.bootstrap]") {
    [[maybe_unused]] VulkanMock& mock = get_and_setup_default();
    auto instance = get_headless_instance();

    vkb::PhysicalDeviceSelector phys_device_selector(instance);
    auto phys_device_ret = phys_device_selector.select();
    REQUIRE(phys_device_ret.has_value());
    auto phys_device = phys_device_ret.value();

    vkb::DeviceBuilder device_builder(phys_device);
    auto device_ret = device_builder.build();
    REQUIRE(device_ret.has_value());
    vkb::destroy_device(device_ret.value());

    vkb::destroy_instance(instance);
}

TEST_CASE("Device Configuration", "[VkBootstrap.bootstrap]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_api_version = VK_API_VERSION_1_1;
    mock.physical_devices_details[0].properties.apiVersion = VK_API_VERSION_1_1;
    auto instance = get_instance(1);
    auto surface = mock.get_new_surface(get_basic_surface_details());

    vkb::PhysicalDeviceSelector phys_device_selector(instance);

    auto phys_device_ret = phys_device_selector.set_minimum_version(1, 1).set_surface(surface).select();
    REQUIRE(phys_device_ret.has_value());
    auto phys_device = phys_device_ret.value();


    SECTION("Custom queue setup") {
        std::vector<vkb::CustomQueueDescription> queue_descriptions;
        auto queue_families = phys_device.get_queue_families();
        for (uint32_t i = 0; i < (uint32_t)queue_families.size(); i++) {
            if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                queue_descriptions.push_back(
                    vkb::CustomQueueDescription(i, std::vector<float>(queue_families[i].queueCount, 1.0f)));
            }
        }
        if (phys_device.has_dedicated_compute_queue()) {
            for (uint32_t i = 0; i < (uint32_t)queue_families.size(); i++) {
                if ((queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) &&
                    (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 &&
                    (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) == 0)
                    queue_descriptions.push_back(
                        vkb::CustomQueueDescription(i, std::vector<float>(queue_families[i].queueCount, 1.0f)));
            }
        } else if (phys_device.has_separate_compute_queue()) {
            for (uint32_t i = 0; i < (uint32_t)queue_families.size(); i++) {
                if ((queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) &&
                    ((queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
                    queue_descriptions.push_back(
                        vkb::CustomQueueDescription(i, std::vector<float>(queue_families[i].queueCount, 1.0f)));
                }
            }
        }

        vkb::DeviceBuilder device_builder(phys_device);
        auto device_ret = device_builder.custom_queue_setup(queue_descriptions).build();
        REQUIRE(device_ret.has_value());
        vkb::destroy_device(device_ret.value());
    }

    SECTION("VkPhysicalDeviceFeatures2 in pNext Chain") {
        VkPhysicalDeviceMultiviewFeatures multiview_features{};
        multiview_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES;

        VkPhysicalDeviceShaderDrawParameterFeatures shader_draw_features{};
        shader_draw_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES;
        shader_draw_features.pNext = &multiview_features;

        vkb::DeviceBuilder device_builder(phys_device_ret.value());
        auto device_ret = device_builder.add_pNext(&shader_draw_features).build();
        REQUIRE(device_ret.has_value());

        // Check that the pNext chain was passed through successfully
        auto& features_pNextChain = mock.physical_devices_details.at(0).created_device_details.at(0).features_pNextChain;
        auto* s1 = reinterpret_cast<VkPhysicalDeviceShaderDrawParameterFeatures*>(features_pNextChain.at(0).data());
        auto* s2 = reinterpret_cast<VkPhysicalDeviceMultiviewFeatures*>(features_pNextChain.at(1).data());

        REQUIRE(s1->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES);
        REQUIRE(s2->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES);

        vkb::destroy_device(device_ret.value());
    }

    vkb::destroy_surface(instance, surface);
    vkb::destroy_instance(instance);
}

TEST_CASE("Physical Device Version", "[VkBootstrap.minimum_device_version]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_api_version = VK_API_VERSION_1_4;
    mock.physical_devices_details[0].properties.apiVersion = VK_API_VERSION_1_1;
    mock.physical_devices_details[0].properties.deviceID = 1;
    add_basic_physical_device(mock);
    mock.physical_devices_details[1].properties.apiVersion = VK_API_VERSION_1_4;
    mock.physical_devices_details[1].properties.deviceID = 4;

    auto instance_ret = vkb::InstanceBuilder{}.set_headless().require_api_version(1, 4).build();
    REQUIRE(instance_ret);
    auto instance = instance_ret.value();

    vkb::PhysicalDeviceSelector phys_device_selector(instance);

    auto phys_device_ret_1_1 = phys_device_selector.set_minimum_version(1, 1).select();
    REQUIRE(phys_device_ret_1_1.has_value());
    REQUIRE(phys_device_ret_1_1.value().properties.deviceID == 1);

    auto phys_device_ret_1_4 = phys_device_selector.set_minimum_version(1, 4).select();
    REQUIRE(phys_device_ret_1_4.has_value());
    REQUIRE(phys_device_ret_1_4.value().properties.deviceID == 4);
}

TEST_CASE("Physical Device Version lower than instance", "[VkBootstrap.minimum_device_version_lower_than_instance]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_api_version = VK_API_VERSION_1_1;
    mock.physical_devices_details[0].properties.apiVersion = VK_API_VERSION_1_1;
    mock.physical_devices_details[0].properties.deviceID = 1;
    add_basic_physical_device(mock);
    mock.physical_devices_details[1].properties.apiVersion = VK_API_VERSION_1_4;
    mock.physical_devices_details[1].properties.deviceID = 4;

    auto instance_ret =
        vkb::InstanceBuilder{}.set_headless().require_api_version(1, 4).set_minimum_instance_version(1, 1).build();
    REQUIRE(instance_ret);
    auto instance = instance_ret.value();

    vkb::PhysicalDeviceSelector phys_device_selector(instance);

    auto phys_device_ret_1_1 = phys_device_selector.set_minimum_version(1, 1).select();
    REQUIRE(phys_device_ret_1_1.has_value());
    REQUIRE(phys_device_ret_1_1.value().properties.deviceID == 1);

    auto phys_device_ret_1_4 = phys_device_selector.set_minimum_version(1, 4).select();
    REQUIRE(phys_device_ret_1_4.has_value());
    REQUIRE(phys_device_ret_1_4.value().properties.deviceID == 4);
}


TEST_CASE("Select all Physical Devices", "[VkBootstrap.bootstrap]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_api_version = VK_API_VERSION_1_1;
    mock.physical_devices_details[0].properties.apiVersion = VK_API_VERSION_1_1;
    std::string message = "mocking_gpus_for_fun_and_profit";
    std::copy_n(message.c_str(), message.size(), mock.physical_devices_details[0].properties.deviceName);

    auto instance = get_instance(1);
    auto surface = mock.get_new_surface(get_basic_surface_details());

    auto instance_dispatch_table = instance.make_table();
    // needs to successfully create an instance dispatch table
    REQUIRE(instance_dispatch_table.fp_vkEnumeratePhysicalDevices);


    vkb::PhysicalDeviceSelector phys_device_selector(instance, surface);

    auto phys_device_ret = phys_device_selector.select_devices();
    REQUIRE(phys_device_ret.has_value());
    auto phys_devices = phys_device_ret.value();

    REQUIRE(phys_devices.at(0).name.size() > 0);

    auto phys_device_names_ret = phys_device_selector.select_device_names();
    REQUIRE(phys_device_names_ret.has_value());
    REQUIRE(phys_device_names_ret.value().at(0).size() > 0);

    vkb::DeviceBuilder device_builder(phys_devices.at(0));
    auto device_ret = device_builder.build();
    REQUIRE(device_ret.has_value());
    auto dispatch_table = device_ret.value().make_table();
    // needs to successfully create a dispatch table
    REQUIRE(dispatch_table.fp_vkCreateCommandPool);
}

TEST_CASE("Select Physical Devices by type", "[VkBootstrap.sort_physical_devices]") {
    VulkanMock& mock = get_and_setup_default();
    for (int i = 0; i < 4; i++) {
        add_basic_physical_device(mock);
    }
    for (uint32_t i = 0; i < 5; i++) {
        mock.physical_devices_details[i].properties.deviceID = i;
        mock.physical_devices_details[i].properties.deviceType = static_cast<VkPhysicalDeviceType>(i);
    }

    auto instance = get_instance(0);
    auto surface = mock.get_new_surface(get_basic_surface_details());

    for (uint32_t i = 0; i < 5; i++) {
        vkb::PhysicalDeviceSelector phys_device_selector(instance, surface);
        auto phys_dev_ret = phys_device_selector.prefer_gpu_device_type(static_cast<vkb::PreferredDeviceType>(i)).select();
        REQUIRE(phys_dev_ret.has_value());
        auto phys_dev = phys_dev_ret.value();
        REQUIRE(phys_dev.properties.deviceID == i);

        auto vector_ret = phys_device_selector.prefer_gpu_device_type(static_cast<vkb::PreferredDeviceType>(i)).select_devices();
        REQUIRE(vector_ret.has_value());
        auto vector = vector_ret.value();
        REQUIRE(vector.size() == 5);
        REQUIRE(vector[0].properties.deviceID == i);

        vector_ret = phys_device_selector.prefer_gpu_device_type(static_cast<vkb::PreferredDeviceType>(i))
                         .allow_any_gpu_device_type(false)
                         .select_devices();
        REQUIRE(vector_ret.has_value());
        vector = vector_ret.value();
        REQUIRE(vector.size() == 1);
        REQUIRE(vector[0].properties.deviceID == i);
    }
}

TEST_CASE("Reject Physical Devices by type", "[VkBootstrap.sort_physical_devices]") {
    VulkanMock& mock = get_and_setup_default();
    // set the physical device to have a weird type
    mock.physical_devices_details.back().properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;

    auto instance = get_instance(0);
    auto surface = mock.get_new_surface(get_basic_surface_details());

    for (uint32_t i = 0; i < 5; i++) {

        vkb::PhysicalDeviceSelector phys_device_selector(instance, surface);
        auto phys_dev_ret = phys_device_selector.prefer_gpu_device_type(static_cast<vkb::PreferredDeviceType>(i))
                                .allow_any_gpu_device_type(false)
                                .select();
        if (i == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU) {
            REQUIRE(phys_dev_ret.has_value());
        } else {
            REQUIRE(!phys_dev_ret.has_value());
            REQUIRE(!phys_dev_ret.full_error().detailed_failure_reasons.empty());
            std::cout << phys_dev_ret.full_error().detailed_failure_reasons.back() << "\n";
        }
    }
}


TEST_CASE("Loading Dispatch Table", "[VkBootstrap.bootstrap]") {
    [[maybe_unused]] VulkanMock& mock = get_and_setup_default();
    auto instance = get_headless_instance(0);
    {
        vkb::PhysicalDeviceSelector selector(instance);
        auto phys_dev_ret = selector.select_first_device_unconditionally().select();
        REQUIRE(phys_dev_ret.has_value());

        vkb::DeviceBuilder device_builder(phys_dev_ret.value());
        auto device_ret = device_builder.build();
        REQUIRE(device_ret.has_value());
        auto dispatch_table = device_ret.value().make_table();

        // Create a basic Device specific type to test with
        VkFenceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        VkFence fence = VK_NULL_HANDLE;
        dispatch_table.createFence(&info, nullptr, &fence);
        REQUIRE(fence != VK_NULL_HANDLE);

        dispatch_table.destroyFence(fence, nullptr);
        vkb::destroy_device(device_ret.value());
    }
    vkb::destroy_instance(instance);
}

TEST_CASE("Swapchain", "[VkBootstrap.bootstrap]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_api_version = VK_API_VERSION_1_1;
    mock.physical_devices_details[0].properties.apiVersion = VK_API_VERSION_1_1;
    auto surface = mock.get_new_surface(get_basic_surface_details());
    GIVEN("A working instance, window, surface, and device") {
        auto instance = get_instance(1);

        vkb::PhysicalDeviceSelector phys_device_selector(instance);
        auto phys_device_ret = phys_device_selector.set_surface(surface).select();
        REQUIRE(phys_device_ret.has_value());
        auto phys_device = phys_device_ret.value();

        vkb::DeviceBuilder device_builder(phys_device);
        auto device_ret = device_builder.build();
        REQUIRE(device_ret.has_value());
        vkb::Device device = device_ret.value();

        auto graphics_queue_index = device.get_queue_index(vkb::QueueType::graphics).value();
        auto present_queue_index = device.get_queue_index(vkb::QueueType::present).value();

        THEN("Swapchain can be made") {
            vkb::SwapchainBuilder swapchain_builder(device);
            auto swapchain_ret = swapchain_builder.build();
            REQUIRE(swapchain_ret.has_value());
            auto swapchain = swapchain_ret.value();

            THEN("Acquire swapchain images and views") {
                auto images = swapchain.get_images();
                REQUIRE(images.has_value());
                REQUIRE(images.value().size() > 0);

                auto image_views = swapchain.get_image_views();
                REQUIRE(image_views.has_value());
                REQUIRE(image_views.value().size() > 0);
                swapchain.destroy_image_views(image_views.value());
            }
            AND_THEN("Acquire swapchain images views with nullptr pNext chain") {
                auto image_views = swapchain.get_image_views(nullptr);
                REQUIRE(image_views.has_value());
                REQUIRE(image_views.value().size() > 0);
                swapchain.destroy_image_views(image_views.value());
            }
            AND_THEN("Acquire swapchain image views with valid pNext chain") {
                VkImageViewUsageCreateInfo usage = { VK_STRUCTURE_TYPE_IMAGE_VIEW_USAGE_CREATE_INFO, nullptr, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
                auto image_views = swapchain.get_image_views(&usage);
                REQUIRE(image_views.has_value());
                REQUIRE(image_views.value().size() > 0);
                swapchain.destroy_image_views(image_views.value());
            }
            AND_THEN("Doesn't leak image views when it fails") {
                mock.fail_image_creation_on_iteration = 2;
                REQUIRE(mock.created_image_view_count == 0);
                auto image_views = swapchain.get_image_views();
                REQUIRE(image_views.error());
                REQUIRE(mock.created_image_view_count == 0);
            }

            vkb::destroy_swapchain(swapchain_ret.value());
        }

        AND_THEN("Swapchain configuration") {
            vkb::SwapchainBuilder swapchain_builder(device);
            auto swapchain_ret = swapchain_builder.set_desired_extent(256, 256)
                                     .set_desired_format({ VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
                                     .set_desired_present_mode(VK_PRESENT_MODE_IMMEDIATE_KHR)
                                     .set_pre_transform_flags(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
                                     .set_composite_alpha_flags(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
                                     .set_clipped(false)
                                     .set_image_array_layer_count(1)
                                     .build();
            REQUIRE(swapchain_ret.has_value());

            vkb::destroy_swapchain(swapchain_ret.value());
        }
        AND_THEN("Swapchain defaults can be used") {
            vkb::SwapchainBuilder swapchain_builder(device);
            auto swapchain_ret = swapchain_builder.use_default_format_selection()
                                     .use_default_present_mode_selection()
                                     .use_default_image_usage_flags()
                                     .build();
            REQUIRE(swapchain_ret.has_value());

            vkb::destroy_swapchain(swapchain_ret.value());
        }
        AND_THEN("Swapchain can be recreated") {
            vkb::SwapchainBuilder swapchain_builder(device);
            auto swapchain_ret = swapchain_builder.build();
            REQUIRE(swapchain_ret.has_value());

            auto swapchain = swapchain_ret.value();

            auto recreated_swapchain_ret = swapchain_builder.set_old_swapchain(swapchain).build();
            REQUIRE(recreated_swapchain_ret.has_value());

            vkb::destroy_swapchain(swapchain_ret.value());
            vkb::destroy_swapchain(recreated_swapchain_ret.value());
        }
        AND_THEN("Swapchain can be created from individual handles") {
            vkb::SwapchainBuilder swapchain_builder(
                device.physical_device.physical_device, device.device, surface, graphics_queue_index, present_queue_index);
            auto swapchain_ret = swapchain_builder.build();
            REQUIRE(swapchain_ret.has_value());

            auto swapchain = swapchain_ret.value();

            auto recreated_swapchain_ret = swapchain_builder.set_old_swapchain(swapchain).build();
            REQUIRE(recreated_swapchain_ret.has_value());

            vkb::destroy_swapchain(swapchain_ret.value());
            vkb::destroy_swapchain(recreated_swapchain_ret.value());
        }
        AND_THEN("Swapchain can be create with default gotten handles") {
            vkb::SwapchainBuilder swapchain_builder(device.physical_device.physical_device, device.device, surface);
            auto swapchain_ret = swapchain_builder.build();
            REQUIRE(swapchain_ret.has_value());

            auto swapchain = swapchain_ret.value();

            auto recreated_swapchain_ret = swapchain_builder.set_old_swapchain(swapchain).build();
            REQUIRE(recreated_swapchain_ret.has_value());

            vkb::destroy_swapchain(swapchain_ret.value());
            vkb::destroy_swapchain(recreated_swapchain_ret.value());
        }

        vkb::destroy_device(device_ret.value());
        vkb::destroy_surface(instance, surface);
        vkb::destroy_instance(instance);
    }
}

void* VKAPI_PTR shim_vkAllocationFunction(
    void* /*pUserData*/, size_t size, size_t /*alignment*/, VkSystemAllocationScope /*allocationScope*/) {
    return malloc(size);
}
void* VKAPI_PTR shim_vkReallocationFunction(
    void* /*pUserData*/, void* pOriginal, size_t size, size_t /*alignment*/, VkSystemAllocationScope /*allocationScope*/) {
    return realloc(pOriginal, size);
}
void VKAPI_PTR shim_vkFreeFunction(void* /*pUserData*/, void* pMemory) { return free(pMemory); }


TEST_CASE("Allocation Callbacks", "[VkBootstrap.bootstrap]") {
    VulkanMock& mock = get_and_setup_default();
    auto surface = mock.get_new_surface(get_basic_surface_details());

    VkAllocationCallbacks allocation_callbacks{};
    allocation_callbacks.pfnAllocation = &shim_vkAllocationFunction;
    allocation_callbacks.pfnReallocation = &shim_vkReallocationFunction;
    allocation_callbacks.pfnFree = &shim_vkFreeFunction;

    vkb::InstanceBuilder builder;

    auto instance_ret = builder.request_validation_layers()
                            .set_allocation_callbacks(&allocation_callbacks)
                            .use_default_debug_messenger()
                            .build();
    REQUIRE(instance_ret.has_value());

    vkb::PhysicalDeviceSelector phys_device_selector(instance_ret.value());

    auto phys_device_ret = phys_device_selector.set_surface(surface).select();
    REQUIRE(phys_device_ret.has_value());
    auto phys_device = phys_device_ret.value();

    vkb::DeviceBuilder device_builder(phys_device);
    auto device_ret = device_builder.set_allocation_callbacks(&allocation_callbacks).build();
    REQUIRE(device_ret.has_value());
    vkb::Device device = device_ret.value();

    vkb::SwapchainBuilder swapchain_builder(device);
    auto swapchain_ret = swapchain_builder.set_allocation_callbacks(&allocation_callbacks).build();
    REQUIRE(swapchain_ret.has_value());
    // auto swapchain = swapchain_ret.value ();

    vkb::destroy_swapchain(swapchain_ret.value());
    vkb::destroy_device(device_ret.value());

    vkb::destroy_surface(instance_ret.value(), surface);
    vkb::destroy_instance(instance_ret.value());
}

TEST_CASE("SystemInfo Loading Vulkan Automatically", "[VkBootstrap.loading]") {
    [[maybe_unused]] VulkanMock& mock = get_and_setup_default();
    auto info_ret = vkb::SystemInfo::get_system_info();
    REQUIRE(info_ret);
}

TEST_CASE("SystemInfo Loading Vulkan Automatically in Multiple Threads", "[VkBootstrap.loading]") {
    [[maybe_unused]] VulkanMock& mock = get_and_setup_default();
    const size_t number_of_threads = 16;
    std::vector<vkb::Result<vkb::SystemInfo>> info_rets(number_of_threads, vkb::Error{});
    for (size_t i = 0; i < number_of_threads; ++i) {
        REQUIRE(!info_rets[i]);
    }
    std::vector<std::thread> threads;
    for (size_t i = 0; i < number_of_threads; ++i) {
        threads.emplace_back(std::thread([&info_ret = info_rets[i]] { info_ret = vkb::SystemInfo::get_system_info(); }));
    }
    for (size_t i = 0; i < number_of_threads; ++i) {
        threads[i].join();
    }
    for (size_t i = 0; i < number_of_threads; ++i) {
        REQUIRE(info_rets[i]);
    }
}

TEST_CASE("Instance Creation Loading Vulkan Automatically in Multiple Threads", "[VkBootstrap.loading]") {
    [[maybe_unused]] VulkanMock& mock = get_and_setup_default();
    const size_t number_of_threads = 16;
    std::vector<vkb::Result<vkb::Instance>> instance_rets(number_of_threads, vkb::Error{});
    for (size_t i = 0; i < number_of_threads; ++i) {
        REQUIRE(!instance_rets[i]);
    }
    std::vector<std::thread> threads;
    for (size_t i = 0; i < number_of_threads; ++i) {
        threads.emplace_back(
            std::thread([&instance_ret = instance_rets[i]] { instance_ret = vkb::InstanceBuilder{}.build(); }));
    }
    for (size_t i = 0; i < number_of_threads; ++i) {
        threads[i].join();
    }
    for (size_t i = 0; i < number_of_threads; ++i) {
        REQUIRE(instance_rets[i]);
    }
}

TEST_CASE("SystemInfo Check Instance API Version", "[VkBootstrap.instance_api_version]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_api_version = VK_API_VERSION_1_2;
    auto info_ret = vkb::SystemInfo::get_system_info();
    REQUIRE(info_ret);
    auto system_info = info_ret.value();
    REQUIRE(system_info.is_instance_version_available(VK_MAKE_API_VERSION(0, 1, 0, 0)));
    REQUIRE(system_info.is_instance_version_available(VK_MAKE_API_VERSION(0, 1, 1, 0)));
    REQUIRE(system_info.is_instance_version_available(VK_MAKE_API_VERSION(0, 1, 2, 0)));
    REQUIRE(!system_info.is_instance_version_available(VK_MAKE_API_VERSION(0, 1, 3, 0)));
    REQUIRE(!system_info.is_instance_version_available(VK_MAKE_API_VERSION(0, 1, 4, 0)));

    REQUIRE(system_info.is_instance_version_available(1, 0));
    REQUIRE(system_info.is_instance_version_available(1, 1));
    REQUIRE(system_info.is_instance_version_available(1, 2));
    REQUIRE(!system_info.is_instance_version_available(1, 3));
    REQUIRE(!system_info.is_instance_version_available(1, 4));
}

TEST_CASE("SystemInfo Loading Vulkan Manually", "[VkBootstrap.loading]") {
    [[maybe_unused]] VulkanMock& mock = get_and_setup_default();
    VulkanLibrary vk_lib;
    REQUIRE(vk_lib.vkGetInstanceProcAddr);
    auto info_ret = vkb::SystemInfo::get_system_info(vk_lib.vkGetInstanceProcAddr);
    REQUIRE(info_ret);
    vk_lib.close();
}

TEST_CASE("InstanceBuilder Loading Vulkan Automatically", "[VkBootstrap.loading]") {
    [[maybe_unused]] VulkanMock& mock = get_and_setup_default();
    vkb::InstanceBuilder builder;
    auto ret = builder.build();
    REQUIRE(ret);
}

TEST_CASE("InstanceBuilder Loading Vulkan Manually", "[VkBootstrap.loading]") {
    [[maybe_unused]] VulkanMock& mock = get_and_setup_default();
    VulkanLibrary vk_lib;
    REQUIRE(vk_lib.vkGetInstanceProcAddr);
    vkb::InstanceBuilder builder{ vk_lib.vkGetInstanceProcAddr };
    auto ret = builder.build();
    REQUIRE(ret);
    vk_lib.close();
}
TEST_CASE("ReLoading Vulkan Automatically", "[VkBootstrap.loading]") {
    [[maybe_unused]] VulkanMock& mock = get_and_setup_default();
    {
        vkb::InstanceBuilder builder;
        auto ret = builder.build();
        REQUIRE(ret);
    }
    {
        vkb::InstanceBuilder builder;
        auto ret = builder.build();
        REQUIRE(ret);
    }
}

TEST_CASE("ReLoading Vulkan Manually", "[VkBootstrap.loading]") {
    [[maybe_unused]] VulkanMock& mock = get_and_setup_default();
    {
        VulkanLibrary vk_lib;
        REQUIRE(vk_lib.vkGetInstanceProcAddr);
        vkb::InstanceBuilder builder{ vk_lib.vkGetInstanceProcAddr };
        auto ret = builder.build();
        REQUIRE(ret);
        vk_lib.close();
    }
    {
        VulkanLibrary vk_lib;
        REQUIRE(vk_lib.vkGetInstanceProcAddr);
        vkb::InstanceBuilder builder{ vk_lib.vkGetInstanceProcAddr };
        auto ret = builder.build();
        REQUIRE(ret);
        vk_lib.close();
    }
}

TEST_CASE("Minimum instance API version", "[VkBootstrap.api_version]") {
    VulkanMock& mock = get_and_setup_default();
    mock.should_save_api_version = true;
    mock.instance_api_version = VK_API_VERSION_1_2;
    mock.physical_devices_details[0].properties.apiVersion = VK_API_VERSION_1_3;
    mock.physical_devices_details[0].properties.deviceID = 1;
    add_basic_physical_device(mock);
    mock.physical_devices_details[1].properties.apiVersion = VK_API_VERSION_1_4;
    mock.physical_devices_details[1].properties.deviceID = 2;

    auto ret = vkb::InstanceBuilder{}.set_headless().require_api_version(1, 4).set_minimum_instance_version(1, 2).build();
    REQUIRE(ret);
    REQUIRE(mock.api_version_set_by_vkCreateInstance == VK_API_VERSION_1_4);
    REQUIRE(ret.value().api_version == VK_API_VERSION_1_4);
    REQUIRE(ret.value().instance_version == VK_API_VERSION_1_2);
    auto pd_ret = vkb::PhysicalDeviceSelector{ ret.value() }.select();
    REQUIRE(pd_ret);
    REQUIRE(pd_ret.value().properties.deviceID == 2);
}
TEST_CASE("Minimum instance API version using macros", "[VkBootstrap.api_version_using_macros]") {
    VulkanMock& mock = get_and_setup_default();
    mock.should_save_api_version = true;
    mock.instance_api_version = VK_API_VERSION_1_2;
    mock.physical_devices_details[0].properties.apiVersion = VK_API_VERSION_1_3;
    mock.physical_devices_details[0].properties.deviceID = 1;
    add_basic_physical_device(mock);
    mock.physical_devices_details[1].properties.apiVersion = VK_API_VERSION_1_4;
    mock.physical_devices_details[1].properties.deviceID = 2;

    auto ret = vkb::InstanceBuilder{}
                   .set_headless()
                   .require_api_version(VK_MAKE_API_VERSION(0, 1, 4, 0))
                   .set_minimum_instance_version(VK_MAKE_API_VERSION(0, 1, 2, 0))
                   .build();
    REQUIRE(ret);
    REQUIRE(ret.value().api_version == VK_API_VERSION_1_4);
    REQUIRE(ret.value().instance_version == VK_API_VERSION_1_2);
    REQUIRE(mock.api_version_set_by_vkCreateInstance == VK_API_VERSION_1_4);
    auto pd_ret = vkb::PhysicalDeviceSelector{ ret.value() }.select();
    REQUIRE(pd_ret);
    REQUIRE(pd_ret.value().properties.deviceID == 2);
}
TEST_CASE("Required Instance API version error codes", "[VkBootstrap.required_api_version]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_api_version = VK_API_VERSION_1_0;

    auto ret_1_0 = vkb::InstanceBuilder{}.set_headless().require_api_version(1, 0).build();
    REQUIRE(ret_1_0);

    auto ret_1_1 = vkb::InstanceBuilder{}.set_headless().require_api_version(1, 1).build();
    REQUIRE(ret_1_1.error() == vkb::InstanceError::vulkan_version_1_1_unavailable);

    auto ret_1_2 = vkb::InstanceBuilder{}.set_headless().require_api_version(1, 2).build();
    REQUIRE(ret_1_2.error() == vkb::InstanceError::vulkan_version_1_2_unavailable);

    auto ret_1_3 = vkb::InstanceBuilder{}.set_headless().require_api_version(1, 3).build();
    REQUIRE(ret_1_3.error() == vkb::InstanceError::vulkan_version_1_3_unavailable);

    auto ret_1_4 = vkb::InstanceBuilder{}.set_headless().require_api_version(1, 4).build();
    REQUIRE(ret_1_4.error() == vkb::InstanceError::vulkan_version_1_4_unavailable);
}
TEST_CASE("Minimum instance API version without required api version", "[VkBootstrap.set_minimum_instance_version]") {
    VulkanMock& mock = get_and_setup_default();
    mock.should_save_api_version = true;
    mock.instance_api_version = VK_API_VERSION_1_2;

    auto ret = vkb::InstanceBuilder{}.set_headless().set_minimum_instance_version(1, 2).build();
    REQUIRE(ret);
    REQUIRE(ret.value().api_version == VK_API_VERSION_1_2);
    REQUIRE(ret.value().instance_version == VK_API_VERSION_1_2);
    REQUIRE(mock.api_version_set_by_vkCreateInstance == VK_API_VERSION_1_2);
}
TEST_CASE("Too low instance API version without required api version too ", "[VkBootstrap.set_minimum_instance_version_too_low]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_api_version = VK_API_VERSION_1_1;

    auto ret = vkb::InstanceBuilder{}.set_headless().set_minimum_instance_version(1, 2).build();
    REQUIRE(!ret.has_value());
}
TEST_CASE("Instance API version error codes", "[VkBootstrap.set_minimum_instance_version_error_codes]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_api_version = VK_API_VERSION_1_0;

    auto ret_1_0 = vkb::InstanceBuilder{}.set_headless().set_minimum_instance_version(1, 0).build();
    REQUIRE(ret_1_0);

    auto ret_1_1 = vkb::InstanceBuilder{}.set_headless().set_minimum_instance_version(1, 1).build();
    REQUIRE(ret_1_1.error() == vkb::InstanceError::vulkan_version_1_1_unavailable);

    auto ret_1_2 = vkb::InstanceBuilder{}.set_headless().set_minimum_instance_version(1, 2).build();
    REQUIRE(ret_1_2.error() == vkb::InstanceError::vulkan_version_1_2_unavailable);

    auto ret_1_3 = vkb::InstanceBuilder{}.set_headless().set_minimum_instance_version(1, 3).build();
    REQUIRE(ret_1_3.error() == vkb::InstanceError::vulkan_version_1_3_unavailable);

    auto ret_1_4 = vkb::InstanceBuilder{}.set_headless().set_minimum_instance_version(1, 4).build();
    REQUIRE(ret_1_4.error() == vkb::InstanceError::vulkan_version_1_4_unavailable);
}

TEST_CASE("Minimum instance API version lower than VkPhysicalDevice", "[VkBootstrap.api_version]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_api_version = VK_API_VERSION_1_2;
    mock.physical_devices_details[0].properties.apiVersion = VK_API_VERSION_1_3;
    mock.physical_devices_details[0].properties.deviceID = 1;
    {
        auto ret = vkb::InstanceBuilder{}.set_headless().require_api_version(1, 4).set_minimum_instance_version(1, 2).build();
        REQUIRE(ret);
        auto pd_ret = vkb::PhysicalDeviceSelector{ ret.value() }.select();
        REQUIRE(pd_ret.error() == vkb::PhysicalDeviceError::no_suitable_device);
    }
}
TEST_CASE("Querying Required Extension Features but with 1.0", "[VkBootstrap.select_features]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_extensions.push_back(get_extension_properties("VK_KHR_get_physical_device_properties2"));
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("VK_EXT_descriptor_indexing"));
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("VK_KHR_maintenance3"));
    auto mock_descriptor_indexing_features = VkPhysicalDeviceDescriptorIndexingFeaturesEXT{};
    mock_descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
    mock_descriptor_indexing_features.runtimeDescriptorArray = true;
    mock.physical_devices_details[0].features_pNextChain.emplace_back(
        create_serialized_struct_from_object(mock_descriptor_indexing_features));
    GIVEN("A working instance") {
        auto instance = get_headless_instance();
        // Requires a device that supports runtime descriptor arrays via descriptor indexing extension.
        {
            VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptor_indexing_features{};
            descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
            descriptor_indexing_features.runtimeDescriptorArray = true;

            vkb::PhysicalDeviceSelector selector(instance);
            auto phys_dev_ret = selector.add_required_extension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)
                                    .add_required_extension(VK_KHR_MAINTENANCE3_EXTENSION_NAME)
                                    .add_required_extension_features(descriptor_indexing_features)
                                    .select();
            REQUIRE(phys_dev_ret.has_value());

            vkb::DeviceBuilder device_builder(phys_dev_ret.value());
            auto device_ret = device_builder.build();
            REQUIRE(device_ret.has_value());
            vkb::destroy_device(device_ret.value());
        }
        vkb::destroy_instance(instance);
    }
}
TEST_CASE("Querying Required Extension Features", "[VkBootstrap.select_features]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_extensions.push_back(get_extension_properties("VK_KHR_get_physical_device_properties2"));
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("VK_EXT_descriptor_indexing"));
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("VK_KHR_maintenance3"));
    auto mock_descriptor_indexing_features = VkPhysicalDeviceDescriptorIndexingFeaturesEXT{};
    mock_descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
    mock_descriptor_indexing_features.runtimeDescriptorArray = true;
    mock.physical_devices_details[0].features_pNextChain.emplace_back(
        create_serialized_struct_from_object(mock_descriptor_indexing_features));
    GIVEN("A working instance") {
        auto instance = get_headless_instance();
        // Requires a device that supports runtime descriptor arrays via descriptor indexing extension.
        {
            VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptor_indexing_features{};
            descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
            descriptor_indexing_features.runtimeDescriptorArray = true;

            vkb::PhysicalDeviceSelector selector(instance);
            auto phys_dev_ret = selector.add_required_extension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)
                                    .add_required_extension(VK_KHR_MAINTENANCE3_EXTENSION_NAME)
                                    .add_required_extension_features(descriptor_indexing_features)
                                    .select();
            REQUIRE(phys_dev_ret.has_value());

            REQUIRE(phys_dev_ret.value().are_extension_features_present(descriptor_indexing_features));

            vkb::DeviceBuilder device_builder(phys_dev_ret.value());
            auto device_ret = device_builder.build();
            REQUIRE(device_ret.has_value());
            vkb::destroy_device(device_ret.value());
        }
        vkb::destroy_instance(instance);
    }
}
TEST_CASE("Error from Required Extension Features", "[VkBootstrap.missing_features]") {
    VulkanMock& mock = get_and_setup_default();
    add_basic_physical_device(mock); // add two devices
    mock.instance_extensions.push_back(get_extension_properties("VK_KHR_get_physical_device_properties2"));
    std::string gpu_name_0 = "Doesn't do Squat (for good reason)";
    std::copy_n(gpu_name_0.c_str(), gpu_name_0.size(), mock.physical_devices_details[0].properties.deviceName);
    std::string gpu_name_1 = "Fake GPU Electric Buckaroo";
    std::copy_n(gpu_name_1.c_str(), gpu_name_1.size(), mock.physical_devices_details[1].properties.deviceName);
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("VK_EXT_descriptor_indexing"));
    mock.physical_devices_details[1].extensions.push_back(get_extension_properties("VK_KHR_maintenance3"));
    auto mock_descriptor_indexing_features_0 = VkPhysicalDeviceDescriptorIndexingFeaturesEXT{};
    mock_descriptor_indexing_features_0.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
    mock_descriptor_indexing_features_0.runtimeDescriptorArray = true;
    mock_descriptor_indexing_features_0.descriptorBindingPartiallyBound = true;
    mock.physical_devices_details[0].features_pNextChain.emplace_back(
        create_serialized_struct_from_object(mock_descriptor_indexing_features_0));
    auto mock_descriptor_indexing_features_1 = VkPhysicalDeviceDescriptorIndexingFeaturesEXT{};
    mock_descriptor_indexing_features_1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
    mock_descriptor_indexing_features_1.descriptorBindingSampledImageUpdateAfterBind = true;
    mock_descriptor_indexing_features_1.descriptorBindingStorageBufferUpdateAfterBind = true;
    mock.physical_devices_details[1].features_pNextChain.emplace_back(
        create_serialized_struct_from_object(mock_descriptor_indexing_features_1));
    GIVEN("A working instance") {
        auto instance = get_headless_instance();
        {
            VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptor_indexing_features{};
            descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
            descriptor_indexing_features.runtimeDescriptorArray = true;
            descriptor_indexing_features.descriptorBindingSampledImageUpdateAfterBind = true;
            vkb::PhysicalDeviceSelector selector(instance);
            auto phys_dev_ret = selector.add_required_extension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)
                                    .add_required_extension(VK_KHR_MAINTENANCE3_EXTENSION_NAME)
                                    .add_required_extension_features(descriptor_indexing_features)
                                    .select();
            REQUIRE(!phys_dev_ret.has_value());
            auto reasons = phys_dev_ret.detailed_failure_reasons();
            // Only reports the missing extensions
            REQUIRE(reasons.size() == 2);
            REQUIRE(reasons.at(0) == "Physical Device " + gpu_name_0 +
                                         " not selected due to: Device extension VK_KHR_maintenance3 not supported");
            REQUIRE(reasons.at(1) == "Physical Device " + gpu_name_1 + " not selected due to: Device extension VK_EXT_descriptor_indexing not supported");
        }
        {
            VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptor_indexing_features{};
            descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
            descriptor_indexing_features.runtimeDescriptorArray = true;
            descriptor_indexing_features.descriptorBindingSampledImageUpdateAfterBind = true;
            vkb::PhysicalDeviceSelector selector(instance);
            auto phys_dev_ret = selector.add_required_extension_features(descriptor_indexing_features).select();
            REQUIRE(!phys_dev_ret.has_value());
            auto reasons = phys_dev_ret.detailed_failure_reasons();
            // Reports the missing features
            REQUIRE(reasons.size() == 2);
            REQUIRE(reasons.at(0) ==
                    "Physical Device " + gpu_name_0 +
                        " not selected due to: Missing feature "
                        "VkPhysicalDeviceDescriptorIndexingFeatures::descriptorBindingSampledImageUpdateAfterBind");
            REQUIRE(reasons.at(1) == "Physical Device " + gpu_name_1 +
                                         " not selected due to: Missing feature "
                                         "VkPhysicalDeviceDescriptorIndexingFeatures::runtimeDescriptorArray");
        }
        vkb::destroy_instance(instance);
    }
}

TEST_CASE("Adding Optional Extension Features", "[VkBootstrap.enable_features_if_present]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_api_version = VK_API_VERSION_1_1;
    mock.physical_devices_details[0].properties.apiVersion = VK_API_VERSION_1_1;
    mock.instance_extensions.push_back(get_extension_properties("VK_KHR_get_physical_device_properties2"));
    auto vulkan_10_features = VkPhysicalDeviceFeatures{};
    vulkan_10_features.multiViewport = true;
    mock.physical_devices_details[0].features = vulkan_10_features;

    auto vulkan_11_features = VkPhysicalDeviceVulkan11Features{};
    vulkan_11_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    vulkan_11_features.shaderDrawParameters = true;
    mock.physical_devices_details[0].features_pNextChain.emplace_back(create_serialized_struct_from_object(vulkan_11_features));

    auto vulkan_12_features = VkPhysicalDeviceVulkan12Features{};
    vulkan_12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    vulkan_12_features.bufferDeviceAddress = true;
    mock.physical_devices_details[0].features_pNextChain.emplace_back(create_serialized_struct_from_object(vulkan_12_features));


    GIVEN("A working instance and physical device which has a VkPhysicalDeviceVulkan12Features in its features pNext "
          "chain") {
        auto instance = get_headless_instance();

        VkPhysicalDeviceVulkan12Features physical_device_features_12{};
        physical_device_features_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        physical_device_features_12.bufferDeviceAddress = true;

        vkb::PhysicalDeviceSelector selector(instance);
        selector.add_required_extension_features(physical_device_features_12);
        {

            SECTION("Require enable_features_if_present to work with an empty feature struct") {
                auto phys_dev = selector.select().value();
                VkPhysicalDeviceFeatures phys_dev_features_empty{};
                REQUIRE(phys_dev.enable_features_if_present(phys_dev_features_empty));
                auto device = vkb::DeviceBuilder(phys_dev).build().value();
                vkb::destroy_device(device);
            }
            SECTION("Require enable_features_if_present to fail with an unsupported feature struct") {
                auto phys_dev = selector.select().value();
                VkPhysicalDeviceFeatures phys_dev_features_bad{};
                phys_dev_features_bad.depthClamp = true;
                REQUIRE(!phys_dev.enable_features_if_present(phys_dev_features_bad));
                auto device = vkb::DeviceBuilder(phys_dev).build().value();
                REQUIRE(!mock.physical_devices_details.at(0).created_device_details.at(0).features.depthClamp);
                vkb::destroy_device(device);
            }
            SECTION("Require enable_features_if_present to work with a supported feature struct") {
                auto phys_dev = selector.select().value();
                VkPhysicalDeviceFeatures phys_dev_features_good{};
                phys_dev_features_good.multiViewport = true;
                REQUIRE(phys_dev.enable_features_if_present(phys_dev_features_good));
                auto device = vkb::DeviceBuilder(phys_dev).build().value();
                REQUIRE(mock.physical_devices_details.at(0).created_device_details.at(0).features.multiViewport);
                vkb::destroy_device(device);
            }

            SECTION("Require enable_extension_features_if_present to work with an empty 1.1 feature struct") {
                auto phys_dev = selector.select().value();
                VkPhysicalDeviceVulkan11Features phys_dev_vulkan_11_features{};
                phys_dev_vulkan_11_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
                REQUIRE(phys_dev.enable_extension_features_if_present(phys_dev_vulkan_11_features));
                auto device = vkb::DeviceBuilder(phys_dev).build().value();
                vkb::destroy_device(device);
            }
            SECTION("Require enable_extension_features_if_present to fail with an unsupported 1.1 feature struct") {
                auto phys_dev = selector.select().value();
                VkPhysicalDeviceVulkan11Features phys_dev_vulkan_11_features{};
                phys_dev_vulkan_11_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
                phys_dev_vulkan_11_features.multiview = true;
                REQUIRE(!phys_dev.enable_extension_features_if_present(phys_dev_vulkan_11_features));
                auto device = vkb::DeviceBuilder(phys_dev).build().value();
                auto* s = reinterpret_cast<VkPhysicalDeviceVulkan12Features*>(
                    mock.physical_devices_details.at(0).created_device_details.at(0).features_pNextChain.at(0).data());
                REQUIRE(s->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES);
                vkb::destroy_device(device);
            }
            SECTION("Require enable_extension_features_if_present to work with a supported 1.1 feature struct") {
                auto phys_dev = selector.select().value();
                VkPhysicalDeviceVulkan11Features phys_dev_vulkan_11_features{};
                phys_dev_vulkan_11_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
                phys_dev_vulkan_11_features.shaderDrawParameters = true;
                REQUIRE(phys_dev.enable_extension_features_if_present(phys_dev_vulkan_11_features));
                auto device = vkb::DeviceBuilder(phys_dev).build().value();
                auto* s = reinterpret_cast<VkPhysicalDeviceVulkan11Features*>(
                    mock.physical_devices_details.at(0).created_device_details.at(0).features_pNextChain.at(1).data());
                REQUIRE(s->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES);
                REQUIRE(s->shaderDrawParameters);
                vkb::destroy_device(device);
            }
        }
        vkb::destroy_instance(instance);
    }
}

TEST_CASE("Passing vkb classes to Vulkan handles", "[VkBootstrap.pass_class_to_handle]") {
    VulkanMock& mock = get_and_setup_default();
    auto surface = mock.get_new_surface(get_basic_surface_details());
    GIVEN("A working instance") {
        auto instance = get_instance();

        // Check if we can get instance functions.
        PFN_vkVoidFunction instanceFunction = instance.fp_vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices"); // validation layers should be provided.
        REQUIRE(instanceFunction);

        vkb::PhysicalDeviceSelector physicalDeviceSelector(instance);
        auto physicalDevice =
            physicalDeviceSelector.add_required_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME).set_surface(surface).select();
        REQUIRE(physicalDevice.has_value());
        vkb::DeviceBuilder deviceBuilder(physicalDevice.value());
        auto device = deviceBuilder.build();
        REQUIRE(device.has_value());

        // Check if we can get a device function address, passing vkb::Device to the function.
        PFN_vkVoidFunction deviceFunction = instance.fp_vkGetDeviceProcAddr(device.value(), "vkAcquireNextImageKHR");
        REQUIRE(deviceFunction);
    }
}

TEST_CASE("Querying Required Extension Features in 1.1", "[VkBootstrap.version]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_extensions.push_back(get_extension_properties("VK_KHR_get_physical_device_properties2"));
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("VK_EXT_descriptor_indexing"));
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("VK_KHR_maintenance3"));
    auto mock_descriptor_indexing_features = VkPhysicalDeviceDescriptorIndexingFeaturesEXT{};
    mock_descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    mock_descriptor_indexing_features.runtimeDescriptorArray = true;
    mock.physical_devices_details[0].features_pNextChain.emplace_back(
        create_serialized_struct_from_object(mock_descriptor_indexing_features));
    GIVEN("A working instance") {
        auto instance = get_headless_instance();
        SECTION("Requires a device that supports runtime descriptor arrays via descriptor indexing extension.") {
            VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptor_indexing_features{};
            descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
            descriptor_indexing_features.runtimeDescriptorArray = true;

            vkb::PhysicalDeviceSelector selector(instance);
            auto phys_dev_ret = selector.add_required_extension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)
                                    .add_required_extension(VK_KHR_MAINTENANCE3_EXTENSION_NAME)
                                    .add_required_extension_features(descriptor_indexing_features)
                                    .select();
            REQUIRE(phys_dev_ret.has_value());

            REQUIRE(phys_dev_ret.value().are_extension_features_present(descriptor_indexing_features));

            VkPhysicalDeviceASTCDecodeFeaturesEXT astc_features{};
            astc_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT;
            astc_features.decodeModeSharedExponent = true;
            REQUIRE(!phys_dev_ret.value().are_extension_features_present(astc_features));

            VkPhysicalDeviceDescriptorIndexingFeaturesEXT unsupported_descriptor_indexing_features{};
            unsupported_descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
            unsupported_descriptor_indexing_features.runtimeDescriptorArray = true;
            unsupported_descriptor_indexing_features.descriptorBindingUniformTexelBufferUpdateAfterBind = true;
            REQUIRE(!phys_dev_ret.value().are_extension_features_present(unsupported_descriptor_indexing_features));

            vkb::DeviceBuilder device_builder(phys_dev_ret.value());
            auto device_ret = device_builder.build();
            REQUIRE(device_ret.has_value());
            vkb::destroy_device(device_ret.value());
        }
        SECTION("Add a custom VkPhysicalDeviceFeatures2 pNext chain to the DeviceBuilder, without using "
                "add_required_extension_features()") {
            VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptor_indexing_features{};
            descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
            descriptor_indexing_features.runtimeDescriptorArray = true;

            vkb::PhysicalDeviceSelector selector(instance);
            auto phys_dev_ret = selector.add_required_extension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)
                                    .add_required_extension(VK_KHR_MAINTENANCE3_EXTENSION_NAME)
                                    .select();
            REQUIRE(phys_dev_ret.has_value());

            REQUIRE(!phys_dev_ret.value().are_extension_features_present(descriptor_indexing_features));

            VkPhysicalDeviceASTCDecodeFeaturesEXT astc_features{};
            astc_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT;
            astc_features.decodeModeSharedExponent = true;
            REQUIRE(!phys_dev_ret.value().are_extension_features_present(astc_features));

            VkPhysicalDeviceFeatures2 phys_dev_feats2{};
            phys_dev_feats2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            phys_dev_feats2.pNext = reinterpret_cast<void*>(&descriptor_indexing_features);

            vkb::DeviceBuilder device_builder(phys_dev_ret.value());
            auto device_ret = device_builder.add_pNext(&phys_dev_feats2).build();
            REQUIRE(device_ret.has_value());
            vkb::destroy_device(device_ret.value());
        }
        SECTION("Try to add a custom VkPhysicalDeviceFeatures2 pNext chain to the DeviceBuilder, resulting in an "
                "error") {
            VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptor_indexing_features{};
            descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
            descriptor_indexing_features.runtimeDescriptorArray = true;

            vkb::PhysicalDeviceSelector selector(instance);
            auto phys_dev_ret = selector.add_required_extension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)
                                    .add_required_extension(VK_KHR_MAINTENANCE3_EXTENSION_NAME)
                                    .add_required_extension_features(descriptor_indexing_features)
                                    .select();
            REQUIRE(phys_dev_ret.has_value());

            REQUIRE(phys_dev_ret.value().are_extension_features_present(descriptor_indexing_features));

            VkPhysicalDeviceFeatures2 phys_dev_feats2{};
            phys_dev_feats2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            phys_dev_feats2.pNext = reinterpret_cast<void*>(&descriptor_indexing_features);

            vkb::DeviceBuilder device_builder(phys_dev_ret.value());
            auto device_ret = device_builder.add_pNext(&phys_dev_feats2).build();
            REQUIRE(device_ret.error() ==
                    vkb::DeviceError::VkPhysicalDeviceFeatures2_in_pNext_chain_while_using_add_required_extension_features);
        }

        vkb::destroy_instance(instance);
    }
}

TEST_CASE("Querying Vulkan 1.1 and 1.2 features", "[VkBootstrap.version]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_api_version = VK_API_VERSION_1_2;
    mock.physical_devices_details[0].properties.apiVersion = VK_API_VERSION_1_2;

    auto mock_vulkan_11_features = VkPhysicalDeviceVulkan11Features{};
    mock_vulkan_11_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    mock_vulkan_11_features.multiview = true;
    mock.physical_devices_details[0].features_pNextChain.emplace_back(create_serialized_struct_from_object(mock_vulkan_11_features));

    auto mock_vulkan_12_features = VkPhysicalDeviceVulkan12Features{};
    mock_vulkan_12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    mock_vulkan_12_features.bufferDeviceAddress = true;
    mock.physical_devices_details[0].features_pNextChain.emplace_back(create_serialized_struct_from_object(mock_vulkan_12_features));

    GIVEN("A working instance") {
        auto instance = get_headless_instance(2); // make sure we use 1.2
        SECTION("Requires a device that supports multiview and bufferDeviceAddress") {
            VkPhysicalDeviceVulkan11Features features_11{};
            features_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
            features_11.multiview = true;
            VkPhysicalDeviceVulkan12Features features_12{};
            features_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
            features_12.bufferDeviceAddress = true;

            vkb::PhysicalDeviceSelector selector(instance);
            auto phys_dev_ret = selector.set_required_features_11(features_11).set_required_features_12(features_12).select();
            REQUIRE(phys_dev_ret.has_value());

            vkb::DeviceBuilder device_builder(phys_dev_ret.value());
            auto device_ret = device_builder.build();
            REQUIRE(device_ret.has_value());
            auto* s1 = reinterpret_cast<VkPhysicalDeviceVulkan11Features*>(
                mock.physical_devices_details.at(0).created_device_details.at(0).features_pNextChain.at(0).data());
            REQUIRE(s1->multiview);
            auto* s2 = reinterpret_cast<VkPhysicalDeviceVulkan12Features*>(
                mock.physical_devices_details.at(0).created_device_details.at(0).features_pNextChain.at(1).data());
            REQUIRE(s2->bufferDeviceAddress);

            vkb::destroy_device(device_ret.value());
        }
        mock.instance_api_version = VK_API_VERSION_1_1;
        mock.physical_devices_details[0].properties.apiVersion = VK_API_VERSION_1_1;
        SECTION("protectedMemory should NOT be supported") {
            VkPhysicalDeviceVulkan11Features features_11{};
            features_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
            features_11.protectedMemory = true;

            vkb::PhysicalDeviceSelector selector(instance);
            auto phys_dev_ret = selector.set_required_features_11(features_11).select();
            REQUIRE(!phys_dev_ret.has_value());
        }
        vkb::destroy_instance(instance);
    }
}

TEST_CASE("Add required features in multiple calls", "[VkBootstrap.required_features]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_api_version = VK_API_VERSION_1_2;
    mock.physical_devices_details[0].properties.apiVersion = VK_API_VERSION_1_2;
    mock.physical_devices_details[0].features.independentBlend = true;
    mock.physical_devices_details[0].features.shaderInt64 = true;

    GIVEN("A working instance") {
        auto instance = get_headless_instance(1); // make sure we use 1.1
        SECTION("Requires a device that supports independentBlend and shaderInt64") {
            VkPhysicalDeviceFeatures features1{};
            features1.independentBlend = true;
            VkPhysicalDeviceFeatures features2{};
            features2.shaderInt64 = true;

            vkb::PhysicalDeviceSelector selector(instance);
            auto phys_dev_ret = selector.set_required_features(features1).set_required_features(features2).select();
            REQUIRE(phys_dev_ret.has_value());

            vkb::DeviceBuilder device_builder(phys_dev_ret.value());
            auto device_ret = device_builder.build();
            REQUIRE(device_ret.has_value());

            REQUIRE(mock.physical_devices_details.at(0).created_device_details.at(0).features.independentBlend);
            REQUIRE(mock.physical_devices_details.at(0).created_device_details.at(0).features.shaderInt64);

            vkb::destroy_device(device_ret.value());
        }
        vkb::destroy_instance(instance);
    }
}

TEST_CASE("Add required extension features in multiple calls", "[VkBootstrap.required_features]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_api_version = VK_API_VERSION_1_2;
    mock.physical_devices_details[0].properties.apiVersion = VK_API_VERSION_1_2;

    auto mock_vulkan_11_features = VkPhysicalDeviceVulkan11Features{};
    mock_vulkan_11_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    mock_vulkan_11_features.multiview = true;
    mock_vulkan_11_features.samplerYcbcrConversion = true;
    mock.physical_devices_details[0].features_pNextChain.emplace_back(create_serialized_struct_from_object(mock_vulkan_11_features));

    GIVEN("A working instance") {
        auto instance = get_headless_instance(1); // make sure we use 1.1
        SECTION("Requires a device that supports multiview and samplerYcbcrConversion") {
            VkPhysicalDeviceVulkan11Features features1{};
            features1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
            features1.multiview = true;

            VkPhysicalDeviceVulkan11Features features2{};
            features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
            features2.samplerYcbcrConversion = true;

            vkb::PhysicalDeviceSelector selector(instance);
            auto phys_dev_ret = selector.set_required_features_11(features1).set_required_features_11(features2).select();
            REQUIRE(phys_dev_ret.has_value());

            vkb::DeviceBuilder device_builder(phys_dev_ret.value());
            auto device_ret = device_builder.build();
            REQUIRE(device_ret.has_value());
            auto* s1 = reinterpret_cast<VkPhysicalDeviceVulkan11Features*>(
                mock.physical_devices_details.at(0).created_device_details.at(0).features_pNextChain.at(0).data());
            REQUIRE(s1->multiview);
            REQUIRE(s1->samplerYcbcrConversion);

            vkb::destroy_device(device_ret.value());
        }
        vkb::destroy_instance(instance);
    }
}

// Simple Rule of 3 type
class Sample {
    public:
    Sample() { data = new int; }
    Sample(Sample const& sample) {
        data = new int;
        *data = *sample.data;
    }
    Sample& operator=(Sample const& sample) {

        *data = *sample.data;
        return *this;
    }

    virtual ~Sample() { delete data; }

    private:
    int* data;
};

TEST_CASE("Check vkb::Result", "[VkBootstrap.Result]") {
    {
        vkb::Result<std::vector<float>> a{ std::vector<float>(30, 3.0f) };
        auto b = std::move(a);
        b.value().push_back(5.0f);
        vkb::Result<std::vector<float>> c{ std::move(b) };
        c.value().push_back(7.0f);
        auto d = std::move(c);
        d.value().push_back(9.0f);
    }
    {
        vkb::Result<std::unique_ptr<float>> a{ std::make_unique<float>(3.0f) };
        vkb::Result<std::unique_ptr<float>> b{ std::make_unique<float>(3.0f) };
        b = std::move(a);
        *b.value().get() = 5.0f;
        vkb::Result<std::unique_ptr<float>> c{ std::move(b) };
        *c.value().get() = 7.0f;
        auto d = std::move(c);
        *d.value().get() = 9.0f;
    }
    {

        vkb::Result<Sample> result(Sample{});
        vkb::Result<Sample> anotherResult(std::move(result));
        vkb::Result<Sample> c = result;
        vkb::Result<Sample> d(Sample{});
        d = c;
    }

    REQUIRE_NOTHROW(vkb::Result<Sample>(Sample{})->operator=(Sample{}));
    REQUIRE_NOTHROW((*vkb::Result<Sample>(Sample{})).operator=(Sample{}));
    REQUIRE_NOTHROW(vkb::Result<Sample>(Sample{}).value());
    REQUIRE_THROWS_AS(vkb::Result<Sample>(Sample{}).error(), std::bad_variant_access);
    REQUIRE_THROWS_AS(vkb::Result<Sample>(Sample{}).vk_result(), std::bad_variant_access);
    REQUIRE_THROWS_AS(vkb::Result<Sample>(Sample{}).full_error(), std::bad_variant_access);
    REQUIRE(!vkb::Result<Sample>(Sample{}).matches_error<int>(0));
    REQUIRE(!vkb::Result<Sample>(Sample{}).matches_error<int>(1));
    REQUIRE(vkb::Result<Sample>(Sample{}).has_value());
    REQUIRE(vkb::Result<Sample>(Sample{}));

    REQUIRE_THROWS_AS(vkb::Result<Sample>(vkb::Error{})->operator=(Sample{}), std::bad_variant_access);
    REQUIRE_THROWS_AS((*vkb::Result<Sample>(vkb::Error{})).operator=(Sample{}), std::bad_variant_access);
    REQUIRE_THROWS_AS(vkb::Result<Sample>(vkb::Error{}).value(), std::bad_variant_access);
    REQUIRE_NOTHROW(vkb::Result<Sample>(vkb::Error{}).error());
    REQUIRE_NOTHROW(vkb::Result<Sample>(vkb::Error{}).vk_result());
    REQUIRE_NOTHROW(vkb::Result<Sample>(vkb::Error{}).full_error());
    REQUIRE(vkb::Result<Sample>(vkb::Error{}).matches_error<int>(0));
    REQUIRE(!vkb::Result<Sample>(vkb::Error{}).matches_error<int>(1));
    REQUIRE(!vkb::Result<Sample>(vkb::Error{}).has_value());
    REQUIRE(!vkb::Result<Sample>(vkb::Error{}));
}

TEST_CASE("Test VK_EXT_layer_settings", "[VkBoostrap.LayerSettings]") {
    VulkanMock& mock = get_and_setup_default();

    SECTION("Missing extension") {
        VkBool32 vkTrue = VK_TRUE;
        const VkLayerSettingEXT layer_setting = {
            "VK_LAYER_KHRONOS_validation", "validate_core", VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &vkTrue
        };

        auto ret = vkb::InstanceBuilder{}.set_headless().add_layer_setting(layer_setting).build();
        REQUIRE(ret.error() == vkb::InstanceError::requested_extensions_not_present);
    }

    mock.instance_extensions.push_back(get_extension_properties(VK_EXT_LAYER_SETTINGS_EXTENSION_NAME));

    SECTION("Set layer setting") {
        VkBool32 vkTrue = VK_TRUE;
        const VkLayerSettingEXT layer_setting = {
            "VK_LAYER_KHRONOS_validation", "validate_core", VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &vkTrue
        };

        auto ret = vkb::InstanceBuilder{}.set_headless().add_layer_setting(layer_setting).build();
        REQUIRE(ret);
    }
}
TEST_CASE("Test span functions", "[VkBootstrap.cpp20]") {
    VulkanMock& mock = get_and_setup_default();
    auto surface = mock.get_new_surface(get_basic_surface_details());

    mock.instance_extensions.push_back(get_extension_properties("VK_EXT_debug_utils"));
    mock.instance_extensions.push_back(get_extension_properties("VK_KHR_surface"));
    mock.instance_extensions.push_back(get_extension_properties("VK_KHR_get_physical_device_properties2"));
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("PhysDevExt0"));
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("PhysDevExt1"));
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("PhysDevExt2"));
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("PhysDevExt3"));
    vkb::InstanceBuilder builder;
    SECTION("InstanceBuilder::enable_extension") {
        std::array exts = { "VK_EXT_debug_utils", "VK_KHR_surface", "VK_KHR_get_physical_device_properties2" };
        builder.enable_extensions(exts);
        builder.enable_extensions(3, exts.data());
    }
    auto instance_ret = builder.build();
    REQUIRE(instance_ret.has_value());
    vkb::PhysicalDeviceSelector selector(instance_ret.value());
    SECTION("PhysicalDeviceSelector::add_required_extensions") {
        std::array exts1 = { "PhysDevExt0", "PhysDevExt1" };
        selector.add_required_extensions(exts1);
        selector.add_required_extensions(2, exts1.data());
    }
    auto physical_device_ret = selector.set_surface(surface).select();
    REQUIRE(physical_device_ret.has_value());
    auto physical_device = physical_device_ret.value();
    SECTION("PhysicalDevice::enable_extensions_if_present") {
        std::array exts2 = { "PhysDevExt2", "PhysDevExt3" };
        physical_device.enable_extensions_if_present(exts2);
        physical_device.enable_extensions_if_present(2, exts2.data());
    }

    vkb::DeviceBuilder device_builder{ physical_device };
    SECTION("CustomQueueDescription and custom_queue_setup") {
        vkb::CustomQueueDescription cqd{ 4, { 1.0, 2.0, 3.0, 4.0 } };
        std::array cqds = { cqd, vkb::CustomQueueDescription{ 3, { 4.0, 3.0, 2.0 } } };
        device_builder.custom_queue_setup(cqds);
        device_builder.custom_queue_setup(2, cqds.data());
    }
    auto device_ret = device_builder.build();
    REQUIRE(device_ret.has_value());
    vkb::SwapchainBuilder swapchain_builder{ device_ret.value() };
    auto swapchain_ret = swapchain_builder.build();
    REQUIRE(swapchain_ret.value());
    SECTION("destroy_image_views") {
        auto image_views_ret = swapchain_ret.value().get_image_views();
        REQUIRE(image_views_ret.has_value());
        auto image_view_vec = image_views_ret.value();
        std::array<VkImageView, 3> image_views{};
        for (size_t i = 0; i < 3; i++)
            image_views[i] = image_view_vec[i];
        swapchain_ret.value().destroy_image_views(image_views);
    }
    SECTION("destroy_image_views with count & data pointer") {
        auto image_views_ret = swapchain_ret.value().get_image_views();
        REQUIRE(image_views_ret.has_value());
        auto image_view_vec = image_views_ret.value();
        swapchain_ret.value().destroy_image_views(3, image_view_vec.data());
    }
}
