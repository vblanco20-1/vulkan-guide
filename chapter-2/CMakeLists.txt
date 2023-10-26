
# Add source to this project's executable.
add_executable (chapter_2 
  main.cpp
  vk_engine.cpp
  vk_engine.h
)

set_property(TARGET chapter_2 PROPERTY CXX_STANDARD 20)
set_property(TARGET chapter_2 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:chapter_2>)

target_include_directories(chapter_2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")


target_link_libraries(chapter_2 vkguide_shared vkbootstrap imgui)

add_custom_command(TARGET chapter_2 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:chapter_2> $<TARGET_FILE_DIR:chapter_2>
  COMMAND_EXPAND_LISTS
  )
