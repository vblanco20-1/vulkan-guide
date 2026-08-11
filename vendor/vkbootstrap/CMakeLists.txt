cmake_minimum_required(VERSION 3.22)

include(gen/CurrentBuildVulkanVersion.cmake)

project(VulkanBootstrap
    LANGUAGES CXX
    DESCRIPTION "A Vulkan utility library to ease the initialization steps in Vulkan"
    VERSION ${VK_BOOTSTRAP_SOURCE_HEADER_VERSION})

option(VK_BOOTSTRAP_DISABLE_WARNINGS "Disable warnings during compilation" OFF)
option(VK_BOOTSTRAP_WERROR "Enable warnings as errors during compilation" OFF)

option(VK_BOOTSTRAP_TEST "Test Vk-Bootstrap using Catch2 as well as build examples" ${PROJECT_IS_TOP_LEVEL})
option(VK_BOOTSTRAP_INSTALL "Enable installing of vk-bootstrap" ${PROJECT_IS_TOP_LEVEL})

option(VK_BOOTSTRAP_FORCE_FETCH_CONTENT "Forces Vulkan-Headers to be acquired using Fetch Content")
set(VK_BOOTSTRAP_VULKAN_HEADER_DIR "" CACHE FILEPATH "Specify the location of the Vulkan-Headers include directory.")
mark_as_advanced(VK_BOOTSTRAP_VULKAN_HEADER_DIR)

# Check if the user has set this variable explicitly
if(IS_DIRECTORY ${VK_BOOTSTRAP_VULKAN_HEADER_DIR})
    add_library(Vulkan-Headers INTERFACE)
    add_library(Vulkan::Headers ALIAS Vulkan-Headers)
    target_include_directories(Vulkan-Headers INTERFACE $<BUILD_INTERFACE:${VK_BOOTSTRAP_VULKAN_HEADER_DIR}>)
    # If we had to use a direct path to get the headers, disable installing
    set(VK_BOOTSTRAP_INSTALL OFF)
# Check if the target is already defined
elseif(NOT TARGET Vulkan::Headers)
    if (NOT VK_BOOTSTRAP_FORCE_FETCH_CONTENT)
        # Try looking for the VulkanHeaders package directly
        find_package(VulkanHeaders CONFIG QUIET)
    endif()
    if (NOT VulkanHeaders_FOUND)
        if (NOT VK_BOOTSTRAP_FORCE_FETCH_CONTENT)
            # Try looking using the CMake built in Vulkan support
            find_package(Vulkan QUIET)
        endif()

        if(Vulkan_FOUND)
            # Older CMake versions don't contain Vulkan::Headers - create it in that case
            if (NOT TARGET Vulkan::Headers)
                add_library(Vulkan-Headers INTERFACE)
                add_library(Vulkan::Headers ALIAS Vulkan-Headers)
                target_include_directories(Vulkan-Headers INTERFACE $<BUILD_INTERFACE:${Vulkan_INCLUDE_DIRS}>)
                set(VK_BOOTSTRAP_INSTALL OFF)
            endif()
        else()
            # Lastly just grab Vulkan-Headers directly using FetchContent
            include(FetchContent)
            FetchContent_Declare(
                Vulkan-Headers-for-vk-bootstrap
                GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers
                GIT_TAG        ${VK_BOOTSTRAP_SOURCE_HEADER_VERSION_GIT_TAG}
            )
            FetchContent_MakeAvailable(Vulkan-Headers-for-vk-bootstrap)
            # If we had to use FetchContent to get the headers, disable installing
            set(VK_BOOTSTRAP_INSTALL OFF)
        endif()
    endif()
endif()

if(NOT TARGET Vulkan::Headers)
    message(FATAL_ERROR "Unable to locate required dependency Vulkan::Headers!")
endif()

add_library(vk-bootstrap-compiler-warnings INTERFACE)

set(VK_BOOTSTRAP_COMPILER_FRONTEND ${CMAKE_CXX_COMPILER_FRONTEND_VARIANT})
if(NOT VK_BOOTSTRAP_COMPILER_FRONTEND)
    set(VK_BOOTSTRAP_COMPILER_FRONTEND "None")
endif()

if(NOT VK_BOOTSTRAP_DISABLE_WARNINGS)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|AppleClang" OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND VK_BOOTSTRAP_COMPILER_FRONTEND MATCHES "GNU"))
        target_compile_options(vk-bootstrap-compiler-warnings INTERFACE -Wall -Wextra -Wconversion -Wsign-conversion)
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            target_compile_options(vk-bootstrap-compiler-warnings INTERFACE -Wstrict-aliasing) # GCC only warning
        endif()
        if(VK_BOOTSTRAP_WERROR)
            target_compile_options(vk-bootstrap-compiler-warnings INTERFACE -Werror -pedantic -pedantic-errors)
        endif()
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC" OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND VK_BOOTSTRAP_COMPILER_FRONTEND MATCHES "MSVC"))
        target_compile_options(vk-bootstrap-compiler-warnings INTERFACE /W4)
        if(VK_BOOTSTRAP_WERROR)
            target_compile_options(vk-bootstrap-compiler-warnings INTERFACE /WX)
        endif()
    endif()
