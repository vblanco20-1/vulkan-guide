find_package(Vulkan REQUIRED)

add_subdirectory(fastgltf)
add_subdirectory(fmt)
add_subdirectory(glm)
add_subdirectory(SDL)
add_subdirectory(vkbootstrap)
add_subdirectory(vma)
add_subdirectory(volk)

# SDB Library
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)

# ImGUI

add_library(imgui STATIC)
target_include_directories(imgui PUBLIC imgui)

target_sources(imgui PRIVATE 
                imgui/imgui.h
                imgui/imgui.cpp
                
                imgui/imgui_draw.cpp
                imgui/backends/imgui_impl_vulkan.cpp
                imgui/backends/imgui_impl_sdl3.cpp)

target_link_libraries(imgui PUBLIC Vulkan::Vulkan SDL3::SDL3)
