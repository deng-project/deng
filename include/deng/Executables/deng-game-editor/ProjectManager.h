// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProjectManager.h - Project manager gui header
// author: Karl-Mihkel Ott

#ifndef PROJECT_MANAGER_H
#define PROJECT_MANAGER_H

#ifdef PROJECT_MANAGER_CPP
    #include <filesystem>
    #include <wx/wx.h>
    #include <wx/clrpicker.h>
    #include <wx/valnum.h>
    #include <wx/collpane.h>
    #include <wx/treectrl.h>
    #include <wx/aui/aui.h>    
    #include "deng/deng.h"
    
    #include "deng/Executables/deng-game-editor/GuiIds.h"
    #include "deng/Executables/deng-game-editor/StaticResources.h"
    #include "deng/Executables/deng-game-editor/NewProjectWizard.h"
    #include "deng/Executables/deng-game-editor/StaticResources.h"
    #include "deng/Executables/deng-game-editor/OpenGLLoader.h"
    #include "deng/Executables/deng-game-editor/RendererViewport.h"
    #include "deng/Executables/deng-game-editor/InspectorPanel.h"
    #include "deng/Executables/deng-game-editor/EditorMenubar.h"
    #include "deng/Executables/deng-game-editor/GameEditor.h"
#endif

#define PROJECT_BTN_SIZE wxSize(120, 60)
#define ABOUT_BTN_SIZE wxSize(120, 30)

namespace DENG {
	namespace Editor {

        class LogoPanel : public wxPanel {
            private:
                wxBitmap m_image;

            public:
                LogoPanel(wxWindow* _parent);
                void PaintEvent(wxPaintEvent& _ev);

                DECLARE_EVENT_TABLE();
        };


        class ProjectManager : public wxFrame {
            private:
                wxPanel* m_panel = nullptr;
                wxBoxSizer* m_sizer = nullptr;

                ProjectDataManager *m_project = nullptr;
                bool m_project_owner_bit = true;
            
            private:
                void _OnNewProject(wxCommandEvent& _ev);
                void _OnOpenProject(wxCommandEvent& _ev);
                void _OnRepairProject(wxCommandEvent& _ev);
                void _OnClose(wxCloseEvent& _ev);

                wxDECLARE_EVENT_TABLE();

            public:
                ProjectManager();
                ~ProjectManager();

                inline ProjectDataManager* GetDataManager() {
                    m_project_owner_bit = false;
                    return m_project;
                }
        };
	}
}

#endif