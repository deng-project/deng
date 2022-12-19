// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Registry.h - Game entity and component registry
// author: Karl-Mihkel Ott

#ifndef REGISTRY_H
#define REGISTRY_H

#ifdef REGISTRY_CPP
	#include <any>
	#include <vector>
	#include <string>
	#include <chrono>
	#include <fstream>
	#include <unordered_map>
	#include <array>
	
	#include <python3.10/pyconfig.h>
	#include <pybind11/embed.h>
#ifdef _WIN32
	#include <wtypes.h>
#endif

	#include "trs/Points.h"
	#include "trs/Vector.h"
	#include "trs/Matrix.h"
	#include "trs/Quaternion.h"

	#include "mar/AsciiStreamReader.h"
	#include "mar/AsciiLineReader.h"

	#include "das/Api.h"
	#include "das/ErrorHandlers.h"
	#include "das/DasStructures.h"
	#include "das/DasReaderCore.h"
	#include "das/DasParser.h"
	#include "das/TextureReader.h"

	#include "dxml/GameStructs.h"

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
	#include "deng/Skybox.h"
#ifdef __DEBUG
	#include "deng/Debug/TriangleTester.h"
#endif
	
	#include "deng/VertexNormalVisualizer.h"
	#include "deng/MeshLoader.h"
	#include "deng/AnimationSampler.h"
	#include "deng/SkeletonLoader.h"
	#include "deng/NodeLoader.h"
	#include "deng/SceneLoader.h"
	#include "deng/ModelLoader.h"
#endif

namespace DENG {

	class DENG_API Registry {
		private:
			std::vector<Entity*> m_entities;
			DXML::GraphicsBackend m_backend = DXML::GRAPHICS_BACKEND_UNKNOWN;
			uint32_t m_msaa = 1;
			pybind11::dict m_custom_data;

			static Registry* m_instance;

		private:
			Registry() = default;

			friend class Entity;
			void Replace(uint32_t _id, Entity* _new);

		public:
			static Registry* GetInstance();
			static void DeleteInstance();

			uint32_t AddEntity(Entity* _ent);

			inline void RemoveEntity(uint32_t _id) {
				DENG_ASSERT(_id < static_cast<uint32_t>(m_entities.size()));
				m_entities[_id] = nullptr;
			}


			void AttachEntities();
			void Update();

			// O(n)
			uint32_t FindIdByName(const std::string &_name);
			Entity* GetFirstEntityByType(EntityType _type);
			Entity* GetLastEntityByType(EntityType _type);

			inline Entity* GetEntityById(uint32_t _id) {
				DENG_ASSERT(_id < static_cast<uint32_t>(m_entities.size()));
				return m_entities[_id];
			}

			inline void SetGraphicsBackend(DXML::GraphicsBackend _backend) {
				m_backend = _backend;
			}

			inline void SetMSAA(uint32_t _msaa) {
				m_msaa = _msaa;
			}

			inline void SetCustomData(const pybind11::dict& _data) {
				m_custom_data = _data;
			}

			inline DXML::GraphicsBackend GetGraphicsBackend() {
				return m_backend;
			}

			inline uint32_t GetMSAA() {
				return m_msaa;
			}

			inline pybind11::dict& GetCustomData() {
				return m_custom_data;
			}
	};
}

#endif