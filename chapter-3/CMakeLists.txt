
# Add source to this project's executable.
add_executable (chapter_3 
  main.cpp
  vk_engine.cpp
  vk_engine.h
  vk_loader.h
  vk_loader.cpp
)

set_property(TARGET chapter_3 PROPERTY CXX_STANDARD 20)
set_property(TARGET chapter_3 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:chapter_3>)

target_include_directories(chapter_3 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(chapter_3 vkguide_shared vkbootstrap imgui fastgltf::fastgltf)

add_custom_command(TARGET chapter_3 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:chapter_3> $<TARGET_FILE_DIR:chapter_3>
  COMMAND_EXPAND_LISTS
  )
