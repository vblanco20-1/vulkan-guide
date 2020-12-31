find_package(Vulkan REQUIRED)

add_library(vkbootstrap STATIC)
add_library(glm INTERFACE)
add_library(vma INTERFACE)

add_library(stb_image INTERFACE)

add_library(tinyobjloader STATIC)

target_sources(vkbootstrap PRIVATE 
  vkbootstrap/VkBootstrap.h
  vkbootstrap/VkBootstrap.cpp
  )

target_include_directories(vkbootstrap PUBLIC vkbootstrap)
target_link_libraries(vkbootstrap PUBLIC Vulkan::Vulkan $<$<BOOL:UNIX>:${CMAKE_DL_LIBS}>)

#both vma and glm and header only libs so we only need the include path
target_include_directories(vma INTERFACE vma)

target_include_directories(glm INTERFACE glm)

target_sources(tinyobjloader PRIVATE 
  tinyobjloader/tiny_obj_loader.h
  tinyobjloader/tiny_obj_loader.cc
  )

target_include_directories(tinyobjloader PUBLIC tinyobjloader)

find_package(SDL2) # Find SDL via installed cmake config files (i.e. on linux)
add_library(sdl2 INTERFACE)
set(sdl2_DIR "SDL_PATH" CACHE FILEPATH "Path to SDL2")
if (sdl2_DIR STREQUAL "" OR sdl2_DIR STREQUAL "SDL_PATH")
  if (SDL2_INCLUDE_DIRS STREQUAL "")
    message(WARNING "Setup path to SDL or install libsdl-2.0")
  endif()
else()
  target_link_directories(sdl2 INTERFACE $<$<BOOL:UNIX>:${sdl2_DIR}/lib;${sdl2_image_DIR}/lib>  $<$<BOOL:WIN32>:"${sdl2_DIR}/lib/x64";"${sdl2_image_DIR}/lib/x64">)
endif()
target_include_directories(sdl2 INTERFACE ${SDL2_INCLUDE_DIRS} "${sdl2_DIR}/include" "${sdl2_image_DIR}/include")
target_link_libraries(sdl2 INTERFACE SDL2 SDL2main)

add_library(imgui STATIC)

target_include_directories(imgui PUBLIC imgui)

target_sources(imgui PRIVATE 
  imgui/imgui.h
  imgui/imgui.cpp

  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_widgets.cpp

  imgui/imgui_impl_vulkan.cpp
  imgui/imgui_impl_sdl.cpp
  )

target_link_libraries(imgui PUBLIC Vulkan::Vulkan sdl2)

target_include_directories(stb_image INTERFACE stb_image)
