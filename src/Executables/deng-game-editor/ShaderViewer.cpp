// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ShaderViewer.h - Shader viewer dialog class implementation
// author: Karl-Mihkel Ott

#define SHADER_VIEWER_CPP
#include "deng/Executables/deng-game-editor/ShaderViewer.h"

namespace DENG {
	namespace Editor {
		
		wxBEGIN_EVENT_TABLE(ShaderViewer, wxDialog)
			EVT_BUTTON(ID_SHADER_VIEWER_EDIT_VERT_SHADER, ShaderViewer::_OnEditShaderClick)
			EVT_BUTTON(ID_SHADER_VIEWER_COMMIT_VERT_SHADER_CHANGES, ShaderViewer::_OnCommitChangesClick)
			EVT_BUTTON(ID_SHADER_VIEWER_SAVE_CHANGES, ShaderViewer::_OnSaveChangesClick)

			EVT_BUTTON(ID_SHADER_VIEWER_EDIT_GEOM_SHADER, ShaderViewer::_OnEditShaderClick)
			EVT_BUTTON(ID_SHADER_VIEWER_COMMIT_GEOM_SHADER_CHANGES, ShaderViewer::_OnCommitChangesClick)

			EVT_BUTTON(ID_SHADER_VIEWER_EDIT_FRAG_SHADER, ShaderViewer::_OnEditShaderClick)
			EVT_BUTTON(ID_SHADER_VIEWER_COMMIT_FRAG_SHADER_CHANGES, ShaderViewer::_OnCommitChangesClick)
		wxEND_EVENT_TABLE()

		ShaderViewer::ShaderViewer(wxWindow* _parent, const std::string &_vert, const std::string &_geom, const std::string &_frag) :
			wxDialog(_parent, wxID_ANY, "View and change a shader"),
			m_vert_src(_vert),
			m_geom_src(_geom),
			m_frag_src(_frag)
		{
			const wxSize text_size = wxSize(500, 200);
			m_sizer = new wxBoxSizer(wxVERTICAL);

			// vertex shader properties
			m_vert_btn_sizer = new wxGridSizer(2);
			m_edit_vert_shader = new wxButton(this, ID_SHADER_VIEWER_EDIT_VERT_SHADER, wxT("Edit vertex shader"));
			m_vert_btn_sizer->Add(m_edit_vert_shader);
			m_commit_vert_shader_changes = new wxButton(this, ID_SHADER_VIEWER_COMMIT_VERT_SHADER_CHANGES, wxT("Commit vertex shader changes"));
			m_vert_btn_sizer->Add(m_commit_vert_shader_changes);
			m_vert_shader_src_area = new wxTextCtrl(this, ID_SHADER_VIEWER_FRAG_SHADER_SOURCE_VIEW, m_vert_src, wxDefaultPosition, text_size, wxTE_READONLY | wxTE_MULTILINE);
			m_sizer->Add(m_vert_btn_sizer);
			m_sizer->Add(m_vert_shader_src_area);

			wxStaticLine* vert_line = new wxStaticLine(this, wxID_ANY);
			m_sizer->Add(vert_line, 0, wxALIGN_LEFT, 5);

			// geometry shader properties
			m_geom_btn_sizer = new wxGridSizer(2);
			m_edit_geom_shader = new wxButton(this, ID_SHADER_VIEWER_EDIT_GEOM_SHADER, wxT("Edit geometry shader"));
			m_geom_btn_sizer->Add(m_edit_geom_shader);
			m_commit_geom_shader_changes = new wxButton(this, ID_SHADER_VIEWER_COMMIT_GEOM_SHADER_CHANGES, wxT("Commit geometry shader changes"));
			m_geom_btn_sizer->Add(m_commit_geom_shader_changes);
			m_geom_shader_src_area = new wxTextCtrl(this, ID_SHADER_VIEWER_GEOM_SHADER_SOURCE_VIEW, m_geom_src, wxDefaultPosition, text_size, wxTE_READONLY | wxTE_MULTILINE);
			m_sizer->Add(m_geom_btn_sizer);
			m_sizer->Add(m_geom_shader_src_area);

			wxStaticLine* geom_line = new wxStaticLine(this, wxID_ANY);
			m_sizer->Add(geom_line, 0, wxALIGN_LEFT, 5);

			// fragment shader properties
			m_frag_btn_sizer = new wxGridSizer(2);
			m_edit_frag_shader = new wxButton(this, ID_SHADER_VIEWER_EDIT_FRAG_SHADER, wxT("Edit fragment shader"));
			m_frag_btn_sizer->Add(m_edit_frag_shader);
			m_commit_frag_shader_changes = new wxButton(this, ID_SHADER_VIEWER_COMMIT_FRAG_SHADER_CHANGES, wxT("Commit fragment shader changes"));
			m_frag_btn_sizer->Add(m_commit_frag_shader_changes);
			m_frag_shader_src_area = new wxTextCtrl(this, ID_SHADER_VIEWER_FRAG_SHADER_SOURCE_VIEW, m_frag_src, wxDefaultPosition, text_size, wxTE_READONLY | wxTE_MULTILINE);
			m_sizer->Add(m_frag_btn_sizer);
			m_sizer->Add(m_frag_shader_src_area);

			wxStaticLine* frag_line = new wxStaticLine(this, wxID_ANY);
			m_sizer->Add(frag_line, 0, wxALIGN_LEFT, 5);

			m_save_changes = new wxButton(this, ID_SHADER_VIEWER_SAVE_CHANGES, "Save changes");
			m_sizer->Add(m_save_changes, 0, wxALIGN_RIGHT);
			SetSizerAndFit(m_sizer);
		}


