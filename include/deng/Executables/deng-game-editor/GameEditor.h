// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GameEditor.h - Main game editor gui header
// author: Karl-Mihkel Ott

#ifndef GAME_EDITOR_H
#define GAME_EDITOR_H

#ifdef GAME_EDITOR_CPP
    #include <wx/wx.h>
    #include <wx/aui/aui.h>

    #include "deng/Executables/deng-game-editor/GuiIds.h"
    #include "deng/Executables/deng-game-editor/StaticResources.h"
#endif


namespace DENG {
    namespace Editor {

        class GameEditor : public wxFrame {
            private:
                wxAuiManager m_mgr;
                wxMenuBar* m_menubar = nullptr;

            public:
                GameEditor();
                ~GameEditor();
        };
    }
}

#endif
