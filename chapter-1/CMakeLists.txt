
# Add source to this project's executable.
add_executable (chapter_1 
  main.cpp
  vk_engine.cpp
  vk_engine.h
)

set_property(TARGET chapter_1 PROPERTY CXX_STANDARD 20)
set_property(TARGET chapter_1 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:chapter_1>)

target_include_directories(chapter_1 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(chapter_1 vkguide_shared vkbootstrap vma glm  imgui stb_image)

target_link_libraries(chapter_1 Vulkan::Vulkan sdl2)
