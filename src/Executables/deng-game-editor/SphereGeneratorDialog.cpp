// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SphereGeneratorDialog.cpp - Icosahedron sphere generator gui dialog class implementation
// author: Karl-Mihkel Ott

#define SPHERE_GENERATOR_DIALOG_CPP
#include "deng/Executables/deng-game-editor/SphereGeneratorDialog.h"

namespace DENG {
	namespace Editor {
		
		wxBEGIN_EVENT_TABLE(SphereGeneratorDialog, wxDialog)
			EVT_TEXT(ID_SPHERE_GENERATOR_DIALOG_RADIUS, SphereGeneratorDialog::_OnValueChange)
			EVT_TEXT(ID_SPHERE_GENERATOR_DIALOG_SUBDIVIDE, SphereGeneratorDialog::_OnValueChange)
			EVT_BUTTON(ID_SPHERE_GENERATOR_DIALOG_OK, SphereGeneratorDialog::_OnOkClick)
		wxEND_EVENT_TABLE()

		SphereGeneratorDialog::SphereGeneratorDialog(wxWindow* _parent) :
			wxDialog(_parent, wxID_ANY, "Generate a sphere", wxDefaultPosition, wxSize(400, 200)),
			m_radius_validator(&m_radius),
			m_subdiv_validator(&m_subdiv)
		{
			m_sizer = new wxBoxSizer(wxVERTICAL);
			
			m_radius_title = new wxStaticText(this, wxID_ANY, "Sphere radius");
			m_sizer->Add(m_radius_title, 0, wxALIGN_LEFT);
			m_radius_ctrl = new wxTextCtrl(this, ID_SPHERE_GENERATOR_DIALOG_RADIUS, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0L, m_radius_validator);
			m_sizer->Add(m_radius_ctrl, 0, wxALIGN_LEFT);
			
			m_subdiv_title = new wxStaticText(this, wxID_ANY, "Subdivision count");
			m_sizer->Add(m_subdiv_title, 0, wxALIGN_LEFT);
			m_subdiv_ctrl = new wxTextCtrl(this, ID_SPHERE_GENERATOR_DIALOG_SUBDIVIDE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0L, m_subdiv_validator);
			m_sizer->Add(m_subdiv_ctrl, 0, wxALIGN_LEFT);

			m_ok = new wxButton(this, ID_SPHERE_GENERATOR_DIALOG_OK, "OK");
			m_sizer->Add(m_ok, 0, wxALIGN_RIGHT, 10);
			
			SetSizerAndFit(m_sizer);

			Validate();
			TransferDataToWindow();
		}

		void SphereGeneratorDialog::_OnValueChange(wxCommandEvent &_evt) {
			Validate();
			TransferDataFromWindow();
			_evt.Skip();
		}

		void SphereGeneratorDialog::_OnOkClick(wxCommandEvent& _evt) {
			EndModal(wxID_OK);
		}
	}
}