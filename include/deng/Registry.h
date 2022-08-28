// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Registry.h - Game entity and component registry
// author: Karl-Mihkel Ott

#ifndef REGISTRY_H
#define REGISTRY_H

#ifdef REGISTRY_CPP
	#include <vector>
	#include <string>
	#include <chrono>

#ifdef _WIN32
	#include <wtypes.h>
#endif

	#include "trs/Points.h"
	#include "trs/Vector.h"
	#include "trs/Matrix.h"
	#include "trs/Quaternion.h"

	#include "deng/Api.h"
	#include "deng/BaseTypes.h"
	#include "deng/ErrorDefinitions.h"
	#include "deng/Entity.h"
	#include "deng/ScriptableEntity.h"
	#include "deng/ShaderDefinitions.h"
	#include "deng/ModelUniforms.h"
	#include "deng/Renderer.h"
	#include "deng/Camera3D.h"
	#include "deng/EditorCamera.h"
	#include "deng/FirstPersonCamera.h"
	#include "deng/GridGenerator.h"
#endif

namespace DENG {

	class DENG_API Registry {
		private:
			std::vector<Entity*> m_entities;
			static Registry* m_instance;

		private:
			Registry() = default;

		public:
			static Registry* GetInstance();
			static void DeleteInstance();

			inline uint32_t AddEntity(Entity *_ent) {
				m_entities.push_back(_ent);
				return static_cast<uint32_t>(m_entities.size() - 1);
			}

			inline void RemoveEntity(uint32_t _id) {
				DENG_ASSERT(_id < static_cast<uint32_t>(m_entities.size()));
				m_entities.erase(m_entities.begin() + _id);
			}


			void AttachEntities();
			void Update();

			// O(n)
			uint32_t FindIdByName(const std::string &_name);

			inline Entity* GetEntityById(uint32_t _id) {
				DENG_ASSERT(_id < static_cast<uint32_t>(m_entities.size()));
				return m_entities[_id];
			}
	};
}

#endif