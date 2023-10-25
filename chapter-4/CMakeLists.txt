
# Add source to this project's executable.
add_executable (chapter_4 
  main.cpp
  vk_engine.cpp
  vk_engine.h
  vk_types.h
  vk_initializers.cpp
  vk_initializers.h
  vk_images.h
  vk_images.cpp
  vk_loader.h
  vk_loader.cpp
  vk_descriptors.h
  vk_descriptors.cpp
 vk_pipelines.h
  vk_pipelines.cpp
 camera.cpp
camera.h
)


set_property(TARGET chapter_4 PROPERTY CXX_STANDARD 20)
set_property(TARGET chapter_4 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:chapter_1>)

target_compile_definitions(chapter_4 PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_include_directories(chapter_4 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(chapter_4 vkbootstrap vma glm imgui stb_image)

target_link_libraries(chapter_4 Vulkan::Vulkan sdl2 fastgltf::fastgltf fmt::fmt)

target_precompile_headers(chapter_4 PUBLIC <optional> <vector> <memory> <string> <vector> <unordered_map> <glm/mat4x4.hpp>  <glm/vec4.hpp> <vulkan/vulkan.h>)
