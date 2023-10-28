macro(compiler_flags TARGET)
    if (NOT ${TARGET} STREQUAL "" AND TARGET ${TARGET})
        # Note that simdjson automatically figures out which SIMD intrinsics to use at runtime based on
        # cpuid, meaning no architecture flags or other compile flags need to be passed.
        # See https://github.com/simdjson/simdjson/blob/master/doc/implementation-selection.md.
        if (MSVC)
            target_compile_options(${TARGET} PRIVATE /EHsc /utf-8 $<$<CONFIG:RELEASE>:/O2 /Ob3 /Ot>)
            if (MSVC_VERSION GREATER 1929)
                target_compile_options(${TARGET} PRIVATE /external:W0 /external:anglebrackets)
            endif()
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${TARGET} PRIVATE $<$<CONFIG:RELEASE>:-O3>)

            # Issue with MinGW: https://github.com/simdjson/simdjson/issues/1963
            target_compile_options(${TARGET} PUBLIC $<$<CONFIG:DEBUG>:-Og>)

            # https://github.com/simdjson/simdjson/blob/master/doc/basics.md#performance-tips
            target_compile_options(${TARGET} PRIVATE $<$<CONFIG:RELEASE>:-DNDEBUG>)
        endif()
    endif()
endmacro()

macro(enable_debug_inlining TARGET)
    if (NOT ${TARGET} STREQUAL "" AND TARGET ${TARGET})
        if (MSVC)
            target_compile_options(${TARGET} PRIVATE $<$<CONFIG:DEBUG>:/Ob2>)
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${TARGET} PRIVATE $<$<CONFIG:DEBUG>:-finline-functions>)
        endif()
    endif()
endmacro()
