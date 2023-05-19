// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Scene.h - scene class header
// author: Karl-Mihkel Ott

#ifndef SCENE_H
#define SCENE_H

#define SCENE_CPP
#ifdef SCENE_CPP
	#include <list>
	
	#include "deng/Api.h"
	#include "deng/Components.h"
	//#include "deng/ComponentSystems.h"
	#include "deng/ShaderDefinitions.h"
	#include "deng/IRenderer.h"
#endif

#include <entt/entt.hpp>
typedef entt::entity Entity;

namespace DENG {
	
	struct EntityRelation {
		EntityRelation() = default;
		EntityRelation(const EntityRelation&) = default;
		EntityRelation(Entity _parent) : parent(_parent) {}

		Entity parent{ entt::null };

		operator Entity() { return parent; }
	};

	class DENG_API Scene {
		private:
			std::string m_sName = "MyScene";
			entt::registry m_registry;

		public:
			Scene() = default;
			Scene(const Scene&) = default;
			Scene(Scene&& _scene) noexcept;

			void RenderScene(IRenderer& _renderer);

			inline Entity AddEntity(Entity _parent = entt::null) {
				Entity ent = m_registry.create();
				
				if (_parent != entt::null)
					m_registry.emplace<EntityRelation>(ent, _parent);
				
				return ent;
			}

			template <typename T>
			inline void AddComponent(Entity _entity, T& _component) {
				m_registry.emplace<T>(_entity, _component);
			}

			template<typename T>
			inline T* TryGetEntityComponent(Entity _uId) {
				return m_registry.try_get<T>(_uId);
			}

			template<typename T>
			inline auto View() {
				return m_registry.view<T>();
			}
	};

}

#endif
