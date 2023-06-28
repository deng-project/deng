// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Scene.h - scene class header
// author: Karl-Mihkel Ott

#ifndef SCENE_H
#define SCENE_H

#include <chrono>
#include <future>
#include <type_traits>
#include <set>

#include "deng/Api.h"
#include "deng/IRenderer.h"
#include "deng/SceneRenderer.h"
#include "deng/IRenderer.h"
#include "deng/IShader.h"
#include "deng/RenderResources.h"
#include "deng/SceneEvents.h"

#ifdef SCENE_CPP
	#include "deng/ErrorDefinitions.h"
#endif

namespace DENG {

	typedef entt::entity Entity;

	struct Instances {
		std::vector<InstanceInfo> instanceInfos;
		std::vector<TransformComponent> transforms;
		std::vector<Material> materials;
		std::vector<DrawDescriptorIndices> drawDescriptorIndices;
	};

	struct Lights {
		std::vector<DirectionalLightComponent> dirLights;
		std::vector<PointLightComponent> pointLights;
		std::vector<SpotlightComponent> spotLights;
	};

	enum RendererCopyFlagBits_T : uint8_t {
		RendererCopyFlagBit_None = 0,
		RendererCopyFlagBit_Reinstance = (1 << 0),
		RendererCopyFlagBit_CopyDirectionalLights = (1 << 1),
		RendererCopyFlagBit_CopySpotLights = (1 << 2),
		RendererCopyFlagBit_CopyPointLights = (1 << 3)
	};

	typedef uint8_t RendererCopyFlagBits;

	class DENG_API Scene {
		private:
			SceneRenderer m_sceneRenderer;

			entt::registry m_registry;
			Entity m_idMainCamera = entt::null;
			Entity m_idSkybox = entt::null;

			Instances m_instances;
			Lights m_lights;

			std::unordered_map<Entity, std::pair<std::size_t, std::size_t>> m_renderableInstanceLookup;
			std::set<std::size_t> m_modifiedTransforms;

			std::unordered_map<Entity, std::size_t> m_lightLookup;
			std::set<std::size_t> m_modifiedDirLights;
			std::set<std::size_t> m_modifiedSpotLights;
			std::set<std::size_t> m_modifiedPointLights;

			std::chrono::time_point<std::chrono::high_resolution_clock> m_tpBegin =
				std::chrono::high_resolution_clock::now();
			std::chrono::time_point<std::chrono::high_resolution_clock> m_tpEnd =
				std::chrono::high_resolution_clock::now();

			TRS::Vector3<float> m_vAmbient = { 0.3f, 0.3f, 0.3f };

			RendererCopyFlagBits m_bmCopyFlags = RendererCopyFlagBit_None;

		private:
			template <typename T>
			void _ApplyLightSourceTransforms() {
				auto view = m_registry.view<T, TransformComponent>();
				for (Entity idLight : view) {
					_ApplyLightSourceTransform<T>(idLight);
				}
			}

			template <typename T>
			void _ApplyLightSourceTransform(Entity _idEntity) {
				auto& [light, transform] = m_registry.get<T, TransformComponent>(_idEntity);
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

			template <typename T>
			void _ReadLightsToVector(std::vector<T>& _vec) {
				auto view = m_registry.view<T>();
				_vec.reserve(view.size());

				for (Entity idLight : view) {
					auto& light = m_registry.get<T>(idLight);
					_vec.push_back(light);
				}
			}

			std::vector<std::pair<std::size_t, std::size_t>> _MakeMemoryRegions(const std::set<std::size_t>& _updateSet);
			void _CorrectMeshResources();
			void _CorrectLightResources();
			void _InstanceRenderablesMSM();
			void _SortRenderableGroup();
			void _UpdateScripts();
			void _RenderLights();
			void _DrawMeshes();

		public:
			Scene(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer);

			inline Entity CreateEntity() {
				return m_registry.create();
			}

			template<typename T, typename ...Args>
			inline T& EmplaceComponent(Entity _idEntity, Args&&... args) {
				return m_registry.emplace<T>(_idEntity, std::forward<Args>(args)...);
			}

			template<typename T>
			inline T& GetComponent(Entity _idEntity) {
				return m_registry.get<T>(_idEntity);
			}

			inline void SetMainCamera(Entity _idEntity) {
				m_idMainCamera = _idEntity;
			}

			inline void SetSkybox(Entity _idEntity) {
				m_idSkybox = _idEntity;
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

			bool OnComponentModifiedEvent(ComponentModifiedEvent& _event);
	};
}

#endif