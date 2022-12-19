// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProjectManager.h - Project manager gui header
// author: Karl-Mihkel Ott

#ifndef PROJECT_MANAGER_H
#define PROJECT_MANAGER_H

#ifdef PROJECT_MANAGER_CPP
    #include <filesystem>
    #include <wx/wx.h>
    #include "deng/deng.h"
    
    #include "deng/Executables/deng-game-editor/StaticResources.h"
    #include "deng/Executables/deng-game-editor/NewProjectWizard.h"
    #include "deng/Executables/deng-game-editor/GuiIds.h"
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
            
            private:
                void _OnNewProject(wxCommandEvent& _ev);
                void _OnOpenProject(wxCommandEvent& _ev);

                wxDECLARE_EVENT_TABLE();

            public:
                ProjectManager();

                inline ProjectDataManager* GetDataManager() {
                    return m_project;
                }
        };
	}
}

#endif