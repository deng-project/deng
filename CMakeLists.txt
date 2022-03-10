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

# Experimental options
option(EXPERIMENTAL_BUILD_IMGUI_VULKAN "Build ImGui application with Vulkan backend" OFF)
option(EXPERIMENTAL_BUILD_IMGUI_OPENGL "Build ImGui application with OpenGL backend" OFF)

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
endif()

# Experimental tests
if(EXPERIMENTAL_BUILD_IMGUI_VULKAN OR EXPERIMENTAL_BUILD_IMGUI_OPENGL)
    if(EXPERIMENTAL_BUILD_IMGUI_VULKAN)
        message(STATUS "Adding Vulkan ImGui application test build config")
        include(cmake/tests/VulkanImGui.cmake)
    endif()

    if(EXPERIMENTAL_BUILD_IMGUI_OPENGL)
        message(STATUS "Adding OpenGL ImGui application test build config")
        include(cmake/tests/OpenGLImGui.cmake)
    endif()

    add_custom_command(TARGET ${OGL_IMGUI_APP_TARGET} ${VK_IMGUI_APP_TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/ImGuiTestAppData
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy_directory ${CMAKE_SOURCE_DIR}/tests/ImGuiTestAppData ${CMAKE_CURRENT_BINARY_DIR}/ImGuiTestAppData
    )
endif()
