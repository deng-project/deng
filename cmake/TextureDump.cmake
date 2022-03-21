# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: TextureDump.cmake - TextureDump executable build config
# author: Karl-Mihkel Ott

add_executable(TextureDump src/TextureDump.cpp)
target_link_libraries(TextureDump PRIVATE ${DENG_SHARED_TARGET})
add_dependencies(TextureDump ${DENG_SHARED_TARGET})
