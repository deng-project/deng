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

	class DENG_API ScriptableEntity : public Entity {
		protected:
			ScriptComponent* m_script = nullptr;
			std::function<void(ScriptComponent*)> _OnAttachFunction = nullptr;
			std::function<void(ScriptComponent*)> _OnUpdateFunction = nullptr;
			std::function<void(ScriptComponent*)> _OnDestroyFunction = nullptr;

		public:
			ScriptableEntity(Entity *_parent, const std::string &_name, EntityType _type) :
				Entity(_parent, _name, _type) {}
			ScriptableEntity(const ScriptableEntity& _se) = delete;
			ScriptableEntity(ScriptableEntity&& _se) noexcept :
				Entity(std::move(_se)),
				m_script(_se.m_script),
				_OnAttachFunction(_se._OnAttachFunction),
				_OnUpdateFunction(_se._OnUpdateFunction),
				_OnDestroyFunction(_se._OnDestroyFunction)
			{
				m_script = nullptr;
			}

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