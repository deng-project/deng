# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: deng-game-editor.cmake - DENG game editor program cmake configuration
# author: Karl-Mihkel Ott

set(DENG_GAME_EDITOR_TARGET deng-game-editor)
set(DENG_GAME_EDITOR_HEADERS 
	include/deng/Executables/deng-game-editor/AnimationInspectorPanel.h
	include/deng/Executables/deng-game-editor/AssetPanel.h
	include/deng/Executables/deng-game-editor/CubeGeneratorDialog.h
	include/deng/Executables/deng-game-editor/EditorCameraController.h
	include/deng/Executables/deng-game-editor/EditorMenubar.h
    include/deng/Executables/deng-game-editor/GameEditor.h
	include/deng/Executables/deng-game-editor/GameEditorApp.h
    include/deng/Executables/deng-game-editor/GuiIds.h
	include/deng/Executables/deng-game-editor/InspectorPanel.h
	include/deng/Executables/deng-game-editor/MeshInspectorPanel.h
	include/deng/Executables/deng-game-editor/NewProjectWizard.h
	include/deng/Executables/deng-game-editor/NodeInspectorPanel.h
	include/deng/Executables/deng-game-editor/OpenGLLoader.h
    include/deng/Executables/deng-game-editor/ProjectManager.h
	include/deng/Executables/deng-game-editor/RendererViewport.h
	include/deng/Executables/deng-game-editor/ShaderViewer.h
	include/deng/Executables/deng-game-editor/SkyboxGeneratorDialog.h
	include/deng/Executables/deng-game-editor/StaticResources.h
	include/deng/Executables/deng-game-editor/SphereGeneratorDialog.h
	include/deng/Executables/deng-game-editor/TreeItemWrappers.h
)
	
set(DENG_GAME_EDITOR_SOURCES
	src/Executables/deng-game-editor/AnimationInspectorPanel.cpp
	src/Executables/deng-game-editor/AssetPanel.cpp
	src/Executables/deng-game-editor/CubeGeneratorDialog.cpp
	src/Executables/deng-game-editor/EditorCameraController.cpp
	src/Executables/deng-game-editor/EditorMenubar.cpp
	src/Executables/deng-game-editor/GameEditor.cpp
	src/Executables/deng-game-editor/GameEditorApp.cpp
	src/Executables/deng-game-editor/InspectorPanel.cpp
	src/Executables/deng-game-editor/MeshInspectorPanel.cpp
	src/Executables/deng-game-editor/NewProjectWizard.cpp
	src/Executables/deng-game-editor/NodeInspectorPanel.cpp
    src/Executables/deng-game-editor/ProjectManager.cpp
	src/Executables/deng-game-editor/RendererViewport.cpp
	src/Executables/deng-game-editor/ShaderViewer.cpp
	src/Executables/deng-game-editor/SkyboxGeneratorDialog.cpp
	src/Executables/deng-game-editor/StaticResources.cpp
	src/Executables/deng-game-editor/SphereGeneratorDialog.cpp
)

if(WIN32)
	list(APPEND DENG_GAME_EDITOR_HEADERS
		include/deng/Executables/deng-game-editor/OpenGLLoaderWin32.h)
	list(APPEND DENG_GAME_EDITOR_SOURCES
		src/Executables/deng-game-editor/OpenGLLoaderWin32.cpp)
endif()

add_executable(${DENG_GAME_EDITOR_TARGET} ${DENG_GAME_EDITOR_HEADERS} ${DENG_GAME_EDITOR_SOURCES})
add_dependencies(${DENG_GAME_EDITOR_TARGET} 
	${DENG_COMPLETE_TARGET}
	${RT_TARGET})

find_package(wxWidgets CONFIG REQUIRED)

target_include_directories(${DENG_GAME_EDITOR_TARGET}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${DENG_GAME_EDITOR_TARGET} 
   PRIVATE ${DENG_COMPLETE_TARGET}
   PRIVATE wx::core 
   PRIVATE wx::base
   PRIVATE wx::aui)
