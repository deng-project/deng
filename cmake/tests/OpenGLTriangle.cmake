# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: OpenGLTriangle.txt - OpenGLTriangle test application CMake configuration file
# author: Karl-Mihkel Ott

set(OGL_TRIANGLE_TARGET OpenGLTriangle)
set(OGL_TRIANGLE_SOURCES tests/OpenGLTriangle.cpp)

add_executable(${OGL_TRIANGLE_TARGET} ${OGL_TRIANGLE_SOURCES})

target_include_directories(${OGL_TRIANGLE_TARGET}
    PRIVATE include
    PRIVATE deps
    PRIVATE deps/nekowin/include/third_party
)

target_link_libraries(${OGL_TRIANGLE_TARGET} deng-shared)

# Check if build dependencies should be built
if(BUILD_DEPS)
    target_link_directories(${OGL_TRIANGLE_TARGET}
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/deps/shaderc/libshaderc
    )
else()
    target_link_directories(${OGL_TRIANGLE_TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endif()

add_custom_command(TARGET ${OGL_TRIANGLE_TARGET}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/OpenGLTriangleData
)

add_custom_command(TARGET ${OGL_TRIANGLE_TARGET}
    POST_BUILD 
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_directory ${CMAKE_SOURCE_DIR}/tests/OpenGLTriangleData/ ${CMAKE_CURRENT_BINARY_DIR}/OpenGLTriangleData
)
