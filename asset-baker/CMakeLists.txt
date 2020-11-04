set(CMAKE_CXX_STANDARD 17)
# Add source to this project's executable.
add_executable (baker
"asset_main.cpp")

set_property(TARGET baker PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:extra>")

target_include_directories(baker PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(baker PUBLIC tinyobjloader stb_image json lz4 assetlib tinyGLTF nvtt)

