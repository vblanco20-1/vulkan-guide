
# Add source to this project's executable.
add_executable (meshbaker 
  main.cpp

)


set_property(TARGET meshbaker PROPERTY CXX_STANDARD 20)
set_property(TARGET meshbaker PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:meshbaker>)

target_include_directories(meshbaker PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(meshbaker vkguide_shared vkbootstrap imgui fastgltf::fastgltf)

add_custom_command(TARGET meshbaker POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:meshbaker> $<TARGET_FILE_DIR:meshbaker>
  COMMAND_EXPAND_LISTS
  )