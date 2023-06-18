// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SDLWindowContext.h - SDL window context class header
// author: Karl-Mihkel Ott

#ifndef SDL_WINDOW_CONTEXT_H
#define SDL_WINDOW_CONTEXT_H

#include <array>
#include <SDL2/SDL.h>

#include "deng/Api.h"
#include "deng/IWindowContext.h"
#include "deng/InputEvents.h"

#ifdef SDL_WINDOW_CONTEXT_CPP
	#include <cstring>
	#include <SDL2/SDL_vulkan.h>
	#include "deng/WindowEvents.h"
	#include "deng/Exceptions.h"
	#include "deng/ErrorDefinitions.h"
#endif


namespace DENG {

	class DENG_API SDLWindowContext : public IWindowContext {
		private:
			static std::array<MouseButton, 5> ms_MouseButtonEventLookup;

			SDL_Event m_event;
			SDL_Window* m_pWindow = nullptr;
			SDL_Surface* m_pScreenSurface = nullptr;

		private:
			Keycode _TranslateKeycode(SDL_Scancode _eCode);
			KeyModifier _TranslateKeyboardModifier(uint16_t _uMod);
			MouseButtonModifier _TranslateButtonModifier(uint32_t _uMod);
			void _TranslateAndDispatchWindowEvent();

		public:
			SDLWindowContext();
			~SDLWindowContext();

			virtual std::vector<const char*> QueryRequiredVulkanExtensions() override;
			virtual void SetTextMode(bool _bEnable) override;
			virtual void Create(const std::string& _sTitle, uint32_t _uWidth, uint32_t _uHeight) override;
			virtual void* CreateVulkanSurface(void* _pInstance) override;
			virtual void Update() override;
	};

}

#endif