endif()

add_library(vk-bootstrap STATIC src/VkBootstrap.h src/VkBootstrap.cpp src/VkBootstrapDispatch.h src/VkBootstrapFeatureChain.h src/VkBootstrapFeatureChain.inl)
add_library(vk-bootstrap::vk-bootstrap ALIAS vk-bootstrap)

target_include_directories(vk-bootstrap PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>)
target_link_libraries(vk-bootstrap
    PUBLIC
        Vulkan::Headers
    PRIVATE
        vk-bootstrap-compiler-warnings
        ${CMAKE_DL_LIBS})
target_compile_features(vk-bootstrap PUBLIC cxx_std_17)

option(VK_BOOTSTRAP_POSITION_INDEPENDENT_CODE "Default value is the value of BUILD_SHARED_LIBS" ${BUILD_SHARED_LIBS})
set_target_properties(vk-bootstrap PROPERTIES POSITION_INDEPENDENT_CODE ${VK_BOOTSTRAP_POSITION_INDEPENDENT_CODE})

if(VK_BOOTSTRAP_TEST)
    enable_testing()

    option(ENABLE_ADDRESS_SANITIZER "Use address sanitization")
    if (ENABLE_ADDRESS_SANITIZER)
        if (MSVC)
            target_compile_options(vk-bootstrap-compiler-warnings INTERFACE /fsanitize=address $<$<NOT:$<CONFIG:DEBUG>>:/wd5072>)
            target_link_options(vk-bootstrap-compiler-warnings INTERFACE /INCREMENTAL:NO)
            add_compile_definitions(_DISABLE_VECTOR_ANNOTATION _DISABLE_STRING_ANNOTATION)
        else()
            target_compile_options(vk-bootstrap-compiler-warnings INTERFACE -fsanitize=address)
            target_link_options(vk-bootstrap-compiler-warnings INTERFACE -fsanitize=address)
        endif()
    endif()

    option(ENABLE_THREAD_SANITIZER "Use thread sanitization")
    if (ENABLE_THREAD_SANITIZER)
        if (MSVC)
            message(FATAL_ERROR "MSVC doesn't support thread sanitization!")
        else()
            target_compile_options(vk-bootstrap-compiler-warnings INTERFACE -fsanitize=thread)
            target_link_options(vk-bootstrap-compiler-warnings INTERFACE -fsanitize=thread)
        endif()
    endif()

    add_subdirectory(ext)
    add_subdirectory(tests)
    add_subdirectory(example)
endif ()

if (VK_BOOTSTRAP_INSTALL)

    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(FILES src/VkBootstrap.h src/VkBootstrapDispatch.h src/VkBootstrapFeatureChain.h src/VkBootstrapFeatureChain.inl DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(
        TARGETS vk-bootstrap vk-bootstrap-compiler-warnings
        EXPORT vk-bootstrap-targets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
    install(
        EXPORT vk-bootstrap-targets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vk-bootstrap
        NAMESPACE vk-bootstrap::
    )

    # Create vk-bootstrap-config.cmake
    set(VK_BOOTSTRAP_EXPORT_TARGETS ${CMAKE_INSTALL_LIBDIR}/cmake/vk-bootstrap/vk-bootstrap-targets.cmake)
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/vk-bootstrap-config.cmake.in" [=[
        @PACKAGE_INIT@

        # Try to find Vulkan-Headers using find_package. Assume the package is available through either Vulkan-Headers or the older Vulkan
        # Package managers should have Vulkan-Headers be a dependency of this repo.
        find_package(VulkanHeaders CONFIG)
        if (NOT VulkanHeaders_FOUND)
            find_package(Vulkan)
            if (NOT Vulkan_FOUND)
                message(FATAL_ERROR "Unable to locate required dependency Vulkan::Headers!")
            endif()
        endif()
        include(@PACKAGE_VK_BOOTSTRAP_EXPORT_TARGETS@)
    ]=])

    configure_package_config_file(
        ${CMAKE_CURRENT_BINARY_DIR}/vk-bootstrap-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/vk-bootstrap-config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vk-bootstrap
        PATH_VARS VK_BOOTSTRAP_EXPORT_TARGETS
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/vk-bootstrap-config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vk-bootstrap
    )

endif()
