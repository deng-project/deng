# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: ViewModels.cmake - ModelLoader application build config
# author: Karl-Mihkel Ott

set(DENG_GAME_EDITOR_TARGET deng-game-editor)
set(DENG_GAME_EDITOR_HEADERS 
    include/deng/Executables/deng-game-editor/GameEditor.h
	include/deng/Executables/deng-game-editor/GameEditorApp.h
	include/deng/Executables/deng-game-editor/NewProjectWizard.h
    include/deng/Executables/deng-game-editor/ProjectManager.h
	include/deng/Executables/deng-game-editor/StaticResources.h)
	
set(DENG_GAME_EDITOR_SOURCES 
	src/Executables/deng-game-editor/GameEditor.cpp
	src/Executables/deng-game-editor/GameEditorApp.cpp
	src/Executables/deng-game-editor/NewProjectWizard.cpp
    src/Executables/deng-game-editor/ProjectManager.cpp
	src/Executables/deng-game-editor/StaticResources.cpp)

add_executable(${DENG_GAME_EDITOR_TARGET} ${DENG_GAME_EDITOR_HEADERS} ${DENG_GAME_EDITOR_SOURCES})
add_dependencies(${DENG_GAME_EDITOR_TARGET} ${DENG_COMPLETE_TARGET})

target_include_directories(${DENG_GAME_EDITOR_TARGET}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_link_libraries(${DENG_GAME_EDITOR_TARGET} 
#    PRIVATE ${DENG_COMPLETE_TARGET})

if(WIN32)
	target_include_directories(${DENG_GAME_EDITOR_TARGET}
		PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps/wxWidgets-${WXWIDGETS_VERSION}/include
		PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps/wxWidgets-${WXWIDGETS_VERSION}/include/msvc)
	target_compile_definitions(${DENG_GAME_EDITOR_TARGET}
		PRIVATE __WXMSW__
		PRIVATE WINVER=0x0400
		PRIVATE wxUSE_GUI=1
		PRIVATE _UNICODE)
    target_link_directories(${DENG_GAME_EDITOR_TARGET}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps/wxWidgets-${WXWIDGETS_VERSION}/lib/vc_x64_lib)
	target_link_libraries(${DENG_GAME_EDITOR_TARGET}
		PRIVATE wxbase32u
		PRIVATE wxexpat
		PRIVATE wxmsw32u_core)
endif()
