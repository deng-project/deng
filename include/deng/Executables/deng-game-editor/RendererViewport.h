// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: RendererViewport.h - Editor viewport class header
// author: Karl-Mihkel Ott

#ifndef RENDERER_VIEWPORT_H
#define RENDERER_VIEWPORT_H

#ifdef RENDERER_VIEWPORT_CPP
	#include <any>
	#include <wx/wx.h>
	#include <wx/clrpicker.h>
	#include <wx/valnum.h>
	#include <wx/collpane.h>
	#include <wx/treectrl.h>
	#include <wx/aui/aui.h>

	#include "deng/deng.h"
	#include "deng/Executables/deng-game-editor/OpenGLLoader.h"
	#include "deng/Executables/deng-game-editor/OpenGLLoaderWin32.h"
	#include "deng/Executables/deng-game-editor/GameEditor.h"
#endif

namespace DENG {
	namespace Editor {
		
		class RendererViewport : public wxWindow {
		private:
			DXML::Configuration::Backend m_backend;
			RendererConfig m_config;
			InputRegistry m_input;
			Renderer* mp_renderer = nullptr;
			OpenGLLoader* mp_opengl_loader = nullptr;
#ifdef _WIN32
			HDC m_dc = nullptr;
#endif

			std::chrono::time_point<std::chrono::high_resolution_clock> m_beg_time = std::chrono::high_resolution_clock::now();
			std::chrono::time_point<std::chrono::high_resolution_clock> m_end_time = std::chrono::high_resolution_clock::now();

			struct {
				bool enabled = false;
				TRS::Vector2<int64_t> center;
				TRS::Vector2<int64_t> delta;
			} m_virtual;
			TRS::Vector2<int64_t> m_cursor_pos;

			private:
				void _OnPaint(wxPaintEvent& _ev);
				void _OnResize(wxSizeEvent &_ev);
				void _OnIdle(wxIdleEvent& _ev);
				void _OnKeyDown(wxKeyEvent& _ev);
				void _OnKeyUp(wxKeyEvent& _ev);

				void _OnMouseLeftBtn(wxMouseEvent& _ev);
				void _OnMouseMiddleBtn(wxMouseEvent& _ev);
				void _OnMouseRightBtn(wxMouseEvent& _ev);
				void _OnMouseMotion(wxMouseEvent& _ev);
				void _OnMouseWheel(wxMouseEvent& _ev);
				
				void _SwapBuffers();

			public:
				RendererViewport(wxWindow *_parent, DXML::Configuration::Backend _backend);
				~RendererViewport();

				void Setup();
				inline void SetVirtualCursor() {
					m_virtual.enabled = true;
					m_virtual.center = { GetSize().x >> 1, GetSize().y >> 1 };
					WarpPointer(static_cast<int>(m_virtual.center.first), static_cast<int>(m_virtual.center.second));
				}

				inline void UnsetVirtualCursor() {
					m_virtual.enabled = false;
				}

				inline TRS::Vector2<int64_t> GetVirtualCursorDelta() {
					return m_virtual.delta;
				}

				inline Renderer* GetRenderer() {
					return mp_renderer;
				}

				inline InputRegistry& GetInputRegistry() {
					return m_input;
				}

				inline DXML::Configuration::Backend GetBackend() {
					return m_backend;
				}
		};
	}
}

#endif