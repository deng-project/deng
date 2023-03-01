#ifndef FILE_CONTROLLER_H
#define FILE_CONTROLLER_H

#ifdef FILE_CONTROLLER_CPP
	#include <wx/wx.h>
	#include <wx/dirctrl.h>
	#include <wx/clipbrd.h>
	#include <ShlObj_core.h>

	#include <filesystem>
	#include <fstream>

	#include "deng/Executables/deng-game-editor/GuiIds.h"
#endif

namespace DENG {
	namespace Editor {

		class RenameDialog : public wxDialog {
			private:
				wxBoxSizer* m_general_sizer;
				wxStaticText* m_description = nullptr;
				wxTextCtrl* m_name_ctrl = nullptr;

				wxBoxSizer* m_btn_sizer;
				wxButton* m_cancel = nullptr;
				wxButton* m_ok = nullptr;

				std::string m_name;

			private:
				void _OnOkClick(wxCommandEvent& _ev);
				void _OnCancelClick(wxCommandEvent& _ev);
				wxDECLARE_EVENT_TABLE();

			public:
				RenameDialog(wxWindow* _parent, const std::string& _fname);

				inline const std::string GetNewName() {
					return m_name;
				}
		};

		class FileController : public wxGenericDirCtrl {
			private:
				wxMenu m_menu;
				wxTreeCtrl* m_right_click_selected_tc = nullptr;

			private:
				void _OnDirCtrlRightClick(wxTreeEvent& _ev);
				void _OnDirCtrlActivate(wxTreeEvent& _ev);

				// menu options
				void _OnMenu_Open(wxCommandEvent& _ev);
				void _OnMenu_OpenWith(wxCommandEvent& _ev);

				void _OnMenu_AddFile(wxCommandEvent& _ev);
				void _OnMenu_AddFolder(wxCommandEvent& _ev);

				void _OnMenu_Copy(wxCommandEvent& _ev);
				void _OnMenu_Cut(wxCommandEvent& _ev);
				void _OnMenu_Paste(wxCommandEvent& _ev);

				void _OnMenu_Delete(wxCommandEvent& _ev);
				void _OnMenu_Rename(wxCommandEvent& _ev);
				
				void _OnMenu_DOSShell(wxCommandEvent& _ev);
				void _OnMenu_Properties(wxCommandEvent& _ev);
				
				wxDECLARE_EVENT_TABLE();

			public:
				FileController(wxWindow* _parent, const std::string& _path);
		};
	}
}

#endif