		void ShaderViewer::_OnEditShaderClick(wxCommandEvent& _evt) {
			const std::array<std::string, 3> filenames = {
				"vert.txt",
				"geom.txt",
				"frag.txt"
			};

			const std::array<std::string*, 3> sources = {
				&m_vert_src,
				&m_geom_src,
				&m_frag_src
			};

			const int id = _evt.GetId() - (int)ID_SHADER_VIEWER_EDIT_VERT_SHADER;
			std::string path = std::filesystem::temp_directory_path().u8string() + filenames[id];
			std::ofstream stream(path);
			stream.write(sources[id]->data(), sources[id]->size());
			stream.close();

			wxFileName fname(path);
			wxFileType *type = wxTheMimeTypesManager->GetFileTypeFromExtension(fname.GetExt());

			if (!type) {
				wxLogMessage(wxT("Cannot open " + path));
				return;
			}

			wxString cmd = type->GetOpenCommand(path);
			wxExecute(cmd);
			delete type;
		}


		void ShaderViewer::_OnCommitChangesClick(wxCommandEvent& _evt) {
			const std::array<std::string, 3> filenames = {
				"vert.txt",
				"geom.txt",
				"frag.txt"
			};

			const std::array<std::string*, 3> sources = {
				&m_vert_src,
				&m_geom_src,
				&m_frag_src
			};

			const std::array<wxTextCtrl*, 3> src_areas = {
				m_vert_shader_src_area,
				m_geom_shader_src_area,
				m_frag_shader_src_area
			};

			const int id = _evt.GetId() - (int)ID_SHADER_VIEWER_COMMIT_VERT_SHADER_CHANGES;

			std::string path = std::filesystem::temp_directory_path().u8string() + filenames[id];
			std::ifstream stream(path);

			std::stringstream buffer;
			buffer << stream.rdbuf();
			*sources[id] = buffer.str();
			src_areas[id]->SetValue(*sources[id]);
		}

		void ShaderViewer::_OnSaveChangesClick(wxCommandEvent& _evt) {
			EndModal(wxID_OK);
		}

	}
}