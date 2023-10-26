cmake_minimum_required(VERSION 3.12)

cmake_policy(SET CMP0077 NEW)

if ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" VERSION_GREATER_EQUAL "3.24")
    cmake_policy(SET CMP0135 NEW)
endif()

project(fastgltf VERSION 0.5.0 LANGUAGES C CXX)

option(FASTGLTF_DOWNLOAD_SIMDJSON "Downloads a copy of simdjson itself to satisfy the dependency" ON)
option(FASTGLTF_USE_CUSTOM_SMALLVECTOR "Uses a custom SmallVector type optimised for small arrays" OFF)
option(FASTGLTF_ENABLE_TESTS "Enables test targets for fastlgtf" OFF)
option(FASTGLTF_ENABLE_EXAMPLES "Enables example targets for fastgltf" OFF)
option(FASTGLTF_ENABLE_DOCS "Enables the configuration of targets that build/generate documentation" OFF)
option(FASTGLTF_ENABLE_GLTF_RS "Enables the benchmark usage of gltf-rs" OFF)
option(FASTGLTF_ENABLE_DEPRECATED_EXT "Enables support for deprecated extensions" OFF)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/add_source_directory.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/compiler_flags.cmake)

if (FASTGLTF_DOWNLOAD_SIMDJSON)
    # Download and configure simdjson
    set(SIMDJSON_TARGET_VERSION "3.3.0")
    set(SIMDJSON_DL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/simdjson")
    file(MAKE_DIRECTORY ${SIMDJSON_DL_DIR})

    set(SIMDJSON_HEADER_FILE "${SIMDJSON_DL_DIR}/simdjson.h")
    set(SIMDJSON_SOURCE_FILE "${SIMDJSON_DL_DIR}/simdjson.cpp")

    macro(download_simdjson)
        file(DOWNLOAD "https://raw.githubusercontent.com/simdjson/simdjson/v${SIMDJSON_TARGET_VERSION}/singleheader/simdjson.h" ${SIMDJSON_HEADER_FILE})
        file(DOWNLOAD "https://raw.githubusercontent.com/simdjson/simdjson/v${SIMDJSON_TARGET_VERSION}/singleheader/simdjson.cpp" ${SIMDJSON_SOURCE_FILE})
    endmacro()

    if (EXISTS ${SIMDJSON_HEADER_FILE})
        # Look for the SIMDJSON_VERSION define in the header to check the version.
        file(STRINGS ${SIMDJSON_HEADER_FILE} SIMDJSON_HEADER_VERSION_LINE REGEX "^#define SIMDJSON_VERSION ")
        string(REGEX MATCHALL "[0-9.]+" SIMDJSON_HEADER_VERSION "${SIMDJSON_HEADER_VERSION_LINE}")
        message(STATUS "fastgltf: Found simdjson (Version ${SIMDJSON_HEADER_VERSION})")
        if (SIMDJSON_HEADER_VERSION VERSION_LESS SIMDJSON_TARGET_VERSION)
            message(STATUS "fastgltf: simdjson outdated, downloading...")
            download_simdjson()
        endif()
    else()
        message(STATUS "fastgltf: Did not find simdjson, downloading...")
        download_simdjson()

        if (NOT EXISTS "${SIMDJSON_HEADER_FILE}")
            message(FATAL_ERROR "fastgltf: Failed to download simdjson.")
        endif()
    endif()

    add_library(fastgltf_simdjson ${SIMDJSON_HEADER_FILE} ${SIMDJSON_SOURCE_FILE})
    target_compile_features(fastgltf_simdjson PRIVATE cxx_std_17)
    target_include_directories(fastgltf_simdjson PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/deps/simdjson> $<INSTALL_INTERFACE:include>)
    compiler_flags(fastgltf_simdjson)
    enable_debug_inlining(fastgltf_simdjson)

    install(
        FILES deps/simdjson/simdjson.h
        DESTINATION include
    )

    install(
        TARGETS fastgltf_simdjson
        EXPORT fastgltf_simdjson-targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
    )

    install(
        EXPORT fastgltf_simdjson-targets
        FILE fastgltf_simdjsonTargets.cmake
        NAMESPACE fastgltf::
        DESTINATION lib/cmake/fastgltf
    )
endif()

# Create the library target
add_library(fastgltf
    "src/fastgltf.cpp" "src/base64.cpp"
    "include/fastgltf/base64.hpp" "include/fastgltf/glm_element_traits.hpp" "include/fastgltf/parser.hpp" "include/fastgltf/tools.hpp" "include/fastgltf/types.hpp" "include/fastgltf/util.hpp")
add_library(fastgltf::fastgltf ALIAS fastgltf)

compiler_flags(fastgltf)
enable_debug_inlining(fastgltf)
target_compile_features(fastgltf PUBLIC cxx_std_17)
target_include_directories(fastgltf PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include> $<INSTALL_INTERFACE:include>)

set_target_properties(fastgltf PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS YES)
set_target_properties(fastgltf PROPERTIES VERSION ${PROJECT_VERSION})

if (TARGET fastgltf_simdjson)
    target_link_libraries(fastgltf PRIVATE fastgltf_simdjson)
elseif(TARGET simdjson::simdjson)
    target_link_libraries(fastgltf PRIVATE simdjson::simdjson)
endif()

if (SIMDJSON_TARGET_VERSION)
    target_compile_definitions(fastgltf PRIVATE SIMDJSON_TARGET_VERSION="${SIMDJSON_TARGET_VERSION}")
endif()

target_compile_definitions(fastgltf PUBLIC "FASTGLTF_USE_CUSTOM_SMALLVECTOR=$<BOOL:${FASTGLTF_USE_CUSTOM_SMALLVECTOR}>")
target_compile_definitions(fastgltf PUBLIC "FASTGLTF_ENABLE_DEPRECATED_EXT=$<BOOL:${FASTGLTF_ENABLE_DEPRECATED_EXT}>")

if (ANDROID)
    target_link_libraries(fastgltf PRIVATE android)
endif()

install(
    FILES "include/fastgltf/base64.hpp" "include/fastgltf/glm_element_traits.hpp" "include/fastgltf/parser.hpp" "include/fastgltf/tools.hpp" "include/fastgltf/types.hpp" "include/fastgltf/util.hpp"
    DESTINATION include/fastgltf
)

install(
    TARGETS fastgltf
    EXPORT fastgltf-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(
    EXPORT fastgltf-targets
    FILE fastgltfConfig.cmake
    NAMESPACE fastgltf::
    DESTINATION lib/cmake/fastgltf
)

if (FASTGLTF_ENABLE_TESTS OR FASTGLTF_ENABLE_EXAMPLES)
    # This is required so that Catch2 compiles with C++17, enabling various features we use in tests.
    if (NOT DEFINED CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD STREQUAL "" OR CMAKE_CXX_STANDARD LESS 17)
        set(CMAKE_CXX_STANDARD "17" CACHE STRING "C++ standard" FORCE)
    endif()

    add_subdirectory(deps)
endif()

if (FASTGLTF_ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif()
if (FASTGLTF_ENABLE_TESTS)
    add_subdirectory(tests)
endif()
if (FASTGLTF_ENABLE_DOCS)
    add_subdirectory(docs)
endif()
