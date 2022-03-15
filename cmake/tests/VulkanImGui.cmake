# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: VulkanImGui.cmake - Vulkan ImGui application build config
# author: Karl-Mihkel Ott

set(VK_IMGUI_APP_TARGET VulkanImGui)
set(VK_IMGUI_APP_SOURCES tests/VulkanImGui.cpp)

add_executable(${VK_IMGUI_APP_TARGET} ${VK_IMGUI_APP_SOURCES})

target_include_directories(${VK_IMGUI_APP_TARGET}
    PRIVATE include
    PRIVATE deps
    PRIVATE deps/nekowin/include/third_party
)

if(WIN32)
	target_link_libraries(${VK_IMGUI_APP_TARGET} ${DENG_STATIC_TARGET})
else()
	target_link_libraries(${VK_IMGUI_APP_TARGET} ${DENG_SHARED_TARGET})
endif()