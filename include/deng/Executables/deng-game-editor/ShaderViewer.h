// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ShaderViewer.h - Shader viewer dialog class header
// author: Karl-Mihkel Ott

#ifndef SHADER_VIEWER_H
#define SHADER_VIEWER_H

#ifdef SHADER_VIEWER_CPP
	#include <filesystem>
	#include <fstream>
	#include <sstream>

	#include <wx/wx.h>
	#include <wx/filename.h>
	#include <wx/mimetype.h>

	#include "deng/deng.h"
	#include "deng/Executables/deng-game-editor/GuiIds.h"
#endif

namespace DENG {
	namespace Editor {

		class ShaderViewer : public wxDialog {
			private:
				wxGridSizer* m_btn_sizer = nullptr;
				wxBoxSizer* m_sizer = nullptr;
				wxButton* m_edit_shader = nullptr;
				wxButton* m_commit_changes = nullptr;
				wxTextCtrl* m_shader_src_area = nullptr;
				wxButton* m_save_changes = nullptr;

				std::string m_source;
				const std::string m_tmp_file_name;

			private:
				void _OnEditShaderClick(wxCommandEvent& _evt);
				void _OnCommitChangesClick(wxCommandEvent& _evt);
				void _OnSaveChangesClick(wxCommandEvent& _evt);
				wxDECLARE_EVENT_TABLE();

			public:
				ShaderViewer(wxWindow *_parent, const std::string &_src, const std::string &_tmpfile_name);
				
				inline const std::string& GetSource() {
					return m_source;
				}
		};

	}
}

#endif