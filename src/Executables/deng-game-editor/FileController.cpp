#define FILE_CONTROLLER_CPP
#include "deng/Executables/deng-game-editor/FileController.h"

namespace DENG {
	namespace Editor {
		
		wxBEGIN_EVENT_TABLE(RenameDialog, wxDialog)
			EVT_BUTTON(ID_RENAME_DIALOG_OK, RenameDialog::_OnOkClick)
			EVT_BUTTON(ID_RENAME_DIALOG_CANCEL, RenameDialog::_OnCancelClick)
		wxEND_EVENT_TABLE()

		RenameDialog::RenameDialog(wxWindow* _parent, const std::string& _fname) :
			wxDialog(_parent, ID_RENAME_DIALOG, wxT("Rename a file"))
		{
			m_general_sizer = new wxBoxSizer(wxVERTICAL);
			m_btn_sizer = new wxBoxSizer(wxHORIZONTAL);

			m_description = new wxStaticText(this, wxID_ANY, wxString("Enter a new filename"));
			m_name_ctrl = new wxTextCtrl(this, ID_RENAME_DIALOG_CTRL, _fname, wxDefaultPosition, wxSize(200, 20));
			m_general_sizer->Add(m_description);
			m_general_sizer->Add(m_name_ctrl);

			m_ok = new wxButton(this, ID_RENAME_DIALOG_OK, wxT("Ok"));
			m_cancel = new wxButton(this, ID_RENAME_DIALOG_CANCEL, wxT("Cancel"));
			m_btn_sizer->Add(m_ok);
			m_btn_sizer->Add(m_cancel);
			m_general_sizer->Add(m_btn_sizer, 0, 10, wxALIGN_RIGHT);

			SetSizerAndFit(m_general_sizer);
		}

		void RenameDialog::_OnOkClick(wxCommandEvent& _ev) {
			m_name = m_name_ctrl->GetValue();
			EndModal(wxID_OK);
		}

		void RenameDialog::_OnCancelClick(wxCommandEvent& _ev) {
			EndModal(wxID_CANCEL);
		}


		wxBEGIN_EVENT_TABLE(FileController, wxGenericDirCtrl)
			EVT_MENU(ID_FILE_CONTROLLER_MENU_OPEN, FileController::_OnMenu_Open)
			EVT_MENU(ID_FILE_CONTROLLER_MENU_OPEN_WITH, FileController::_OnMenu_OpenWith)
			EVT_MENU(ID_FILE_CONTROLLER_MENU_ADD_FILE, FileController::_OnMenu_AddFile)
			EVT_MENU(ID_FILE_CONTROLLER_MENU_ADD_FOLDER, FileController::_OnMenu_AddFolder)
			EVT_MENU(ID_FILE_CONTROLLER_MENU_COPY, FileController::_OnMenu_Copy)
			EVT_MENU(ID_FILE_CONTROLLER_MENU_CUT, FileController::_OnMenu_Cut)
			EVT_MENU(ID_FILE_CONTROLLER_MENU_PASTE, FileController::_OnMenu_Paste)
			EVT_MENU(ID_FILE_CONTROLLER_MENU_DELETE, FileController::_OnMenu_Delete)
			EVT_MENU(ID_FILE_CONTROLLER_MENU_RENAME, FileController::_OnMenu_Rename)
			EVT_MENU(ID_FILE_CONTROLLER_MENU_DOS_SHELL, FileController::_OnMenu_DOSShell)
			EVT_MENU(ID_FILE_CONTROLLER_MENU_PROPERTIES, FileController::_OnMenu_Properties)
		wxEND_EVENT_TABLE()

		FileController::FileController(wxWindow *_parent, const std::string &_path) :
			wxGenericDirCtrl(_parent, ID_FILE_CONTROLLER, wxString(_path), wxDefaultPosition, wxDefaultSize, 0)
		{
			Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, &FileController::_OnDirCtrlRightClick, this);

			m_menu.Append(ID_FILE_CONTROLLER_MENU_OPEN, wxT("&Open"));
			m_menu.Append(ID_FILE_CONTROLLER_MENU_OPEN_WITH, wxT("&Open with..."));
			m_menu.AppendSeparator();
			
			m_menu.Append(ID_FILE_CONTROLLER_MENU_ADD_FILE, wxT("&Add File"));
			m_menu.Append(ID_FILE_CONTROLLER_MENU_ADD_FOLDER, wxT("&Add Folder"));
			m_menu.AppendSeparator();
			
