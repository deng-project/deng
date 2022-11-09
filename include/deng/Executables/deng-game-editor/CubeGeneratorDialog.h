// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: CubeGeneratorDialog.h - Cube generation gui dialog class header
// author: Karl-Mihkel Ott

#ifndef CUBE_GENERATOR_DIALOG_H
#define CUBE_GENERATOR_DIALOG_H

#ifdef CUBE_GENERATOR_DIALOG_CPP
	#include <wx/wx.h>
	#include <wx/valnum.h>

	#include "deng/deng.h"
	#include "deng/Executables/deng-game-editor/GuiIds.h"
#endif

namespace DENG {
	namespace Editor {

		class CubeGeneratorDialog : public wxDialog {
			private:
				wxBoxSizer* m_sizer = nullptr;
				wxStaticText* m_min_title = nullptr;
				wxFloatingPointValidator<float> m_min_validator;
				wxTextCtrl* m_min_box = nullptr;
				wxStaticText* m_max_title = nullptr;
				wxFloatingPointValidator<float> m_max_validator;
				wxTextCtrl* m_max_box = nullptr;
				wxCheckBox* m_use_normals = nullptr;
				wxButton* m_ok = nullptr;

				float m_min = -.5f;
				float m_max = .5f;
				bool m_use_normals_bit = false;

			private:
				void _OnValueChange(wxCommandEvent& _evt);
				void _OnUseNormalsCheck(wxCommandEvent& _evt);
				void _OnOkClick(wxCommandEvent& _evt);
				wxDECLARE_EVENT_TABLE();

			public:
				CubeGeneratorDialog(wxWindow* _parent);
				CubeGenerator ConstructCube();
		};
	}
}

#endif