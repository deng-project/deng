// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Scene.h - scene class header
// author: Karl-Mihkel Ott

#ifndef SCENE_H
#define SCENE_H

#include <chrono>

#include "deng/Api.h"
#include "deng/IRenderer.h"
#include "deng/SceneRenderer.h"

#ifdef SCENE_CPP
	#include "deng/ErrorDefinitions.h"
#endif

namespace DENG {

	typedef entt::entity Entity;

	class DENG_API Scene {
		private:
			SceneRenderer m_sceneRenderer;

			entt::registry m_registry;
			Entity m_idMainCamera = entt::null;

			std::chrono::time_point<std::chrono::high_resolution_clock> m_tpBegin =
				std::chrono::high_resolution_clock::now();
			std::chrono::time_point<std::chrono::high_resolution_clock> m_tpEnd =
				std::chrono::high_resolution_clock::now();

			TRS::Vector3<float> m_vAmbient = { 0.3f, 0.3f, 0.3f };

		public:
			Scene(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer);

			inline Entity CreateEntity() {
				return m_registry.create();
			}

			template<typename T, typename ...Args>
			inline T& EmplaceComponent(Entity _idEntity, Args... args) {
				return m_registry.emplace<T>(_idEntity, std::forward(args)...);
			}

			template<typename T>
			inline T& GetComponent(Entity _idEntity) {
				return m_registry.get<T>(_idEntity);
			}

			inline void SetMainCamera(Entity _idEntity) {
				m_idMainCamera = _idEntity;
			}

			inline entt::registry& GetRegistry() {
				return m_registry;
			}

			inline const TRS::Vector3<float>& GetAmbient() const {
				return m_vAmbient;
			}

			inline TRS::Vector3<float>& GetAmbient() {
				return m_vAmbient;
			}

			inline void SetAmbient(const TRS::Vector3<float>& _vAmbient) {
				m_vAmbient = _vAmbient;
			}

			void AttachComponents();
			void RenderScene();
			void DestroyComponents();
	};
}

#endif