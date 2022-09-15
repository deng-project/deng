// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: InspectorPanel.h - Inspector panel class implementation
// author: Karl-Mihkel Ott

#define INSPECTOR_PANEL_CPP
#include "deng/Executables/deng-game-editor/InspectorPanel.h"

namespace DENG {
	namespace Editor {

		InspectorPanel::InspectorPanel(wxWindow* _parent) :
			wxWindow(_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize) 
		{
			m_sizer = new wxBoxSizer(wxVERTICAL);
			m_sizer->Add((m_ani_inspector = new AnimationInspectorPanel(this)), 0, wxALIGN_CENTER);
			SetSizerAndFit(m_sizer);
		}

	}
}