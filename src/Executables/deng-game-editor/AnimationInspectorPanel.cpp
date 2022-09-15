// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: AnimationInspectorPanel.h - Animation inspector panel class implementation
// author: Karl-Mihkel Ott

#define ANIMATION_INSPECTOR_PANEL_CPP
#include "deng/Executables/deng-game-editor/AnimationInspectorPanel.h"

namespace DENG {
	namespace Editor {

		AnimationInspectorPanel::AnimationInspectorPanel(wxWindow* _parent, Animation *_ani) :
			wxScrolledWindow(_parent, wxID_ANY),
			m_animation(_ani)
		{
			m_sizer = new wxBoxSizer(wxVERTICAL);
			m_title = new wxStaticText(this, ID_ANIMATION_PANEL_TITLE, "(null)");
			m_sizer->Add(m_title, 0, wxALIGN_CENTER);
			
			m_samplers = new wxGenericCollapsiblePane(this, wxID_ANY, "List of all samplers", wxDefaultPosition, wxDefaultSize, wxCP_DEFAULT_STYLE | wxCP_NO_TLW_RESIZE);
			m_sizer->Add(m_samplers, 0, wxGROW | wxEXPAND);
			
			wxWindow* win = m_samplers->GetPane();
			wxBoxSizer* m_pane_sizer = new wxBoxSizer(wxVERTICAL);
			
			for (auto it = m_animation->samplers.begin(); it != m_animation->samplers.end(); it++) {
				m_pane_sizer->Add(new wxStaticText(this, wxID_ANY, it->GetName()));
			}

			win->SetSizer(m_pane_sizer);
			m_samplers->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, 
				[this](wxCollapsiblePaneEvent& _ev) { 
					m_samplers->Layout();
					m_sizer->Layout();
					FitInside();
				}
			);
		
			m_bind = new wxCheckBox(this, ID_BIND_ANIMATION, "Bind animation");
			m_sizer->Add(m_bind, 0, wxGROW | wxEXPAND);
			m_repeat = new wxCheckBox(this, ID_REPEAT_ANIMATION, "Repeat animation");
			m_sizer->Add(m_repeat, 0, wxGROW | wxEXPAND);

			m_bind->SetValue(!_ani->is_unbound);
			m_repeat->SetValue(!_ani->is_animated);

			m_animate_toggle = new wxButton(this, ID_ANIMATE_TOGGLE, "Animate");
			m_sizer->Add(m_animate_toggle, 0, wxGROW | wxEXPAND);
			SetSizer(m_sizer);
			SetScrollRate(2, 2);
		}


		void AnimationInspectorPanel::View(Animation* _ani) {
			m_title->SetLabelText(_ani->name);

			// for each sampler in animation view it in the sampler pane
			m_pane_sizer->Clear(true);
			for (auto it = _ani->samplers.begin(); it != _ani->samplers.end(); it++) {
				m_pane_sizer->Add(new wxStaticText(m_samplers->GetPane(), wxID_ANY, it->GetName()), 0, wxGROW | wxEXPAND);
			}

			m_bind->SetValue(!_ani->is_unbound);
			m_repeat->SetValue(!_ani->is_animated);

			if (_ani->is_animated) {
				m_animate_toggle->SetLabelText("Animate");
			} else {
				m_animate_toggle->SetLabelText("Stop animation");
			}
		}
	}
}