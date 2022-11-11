// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: AnimationInspectorPanel.h - Animation inspector panel class implementation
// author: Karl-Mihkel Ott

#define ANIMATION_INSPECTOR_PANEL_CPP
#include "deng/Executables/deng-game-editor/AnimationInspectorPanel.h"

namespace DENG {
	namespace Editor {

		wxBEGIN_EVENT_TABLE(AnimationInspectorPanel, wxScrolledWindow)
			EVT_CHECKBOX(ID_EDITOR_ANIMATION_PANEL_BIND, AnimationInspectorPanel::_OnBindCheck)
			EVT_CHECKBOX(ID_EDITOR_ANIMATION_PANEL_REPEAT, AnimationInspectorPanel::_OnRepeatCheck)
			EVT_BUTTON(ID_EDITOR_ANIMATION_PANEL_TOGGLE, AnimationInspectorPanel::_OnAnimateToggleClick)
		wxEND_EVENT_TABLE()

		AnimationInspectorPanel::AnimationInspectorPanel(wxWindow* _parent) :
			wxScrolledWindow(_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
		{
			Bind(wxEVT_SIZE, &AnimationInspectorPanel::_OnResize, this, wxID_ANY);
			m_sizer = new wxBoxSizer(wxVERTICAL);
			m_title = new wxStaticText(this, ID_EDITOR_ANIMATION_PANEL_TITLE, "(null)");
			m_sizer->Add(m_title, 0, wxALIGN_LEFT);
			m_sizer->AddSpacer(10);
			
			m_samplers = new wxGenericCollapsiblePane(this, wxID_ANY, "List of all samplers", wxDefaultPosition, wxDefaultSize, wxCP_DEFAULT_STYLE | wxCP_NO_TLW_RESIZE);
			m_sizer->Add(m_samplers, 0, wxGROW | wxEXPAND);
			
			wxWindow* win = m_samplers->GetPane();
			m_pane_sizer = new wxBoxSizer(wxVERTICAL);
			
			win->SetSizer(m_pane_sizer);
			m_samplers->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, 
				[this](wxCollapsiblePaneEvent& _ev) { 
					wxWindow* win = m_samplers->GetPane();
					m_samplers->Layout();
					FitInside();
				}
			);
		
			m_bind = new wxCheckBox(this, ID_EDITOR_ANIMATION_PANEL_BIND, "Bind animation");
			m_sizer->Add(m_bind, 0, wxGROW | wxEXPAND);
			m_repeat = new wxCheckBox(this, ID_EDITOR_ANIMATION_PANEL_REPEAT, "Repeat animation");
			m_sizer->Add(m_repeat, 0, wxGROW | wxEXPAND);

			m_bind->SetValue(false);
			m_repeat->SetValue(false);

			m_animate_toggle = new wxButton(this, ID_EDITOR_ANIMATION_PANEL_TOGGLE, "Animate");
			m_sizer->Add(m_animate_toggle, 0, wxGROW | wxEXPAND);
			SetSizerAndFit(m_sizer);
			SetScrollRate(3, 3);
		}


		void AnimationInspectorPanel::View(Animation* _ani) {
			m_animation = _ani;
			m_title->SetLabelText(_ani->name);
			SetSizerAndFit(m_sizer);
			Show();

			// for each sampler in animation view it in the sampler pane
			m_pane_sizer->Clear(true);
			for (auto it = _ani->samplers.begin(); it != _ani->samplers.end(); it++) {
				m_pane_sizer->Add(new wxStaticText(m_samplers->GetPane(), wxID_ANY, it->GetName()), 0, wxGROW | wxEXPAND);
			}

			m_bind->SetValue(!_ani->is_unbound);
			m_repeat->SetValue(_ani->is_repeated);

			if (!_ani->is_animated) {
				m_animate_toggle->SetLabelText("Animate");
			} else {
				m_animate_toggle->SetLabelText("Stop animation");
			}
		}

		void AnimationInspectorPanel::_OnBindCheck(wxCommandEvent& _evt) {
			m_animation->is_unbound = !_evt.IsChecked();
		}

		void AnimationInspectorPanel::_OnRepeatCheck(wxCommandEvent& _evt) {
			m_animation->is_repeated = _evt.IsChecked();
			for (auto it = m_animation->samplers.begin(); it != m_animation->samplers.end(); it++) {
				it->SetRepeat(m_animation->is_repeated);
			}
		}

		void AnimationInspectorPanel::_OnResize(wxSizeEvent& _evt) {
			m_sizer->Layout();
			SetSize(GetParent()->GetVirtualSize());
			FitInside();
		}

		void AnimationInspectorPanel::_OnAnimateToggleClick(wxCommandEvent& _evt) {
			m_animation->is_animated = !m_animation->is_animated;
			if (!m_animation->is_animated) {
				m_animate_toggle->SetLabelText("Animate");
				for (auto it = m_animation->samplers.begin(); it != m_animation->samplers.end(); it++) {
					it->Stop();
				}
			} else {
				m_animate_toggle->SetLabelText("Stop animation");
				for (auto it = m_animation->samplers.begin(); it != m_animation->samplers.end(); it++) {
					it->Animate(m_animation->is_repeated);
				}
			}
		}
	}
}