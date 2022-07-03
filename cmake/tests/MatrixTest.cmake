# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: MatrixTest.cmake - MatrixTest application build configuration
# author: Karl-Mihkel Ott

add_executable(MatrixTest tests/MatrixTest.cpp)
target_include_directories(MatrixTest 
    PRIVATE deps 
    PRIVATE include
)

target_link_libraries(MatrixTest PRIVATE ${DENG_TARGET})
