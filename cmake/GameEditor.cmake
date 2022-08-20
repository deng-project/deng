# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: ViewModels.cmake - ModelLoader application build config
# author: Karl-Mihkel Ott

set(GAME_EDITOR_TARGET deng-game-editor)
set(GAME_EDITOR_HEADERS 
    include/deng/Executables/GameEditor.h
    include/deng/Executables/ProjectManager.h)
set(GAME_EDITOR_SOURCES 
    src/Executables/ProjectManager.cpp)

add_executable(${GAME_EDITOR_TARGET} ${GAME_EDITOR_HEADERS} ${GAME_EDITOR_SOURCES})
add_dependencies(${GAME_EDITOR_TARGET} ${DENG_COMPLETE_TARGET})

target_include_directories(${GAME_EDITOR_TARGET}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps/wxWidgets-${WXWIDGETS_VERSION}/include
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps/wxWidgets-${WXWIDGETS_VERSION}/include/msvc)
target_link_libraries(${GAME_EDITOR_TARGET} 
    PRIVATE ${DENG_COMPLETE_TARGET})

if(WIN32)
	target_compile_definitions(${GAME_EDITOR_TARGET}
		PRIVATE __WXMSW__
		PRIVATE WINVER=0x0400
		PRIVATE wxUSE_GUI=1
		PRIVATE _UNICODE)
    target_link_directories(${GAME_EDITOR_TARGET}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps/wxWidgets-${WXWIDGETS_VERSION}/lib/vc_x64_lib)
	target_link_libraries(${GAME_EDITOR_TARGET}
		PRIVATE wxbase32u
		PRIVATE wxexpat
		PRIVATE wxmsw32u_core)
endif()