			m_menu.Append(ID_FILE_CONTROLLER_MENU_COPY, wxT("&Copy (Ctrl-C)"));
			m_menu.Append(ID_FILE_CONTROLLER_MENU_CUT, wxT("&Cut (Ctrl-X)"));
			m_menu.Append(ID_FILE_CONTROLLER_MENU_PASTE, wxT("&Paste (Ctrl-V)"));
			m_menu.AppendSeparator();

			m_menu.Append(ID_FILE_CONTROLLER_MENU_DELETE, wxT("&Delete (Del)"));
			m_menu.Append(ID_FILE_CONTROLLER_MENU_RENAME, wxT("&Rename"));
			m_menu.AppendSeparator();

			m_menu.Append(ID_FILE_CONTROLLER_MENU_DOS_SHELL, wxT("&DOS Shell"));
			m_menu.Append(ID_FILE_CONTROLLER_MENU_PROPERTIES, wxT("&Properties"));
		}


		void FileController::_OnDirCtrlRightClick(wxTreeEvent& _ev) {
			m_right_click_selected_tc = nullptr;
			wxTreeItemId id = _ev.GetItem();
			m_right_click_selected_tc = wxDynamicCast(_ev.GetEventObject(), wxTreeCtrl);

			if (m_right_click_selected_tc) {
				m_right_click_selected_tc->SelectItem(id);
				PopupMenu(&m_menu);
			}
		}

		void FileController::_OnMenu_Open(wxCommandEvent& _ev) {
			wxString fpth = GetFilePath();
			if (fpth != "") {
				int needed_size = MultiByteToWideChar(CP_UTF8, 0, fpth.c_str(), (int)fpth.size(), NULL, 0);
				std::wstring wpth(needed_size, 0);
				MultiByteToWideChar(CP_UTF8, 0, fpth.c_str(), needed_size, wpth.data(), needed_size);

				SHELLEXECUTEINFO info = {};
				info.cbSize = sizeof(info);
				info.lpFile = fpth.c_str();
				info.nShow = SW_SHOW;
				info.fMask = SEE_MASK_INVOKEIDLIST;
				info.lpVerb = L"open";
				ShellExecuteEx(&info);
			}
			else {
				ExpandPath(GetPath());
			}
		}

		void FileController::_OnMenu_OpenWith(wxCommandEvent& _ev) {
			wxString file = GetFilePath();
			if (file != "") {
				OPENASINFO info = {};
				info.pcszFile = file.c_str();
				info.oaifInFlags = OAIF_ALLOW_REGISTRATION | OAIF_EXEC;
				SHOpenWithDialog(GetHWND(), &info);
			}
		}

		void FileController::_OnMenu_AddFile(wxCommandEvent& _ev) {
			if (GetFilePath() == "") {
				std::string pth = GetPath();
				std::string new_file_name = "New File.txt";
				int lvl = 0;
				while (std::filesystem::exists(pth + '/' + new_file_name)) {
					new_file_name = "New File (" + std::to_string(++lvl) + ").txt";
				}

				std::ofstream stream(pth + '/' + new_file_name, std::ios::out);
				stream.close();
				
				SetPath(pth);
				ReCreateTree();
			}
		}

		void FileController::_OnMenu_AddFolder(wxCommandEvent& _ev) {
			if (GetFilePath() == "") {
				std::string pth = GetPath();
				std::string new_dir_name = "New Folder";
				int lvl = 0;
				while (std::filesystem::exists(pth + '/' + new_dir_name)) {
					new_dir_name = "New Folder (" + std::to_string(++lvl) + ").txt";
				}

				std::filesystem::create_directory(pth + '/' + new_dir_name);
				SetPath(pth);
				ReCreateTree();
			}
		}

		void FileController::_OnMenu_Copy(wxCommandEvent& _ev) {
			if (wxTheClipboard->Open()) {
				wxTheClipboard->Clear();

				wxFileDataObject* data = new wxFileDataObject;
				data->AddFile(GetPath());

				wxTheClipboard->SetData(data);
				wxTheClipboard->Close();
			}
		}


