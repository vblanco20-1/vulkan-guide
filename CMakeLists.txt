# CMakeList.txt : CMake project for vulkan_guide, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("vulkan_guide")

set(sdl2_DIR "SDL_PATH" CACHE FILEPATH "Path to SDL2")
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