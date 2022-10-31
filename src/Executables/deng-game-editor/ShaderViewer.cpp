// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ShaderViewer.h - Shader viewer dialog class implementation
// author: Karl-Mihkel Ott

#define SHADER_VIEWER_CPP
#include "deng/Executables/deng-game-editor/ShaderViewer.h"

namespace DENG {
	namespace Editor {
		
		wxBEGIN_EVENT_TABLE(ShaderViewer, wxDialog)
			EVT_BUTTON(ID_SHADER_VIEWER_EDIT_SHADER, ShaderViewer::_OnEditShaderClick)
			EVT_BUTTON(ID_SHADER_VIEWER_COMMIT_CHANGES, ShaderViewer::_OnCommitChangesClick)
			EVT_BUTTON(ID_SHADER_VIEWER_SAVE_CHANGES, ShaderViewer::_OnSaveChangesClick)
		wxEND_EVENT_TABLE()

		ShaderViewer::ShaderViewer(wxWindow* _parent, const std::string& _src, const std::string& _tmpfile_name) :
			wxDialog(_parent, wxID_ANY, "View and change a shader"),
			m_source(_src),
			m_tmp_file_name(_tmpfile_name)
		{
			m_btn_sizer = new wxGridSizer(2);
			m_sizer = new wxBoxSizer(wxVERTICAL);

			m_edit_shader = new wxButton(this, ID_SHADER_VIEWER_EDIT_SHADER, "Edit shader");
			m_btn_sizer->Add(m_edit_shader);
			m_commit_changes = new wxButton(this, ID_SHADER_VIEWER_COMMIT_CHANGES, "Commit changes");
			m_btn_sizer->Add(m_commit_changes);
			m_sizer->Add(m_btn_sizer);
			m_shader_src_area = new wxTextCtrl(this, ID_SHADER_VIEWER_SHADER_SOURCE_VIEW, m_source, wxDefaultPosition, wxSize(500, 500), wxTE_READONLY | wxTE_MULTILINE);
			m_sizer->Add(m_shader_src_area, 0, wxALIGN_LEFT, 10);

			m_save_changes = new wxButton(this, ID_SHADER_VIEWER_SAVE_CHANGES, "Save changes");
			m_sizer->Add(m_save_changes, 0, wxALIGN_RIGHT);
			SetSizerAndFit(m_sizer);
		}


		void ShaderViewer::_OnEditShaderClick(wxCommandEvent& _evt) {
			std::string path = std::filesystem::temp_directory_path().u8string() + m_tmp_file_name;
			std::ofstream stream(path);
			stream.write(m_source.data(), m_source.size());
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
			std::string path = std::filesystem::temp_directory_path().u8string() + m_tmp_file_name;
			std::ifstream stream(path);

			std::stringstream buffer;
			buffer << stream.rdbuf();
			m_source = buffer.str();
			m_shader_src_area->SetValue(m_source);
		}

		void ShaderViewer::_OnSaveChangesClick(wxCommandEvent& _evt) {
			EndModal(wxID_OK);
		}

	}
}