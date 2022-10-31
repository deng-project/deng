// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GameEditorApp.h - Main entry point for DENG game editor application header
// author: Karl-Mihkel Ott

#ifndef GAME_EDITOR_APP_H
#define GAME_EDITOR_APP_H

#ifdef GAME_EDITOR_APP_CPP
    #include <any>
    #include <wx/wx.h>
    #include <wx/clrpicker.h>
    #include <wx/valnum.h>
    #include <wx/collpane.h>
    #include <wx/treectrl.h>
    #include <wx/aui/aui.h>
    #include "deng/deng.h"

    #include "deng/Executables/deng-game-editor/ProjectManager.h"
    #include "deng/Executables/deng-game-editor/StaticResources.h"
    #include "deng/Executables/deng-game-editor/OpenGLLoader.h"
    #include "deng/Executables/deng-game-editor/RendererViewport.h"
    #include "deng/Executables/deng-game-editor/InspectorPanel.h"
    #include "deng/Executables/deng-game-editor/EditorMenubar.h"
    #include "deng/Executables/deng-game-editor/GameEditor.h"
    #include "deng/Version.h"
    
    #ifdef _WIN32
        #ifdef __DEBUG
            #pragma comment(linker, "/SUBSYSTEM:CONSOLE /ENTRY:WinMainCRTStartup")
        #else
            #pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup")
        #endif    
    #endif
#endif

namespace DENG {
    namespace Editor {
     
        class GameEditorApp : public wxApp {
            private:
                ProjectManager* m_frame = nullptr;

            public:
                virtual bool OnInit() override;
                virtual void OnUnhandledException() override;
                virtual bool OnExceptionInMainLoop() override;
        };

    }
}

#endif