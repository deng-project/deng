// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: WindowEvents.h - Input events class header
// author: Karl-Mihkel Ott

#ifndef INPUT_EVENTS_H
#define INPUT_EVENTS_H

#include "deng/Event.h"

namespace DENG {
	
	enum class Keycode {
		None, Space, Apostrophe, Comma, Minus, Period, Slash,
		N0, N1, N2, N3, N4, N5, N6, N7, N8, N9,
		Semicolon, Equal,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N,
		O, P, Q, R, S, T, U, V, W, X, Y, Z,
		LeftBracket,      // '['
		RightBracket,     // ']'
		Backslash,         // '\'
		GraveAccent,      // '`'
		Escape, Enter, Tab, Backspace, Insert, Delete, 
		RightArrow, LeftArrow, DownArrow, UpArrow,
		PageUp, PageDown, Home, End,
		CapsLock, ScrollLock, NumLock,
		PrintScreen, Pause,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, 
		F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25,
		Keypad_0, Keypad_1, Keypad_2, Keypad_3, Keypad_4,
		Keypad_5, Keypad_6, Keypad_7, Keypad_8, Keypad_9,
		Keypad_Decimal, Keypad_Divide, Keypad_Multiply,
		Keypad_Subtract, Keypad_Add, Keypad_Enter,
		Keypad_Equal,
		LeftShift, LeftCtrl, LeftAlt, LeftSuper,
		RightShift, RightCtrl, RightAlt, RightSuper,
		Menu
	};

	enum KeyModifier_T {
		KeyModifier_None = 0,
		KeyModifier_LeftShift = 1 << 0,
		KeyModifier_RightShift = 1 << 1,
		KeyModifier_LeftCtrl = 1 << 2,
		KeyModifier_RightCtrl = 1 << 3,
		KeyModifier_LeftAlt = 1 << 4,
		KeyModifier_RightAlt = 1 << 5,
		KeyModifier_LeftSuper = 1 << 6,
		KeyModifier_RightSuper = 1 << 7,
		KeyModifier_NumLock = 1 << 8,
		KeyModifier_CapsLock = 1 << 9,
		KeyModifier_AltGr = 1 << 10,
		KeyModifier_ScrollLock = 1 << 11
	};

	typedef uint32_t KeyModifier;

	enum class MouseButton {
		None,
		LeftClick,
		MiddleClick,
		RightClick
	};

	enum MouseButtonModifier_T {
		MouseButtonModifier_LeftClick = 1 << 0,
		MouseButtonModifier_MiddleClick = 1 << 1,
		MouseButtonModifier_RightClick = 1 << 2
	};

	typedef uint16_t MouseButtonModifier;


	enum class GamepadButton {
		None,
		A, B, X, Y,
		LB, RB, LT, RT,
		DPad_Up,
		DPad_Right,
		DPad_Left,
		DPad_Down,
		LS, RS,
		Touchpad
	};

	class DENG_API InputEvent : public IEvent {
		public:
			EVENT_CLASS_TYPE("InputEvent", IEvent);
	};


	class DENG_API KeyboardEvent : public InputEvent {
		public:
			EVENT_CLASS_TYPE("KeyboardEvent", InputEvent);
	};


	class DENG_API KeyPressedEvent : public KeyboardEvent {
		private:
			Keycode m_keycode = Keycode::None;
			KeyModifier m_keyModifier = 0;

		public:
			KeyPressedEvent(Keycode _keycode, KeyModifier _keyModifier) :
				m_keycode(_keycode),
				m_keyModifier(_keyModifier) {}
			KeyPressedEvent(const KeyPressedEvent&) = default;

			inline Keycode GetKeycode() {
				return m_keycode;
			}

			inline KeyModifier GetKeyModifier() {
				return m_keyModifier;
			}

			EVENT_CLASS_TYPE("KeyPressedEvent", KeyboardEvent);
	};


	class DENG_API KeyReleasedEvent : public KeyboardEvent {
		private:
			Keycode m_keycode = Keycode::None;
			KeyModifier m_keyModifier = 0;

