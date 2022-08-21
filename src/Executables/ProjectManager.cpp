// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProjectManager.cpp - Project manager gui implementation
// author: Karl-Mihkel Ott

#include "deng/Executables/ProjectManager.h"

wxBEGIN_EVENT_TABLE(ProjectManagerFrame, wxFrame)
    EVT_BUTTON(ID_NEW_PROJECT, ProjectManagerFrame::_OnNewProject)
    EVT_BUTTON(ID_OPEN_PROJECT, ProjectManagerFrame::_OnOpenProject)
    EVT_BUTTON(wxID_EXIT, ProjectManagerFrame::_OnCancel)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(ProjectManagerApp);

bool ProjectManagerApp::OnInit()
{
    wxInitAllImageHandlers();
    ProjectManagerFrame* frame = new ProjectManagerFrame();
    frame->Show(true);
    return true;
}


BEGIN_EVENT_TABLE(LogoPanel, wxPanel)
    EVT_PAINT(LogoPanel::PaintEvent)
END_EVENT_TABLE()

LogoPanel::LogoPanel(wxFrame* _parent) : 
    wxPanel(_parent, wxID_ANY, wxDefaultPosition, wxSize(280, 280)) 
{
    SetBackgroundColour(wxColor(*wxWHITE));
    m_image.LoadFile(wxT("../logo/logo_small.png"), wxBITMAP_TYPE_PNG);
    m_image.NewFromPNGData()
}


void LogoPanel::PaintEvent(wxPaintEvent &_ev) {
    wxClientDC dc(this);
    dc.DrawBitmap(m_image, 0, 0, true);
}


ProjectManagerFrame::ProjectManagerFrame()
    : wxFrame(NULL, wxID_ANY, "DENG project manager", wxDefaultPosition, DEFAULT_SIZE)
{
    SetBackgroundColour(wxColor(*wxWHITE));
    wxBoxSizer *button_sizer = new wxBoxSizer(wxHORIZONTAL);
    button_sizer->Add(
        new wxButton(this, ID_NEW_PROJECT, "New project", wxDefaultPosition, wxSize(120, 60)),
        0,
        wxALL,
        10
    );
    
    button_sizer->Add(
        new wxButton(this, ID_OPEN_PROJECT, "Open project", wxDefaultPosition, wxSize(120, 60)),
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
        new wxStaticText(this, -1, "Welcome to DENG! Please either select an existing project or create a new one to continue."),
        0,
        wxALIGN_CENTER | wxALL,
        10
    );
    main_sizer->Add(button_sizer, wxSizerFlags(0).Center());
    //main_sizer->Add(
    //    new wxButton(this, wxID_EXIT, "Cancel"),
    //    0,
    //    wxALIGN_RIGHT
    //);
    SetSizerAndFit(main_sizer);
}

// Event handler callbacks
void ProjectManagerFrame::_OnNewProject(wxCommandEvent &_ev) {
    wxMessageBox("This button creates a new DENG project", "About 'New Project'", wxOK | wxICON_INFORMATION);
}

void ProjectManagerFrame::_OnOpenProject(wxCommandEvent &_ev) {
    wxMessageBox("This button opens an existing DENG project", "About 'Open Project'", wxOK | wxICON_INFORMATION);
}

void ProjectManagerFrame::_OnCancel(wxCommandEvent &_ev)
{
    Close(true);
}