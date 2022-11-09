// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SkyboxGeneratorDialog.cpp - Skybox generator dialog implementation
// author: Karl-Mihkel Ott

#define SKYBOX_GENERATOR_DIALOG_CPP
#include "deng/Executables/deng-game-editor/SkyboxGeneratorDialog.h"

namespace DENG {
	namespace Editor {

		wxBEGIN_EVENT_TABLE(SkyboxGeneratorDialog, wxDialog)
			EVT_BUTTON(ID_SKYBOX_GENERATOR_TOP, SkyboxGeneratorDialog::_OnTextureClick)
			EVT_BUTTON(ID_SKYBOX_GENERATOR_LEFT, SkyboxGeneratorDialog::_OnTextureClick)
			EVT_BUTTON(ID_SKYBOX_GENERATOR_FRONT, SkyboxGeneratorDialog::_OnTextureClick)
			EVT_BUTTON(ID_SKYBOX_GENERATOR_RIGHT, SkyboxGeneratorDialog::_OnTextureClick)
			EVT_BUTTON(ID_SKYBOX_GENERATOR_BOTTOM, SkyboxGeneratorDialog::_OnTextureClick)
			EVT_BUTTON(ID_SKYBOX_GENERATOR_BACK, SkyboxGeneratorDialog::_OnTextureClick)

			EVT_TEXT(ID_SKYBOX_GENERATOR_SIZE, SkyboxGeneratorDialog::_OnSizeCtrlValueChange)
			EVT_BUTTON(ID_SKYBOX_GENERATOR_OK, SkyboxGeneratorDialog::_OnOkClick)
		wxEND_EVENT_TABLE()

		SkyboxGeneratorDialog::SkyboxGeneratorDialog(wxWindow* _parent) :
			wxDialog(_parent, wxID_ANY, "Create or edit skybox", wxDefaultPosition, wxSize(512, 684)),
			m_size_validator(&m_size)
		{
			int x, y, size;
			unsigned char* data = (unsigned char*) GetMissingTextureRGB(x, y, size);
			wxImage img = wxImage(wxSize(x, y), data, NULL, true);

			// bitmap button size
			img.Rescale(BMP_BTN_SIZE_X - BMP_MARGIN_X, BMP_BTN_SIZE_Y - BMP_MARGIN_Y);
			wxBitmap bmp = wxBitmap(img);

			m_vertical = new wxBoxSizer(wxVERTICAL);
			m_horizontal = new wxBoxSizer(wxHORIZONTAL);

			const wxSize bmpbtn_size = { BMP_BTN_SIZE_X, BMP_BTN_SIZE_Y };
			m_bmp_buttons[0] = new wxBitmapButton(this, ID_SKYBOX_GENERATOR_TOP, bmp, wxDefaultPosition, bmpbtn_size, wxBU_LEFT);
			m_vertical->Add(m_bmp_buttons[0], 0, wxALIGN_CENTER, 0);
			m_bmp_buttons[1] = new wxBitmapButton(this, ID_SKYBOX_GENERATOR_LEFT, bmp, wxDefaultPosition, bmpbtn_size);
			m_horizontal->Add(m_bmp_buttons[1]);
			m_bmp_buttons[2] = new wxBitmapButton(this, ID_SKYBOX_GENERATOR_FRONT, bmp, wxDefaultPosition, bmpbtn_size);
			m_horizontal->Add(m_bmp_buttons[2]);
			m_bmp_buttons[3] = new wxBitmapButton(this, ID_SKYBOX_GENERATOR_RIGHT, bmp, wxDefaultPosition, bmpbtn_size);
			m_horizontal->Add(m_bmp_buttons[3]);
			m_vertical->Add(m_horizontal, 0, wxALIGN_CENTER, 0);
			m_bmp_buttons[4] = new wxBitmapButton(this, ID_SKYBOX_GENERATOR_BOTTOM, bmp, wxDefaultPosition, bmpbtn_size);
			m_vertical->Add(m_bmp_buttons[4], 0, wxALIGN_CENTER, 0);
			m_bmp_buttons[5] = new wxBitmapButton(this, ID_SKYBOX_GENERATOR_BACK, bmp, wxDefaultPosition, bmpbtn_size);
			m_vertical->Add(m_bmp_buttons[5], 0, wxALIGN_CENTER, 0);

			m_size_title = new wxStaticText(this, wxID_ANY, wxT("Skybox size"));
			m_vertical->Add(m_size_title, 0, wxALIGN_LEFT, 4);
			m_size_ctrl = new wxTextCtrl(this, ID_SKYBOX_GENERATOR_SIZE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0L, m_size_validator);
			m_vertical->Add(m_size_ctrl, 0, wxALIGN_LEFT, 0);

			m_ok = new wxButton(this, ID_SKYBOX_GENERATOR_OK, "OK");
			m_vertical->Add(m_ok, 0, wxALIGN_RIGHT, 10);

			SetSizerAndFit(m_vertical);
			TransferDataToWindow();
		}


		void SkyboxGeneratorDialog::_OnTextureClick(wxCommandEvent& _evt) {
			std::string& ref = m_files[_evt.GetId() - ID_SKYBOX_GENERATOR_TOP];
			wxFileDialog dialog = wxFileDialog(this, "Choose a texture", wxEmptyString, wxEmptyString, 
											   wxT("All images|*.jpg;*.jpeg;*.png;*.ppm;*.bmp|JPEG image (*.jpg;*.jpeg)|*.jpg;*.jpeg|PNG image (*.png)|*.png|PPM image (*.ppm)|*.ppm|BMP bitmap image (*.bmp)|*.bmp"), 
											   wxFD_OPEN | wxFD_FILE_MUST_EXIST);
			if (dialog.ShowModal() == wxID_OK) {
				ref = dialog.GetPath();

				// attempt loading the texture
				int x, y, w;
				unsigned char *data = stbi_load(ref.c_str(), &x, &y, &w, 3);
				if (!data)
					throw std::runtime_error("Could not load texture file '" + ref + "'");

				wxImage img(wxSize(x, y), data, NULL, false);
				img.Rescale(BMP_BTN_SIZE_X - BMP_MARGIN_X, BMP_BTN_SIZE_Y - BMP_MARGIN_Y);
				wxBitmap bmp(img);
				m_bmp_buttons[_evt.GetId() - ID_SKYBOX_GENERATOR_TOP]->SetBitmap(bmp);
			}

			_evt.Skip();
		}


		void SkyboxGeneratorDialog::_OnSizeCtrlValueChange(wxCommandEvent& _evt) {
			Validate();
			TransferDataFromWindow();
			_evt.Skip();
		}


		void SkyboxGeneratorDialog::_OnOkClick(wxCommandEvent& _evt) {
			// check if all models were chosen
			for (auto it = m_files.begin(); it != m_files.end(); it++) {
				if (it->empty())
					EndModal(wxID_ANY);
			}

			EndModal(wxID_OK);
		}

	}
}