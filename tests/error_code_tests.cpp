#include <catch2/catch_test_macros.hpp>

#include "VkBootstrap.h"

TEST_CASE("is_error_code_enum", "[VkBootstrap.error_code]") {
    STATIC_REQUIRE(std::is_error_code_enum<vkb::InstanceError>::value);
    STATIC_REQUIRE(std::is_error_code_enum<vkb::PhysicalDeviceError>::value);
    STATIC_REQUIRE(std::is_error_code_enum<vkb::QueueError>::value);
    STATIC_REQUIRE(std::is_error_code_enum<vkb::DeviceError>::value);
    STATIC_REQUIRE(std::is_error_code_enum<vkb::SwapchainError>::value);
}

TEST_CASE("make_error_code", "[VkBootstrap.error_code]") {
    GIVEN("An InstanceError") {
        const auto error = vkb::InstanceError::vulkan_unavailable;

        WHEN("Creating an error code from it") {
            std::error_code ec = make_error_code(error);
            THEN("The error code is equal to the original error") { REQUIRE(ec == error); }

            THEN("The error code is not equal to an unrelated error") {
                REQUIRE(ec != vkb::InstanceError::failed_create_instance);
            }

            THEN("We can get the error message") { REQUIRE(ec.message() == "vulkan_unavailable"); }
        }
    }

    GIVEN("A PhysicalDeviceError") {
        const auto error = vkb::PhysicalDeviceError::no_physical_devices_found;

        WHEN("Creating an error code from it") {
            std::error_code ec = make_error_code(error);
            THEN("The error code is equal to the original error") { REQUIRE(ec == error); }

            THEN("The error code is not equal to an unrelated error") {
                REQUIRE(ec != vkb::InstanceError::failed_create_instance);
            }

            THEN("We can get the error message") { REQUIRE(ec.message() == "no_physical_devices_found"); }
        }
    }

    GIVEN("A QueueError") {
        const auto error = vkb::QueueError::invalid_queue_family_index;

        WHEN("Creating an error code from it") {
            std::error_code ec = make_error_code(error);
            THEN("The error code is equal to the original error") { REQUIRE(ec == error); }

            THEN("The error code is not equal to an unrelated error") {
                REQUIRE(ec != vkb::InstanceError::failed_create_instance);
            }

            THEN("We can get the error message") { REQUIRE(ec.message() == "invalid_queue_family_index"); }
        }
    }

    GIVEN("A DeviceError") {
        const auto error = vkb::DeviceError::failed_create_device;

        WHEN("Creating an error code from it") {
            std::error_code ec = make_error_code(error);
            THEN("The error code is equal to the original error") { REQUIRE(ec == error); }

            THEN("The error code is not equal to an unrelated error") {
                REQUIRE(ec != vkb::InstanceError::failed_create_instance);
            }

            THEN("We can get the error message") { REQUIRE(ec.message() == "failed_create_device"); }
        }
    }

    GIVEN("A SwapchainError") {
        const auto error = vkb::SwapchainError::failed_create_swapchain;

        WHEN("Creating an error code from it") {
            std::error_code ec = make_error_code(error);
            THEN("The error code is equal to the original error") { REQUIRE(ec == error); }

            THEN("The error code is not equal to an unrelated error") {
                REQUIRE(ec != vkb::InstanceError::failed_create_instance);
            }

            THEN("We can get the error message") { REQUIRE(ec.message() == "failed_create_swapchain"); }
        }
    }
}
