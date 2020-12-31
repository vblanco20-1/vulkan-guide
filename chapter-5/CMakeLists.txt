
# Add source to this project's executable.
add_executable (chapter_5 
  main.cpp
  vk_engine.cpp
  vk_engine.h
  vk_types.h
  vk_mesh.cpp
  vk_mesh.h
  vk_textures.cpp
  vk_textures.h
  vk_initializers.cpp
  vk_initializers.h)


set_property(TARGET chapter_5 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:chapter_5>)

target_include_directories(chapter_5 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(chapter_5 vkbootstrap vma glm tinyobjloader imgui stb_image)

target_link_libraries(chapter_5 Vulkan::Vulkan sdl2)
