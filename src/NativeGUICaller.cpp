// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: NativeGUICaller.h - Platform independent singleton GUI caller class implementation
// author: Karl-Mihkel Ott

#define NATIVE_GUI_CALLER_CPP
#include <NativeGUICaller.h>

namespace DENG {

	// Windows specific gui calls (using WIN32 api)
#ifdef _WIN32
	std::string NativeGUICaller::FilePicker(Window &_win) {
		char *filter = ".DAS";
		char buf[256] = { 0 };
		OPENFILENAMEA open_file_name = {};
		open_file_name.lStructSize = sizeof(OPENFILENAMEA);
		open_file_name.hwndOwner = _win.GetWin32Handle();
		open_file_name.lpstrFile = buf;
		open_file_name.nMaxFile = sizeof(buf);
		open_file_name.lpstrFilter = filter;
		open_file_name.lpstrTitle = "ViewModelsApp - Open model";
		open_file_name.Flags = OFN_CREATEPROMPT | OFN_EXPLORER | OFN_FILEMUSTEXIST;
		BOOL stat = GetOpenFileNameA(&open_file_name);

		if(stat && open_file_name.lpstrFile) return std::string(open_file_name.lpstrFile);
		else return std::string();
	}
#endif
}