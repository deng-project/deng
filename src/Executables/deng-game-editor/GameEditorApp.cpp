// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GameEditorApp.cpp - Main entry point for DENG game editor application
// author: Karl-Mihkel Ott

#define GAME_EDITOR_APP_CPP
#include "deng/Executables/deng-game-editor/GameEditorApp.h"


wxIMPLEMENT_APP(DENG::Editor::GameEditorApp);

namespace DENG {
    namespace Editor {

        bool GameEditorApp::OnInit()
        {
            wxInitAllImageHandlers();
            //ProjectManager* frame = new ProjectManager();
            //frame->Show(true);
            GameEditor* editor = new GameEditor();
            editor->Show(true);

            return true;
        }

        void GameEditorApp::OnUnhandledException() {
            try {
                throw;
            }
            catch (std::exception& e) {
                MessageBoxA(NULL, e.what(), "Unhandled C++ Exception Caught", MB_OK);
            }
        }

        bool GameEditorApp::OnExceptionInMainLoop() {
            try { 
                throw;  
            }
            catch (std::exception& e) {
                MessageBoxA(NULL, e.what(), "C++ Exception Caught", MB_OK);
            }

            return true;
        }
    }

}
