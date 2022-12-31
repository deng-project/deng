// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GameEditor.h - Main game editor gui header
// author: Karl-Mihkel Ott

#ifndef GAME_EDITOR_H
#define GAME_EDITOR_H

#ifdef GAME_EDITOR_CPP
    #include <queue>
    #include <list>
    #include <wx/wx.h>
    #include <wx/clrpicker.h>
    #include <wx/valnum.h>
    #include <wx/collpane.h>
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
    #include "deng/Executables/deng-game-editor/TreeItemWrappers.h"
    #include "deng/Executables/deng-game-editor/InspectorPanel.h"
    #include "deng/Executables/deng-game-editor/CubeGeneratorDialog.h"
    #include "deng/Executables/deng-game-editor/SphereGeneratorDialog.h"
    #include "deng/Executables/deng-game-editor/SkyboxGeneratorDialog.h"
    #include "deng/Executables/deng-game-editor/EditorMenubar.h"
#else
    // forward declarations
    namespace DENG {
        namespace Editor {
            class EditorMenubar;
            class RendererViewport;
            class InspectorPanel;
        }

        class EditorCamera;
        class GridGenerator;
        class ProjectDataManager;
    }
#endif


namespace DENG {
    namespace Editor {

        class GameEditor : public wxFrame {
            private:
                wxAuiManager m_mgr;

                wxToolBar* m_toolbar = nullptr;
                wxStatusBar* m_status = nullptr;
                wxTreeCtrl* m_hierarchy = nullptr;
                wxTreeItemId m_root;
                RendererViewport* m_viewport = nullptr;
                wxTextCtrl* m_assets = nullptr;

                EditorMenubar* m_menubar = nullptr;
                InspectorPanel *m_inspector_panel = nullptr;

                EditorCamera* m_camera = nullptr;
                GridGenerator* m_grid = nullptr;
                Skybox* m_skybox = nullptr;

#ifdef __DEBUG
                TriangleTester* m_triangle = nullptr;
#endif

                ProjectDataManager* m_project;
                std::list<ModelLoader> m_model_loaders;

            private:
                void _CreateStatusBar();
                void _CreateRuntimeToolbar();
                void _CreateEditorLayout();
                void _SetupViewport();
                void _RecurseNodesToHierarchy(wxTreeItemId _root_id, NodeLoader &_node);
                void _LoadProject();

                void _OnNewProjectWizardClose(wxCloseEvent& _ev);

                // tree events
                void _OnHierarchyItemClick(wxTreeEvent& _ev);

                wxDECLARE_EVENT_TABLE();

            public:
                GameEditor(ProjectDataManager *_data_man);
                void AppendModelToHierarchy();

                inline std::list<ModelLoader>& GetModelLoaders() {
                    return m_model_loaders;
                }

                inline EditorCamera* GetCamera() {
                    return m_camera;
                }

                inline RendererViewport* GetViewport() {
                    return m_viewport;
                }

                inline Skybox* GetSkybox() {
                    return m_skybox;
                }

                inline void SetSkybox(Skybox* _skybox) {
                    m_skybox = _skybox;
                }

                ~GameEditor();
        };
    }
}

#endif
