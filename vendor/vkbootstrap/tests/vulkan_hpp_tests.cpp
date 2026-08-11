#include "vulkan_library_loader.hpp"

#include "vulkan_mock.hpp"
#include "vulkan_mock_setup.hpp"

// Because vulkan-hpp isn't clean of warnings, disable them while including it
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
#define VULKAN_HPP_NO_SPACESHIP_OPERATOR 
#include <vulkan/vulkan.hpp>
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic pop
#endif

TEST_CASE("VulkanHpp Instance with surface", "[VkBootstrap.vulkan_hpp]") {
    VulkanMock& mock = get_and_setup_default();
    mock.instance_api_version = VK_API_VERSION_1_1;
    mock.physical_devices_details[0].properties.apiVersion = VK_API_VERSION_1_1;
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("VK_KHR_multiview"));
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("VK_KHR_driver_properties"));
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("VK_EXT_robustness2"));
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("VK_EXT_descriptor_indexing"));
    VkPhysicalDeviceDescriptorIndexingFeatures physical_device_descriptor_indexing_features{};
    physical_device_descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    physical_device_descriptor_indexing_features.runtimeDescriptorArray = true;
    physical_device_descriptor_indexing_features.descriptorBindingPartiallyBound = true;
    mock.physical_devices_details[0].features_pNextChain.emplace_back(create_serialized_struct_from_object(physical_device_descriptor_indexing_features));
    mock.physical_devices_details[0].extensions.push_back(get_extension_properties("VK_EXT_subgroup_size_control"));
    VkPhysicalDeviceSubgroupSizeControlFeatures physical_device_subgroup_size_control_features{};
    physical_device_subgroup_size_control_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES;
    physical_device_subgroup_size_control_features.subgroupSizeControl = true;
    physical_device_subgroup_size_control_features.computeFullSubgroups = true;
    mock.physical_devices_details[0].features_pNextChain.emplace_back(create_serialized_struct_from_object(physical_device_subgroup_size_control_features));

    auto surface = mock.get_new_surface(get_basic_surface_details());
    vk::SurfaceKHR hpp_surface{ surface };
    GIVEN("A window and a vulkan instance") {

        auto sys_info_ret = vkb::SystemInfo::get_system_info();
        REQUIRE(sys_info_ret);

        vkb::InstanceBuilder instance_builder;
        auto instance_ret = instance_builder.require_api_version(1, 1, 0).use_default_debug_messenger().build();
        REQUIRE(instance_ret);
        vkb::Instance instance = instance_ret.value();
        vk::Instance hpp_instance{ instance };

        GIVEN("A default selected physical device") {
            vkb::PhysicalDeviceSelector phys_device_selector(instance);
#if VK_USE_64_BIT_PTR_DEFINES == 1
            auto phys_device_ret = phys_device_selector.set_surface(hpp_surface).select();
#else
            auto phys_device_ret = phys_device_selector.set_surface(static_cast<VkSurfaceKHR>(hpp_surface)).select();
#endif
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
            auto phys_dev_ret =
#if VK_USE_64_BIT_PTR_DEFINES == 1
                selector
                    .set_surface(hpp_surface)
#else
                selector
                    .set_surface(static_cast<VkSurfaceKHR>(hpp_surface))
#endif
                    .add_required_extension_features(
                        vk::PhysicalDeviceSubgroupSizeControlFeatures{}.setSubgroupSizeControl(true).setComputeFullSubgroups(true))
                    .set_minimum_version(1, 0)
                    .select();

            REQUIRE(phys_dev_ret.has_value());

            REQUIRE(phys_dev_ret->is_extension_present(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME));

            const std::vector<const char*> extension_set_1 = { VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME };

            REQUIRE(phys_dev_ret->enable_extensions_if_present(extension_set_1));

            vk::PhysicalDeviceDescriptorIndexingFeatures physical_device_descriptor_indexing_features_hpp;
            physical_device_descriptor_indexing_features_hpp.setRuntimeDescriptorArray(vk::True);
            physical_device_descriptor_indexing_features_hpp.setDescriptorBindingPartiallyBound(vk::True);

            phys_dev_ret->enable_extension_features_if_present(physical_device_descriptor_indexing_features_hpp);

            auto device_ret = vkb::DeviceBuilder(phys_dev_ret.value()).build();
            REQUIRE(device_ret.has_value());
            vk::Device hpp_device{ device_ret.value().device };
            vkb::destroy_device(device_ret.value());
        }
#if VK_USE_64_BIT_PTR_DEFINES == 1
        vkb::destroy_surface(hpp_instance, hpp_surface);
#else
        vkb::destroy_surface(hpp_instance, static_cast<VkSurfaceKHR>(hpp_surface));
#endif

        vkb::destroy_instance(instance);
    }
}