		void FileController::_OnMenu_Cut(wxCommandEvent& _ev) {
			const std::string pth = GetPath();
			const std::string tmp_pth = std::filesystem::temp_directory_path().u8string();

			if (std::filesystem::exists(pth) && wxTheClipboard->Open()) {
				// move files to %Temp%
				std::string fname = std::filesystem::path(pth).filename().u8string();
				std::filesystem::copy(pth, tmp_pth, std::filesystem::copy_options::recursive |
													std::filesystem::copy_options::overwrite_existing);
				std::filesystem::remove_all(pth);

				wxFileDataObject* data = new wxFileDataObject;
				data->AddFile(wxString(tmp_pth + '/' + fname));

				wxTheClipboard->SetData(data);
				wxTheClipboard->Close();

				if (GetFilePath() == "" && !std::filesystem::is_empty(pth)) {
					std::filesystem::remove_all(pth);
				} else {
					std::filesystem::remove(pth);
				}

				SetPath(std::filesystem::path(pth).parent_path().u8string());
				ReCreateTree();
			}
		}

		void FileController::_OnMenu_Paste(wxCommandEvent& _ev) {
			if (GetFilePath() == "" && wxTheClipboard->Open()) {
				const std::string pth = GetPath();
				if (wxTheClipboard->IsSupported(wxDF_FILENAME)) {
					wxFileDataObject data;
					wxTheClipboard->GetData(data);
					
					auto& fnames = data.GetFilenames();
					for (auto& f : fnames) {
						const std::string stdf = f;
						if (std::filesystem::exists(stdf)) {
							std::filesystem::copy(stdf, pth, std::filesystem::copy_options::recursive);
						}
					}

					SetPath(pth);
					ReCreateTree();
				}
			}
		}


		void FileController::_OnMenu_Delete(wxCommandEvent& _ev) {
			// is directory
			std::string pth = GetPath();
			if (GetFilePath() == "" && !std::filesystem::is_empty(pth)) {
				const std::string msg = "Folder " + pth + " is not empty.\nWould you like to delete this folder anyways?";
				if (wxMessageBox(wxString(msg), wxT("Folder is not empty"), wxYES_NO | wxCANCEL | wxICON_WARNING, GetParent()) == wxYES) {
					std::filesystem::remove_all(pth);
				}
			}
			else {
				std::filesystem::remove(pth);
			}

			SetPath(pth);
			ReCreateTree();
		}

		void FileController::_OnMenu_Rename(wxCommandEvent& _ev) {
			auto pth = std::filesystem::path(GetPath().ToStdString());
			const std::string old_spth = pth.u8string();
			RenameDialog* diag = new RenameDialog(this, pth.filename().u8string());

			if (diag->ShowModal() == wxID_OK) {
				const std::string nname = diag->GetNewName();
				pth.replace_filename(nname);
				// copy to new file
				std::filesystem::copy(old_spth, pth, std::filesystem::copy_options::recursive |
													 std::filesystem::copy_options::overwrite_existing);
				// remove old file
				std::filesystem::remove_all(old_spth);

				SetPath(pth.u8string());
				ReCreateTree();
			}
		}

		void FileController::_OnMenu_DOSShell(wxCommandEvent& _ev) {
			if (GetFilePath() == "") {
				wxString pth = GetPath();
				int needed_size = MultiByteToWideChar(CP_UTF8, 0, pth.c_str(), (int)pth.size(), NULL, 0);
				std::wstring wpth(needed_size, 0);
				MultiByteToWideChar(CP_UTF8, 0, pth.c_str(), needed_size, wpth.data(), needed_size);

				const std::wstring cmd = L"C:\\Windows\\System32\\cmd.exe";

				SHELLEXECUTEINFO info = {};
				info.cbSize = sizeof(info);
				info.lpFile = cmd.c_str();
				info.nShow = SW_SHOW;
				info.fMask = SEE_MASK_INVOKEIDLIST;
				info.lpDirectory = wpth.c_str();
				info.lpVerb = L"open";
				ShellExecuteEx(&info);
			}
		}

		void FileController::_OnMenu_Properties(wxCommandEvent& _ev) {
			wxString pth = GetPath();
			int needed_size = MultiByteToWideChar(CP_UTF8, 0, pth.c_str(), (int)pth.size(), NULL, 0);
			std::wstring wpth(needed_size, 0);
			MultiByteToWideChar(CP_UTF8, 0, pth.c_str(), (int)pth.size(), wpth.data(), needed_size);

			SHELLEXECUTEINFO info = {};
			info.cbSize = sizeof(info);
			info.lpFile = wpth.c_str();
			info.nShow = SW_SHOW;
			info.fMask = SEE_MASK_INVOKEIDLIST;
			info.lpVerb = L"properties";
			ShellExecuteEx(&info);
		}
	}
}