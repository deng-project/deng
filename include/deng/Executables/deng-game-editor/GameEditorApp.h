// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GameEditorApp.h - Main entry point for DENG game editor application header
// author: Karl-Mihkel Ott

#ifndef GAME_EDITOR_APP_H
#define GAME_EDITOR_APP_H

#ifdef GAME_EDITOR_APP_CPP
    #include <wx/wx.h>
    #include <wx/aui/aui.h>
    #include <string>    
    #include <unordered_map>

    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Quaternion.h"
    #include "trs/Matrix.h"

    #include "deng/Api.h"
    #include "deng/BaseTypes.h"
    #include "deng/Entity.h"
    #include "deng/ScriptableEntity.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/ModelUniforms.h"
    #include "deng/Renderer.h"
    #include "deng/GridGenerator.h"
    #include "deng/Camera3D.h"
    #include "deng/EditorCamera.h"

    #include "deng/Executables/deng-game-editor/ProjectManager.h"
    #include "deng/Executables/deng-game-editor/StaticResources.h"
    #include "deng/Executables/deng-game-editor/GameEditor.h"
    #include "deng/Version.h"
    
    #ifdef _WIN32
        #pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup")
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