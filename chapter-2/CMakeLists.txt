
# Add source to this project's executable.
add_executable (chapter_2 
  main.cpp
  vk_engine.cpp
  vk_engine.h
  vk_types.h
  vk_initializers.cpp
  vk_initializers.h)


set_property(TARGET chapter_2 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:chapter_2>)

target_include_directories(chapter_2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(chapter_2  vkbootstrap vma glm tinyobjloader imgui stb_image)

target_link_libraries(chapter_2 Vulkan::Vulkan sdl2)
