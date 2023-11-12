
# Add source to this project's executable.
add_executable (chapter_5 
  main.cpp
  vk_engine.cpp
  vk_engine.h

  vk_loader.h
  vk_loader.cpp
)


set_property(TARGET chapter_5 PROPERTY CXX_STANDARD 20)
set_property(TARGET chapter_5 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:chapter_5>)

target_include_directories(chapter_5 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(chapter_5 vkguide_shared vkbootstrap imgui fastgltf::fastgltf)

add_custom_command(TARGET chapter_5 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:chapter_5> $<TARGET_FILE_DIR:chapter_5>
  COMMAND_EXPAND_LISTS
  )