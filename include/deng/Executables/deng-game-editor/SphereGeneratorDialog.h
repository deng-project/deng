// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SphereGeneratorDialog.h - Icosahedron sphere generator gui dialog class header
// author: Karl-Mihkel Ott

#ifndef SPHERE_GENERATOR_DIALOG_H
#define SPHERE_GENERATOR_DIALOG_H

#ifdef SPHERE_GENERATOR_DIALOG_CPP
	#include <wx/wx.h>
	#include <wx/valnum.h>

	#include "deng/deng.h"
	#include "deng/Executables/deng-game-editor/GuiIds.h"
#endif

namespace DENG {
	namespace Editor {

		class SphereGeneratorDialog : public wxDialog {
			private:
				wxBoxSizer* m_sizer = nullptr;
				wxStaticText* m_radius_title = nullptr;
				wxTextCtrl* m_radius_ctrl = nullptr;
				wxFloatingPointValidator<float> m_radius_validator;

				wxStaticText* m_subdiv_title = nullptr;
				wxTextCtrl* m_subdiv_ctrl = nullptr;
				wxIntegerValidator<uint32_t> m_subdiv_validator;

				wxCheckBox *m_use_normals_box = nullptr;
				wxButton* m_ok = nullptr;

				float m_radius = 0.5f;
				uint32_t m_subdiv = 1;
				bool m_use_normals = false;

			private:
				void _OnValueChange(wxCommandEvent& _evt);
				void _OnOkClick(wxCommandEvent& _evt);
				void _OnUseNormalsChange(wxCommandEvent& _evt);

				wxDECLARE_EVENT_TABLE();

			public:
				SphereGeneratorDialog(wxWindow *_parent);
				inline SphereGenerator ConstructSphere() {
					return SphereGenerator(m_radius, m_subdiv, m_use_normals);
				}
		};
	}
}

#endif