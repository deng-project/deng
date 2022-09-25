// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: RendererViewport.cpp - Editor viewport class implementation
// author: Karl-Mihkel Ott

#define RENDERER_VIEWPORT_CPP
#include "deng/Executables/deng-game-editor/RendererViewport.h"

#define WIDTH	640
#define HEIGHT	480

namespace DENG {
	namespace Editor {

		RendererViewport::RendererViewport(wxWindow *_parent, DXML::Configuration::Backend _backend) :
			wxWindow(_parent, wxID_ANY),
			m_backend(_backend)
		{
			Bind(wxEVT_PAINT, &RendererViewport::_OnPaint, this, wxID_ANY);
			Bind(wxEVT_IDLE, &RendererViewport::_OnIdle, this, wxID_ANY);
			Bind(wxEVT_SIZE, &RendererViewport::_OnResize, this, wxID_ANY);
			Bind(wxEVT_KEY_DOWN, &RendererViewport::_OnKeyDown, this, wxID_ANY);
			Bind(wxEVT_KEY_UP, &RendererViewport::_OnKeyUp, this, wxID_ANY);

			Bind(wxEVT_LEFT_DOWN, &RendererViewport::_OnMouseLeftBtn, this, wxID_ANY);
			Bind(wxEVT_LEFT_UP, &RendererViewport::_OnMouseLeftBtn, this, wxID_ANY);
			Bind(wxEVT_MIDDLE_DOWN, &RendererViewport::_OnMouseMiddleBtn, this, wxID_ANY);
			Bind(wxEVT_MIDDLE_UP, &RendererViewport::_OnMouseMiddleBtn, this, wxID_ANY);
			Bind(wxEVT_RIGHT_DOWN, &RendererViewport::_OnMouseRightBtn, this, wxID_ANY);
			Bind(wxEVT_RIGHT_UP, &RendererViewport::_OnMouseRightBtn, this, wxID_ANY);
			Bind(wxEVT_MOTION, &RendererViewport::_OnMouseMotion, this, wxID_ANY);
			Bind(wxEVT_MOUSEWHEEL, &RendererViewport::_OnMouseWheel, this, wxID_ANY);
		}


		RendererViewport::~RendererViewport() {
			delete mp_renderer;
			if (mp_opengl_loader) {
				mp_opengl_loader->DeleteContext();
				delete mp_opengl_loader;
			}
		}


		void RendererViewport::Setup() {
			wxSize size = GetSize();
			m_config.canvas_size = {
				static_cast<uint32_t>(size.GetWidth()),
				static_cast<uint32_t>(size.GetHeight())
			};
			m_config.title = "DENG game editor viewport";
			RenderState* rs = RenderState::GetInstance();
			switch (m_backend) {
				case DXML::Configuration::Backend::VULKAN:
#ifdef _WIN32
					m_config.win32_hwnd = GetHWND();
					m_config.win32_instance = GetModuleHandle(NULL);
#else
#error "Consider implementing DENG::RendererConfig::xlib_dpy and DENG::RendererConfig::xlib_win values"
#endif
					m_rend_type = RENDERER_TYPE_VULKAN;
					rs->SetPrimary(RENDERER_TYPE_VULKAN);
					mp_renderer = new VulkanRenderer(m_config);
					break;
				
				case DXML::Configuration::Backend::OPENGL:
					mp_opengl_loader = new OpenGLLoaderWin32;
					mp_opengl_loader->CreateContext(GetHWND());
					mp_opengl_loader->MakeCurrent();
					Window::LoadOpenGLFunctions();
					m_rend_type = RENDERER_TYPE_OPENGL;

					rs->SetPrimary(RENDERER_TYPE_OPENGL);
					mp_renderer = new OpenGLRenderer(m_config);
					break;

				default:
					DENG_ASSERT(false);
					break;
			}
		}


		void RendererViewport::_OnPaint(wxPaintEvent &_ev) {
			if (mp_renderer) {
				Registry* reg = Registry::GetInstance();
				reg->Update();
				m_input.FlushReleased();
				
				if(m_rend_type == RENDERER_TYPE_OPENGL)
					_SwapBuffers();

				mp_renderer->RenderFrame();

				m_end_time = std::chrono::high_resolution_clock::now();
				auto fps = 1000.f / std::chrono::duration_cast<std::chrono::milliseconds>(m_end_time - m_beg_time).count();
				GameEditor* ed = (GameEditor*)GetParent();
				ed->SetStatusText("FPS: " + std::to_string(fps));
				m_beg_time = std::chrono::high_resolution_clock::now();
			}
			_ev.Skip();
		}


