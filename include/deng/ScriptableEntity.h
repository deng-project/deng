// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ScriptableEntity.h - ScriptableEntity class definition header
// author: Karl-Mihkel Ott

#ifndef SCRIPTABLE_ENTITY_H
#define SCRIPTABLE_ENTITY_H

#include <functional>
#include <string>
#include "deng/Entity.h"

namespace DENG {

	// NOTE: Each scriptable entity can have one instance of ScriptComponent
	class DENG_API ScriptComponent {
		protected:
			Entity *m_scriptable_entity;
		
		public:
			ScriptComponent(Entity *_scriptable_entity) :
				m_scriptable_entity(_scriptable_entity) {}
			
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

	class DENG_API ScriptableEntity : public Entity {
		protected:
			ScriptComponent* m_script = nullptr;
			std::function<void(ScriptComponent*)> _OnAttachFunction = nullptr;
			std::function<void(ScriptComponent*)> _OnUpdateFunction = nullptr;
			std::function<void(ScriptComponent*)> _OnDestroyFunction = nullptr;

		public:
			ScriptableEntity(Entity *_parent, const std::string &_name, EntityType _type) :
				Entity(_parent, _name, _type) {}
			~ScriptableEntity() {
				delete m_script;
			}

			template<typename T, typename... Types>
			void BindScript(Types... args) {
				m_script = new T(this, args...);
				_OnAttachFunction = [](ScriptComponent *_instance) { ((T*)_instance)->OnAttach(); };
				_OnUpdateFunction = [](ScriptComponent *_instance) { ((T*)_instance)->OnUpdate(); };
				_OnDestroyFunction = [](ScriptComponent* _instance) { ((T*)_instance)->OnDestroy(); };
			}
	};
}

#endif