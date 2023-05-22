// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: IWindowContext.h - window context base class header
// author: Karl-Mihkel Ott

#ifndef IWINDOW_CONTEXT_H
#define IWINDOW_CONTEXT_H

#include <string>
#include <array>
#include <queue>
#include <vector>

#include "deng/Api.h"
#define TEXT_BUFFER_LENGTH 32

namespace DENG {

	enum class EventType {
		WINDOW,
		KEY_DOWN,
		KEY_UP,
		TEXT_EDITING,
		TEXT_INPUT,
		MOUSE_MOTION,
		MOUSE_BUTTON_DOWN,
		MOUSE_BUTTON_UP,
		MOUSE_WHEEL,
		GAMEPAD_AXIS_MOTION,
		GAMEPAD_BUTTON_DOWN,
		GAMEPAD_BUTTON_UP,
		GAMEPAD_TOUCHPAD_DOWN,
		GAMEPAD_TOUCHPAD_UP
	};

	enum class WindowEvent {
		UNKNOWN = 0x001,
		SHOWN,
		HIDDEN,
		EXPOSED,
		RESIZED,
		SIZE_CHANGED,
		MINIMIZED,
		MAXIMIZED,
		RESTORED,
		GAINED_MOUSE_FOCUS,
		LOST_MOUSE_FOCUS,
		GAINED_KEYBOARD_FOCUS,
		LOST_KEYBOARD_FOCUS,
		DISPLAY_CHANGED
	};

	enum class KeySymbol {
		KEY_UNKNOWN = 0x010,
		KEY_SPACE,
		KEY_APOSTROPHE,
		KEY_COMMA,
		KEY_MINUS,
		KEY_DOT,
		KEY_SLASH,
		KEY_0,
		KEY_1,
		KEY_2,
		KEY_3,
		KEY_4,
		KEY_5,
		KEY_6,
		KEY_7,
		KEY_8,
		KEY_9,
		KEY_SEMICOLON,
		KEY_EQUAL,
		KEY_A,
		KEY_B,
		KEY_C,
		KEY_D,
		KEY_E,
		KEY_F,
		KEY_G,
		KEY_H,
		KEY_I,
		KEY_J,
		KEY_K,
		KEY_L,
		KEY_M,
		KEY_N,
		KEY_O,
		KEY_P,
		KEY_Q,
		KEY_R,
		KEY_S,
		KEY_T,
		KEY_U,
		KEY_V,
		KEY_W,
		KEY_X,
		KEY_Y,
		KEY_Z,
		KEY_LEFT_BRACKET,      // '['
		KEY_RIGHT_BRACKET,     // ']'
		KEY_BACKSLASH,         // '\'
		KEY_GRAVE_ACCENT,      // '`'
		KEY_ESCAPE,
		KEY_ENTER,
		KEY_TAB,
		KEY_BACKSPACE,
		KEY_INSERT,
		KEY_DELETE,
		KEY_RIGHT,
		KEY_LEFT,
		KEY_DOWN,
		KEY_UP,
		KEY_PAGE_UP,
		KEY_PAGE_DOWN,
		KEY_HOME,
		KEY_END,
		KEY_CAPS_LOCK,
		KEY_SCROLL_LOCK,
		KEY_NUM_LOCK,
		KEY_PRINT_SCREEN,
		KEY_PAUSE,
		KEY_F1,
		KEY_F2,
		KEY_F3,
		KEY_F4,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8,
		KEY_F9,
		KEY_F10,
		KEY_F11,
		KEY_F12,
		KEY_F13,
		KEY_F14,
		KEY_F15,
		KEY_F16,
		KEY_F17,
		KEY_F18,
		KEY_F19,
		KEY_F20,
		KEY_F21,
		KEY_F22,
		KEY_F23,
		KEY_F24,
		KEY_F25,
		KEY_NP_0,
		KEY_NP_1,
		KEY_NP_2,
		KEY_NP_3,
		KEY_NP_4,
		KEY_NP_5,
		KEY_NP_6,
		KEY_NP_7,
		KEY_NP_8,
		KEY_NP_9,
		KEY_NP_DECIMAL,
		KEY_NP_DIVIDE,
		KEY_NP_MULTIPLY,
		KEY_NP_SUBTRACT,
		KEY_NP_ADD,
		KEY_NP_ENTER,
		KEY_NP_EQUAL,
		KEY_LEFT_SHIFT,
		KEY_LEFT_CTRL,
		KEY_LEFT_ALT,
		KEY_LEFT_SUPER,
		KEY_RIGHT_SHIFT,
		KEY_RIGHT_CTRL,
		KEY_RIGHT_ALT,
		KEY_RIGHT_SUPER,
		KEY_MENU
	};


