// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GameEditor.h - Main game editor gui header
// author: Karl-Mihkel Ott

#ifndef GAME_EDITOR_H
#define GAME_EDITOR_H

#ifdef GAME_EDITOR_CPP
    #include <any>
    #include <wx/wx.h>
    #include <wx/treectrl.h>
    #include <wx/aui/aui.h>

    #include "deng/deng.h"

    #include "deng/Executables/deng-game-editor/GuiIds.h"
    #include "deng/Executables/deng-game-editor/StaticResources.h"
    #include "deng/Executables/deng-game-editor/StaticResources.h"
    #include "deng/Executables/deng-game-editor/OpenGLLoader.h"
    #include "deng/Executables/deng-game-editor/RendererViewport.h"
    #include "deng/Executables/deng-game-editor/NewProjectWizard.h"
    #include "deng/Executables/deng-game-editor/EditorCameraController.h"
#endif


namespace DENG {
    namespace Editor {

        class GameEditor : public wxFrame {
            private:
                wxAuiManager m_mgr;

                wxToolBar* m_toolbar = nullptr;
                wxTreeCtrl* m_hierarchy = nullptr;
                RendererViewport* m_viewport = nullptr;
                wxTextCtrl* m_assets = nullptr;
                wxTextCtrl* m_inspector = nullptr;

                wxMenuBar* m_menubar = nullptr;
                EditorCamera* m_camera = nullptr;
                GridGenerator* m_grid = nullptr;

                ProjectDataManager m_project;

            private:
                void _CreateMenubar();
                void _CreateRuntimeToolbar();
                void _CreateEditorLayout();
                void _SetupViewport();
                void _LoadProject();

                void _OnNewProjectWizardClose(wxCloseEvent& _ev);

                // file
                void _OnNewClick(wxCommandEvent& _ev);
                void _OnOpenClick(wxCommandEvent& _ev);
                void _OnSaveClick(wxCommandEvent& _ev);

                // file::import
                void _OnDasImportClick(wxCommandEvent& _ev);
                void _OnGLTFImportClick(wxCommandEvent& _ev);
                void _OnWavefrontOBJImportClick(wxCommandEvent& _ev);
                void _OnSTLImportClick(wxCommandEvent& _ev);

                // add
                void _OnNodeClick(wxCommandEvent& _ev);
                
                // add::mesh
                void _OnSphereMeshClick(wxCommandEvent& _ev);
                void _OnCubeMeshClick(wxCommandEvent& _ev);
                void _OnConeMeshClick(wxCommandEvent& _ev);

                // add::light
                void _OnDirectionalLightClick(wxCommandEvent& _ev);
                void _OnPointLightClick(wxCommandEvent& _ev);
                void _OnAmbientLightClick(wxCommandEvent& _ev);
                
                // add::camera
                void _OnFirstPersonCameraClick(wxCommandEvent& _ev);
                void _OnThirdPersonCameraClick(wxCommandEvent& _ev);
                void _OnEditorCameraClick(wxCommandEvent& _ev);

                wxDECLARE_EVENT_TABLE();

            public:
                GameEditor();
                ~GameEditor();
        };
    }
}

#endif
