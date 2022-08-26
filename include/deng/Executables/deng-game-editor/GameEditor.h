// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GameEditor.h - Main game editor gui header
// author: Karl-Mihkel Ott

#ifndef GAME_EDITOR_H
#define GAME_EDITOR_H

#ifdef GAME_EDITOR_CPP
    #include <any>
    #include <wx/wx.h>
    #include <wx/aui/aui.h>

    #include "deng/deng.h"

    #include "deng/Executables/deng-game-editor/GuiIds.h"
    #include "deng/Executables/deng-game-editor/StaticResources.h"
    #include "deng/Executables/deng-game-editor/OpenGLLoader.h"
    #include "deng/Executables/deng-game-editor/RendererViewport.h"
    #include "deng/Executables/deng-game-editor/EditorCameraController.h"
#endif


namespace DENG {
    namespace Editor {

        class GameEditor : public wxFrame {
            private:
                wxAuiManager m_mgr;
                wxMenuBar* m_menubar = nullptr;
                EditorCamera* m_camera = nullptr;
                GridGenerator* m_grid = nullptr;

            public:
                GameEditor();
                ~GameEditor();
        };
    }
}

#endif
