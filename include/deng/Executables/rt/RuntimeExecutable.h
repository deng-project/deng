// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: RuntimeExecutable.h - Main runtime binary header file
// author: Karl-Mihkel Ott

#ifndef RUNTIME_EXECUTABLE_H
#define RUNTIME_EXECUTABLE_H

#ifdef RUNTIME_EXECUTABLE_CPP
	#ifdef _WIN32
		// #pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	#endif

	#include "deng/deng.h"
#endif

namespace DENG {
	namespace Runtime {

		class GameRuntime {
			private:
				DXML::GameConfigurationReader m_conf_reader;
				Window* m_window = nullptr;
				Renderer* m_rend = nullptr;

			public:
				GameRuntime();
				~GameRuntime();

				void Run();
		};

	}
}

#endif