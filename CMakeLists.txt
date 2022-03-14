# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: CMakeFiles.txt - main CMake configuration file
# author: Karl-Mihkel Ott

cmake_minimum_required(VERSION 3.10)
project(deng LANGUAGES CXX C)

# Constant config values
set(CXX_STANDARD C++17)
set(C_STANDARD C99)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(VULKAN_SDK_PATH CACHE STRING "")
option(BUILD_DEPS "Build DENG dependencies" OFF)
option(BUILD_TESTS "Build DENG testing applications" ON)
option(BUILD_DENG "Build all DENG targets" ON)

# Set appropriate compiler flags
if(MSVC)
    add_compile_options(/W4 /WX)

    # Set correct optimisation flags
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(/Od)
    else()
        add_compile_options(/O2)
    endif()
else()
    add_compile_options(-Wall -Wextra)

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
if(BUILD_DEPS OR BUILD_ALL)
    include(cmake/libdas.cmake)
    include(cmake/libnwin.cmake)
    include(cmake/imgui.cmake)
endif()

if(BUILD_DENG)
    include(cmake/libdeng.cmake)
endif()


# Non-experimental tests
if(BUILD_TESTS)
    message(STATUS "Adding test build configurations")
    include(cmake/tests/TriangleApp.cmake)
    include(cmake/tests/VulkanImGui.cmake)
    include(cmake/tests/OpenGLImGui.cmake)
endif()
