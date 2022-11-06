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
	#include <wx/statline.h>

	#include "deng/deng.h"
	#include "deng/Executables/deng-game-editor/GuiIds.h"
#endif

namespace DENG {
	namespace Editor {

		class ShaderViewer : public wxDialog {
			private:
				wxBoxSizer* m_sizer = nullptr;
				
				wxGridSizer* m_vert_btn_sizer = nullptr;
				wxButton* m_edit_vert_shader = nullptr;
				wxButton* m_commit_vert_shader_changes = nullptr;
				wxTextCtrl* m_vert_shader_src_area = nullptr;

				wxGridSizer* m_geom_btn_sizer = nullptr;
				wxButton* m_edit_geom_shader = nullptr;
				wxButton* m_commit_geom_shader_changes = nullptr;
				wxTextCtrl* m_geom_shader_src_area = nullptr;

				wxGridSizer* m_frag_btn_sizer = nullptr;
				wxButton* m_edit_frag_shader = nullptr;
				wxButton* m_commit_frag_shader_changes = nullptr;
				wxTextCtrl* m_frag_shader_src_area = nullptr;
				
				wxButton* m_save_changes = nullptr;

				std::string m_vert_src;
				std::string m_geom_src;
				std::string m_frag_src;

			private:
				void _OnEditShaderClick(wxCommandEvent& _evt);
				void _OnCommitChangesClick(wxCommandEvent& _evt);
				void _OnSaveChangesClick(wxCommandEvent& _evt);
				wxDECLARE_EVENT_TABLE();

			public:
				ShaderViewer(wxWindow *_parent, const std::string &_vert, const std::string &_geom, const std::string &_frag);
				
				inline const std::string& GetVertexSource() {
					return m_vert_src;
				}

				inline const std::string& GetGeometrySource() {
					return m_geom_src;
				}

				inline const std::string& GetFragmentSource() {
					return m_frag_src;
				}
		};

	}
}

#endif