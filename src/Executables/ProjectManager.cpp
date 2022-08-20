// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProjectManager.cpp - Project manager gui implementation
// author: Karl-Mihkel Ott

#include "deng/Executables/ProjectManager.h"

wxIMPLEMENT_APP(ProjectManagerApp);

bool ProjectManagerApp::OnInit()
{
    ProjectManagerFrame* frame = new ProjectManagerFrame();
    frame->Show(true);
    return true;
}

ProjectManagerFrame::ProjectManagerFrame()
    : wxFrame(NULL, wxID_ANY, "DENG project manager", wxDefaultPosition, DEFAULT_SIZE)
{
    SetSizeHints(DEFAULT_SIZE, DEFAULT_SIZE, DEFAULT_SIZE);
}

void ProjectManagerFrame::_OnExit(wxCommandEvent& event)
{
    Close(true);
}

void ProjectManagerFrame::_OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
        "About Hello World", wxOK | wxICON_INFORMATION);
}

void ProjectManagerFrame::_OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}