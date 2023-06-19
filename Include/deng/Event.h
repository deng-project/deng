// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Event.h - Event interaface and manager header
// author: Karl-Mihkel Ott

#ifndef EVENT_H
#define EVENT_H

#include <cstdint>
#include <list>
#include <array>
#include <chrono>
#include <functional>
#include <mutex>

#include "deng/Api.h"
#include "deng/SID.h"

namespace DENG {

	//enum class EventType {
	//	None = 0,
	//	Window,
	//	WindowFocus,
	//	WindowGainedMouseFocus, WindowLostMouseFocus, WindowGainedKeyboardFocus, WindowLostKeyboardFocus,
	//	WindowShown, WindowHidden, WindowExposed, WindowResized, WindowMinimized, WindowMaximized,
	//	WindowDisplayChanged,
	//	Input,
	//	Keyboard,
	//	KeyPressed, KeyReleased, KeyTyped,
	//	Mouse,
	//	MouseMoved, MouseScrolled,
	//	MouseButton,
	//	MouseButtonPressed, MouseButtonReleased,
	//	Gamepad,
	//	GamepadButton,
	//	GamepadButtonPressed, GamepadButtonReleased,
	//	GamepadJoystickMoved,
	//	EventCount
	//};

#define EVENT_CLASS_TYPE(str_type, parent)	static constexpr DENG::hash_t GetStaticType() { return SID(str_type); }\
											virtual DENG::hash_t GetEventType() const override { return GetStaticType(); }\
											const parent* GetParent() const { return static_cast<const parent*>(this); }\
											virtual const char* GetName() const override { return str_type; }

	class DENG_API IEvent {
		private:
			std::chrono::time_point<std::chrono::high_resolution_clock> m_timestamp =
				std::chrono::high_resolution_clock::now();

		public:
			static constexpr hash_t GetStaticType() {
				return 0;
			}

			virtual hash_t GetEventType() const {
				return IEvent::GetStaticType();
			}

			IEvent* GetParent() const {
				return nullptr;
			};

			virtual const char* GetName() const = 0;
	
			inline time_t GetTimestamp() {
				return static_cast<time_t>(m_timestamp.time_since_epoch().count());
			}
	};

	class EventListener {
		private:
			using PFN_CallbackType = std::function<bool(IEvent&)>;

			PFN_CallbackType m_CallbackLambda = nullptr;
			void* m_pInstance = nullptr;

		public:
			EventListener(void* _pInstance, const PFN_CallbackType& _CallbackLambda) :
				m_CallbackLambda(_CallbackLambda),
				m_pInstance(_pInstance) {}
			EventListener(const EventListener&) = default;

			inline bool OnEvent(IEvent& _event) {
				if (m_CallbackLambda)
					return m_CallbackLambda(_event);
				return false;
			}

			template<typename T>
			inline bool IsInstance(T* _pInstance) {
				return reinterpret_cast<T*>(m_pInstance) == _pInstance;
			}
	};


	class DENG_API EventManager {
		private:
			std::mutex m_mutex;
			std::unordered_map<hash_t, std::list<EventListener>, NoHash> m_eventHandlers;
			static EventManager m_sEventManager;

		private:
			template<typename T>
			constexpr void _TraverseInheritanceHierarchy(T* _pEvent, std::vector<hash_t>& _eventTypesToDispatch) {
				_eventTypesToDispatch.push_back(T::GetStaticType());

				if constexpr (T::GetStaticType() != 0) {
					_TraverseInheritanceHierarchy(_pEvent->GetParent(), _eventTypesToDispatch);
				}
			}

			template<typename T>
			constexpr std::vector<hash_t> _GetEventTypesToDispatch(T* _pEvent) {
				std::vector<hash_t> eventTypesToDispatch;

				_TraverseInheritanceHierarchy(_pEvent, eventTypesToDispatch);

				return eventTypesToDispatch;
			}
			
			EventManager() = default;

		public:
			static EventManager& GetInstance() {
				return m_sEventManager;
			}

			template<typename T, typename E>
			using PFN_ListenerCallback_T = bool(T::*)(E&);

			template<typename T, typename E>
			inline void AddListener(PFN_ListenerCallback_T<T, E> _pfnCallback, T* _pClassInstance) {
				std::scoped_lock lock(m_mutex);
				m_eventHandlers[E::GetStaticType()].emplace_back(
					_pClassInstance,
					[=](IEvent& _event) {
						return (*_pClassInstance.*_pfnCallback)(static_cast<E&>(_event));
					}
				);
			}

			template<typename T, typename E>
			inline void RemoveListener(T* _pInstance) {
				std::scoped_lock lock(m_mutex);
				auto& listeners = m_eventHandlers[E::GetStaticType()];

				for (auto it = listeners.begin(); it != listeners.end(); it++) {
					if (it->IsInstance(_pInstance)) {
						listeners.erase(it);
						break;
					}
				}
			}


			template<typename T, typename... Args>
			void Dispatch(Args... args) {
				std::scoped_lock lock(m_mutex);
				T event(std::forward<Args>(args)...);
				
				std::vector<hash_t> eventTypesToDispatch = _GetEventTypesToDispatch(&event);

				for (hash_t hshEventType : eventTypesToDispatch) {
					auto& eventHandlers = m_eventHandlers[hshEventType];

					for (auto it = eventHandlers.rbegin(); it != eventHandlers.rend(); it++) {
						if (it->OnEvent(event))
							break;
					}
				}
			}
	};
}

#endif
