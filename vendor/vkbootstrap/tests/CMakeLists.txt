add_library(VulkanMock SHARED vulkan_mock.hpp vulkan_mock.cpp)
# Need to set the target name to "vulkan" so that it'll be loaded instead of the *actual* Vulkan Loader on the system
if (WIN32)
    set_target_properties(VulkanMock PROPERTIES OUTPUT_NAME "vulkan-1")
else()
    set_target_properties(VulkanMock PROPERTIES OUTPUT_NAME "vulkan")
endif()
set_target_properties(VulkanMock PROPERTIES SOVERSION 1)

target_link_libraries(VulkanMock
    PUBLIC
        Vulkan::Headers Catch2::Catch2
    PRIVATE
        vk-bootstrap-compiler-warnings
)
target_compile_features(VulkanMock PUBLIC cxx_std_20)
target_compile_definitions(VulkanMock PUBLIC VK_NO_PROTOTYPES COMPILING_DLL)
if(WIN32)
    if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC" OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")
        target_link_options(VulkanMock PRIVATE /DEF:${CMAKE_CURRENT_SOURCE_DIR}/vulkan_mock.def)
    elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "GNU")
        target_link_options(VulkanMock PRIVATE -Wl,/DEF:${CMAKE_CURRENT_SOURCE_DIR}/vulkan_mock.def)
    endif()
endif()

add_executable(vk-bootstrap-test
    vulkan_library_loader.hpp
    vulkan_mock_setup.hpp
    vulkan_mock_setup.cpp
    bootstrap_tests.cpp
    error_code_tests.cpp
    unit_tests.cpp
    include_checks.cpp
    vulkan_hpp_tests.cpp
)

target_link_libraries(vk-bootstrap-test
    PRIVATE
    vk-bootstrap
    Vulkan::Headers
    vk-bootstrap-compiler-warnings
    VulkanMock
    Catch2::Catch2WithMain
)
target_compile_features(vk-bootstrap-test PUBLIC cxx_std_20)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(vk-bootstrap-test PRIVATE -Wno-error=deprecated-declarations)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(vk-bootstrap-test PRIVATE /wd4996)
endif()

if (WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(vk-bootstrap-compiler-warnings INTERFACE -Wno-microsoft-cast)
endif()

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(vk-bootstrap-test)

# Test add_subdirectory support using fetch content vulkan headers
add_test(NAME integration.add_subdirectory.fetch_content_vulkan_headers
    COMMAND ${CMAKE_CTEST_COMMAND}
        --build-and-test ${CMAKE_CURRENT_LIST_DIR}/integration
                         ${CMAKE_CURRENT_BINARY_DIR}/add_subdirectory/fetch_content_vulkan_headers
        --build-generator ${CMAKE_GENERATOR}
        --build-options -DADD_SUBDIRECTORY_TESTING=ON -DVULKAN_HEADER_VERSION_GIT_TAG=${VK_BOOTSTRAP_SOURCE_HEADER_VERSION_GIT_TAG}
)

get_target_property(vulkan_headers_include_dir Vulkan::Headers INTERFACE_INCLUDE_DIRECTORIES)

# Test add_subdirectory support by setting VK_BOOTSTRAP_VULKAN_HEADER_DIR to the include directory of the Vulkan::Headers
add_test(NAME integration.add_subdirectory.vulkan_header_dir
    COMMAND ${CMAKE_CTEST_COMMAND}
        --build-and-test ${CMAKE_CURRENT_LIST_DIR}/integration
                         ${CMAKE_CURRENT_BINARY_DIR}/add_subdirectory/vulkan_header_dir
        --build-generator ${CMAKE_GENERATOR}
        --build-options -DADD_SUBDIRECTORY_TESTING=ON -DVK_BOOTSTRAP_VULKAN_HEADER_DIR=${vulkan_headers_include_dir}
)

if (VK_BOOTSTRAP_INSTALL)
    set(test_install_dir "${CMAKE_CURRENT_BINARY_DIR}/install")
    add_test(NAME integration.install
        COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --prefix ${test_install_dir} --config $<CONFIG>
    )
endif()

find_package(VulkanHeaders CONFIG)
if (VulkanHeaders_FOUND)
    set(vulkan_headers_install_dir  ${VulkanHeaders_DIR})

    # Test add_subdirectory support using find_package(VulkanHeaders)
    add_test(NAME integration.add_subdirectory.find_package_vulkan_headers
    COMMAND ${CMAKE_CTEST_COMMAND}
        --build-and-test ${CMAKE_CURRENT_LIST_DIR}/integration
                        ${CMAKE_CURRENT_BINARY_DIR}/add_subdirectory/find_package_vulkan_headers
        --build-generator ${CMAKE_GENERATOR}
        --build-options -DADD_SUBDIRECTORY_TESTING=ON -DCMAKE_PREFIX_PATH=${vulkan_headers_install_dir}
    )

    if (VK_BOOTSTRAP_INSTALL)
        # Test find_package support using find_package(VulkanHeaders)
        add_test(NAME integration.find_package.find_package_vulkan_headers
            COMMAND ${CMAKE_CTEST_COMMAND}
                --build-and-test ${CMAKE_CURRENT_LIST_DIR}/integration
                                ${CMAKE_CURRENT_BINARY_DIR}/find_package/find_package_vulkan_headers
                --build-generator ${CMAKE_GENERATOR}
                --build-options -DFIND_PACKAGE_TESTING=ON "-DCMAKE_PREFIX_PATH=${vulkan_headers_install_dir};${test_install_dir}"
        )

        set_tests_properties(integration.find_package.find_package_vulkan_headers PROPERTIES DEPENDS integration.install)
    endif()
endif()


find_package(Vulkan)
if (Vulkan_FOUND)
    # Test add_subdirectory support using find_package(Vulkan)
    add_test(NAME integration.add_subdirectory.find_package_vulkan
        COMMAND ${CMAKE_CTEST_COMMAND}
            --build-and-test ${CMAKE_CURRENT_LIST_DIR}/integration
                            ${CMAKE_CURRENT_BINARY_DIR}/add_subdirectory/find_package_vulkan
            --build-generator ${CMAKE_GENERATOR}
            --build-options -DADD_SUBDIRECTORY_TESTING=ON -DFIND_PACKAGE_VULKAN=ON
    )

    if (VK_BOOTSTRAP_INSTALL)
        # Test find_package support using find_package(Vulkan)
        add_test(NAME integration.find_package.find_package_vulkan
        COMMAND ${CMAKE_CTEST_COMMAND}
            --build-and-test ${CMAKE_CURRENT_LIST_DIR}/integration
                            ${CMAKE_CURRENT_BINARY_DIR}/find_package/find_package_vulkan
            --build-generator ${CMAKE_GENERATOR}
            --build-options -DFIND_PACKAGE_TESTING=ON -DCMAKE_PREFIX_PATH=${test_install_dir} -DFIND_PACKAGE_VULKAN=ON
        )

        set_tests_properties(integration.find_package.find_package_vulkan PROPERTIES DEPENDS integration.install)
    endif()
endif()
