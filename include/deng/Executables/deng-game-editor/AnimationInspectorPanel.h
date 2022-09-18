// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: AnimationInspectorPanel.h - Animation inspector panel class header
// author: Karl-Mihkel Ott

#ifndef ANIMATION_INSPECTOR_PANEL_H
#define ANIMATION_INSPECTOR_PANEL_H

#ifdef ANIMATION_INSPECTOR_PANEL_CPP
	#include <wx/wx.h>
	#include <wx/collpane.h>
	#include <wx/collheaderctrl.h>

	#include "deng/deng.h"
	
	#include "deng/Executables/deng-game-editor/GuiIds.h"
#endif

namespace DENG {
	namespace Editor {

		class AnimationInspectorPanel : public wxScrolledWindow {
			private:
				Animation* m_animation = nullptr;

				// wxwidgets
				wxBoxSizer* m_sizer = nullptr;
				wxStaticText* m_title = nullptr;
				wxGenericCollapsiblePane* m_samplers = nullptr;
				wxBoxSizer* m_pane_sizer = nullptr;
				wxCheckBox* m_bind = nullptr;
				wxCheckBox* m_repeat = nullptr;
				wxButton* m_animate_toggle = nullptr;

			private:
				void _OnBindCheck(wxCommandEvent &_evt);
				void _OnRepeatCheck(wxCommandEvent& _evt);
				void _OnAnimateToggleClick(wxCommandEvent& _evt);
				void _OnResize(wxSizeEvent& _evt);
				wxDECLARE_EVENT_TABLE();

			public:
				AnimationInspectorPanel(wxWindow *_parent);
				void View(Animation* _ani);
		};

	}
}

#endif
