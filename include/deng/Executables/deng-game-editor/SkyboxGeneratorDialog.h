// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SkyboxGeneratorDialog.h - Skybox generator dialog header
// author: Karl-Mihkel Ott

#ifndef SKYBOX_GENERATOR_DIALOG_H
#define SKYBOX_GENERATOR_DIALOG_H

#ifdef SKYBOX_GENERATOR_DIALOG_CPP
	#include <array>	

	#include <wx/wx.h>
	#include <wx/valnum.h>

	#include "das/stb_image.h"

	#include "deng/deng.h"
	#include "deng/Executables/deng-game-editor/GuiIds.h"

#define BMP_BTN_SIZE_X 128
#define BMP_BTN_SIZE_Y 128

#define BMP_MARGIN_X	8
#define BMP_MARGIN_Y	8
#endif

namespace DENG {
	namespace Editor {
		
		class SkyboxGeneratorDialog : public wxDialog {
		private:
			wxBoxSizer* m_vertical = nullptr;
			wxBoxSizer* m_horizontal = nullptr;
			// default texture ordering is following:
			//	0 - top
			//	1 - left
			//	2 - front
			//	3 - right
			//	4 - bottom
			//	5 - back
			std::array<wxBitmapButton*, 6> m_bmp_buttons;
			std::array<std::string, 6> m_files;

			wxStaticText* m_size_title = nullptr;
			wxTextCtrl* m_size_ctrl = nullptr;
			float m_size = 1.0f;
			wxFloatingPointValidator<float> m_size_validator;
			wxButton* m_ok = nullptr;

		private:
			void _OnTextureClick(wxCommandEvent& _evt);

			void _OnSizeCtrlValueChange(wxCommandEvent& _evt);
			void _OnOkClick(wxCommandEvent& _evt);

			wxDECLARE_EVENT_TABLE();

		public:
			SkyboxGeneratorDialog(wxWindow* _parent);
			const std::array<std::string, 6>& GetTextures() {
				return m_files;
			}

			inline float GetScale() {
				return m_size;
			}
		};
	}
}

#endif