
# Add source to this project's executable.
add_executable (extra
"main.cpp"
 "vk_engine.cpp"
 "vk_engine.h" 
 "vk_types.h" 
 "vk_mesh.cpp"
 "vk_mesh.h"
"vk_textures.cpp"
 "vk_textures.h"
"vk_shaders.cpp"
 "vk_shaders.h"
 "vk_initializers.cpp" 
 "vk_initializers.h")


set_property(TARGET extra PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:extra>")

target_include_directories(extra PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(extra vkbootstrap vma glm tinyobjloader imgui stb_image spirv_reflect)

target_link_libraries(extra Vulkan::Vulkan sdl2)