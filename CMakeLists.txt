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
message(STATUS "Vulkan SDK path is ${VULKAN_SDK_PATH}")

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


# cvar and das2 library builds
set(CVAR_BUILD_DEMOS OFF CACHE BOOL "Build cvar demos")
set(CVAR_BUILD_STATIC ON CACHE BOOL "Build cvar as a static library")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/cvar)

set(DAS2_BUILD_DEMOS OFF CACHE BOOL "Build das2 demos")
set(DAS2_BUILD_STATIC ON CACHE BOOL "Build das2 as a static library")
set(DAS2_BUILD_DASTOOL OFF CACHE BOOL "Build dastool utility")
set(DAS2_BUILD_EXTERNAL_DEPENDENCIES OFF CACHE BOOL "Build external das2 dependencies")

# Include all possible converters
set(DAS2_WAVEFRONT_OBJ ON CACHE BOOL "Build conversion support from and to Wavefront Obj format")
set(DAS2_COLLADA ON CACHE BOOL "Build conversion support from and to Collada (.dae) format")
set(DAS2_GLTF ON CACHE BOOL "Build conversion support from and to GLTF format")
set(DAS2_STL ON CACHE BOOL "Build conversion support from and to STL format")
set(DAS2_STANFORD_PLY ON CACHE BOOL "Build conversion support from and to PLY format")
set(DAS2_STANFORD_X3D ON CACHE BOOL "Build conversion support from and to X3D format")
set(DAS2_FILMBOX_PROPRIETARY ON CACHE BOOL "Build conversion support from and to fbx (built on top of proprietary SDK)")
set(DAS2_FILMBOX_RSE ON CACHE BOOL "Build conversion support from and to fbx (reverse engineered implementation)")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/das2)
target_include_directories(das2
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/trs/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/cvar/Include)

include(CMake/libdeng.cmake)
include(CMake/layers.cmake)
include(CMake/Sandbox.cmake)

# Non-experimental tests
if(DENG_BUILD_TESTS)
    message(STATUS "Adding demo build configurations")
	set(DEMO_APPS_DIR DemoApps)
	
	include(CMake/Demos/SIDTest.cmake)
	include(CMake/Demos/CVarTest.cmake)
	include(CMake/Demos/CompileTimeMapTest.cmake)
	include(CMake/Demos/TriangleApp.cmake)
	include(CMake/Demos/ImGuiApp.cmake)
endif()
