// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: IWindowContext.h - window context base class header
// author: Karl-Mihkel Ott

#ifndef IWINDOW_CONTEXT_H
#define IWINDOW_CONTEXT_H

#include <string>
#include <array>
#include <list>
#include <vector>

#include "deng/Api.h"
#include "deng/Event.h"
#define TEXT_BUFFER_LENGTH 32

namespace DENG {

	enum class Cursor {
		Standard,
		Waiting,
		Pointer,
		Hidden
	};

	enum WindowHint_T {
		WindowHint_None = 0,
		WindowHint_FullScreen = 1 << 0,
		WindowHint_OpenGL = 1 << 1,
		WindowHint_Vulkan = 1 << 2,
		WindowHint_DirectX = 1 << 3,
		WindowHint_Shown = 1 << 4,
		WindowHint_Hidden = 1 << 5,
		WindowHint_Borderless = 1 << 6,
		WindowHint_Resizeable = 1 << 7,
		WindowHint_Minimized = 1 << 8,
		WindowHint_Maximized = 1 << 9,
		WindowHint_InputFocus = 1 << 10,
		WindowHint_MouseFocus = 1 << 11
	};

	typedef uint16_t WindowHint;

	class DENG_API IWindowContext {
		protected:
			std::string m_sTitle;
			uint32_t m_uHeight = 0;
			uint32_t m_uWidth = 0;

			WindowHint m_uHints = 0;
			bool m_bIsAlive = false;
			EventManager& m_eventManager;

		public:
			IWindowContext(EventManager& _eventManager) :
				m_eventManager(_eventManager) {}
			
			virtual void Create(const std::string& _sTitle, uint32_t _uWidth, uint32_t _uHeight) = 0;
			virtual void* CreateVulkanSurface(void* _pInstance) = 0;
			
			inline bool IsAlive() {
				return m_bIsAlive;
			}
			
			virtual void Update() = 0;
			
			virtual std::vector<const char*> QueryRequiredVulkanExtensions() {
				return {
#ifdef _DEBUG
					"VK_EXT_debug_utils",
#endif
					"VK_KHR_surface",
#ifdef _WIN32
					"VK_KHR_win32_surface",
#else
					"VK_KHR_xlib_surface",
#endif
				};
			}

			virtual void SetTextMode(bool _bEnable) = 0;
			
			virtual void SetHints(WindowHint _uHints) {
				m_uHints = _uHints;
			}

			virtual void SetHeight(uint32_t _uHeight) {
				m_uHeight = _uHeight;
			}

			virtual void SetWidth(uint32_t _uWidth) {
				m_uWidth = _uWidth;
			}

			virtual void SetTitle(const std::string& _sTitle) {
				m_sTitle = _sTitle;
			}

			inline uint32_t GetHeight() {
				return m_uHeight;
			}

			inline uint32_t GetWidth() {
				return m_uWidth;
			}

			inline const std::string& GetTitle() {
				return m_sTitle;
			}
	};
}

#endif
