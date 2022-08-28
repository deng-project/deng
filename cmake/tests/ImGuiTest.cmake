# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: ImGuiTest.cmake - ImGui test application cmake configuration file
# author: Karl-Mihkel Ott

# OpenGL ImGui application
set(OGL_IMGUI_TEST_TARGET OpenGLImGui)
set(OGL_IMGUI_TEST_HEADERS tests/ImGuiTest.h)
set(OGL_IMGUI_TEST_SOURCES tests/OpenGLImGui.cpp)

add_executable(${OGL_IMGUI_TEST_TARGET} 
	${OGL_IMGUI_TEST_HEADERS}
	${OGL_IMGUI_TEST_SOURCES})
add_dependencies(${OGL_IMGUI_TEST_TARGET} ${DENG_COMPLETE_TARGET})
target_link_libraries(${OGL_IMGUI_TEST_TARGET} ${DENG_COMPLETE_TARGET})

# Vulkan ImGui application
set(VK_IMGUI_TEST_TARGET VulkanImGui)
set(VK_IMGUI_TEST_HEADERS tests/ImGuiTest.h)
set(VK_IMGUI_TEST_SOURCES tests/VulkanImGui.cpp)

add_executable(${VK_IMGUI_TEST_TARGET} 
	${VK_IMGUI_TEST_HEADERS}
	${VK_IMGUI_TEST_SOURCES})
add_dependencies(${VK_IMGUI_TEST_TARGET} ${DENG_COMPLETE_TARGET})
target_link_libraries(${VK_IMGUI_TEST_TARGET} ${DENG_COMPLETE_TARGET})
