// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SDLWindowContext.cpp - SDL window context class header
// author: Karl-Mihkel Ott

#define SDL_WINDOW_CONTEXT_CPP
#include "deng/SDLWindowContext.h"

#define MOUSE_BUTTON_LOOKUP(x) (ms_MouseButtonEventLookup[x - SDL_BUTTON_LEFT])


namespace DENG {

	std::array<MouseEvent, 5> SDLWindowContext::ms_MouseButtonEventLookup = {
		MouseEvent::BTN_1,
		MouseEvent::BTN_2,
		MouseEvent::BTN_3,
		MouseEvent::BTN_X1,
		MouseEvent::BTN_X2
	};

	SDLWindowContext::SDLWindowContext() {
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			const std::string sMessage = "SDL_Init() video initialization failed: " + std::string(SDL_GetError());
			throw WindowContextException(sMessage);
		}
	}

	SDLWindowContext::~SDLWindowContext() {
		SDL_DestroyWindow(m_pWindow);
		SDL_Quit();
	}


	// evil keycode translation
	KeySymbol SDLWindowContext::_TranslateKeyCode(SDL_Scancode _eCode) {
		switch (_eCode) {
		case SDL_SCANCODE_SPACE:
			return KeySymbol::KEY_SPACE;

		case SDL_SCANCODE_APOSTROPHE:
			return KeySymbol::KEY_APOSTROPHE;

		case SDL_SCANCODE_COMMA:
			return KeySymbol::KEY_COMMA;

		case SDL_SCANCODE_MINUS:
			return KeySymbol::KEY_MINUS;

		case SDL_SCANCODE_PERIOD:
			return KeySymbol::KEY_DOT;

		case SDL_SCANCODE_SLASH:
			return KeySymbol::KEY_SLASH;

		case SDL_SCANCODE_0:
			return KeySymbol::KEY_0;

		case SDL_SCANCODE_1:
			return KeySymbol::KEY_1;

		case SDL_SCANCODE_2:
			return KeySymbol::KEY_2;

		case SDL_SCANCODE_3:
			return KeySymbol::KEY_3;

		case SDL_SCANCODE_4:
			return KeySymbol::KEY_4;

		case SDL_SCANCODE_5:
			return KeySymbol::KEY_5;

		case SDL_SCANCODE_6:
			return KeySymbol::KEY_6;

		case SDL_SCANCODE_7:
			return KeySymbol::KEY_7;

		case SDL_SCANCODE_8:
			return KeySymbol::KEY_8;

		case SDL_SCANCODE_9:
			return KeySymbol::KEY_9;

		case SDL_SCANCODE_SEMICOLON:
			return KeySymbol::KEY_SEMICOLON;

		case SDL_SCANCODE_EQUALS:
			return KeySymbol::KEY_EQUAL;

		case SDL_SCANCODE_A:
			return KeySymbol::KEY_A;

		case SDL_SCANCODE_B:
			return KeySymbol::KEY_B;

		case SDL_SCANCODE_C:
			return KeySymbol::KEY_C;

		case SDL_SCANCODE_D:
			return KeySymbol::KEY_D;

		case SDL_SCANCODE_E:
			return KeySymbol::KEY_E;

		case SDL_SCANCODE_F:
			return KeySymbol::KEY_F;

		case SDL_SCANCODE_G:
			return KeySymbol::KEY_G;

		case SDL_SCANCODE_H:
			return KeySymbol::KEY_H;

		case SDL_SCANCODE_I:
			return KeySymbol::KEY_I;

		case SDL_SCANCODE_J:
			return KeySymbol::KEY_J;

		case SDL_SCANCODE_K:
			return KeySymbol::KEY_K;

		case SDL_SCANCODE_L:
			return KeySymbol::KEY_L;

		case SDL_SCANCODE_M:
			return KeySymbol::KEY_M;

		case SDL_SCANCODE_N:
			return KeySymbol::KEY_N;

		case SDL_SCANCODE_O:
			return KeySymbol::KEY_O;

		case SDL_SCANCODE_P:
			return KeySymbol::KEY_P;

		case SDL_SCANCODE_Q:
			return KeySymbol::KEY_Q;

		case SDL_SCANCODE_R:
			return KeySymbol::KEY_R;

		case SDL_SCANCODE_S:
			return KeySymbol::KEY_S;

		case SDL_SCANCODE_T:
			return KeySymbol::KEY_T;

		case SDL_SCANCODE_U:
			return KeySymbol::KEY_U;

		case SDL_SCANCODE_V:
			return KeySymbol::KEY_V;

		case SDL_SCANCODE_W:
			return KeySymbol::KEY_W;

		case SDL_SCANCODE_X:
			return KeySymbol::KEY_X;

		case SDL_SCANCODE_Y:
			return KeySymbol::KEY_Y;

		case SDL_SCANCODE_Z:
			return KeySymbol::KEY_Z;

		case SDL_SCANCODE_LEFTBRACKET:
			return KeySymbol::KEY_LEFT_BRACKET;

		case SDL_SCANCODE_RIGHTBRACKET:
			return KeySymbol::KEY_RIGHT_BRACKET;

		case SDL_SCANCODE_BACKSLASH:
			return KeySymbol::KEY_BACKSLASH;

		case SDL_SCANCODE_GRAVE:
			return KeySymbol::KEY_GRAVE_ACCENT;

		case SDL_SCANCODE_ESCAPE:
			return KeySymbol::KEY_ESCAPE;

		case SDL_SCANCODE_RETURN:
			return KeySymbol::KEY_ENTER;

		case SDL_SCANCODE_TAB:
			return KeySymbol::KEY_TAB;

		case SDL_SCANCODE_BACKSPACE:
			return KeySymbol::KEY_BACKSPACE;

		case SDL_SCANCODE_INSERT:
			return KeySymbol::KEY_INSERT;

		case SDL_SCANCODE_DELETE:
			return KeySymbol::KEY_DELETE;

		case SDL_SCANCODE_RIGHT:
			return KeySymbol::KEY_RIGHT;

		case SDL_SCANCODE_LEFT:
			return KeySymbol::KEY_LEFT;

		case SDL_SCANCODE_DOWN:
			return KeySymbol::KEY_DOWN;

		case SDL_SCANCODE_UP:
			return KeySymbol::KEY_UP;

		case SDL_SCANCODE_PAGEUP:
			return KeySymbol::KEY_PAGE_UP;

		case SDL_SCANCODE_PAGEDOWN:
			return KeySymbol::KEY_PAGE_DOWN;

		case SDL_SCANCODE_HOME:
			return KeySymbol::KEY_HOME;

		case SDL_SCANCODE_END:
			return KeySymbol::KEY_END;

		case SDL_SCANCODE_CAPSLOCK:
			return KeySymbol::KEY_CAPS_LOCK;

		case SDL_SCANCODE_SCROLLLOCK:
			return KeySymbol::KEY_SCROLL_LOCK;

		case SDL_SCANCODE_NUMLOCKCLEAR:
			return KeySymbol::KEY_NUM_LOCK;

		case SDL_SCANCODE_PRINTSCREEN:
			return KeySymbol::KEY_PRINT_SCREEN;

		case SDL_SCANCODE_PAUSE:
			return KeySymbol::KEY_PAUSE;

		case SDL_SCANCODE_F1:
			return KeySymbol::KEY_F1;

		case SDL_SCANCODE_F2:
			return KeySymbol::KEY_F2;

		case SDL_SCANCODE_F3:
			return KeySymbol::KEY_F3;

		case SDL_SCANCODE_F4:
			return KeySymbol::KEY_F4;

		case SDL_SCANCODE_F5:
			return KeySymbol::KEY_F5;

		case SDL_SCANCODE_F6:
			return KeySymbol::KEY_F6;

		case SDL_SCANCODE_F7:
			return KeySymbol::KEY_F7;

		case SDL_SCANCODE_F8:
			return KeySymbol::KEY_F8;

		case SDL_SCANCODE_F9:
			return KeySymbol::KEY_F9;

		case SDL_SCANCODE_F10:
			return KeySymbol::KEY_F10;

		case SDL_SCANCODE_F11:
			return KeySymbol::KEY_F11;

		case SDL_SCANCODE_F12:
			return KeySymbol::KEY_F12;

		case SDL_SCANCODE_F13:
			return KeySymbol::KEY_F13;

		case SDL_SCANCODE_F14:
			return KeySymbol::KEY_F14;

		case SDL_SCANCODE_F15:
			return KeySymbol::KEY_F15;

		case SDL_SCANCODE_F16:
			return KeySymbol::KEY_F16;

		case SDL_SCANCODE_F17:
			return KeySymbol::KEY_F17;

		case SDL_SCANCODE_F18:
			return KeySymbol::KEY_F18;

		case SDL_SCANCODE_F19:
			return KeySymbol::KEY_F19;

		case SDL_SCANCODE_F20:
			return KeySymbol::KEY_F20;

		case SDL_SCANCODE_F21:
			return KeySymbol::KEY_F21;

		case SDL_SCANCODE_F22:
			return KeySymbol::KEY_F22;

		case SDL_SCANCODE_F23:
			return KeySymbol::KEY_F23;

		case SDL_SCANCODE_F24:
			return KeySymbol::KEY_F24;

		case SDL_SCANCODE_KP_0:
			return KeySymbol::KEY_NP_0;

		case SDL_SCANCODE_KP_1:
			return KeySymbol::KEY_NP_1;

		case SDL_SCANCODE_KP_2:
			return KeySymbol::KEY_NP_2;

		case SDL_SCANCODE_KP_3:
			return KeySymbol::KEY_NP_3;

		case SDL_SCANCODE_KP_4:
			return KeySymbol::KEY_NP_4;

		case SDL_SCANCODE_KP_5:
			return KeySymbol::KEY_NP_5;

		case SDL_SCANCODE_KP_6:
			return KeySymbol::KEY_NP_6;

		case SDL_SCANCODE_KP_7:
			return KeySymbol::KEY_NP_7;

		case SDL_SCANCODE_KP_8:
			return KeySymbol::KEY_NP_8;

		case SDL_SCANCODE_KP_9:
			return KeySymbol::KEY_NP_9;

		case SDL_SCANCODE_KP_DECIMAL:
			return KeySymbol::KEY_NP_DECIMAL;

		case SDL_SCANCODE_KP_DIVIDE:
			return KeySymbol::KEY_NP_DIVIDE;

		case SDL_SCANCODE_KP_MULTIPLY:
			return KeySymbol::KEY_NP_MULTIPLY;

		case SDL_SCANCODE_KP_MINUS:
			return KeySymbol::KEY_NP_SUBTRACT;

		case SDL_SCANCODE_KP_PLUS:
			return KeySymbol::KEY_NP_ADD;

		case SDL_SCANCODE_KP_ENTER:
			return KeySymbol::KEY_NP_ENTER;

		case SDL_SCANCODE_KP_EQUALS:
			return KeySymbol::KEY_NP_EQUAL;

		case SDL_SCANCODE_LSHIFT:
			return KeySymbol::KEY_LEFT_SHIFT;

		case SDL_SCANCODE_LCTRL:
			return KeySymbol::KEY_LEFT_CTRL;

		case SDL_SCANCODE_LALT:
			return KeySymbol::KEY_LEFT_ALT;

		case SDL_SCANCODE_LGUI:
			return KeySymbol::KEY_LEFT_SUPER;

		case SDL_SCANCODE_RSHIFT:
			return KeySymbol::KEY_RIGHT_SHIFT;

		case SDL_SCANCODE_RCTRL:
			return KeySymbol::KEY_RIGHT_CTRL;

		case SDL_SCANCODE_RALT:
			return KeySymbol::KEY_RIGHT_ALT;

		case SDL_SCANCODE_RGUI:
			return KeySymbol::KEY_RIGHT_SUPER;

		case SDL_SCANCODE_MENU:
			return KeySymbol::KEY_MENU;

		default:
			return KeySymbol::KEY_UNKNOWN;
		}
	}

	int32_t SDLWindowContext::_TranslateKeyboardModifier(uint16_t _uMod) {
		int32_t uKeymod = 0;
		
		if (_uMod & KMOD_LSHIFT)
			uKeymod |= KeyModifier::MOD_LSHIFT;
		if (_uMod & KMOD_RSHIFT)
			uKeymod |= KeyModifier::MOD_RSHIFT;
		if (_uMod & KMOD_LCTRL)
			uKeymod |= KeyModifier::MOD_LCTRL;
		if (_uMod & KMOD_RCTRL)
			uKeymod |= KeyModifier::MOD_RCTRL;
		if (_uMod & KMOD_LALT)
			uKeymod |= KeyModifier::MOD_LALT;
		if (_uMod & KMOD_RALT)
			uKeymod |= KeyModifier::MOD_RALT;
		if (_uMod & KMOD_LGUI)
			uKeymod |= KeyModifier::MOD_LSUPER;
		if (_uMod & KMOD_RGUI)
			uKeymod |= KeyModifier::MOD_RSUPER;
		if (_uMod & KMOD_NUM)
			uKeymod |= KeyModifier::MOD_NUM;
		if (_uMod & KMOD_CAPS)
			uKeymod |= KeyModifier::MOD_CAPS;
		if (_uMod & KMOD_MODE)
			uKeymod |= KeyModifier::MOD_ALTGR;
		if (_uMod & KMOD_SCROLL)
			uKeymod |= KeyModifier::MOD_SCROLL;

		return uKeymod;
	}

	int32_t SDLWindowContext::_TranslateButtonModifier(uint32_t _uMod) {
		int32_t uButtonMod = 0;

		if (_uMod & SDL_BUTTON_LMASK)
			uButtonMod |= ButtonModifier::MOD_LCLICK;
		if (_uMod & SDL_BUTTON_MMASK)
			uButtonMod |= ButtonModifier::MOD_MCLICK;
		if (_uMod & SDL_BUTTON_RMASK)
			uButtonMod |= ButtonModifier::MOD_RCLICK;
		if (_uMod & SDL_BUTTON_X1MASK)
			uButtonMod |= ButtonModifier::MOD_X1CLICK;
		if (_uMod & SDL_BUTTON_X2MASK)
			uButtonMod |= ButtonModifier::MOD_X2CLICK;

		return 0;
	}


	WindowEvent SDLWindowContext::_TranslateWindowEvent(uint32_t _uWinEvent) {
		switch (_uWinEvent) {
			case SDL_WINDOWEVENT_SHOWN: return WindowEvent::SHOWN;
			case SDL_WINDOWEVENT_HIDDEN: return WindowEvent::HIDDEN;
			case SDL_WINDOWEVENT_EXPOSED: return WindowEvent::EXPOSED;
			case SDL_WINDOWEVENT_RESIZED: return WindowEvent::RESIZED;
			case SDL_WINDOWEVENT_SIZE_CHANGED: return WindowEvent::SIZE_CHANGED;
			case SDL_WINDOWEVENT_MINIMIZED: return WindowEvent::MINIMIZED;
			case SDL_WINDOWEVENT_MAXIMIZED: return WindowEvent::MAXIMIZED;
			case SDL_WINDOWEVENT_RESTORED: return WindowEvent::RESTORED;
			case SDL_WINDOWEVENT_ENTER: return WindowEvent::GAINED_MOUSE_FOCUS;
			case SDL_WINDOWEVENT_LEAVE: return WindowEvent::LOST_MOUSE_FOCUS;
			case SDL_WINDOWEVENT_FOCUS_GAINED: return WindowEvent::GAINED_KEYBOARD_FOCUS;
			case SDL_WINDOWEVENT_FOCUS_LOST: return WindowEvent::LOST_KEYBOARD_FOCUS;
			case SDL_WINDOWEVENT_DISPLAY_CHANGED: return WindowEvent::DISPLAY_CHANGED;
			default: return WindowEvent::UNKNOWN;
		}
	}


	std::vector<const char*> SDLWindowContext::QueryRequiredVulkanExtensions() {
		unsigned int uCount;
		SDL_Vulkan_GetInstanceExtensions(m_pWindow, &uCount, nullptr);
		std::vector<const char*> extensions(uCount);
		SDL_Vulkan_GetInstanceExtensions(m_pWindow, &uCount, extensions.data());
		return extensions;
	}


	void SDLWindowContext::SetTextMode(bool _bEnable) {
		if (_bEnable) {
			SDL_StartTextInput();
		}
		else {
			SDL_StopTextInput();
		}
	}


	void SDLWindowContext::Create(const std::string& _sTitle, uint32_t _uWidth, uint32_t _uHeight) {
		m_sTitle = _sTitle;
		m_uWidth = _uWidth;
		m_uHeight = _uHeight;


		Uint32 uFlags = 0;
		if (m_uHints & WindowHints::FULL_SCREEN)
			uFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		if (m_uHints & WindowHints::OPENGL)
			uFlags |= SDL_WINDOW_OPENGL;
		if (m_uHints & WindowHints::VULKAN)
			uFlags |= SDL_WINDOW_VULKAN;
		if (m_uHints & WindowHints::SHOWN)
			uFlags |= SDL_WINDOW_SHOWN;
		if (m_uHints & WindowHints::HIDDEN)
			uFlags |= SDL_WINDOW_HIDDEN;
		if (m_uHints & WindowHints::BORDERLESS)
			uFlags |= SDL_WINDOW_BORDERLESS;
		if (m_uHints & WindowHints::RESIZEABLE)
			uFlags |= SDL_WINDOW_RESIZABLE;
		if (m_uHints & WindowHints::MINIMIZED)
			uFlags |= SDL_WINDOW_MINIMIZED;
		if (m_uHints & WindowHints::MAXIMIZED)
			uFlags |= SDL_WINDOW_MAXIMIZED;
		if (m_uHints & WindowHints::INPUT_FOCUS)
			uFlags |= SDL_WINDOW_INPUT_FOCUS;
		if (m_uHints & WindowHints::MOUSE_FOCUS)
			uFlags |= SDL_WINDOW_MOUSE_FOCUS;

		m_pWindow = SDL_CreateWindow(
			m_sTitle.c_str(),
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			static_cast<int>(m_uWidth),
			static_cast<int>(m_uHeight),
			uFlags);

		if (!m_pWindow) {
			throw WindowContextException("SDL_CreateWindow() failed to create a new window: " + std::string(SDL_GetError()));
		}

		m_bIsAlive = true;
		m_pScreenSurface = SDL_GetWindowSurface(m_pWindow);
	}

	void* SDLWindowContext::CreateVulkanSurface(void* _pInstance) {
		DENG_ASSERT(m_pWindow);
		DENG_ASSERT(m_uHints & WindowHints::VULKAN);

		VkSurfaceKHR hSurface = nullptr;
		SDL_Vulkan_CreateSurface(m_pWindow, (VkInstance)_pInstance, &hSurface);
		return (void*)hSurface;
	}

	void SDLWindowContext::Update() {
		DENG_ASSERT(m_pWindow);
		//SDL_UpdateWindowSurface(m_pWindow);

		// poll events that might've occured
		while (SDL_PollEvent(&m_event) != 0) {
			switch (m_event.type) {
				case SDL_QUIT:
					m_bIsAlive = false;
					return;

				case SDL_WINDOWEVENT:
					m_eventQueue.push({
						{},
						EventType::WINDOW,
						static_cast<int32_t>(_TranslateWindowEvent(m_event.window.type)),
						0,
						m_event.window.data1,
						m_event.window.data2,
						0, 0
						});
					break;

				case SDL_KEYDOWN:
					m_eventQueue.push({
						{},
						EventType::KEY_DOWN,
						static_cast<int32_t>(_TranslateKeyCode(m_event.key.keysym.scancode)),
						static_cast<int32_t>(_TranslateKeyboardModifier(m_event.key.keysym.mod)),
						0, 0, 0, 0
						});
					break;

				case SDL_KEYUP:
					m_eventQueue.push({
						{},
						EventType::KEY_UP,
						static_cast<int32_t>(_TranslateKeyCode(m_event.key.keysym.scancode)),
						static_cast<int32_t>(_TranslateKeyboardModifier(m_event.key.keysym.mod)),
						0, 0, 0, 0
						});
					break;

				case SDL_MOUSEBUTTONDOWN:
					m_eventQueue.push({
						{},
						EventType::MOUSE_BUTTON_DOWN,
						static_cast<int32_t>(MOUSE_BUTTON_LOOKUP(m_event.button.button)),
						0, 0, 0,
						m_event.button.x,
						m_event.button.y
						});
					break;

				case SDL_MOUSEBUTTONUP:
					m_eventQueue.push({
						{},
						EventType::MOUSE_BUTTON_UP,
						static_cast<int32_t>(MOUSE_BUTTON_LOOKUP(m_event.button.button)),
						0, 0, 0,
						m_event.button.x,
						m_event.button.y
						});
					break;

				case SDL_MOUSEWHEEL:
					m_eventQueue.push({
						{},
						EventType::MOUSE_WHEEL,
						0, 0,
						0, 0,
						m_event.wheel.x,
						m_event.wheel.y
						});
					break;

				case SDL_MOUSEMOTION:
					m_eventQueue.push({
						{},
						EventType::MOUSE_MOTION,
						0, _TranslateButtonModifier(m_event.motion.state),
						m_event.motion.x,
						m_event.motion.y,
						m_event.motion.xrel,
						m_event.motion.yrel
						});
					break;

				case SDL_TEXTINPUT:
					m_eventQueue.push({
						{},
						EventType::TEXT_INPUT,
						0, 0,
						0, 0, 0, 0
						});

					std::strcat(m_eventQueue.back().szEditedText, m_event.text.text);
					break;

				case SDL_TEXTEDITING:
					m_eventQueue.push({
						{},
						EventType::TEXT_EDITING,
						m_event.edit.start,
						m_event.edit.length,
						0, 0, 0, 0
						});
					break;

				default:
					break;
			}
		}
	}
}

