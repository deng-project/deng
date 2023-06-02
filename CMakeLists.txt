# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: CMakeFiles.txt - main CMake configuration file
# author: Karl-Mihkel Ott

cmake_minimum_required(VERSION 3.10)
project(deng)

# Constant config values
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(VULKAN_VERSION "1.3.246.1" CACHE STRING "Vulkan SDK version to use")
set(VULKAN_SDK_PATH "C:/VulkanSDK/${VULKAN_VERSION}" CACHE STRING "Vulkan SDK path to use")

include(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vcpkg/scripts/buildsystems/vcpkg.cmake)

option(DENG_BUILD_DEMOS "Build DENG demo applications" ON)
option(DENG_STATIC "Build DENG as a static library" ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# Set appropriate compiler flags
if(MSVC)
    add_compile_options(/W3 $<$<COMPILE_LANGUAGE:CXX>:/std:c++17> $<$<COMPILE_LANGUAGE:C>:/std:c17> /wd4251 /D_CRTDBG_MAP_ALLOC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

    # Set correct optimisation flags
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/Debug)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/Release)
	set(VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Debug)
else()
    add_compile_options(-Wall -msse4.1 -Wextra $<$<COMPILE_LANGUAGE:CXX>:-std=c++17> $<$<COMPILE_LANGUAGE:C>:-std=c99>)

    if(CMAKE_BUILD_TYPE MATCHES Release)
        add_compile_options(-O3)
    endif()
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/mar)

# Libdas and dastool configuration
set(LIBDAS_BUILD_TESTS OFF CACHE BOOL "Build libdas tests")
set(LIBDAS_BUILD_STATIC_LIB ON CACHE BOOL "Build static libdas library")
set(LIBDAS_BUILD_SHARED_LIB OFF CACHE BOOL "Build shared libdas library")
set(LIBDAS_BUILD_DASTOOL ON CACHE BOOL "Build dastool application")
set(LIBDAS_BUILD_TESTS OFF CACHE BOOL "Build libdas testing applications")
set(LIBDAS_BUILD_DEPENDENCIES OFF CACHE BOOL "Build libdas dependencies")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/libdas)
target_include_directories(das-static
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/trs/include)

# dengxml configuration
set(DENGXML_BUILD_TESTS OFF CACHE BOOL "Build dengxml tests")
set(DENGXML_BUILD_DEPENDENCIES OFF CACHE BOOL "Build all dengxml dependencies")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/dxml)
target_include_directories(dxml
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/trs/include)

include(CMake/libdeng.cmake)

# Non-experimental tests
if(DENG_BUILD_TESTS)
    message(STATUS "Adding demo build configurations")
	set(DEMO_APPS_DIR DemoApps)
	
	include(CMake/Demos/TriangleApp.cmake)
	include(CMake/Demos/ImGuiApp.cmake)
	include(CMake/Demos/BasicLightingDemo.cmake)
endif()
