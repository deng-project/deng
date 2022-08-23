// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLLoaderWin32.h - Win32 specific OpenGL loader class header
// author: Karl-Mihkel Ott

#ifndef OPENGL_LOADER_WIN32_H
#define OPENGL_LOADER_WIN32_H

#ifdef OPENGL_LOADER_WIN32_CPP
	#include <any>
	#include <string>
	#include <cstdint>
	#include <vulkan/vulkan.h>
	#include "trs/Points.h"
	// #include "trs/Vector.h"

	#include "deng/Api.h"
	#include "deng/BaseTypes.h"
	#include "deng/ErrorDefinitions.h"
	#include "deng/Window.h"

	#include "deng/Executables/deng-game-editor/OpenGLLoader.h"
#endif

namespace DENG {
	namespace Editor {

		typedef HGLRC(WINAPI* PFN_wglCreateContext)(HDC);
		typedef BOOL(WINAPI* PFN_wglDeleteContext)(HGLRC);
		typedef BOOL(WINAPI* PFN_wglMakeCurrent)(HDC, HGLRC);

		class OpenGLLoaderWin32 : public OpenGLLoader {
			private:
				PFN_wglCreateContext wglCreateContext = nullptr;
				PFN_wglDeleteContext wglDeleteContext = nullptr;
				PFN_wglMakeCurrent wglMakeCurrent = nullptr;

				HDC m_hdc = nullptr;
				HGLRC m_context = nullptr;

			public:
				virtual void CreateContext(std::any _hwnd) override;
				virtual void MakeCurrent() override;
				virtual void DeleteContext() override;

		};
	}
}

#endif