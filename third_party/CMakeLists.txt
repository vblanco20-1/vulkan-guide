add_library(volk INTERFACE)
add_library(vkbootstrap INTERFACE)

target_sources(volk INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/volk/volk.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/volk/volk.c"
)
target_include_directories(volk INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/volk" )

target_sources(vkbootstrap INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/vkbootstrap/VkBootstrap.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/vkbootstrap/VkBootstrap.cpp"
)

target_include_directories(vkbootstrap INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/vkbootstrap" )
