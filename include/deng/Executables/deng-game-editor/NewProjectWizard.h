// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: NewProjectWizard.h - New project wizard gui header
// author: Karl-Mihkel Ott

#ifndef NEW_PROJECT_WIZARD_H
#define NEW_PROJECT_WIZARD_H

#ifdef NEW_PROJECT_WIZARD_CPP
    #include <wx/wx.h>
#endif

namespace DENG {
	namespace Editor {

        class NewProjectWizard : public wxFrame {
            private:
                void _OnContinue();

            public:
                NewProjectWizard(wxFrame* _parent);
        };

	}
}

#endif