# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: CMakeFiles.txt - main CMake configuration file
# author: Karl-Mihkel Ott

cmake_minimum_required(VERSION 3.10)
project(deng)
set(CXX_STANDARD C++17)
set(C_STANDARD C99)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
option(BUILD_DEPS "Build DENG dependencies only" OFF)
option(BUILD_TESTS "Build DENG testing applications" ON)
option(BUILD_DENG "Build all DENG targets" ON)

# Check if debugging definition should be considered
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
endif()

# Check if current operating system is supported
if(BUILD_DEPS OR BUILD_ALL)
    include(cmake/libdas.cmake)
    include(cmake/libnwin.cmake)
    add_subdirectory(deps/shaderc)
endif()

if(BUILD_DENG)
    include(cmake/libdeng.cmake)
endif()


if(BUILD_TESTS)
    message(STATUS "Adding test build configurations")
    include(cmake/tests/OpenGLTriangle.cmake)
endif()
