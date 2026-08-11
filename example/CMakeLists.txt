function(add_vulkan_example name)
    add_executable(vk-bootstrap-${name} ${name}.cpp)
    target_link_libraries(vk-bootstrap-${name}
        PRIVATE
            glfw
            vk-bootstrap
            vk-bootstrap-compiler-warnings
            Vulkan::Headers)
    target_include_directories(vk-bootstrap-${name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}) # path to build directory for shaders
endfunction()

add_vulkan_example(basic_usage)
add_vulkan_example(custom_debug_callback)
add_vulkan_example(simple_compute)
add_vulkan_example(system_info)
add_vulkan_example(triangle)

find_program(GLSLANG_FOUND glslang)
if(GLSLANG_FOUND)
    message(STATUS "glslang found, will compile shaders automatically")

    set(COMPILED_SHADER_FILES)
    macro(compile_shader SHADER_NAME)
        set(SHADER_SOURCE ${CMAKE_SOURCE_DIR}/example/shaders/${SHADER_NAME})
        set(SHADER_SPIRV_NAME ${SHADER_NAME}.spv)
        set(SHADER_SPIRV_PATH ${CMAKE_SOURCE_DIR}/example/shaders/${SHADER_SPIRV_NAME})

        add_custom_command(
            OUTPUT ${SHADER_SPIRV_PATH}
            COMMAND glslang -V ${SHADER_SOURCE} -o ${SHADER_SPIRV_PATH} --target-env vulkan1.0
            DEPENDS ${SHADER_SOURCE}
            COMMENT "Shader ${SHADER_NAME} compiled"
        )

        list(APPEND COMPILED_SHADER_FILES ${SHADER_SPIRV_PATH})
    endmacro()

    compile_shader(triangle.frag)
    compile_shader(triangle.vert)
    compile_shader(simple_compute.comp)
    add_custom_target(generate_shaders DEPENDS ${COMPILED_SHADER_FILES})
    add_dependencies(vk-bootstrap-triangle generate_shaders)
endif()


configure_file (
    "${PROJECT_SOURCE_DIR}/example/example_config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/example_config.h"
)
