// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: MeshInspectorPanel.h - Mesh inspector panel class header
// author: Karl-Mihkel Ott

#ifndef MESH_INSPECTOR_PANEL_H
#define MESH_INSPECTOR_PANEL_H

#ifdef MESH_INSPECTOR_PANEL_CPP
	#include <wx/wx.h>
	#include <wx/collpane.h>
	#include <wx/collheaderctrl.h>
	#include <wx/clrpicker.h>
	#include "deng/deng.h"

	#include "deng/Executables/deng-game-editor/GuiIds.h"
#endif

namespace DENG {
	namespace Editor {

		class TextureWidget : public wxWindow {
			private:
				MeshLoader* m_mesh = nullptr;
				RawTextureData m_data;
				wxBitmap m_image;
				wxCheckBox* m_use = nullptr;
				uint32_t m_id = 0;

			private:
				void _OnPaint(wxPaintEvent& _evt);
				void _OnCheckBoxClick(wxCommandEvent& _evt);
				wxDECLARE_EVENT_TABLE();

			public:
				TextureWidget(wxWindow* _parent, MeshLoader *_mesh, RawTextureData &_data, uint32_t _id);
				~TextureWidget();
		};


		class TexturePicker : public wxDialog {
			private:
				std::vector<bool> m_picked_textures;
				std::vector<TextureWidget*> m_widgets;
				uint32_t m_picked_counter = 0;

				MeshLoader* m_mesh = nullptr;
				wxBoxSizer* m_sizer = nullptr;
				wxStaticText* m_selected = nullptr;
				wxGridSizer* m_grid = nullptr;
				wxButton* m_ok = nullptr;

			private:
				// if texture limit is reached, disable all non-active textures
				void _CheckLimit();
				void _OnOk(wxCommandEvent &_evt);
				wxDECLARE_EVENT_TABLE();

			public:
				TexturePicker(wxWindow *_parent, MeshLoader *_mesh, Renderer *_rend);

				inline void PickTexture(uint32_t _id) {
					DENG_ASSERT(_id < m_picked_textures.size());
					m_picked_textures[_id] = true;
					m_picked_counter++;
					_CheckLimit();
				}

				inline void UnpickTexture(uint32_t _id) {
					DENG_ASSERT(_id < m_picked_textures.size());
					m_picked_textures[_id] = false;
					m_picked_counter--;
					_CheckLimit();
				}

				inline bool IsPicked() {
					return m_picked_counter == m_mesh->GetSupportedTextureCount();
				}

				inline const std::vector<bool>& GetPickedTextures() {
					return m_picked_textures;
				}
		};

		class MeshInspectorPanel : public wxWindow {
			private:
				Renderer* m_renderer;
				MeshLoader* m_mesh = nullptr;
				wxBoxSizer* m_sizer = nullptr;
				wxStaticText* m_title = nullptr;
				wxCheckBox* m_use_textures = nullptr;
				wxStaticText* m_color_picker_text = nullptr;
				wxColourPickerCtrl* m_color_picker = nullptr;

				wxGenericCollapsiblePane* m_texture_names = nullptr;
				wxButton* m_pick_textures = nullptr;
				// TexturePicker* m_texture_picker = nullptr;

			private:
				void _OnColorChanged(wxColourPickerEvent& _evt);
				void _OnUseTextureChecked(wxCommandEvent& _evt);
				void _OnPickTexturesClick(wxCommandEvent& _evt);
				wxDECLARE_EVENT_TABLE();

			public:
				MeshInspectorPanel(wxWindow* _parent, Renderer *_rend);
				void View(MeshLoader* _mesh);
		};
	}
}

#endif