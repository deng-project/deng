# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: VulkanImGui.cmake - Vulkan ImGui application build config
# author: Karl-Mihkel Ott

set(VK_IMGUI_APP_TARGET VulkanImGui)
set(VK_IMGUI_APP_SOURCES tests/VulkanImGui.cpp)

add_executable(${VK_IMGUI_APP_TARGET} ${VK_IMGUI_APP_SOURCES})
add_dependencies(${VK_IMGUI_APP_TARGET} ${DENG_COMPLETE_TARGET})
target_link_libraries(${VK_IMGUI_APP_TARGET} 
    PUBLIC ${DENG_COMPLETE_TARGET})
