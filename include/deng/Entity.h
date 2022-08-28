// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Entity.h - Generic entity structure definition header
// author: Karl-Mihkel Ott

#ifndef ENTITY_H
#define ENTITY_H

#ifdef ENTITY_CPP
	#include <string>
	#include <vector>

	#include "deng/Api.h"
	#include "deng/ErrorDefinitions.h"
	namespace DENG {
		class Entity;
	}
	#include "deng/Registry.h"
#endif

namespace DENG {

	enum EntityType {
		ENTITY_TYPE_LEVEL,
		ENTITY_TYPE_EDITOR_CAMERA,
		ENTITY_TYPE_THIRD_PERSON_CAMERA,
		ENTITY_TYPE_FIRST_PERSON_CAMERA,
		ENTITY_TYPE_LIGHT_SOURCE_POINT,
		ENTITY_TYPE_LIGHT_SOURCE_DIRECTIONAL,
		ENTITY_TYPE_LIGHT_SOURCE_AMBIENT,
		ENTITY_TYPE_MODEL,
		ENTITY_TYPE_NODE,
		ENTITY_TYPE_SKELETON,
		ENTITY_TYPE_SKELETON_JOINT,
		ENTITY_TYPE_ANIMATION,
		ENTITY_TYPE_SCRIPT,
		ENTITY_TYPE_MESH,
		ENTITY_TYPE_GRID_MESH
	};

	class DENG_API Entity {
		private:
			Entity* m_parent;
			std::string m_name;
			const EntityType m_type;
			uint32_t m_registry_id = UINT32_MAX;

		public:
			Entity(Entity* _parent, const std::string& _name, const EntityType _type);
			~Entity();

			inline Entity* GetParent() {
				return m_parent;
			}

			inline const std::string& GetName() {
				return m_name;
			}

			inline const EntityType GetType() {
				return m_type;
			}

			inline uint32_t GetId() {
				return m_registry_id;
			}
	};
}

#endif