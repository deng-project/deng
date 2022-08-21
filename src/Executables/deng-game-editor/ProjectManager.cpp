// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProjectManager.cpp - Project manager gui implementation
// author: Karl-Mihkel Ott

#define PROJECT_MANAGER_CPP
#include "deng/Executables/deng-game-editor/ProjectManager.h"



namespace DENG {
	namespace Editor {

        BEGIN_EVENT_TABLE(LogoPanel, wxPanel)
            EVT_PAINT(LogoPanel::PaintEvent)
            END_EVENT_TABLE()

            LogoPanel::LogoPanel(wxFrame* _parent) :
            wxPanel(_parent, wxID_ANY, wxDefaultPosition, wxSize(280, 280))
        {
            SetBackgroundColour(wxColor(*wxWHITE));
            m_image = wxBITMAP_PNG_FROM_DATA(g_logo_small);
        }


        void LogoPanel::PaintEvent(wxPaintEvent& _ev) {
            wxClientDC dc(this);
            dc.DrawBitmap(m_image, 0, 0, true);
        }

        wxBEGIN_EVENT_TABLE(ProjectManager, wxFrame)
            // Project manager callbacks
            EVT_BUTTON(ID_NEW_PROJECT, ProjectManager::_OnNewProject)
            EVT_BUTTON(ID_OPEN_PROJECT, ProjectManager::_OnOpenProject)

            // About button callbacks
            EVT_BUTTON(ID_DENG_WEBSITE, ProjectManager::_OnWebsite)
            EVT_BUTTON(ID_DENG_FORUM, ProjectManager::_OnForum)
            EVT_BUTTON(ID_GIT, ProjectManager::_OnGit)
        wxEND_EVENT_TABLE()


        ProjectManager::ProjectManager()
            : wxFrame(NULL, wxID_ANY, "DENG project manager", wxDefaultPosition, wxDefaultSize, wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN)
        {
            wxIcon icon;
            wxBitmap bmp = wxBITMAP_PNG_FROM_DATA(g_32x32);
            icon.CopyFromBitmap(bmp);
            SetIcon(icon);

            SetBackgroundColour(wxColor(*wxWHITE));
            wxBoxSizer* prj_btn_sizer = new wxBoxSizer(wxHORIZONTAL);
            prj_btn_sizer->Add(
                new wxButton(this, ID_NEW_PROJECT, "New project", wxDefaultPosition, PROJECT_BTN_SIZE),
                0,
                wxALL,
                10
            );

            prj_btn_sizer->Add(
                new wxButton(this, ID_OPEN_PROJECT, "Open project", wxDefaultPosition, PROJECT_BTN_SIZE),
                0,
                wxALL,
                10
            );

            wxBoxSizer* about_btn_sizer = new wxBoxSizer(wxHORIZONTAL);
            about_btn_sizer->Add(
                new wxButton(this, ID_DENG_WEBSITE, "DENG website", wxDefaultPosition, ABOUT_BTN_SIZE),
                0,
                wxALL,
                10
            );
            about_btn_sizer->Add(
                new wxButton(this, ID_DENG_FORUM, "Forum", wxDefaultPosition, ABOUT_BTN_SIZE),
                0,
                wxALL,
                10
            );
            about_btn_sizer->Add(
                new wxButton(this, ID_GIT, "Git repositories", wxDefaultPosition, ABOUT_BTN_SIZE),
                0,
                wxALL,
                10
            );

            wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
            main_sizer->Add(
                new LogoPanel(this),
                0,
                wxALIGN_CENTER
            );
            main_sizer->Add(
                new wxStaticText(this, wxID_ANY, "Welcome to DENG! Please either select an existing project or create a new one to continue."),
                0,
                wxALIGN_CENTER | wxALL,
                10
            );

            main_sizer->Add(prj_btn_sizer, wxSizerFlags(0).Center());
            main_sizer->Add(about_btn_sizer, wxSizerFlags(0).Center());
            main_sizer->Add(
                new wxStaticText(this, wxID_ANY, std::string("Version: " + std::to_string(DENG_VERSION_MAJOR) + "." + std::to_string(DENG_VERSION_MINOR) + "." + std::to_string(DENG_VERSION_REVISION))),
                0,
                wxALIGN_CENTER | wxTOP | wxBOTTOM,
                10
            );
            SetSizerAndFit(main_sizer);
        }

        // Event handler callbacks for ProjectManager
        void ProjectManager::_OnNewProject(wxCommandEvent& _ev) {
            NewProjectWizard* wiz = new NewProjectWizard(this);
            wiz->Show(true);
            _ev.Skip();
        }

        void ProjectManager::_OnOpenProject(wxCommandEvent& _ev) {
            wxMessageBox("This button opens an existing DENG project", "About 'Open Project'", wxOK | wxICON_INFORMATION);
            _ev.Skip();
        }

        void ProjectManager::_OnWebsite(wxCommandEvent& _ev) {
            wxLaunchDefaultBrowser("https://dengproject.org");
            _ev.Skip();
        }

        void ProjectManager::_OnForum(wxCommandEvent& _ev) {
            wxLaunchDefaultBrowser("https://forum.dengproject.org");
            _ev.Skip();
        }

        void ProjectManager::_OnGit(wxCommandEvent& _ev) {
            wxLaunchDefaultBrowser("https://git.dengproject.org/deng");
            _ev.Skip();
        }
	}
}