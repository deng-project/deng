// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SDLWindowContext.cpp - SDL window context class header
// author: Karl-Mihkel Ott

#define SDL_WINDOW_CONTEXT_CPP
#include "deng/SDLWindowContext.h"

#define MOUSE_BUTTON_LOOKUP(x) (ms_MouseButtonEventLookup[x - SDL_BUTTON_LEFT])


namespace DENG {

	std::array<MouseButton, 5> SDLWindowContext::ms_MouseButtonEventLookup = {
		MouseButton::LeftClick,
		MouseButton::MiddleClick,
		MouseButton::RightClick,
		MouseButton::None,
		MouseButton::None
	};

	SDLWindowContext::SDLWindowContext(EventManager& _eventManager) :
		IWindowContext(_eventManager)
	{
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
	Keycode SDLWindowContext::_TranslateKeycode(SDL_Scancode _eCode) {
		switch (_eCode) {
		case SDL_SCANCODE_SPACE:
			return Keycode::Space;

		case SDL_SCANCODE_APOSTROPHE:
			return Keycode::Apostrophe;

		case SDL_SCANCODE_COMMA:
			return Keycode::Comma;

		case SDL_SCANCODE_MINUS:
			return Keycode::Minus;

		case SDL_SCANCODE_PERIOD:
			return Keycode::Period;

		case SDL_SCANCODE_SLASH:
			return Keycode::Slash;

		case SDL_SCANCODE_0:
			return Keycode::N0;

		case SDL_SCANCODE_1:
			return Keycode::N1;

		case SDL_SCANCODE_2:
			return Keycode::N2;

		case SDL_SCANCODE_3:
			return Keycode::N3;

		case SDL_SCANCODE_4:
			return Keycode::N4;

		case SDL_SCANCODE_5:
			return Keycode::N5;

		case SDL_SCANCODE_6:
			return Keycode::N6;

		case SDL_SCANCODE_7:
			return Keycode::N7;

		case SDL_SCANCODE_8:
			return Keycode::N8;

		case SDL_SCANCODE_9:
			return Keycode::N9;

		case SDL_SCANCODE_SEMICOLON:
			return Keycode::Semicolon;

		case SDL_SCANCODE_EQUALS:
			return Keycode::Equal;

		case SDL_SCANCODE_A:
			return Keycode::A;

		case SDL_SCANCODE_B:
			return Keycode::B;

		case SDL_SCANCODE_C:
			return Keycode::C;

		case SDL_SCANCODE_D:
			return Keycode::D;

		case SDL_SCANCODE_E:
			return Keycode::E;

		case SDL_SCANCODE_F:
			return Keycode::F;

		case SDL_SCANCODE_G:
			return Keycode::G;

		case SDL_SCANCODE_H:
			return Keycode::H;

		case SDL_SCANCODE_I:
			return Keycode::I;

		case SDL_SCANCODE_J:
			return Keycode::J;

		case SDL_SCANCODE_K:
			return Keycode::K;

		case SDL_SCANCODE_L:
			return Keycode::L;

		case SDL_SCANCODE_M:
			return Keycode::M;

		case SDL_SCANCODE_N:
			return Keycode::N;

		case SDL_SCANCODE_O:
			return Keycode::O;

		case SDL_SCANCODE_P:
			return Keycode::P;

		case SDL_SCANCODE_Q:
			return Keycode::Q;

		case SDL_SCANCODE_R:
			return Keycode::R;

		case SDL_SCANCODE_S:
			return Keycode::S;

		case SDL_SCANCODE_T:
			return Keycode::T;

		case SDL_SCANCODE_U:
			return Keycode::U;

		case SDL_SCANCODE_V:
			return Keycode::V;

		case SDL_SCANCODE_W:
			return Keycode::W;

		case SDL_SCANCODE_X:
			return Keycode::X;

		case SDL_SCANCODE_Y:
			return Keycode::Y;

		case SDL_SCANCODE_Z:
			return Keycode::Z;

		case SDL_SCANCODE_LEFTBRACKET:
			return Keycode::LeftBracket;

		case SDL_SCANCODE_RIGHTBRACKET:
			return Keycode::RightBracket;

		case SDL_SCANCODE_BACKSLASH:
			return Keycode::Backslash;

		case SDL_SCANCODE_GRAVE:
			return Keycode::GraveAccent;

		case SDL_SCANCODE_ESCAPE:
			return Keycode::Escape;

		case SDL_SCANCODE_RETURN:
			return Keycode::Enter;

		case SDL_SCANCODE_TAB:
			return Keycode::Tab;

		case SDL_SCANCODE_BACKSPACE:
			return Keycode::Backspace;

		case SDL_SCANCODE_INSERT:
			return Keycode::Insert;

		case SDL_SCANCODE_DELETE:
			return Keycode::Delete;

		case SDL_SCANCODE_RIGHT:
			return Keycode::RightArrow;

		case SDL_SCANCODE_LEFT:
			return Keycode::LeftArrow;

		case SDL_SCANCODE_DOWN:
			return Keycode::DownArrow;

		case SDL_SCANCODE_UP:
			return Keycode::UpArrow;

		case SDL_SCANCODE_PAGEUP:
			return Keycode::PageUp;

		case SDL_SCANCODE_PAGEDOWN:
			return Keycode::PageDown;

		case SDL_SCANCODE_HOME:
			return Keycode::Home;

		case SDL_SCANCODE_END:
			return Keycode::End;

		case SDL_SCANCODE_CAPSLOCK:
			return Keycode::CapsLock;

		case SDL_SCANCODE_SCROLLLOCK:
			return Keycode::ScrollLock;

		case SDL_SCANCODE_NUMLOCKCLEAR:
			return Keycode::NumLock;

		case SDL_SCANCODE_PRINTSCREEN:
			return Keycode::PrintScreen;

		case SDL_SCANCODE_PAUSE:
			return Keycode::Pause;

		case SDL_SCANCODE_F1:
			return Keycode::F1;

		case SDL_SCANCODE_F2:
			return Keycode::F2;

		case SDL_SCANCODE_F3:
			return Keycode::F3;

		case SDL_SCANCODE_F4:
			return Keycode::F4;

		case SDL_SCANCODE_F5:
			return Keycode::F5;

		case SDL_SCANCODE_F6:
			return Keycode::F6;

		case SDL_SCANCODE_F7:
			return Keycode::F7;

		case SDL_SCANCODE_F8:
			return Keycode::F8;

		case SDL_SCANCODE_F9:
			return Keycode::F9;

		case SDL_SCANCODE_F10:
			return Keycode::F10;

		case SDL_SCANCODE_F11:
			return Keycode::F11;

		case SDL_SCANCODE_F12:
			return Keycode::F12;

		case SDL_SCANCODE_F13:
			return Keycode::F13;

		case SDL_SCANCODE_F14:
			return Keycode::F14;

		case SDL_SCANCODE_F15:
			return Keycode::F15;

		case SDL_SCANCODE_F16:
			return Keycode::F16;

		case SDL_SCANCODE_F17:
			return Keycode::F17;

		case SDL_SCANCODE_F18:
			return Keycode::F18;

		case SDL_SCANCODE_F19:
			return Keycode::F19;

		case SDL_SCANCODE_F20:
			return Keycode::F20;

		case SDL_SCANCODE_F21:
			return Keycode::F21;

		case SDL_SCANCODE_F22:
			return Keycode::F22;

		case SDL_SCANCODE_F23:
			return Keycode::F23;

		case SDL_SCANCODE_F24:
			return Keycode::F24;

		case SDL_SCANCODE_KP_0:
			return Keycode::Keypad_0;

		case SDL_SCANCODE_KP_1:
			return Keycode::Keypad_1;

		case SDL_SCANCODE_KP_2:
			return Keycode::Keypad_2;

		case SDL_SCANCODE_KP_3:
			return Keycode::Keypad_3;

		case SDL_SCANCODE_KP_4:
			return Keycode::Keypad_4;

		case SDL_SCANCODE_KP_5:
			return Keycode::Keypad_5;

		case SDL_SCANCODE_KP_6:
			return Keycode::Keypad_6;

		case SDL_SCANCODE_KP_7:
			return Keycode::Keypad_7;

		case SDL_SCANCODE_KP_8:
			return Keycode::Keypad_8;

		case SDL_SCANCODE_KP_9:
			return Keycode::Keypad_9;

		case SDL_SCANCODE_KP_DECIMAL:
			return Keycode::Keypad_Decimal;

		case SDL_SCANCODE_KP_DIVIDE:
			return Keycode::Keypad_Divide;

		case SDL_SCANCODE_KP_MULTIPLY:
			return Keycode::Keypad_Multiply;

		case SDL_SCANCODE_KP_MINUS:
			return Keycode::Keypad_Subtract;

		case SDL_SCANCODE_KP_PLUS:
			return Keycode::Keypad_Add;

		case SDL_SCANCODE_KP_ENTER:
			return Keycode::Keypad_Enter;

		case SDL_SCANCODE_KP_EQUALS:
			return Keycode::Keypad_Equal;

		case SDL_SCANCODE_LSHIFT:
			return Keycode::LeftShift;

		case SDL_SCANCODE_LCTRL:
			return Keycode::LeftCtrl;

		case SDL_SCANCODE_LALT:
			return Keycode::LeftAlt;

		case SDL_SCANCODE_LGUI:
			return Keycode::LeftSuper;

		case SDL_SCANCODE_RSHIFT:
			return Keycode::RightShift;

		case SDL_SCANCODE_RCTRL:
			return Keycode::RightCtrl;

		case SDL_SCANCODE_RALT:
			return Keycode::RightAlt;

		case SDL_SCANCODE_RGUI:
			return Keycode::RightSuper;

		case SDL_SCANCODE_MENU:
			return Keycode::Menu;

		default:
			return Keycode::None;
		}
	}

	KeyModifier SDLWindowContext::_TranslateKeyboardModifier(uint16_t _uMod) {
		KeyModifier uKeymod = 0;
		
		if (_uMod & KMOD_LSHIFT)
			uKeymod |= KeyModifier_LeftShift;
		if (_uMod & KMOD_RSHIFT)
			uKeymod |= KeyModifier_RightShift;
		if (_uMod & KMOD_LCTRL)
			uKeymod |= KeyModifier_LeftCtrl;
		if (_uMod & KMOD_RCTRL)
			uKeymod |= KeyModifier_RightCtrl;
		if (_uMod & KMOD_LALT)
			uKeymod |= KeyModifier_LeftAlt;
		if (_uMod & KMOD_RALT)
			uKeymod |= KeyModifier_RightAlt;
		if (_uMod & KMOD_LGUI)
			uKeymod |= KeyModifier_LeftSuper;
		if (_uMod & KMOD_RGUI)
			uKeymod |= KeyModifier_RightSuper;
		if (_uMod & KMOD_NUM)
			uKeymod |= KeyModifier_NumLock;
		if (_uMod & KMOD_CAPS)
			uKeymod |= KeyModifier_CapsLock;
		if (_uMod & KMOD_MODE)
			uKeymod |= KeyModifier_AltGr;
		if (_uMod & KMOD_SCROLL)
			uKeymod |= KeyModifier_ScrollLock;

		return uKeymod;
	}

	MouseButtonModifier SDLWindowContext::_TranslateButtonModifier(uint32_t _uMod) {
		MouseButtonModifier uButtonMod = 0;

		if (_uMod & SDL_BUTTON_LMASK)
			uButtonMod |= MouseButtonModifier_LeftClick;
		if (_uMod & SDL_BUTTON_MMASK)
			uButtonMod |= MouseButtonModifier_MiddleClick;
		if (_uMod & SDL_BUTTON_RMASK)
			uButtonMod |= MouseButtonModifier_RightClick;

		return uButtonMod;
	}


	void SDLWindowContext::_TranslateAndDispatchWindowEvent() {
		switch (m_event.window.type) {
			case SDL_WINDOWEVENT_SHOWN:
				m_eventManager.Dispatch<WindowShownEvent>(m_event.window.windowID);
				break;

			case SDL_WINDOWEVENT_HIDDEN: 
				m_eventManager.Dispatch<WindowHiddenEvent>(m_event.window.windowID);
				break;
			
			case SDL_WINDOWEVENT_EXPOSED: 
				m_eventManager.Dispatch<WindowExposedEvent>(m_event.window.windowID);
				break;

			case SDL_WINDOWEVENT_RESIZED:
			case SDL_WINDOWEVENT_SIZE_CHANGED:
			case SDL_WINDOWEVENT_RESTORED:
				m_eventManager.Dispatch<WindowResizedEvent>(
					m_event.window.windowID,
					static_cast<uint32_t>(m_event.window.data1),
					static_cast<uint32_t>(m_event.window.data2));
				break;
			
			case SDL_WINDOWEVENT_MINIMIZED:
				m_eventManager.Dispatch<WindowMinimizedEvent>(m_event.window.windowID);
				break;
			
			case SDL_WINDOWEVENT_MAXIMIZED:
				m_eventManager.Dispatch<WindowMaximizedEvent>(m_event.window.windowID);
				break;
			
			case SDL_WINDOWEVENT_ENTER:
				m_eventManager.Dispatch<WindowGainedMouseFocusEvent>(
					m_event.window.windowID,
					m_event.window.data1,
					m_event.window.data2);
				break;

			case SDL_WINDOWEVENT_LEAVE:
				m_eventManager.Dispatch<WindowLostMouseFocusEvent>(
					m_event.window.windowID,
					m_event.window.data1,
					m_event.window.data2);
				break;
			
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				m_eventManager.Dispatch<WindowGainedKeyboardFocusEvent>(m_event.window.windowID);
				break;
			
			case SDL_WINDOWEVENT_FOCUS_LOST: 
				m_eventManager.Dispatch<WindowLostKeyboardFocusEvent>(m_event.window.windowID);
				break;

			case SDL_WINDOWEVENT_DISPLAY_CHANGED: 
				m_eventManager.Dispatch<WindowDisplayChangedEvent>(m_event.window.windowID, m_event.window.data1);
				break;
			
			default: 
				break;
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
		if (m_uHints & WindowHint_FullScreen)
			uFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		if (m_uHints & WindowHint_OpenGL)
			uFlags |= SDL_WINDOW_OPENGL;
		if (m_uHints & WindowHint_Vulkan)
			uFlags |= SDL_WINDOW_VULKAN;
		if (m_uHints & WindowHint_Shown)
			uFlags |= SDL_WINDOW_SHOWN;
		if (m_uHints & WindowHint_Hidden)
			uFlags |= SDL_WINDOW_HIDDEN;
		if (m_uHints & WindowHint_Borderless)
			uFlags |= SDL_WINDOW_BORDERLESS;
		if (m_uHints & WindowHint_Resizeable)
			uFlags |= SDL_WINDOW_RESIZABLE;
		if (m_uHints & WindowHint_Minimized)
			uFlags |= SDL_WINDOW_MINIMIZED;
		if (m_uHints & WindowHint_Maximized)
			uFlags |= SDL_WINDOW_MAXIMIZED;
		if (m_uHints & WindowHint_InputFocus)
			uFlags |= SDL_WINDOW_INPUT_FOCUS;
		if (m_uHints & WindowHint_MouseFocus)
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
		DENG_ASSERT(m_uHints & WindowHint_Vulkan);

		VkSurfaceKHR hSurface = nullptr;
		SDL_Vulkan_CreateSurface(m_pWindow, (VkInstance)_pInstance, &hSurface);
		return (void*)hSurface;
	}

	void SDLWindowContext::Update() {
		DENG_ASSERT(m_pWindow);

		// poll events that might've occured
		while (SDL_PollEvent(&m_event) != 0) {
			switch (m_event.type) {
				case SDL_QUIT:
					m_bIsAlive = false;
					return;

				case SDL_WINDOWEVENT:
					_TranslateAndDispatchWindowEvent();
					break;

				case SDL_KEYDOWN:
					m_eventManager.Dispatch<KeyPressedEvent>(
						_TranslateKeycode(m_event.key.keysym.scancode),
						_TranslateKeyboardModifier(m_event.key.keysym.mod));
					break;

				case SDL_KEYUP:
					m_eventManager.Dispatch<KeyReleasedEvent>(
						_TranslateKeycode(m_event.key.keysym.scancode),
						_TranslateKeyboardModifier(m_event.key.keysym.mod));
					break;

				case SDL_MOUSEBUTTONDOWN:
					m_eventManager.Dispatch<MouseButtonPressedEvent>(
						m_event.button.x,
						m_event.button.y,
						MOUSE_BUTTON_LOOKUP(m_event.button.button),
						0, 0);
					break;

				case SDL_MOUSEBUTTONUP:
					m_eventManager.Dispatch<MouseButtonReleasedEvent>(
						m_event.button.x,
						m_event.button.y,
						MOUSE_BUTTON_LOOKUP(m_event.button.button),
						0, 0);
					break;

				case SDL_MOUSEWHEEL:
					m_eventManager.Dispatch<MouseScrolledEvent>(
						m_event.wheel.mouseX,
						m_event.wheel.mouseY,
						m_event.wheel.x,
						m_event.wheel.y);
					break;

				case SDL_MOUSEMOTION:
					m_eventManager.Dispatch<MouseMovedEvent>(
						m_event.motion.x,
						m_event.motion.y,
						m_event.motion.xrel,
						m_event.motion.yrel);
					break;

				case SDL_TEXTINPUT:
				{
					char szBuffer[20] = {};
					std::strcat(szBuffer, m_event.text.text);
					m_eventManager.Dispatch<KeyTypedEvent>(szBuffer);
					break;
				}

				default:
					break;
			}
		}
	}
}

