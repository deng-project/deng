// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: NewProjectWizard.h - New project wizard gui header
// author: Karl-Mihkel Ott

#ifndef NEW_PROJECT_WIZARD_H
#define NEW_PROJECT_WIZARD_H

#ifdef NEW_PROJECT_WIZARD_CPP
    #include <wx/wx.h>
    #include <wx/statline.h>
    #include <wx/aui/aui.h>
#ifdef _WIN32
    #include <ShlObj.h>
#endif
    
    #include "deng/deng.h"

    #include "deng/Executables/deng-game-editor/StaticResources.h"
    #include "deng/Executables/deng-game-editor/GuiIds.h"
#endif

namespace DENG {
	namespace Editor {
        
        class NewProjectWizard : public wxFrame {
            private:
                ProjectDataManager *m_data_mgr;
                bool m_success_bit = false;

            private:
                bool _CreateNewProject();
                const std::string _GetDefaultProjectPath();

                void _OnChooseDirectoryClick(wxCommandEvent &_ev);
                void _OnCreateNewProjectClick(wxCommandEvent& _ev);
                void _OnCancelClick(wxCommandEvent& _ev);

                wxDECLARE_EVENT_TABLE();

            public:
                NewProjectWizard(wxFrame* _parent, ProjectDataManager *_p_mgr);
                
                inline bool Success() {
                    return m_success_bit;
                }
        };
	}
}

#endif