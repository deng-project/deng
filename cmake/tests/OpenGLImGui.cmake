# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: OpenGLImGui.cmake - Vulkan ImGui application build config
# author: Karl-Mihkel Ott

set(OGL_TRIANGLE_TARGET OpenGLImGui)
set(OGL_TRIANGLE_SOURCES tests/OpenGLImGui.cpp)

add_executable(${OGL_TRIANGLE_TARGET} ${OGL_TRIANGLE_SOURCES})
target_include_directories(${OGL_TRIANGLE_TARGET}
    PRIVATE include
    PRIVATE deps
    PRIVATE deps/nekowin/include/third_party
)

if(WIN32)
	target_link_libraries(${OGL_TRIANGLE_TARGET} ${DENG_STATIC_TARGET})
else()
	target_link_libraries(${OGL_TRIANGLE_TARGET} ${DENG_SHARED_TARGET})
endif()