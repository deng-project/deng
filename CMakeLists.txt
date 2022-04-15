# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: CMakeFiles.txt - main CMake configuration file
# author: Karl-Mihkel Ott

cmake_minimum_required(VERSION 3.10)
project(deng LANGUAGES CXX C)

# Constant config values
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_GENERATOR_PLATFORM x64)

set(VULKAN_SDK_PATH CACHE STRING "")
option(BUILD_DEPS "Build DENG dependencies" OFF)
option(BUILD_TESTS "Build DENG testing applications" ON)
option(BUILD_DENG "Build all DENG targets" ON)
option(EXPERIMENTAL_BUILD_MODEL_LOADER "Build model loader testing applications" OFF)
option(EXPERIMENTAL_BUILD_CUBIC_SPLINE_TESTS "Build cubic spline interpolation tests" OFF)
option(EXPERIMENTAL_BUILD_MATRIX_TESTS "Build matrix tests" OFF)

# Set appropriate compiler flags
if(MSVC)
    add_compile_options(/W4 $<$<COMPILE_LANGUAGE:CXX>:/std:c++17> $<$<COMPILE_LANGUAGE:C>:/std:c99>)
	add_compile_definitions(_ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH)

    # Set correct optimisation flags
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(/Od)
    else()
        add_compile_options(/O2)
    endif()
else()
    add_compile_options(-Wall -msse4.1 -Wextra $<$<COMPILE_LANGUAGE:CXX>:-std=c++17> $<$<COMPILE_LANGUAGE:C>:-std=c99>)

    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(-O0)
    else()
        add_compile_options(-O3)
    endif()
endif()


# Set appropriate include and linking directories
if(UNIX AND NOT APPLE)
    include_directories(${VULKAN_PATH}/x86_64/include)
    link_directories(${VULKAN_PATH}/x86_64/lib)
elseif(WIN32)
    include_directories(${VULKAN_PATH}/Include)
    link_directories(${VULKAN_PATH}/Lib)
endif()


# Check if debugging definition should be considered
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
endif()

# Check if current operating system is supported
if(BUILD_DEPS)
    include(cmake/libdas.cmake)
    include(cmake/libnwin.cmake)
    include(cmake/imgui.cmake)
	
	if(WIN32)
		set(SHADERC_ENABLE_SHARED_CRT ON)
		set(SHADERC_SKIP_TESTS ON)
		set(SHADERC_SKIP_EXAMPLES ON)
		set(SHADERC_SKIP_COPYRIGHT_CHECK ON)
		add_subdirectory(deps/shaderc)
	endif()
endif()

if(BUILD_DENG)
    include(cmake/libdeng.cmake)
    include(cmake/ViewModels.cmake)
    include(cmake/TextureDump.cmake)
endif()


# Non-experimental tests
if(BUILD_TESTS)
    message(STATUS "Adding test build configurations")
    include(cmake/tests/TriangleApp.cmake)
    include(cmake/tests/VulkanImGui.cmake)
    include(cmake/tests/OpenGLImGui.cmake)

    if(EXPERIMENTAL_BUILD_MODEL_LOADER)
    endif()

    if(EXPERIMENTAL_BUILD_LUA_TESTS)
        include(cmake/tests/LuaTest.cmake)
    endif()

    if(EXPERIMENTAL_BUILD_CUBIC_SPLINE_TESTS)
        include(cmake/tests/CubicsInterp.cmake)
    endif()

    if(EXPERIMENTAL_BUILD_MATRIX_TESTS)
        include(cmake/tests/MatrixTest.cmake)
    endif()
endif()
