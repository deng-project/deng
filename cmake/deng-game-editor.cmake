# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: deng-game-editor.cmake - DENG game editor program cmake configuration
# author: Karl-Mihkel Ott

set(DENG_GAME_EDITOR_TARGET deng-game-editor)
set(DENG_GAME_EDITOR_HEADERS 
	include/deng/Executables/deng-game-editor/EditorCameraController.h
    include/deng/Executables/deng-game-editor/GameEditor.h
	include/deng/Executables/deng-game-editor/GameEditorApp.h
    include/deng/Executables/deng-game-editor/GuiIds.h
	include/deng/Executables/deng-game-editor/NewProjectWizard.h
	include/deng/Executables/deng-game-editor/OpenGLLoader.h
    include/deng/Executables/deng-game-editor/ProjectManager.h
	include/deng/Executables/deng-game-editor/RendererViewport.h
	include/deng/Executables/deng-game-editor/StaticResources.h
)
	
set(DENG_GAME_EDITOR_SOURCES 
	src/Executables/deng-game-editor/EditorCameraController.cpp
	src/Executables/deng-game-editor/GameEditor.cpp
	src/Executables/deng-game-editor/GameEditorApp.cpp
	src/Executables/deng-game-editor/NewProjectWizard.cpp
    src/Executables/deng-game-editor/ProjectManager.cpp
	src/Executables/deng-game-editor/RendererViewport.cpp
	src/Executables/deng-game-editor/StaticResources.cpp
)

if(WIN32)
	list(APPEND DENG_GAME_EDITOR_HEADERS
		include/deng/Executables/deng-game-editor/OpenGLLoaderWin32.h)
	list(APPEND DENG_GAME_EDITOR_SOURCES
		src/Executables/deng-game-editor/OpenGLLoaderWin32.cpp)
endif()

add_executable(${DENG_GAME_EDITOR_TARGET} ${DENG_GAME_EDITOR_HEADERS} ${DENG_GAME_EDITOR_SOURCES})
add_dependencies(${DENG_GAME_EDITOR_TARGET} ${DENG_COMPLETE_TARGET})

target_include_directories(${DENG_GAME_EDITOR_TARGET}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${DENG_GAME_EDITOR_TARGET} 
   PRIVATE ${DENG_COMPLETE_TARGET})

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
else()
    target_include_directories(${DENG_GAME_EDITOR_TARGET}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps/wxWidgets-${WXWIDGETS_VERSION}/include/wx-3.2
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps/wxWidgets-${WXWIDGETS_VERSION}/lib/wx/include/gtk3-unicode-static-3.2)
    target_compile_definitions(${DENG_GAME_EDITOR_TARGET}
        PRIVATE __WXGTK__
        PRIVATE _FILE_OFFSET_BITS=64)
    target_link_directories(${DENG_GAME_EDITOR_TARGET}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps/wxWidgets-${WXWIDGETS_VERSION}/lib)
    target_link_libraries(${DENG_GAME_EDITOR_TARGET}    
        PRIVATE wx_gtk3u-3.2
        PRIVATE gstplayer-1.0
        PRIVATE gstvideo-1.0
        PRIVATE gstbase-1.0
        PRIVATE gstreamer-1.0
        PRIVATE gobject-2.0
        PRIVATE glib-2.0
        PRIVATE gtk-3
        PRIVATE gdk-3
        PRIVATE z
        PRIVATE pangocairo-1.0
        PRIVATE pango-1.0
        PRIVATE harfbuzz
        PRIVATE atk-1.0
        PRIVATE cairo-gobject
        PRIVATE cairo
        PRIVATE gdk_pixbuf-2.0
        PRIVATE gio-2.0
        PRIVATE gobject-2.0
        PRIVATE gthread-2.0
        PRIVATE -pthread
        PRIVATE glib-2.0
        PRIVATE X11
        PRIVATE Xxf86vm
        PRIVATE SM
        PRIVATE gspell-1
        PRIVATE gtk-3
        PRIVATE gdk-3
        PRIVATE z
        PRIVATE pangocairo-1.0
        PRIVATE pango-1.0
        PRIVATE harfbuzz
        PRIVATE atk-1.0
        PRIVATE cairo-gobject
        PRIVATE cairo
        PRIVATE gdk_pixbuf-2.0
        PRIVATE gio-2.0
        PRIVATE gobject-2.0
        PRIVATE glib-2.0
        PRIVATE enchant-2
        PRIVATE gtk-3
        PRIVATE gdk-3
        PRIVATE z
        PRIVATE pangocairo-1.0
        PRIVATE pango-1.0
        PRIVATE harfbuzz
        PRIVATE atk-1.0
        PRIVATE cairo-gobject
        PRIVATE cairo
        PRIVATE gdk_pixbuf-2.0
        PRIVATE gio-2.0
        PRIVATE gobject-2.0
        PRIVATE glib-2.0
        PRIVATE notify
        PRIVATE gdk_pixbuf-2.0
        PRIVATE gio-2.0
        PRIVATE gobject-2.0
        PRIVATE glib-2.0
        PRIVATE Xtst
        PRIVATE pangoft2-1.0
        PRIVATE pango-1.0
        PRIVATE gobject-2.0
        PRIVATE glib-2.0
        PRIVATE harfbuzz
        PRIVATE fontconfig
        PRIVATE freetype
        PRIVATE wxtiff-3.2
        PRIVATE wxjpeg-3.2
        PRIVATE wxpng-3.2
        PRIVATE wxregexu-3.2
        PRIVATE wxscintilla-3.2
        PRIVATE wxexpat-3.2
        PRIVATE wxzlib-3.2
        PRIVATE secret-1
        PRIVATE gio-2.0
        PRIVATE gobject-2.0
        PRIVATE glib-2.0
        PRIVATE curl
        PRIVATE m)
endif()
