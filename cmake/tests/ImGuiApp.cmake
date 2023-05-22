# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: ImGuiTest.cmake - ImGui test application cmake configuration file
# author: Karl-Mihkel Ott

set(IMGUI_APP_TARGET ImGuiApp)
set(IMGUI_APP_SOURCES tests/ImGuiApp.cpp)

add_executable(${IMGUI_APP_TARGET} ${IMGUI_APP_SOURCES})
add_dependencies(${IMGUI_APP_TARGET} ${DENG_MINIMAL_TARGET})
target_link_libraries(${IMGUI_APP_TARGET} 
	PRIVATE ${DENG_MINIMAL_TARGET})
set_target_properties(${IMGUI_APP_TARGET} PROPERTIES FOLDER ${MANUAL_TEST_DIR})