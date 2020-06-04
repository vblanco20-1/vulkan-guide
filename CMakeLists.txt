# CMakeList.txt : CMake project for vulkan_guide, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("vulkan_guide")

set(sdl2_DIR "K:/Programming/SDL2-2.0.8" CACHE FILEPATH "Path to SDL2")
add_library(sdl2 INTERFACE)
target_include_directories(sdl2 INTERFACE "${sdl2_DIR}/include" "${sdl2_image_DIR}/include" )
target_link_directories(sdl2 INTERFACE "${sdl2_DIR}/lib/x64" "${sdl2_image_DIR}/lib/x64")
target_link_libraries(sdl2 INTERFACE SDL2 SDL2main)

find_package(Vulkan REQUIRED)

add_subdirectory(third_party)

# Add source to this project's executable.
add_executable (vulkan_guide "vulkan_guide.cpp" "vulkan_guide.h")

target_link_libraries(vulkan_guide volk vkbootstrap)

target_link_libraries(vulkan_guide Vulkan::Vulkan sdl2 )



if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
  set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")
else()
  set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin32/glslangValidator.exe")
endif()


file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/shaders/*.frag"
    "${PROJECT_SOURCE_DIR}/shaders/*.vert"
    "${PROJECT_SOURCE_DIR}/shaders/*.comp"
    )

foreach(GLSL ${GLSL_SOURCE_FILES})
  message(STATUS "BUILDING SHADER")
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
  message(STATUS ${GLSL})
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
    )
