// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProjectManager.h - Project manager gui header
// author: Karl-Mihkel Ott

#ifndef PROJECT_MANAGER_H
#define PROJECT_MANAGER_H

// wxWidgets "Hello World" Program

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wx.h>
#define DEFAULT_SIZE wxSize(600, 800)

class ProjectManagerFrame : public wxFrame {
    private:
        void _OnHello(wxCommandEvent& event);
        void _OnExit(wxCommandEvent& event);
        void _OnAbout(wxCommandEvent& event);

    public:
        ProjectManagerFrame();

};


class ProjectManagerApp : public wxApp {
    public:
        virtual bool OnInit();
};

enum
{
    ID_Hello = 1
};

#endif