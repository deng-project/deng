// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: MeshGeneratorDialog.h - Mesh generator gui dialog class implementation
// author: Karl-Mihkel Ott

#define MESH_GENERATOR_DIALOG_CPP
#include "deng/Executables/deng-game-editor/MeshGeneratorDialog.h"

namespace DENG {
	namespace Editor {

		wxBEGIN_EVENT_TABLE(CubeGeneratorDialog, wxDialog)
			EVT_TEXT(ID_CUBE_GENERATOR_DIALOG_MIN, CubeGeneratorDialog::_OnValueChange)
			EVT_TEXT(ID_CUBE_GENERATOR_DIALOG_MAX, CubeGeneratorDialog::_OnValueChange)
			EVT_CHECKBOX(ID_CUBE_GENERATOR_DIALOG_USE_NORMALS, CubeGeneratorDialog::_OnUseNormalsCheck)
			EVT_BUTTON(ID_CUBE_GENERATOR_DIALOG_OK, CubeGeneratorDialog::_OnOkClick)
		wxEND_EVENT_TABLE()

		CubeGeneratorDialog::CubeGeneratorDialog(wxWindow* _parent) :
			wxDialog(_parent, wxID_ANY, "Generate a new cube", wxDefaultPosition, wxSize(300, 200)),
			m_min_validator(4, &m_min),
			m_max_validator(4, &m_max)
		{
			m_sizer = new wxBoxSizer(wxVERTICAL);
			m_min_title = new wxStaticText(this, wxID_ANY, "Choose a minimum coordinate");
			m_sizer->Add(m_min_title, 0, wxALIGN_LEFT);
			m_min_box = new wxTextCtrl(this, ID_CUBE_GENERATOR_DIALOG_MIN, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0L, m_min_validator);
			m_sizer->Add(m_min_box, 0, wxALIGN_LEFT);
			m_max_title = new wxStaticText(this, wxID_ANY, "Choose a maximum coordinate");
			m_sizer->Add(m_max_title, 0, wxALIGN_LEFT);
			m_max_box = new wxTextCtrl(this, ID_CUBE_GENERATOR_DIALOG_MAX, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0L, m_max_validator);
			m_sizer->Add(m_max_box, 0, wxALIGN_LEFT);

			m_use_normals = new wxCheckBox(this, ID_CUBE_GENERATOR_DIALOG_USE_NORMALS, "Use normals");
			m_sizer->Add(m_use_normals, 0, wxALIGN_LEFT, 5);

			m_ok = new wxButton(this, ID_CUBE_GENERATOR_DIALOG_OK, "OK");
			m_sizer->Add(m_ok, 0, wxALIGN_RIGHT);

			SetSizerAndFit(m_sizer);

			Validate();
			TransferDataToWindow();
		}



		void CubeGeneratorDialog::_OnValueChange(wxCommandEvent& _evt) {
			Validate();
			TransferDataFromWindow();
			_evt.Skip();
		}

		void CubeGeneratorDialog::_OnUseNormalsCheck(wxCommandEvent& _evt) {
			m_use_normals_bit = _evt.IsChecked();
		}

		void CubeGeneratorDialog::_OnOkClick(wxCommandEvent& _evt) {
			EndModal(wxID_OK);
		}


		CubeGenerator CubeGeneratorDialog::ConstructCube() {
			TRS::Point3D<float> min = { m_min, m_min, m_min };
			TRS::Point3D<float> max = { m_max, m_max, m_max };
			return CubeGenerator(min, max, m_use_normals_bit);
		}
	}
}