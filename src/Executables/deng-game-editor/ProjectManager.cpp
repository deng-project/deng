// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProjectManager.cpp - Project manager gui implementation
// author: Karl-Mihkel Ott

#define PROJECT_MANAGER_CPP
#include "deng/Executables/deng-game-editor/ProjectManager.h"

namespace DENG {
	namespace Editor {

        wxBEGIN_EVENT_TABLE(LogoPanel, wxPanel)
            EVT_PAINT(LogoPanel::PaintEvent)
        wxEND_EVENT_TABLE()

        LogoPanel::LogoPanel(wxWindow* _parent) :
            wxPanel(_parent, wxID_ANY, wxDefaultPosition, wxSize(280, 280))
        {
            m_image = wxBitmap::NewFromPNGData(GetSmallLogo(), GetSmallLogoSize());
        }


        void LogoPanel::PaintEvent(wxPaintEvent& _ev) {
            wxClientDC dc(this);
            dc.DrawBitmap(m_image, 0, 0, true);
        }

        wxBEGIN_EVENT_TABLE(ProjectManager, wxFrame)
            // Project manager callbacks
            EVT_BUTTON(ID_PROJECT_MANAGER_NEW, ProjectManager::_OnNewProject)
            EVT_BUTTON(ID_PROJECT_MANAGER_OPEN, ProjectManager::_OnOpenProject)
        wxEND_EVENT_TABLE()


        ProjectManager::ProjectManager()
            : wxFrame(NULL, wxID_ANY, "DENG project manager", wxDefaultPosition, wxDefaultSize, wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN)
        {
            m_panel = new wxPanel(this);
            m_sizer = new wxBoxSizer(wxVERTICAL);

            wxIcon icon;
            wxBitmap bmp = wxBitmap::NewFromPNGData(GetLogoIcon32x32(), GetLogoIcon32x32Size());
            icon.CopyFromBitmap(bmp);
            SetIcon(icon);
            
            // Project buttons
            wxBoxSizer* prj_btn_sizer = new wxBoxSizer(wxHORIZONTAL);
            prj_btn_sizer->Add(new wxButton(m_panel, ID_PROJECT_MANAGER_NEW, "New project", wxDefaultPosition, PROJECT_BTN_SIZE),
                               0, wxALL, 10);
            prj_btn_sizer->Add(new wxButton(m_panel, ID_PROJECT_MANAGER_OPEN, "Open project", wxDefaultPosition, PROJECT_BTN_SIZE),
                               0, wxALL, 10);

            // Main sizer with image, text and previously defined buttons
            wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
            main_sizer->Add(new LogoPanel(m_panel),
                            0, wxALIGN_CENTER);
            main_sizer->Add(new wxStaticText(m_panel, wxID_ANY, "Welcome to DENG! Please either select an existing project or create a new one to continue."),
                            0, wxALIGN_CENTER | wxALL, 10);
            main_sizer->Add(prj_btn_sizer, wxSizerFlags(0).Center());
            main_sizer->Add(new wxStaticText(m_panel, wxID_ANY, std::string("Version: " + std::to_string(DENG_VERSION_MAJOR) + "." + std::to_string(DENG_VERSION_MINOR) + "." + std::to_string(DENG_VERSION_REVISION))),
                            0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

            m_panel->SetSizerAndFit(main_sizer);
            m_sizer->Add(m_panel, 1, wxEXPAND);
            SetSizerAndFit(m_sizer);
        }

        // Event handler callbacks for ProjectManager
        void ProjectManager::_OnNewProject(wxCommandEvent& _ev) {
            NewProjectWizard* wiz = new NewProjectWizard(this);
            if (wiz->ShowModal() == wxID_OK) {
                std::string project_dir = wiz->GetProjectName();
                for (auto it = project_dir.begin(); it != project_dir.end(); it++) {
                    if (*it == ' ')
                        *it = '-';
                }

                if (wiz->GetUseXMLBit())
                    m_project = new ProjectDataManager("game.xml", wiz->GetRootDir() + "\\" + project_dir, false);
                else 
                    m_project = new ProjectDataManager("game.dat", wiz->GetRootDir() + "\\" + project_dir, false);
                Close(true);
            }
        }

        void ProjectManager::_OnOpenProject(wxCommandEvent& _ev) {
            wxDirDialog dir(this, wxT("Select a project directory"));
            if (dir.ShowModal() == wxID_OK) {
                std::string project_path = dir.GetPath();
                std::string data_file_name;

                if (std::filesystem::exists(project_path + "\\game.xml"))
                    data_file_name = "game.xml";
                else if (std::filesystem::exists(project_path + "\\game.dat"))
                    data_file_name = "game.dat";
                else {
                    wxMessageBox(wxT("Invalid project directory."), wxT("Data file not found"), wxOK | wxICON_ERROR, this);
                    _ev.Skip();
                    return;
                }

                m_project = new ProjectDataManager(data_file_name, project_path, true);
                Close(true);
            }
            _ev.Skip();
        }
	}
}