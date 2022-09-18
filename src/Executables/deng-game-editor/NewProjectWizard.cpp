// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: NewProjectWizard.h - New project wizard gui implementation
// author: Karl-Mihkel Ott

#define NEW_PROJECT_WIZARD_CPP
#include "deng/Executables/deng-game-editor/NewProjectWizard.h"

namespace DENG {
    namespace Editor {

        wxBEGIN_EVENT_TABLE(NewProjectWizard, wxFrame)
            EVT_BUTTON(ID_NEW_PROJECT_WIZARD_CHOOSE_DIRECTORY, NewProjectWizard::_OnChooseDirectoryClick)
            EVT_BUTTON(ID_NEW_PROJECT_WIZARD_CREATE_NEW_PROJECT, NewProjectWizard::_OnCreateNewProjectClick)
            EVT_BUTTON(ID_NEW_PROJECT_WIZARD_CANCEL, NewProjectWizard::_OnCancelClick)
        wxEND_EVENT_TABLE()

        NewProjectWizard::NewProjectWizard(wxFrame* _parent, ProjectDataManager *_p_mgr) :
            wxFrame(_parent, ID_NEW_PROJECT_WIZARD, "Create a new project"),
            m_data_mgr(_p_mgr)
        {
            wxIcon icon;
            wxBitmap bmp = wxBitmap::NewFromPNGData(GetLogoIcon32x32(), GetLogoIcon32x32Size());
            icon.CopyFromBitmap(bmp);
            SetIcon(icon);

            wxBoxSizer* property_sizer = new wxBoxSizer(wxVERTICAL);
            // project root directory
            property_sizer->Add(new wxStaticText(this, wxID_ANY, "Project root directory"),
                                0, wxALIGN_LEFT | wxLEFT | wxTOP, 10);
            wxBoxSizer* path_sizer = new wxBoxSizer(wxHORIZONTAL);
            path_sizer->Add(new wxTextCtrl(this, ID_NEW_PROJECT_WIZARD_PATH, _GetDefaultProjectPath(), wxDefaultPosition, wxSize(300, 20)),
                            0, wxALIGN_LEFT | wxLEFT, 10);
            path_sizer->Add(new wxButton(this, ID_NEW_PROJECT_WIZARD_CHOOSE_DIRECTORY, "Choose path"),
                            0, wxRIGHT, 10);
            property_sizer->Add(path_sizer, wxSizerFlags(0).Left());

            // project name
            property_sizer->Add(new wxStaticText(this, wxID_ANY, "Project name"), 0, 
                                wxALIGN_LEFT | wxLEFT | wxTOP, 10);
            property_sizer->Add(new wxTextCtrl(this, ID_NEW_PROJECT_WIZARD_PROJECT_NAME, "New Project", wxDefaultPosition, wxSize(300, 20)),
                                0, wxALIGN_LEFT | wxLEFT, 10);

            // directory name
            property_sizer->Add(new wxStaticText(this, wxID_ANY, "Project directory name"), 0,
                                wxALIGN_LEFT | wxLEFT | wxTOP, 10);
            property_sizer->Add(new wxTextCtrl(this, ID_NEW_PROJECT_WIZARD_DIRECTORY_NAME, "new-project", wxDefaultPosition, wxSize(300, 20)),
                                0, wxALIGN_LEFT | wxLEFT, 10);

            // api selector
            property_sizer->Add(new wxStaticText(this, wxID_ANY, "Default rendering backend"), 0,
                                wxALIGN_LEFT | wxLEFT | wxTOP, 10);
            wxComboBox* backends = new wxComboBox(this, ID_NEW_PROJECT_WIZARD_BACKEND, "Select a default backend", wxDefaultPosition, wxSize(300, 20));
            backends->Insert(wxT("Vulkan"), (unsigned int)DXML::Configuration::Backend::VULKAN);
            backends->Insert(wxT("OpenGL"), (unsigned int) DXML::Configuration::Backend::OPENGL);
            property_sizer->Add(backends, 0, wxALIGN_LEFT | wxLEFT, 10);

            // separator
            property_sizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(450, 2)), 0, wxALL, 10);
            
            // OK / Cancel
            wxBoxSizer* btns = new wxBoxSizer(wxHORIZONTAL);
            btns->Add(new wxButton(this, ID_NEW_PROJECT_WIZARD_CREATE_NEW_PROJECT, "Create new project"), 0, wxALL, 5);
            btns->Add(new wxButton(this, ID_NEW_PROJECT_WIZARD_CANCEL, "Cancel"), 0, wxALL, 5);
            property_sizer->Add(btns, wxSizerFlags(0).Right());

            SetSizerAndFit(property_sizer);
        }


        bool NewProjectWizard::_CreateNewProject() {
            // check for possible errors
            const std::string root_path = ((wxTextCtrl*) wxWindow::FindWindowById(ID_NEW_PROJECT_WIZARD_PATH))->GetLineText(0).ToStdString();
            const std::string dir_name = ((wxTextCtrl*) wxWindow::FindWindowById(ID_NEW_PROJECT_WIZARD_DIRECTORY_NAME))->GetLineText(0).ToStdString();
            const std::string project_name = ((wxTextCtrl*) wxWindow::FindWindowById(ID_NEW_PROJECT_WIZARD_PROJECT_NAME))->GetLineText(0).ToStdString();
            const int api = ((wxComboBox*) wxWindow::FindWindowById(ID_NEW_PROJECT_WIZARD_BACKEND))->GetSelection();

            // check if root_path or dir_name are empty strings
            if (root_path == "") {
                wxMessageBox("Root directory path cannot be empty", "Error", wxICON_ERROR | wxOK);
                return false;
            }

            if (dir_name == "") {
                wxMessageBox("Directory name cannot be empty", "Error", wxICON_ERROR | wxOK);
                return false;
            }

            if (project_name == "") {
                wxMessageBox("Project name cannot be empty", "Error", wxICON_ERROR | wxOK);
                return false;
            }

            if (api != (int) DXML::Configuration::Backend::VULKAN && api != (int) DXML::Configuration::Backend::OPENGL) {
                wxMessageBox("Please select appropriate rendering backend", "Error", wxICON_ERROR | wxOK);
                return false;
            }

#ifdef _WIN32
            if (root_path.back() == '\\') {
                m_data_mgr->SetProjectPath(root_path + dir_name);
            } else {
                m_data_mgr->SetProjectPath(root_path + '\\' + dir_name);
            }
#else
            if (root_path.back() == '/') {
                data.SetProjectPath(root_path + dir_name);
            } else {
                data.SetProjectPath(root_path + '/' + dir_name);
            }
#endif

            m_data_mgr->SetProjectName(project_name);
            m_data_mgr->SetDefaultBackend(DXML::Configuration::VULKAN);
            if (!m_data_mgr->CreateEmptyProject()) {
                wxMessageBox("Could not create an empty project in " + m_data_mgr->GetProjectPath(), "Error", wxICON_ERROR | wxOK);
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

        void NewProjectWizard::_OnCreateNewProjectClick(wxCommandEvent &_ev) {
            if (_CreateNewProject()) {
                m_success_bit = true;
                Close(true);
            }
        }

        void NewProjectWizard::_OnCancelClick(wxCommandEvent& _ev) {
            Close(true);
        }
    }
}
