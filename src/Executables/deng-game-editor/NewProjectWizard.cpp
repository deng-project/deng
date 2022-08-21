// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: NewProjectWizard.h - New project wizard gui implementation
// author: Karl-Mihkel Ott

#define NEW_PROJECT_WIZARD_CPP
#include "deng/Executables/deng-game-editor/NewProjectWizard.h"

namespace DENG {
    namespace Editor {

        wxBEGIN_EVENT_TABLE(NewProjectWizard, wxFrame)
            EVT_BUTTON(ID_CHOOSE_DIRECTORY, NewProjectWizard::_OnChooseDirectoryClick)
            EVT_BUTTON(ID_CREATE_NEW_PROJECT, NewProjectWizard::_OnCreateNewProjectClick)
            EVT_BUTTON(ID_CANCEL, NewProjectWizard::_OnCancelClick)
        wxEND_EVENT_TABLE()

        NewProjectWizard::NewProjectWizard(wxFrame* _parent) :
            wxFrame(_parent, wxID_ANY, "Create a new project")
        {
            wxIcon icon;
            wxBitmap bmp = wxBitmap::NewFromPNGData(GetLogoIcon32x32(), GetLogoIcon32x32Size());
            icon.CopyFromBitmap(bmp);
            SetIcon(icon);
            SetBackgroundColour(wxColor(*wxWHITE));

            wxBoxSizer* property_sizer = new wxBoxSizer(wxVERTICAL);
            // project root directory
            property_sizer->Add(new wxStaticText(this, wxID_ANY, "Project root directory"),
                                0, wxALIGN_LEFT | wxLEFT | wxTOP, 10);
            wxBoxSizer* path_sizer = new wxBoxSizer(wxHORIZONTAL);
            path_sizer->Add(new wxTextCtrl(this, ID_PATH, "C:\\Users\\user\\Documents\\deng-projects", wxDefaultPosition, wxSize(300, 20)),
                            0, wxALIGN_LEFT | wxLEFT, 10);
            path_sizer->Add(new wxButton(this, ID_CHOOSE_DIRECTORY, "Choose path"),
                            0, wxRIGHT, 10);
            property_sizer->Add(path_sizer, wxSizerFlags(0).Left());

            // project name
            property_sizer->Add(new wxStaticText(this, wxID_ANY, "Project name"), 0, 
                                wxALIGN_LEFT | wxLEFT | wxTOP, 10);
            property_sizer->Add(new wxTextCtrl(this, ID_PROJECT_NAME, "New Project", wxDefaultPosition, wxSize(300, 20)),
                                0, wxALIGN_LEFT | wxLEFT, 10);

            // directory name
            property_sizer->Add(new wxStaticText(this, wxID_ANY, "Project directory name"), 0,
                                wxALIGN_LEFT | wxLEFT | wxTOP, 10);
            property_sizer->Add(new wxTextCtrl(this, ID_DIRECTORY_NAME, "new-project", wxDefaultPosition, wxSize(300, 20)),
                                0, wxALIGN_LEFT | wxLEFT, 10);

            // separator
            property_sizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(450, 2)), 0, wxALL, 10);
            
            // OK / Cancel
            wxBoxSizer* btns = new wxBoxSizer(wxHORIZONTAL);
            btns->Add(new wxButton(this, ID_CREATE_NEW_PROJECT, "Create new project"), 0, wxALL, 5);
            btns->Add(new wxButton(this, ID_CANCEL, "Cancel"), 0, wxALL, 5);
            property_sizer->Add(btns, wxSizerFlags(0).Right());

            SetSizerAndFit(property_sizer);
        }


        // Callbacks
        void NewProjectWizard::_OnChooseDirectoryClick(wxCommandEvent &_ev) {
            wxDirDialog dir_dialog = wxDirDialog(this, "Choose project directory");
            if (dir_dialog.ShowModal() == wxID_OK) {
                auto path = dir_dialog.GetPath();
                wxWindow::FindWindowById(ID_PATH, this)->SetLabel(path);
            }
            _ev.Skip();
        }

        void NewProjectWizard::_OnCreateNewProjectClick(wxCommandEvent &_ev) {
            Close(true);
            GetParent()->Close(true);
        }

        void NewProjectWizard::_OnCancelClick(wxCommandEvent& _ev) {
            Close(true);
        }
    }
}