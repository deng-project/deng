// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: NewProjectWizard.h - New project wizard gui header
// author: Karl-Mihkel Ott

#ifndef NEW_PROJECT_WIZARD_H
#define NEW_PROJECT_WIZARD_H

#ifdef NEW_PROJECT_WIZARD_CPP
    #include <wx/wx.h>
    #include <wx/statline.h>
#ifdef _WIN32
    #include <ShlObj.h>
#endif
    
    #include "trs/Vector.h"
    #include "trs/Points.h"
    #include "trs/Quaternion.h"

    #include "deng/Api.h"
    #include "dxml/GameStructs.h"
    #include "deng/ProjectDataManager.h"
    #include "deng/Executables/deng-game-editor/StaticResources.h"
#endif

namespace DENG {
	namespace Editor {
        
        class NewProjectWizard : public wxFrame {
            private:
                bool _CreateNewProject();
                const std::string _GetDefaultProjectPath();

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
            ID_BACKEND = 10,
            ID_CREATE_NEW_PROJECT = 11,
            ID_CANCEL = 12
        };

	}
}

#endif