// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: AssetPanel.cpp - Asset panel class implementation
// author: Karl-Mihkel Ott

#define ASSET_PANEL_CPP
#include "deng/Executables/deng-game-editor/AssetPanel.h"

namespace DENG {
	namespace Editor {

		wxBitmap AssetWidget::m_audio_icon;
		wxBitmap AssetWidget::m_config_icon;
		wxBitmap AssetWidget::m_image_icon;
		wxBitmap AssetWidget::m_model_icon;
		wxBitmap AssetWidget::m_playback_icon;
		wxBitmap AssetWidget::m_pyscript_icon;
		wxBitmap AssetWidget::m_unknown_icon;
		wxBitmap AssetWidget::m_xml_document_icon;

		AssetWidget::AssetWidget(wxWindow* _parent, const std::string& _file_name) :
			wxWindow(_parent, wxID_ANY)
		{

		}

		void AssetWidget::InitIcons() {
			// audio icon
			m_audio_icon = wxBitmap::NewFromPNGData(GetAssetAudioIconPng(), GetAssetAudioIconPngSize());
			m_config_icon = wxBitmap::NewFromPNGData(GetAssetConfigIconPng(), GetAssetConfigIconPngSize());
			m_image_icon = wxBitmap::NewFromPNGData(GetAssetImageIconPng(), GetAssetImageIconPngSize());
			m_model_icon = wxBitmap::NewFromPNGData(GetAssetModelIconPng(), GetAssetModelIconPngSize());
			m_playback_icon = wxBitmap::NewFromPNGData(GetAssetPlaybackIconPng(), GetAssetPlaybackIconPngSize());
			m_pyscript_icon = wxBitmap::NewFromPNGData(GetAssetPyScriptIconPng(), GetAssetPyScriptIconPngSize());
			m_unknown_icon = wxBitmap::NewFromPNGData(GetAssetUnknownIconPng(), GetAssetUnknownIconPngSize());
			m_xml_document_icon = wxBitmap::NewFromPNGData(GetAssetXMLDocumentIconPng(), GetAssetXMLDocumentIconPngSize());
		}


		wxBEGIN_EVENT_TABLE(AssetPanel, wxWindow)
			EVT_SIZE(AssetPanel::_OnResize)
			EVT_DIRCTRL_SELECTIONCHANGED(ID_ASSET_PANEL_DIR_CTRL, AssetPanel::_OnDirCtrlSelectionChanged)
		wxEND_EVENT_TABLE()


		AssetPanel::AssetPanel(wxWindow* _parent, const std::string& _path) :
			wxWindow(_parent, ID_ASSET_PANEL, wxDefaultPosition, wxSize(600, 400)),
			m_splitter(this, wxAUI_MGR_DEFAULT)
		{
			m_dir_ctrl = new wxGenericDirCtrl(this, ID_ASSET_PANEL_DIR_CTRL, _path, wxDefaultPosition, wxSize(300, 100), wxDIRCTRL_DIR_ONLY | wxDIRCTRL_EDIT_LABELS);
			wxPanel* panel = new AssetWindow(this, wxID_ANY, wxDefaultPosition, wxSize(300, 200));
			panel->SetBackgroundColour(wxColour(255, 0, 0, 255));

			m_splitter.AddPane(m_dir_ctrl, wxLEFT, wxT("Directory tree"));
			m_splitter.AddPane(panel, wxCENTER, wxT("Assets"));
			m_splitter.Update();
		}


		void AssetPanel::_OnResize(wxSizeEvent& _evt) {
			std::cout << "Asset panel was resized" << std::endl;
			Layout();
		}


		void AssetPanel::_OnDirCtrlSelectionChanged(wxTreeEvent& _evt) {
			if (!m_dir_ctrl) 
				return;

			std::string path = m_dir_ctrl->GetPath();
			for (auto const& entry : std::filesystem::directory_iterator{ path }) {
				if (entry.is_regular_file()) {
					std::string ext = entry.path().extension().u8string();
					std::transform(ext.begin(), ext.end(), ext.begin(),
						[](unsigned char c) { return std::tolower(c); });

					if (ext == ".mp3" || ext == ".wav" || ext == ".flac" || ext == ".ogg" || ext == ".wma") {
						std::cout << "[AudioAsset] " << entry.path().u8string() << std::endl;
					}
					else if (ext == ".cnf" || ext == ".conf" || ext == ".dat") {
						std::cout << "[ConfigAsset] " << entry.path().u8string() << std::endl;
					}
					else if (ext == ".png" || ext == ".jpeg" || ext == ".jpg" || ext == ".gif" || ext == ".tiff" || ext == ".ppm" || ext == ".bmp") {
						std::cout << "[ImageAsset] " << entry.path().u8string() << std::endl;
					}
					else if (ext == ".das" || ext == ".gltf" || ext == ".obj" || ext == ".stl") {
						std::cout << "[ModelAsset] " << entry.path().u8string() << std::endl;
					}
					else if (ext == ".webm" || ext == ".mkv" || ext == ".avi" || ext == ".mp4" || ext == ".flv") {
						std::cout << "[VideoAsset] " << entry.path().u8string() << std::endl;
					}
					else if (ext == ".py" || ext == ".pyd" || ext == ".pyc") {
						std::cout << "[ScriptAsset] " << entry.path().u8string() << std::endl;
					}
					else if (ext == ".xml") {
						std::cout << "[XMLAsset] " << entry.path().u8string() << std::endl;
					} 
					else {
						std::cout << "[UnknownAsset] " << entry.path().u8string() << std::endl;
					}
				}
			}
		}
	}
}