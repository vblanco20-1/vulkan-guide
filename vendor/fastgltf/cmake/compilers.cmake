macro(fastgltf_compiler_flags TARGET)
    if (NOT ${TARGET} STREQUAL "" AND TARGET ${TARGET})
        # Note that simdjson automatically figures out which SIMD intrinsics to use at runtime based on
        # cpuid, meaning no architecture flags or other compile flags need to be passed.
        # See https://github.com/simdjson/simdjson/blob/master/doc/implementation-selection.md.
        if (MSVC)
            target_compile_options(${TARGET} PRIVATE /EHsc /utf-8 $<$<CONFIG:RELEASE>:/O2 /Ob3 /Ot>)
            if (MSVC_VERSION GREATER 1929)
                target_compile_options(${TARGET} PRIVATE /external:W0 /external:anglebrackets)
            endif()
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${TARGET} PRIVATE $<$<CONFIG:RELEASE>:-O3> -Wall -Wno-unknown-pragmas)

            if (MINGW)
                # Issue with MinGW: https://github.com/simdjson/simdjson/issues/1963
                target_compile_options(${TARGET} PRIVATE $<$<CONFIG:DEBUG>:-Og>)
            endif()

            # https://github.com/simdjson/simdjson/blob/master/doc/basics.md#performance-tips
            target_compile_options(${TARGET} PRIVATE $<$<CONFIG:RELEASE>:-DNDEBUG>)

            if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
                # For the conversion of ARM Neon vectors (say int16x8_t to int8x16_t)
                target_compile_options(${TARGET} PRIVATE -flax-vector-conversions -fpermissive)
            endif()
        endif()
    endif()
endmacro()

macro(fastgltf_enable_debug_inlining TARGET)
    if (NOT ${TARGET} STREQUAL "" AND TARGET ${TARGET})
        if (MSVC)
            target_compile_options(${TARGET} PRIVATE $<$<CONFIG:DEBUG>:/Ob2>)
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${TARGET} PRIVATE $<$<CONFIG:DEBUG>:-finline-functions>)
        endif()
    endif()
endmacro()

function(fastgltf_check_modules_support)
    # Clang 16 and newer support modules, as well as Visual Studio 17.1 (1931) and newer.
    if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "16.0.0") OR (MSVC AND MSVC_VERSION GREATER_EQUAL 1931) OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "14.1.0"))
        set(FASTGLTF_SUPPORTS_MODULES true PARENT_SCOPE)
    else()
        set(FASTGLTF_SUPPORTS_MODULES false PARENT_SCOPE)
    endif()
endfunction()
