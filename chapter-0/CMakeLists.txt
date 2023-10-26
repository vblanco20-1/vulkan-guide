
# Add source to this project's executable.
add_executable (chapter_0 
  main.cpp
  vk_engine.cpp
  vk_engine.h
 )

 set_property(TARGET chapter_0 PROPERTY CXX_STANDARD 20)
set_property(TARGET chapter_0 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:chapter_0>)

target_include_directories(chapter_0 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(chapter_0 vkguide_shared  vkbootstrap)

add_custom_command(TARGET chapter_0 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:chapter_0> $<TARGET_FILE_DIR:chapter_0>
  COMMAND_EXPAND_LISTS
  )
