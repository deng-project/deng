// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: InspectorPanel.h - Inspector panel class header
// author: Karl-Mihkel Ott


#ifndef INSPECTOR_PANEL_H
#define INSPECTOR_PANEL_H

#ifdef INSPECTOR_PANEL_CPP
	#include <wx/wx.h>
	#include <wx/collpane.h>
	#include "deng/deng.h"
#endif

#include "deng/Executables/deng-game-editor/AnimationInspectorPanel.h"

namespace DENG {
	namespace Editor {

		class InspectorPanel : public wxWindow {
			private:
				wxBoxSizer* m_sizer = nullptr;
				AnimationInspectorPanel* m_ani_inspector = nullptr;

			public:
				InspectorPanel(wxWindow* _parent);
				void ViewAnimationPanel(Animation *_ani);
		};
	}
}

#endif