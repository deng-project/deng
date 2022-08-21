// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProjectManager.h - Project manager gui header
// author: Karl-Mihkel Ott

#ifndef PROJECT_MANAGER_H
#define PROJECT_MANAGER_H

#include <wx/wx.h>
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")

#define DEFAULT_SIZE wxSize(960, 640)

class ProjectManagerFrame : public wxFrame {
    private:
        void _OnNewProject(wxCommandEvent &_ev);
        void _OnOpenProject(wxCommandEvent &_ev);
        void _OnCancel(wxCommandEvent& _ev);

        wxDECLARE_EVENT_TABLE();

    public:
        ProjectManagerFrame();

};


class LogoPanel : public wxPanel {
    private:
        wxBitmap m_image;

    public:
        LogoPanel(wxFrame* _parent);
        void PaintEvent(wxPaintEvent& _ev);

        DECLARE_EVENT_TABLE();
};


class ProjectManagerApp : public wxApp {
    private:
        ProjectManagerFrame* m_frame = nullptr;

    public:
        virtual bool OnInit();
};

enum
{
    ID_NEW_PROJECT,
    ID_OPEN_PROJECT
};

#endif