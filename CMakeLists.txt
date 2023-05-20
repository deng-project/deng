# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: CMakeFiles.txt - main CMake configuration file
# author: Karl-Mihkel Ott

cmake_minimum_required(VERSION 3.10)
project(deng)

# Constant config values
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT DEFINED VULKAN_VERSION)
	set(VULKAN_VERSION "1.3.243.0")
endif()

set(VULKAN_SDK_PATH "C:/VulkanSDK/${VULKAN_VERSION}")

include(${CMAKE_CURRENT_SOURCE_DIR}/deps/vcpkg/scripts/buildsystems/vcpkg.cmake)

option(DENG_BUILD_TESTS "Build DENG testing applications" ON)
option(DENG_USE_LIBDAS_GIT "Use git version of libdas instead of the release version (requires more bandwidth)" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# List of dependencies to install
#	wxWidgets:x64-windows
#	vulkan-headers:x64-windows
# 	spirv-tools:x64-windows
#	shaderc:x64-windows
#	ode:x64-windows
#	python3:x64-windows
# 	pybind11
find_package(VulkanHeaders CONFIG REQUIRED)
find_package(ode CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)

# SPRIV dependency madness
find_package(SPIRV-Tools CONFIG REQUIRED)
find_package(SPIRV-Tools-opt CONFIG REQUIRED)
find_package(SPIRV-Tools-link CONFIG REQUIRED)
find_package(SPIRV-Tools-lint CONFIG REQUIRED)

find_package(unofficial-shaderc_util CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
find_package(Python3 COMPONENTS Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)
find_package(cryptopp CONFIG REQUIRED)


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


# Libnwin configuration
set(NEKOWIN_BUILD_STATIC_LIB ON CACHE BOOL "Build static nekowin library")
set(NEKOWIN_BUILD_SHARED_LIB OFF CACHE BOOL "Build shared nekowin library")
set(NEKOWIN_BUILD_GLAPP OFF CACHE BOOL "Build glapp application")
set(NEKOWIN_BUILD_GAMEPAD_INFO OFF CACHE BOOL "Build a sample console application for testing controller inputs")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/nekowin)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/mar)

# Libdas and dastool configuration
set(LIBDAS_BUILD_TESTS OFF CACHE BOOL "Build libdas tests")
set(LIBDAS_BUILD_STATIC_LIB ON CACHE BOOL "Build static libdas library")
set(LIBDAS_BUILD_SHARED_LIB OFF CACHE BOOL "Build shared libdas library")
set(LIBDAS_BUILD_DASTOOL ON CACHE BOOL "Build dastool application")
set(LIBDAS_BUILD_TESTS OFF CACHE BOOL "Build libdas testing applications")
set(LIBDAS_BUILD_DEPENDENCIES OFF CACHE BOOL "Build libdas dependencies")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/libdas)
target_include_directories(das-static
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/trs/include)

# dengxml configuration
set(DENGXML_BUILD_TESTS OFF CACHE BOOL "Build dengxml tests")
set(DENGXML_BUILD_DEPENDENCIES OFF CACHE BOOL "Build all dengxml dependencies")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/dxml)
target_include_directories(dxml
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/trs/include)

include(cmake/libdeng.cmake)
include(cmake/TextureDump.cmake)

# Pydeng include
include(cmake/pydeng.cmake)
include(cmake/rt.cmake)

include(cmake/deng-game-editor.cmake)

# Non-experimental tests
if(DENG_BUILD_TESTS)
    message(STATUS "Adding test build configurations")
	set(MANUAL_TEST_DIR ManualTests)
	
	include(cmake/tests/TriangleApp.cmake)
endif()
