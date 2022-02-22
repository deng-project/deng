# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: CMakeFiles.txt - main CMake configuration file
# author: Karl-Mihkel Ott

cmake_minimum_required(VERSION 3.10)
project(deng LANGUAGES CXX C)
set(CXX_STANDARD C++17)
set(C_STANDARD C99)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(VULKAN_PATH "<Path>")
option(BUILD_DEPS "Build DENG dependencies" OFF)
option(BUILD_TESTS "Build DENG testing applications" ON)
option(BUILD_DENG "Build all DENG targets" ON)
option(AUTO_LOCATE_VULKAN_SDK "Automatically attempt to find VulkanSDK path" ON)

# Search for Vulkan SDK
if(AUTO_LOCATE_VULKAN_SDK)
    message(STATUS "Attempting to find Vulkan SDK using cmake ...")
    find_package(Vulkan)

    if(NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")
        set(VULKAN_PATH ${Vulkan_INCLUDE_DIRS})
        STRING(REGEX REPLACE "/Include" "" VULKAN_PATH ${VULKAN_PATH})
        message(STATUS "Found Vulkan SDK at: ${VULKAN_PATH}")
    else()
        message(STATUS "Could not find Vulkan SDK path using find_package module, reattempting ...")
        if(EXISTS "${VULKAN_PATH}")
            message(STATUS "Found Vulkan SDK path: ${VULKAN_PATH}")
        else()
            message(FATAL_ERROR "Could not find Vulkan SDK, please specify the SDK path manually using VULKAN_PATH variable")
        endif()
    endif()
else(NOT EXISTS "${VULKAN_PATH}")
    message(FATAL_ERROR "Invalid Vulkan SDK path: ${VULKAN_PATH}")
endif()

# Check if debugging definition should be considered
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
endif()

# Check if current operating system is supported
if(BUILD_DEPS OR BUILD_ALL)
    include(cmake/libdas.cmake)
    include(cmake/libnwin.cmake)
endif()

if(BUILD_DENG)
    include(cmake/libdeng.cmake)
endif()


if(BUILD_TESTS)
    message(STATUS "Adding test build configurations")
    include(cmake/tests/TriangleApp.cmake)
endif()
