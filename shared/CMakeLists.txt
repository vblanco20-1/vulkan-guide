
# Add source to this project's executable.
add_library (vkguide_shared STATIC
  vk_types.h
  vk_initializers.cpp
  vk_initializers.h
  vk_images.h
  vk_images.cpp 
  vk_descriptors.h
  vk_descriptors.cpp
  vk_pipelines.h
  vk_pipelines.cpp
  camera.cpp
  camera.h
  meshes.h
  meshes.cpp
)

set_property(TARGET vkguide_shared PROPERTY CXX_STANDARD 20)
target_compile_definitions(vkguide_shared PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)

target_include_directories(vkguide_shared PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(vkguide_shared PUBLIC vma glm Vulkan::Vulkan fmt::fmt stb_image SDL2::SDL2)

target_precompile_headers(vkguide_shared PUBLIC <optional> <vector> <memory> <string> <vector> <unordered_map> <glm/mat4x4.hpp>  <glm/vec4.hpp> <vulkan/vulkan.h>)
