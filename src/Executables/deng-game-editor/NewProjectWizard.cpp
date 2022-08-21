// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: NewProjectWizard.h - New project wizard gui implementation
// author: Karl-Mihkel Ott

#define NEW_PROJECT_WIZARD_CPP
#include "deng/Executables/deng-game-editor/NewProjectWizard.h"

namespace DENG {
    namespace Editor {
        NewProjectWizard::NewProjectWizard(wxFrame* _parent) :
            wxFrame(_parent, wxID_ANY, "DENG | Create a new project")
        {
        }
    }
}