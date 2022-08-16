# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: OpenGLImGui.cmake - Vulkan ImGui application build config
# author: Karl-Mihkel Ott

set(OGL_TRIANGLE_TARGET OpenGLImGui)
set(OGL_TRIANGLE_SOURCES tests/OpenGLImGui.cpp)

add_executable(${OGL_TRIANGLE_TARGET} ${OGL_TRIANGLE_SOURCES})
add_dependencies(${OGL_TRIANGLE_TARGET} ${DENG_COMPLETE_TARGET})
target_link_libraries(${OGL_TRIANGLE_TARGET} PRIVATE ${DENG_COMPLETE_TARGET})
