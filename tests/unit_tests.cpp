#include <catch2/catch_test_macros.hpp>

#include "VkBootstrap.cpp"


TEST_CASE("Single Queue Device", "[UnitTests.queue_selection_logic]") {
    std::vector<VkQueueFamilyProperties> families = { VkQueueFamilyProperties{
        VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 1, 0, VkExtent3D{ 1, 1, 1 } } };

    REQUIRE(0 == vkb::detail::get_first_queue_index(families, VK_QUEUE_GRAPHICS_BIT));
    REQUIRE(vkb::detail::QUEUE_INDEX_MAX_VALUE ==
            vkb::detail::get_separate_queue_index(families, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT));
    REQUIRE(vkb::detail::QUEUE_INDEX_MAX_VALUE ==
            vkb::detail::get_separate_queue_index(families, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT));
    REQUIRE(vkb::detail::QUEUE_INDEX_MAX_VALUE ==
            vkb::detail::get_dedicated_queue_index(families, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT));
    REQUIRE(vkb::detail::QUEUE_INDEX_MAX_VALUE ==
            vkb::detail::get_dedicated_queue_index(families, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT));
}

TEST_CASE("Dedicated Compute Queue, Separate Transfer", "[UnitTests.queue_selection_logic]") {
    SECTION("Dedicated Queue First") {
        std::vector<VkQueueFamilyProperties> families = {
            VkQueueFamilyProperties{
                VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 1, 0, VkExtent3D{ 1, 1, 1 } },
            VkQueueFamilyProperties{ VK_QUEUE_COMPUTE_BIT, 1, 0, VkExtent3D{ 1, 1, 1 } },
            VkQueueFamilyProperties{ VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 1, 0, VkExtent3D{ 1, 1, 1 } }
        };

        REQUIRE(0 == vkb::detail::get_first_queue_index(families, VK_QUEUE_GRAPHICS_BIT));
        REQUIRE(1 == vkb::detail::get_separate_queue_index(families, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT));
        REQUIRE(2 == vkb::detail::get_separate_queue_index(families, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT));
        REQUIRE(1 == vkb::detail::get_dedicated_queue_index(families, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT));
        REQUIRE(vkb::detail::QUEUE_INDEX_MAX_VALUE ==
                vkb::detail::get_dedicated_queue_index(families, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT));
    }
    SECTION("Dedicated Queue Last") {
        std::vector<VkQueueFamilyProperties> families = {
            VkQueueFamilyProperties{
                VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 1, 0, VkExtent3D{ 1, 1, 1 } },
            VkQueueFamilyProperties{ VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 1, 0, VkExtent3D{ 1, 1, 1 } },
            VkQueueFamilyProperties{ VK_QUEUE_COMPUTE_BIT, 1, 0, VkExtent3D{ 1, 1, 1 } }
        };

        REQUIRE(0 == vkb::detail::get_first_queue_index(families, VK_QUEUE_GRAPHICS_BIT));
        REQUIRE(2 == vkb::detail::get_separate_queue_index(families, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT));
        REQUIRE(1 == vkb::detail::get_separate_queue_index(families, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT));
        REQUIRE(2 == vkb::detail::get_dedicated_queue_index(families, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT));
        REQUIRE(vkb::detail::QUEUE_INDEX_MAX_VALUE ==
                vkb::detail::get_dedicated_queue_index(families, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT));
    }
}

TEST_CASE("Dedicated Transfer Queue, Separate Compute", "[UnitTests.queue_selection_logic]") {
    SECTION("Dedicated Queue First") {
        std::vector<VkQueueFamilyProperties> families = {
            VkQueueFamilyProperties{
                VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 1, 0, VkExtent3D{ 1, 1, 1 } },
            VkQueueFamilyProperties{ VK_QUEUE_TRANSFER_BIT, 1, 0, VkExtent3D{ 1, 1, 1 } },
            VkQueueFamilyProperties{ VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 1, 0, VkExtent3D{ 1, 1, 1 } }
        };

        REQUIRE(0 == vkb::detail::get_first_queue_index(families, VK_QUEUE_GRAPHICS_BIT));
        REQUIRE(2 == vkb::detail::get_separate_queue_index(families, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT));
        REQUIRE(1 == vkb::detail::get_separate_queue_index(families, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT));
        REQUIRE(vkb::detail::QUEUE_INDEX_MAX_VALUE ==
                vkb::detail::get_dedicated_queue_index(families, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT));
        REQUIRE(1 == vkb::detail::get_dedicated_queue_index(families, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT));
    }
    SECTION("Dedicated Queue Last") {
        std::vector<VkQueueFamilyProperties> families = {
            VkQueueFamilyProperties{
                VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 1, 0, VkExtent3D{ 1, 1, 1 } },
            VkQueueFamilyProperties{ VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 1, 0, VkExtent3D{ 1, 1, 1 } },
            VkQueueFamilyProperties{ VK_QUEUE_TRANSFER_BIT, 1, 0, VkExtent3D{ 1, 1, 1 } }
        };

        REQUIRE(0 == vkb::detail::get_first_queue_index(families, VK_QUEUE_GRAPHICS_BIT));
        REQUIRE(1 == vkb::detail::get_separate_queue_index(families, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT));
        REQUIRE(2 == vkb::detail::get_separate_queue_index(families, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT));
        REQUIRE(vkb::detail::QUEUE_INDEX_MAX_VALUE ==
                vkb::detail::get_dedicated_queue_index(families, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT));
        REQUIRE(2 == vkb::detail::get_dedicated_queue_index(families, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT));
    }
}
