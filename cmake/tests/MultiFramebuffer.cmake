# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: MultiFramebuffer.cmake - Multi framebuffer application build config
# author: Karl-Mihkel Ott

set(OPENGL_MULTI_FRAMEBUFFER_TARGET OpenGLMultiFramebufferTest)
set(OPENGL_MULTI_FRAMEBUFFER_SOURCES 
    tests/MultiFramebuffer.h
    tests/OpenGLMultiFramebuffer.cpp)

set(VULKAN_MULTI_FRAMEBUFFER_TARGET VulkanMultiFramebufferTest)
set(VULKAN_MULTI_FRAMEBUFFER_SOURCES 
    tests/MultiFramebuffer.h
    tests/VulkanMultiFramebuffer.cpp)


# OpenGL test
add_executable(${OPENGL_MULTI_FRAMEBUFFER_TARGET} ${OPENGL_MULTI_FRAMEBUFFER_SOURCES})
add_dependencies(${OPENGL_MULTI_FRAMEBUFFER_TARGET} ${DENG_SHARED_TARGET})
target_link_libraries(${OPENGL_MULTI_FRAMEBUFFER_TARGET} PRIVATE ${DENG_SHARED_TARGET})

# Vulkan test
add_executable(${VULKAN_MULTI_FRAMEBUFFER_TARGET} ${VULKAN_MULTI_FRAMEBUFFER_SOURCES})
add_dependencies(${VULKAN_MULTI_FRAMEBUFFER_TARGET} ${DENG_SHARED_TARGET})
target_link_libraries(${VULKAN_MULTI_FRAMEBUFFER_TARGET} PRIVATE ${DENG_SHARED_TARGET})
