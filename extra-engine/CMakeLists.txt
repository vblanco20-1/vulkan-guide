set(CMAKE_CXX_STANDARD 17)
# Add source to this project's executable.
add_executable (extra
"main.cpp"
"logger.h"
 "vk_engine.cpp"
"vk_engine_scenerender.cpp"
 "vk_engine.h" 
 "vk_types.h" 
 "vk_mesh.cpp"
 "vk_mesh.h"
 "material_system.cpp"
 "material_system.h"
  "vk_scene.cpp"
 "vk_scene.h"
"frustum_cull.h"
"vk_textures.cpp"
 "vk_textures.h"
"vk_descriptors.cpp"
 "vk_descriptors.h"
"vk_shaders.cpp"
 "vk_shaders.h"
 "vk_initializers.cpp" 
 "vk_initializers.h"
  "vk_profiler.cpp" 
  "vk_profiler.h"
 )


set_property(TARGET extra PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:extra>")

target_include_directories(extra PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")


target_compile_definitions(extra PUBLIC TRACY_ENABLE)
target_compile_definitions(extra PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_FORCE_LEFT_HANDED)


target_precompile_headers(extra PUBLIC "vk_types.h" "<unordered_map>" "<vector>" "<iostream>" "<fstream>" "<string>" )
target_link_libraries(extra vkbootstrap vma glm tinyobjloader imgui stb_image spirv_reflect)

target_link_libraries(extra Vulkan::Vulkan sdl2 assetlib tracy fmt_lib)