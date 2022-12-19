// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: NewProjectWizard.h - New project wizard gui header
// author: Karl-Mihkel Ott

#ifndef NEW_PROJECT_WIZARD_H
#define NEW_PROJECT_WIZARD_H

#ifdef NEW_PROJECT_WIZARD_CPP
    #include <wx/wx.h>
    #include <wx/statline.h>

#ifdef _WIN32
    #include <ShlObj.h>
#endif
    
    #include "deng/deng.h"

    #include "deng/Executables/deng-game-editor/StaticResources.h"
    #include "deng/Executables/deng-game-editor/GuiIds.h"
#endif

namespace DENG {
	namespace Editor {
        
        class NewProjectWizard : public wxDialog {
            private:
                // wxWidgets
                wxBoxSizer* m_sizer;
                wxStaticText* m_project_name_desc = nullptr;
                wxTextCtrl* m_project_name_ctrl = nullptr;

                wxStaticText* m_project_root_desc = nullptr;
                wxStaticText* m_picked_root_dir = nullptr;
                wxButton* m_project_root_picker = nullptr;
                
                wxCheckBox* m_use_xml_checkbox = nullptr;

                wxButton* m_ok_btn = nullptr;
                wxButton* m_cancel_btn = nullptr;

                std::string m_root_dir;
                std::string m_project_name;
                bool m_use_xml = true;

                bool m_success_bit = false;

            private:
                bool _CreateNewProject();
                const std::string _GetDefaultProjectPath();

                void _OnChooseDirectoryClick(wxCommandEvent &_ev);
                void _OnCheckUseXMLGame(wxCommandEvent& _ev);
                void _OnCreateNewProjectClick(wxCommandEvent& _ev);
                void _OnCancelClick(wxCommandEvent& _ev);

                wxDECLARE_EVENT_TABLE();

            public:
                NewProjectWizard(wxWindow* _parent);
                
                inline bool Success() {
                    return m_success_bit;
                }

                inline const std::string &GetRootDir() const {
                    return m_root_dir;
                }

                inline const std::string &GetProjectName() const {
                    return m_project_name;
                }

                inline bool GetUseXMLBit() const {
                    return m_use_xml;
                }
        };
	}
}

#endif