cmake_minimum_required(VERSION 3.14.2)

project(IntegrationTests LANGUAGES CXX)

if (VULKAN_HEADER_VERSION_GIT_TAG)
    include(FetchContent)
    FetchContent_Declare(
        VulkanHeadersIntegrationTest
        GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers
        GIT_TAG        ${VULKAN_HEADER_VERSION_GIT_TAG}
    )
    FetchContent_MakeAvailable(VulkanHeadersIntegrationTest)

    if (NOT TARGET Vulkan::Headers)
        message(FATAL_ERROR "Vulkan::Headers target not defined")
    endif()
endif()

if (FIND_PACKAGE_VULKAN_HEADERS)
    find_package(VulkanHeaders REQUIRED)
endif()

if (FIND_PACKAGE_VULKAN)
    find_package(Vulkan REQUIRED)
endif()

if (ADD_SUBDIRECTORY_TESTING)
    add_subdirectory(../.. ${CMAKE_CURRENT_BINARY_DIR}/vk-bootstrap)
endif()

if (FIND_PACKAGE_TESTING)
    find_package(vk-bootstrap CONFIG REQUIRED)
endif()

if (NOT TARGET vk-bootstrap::vk-bootstrap)
    message(FATAL_ERROR "vk-bootstrap::vk-bootstrap target not defined")
endif()

add_executable(integration_test integration_test.cpp)
target_link_libraries(integration_test vk-bootstrap::vk-bootstrap)

