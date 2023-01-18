// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: AssetPanel.h - Asset panel class header
// author: Karl-Mihkel Ott

#ifndef ASSET_PANEL_H
#define ASSET_PANEL_H

#ifdef ASSET_PANEL_CPP
	#include <filesystem>
	#include <algorithm>

	#include <wx/wx.h>
	#include <wx/aui/aui.h>
	#include <wx/splitter.h>
	#include <wx/dirctrl.h>

	#include "deng/Executables/deng-game-editor/StaticResources.h"
	#include "deng/Executables/deng-game-editor/GuiIds.h"

	#define ASSET_WIDGET_WIDTH 128
	#define ASSET_WIDGET_HEIGHT 128
#endif

namespace DENG {
	namespace Editor {

		class AssetWidget : public wxWindow {
			private:
				// all static icons
				static wxBitmap m_audio_icon;
				static wxBitmap m_config_icon;
				static wxBitmap m_image_icon;
				static wxBitmap m_model_icon;
				static wxBitmap m_playback_icon;
				static wxBitmap m_pyscript_icon;
				static wxBitmap m_unknown_icon;
				static wxBitmap m_xml_document_icon;

				wxBitmap* m_current = nullptr;
				wxStaticText* m_title = nullptr;
				wxBoxSizer* m_sizer = nullptr;

			public:
				AssetWidget(wxWindow* _parent, const std::string& _file_name);
				static void InitIcons();
		};

		class AssetWindow : public wxWindow {
			std::vector<AssetWidget*> m_widgets;
		};


		class AssetPanel : public wxWindow {
			private:
				wxAuiManager m_splitter;
				wxGenericDirCtrl* m_dir_ctrl = nullptr;
				std::vector<AssetWidget*> m_widgets;

			private:
				void _OnResize(wxSizeEvent& _evt);
				void _OnDirCtrlSelectionChanged(wxTreeEvent& _evt);
				wxDECLARE_EVENT_TABLE();


			public:
				AssetPanel(wxWindow* _parent, const std::string& _path);
		};
	}
}

#endif