// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: NewProjectWizard.h - New project wizard gui header
// author: Karl-Mihkel Ott

#ifndef NEW_PROJECT_WIZARD_H
#define NEW_PROJECT_WIZARD_H

#ifdef NEW_PROJECT_WIZARD_CPP
    #include <wx/wx.h>
    #include <wx/statline.h>
    #include "deng/Executables/deng-game-editor/StaticResources.h"
#endif

namespace DENG {
	namespace Editor {

        class NewProjectWizard : public wxFrame {
            private:
                void _CreateNewProject();
                void _OnChooseDirectoryClick(wxCommandEvent &_ev);
                void _OnCreateNewProjectClick(wxCommandEvent& _ev);
                void _OnCancelClick(wxCommandEvent& _ev);

                wxDECLARE_EVENT_TABLE();

            public:
                NewProjectWizard(wxFrame* _parent);
        };

        enum {
            ID_PATH = 6,
            ID_CHOOSE_DIRECTORY = 7,
            ID_PROJECT_NAME = 8,
            ID_DIRECTORY_NAME = 9,
            ID_CREATE_NEW_PROJECT = 10,
            ID_CANCEL = 11
        };

	}
}

#endif