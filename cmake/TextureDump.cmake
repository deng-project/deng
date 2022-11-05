# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: TextureDump.cmake - TextureDump executable build config
# author: Karl-Mihkel Ott

add_executable(TextureDump src/Executables/TextureDump.cpp)
add_dependencies(TextureDump ${DENG_COMPLETE_TARGET})
target_link_libraries(TextureDump PRIVATE ${DENG_COMPLETE_TARGET})
