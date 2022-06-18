# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: MultiFramebuffer.cmake - Multi framebuffer application build config
# author: Karl-Mihkel Ott

set(MULTI_FRAMEBUFFER_TARGET MultiFramebufferTest)
set(MULTI_FRAMEBUFFER_SOURCES tests/MultiFramebuffer.cpp)

add_executable(${MULTI_FRAMEBUFFER_TARGET} ${MULTI_FRAMEBUFFER_SOURCES})
add_dependencies(${MULTI_FRAMEBUFFER_TARGET} ${DENG_SHARED_TARGET})
target_link_libraries(${MULTI_FRAMEBUFFER_TARGET} PRIVATE ${DENG_SHARED_TARGET})
