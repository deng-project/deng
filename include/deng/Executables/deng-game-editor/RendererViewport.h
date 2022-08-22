// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: RendererViewport.h - Editor viewport class header
// author: Karl-Mihkel Ott

#ifndef RENDERER_VIEWPORT_H
#define RENDERER_VIEWPORT_H

#ifdef RENDERER_VIEWPORT_CPP
	#include <string>
	#include <vector>
	#include <array>
	#include <variant>
	#include <chrono>
	#include <unordered_map>

	#include <wx/wx.h>
	#include <vulkan/vulkan.h>
	
	// #include "nwin/glad/glad.h"
	#include "trs/Vector.h"
	#include "trs/Points.h"
	#include "trs/Quaternion.h"	
	#include "dxml/GameStructs.h"

	#include "deng/Api.h"
	#include "deng/ProjectDataManager.h"
	#include "deng/ErrorDefinitions.h"
	#include "deng/ShaderDefinitions.h"
	#include "deng/Renderer.h"
	#include "deng/VulkanRenderer.h"
	// #include "deng/OpenGLRenderer.h"
#endif

namespace DENG {
	namespace Editor {
		
		class RendererViewport : public wxWindow {
			private:
				DXML::Configuration::Backend m_backend;
				RendererConfig m_config;
				Renderer *mp_renderer = nullptr;
				std::chrono::time_point<std::chrono::high_resolution_clock> m_beg_time = std::chrono::high_resolution_clock::now();
				std::chrono::time_point<std::chrono::high_resolution_clock> m_active_time = std::chrono::high_resolution_clock::now();

			private:
				void _OnPaint(wxPaintEvent& _ev);
				void _OnResize(wxSizeEvent &_ev);
				void _OnIdle(wxIdleEvent& _ev);

			public:
				RendererViewport(wxWindow *_parent, DXML::Configuration::Backend _backend);
				void Setup();
				~RendererViewport();

				inline Renderer* GetRenderer() {
					return mp_renderer;
				}
		};
	}
}

#endif