// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: NewProjectWizard.h - New project wizard gui implementation
// author: Karl-Mihkel Ott

#define NEW_PROJECT_WIZARD_CPP
#include "deng/Executables/deng-game-editor/NewProjectWizard.h"

namespace DENG {
    namespace Editor {

        wxBEGIN_EVENT_TABLE(NewProjectWizard, wxDialog)
            EVT_BUTTON(ID_NEW_PROJECT_WIZARD_CHOOSE_DIRECTORY, NewProjectWizard::_OnChooseDirectoryClick)
            EVT_BUTTON(ID_NEW_PROJECT_WIZARD_USE_XML_DATA, NewProjectWizard::_OnCheckUseXMLGame)
            EVT_BUTTON(ID_NEW_PROJECT_WIZARD_CREATE_NEW_PROJECT, NewProjectWizard::_OnCreateNewProjectClick)
            EVT_BUTTON(ID_NEW_PROJECT_WIZARD_CANCEL, NewProjectWizard::_OnCancelClick)
        wxEND_EVENT_TABLE()

        NewProjectWizard::NewProjectWizard(wxWindow* _parent) :
            wxDialog(_parent, ID_NEW_PROJECT_WIZARD, "Create a new project", wxDefaultPosition, wxSize(500, 200))
        {
            wxIcon icon;
            wxBitmap bmp = wxBitmap::NewFromPNGData(GetLogoIcon32x32(), GetLogoIcon32x32Size());
            icon.CopyFromBitmap(bmp);
            SetIcon(icon);

            m_sizer = new wxBoxSizer(wxVERTICAL);
            m_project_name_desc = new wxStaticText(this, wxID_ANY, wxT("Project name:"));
            m_project_name_ctrl = new wxTextCtrl(this, ID_NEW_PROJECT_WIZARD_PROJECT_NAME, wxT("New Project"), wxDefaultPosition, wxSize(300, 20));
            m_sizer->Add(m_project_name_desc, 0, wxALIGN_LEFT, 0);
            m_sizer->Add(m_project_name_ctrl, 0, wxALIGN_LEFT, 0);

            m_project_root_desc = new wxStaticText(this, wxID_ANY, wxT("Root directory:"));

            m_picked_root_dir = new wxStaticText(this, ID_NEW_PROJECT_WIZARD_PATH, wxT("Not selected"), wxDefaultPosition, wxSize(200, 20));
            m_project_root_picker = new wxButton(this, ID_NEW_PROJECT_WIZARD_CHOOSE_DIRECTORY, wxT("Choose directory"), wxDefaultPosition, wxSize(100, 30));

            m_sizer->Add(m_project_root_desc, 0, wxALIGN_LEFT, 0);
            m_sizer->Add(m_picked_root_dir, 0, wxALIGN_LEFT, 0);
            m_sizer->Add(m_project_root_picker, 0, wxALIGN_LEFT, 0);
            
            m_use_xml_checkbox = new wxCheckBox(this, ID_NEW_PROJECT_WIZARD_USE_XML_DATA, "Use XML data");
            m_use_xml_checkbox->SetValue(true);
            m_sizer->Add(m_use_xml_checkbox, 0, wxALIGN_LEFT, 0);

            m_ok_btn = new wxButton(this, ID_NEW_PROJECT_WIZARD_CREATE_NEW_PROJECT, wxT("OK"));
            m_cancel_btn = new wxButton(this, ID_NEW_PROJECT_WIZARD_CANCEL, wxT("Cancel"));
            
            wxBoxSizer* btn_sizer = new wxBoxSizer(wxHORIZONTAL);
            btn_sizer->Add(m_ok_btn);
            btn_sizer->Add(m_cancel_btn);

            m_sizer->Add(btn_sizer);

            SetSizerAndFit(m_sizer);
        }


        bool NewProjectWizard::_CreateNewProject() {
            // check for possible errors
            m_project_name = ((wxTextCtrl*) wxWindow::FindWindowById(ID_NEW_PROJECT_WIZARD_PROJECT_NAME))->GetLineText(0).ToStdString();
            m_root_dir = ((wxStaticText*) wxWindow::FindWindowById(ID_NEW_PROJECT_WIZARD_PATH))->GetLabel().ToStdString();

            // check if root_path or dir_name are empty strings
            if (m_root_dir == "Not selected") {
                wxMessageBox("Root directory path cannot be empty", "Error", wxICON_ERROR | wxOK);
                return false;
            }

            if (m_project_name == "") {
                wxMessageBox("Project name cannot be empty", "Error", wxICON_ERROR | wxOK);
                return false;
            }

            return true;
        }


        const std::string NewProjectWizard::_GetDefaultProjectPath() {
#ifdef _WIN32
            WCHAR wpath[MAX_PATH] = { 0 };
            if (!SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, wpath))) {
                return "C:\\Users\\user\\Documents\\deng-projects";
            } else {
                CHAR path[MAX_PATH * sizeof(WCHAR)] = {0};
                WideCharToMultiByte(CP_UTF8, 0, wpath, MAX_PATH, path, MAX_PATH * sizeof(WCHAR), NULL, NULL);
                return std::string(path) + "\\Documents\\deng-projects";
            }
#else
            return "(null)";
#endif
        }

        // Callbacks
        void NewProjectWizard::_OnChooseDirectoryClick(wxCommandEvent &_ev) {
            wxDirDialog dir_dialog = wxDirDialog(this, "Choose a project directory");
            if (dir_dialog.ShowModal() == wxID_OK) {
                auto path = dir_dialog.GetPath();
                wxWindow::FindWindowById(ID_NEW_PROJECT_WIZARD_PATH, this)->SetLabel(path);
            }
            _ev.Skip();
        }


        void NewProjectWizard::_OnCheckUseXMLGame(wxCommandEvent& _ev) {
            m_use_xml = m_use_xml_checkbox->IsChecked();
        }


        void NewProjectWizard::_OnCreateNewProjectClick(wxCommandEvent &_ev) {
            if (_CreateNewProject()) {
                m_success_bit = true;
                EndModal(wxID_OK);
            }
        }

        void NewProjectWizard::_OnCancelClick(wxCommandEvent& _ev) {
            Close(true);
        }
    }
}
