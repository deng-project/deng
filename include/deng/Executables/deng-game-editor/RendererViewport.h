// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: RendererViewport.h - Editor viewport class header
// author: Karl-Mihkel Ott

#ifndef RENDERER_VIEWPORT_H
#define RENDERER_VIEWPORT_H

#ifdef RENDERER_VIEWPORT_CPP
	#include <any>
	#include <wx/wx.h>

	#include "deng/deng.h"
	#include "deng/Executables/deng-game-editor/OpenGLLoader.h"
	#include "deng/Executables/deng-game-editor/OpenGLLoaderWin32.h"
#endif

namespace DENG {
	namespace Editor {
		
		class RendererViewport : public wxWindow {
			private:
				DXML::Configuration::Backend m_backend;
				RendererConfig m_config;
				Renderer *mp_renderer = nullptr;
				OpenGLLoader* mp_opengl_loader = nullptr;
				std::chrono::time_point<std::chrono::high_resolution_clock> m_beg_time = std::chrono::high_resolution_clock::now();
				std::chrono::time_point<std::chrono::high_resolution_clock> m_active_time = std::chrono::high_resolution_clock::now();

			private:
				void _OnPaint(wxPaintEvent& _ev);
				void _OnResize(wxSizeEvent &_ev);
				void _OnIdle(wxIdleEvent& _ev);
				void _SwapBuffers();

			public:
				RendererViewport(wxWindow *_parent, DXML::Configuration::Backend _backend);
				~RendererViewport();

				void Setup();

				inline Renderer* GetRenderer() {
					return mp_renderer;
				}
		};
	}
}

#endif