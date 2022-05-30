// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: NativeGUICaller.h - Platform independent singleton GUI caller class header
// author: Karl-Mihkel Ott

#ifndef NATIVE_GUI_CALLER_H
#define NATIVE_GUI_CALLER_H

#ifdef NATIVE_GUI_CALLER_CPP
	#include <string>
	#ifdef _WIN32
		#include <Windows.h>
	#endif

	#include <Api.h>
	#include <libdas/include/Points.h>
	#include <Window.h>	
#endif

namespace DENG {

	struct DENG_API NativeGUICaller {
		static std::string FilePicker(Window &_win);
	};
}

#endif