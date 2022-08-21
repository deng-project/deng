# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: CMakeFiles.txt - main CMake configuration file
# author: Karl-Mihkel Ott

cmake_minimum_required(VERSION 3.10)
project(deng LANGUAGES CXX C)

# Constant config values
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build DENG testing applications" ON)


# Set appropriate compiler flags
if(MSVC)
    add_compile_options(/MT /W3 $<$<COMPILE_LANGUAGE:CXX>:/std:c++17> $<$<COMPILE_LANGUAGE:C>:/std:c17> /wd4251)
	add_compile_definitions(_ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH _CRT_SECURE_NO_WARNINGS)

    # Set correct optimisation flags
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(/Od)
    else()
        add_compile_options(/O2)
    endif()
	
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/Debug)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/Release)
else()
    add_compile_options(-Wall -msse4.1 -Wextra $<$<COMPILE_LANGUAGE:CXX>:-std=c++17> $<$<COMPILE_LANGUAGE:C>:-std=c99>)

    if(CMAKE_BUILD_TYPE MATCHES Release)
        add_compile_options(-O3)
    endif()
endif()


# Check if debugging definition should be considered
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D__DEBUG)
endif()

# Dependency versioning
set(PYTHON_VERSION 3.10.6)
set(VULKAN_SDK_VERSION 1.3.216)
set(ODE_VERSION 0.16.2)
set(WXWIDGETS_VERSION 3.2.0)
set(SHADERC_VERSION 2022.2)

# Libnwin configuration
set(NEKOWIN_BUILD_STATIC_LIB ON CACHE BOOL "Build static nekowin library")
set(NEKOWIN_BUILD_SHARED_LIB OFF CACHE BOOL "Build shared nekowin library")
set(NEKOWIN_BUILD_GLAPP OFF CACHE BOOL "Build glapp application")
set(NEKOWIN_BUILD_GAMEPAD_INFO OFF CACHE BOOL "Build a sample console application for testing controller inputs")

# Libdas and dastool configuration
set(LIBDAS_BUILD_TESTS OFF CACHE BOOL "Build libdas tests")
set(LIBDAS_BUILD_STATIC_LIB ON CACHE BOOL "Build static libdas library")
set(LIBDAS_BUILD_SHARED_LIB OFF CACHE BOOL "Build shared libdas library")
set(LIBDAS_BUILD_DASTOOL ON CACHE BOOL "Build dastool application")
set(LIBDAS_BUILD_TESTS OFF CACHE BOOL "Build libdas testing applications")
set(LIBDAS_BUILD_DEPENDENCIES OFF CACHE BOOL "Build libdas dependencies")

# dengxml configuration
set(DENGXML_BUILD_TESTS OFF CACHE BOOL "Build dengxml tests")
set(DENGXML_BUILD_DEPENDENCIES OFF CACHE BOOL "Build all dengxml dependencies")

# include(cmake/copy.cmake)
include(cmake/FetchDependencies.cmake)
include(cmake/libdeng.cmake)
include(cmake/deng-game-editor.cmake)
include(cmake/TextureDump.cmake)


# Non-experimental tests
if(BUILD_TESTS)
    message(STATUS "Adding test build configurations")
    include(cmake/tests/TriangleApp.cmake)
    include(cmake/tests/VulkanImGui.cmake)
    include(cmake/tests/OpenGLImGui.cmake)
    include(cmake/tests/ShaderGenerator.cmake)
    include(cmake/tests/FilePickerTest.cmake)
    include(cmake/tests/MultiFramebuffer.cmake)
    include(cmake/tests/Grid3DTest.cmake)

    if(EXPERIMENTAL_BUILD_CUBIC_SPLINE_TESTS)
        include(cmake/tests/CubicsInterp.cmake)
    endif()

    if(EXPERIMENTAL_BUILD_MATRIX_TESTS)
        include(cmake/tests/MatrixTest.cmake)
    endif()
endif()
