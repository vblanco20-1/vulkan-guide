
# Add source to this project's executable.
add_executable (chapter_3 
  main.cpp
  vk_engine.cpp
  vk_engine.h
  vk_types.h
  vk_mesh.cpp
  vk_mesh.h
  vk_initializers.cpp
  vk_initializers.h)


set_property(TARGET chapter_3 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:chapter_3>)

target_include_directories(chapter_3 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(chapter_3 vkbootstrap vma glm tinyobjloader imgui stb_image)

target_link_libraries(chapter_3 Vulkan::Vulkan sdl2)