	enum KeyModifier {
		MOD_NONE	=	0x00000000,
		MOD_LSHIFT	=	0x00000001,
		MOD_RSHIFT  =	0x00000002,
		MOD_LCTRL	=	0x00000004,
		MOD_RCTRL	=	0x00000008,
		MOD_LALT	=	0x00000010,
		MOD_RALT	=	0x00000020,
		MOD_LSUPER	=	0x00000040,
		MOD_RSUPER	=	0x00000080,
		MOD_NUM		=	0x00000100,
		MOD_CAPS	=	0x00000200,
		MOD_ALTGR	=	0x00000400,
		MOD_SCROLL	=	0x00000800,
	};

	enum ButtonModifier {
		MOD_LCLICK	=	0x00001000,
		MOD_MCLICK	=	0x00002000,
		MOD_RCLICK	=	0x00004000,
		MOD_X1CLICK	=	0x00008000,
		MOD_X2CLICK	=	0x00010000
	};
	
	enum class MouseEvent {
		BTN_UNKNOWN = 0x087,
		BTN_1,
		BTN_2,
		BTN_3,
		BTN_X1,
		BTN_X2,
	};

	enum class GamepadEvent {
		BUTTON_A = 0x08d,
		BUTTON_B,
		BUTTON_X,
		BUTTON_Y,
		BUTTON_LB,
		BUTTON_RB,
		BUTTON_LT,
		BUTTON_RT,
		BUTTON_DPAD_UP,
		BUTTON_DPAD_RIGHT,
		BUTTON_DPAD_LEFT,
		BUTTON_DPAD_DOWN,
		JOYSTICK_1,
		JOYSTICK_2,
	};


	struct Event {
		char szEditedText[TEXT_BUFFER_LENGTH] = {};
		EventType eType;
		int32_t uDescription = 0;
		int32_t uModifier = 0;
		int32_t x = 0;
		int32_t y = 0;
		int32_t xrel = 0;
		int32_t yrel = 0;
	};

	typedef uint64_t HidMask;

	
#define READ_HID(mask, i) static_cast<HidEvent>((mask >> ((7 - i) * 8)) & 0xff)
#define MASK_HID(first, ...) IWindowContext::MaskHidEvent(0, 56, first, __VA_ARGS__)


	enum class Cursor {
		STANDARD,
		WAITING,
		POINTER,
		HIDDEN
	};

	enum WindowHints{
		NONE =			0x00000000,
		FULL_SCREEN =	0x00000001,
		OPENGL =		0x00000002,
		VULKAN =		0x00000004,
		DIRECTX =		0x00000008,
		SHOWN =			0x00000010,
		HIDDEN =		0x00000020,
		BORDERLESS =	0x00000040,
		RESIZEABLE =	0x00000080,
		MINIMIZED =		0x00000100,
		MAXIMIZED =		0x00000200,
		INPUT_FOCUS =	0x00000400,
		MOUSE_FOCUS =	0x00000800
	};

	class DENG_API IWindowContext {
		protected:
			std::string m_sTitle;
			uint32_t m_uHeight = 0;
			uint32_t m_uWidth = 0;

			uint32_t m_uHints = WindowHints::NONE;
			bool m_bIsAlive = false;

			std::queue<Event> m_eventQueue;

		public:
			IWindowContext(const std::string& _sTitle = "MyApplication") :
				m_sTitle(_sTitle) {}
			
			virtual void Create(const std::string& _sTitle, uint32_t _uWidth, uint32_t _uHeight) = 0;
			virtual void* CreateVulkanSurface(void* _pInstance) = 0;
			
			inline bool IsAlive() {
				return m_bIsAlive;
			}
			
			virtual void Update() = 0;
			
			inline std::queue<Event>& GetEventQueue() {
				return m_eventQueue;
			}

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

			template<typename T, typename ...Args>
			static HidMask MaskHidEvent(HidMask base, HidMask bits, T first, Args... args) {
				base |= (static_cast<HidMask>(first) << bits);
				return MaskHidEvent(base, bits - 8, args);
			}

			inline void SetHints(uint32_t _uHints) {
				m_uHints = _uHints;
			}

			inline void SetHeight(uint32_t _uHeight) {
				m_uHeight = _uHeight;
			}

			inline void SetWidth(uint32_t _uWidth) {
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