		public:
			KeyReleasedEvent(Keycode _keycode, KeyModifier _keyModifier) :
				m_keycode(_keycode),
				m_keyModifier(_keyModifier) {}
			KeyReleasedEvent(const KeyReleasedEvent&) = default;

			inline Keycode GetKeycode() {
				return m_keycode;
			}

			inline KeyModifier GetKeyModifier() {
				return m_keyModifier;
			}

			EVENT_CLASS_TYPE("KeyReleasedEvent", KeyboardEvent);
	};


	class DENG_API KeyTypedEvent : public KeyboardEvent {
		private:
			char m_buffer[20] = {};

		public:
			KeyTypedEvent(const char* _pGlyphs) {
				std::strncpy(m_buffer, _pGlyphs, 20);
			}
			KeyTypedEvent(const KeyTypedEvent&) = default;

			inline const char* GetBuffer() {
				return m_buffer;
			}

			EVENT_CLASS_TYPE("KeyTypedEvent", KeyboardEvent);
	};


	class DENG_API MouseEvent : public InputEvent {
		private:
			int32_t m_iPositionX;
			int32_t m_iPositionY;

		public:
			MouseEvent(int32_t _iPositionX, int32_t _iPositionY) :
				m_iPositionX(_iPositionX),
				m_iPositionY(_iPositionY) {}
			MouseEvent(const MouseEvent&) = default;

			inline int32_t GetPositionX() {
				return m_iPositionX;
			}

			inline int32_t GetPositionY() {
				return m_iPositionY;
			}

			EVENT_CLASS_TYPE("MouseEvent", InputEvent);
	};


	class DENG_API MouseMovedEvent : public MouseEvent {
		private:
			int32_t m_iDeltaX;
			int32_t m_iDeltaY;

		public:
			MouseMovedEvent(int32_t _iPositionX, int32_t _iPositionY, int32_t _iDeltaX, int32_t _iDeltaY) :
				MouseEvent(_iPositionX, _iPositionY),
				m_iDeltaX(_iDeltaX),
				m_iDeltaY(_iDeltaY) {}
			MouseMovedEvent(const MouseMovedEvent&) = default;

			inline int32_t GetMouseDeltaX() {
				return m_iDeltaX;
			}

			inline int32_t GetMouseDeltaY() {
				return m_iDeltaY;
			}

			EVENT_CLASS_TYPE("MouseMovedEvent", MouseEvent);
	};


	class DENG_API MouseScrolledEvent : public MouseEvent {
		private:
			int32_t m_uScrollDeltaX;
			int32_t m_uScrollDeltaY;

		public:
			MouseScrolledEvent(uint32_t _uPositionX, uint32_t _uPositionY, int32_t _uScrollDeltaX, int32_t _uScrollDeltaY) :
				MouseEvent(_uPositionX, _uPositionY),
				m_uScrollDeltaX(_uScrollDeltaX),
				m_uScrollDeltaY(_uScrollDeltaY) {}
			MouseScrolledEvent(const MouseScrolledEvent&) = default;

			inline int32_t GetScrollDeltaX() {
				return m_uScrollDeltaX;
			}

			inline int32_t GetScrollDeltaY() {
				return m_uScrollDeltaY;
			}

			EVENT_CLASS_TYPE("MouseScrolledEvent", MouseEvent);
	};


	class DENG_API MouseButtonEvent : public MouseEvent {
		private:
			MouseButton m_eButton = MouseButton::None;
			MouseButtonModifier m_uMouseButtonModifier = 0;
			KeyModifier m_uKeyModifier = 0;

		public:
			MouseButtonEvent(
				uint32_t _uPositionX, 
				uint32_t _uPositionY, 
				MouseButton _eButton, 
				MouseButtonModifier _uMouseModifier = 0,
				KeyModifier _uKeyModifier = 0) :
				MouseEvent(_uPositionX, _uPositionY),
				m_eButton(_eButton),
				m_uMouseButtonModifier(_uMouseModifier),
				m_uKeyModifier(_uKeyModifier) {}
			MouseButtonEvent(const MouseButtonEvent&) = default;

