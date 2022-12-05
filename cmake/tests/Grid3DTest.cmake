# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: Grid3DTest.cmake - 3D grid space test application cmake configuration
# author: Karl-Mihkel Ott

set(OPENGL_GRID_TEST_TARGET OpenGLGrid3DTest)
set(OPENGL_GRID_TEST_SOURCES 
    tests/Grid3DTest.h
    tests/OpenGLGrid3DTest.cpp)

set(VULKAN_GRID_TEST_TARGET VulkanGrid3DTest)
set(VULKAN_GRID_TEST_SOURCES
    tests/Grid3DTest.h
    tests/VulkanGrid3DTest.cpp)

# OpenGL test
add_executable(${OPENGL_GRID_TEST_TARGET} ${OPENGL_GRID_TEST_SOURCES})
add_dependencies(${OPENGL_GRID_TEST_TARGET} ${DENG_COMPLETE_TARGET})
target_link_libraries(${OPENGL_GRID_TEST_TARGET} PRIVATE ${DENG_COMPLETE_TARGET})
set_target_properties(${OPENGL_GRID_TEST_TARGET} PROPERTIES FOLDER ${MANUAL_TEST_DIR})

# Vulkan test
add_executable(${VULKAN_GRID_TEST_TARGET} ${VULKAN_GRID_TEST_SOURCES})
add_dependencies(${VULKAN_GRID_TEST_TARGET} ${DENG_COMPLETE_TARGET})
target_link_libraries(${VULKAN_GRID_TEST_TARGET} PRIVATE ${DENG_COMPLETE_TARGET})
set_target_properties(${OPENGL_GRID_TEST_TARGET} PROPERTIES FOLDER ${MANUAL_TEST_DIR})