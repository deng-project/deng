// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: RendererViewport.h - Editor viewport class header
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
			Bind(wxEVT_SIZE, &RendererViewport::_OnResize, this, wxID_ANY);
			Bind(wxEVT_IDLE, &RendererViewport::_OnIdle, this, wxID_ANY);
		}


		RendererViewport::~RendererViewport() {
			delete mp_renderer;
		}


		void RendererViewport::Setup() {
			wxSize size = GetSize();
			m_config.canvas_size = {
				static_cast<uint32_t>(size.GetWidth()),
				static_cast<uint32_t>(size.GetHeight())
			};
			m_config.title = "DENG game editor viewport";
			switch (m_backend) {
				case DXML::Configuration::Backend::VULKAN:
#ifdef _WIN32
					m_config.win32_hwnd = GetHWND();
					m_config.win32_instance = GetModuleHandle(NULL);
#else
#error "Consider implementing DENG::RendererConfig::xlib_dpy and DENG::RendererConfig::xlib_win values"
#endif
					mp_renderer = new VulkanRenderer(m_config);
					break;
				
				case DXML::Configuration::Backend::OPENGL:
					// mp_renderer = new OpenGLRenderer(m_config);
					break;

				default:
					DENG_ASSERT(false);
					break;
			}

			Refresh();
			Update();
		}


		void RendererViewport::_OnPaint(wxPaintEvent &_ev) {
			std::cout << "_OnPaint() called" << std::endl;
			if (mp_renderer) {
				m_active_time = std::chrono::high_resolution_clock::now();
				std::chrono::duration<float, std::milli> duration = m_active_time - m_beg_time;
				std::cout << "FPS: " << 1000.f / duration.count() << std::endl;
				mp_renderer->RenderFrame();
			}
			_ev.Skip();
			m_beg_time = std::chrono::high_resolution_clock::now();
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
				m_active_time = std::chrono::high_resolution_clock::now();
				std::chrono::duration<float, std::milli> duration = m_active_time - m_beg_time;
				std::cout << "FPS: " << 1000.f / duration.count() << std::endl;
				mp_renderer->RenderFrame();
				_ev.RequestMore();
			}
			m_beg_time = std::chrono::high_resolution_clock::now();
		}
	}
}