			inline MouseButton GetMouseButton() {
				return m_eButton;
			}

			inline MouseButtonModifier GetMouseButtonModifier() {
				return m_uMouseButtonModifier;
			}

			inline KeyModifier GetKeyModifier() {
				return m_uKeyModifier;
			}

			EVENT_CLASS_TYPE("MouseButtonEvent", MouseEvent);
	};


	class DENG_API MouseButtonPressedEvent : public MouseButtonEvent {
		public:
			MouseButtonPressedEvent(
				uint32_t _uPositionX, 
				uint32_t _uPositionY, 
				MouseButton _eButton, 
				MouseButtonModifier _uMouseModifier, 
				KeyModifier _uKeyModifier) :
				MouseButtonEvent(_uPositionX, _uPositionY, _eButton, _uMouseModifier, _uKeyModifier) {}
			MouseButtonPressedEvent(const MouseButtonPressedEvent&) = default;

			EVENT_CLASS_TYPE("MouseButtonPressedEvent", MouseButtonEvent);
	};


	class DENG_API MouseButtonReleasedEvent : public MouseButtonEvent {
		public:
			MouseButtonReleasedEvent(
				uint32_t _uPositionX,
				uint32_t _uPositionY,
				MouseButton _eButton,
				MouseButtonModifier _uMouseModifier,
				KeyModifier _uKeyModifier) :
				MouseButtonEvent(_uPositionX, _uPositionY, _eButton, _uMouseModifier, _uKeyModifier) {}
			MouseButtonReleasedEvent(const MouseButtonReleasedEvent&) = default;

			EVENT_CLASS_TYPE("MouseButtonReleasedEvent", MouseButtonEvent);
	};


	class DENG_API GamepadEvent : public InputEvent {
		public:
			EVENT_CLASS_TYPE("GamepadEvent", InputEvent);
	};


	class DENG_API GamepadButtonEvent : public GamepadEvent {
		private:
			GamepadButton m_eButton;

		public:
			GamepadButtonEvent(GamepadButton _eButton) :
				m_eButton(_eButton) {}
			GamepadButtonEvent(const GamepadButtonEvent&) = default;

			inline GamepadButton GetButton() {
				return m_eButton;
			}

			EVENT_CLASS_TYPE("GamepadButtonEvent", GamepadEvent)
	};


	class DENG_API GamepadButtonPressedEvent : public GamepadButtonEvent {
		public:
			GamepadButtonPressedEvent(GamepadButton _eButton) :
				GamepadButtonEvent(_eButton) {}
			GamepadButtonPressedEvent(const GamepadButtonPressedEvent&) = default;

			EVENT_CLASS_TYPE("GamepadButtonPressedEvent", GamepadButtonEvent);
	};


	class DENG_API GamepadButtonReleasedEvent : public GamepadButtonEvent {
		public:
			GamepadButtonReleasedEvent(GamepadButton _eButton) :
				GamepadButtonEvent(_eButton) {}
			GamepadButtonReleasedEvent(const GamepadButtonReleasedEvent&) = default;

			EVENT_CLASS_TYPE("GamepadButtonReleasedEvent", GamepadButtonEvent);
	};


	class DENG_API GamepadJoystickMovedEvent : public GamepadEvent {
		private:
			int32_t m_iAxisX;
			int32_t m_iAxisY;

		public:
			GamepadJoystickMovedEvent(int32_t _iAxisX, int32_t _iAxisY) :
				m_iAxisX(_iAxisX),
				m_iAxisY(_iAxisY) {}
			GamepadJoystickMovedEvent(const GamepadJoystickMovedEvent&) = default;

			inline int32_t GetJoystickAxisX() {
				return m_iAxisX;
			}

			inline int32_t GetJoystickAxisY() {
				return m_iAxisY;
			}

			EVENT_CLASS_TYPE("GamepadJoystickMovedEvent", GamepadEvent);
	};
}

#endif
