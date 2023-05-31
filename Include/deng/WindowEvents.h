// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: WindowEvents.h - Window events class header
// author: Karl-Mihkel Ott

#ifndef WINDOW_EVENTS_H
#define WINDOW_EVENTS_H

#include "deng/Event.h"

namespace DENG {

	class DENG_API WindowEvent : public IEvent {
		private:
			uint32_t m_uWindowId = 0;

		public:
			WindowEvent(uint32_t _uWindowId) :
				m_uWindowId(_uWindowId) {}
			WindowEvent(const WindowEvent&) = default;

			inline uint32_t GetWindowId() {
				return m_uWindowId;
			}

			EVENT_CLASS_TYPE(EventType::Window, IEvent)
	};


	class DENG_API WindowShownEvent : public WindowEvent {
		public:
			WindowShownEvent(uint32_t _uWindowId) :
				WindowEvent(_uWindowId) {}
			WindowShownEvent(const WindowShownEvent&) = default;

			EVENT_CLASS_TYPE(EventType::WindowShown, WindowEvent);
	};

	
	class DENG_API WindowHiddenEvent : public WindowEvent {
		public:
			WindowHiddenEvent(uint32_t _uWindowId) :
				WindowEvent(_uWindowId) {}
			WindowHiddenEvent(const WindowHiddenEvent&) = default;

			EVENT_CLASS_TYPE(EventType::WindowHidden, WindowEvent);
	};


	class DENG_API WindowExposedEvent : public WindowEvent {
		private:
			uint32_t m_uWidth;
			uint32_t m_uHeight;

		public:
			WindowExposedEvent(uint32_t _uWindowId, uint32_t _uWidth, uint32_t _uHeight) :
				WindowEvent(_uWindowId),
				m_uWidth(_uWidth),
				m_uHeight(_uHeight) {}
			WindowExposedEvent(const WindowExposedEvent&) = default;
			
			inline uint32_t GetWidth() {
				return m_uWidth;
			}

			inline uint32_t GetHeight() {
				return m_uHeight;
			}

			EVENT_CLASS_TYPE(EventType::WindowExposed, WindowEvent);
	};


	class DENG_API WindowResizedEvent : public WindowEvent {
		private:
			uint32_t m_uWidth;
			uint32_t m_uHeight;

		public:
			WindowResizedEvent(uint32_t _uWindowId, uint32_t _uWidth, uint32_t _uHeight) :
				WindowEvent(_uWindowId),
				m_uWidth(_uWidth),
				m_uHeight(_uHeight) {}
			WindowResizedEvent(const WindowResizedEvent&) = default;

			inline uint32_t GetWidth() {
				return m_uWidth;
			}

			inline uint32_t GetHeight() {
				return m_uHeight;
			}

			EVENT_CLASS_TYPE(EventType::WindowResized, WindowEvent);
	};


	class DENG_API WindowMinimizedEvent : public WindowEvent {
		public:
			WindowMinimizedEvent(uint32_t _uWindowId) :
				WindowEvent(_uWindowId) {}
			WindowMinimizedEvent(const WindowMinimizedEvent&) = default;

			EVENT_CLASS_TYPE(EventType::WindowMinimized, WindowEvent);
	};


	class DENG_API WindowMaximizedEvent : public WindowEvent {
		public:
			WindowMaximizedEvent(uint32_t _uWindowId) :
				WindowEvent(_uWindowId) {}
			WindowMaximizedEvent(const WindowMaximizedEvent&) = default;

			EVENT_CLASS_TYPE(EventType::WindowMaximized, WindowEvent);
	};


	class DENG_API WindowDisplayChangedEvent : public WindowEvent {
		private:
			uint32_t m_uDisplayIndex;

		public:
			WindowDisplayChangedEvent(uint32_t _uWindowId, uint32_t _uDisplayIndex) :
				WindowEvent(_uWindowId),
				m_uDisplayIndex(_uDisplayIndex) {}
			WindowDisplayChangedEvent(const WindowDisplayChangedEvent&) = default;

			inline uint32_t GetDisplayIndex() {
				return m_uDisplayIndex;
			}

			EVENT_CLASS_TYPE(EventType::WindowDisplayChanged, WindowEvent)
	};


	class DENG_API WindowFocusEvent : public WindowEvent {
		public:
			WindowFocusEvent(uint32_t _uWindowId) :
				WindowEvent(_uWindowId) {}
			WindowFocusEvent(const WindowFocusEvent&) = default;

			EVENT_CLASS_TYPE(EventType::WindowFocus, WindowEvent);
	};


	class DENG_API WindowGainedMouseFocusEvent : public WindowFocusEvent {
		private:
			uint32_t m_uMouseX;
			uint32_t m_uMouseY;

		public:
			WindowGainedMouseFocusEvent(uint32_t _uWindowId, uint32_t _uMouseX, uint32_t _uMouseY) :
				WindowFocusEvent(_uWindowId),
				m_uMouseX(_uMouseX),
				m_uMouseY(_uMouseY) {}
			WindowGainedMouseFocusEvent(const WindowGainedMouseFocusEvent&) = default;
			
			inline uint32_t GetCoordinateX() const {
				return m_uMouseX;
			}

			inline uint32_t GetCoordinateY() const {
				return m_uMouseY;
			}

			EVENT_CLASS_TYPE(EventType::WindowGainedMouseFocus, WindowFocusEvent);
	};


	class DENG_API WindowLostMouseFocusEvent : public WindowFocusEvent {
		private:
			uint32_t m_uMouseX;
			uint32_t m_uMouseY;

		public:
			WindowLostMouseFocusEvent(uint32_t _uWindowId, uint32_t _uMouseX, uint32_t _uMouseY) :
				WindowFocusEvent(_uWindowId),
				m_uMouseX(_uMouseX),
				m_uMouseY(_uMouseY) {}
			WindowLostMouseFocusEvent(const WindowLostMouseFocusEvent&) = default;

			inline uint32_t GetLastMousePositionX() const {
				return m_uMouseX;
			}

			inline uint32_t GetLastMousePositionY() const {
				return m_uMouseY;
			}

			EVENT_CLASS_TYPE(EventType::WindowLostMouseFocus, WindowFocusEvent);
	};


	class DENG_API WindowGainedKeyboardFocusEvent : public WindowFocusEvent {
		public:
			WindowGainedKeyboardFocusEvent(uint32_t _uWindowId) :
				WindowFocusEvent(_uWindowId) {}
			WindowGainedKeyboardFocusEvent(const WindowGainedKeyboardFocusEvent&) = default;

			EVENT_CLASS_TYPE(EventType::WindowGainedKeyboardFocus, WindowFocusEvent);
	};


	class DENG_API WindowLostKeyboardFocusEvent : public WindowFocusEvent {
		public:
			WindowLostKeyboardFocusEvent(uint32_t _uWindowId) :
				WindowFocusEvent(_uWindowId) {}
			WindowLostKeyboardFocusEvent(const WindowLostKeyboardFocusEvent&) = default;

			EVENT_CLASS_TYPE(EventType::WindowLostKeyboardFocus, WindowFocusEvent);
	};
}

#endif