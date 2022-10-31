// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: MeshInspectorPanel.cpp - Mesh inspector panel class implementation
// author: Karl-Mihkel Ott

#define MESH_INSPECTOR_PANEL_CPP
#include "deng/Executables/deng-game-editor/MeshInspectorPanel.h"

namespace DENG {
	namespace Editor {

		wxBEGIN_EVENT_TABLE(TextureWidget, wxWindow)
			EVT_PAINT(TextureWidget::_OnPaint)
			EVT_CHECKBOX(ID_EDITOR_MESH_PANEL_TEXTURE_PICKER_USE_TEXTURE, TextureWidget::_OnCheckBoxClick)
		wxEND_EVENT_TABLE()
		
		TextureWidget::TextureWidget(wxWindow* _parent, MeshLoader *_mesh, RawTextureData &_data, uint32_t _id) :
			wxWindow(_parent, wxID_ANY, wxDefaultPosition, wxSize(96, 96)),
			m_mesh(_mesh),
			m_data(_data),
			m_id(_id)
		{
			wxImage img = wxImage(wxSize(_data.width, _data.height), _data.raw);
			img.Rescale(64, 64);
			m_image = wxBitmap(img);

			m_use = new wxCheckBox(this, ID_EDITOR_MESH_PANEL_TEXTURE_PICKER_USE_TEXTURE, "Use this texture", wxPoint(0, 68));
		}


		void TextureWidget::_OnPaint(wxPaintEvent& _evt) {
			wxClientDC dc(this);
			dc.DrawBitmap(m_image, wxPoint(0, 0), true);
		}


		void TextureWidget::_OnCheckBoxClick(wxCommandEvent &_evt) {
			TexturePicker* picker = (TexturePicker*)GetParent();
			if (_evt.IsChecked()) {
				picker->PickTexture(m_id);
			} else {
				picker->UnpickTexture(m_id);
			}
		}


		wxBEGIN_EVENT_TABLE(TexturePicker, wxDialog)
			EVT_BUTTON(ID_EDITOR_MESH_PANEL_TEXTURE_PICKER_OK, TexturePicker::_OnOk)
		wxEND_EVENT_TABLE()

		TexturePicker::TexturePicker(wxWindow* _parent, MeshLoader* _mesh, Renderer *_rend) :
			wxDialog(_parent, wxID_ANY, "Pick a texture", wxDefaultPosition, wxSize(600, 400)),
			m_mesh(_mesh)
		{
			m_sizer = new wxBoxSizer(wxVERTICAL);
			m_selected = new wxStaticText(this, wxID_ANY, "Selected " + std::to_string(m_picked_counter) + " textures");
			m_grid = new wxGridSizer(4, wxSize(96, 96));
			m_ok = new wxButton(this, ID_EDITOR_MESH_PANEL_TEXTURE_PICKER_OK, "OK");

			m_sizer->Add(m_selected, 0, wxALIGN_CENTER);
			m_sizer->Add(m_grid, 0, wxALIGN_CENTER, 10);
			m_sizer->Add(m_ok, 0, wxALIGN_RIGHT, 20);

			auto texture_names = _rend->GetTextureNames();

			for (auto it = texture_names.begin(); it != texture_names.end(); it++) {
				RawTextureData data;
				_rend->GetTexturePointer(*it, data);
				m_widgets.push_back(new TextureWidget(this, m_mesh, data, m_widgets.size()));
				m_grid->Add(m_widgets.back(), 0, wxALIGN_CENTER);
				m_picked_textures.push_back(false);
			}

			SetSizerAndFit(m_sizer);
		}


		void TexturePicker::_CheckLimit() {
			m_selected->SetLabel("Selected " + std::to_string(m_picked_counter) + " textures");
			if (m_picked_counter == m_mesh->GetSupportedTextureCount()) {
				for (size_t i = 0; i < m_picked_textures.size(); i++) {
					if (!m_picked_textures[i]) {
						m_widgets[i]->Disable();
					}
				}
			} else if (m_picked_counter == m_mesh->GetSupportedTextureCount() - 1) {
				for (size_t i = 0; i < m_picked_textures.size(); i++) {
					if (!m_picked_textures[i]) {
						m_widgets[i]->Enable();
					}
				}
			}
		}


		void TexturePicker::_OnOk(wxCommandEvent& _evt) {
			EndModal(wxID_OK);
		}


		wxBEGIN_EVENT_TABLE(MeshInspectorPanel, wxWindow)
			EVT_CHECKBOX(ID_EDITOR_MESH_PANEL_USE_TEXTURES, MeshInspectorPanel::_OnUseTextureChecked)
			EVT_COLOURPICKER_CHANGED(ID_EDITOR_MESH_PANEL_COLOR_PICKER, MeshInspectorPanel::_OnColorChanged)
			EVT_BUTTON(ID_EDITOR_MESH_PANEL_PICK_TEXTURE, MeshInspectorPanel::_OnPickTexturesClick)

			EVT_BUTTON(ID_EDITOR_MESH_PANEL_EDIT_VERTEX_SHADER, MeshInspectorPanel::_OnEditVertexShaderClick)
			EVT_BUTTON(ID_EDITOR_MESH_PANEL_EDIT_FRAGMENT_SHADER, MeshInspectorPanel::_OnEditFragmentShaderClick)
			EVT_BUTTON(ID_EDITOR_MESH_PANEL_EDIT_GEOMETRY_SHADER, MeshInspectorPanel::_OnEditGeometryShaderClick)
		wxEND_EVENT_TABLE()

