// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Scene.h - scene class header
// author: Karl-Mihkel Ott

#ifndef SCENE_H
#define SCENE_H

#include <chrono>
#include <type_traits>

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


		private:
			template <typename T>
			void _ApplyLightSourceTransforms() {
				auto view = m_registry.view<T, TransformComponent>();
				for (Entity idLight : view) {
					auto& [light, transform] = m_registry.get<T, TransformComponent>(idLight);
					const TRS::Matrix4<float> cmTranslation = {
					{ 1.f, 0.f, 0.f, transform.vTranslation.first },
					{ 0.f, 1.f, 0.f, transform.vTranslation.second },
					{ 0.f, 0.f, 1.f, transform.vTranslation.third },
					{ 0.f, 0.f, 0.f, 1.f }
					};

					const TRS::Quaternion qX(std::sinf(transform.vRotation.first / 2.f), 0.f, 0.f, std::cosf(transform.vRotation.first / 2.f));
					const TRS::Quaternion qY(0.f, std::sinf(transform.vRotation.second / 2.f), 0.f, std::cosf(transform.vRotation.second / 2.f));
					const TRS::Quaternion qZ(0.f, 0.f, std::sinf(transform.vRotation.third / 2.f), std::cosf(transform.vRotation.third / 2.f));
					const TRS::Matrix4<float> cmRotation = (qX * qY * qZ).ExpandToMatrix4();

					light.vPosition = cmRotation * cmTranslation * TRS::Vector4<float>(0.f, 0.f, 0.f, 1.f);
				}
			}

			template <typename T>
			void _ReadLightsToVector(std::vector<T>& _vec) {
				auto view = m_registry.view<T>();
				_vec.reserve(view.size());

				for (Entity idLight : view) {
					auto& light = m_registry.get<T>(idLight);
					_vec.push_back(light);
				}
			}

			void _UpdateScripts();
			void _RenderLights();
			void _DrawMeshes();

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