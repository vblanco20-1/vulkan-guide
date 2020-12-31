
# Add source to this project's executable.
add_executable (chapter_1 
  main.cpp
  vk_engine.cpp
  vk_engine.h
  vk_types.h
  vk_initializers.cpp
  vk_initializers.h)


set_property(TARGET chapter_1 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:chapter_1>)

target_include_directories(chapter_1 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(chapter_1 vkbootstrap vma glm tinyobjloader imgui stb_image)

target_link_libraries(chapter_1 Vulkan::Vulkan sdl2)
