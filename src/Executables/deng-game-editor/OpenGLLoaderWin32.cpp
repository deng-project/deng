// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLLoaderWin32.cpp - Win32 specific OpenGL loader class implementation
// author: Karl-Mihkel Ott

#define OPENGL_LOADER_WIN32_CPP
#include "deng/Executables/deng-game-editor/OpenGLLoaderWin32.h"

namespace DENG {
	namespace Editor {

		void OpenGLLoaderWin32::CreateContext(std::any _hwnd) {
			HINSTANCE libopengl = LoadLibraryW(L"opengl32.dll");
			if (!libopengl) {
				throw std::runtime_error("Could not open opengl32.dll. Please make sure that you have required graphics drivers installed");
			}

			wglCreateContext = (PFN_wglCreateContext)GetProcAddress(libopengl, "wglCreateContext");
			wglDeleteContext = (PFN_wglDeleteContext)GetProcAddress(libopengl, "wglDeleteContext");
			wglMakeCurrent = (PFN_wglMakeCurrent)GetProcAddress(libopengl, "wglMakeCurrent");

			PIXELFORMATDESCRIPTOR pfd = { 0 };
			pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = 32;
			pfd.cDepthBits = 24;
			pfd.cStencilBits = 8;
			pfd.iLayerType = PFD_MAIN_PLANE;

			m_hdc = GetDC(std::any_cast<HWND>(_hwnd));
			int px_format = ChoosePixelFormat(m_hdc, &pfd);
			SetPixelFormat(m_hdc, px_format, &pfd);
			m_context = wglCreateContext(m_hdc);
			if (!m_context) {
				throw std::runtime_error("Failed to create an OpenGL context");
			}
		}


		void OpenGLLoaderWin32::MakeCurrent() {
			wglMakeCurrent(m_hdc, m_context);
		}


		void OpenGLLoaderWin32::DeleteContext() {
			wglDeleteContext(m_context);;
		}
	}
}