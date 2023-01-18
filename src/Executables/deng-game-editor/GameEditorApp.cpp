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
            ProjectManager* frame = new ProjectManager();
            frame->Show(true);

            _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
            return true;
        }

        int GameEditorApp::OnExit() {
            return 0;
        }

        void GameEditorApp::OnUnhandledException() {
            try {
                throw;
            }
            catch (std::exception& e) {
                wxMessageBox(e.what(), "Unhandled C++ Exception Caught", wxICON_ERROR | wxOK);
            }
        }

        bool GameEditorApp::OnExceptionInMainLoop() {
            try { 
                throw;
            }
            catch (std::exception& e) {
                wxMessageBox(e.what(), "C++ Exception Caught", wxICON_ERROR | wxOK);
            }

            return true;
        }
    }

}
