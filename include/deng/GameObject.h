// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ModelLoader.h - DAS model loader class header
// author: Karl-Mihkel Ott

#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#define GAME_OBJECT_CPP
#ifdef GAME_OBJECT_CPP
	#include <cmath>
	#include <functional>

	#include "trs/Points.h"
	#include "trs/Vector.h"
	#include "trs/Matrix.h"
	#include <trs/Quaternion.h>

	#include "deng/Api.h"
	#include "deng/Entity.h"
	#include "deng/Components.h"
#endif

namespace DENG {

	class GameObject; // forward declartation for reasons

	// NOTE: Each scriptable entity can have one instance of ScriptComponent
	class ScriptBehaviour {
		protected:
			GameObject* m_gameObject;

		public:
			ScriptBehaviour(GameObject* _gameObject) :
				m_gameObject(_gameObject) {}

			inline GameObject* GetGameObject() {
				return m_gameObject;
			}
	};

	template<typename T>
	class TypeTest {
		public:
			template <typename U> static char TestOnAttach(decltype(&U::OnAttach));
			template <typename U> static short TestOnAttach(...);

			template <typename U> static char TestOnUpdate(decltype(&U::OnUpdate));
			template <typename U> static short TestOnUpdate(...);

			template <typename U> static char TestOnDestroy(decltype(&U::OnDestroy));
			template <typename U> static short TestOnDestroy(...);

			enum {
				HAS_ON_ATTACH = sizeof(TestOnAttach<T>(0)) == sizeof(char),
				HAS_ON_UPDATE = sizeof(TestOnAttach<T>(0)) == sizeof(char),
				HAS_ON_DESTROY = sizeof(TestOnDestroy<T>(0)) == sizeof(char)
			};
	};

	class DENG_API GameObject : public Entity {
		private:
			std::list<GameObject*> m_children;
			std::list<Component*> m_components;

			ScriptBehaviour* m_pScript = nullptr;
			std::function<void(ScriptBehaviour*)> _OnAttachFunction = nullptr;
			std::function<void(ScriptBehaviour*)> _OnUpdateFunction = nullptr;
			std::function<void(ScriptBehaviour*)> _OnDestroyFunction = nullptr;

		public:

			template <typename T, typename... Types>
			void BindScript(Types... args) {
				m_pScript = new T(this, args);
				
				if constexpr (TypeTest<T>::HAS_ON_ATTACH) {
					_OnAttachFunction = [](ScriptComponent* _instance) {
						static_cast<T*>(_instance)->OnAttach();
					};
				}

				if constexpr (TypeTest<T>::HAS_ON_UPDATE) {
					_OnUpdateFunction = [](ScriptComponent* _instance) {
						static_cast<T*>(_instance)->OnUpdate();
					};
				}

				if constexpr (TypeTest<T>::HAS_ON_DESTROY) {
					_OnDestroyFunction = [](ScriptComponent* _instance) {
						static_cast<T*>(_instance)->OnDestroy();
					};
				}
			}

			template <typename T>
			bool HasComponent() {
				for (Component* component : m_components) {
					if (dynamic_cast<T*>(component))
						return true;
				}

				return false;
			}

			template <typename T>
			void AddComponent() {
				if (!HasComponent<T>()) {
					m_components.emplace_back();
				}
			}


			template <typename T>
			void RemoveComponent() {
				for (auto it = m_components.begin(); it != m_components.end(); it++) {
					if (dynamic_cast<T*>(*it)) {
						m_components.erase(it);
						break;
					}
				}
			}

			template <typename T>
			T* GetComponent() {
				for (auto it = m_components.begin(); it != m_components.end(); it++) {
					if (dynamic_cast<T*>(*it)) {
						return dynamic_cast<T*>(*it)
					}
				}
			}
	};
}

#endif