		void RendererViewport::_OnResize(wxSizeEvent &_ev) {
			wxSize size = GetSize();
			m_config.canvas_size = {
				static_cast<uint32_t>(size.GetWidth()),
				static_cast<uint32_t>(size.GetHeight())
			};
			_ev.Skip();
		}


		void RendererViewport::_OnIdle(wxIdleEvent& _ev) {
			if (mp_renderer) {
				Registry* reg = Registry::GetInstance();
				reg->Update();
				m_input.FlushReleased();

				if(m_rend_type == RENDERER_TYPE_OPENGL)
					_SwapBuffers();

				mp_renderer->RenderFrame();
				_ev.RequestMore();

				m_end_time = std::chrono::high_resolution_clock::now();
				auto fps = 1000.f / std::chrono::duration_cast<std::chrono::milliseconds>(m_end_time - m_beg_time).count();
				GameEditor *ed = (GameEditor*)GetParent();
				ed->SetStatusText("FPS: " + std::to_string(fps));
				m_beg_time = std::chrono::high_resolution_clock::now();
			}
		}


		// keyboard events
		void RendererViewport::_OnKeyDown(wxKeyEvent& _ev) {
			neko_HidEvent key = Window::TranslateNativeKeyCode(_ev.GetRawKeyCode());
			m_input.RegisterActiveKeyEvent(key);
		}


		void RendererViewport::_OnKeyUp(wxKeyEvent& _ev) {
			neko_HidEvent key = Window::TranslateNativeKeyCode(_ev.GetRawKeyCode());
			m_input.RegisterReleasedKeyEvent(key);
		}


		// mouse events
		void RendererViewport::_OnMouseLeftBtn(wxMouseEvent& _ev) {
			if (_ev.LeftDown())
				m_input.RegisterActiveKeyEvent(NEKO_MOUSE_BTN_1);
			else if (_ev.LeftUp())
				m_input.RegisterReleasedKeyEvent(NEKO_MOUSE_BTN_1);
		}


		void RendererViewport::_OnMouseMiddleBtn(wxMouseEvent& _ev) {
			if (_ev.MiddleDown())
				m_input.RegisterActiveKeyEvent(NEKO_MOUSE_BTN_2);
			else if (_ev.MiddleUp())
				m_input.RegisterReleasedKeyEvent(NEKO_MOUSE_BTN_2);
		}


		void RendererViewport::_OnMouseRightBtn(wxMouseEvent& _ev) {
			if (_ev.RightDown())
				m_input.RegisterActiveKeyEvent(NEKO_MOUSE_BTN_3);
			else if (_ev.RightUp())
				m_input.RegisterReleasedKeyEvent(NEKO_MOUSE_BTN_3);
		}


		void RendererViewport::_OnMouseMotion(wxMouseEvent& _ev) {
			TRS::Vector2<int64_t> pos = {
				static_cast<int64_t>(_ev.GetPosition().x),
				static_cast<int64_t>(_ev.GetPosition().y)
			};

			if (m_virtual.enabled) {
				m_virtual.center = {
					static_cast<int64_t>(GetSize().x >> 1),
					static_cast<int64_t>(GetSize().y >> 1)
				};
				m_virtual.delta = pos - m_virtual.center;
			} else {
				m_cursor_pos = pos;
			}
		}


		void RendererViewport::_OnMouseWheel(wxMouseEvent &_ev) {
			if (_ev.GetWheelRotation() > 0) {
				m_input.RegisterActiveKeyEvent(NEKO_MOUSE_SCROLL_UP);
			} else if (_ev.GetWheelRotation() < 0) {
				m_input.RegisterActiveKeyEvent(NEKO_MOUSE_SCROLL_DOWN);
			}
		}

		void RendererViewport::_SwapBuffers() {
#ifdef _WIN32
			SwapBuffers(GetDC(GetHWND()));
#endif
		}
	}
}