		MeshInspectorPanel::MeshInspectorPanel(wxWindow *_parent, Renderer *_rend) :
			wxWindow(_parent, wxID_ANY, wxDefaultPosition, wxSize(200, 400)),
			m_renderer(_rend)
		{
			m_sizer = new wxBoxSizer(wxVERTICAL);
			m_title = new wxStaticText(this, ID_EDITOR_MESH_PANEL_TITLE, wxT("(null)"));
			m_use_textures = new wxCheckBox(this, ID_EDITOR_MESH_PANEL_USE_TEXTURES, "Use textures");
			m_use_textures->SetValue(false);
			
			m_color_picker_text = new wxStaticText(this, wxID_ANY, wxT("Pick a color"));
			m_color_picker = new wxColourPickerCtrl(this, ID_EDITOR_MESH_PANEL_COLOR_PICKER, wxColour(0, 255, 0, 255));
			
			m_pick_textures = new wxButton(this, ID_EDITOR_MESH_PANEL_PICK_TEXTURE, "Pick textures");
			m_pick_textures->Disable();

			m_sizer->Add(m_title, 0, wxALIGN_CENTER);
			m_sizer->Add(m_use_textures, 0, wxALIGN_LEFT, 10);
			m_sizer->Add(m_color_picker_text, 0, wxALIGN_LEFT);
			m_sizer->Add(m_color_picker, 0, wxALIGN_LEFT);
			m_sizer->Add(m_pick_textures, 0, wxALIGN_LEFT, 5);

			m_shader_mod_title = new wxStaticText(this , wxID_ANY, wxT("Shader modifications"));
			m_edit_vertex_shader = new wxButton(this, ID_EDITOR_MESH_PANEL_EDIT_VERTEX_SHADER, wxT("Edit vertex shader"));
			m_edit_fragment_shader = new wxButton(this, ID_EDITOR_MESH_PANEL_EDIT_FRAGMENT_SHADER, wxT("Edit fragment shader"));
			m_edit_geometry_shader = new wxButton(this, ID_EDITOR_MESH_PANEL_EDIT_GEOMETRY_SHADER, wxT("Edit geometry shader"));

			m_sizer->Add(m_shader_mod_title, 0, wxALIGN_CENTER, 10);
			m_sizer->Add(m_edit_vertex_shader, 0, wxALIGN_LEFT);
			m_sizer->Add(m_edit_fragment_shader, 0, wxALIGN_LEFT);
			m_sizer->Add(m_edit_geometry_shader, 0, wxALIGN_LEFT);
			SetSizerAndFit(m_sizer);
		}


		void MeshInspectorPanel::View(MeshLoader* _mesh) {
			m_mesh = _mesh;
			m_title->SetLabelText(m_mesh->GetName());

			Show();
		}


		void MeshInspectorPanel::_OnColorChanged(wxColourPickerEvent& _evt) {
			TRS::Vector4<float> color = {
				static_cast<float>(_evt.GetColour().Red()) / 255.f,
				static_cast<float>(_evt.GetColour().Green()) / 255.f,
				static_cast<float>(_evt.GetColour().Blue()) / 255.f,
				static_cast<float>(_evt.GetColour().Alpha()) / 255.f
			};
			m_mesh->SetColor(color);
		}


		void MeshInspectorPanel::_OnUseTextureChecked(wxCommandEvent& _evt) {
			if (_evt.IsChecked()) {
				m_color_picker->Disable();
				m_pick_textures->Enable();
			} else {
				m_pick_textures->Disable();
				m_color_picker->Enable();
				m_mesh->SetUseColorBit(true);
			}
		}


		void MeshInspectorPanel::_OnPickTexturesClick(wxCommandEvent& _evt) {
			TexturePicker* picker = new TexturePicker(this, m_mesh, m_renderer);
			if (picker->ShowModal() == wxID_OK) {
				std::vector<std::string> names;
				names.reserve(m_renderer->GetTextureNames().size());
				for (size_t i = 0; i < m_renderer->GetTextureNames().size(); i++) {
					if (picker->GetPickedTextures()[i]) {
						names.push_back(m_renderer->GetTextureNames()[i]);
					}
				}

				m_mesh->UseTextures(names);
				m_mesh->SetUseColorBit(false);
			} 
		}

		void MeshInspectorPanel::_OnEditVertexShaderClick(wxCommandEvent& _evt) {
			ShaderViewer* viewer = new ShaderViewer(this, m_renderer->GetShaderModule(m_mesh->GetShaderId()).vertex_shader_src, "vert.txt");
			if (viewer->ShowModal() == wxID_OK) {
				m_renderer->GetShaderModule(m_mesh->GetShaderId()).vertex_shader_src = viewer->GetSource();
				m_renderer->ReloadShaderModule(m_mesh->GetShaderId());
			}
		}

		void MeshInspectorPanel::_OnEditFragmentShaderClick(wxCommandEvent& _evt) {
			ShaderViewer* viewer = new ShaderViewer(this, m_renderer->GetShaderModule(m_mesh->GetShaderId()).fragment_shader_src, "frag.txt");
			if (viewer->ShowModal() == wxID_OK) {
				auto& shader = m_renderer->GetShaderModule(m_mesh->GetShaderId());
				shader.fragment_shader_src = viewer->GetSource();
				m_renderer->ReloadShaderModule(m_mesh->GetShaderId());
			}
		}

		void MeshInspectorPanel::_OnEditGeometryShaderClick(wxCommandEvent& _evt) {
			ShaderViewer* viewer = new ShaderViewer(this, m_renderer->GetShaderModule(m_mesh->GetShaderId()).geometry_shader_src, "geom.txt");
			if (viewer->ShowModal() == wxID_OK) {
				m_renderer->GetShaderModule(m_mesh->GetShaderId()).geometry_shader_src = viewer->GetSource();
				m_renderer->ReloadShaderModule(m_mesh->GetShaderId());
			}
		}
	}
}