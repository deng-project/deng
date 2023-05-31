// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Scene.cpp - scene class implementation
// author: Karl-Mihkel Ott

#define SCENE_CPP
#include "deng/Scene.h"

namespace DENG {

	Scene::Scene(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer) :
		m_sceneRenderer(_pRenderer, _pFramebuffer) {}


	void Scene::AttachComponents() {
		// initialise all ScriptComponents
		{
			auto view = m_registry.view<ScriptComponent>();
			for (Entity idScriptEntity : view) {
				ScriptComponent& scriptComponent = m_registry.get<ScriptComponent>(idScriptEntity);
				if (scriptComponent.OnAttach)
					scriptComponent.OnAttach(scriptComponent);
			}
		}
	}


	void Scene::RenderScene() {
		static CameraComponent s_cameraComponent;
		std::chrono::duration<float, std::milli> frametime;
		// step 1: update scripts
		{
			auto view = m_registry.view<ScriptComponent>();
			for (Entity idScript : view) {
				ScriptComponent& script = m_registry.get<ScriptComponent>(idScript);
				if (script.OnUpdate) {
					m_tpEnd = std::chrono::high_resolution_clock::now();
					frametime = m_tpEnd - m_tpBegin;
					script.OnUpdate(script, frametime.count() / 1000.f);
				}
			}
		}

		m_tpBegin = std::chrono::high_resolution_clock::now();

		// step 2: render camera if it exists
		CameraComponent* pCamera = nullptr;
		if (m_idMainCamera != entt::null) {
			pCamera = &m_registry.get<CameraComponent>(m_idMainCamera);
			m_sceneRenderer.RenderCamera(*pCamera);
		}
		else {
			pCamera = &s_cameraComponent;
		}

		// step 3: apply transforms to light sources
		{
			auto view = m_registry.view<LightComponent, TransformComponent>();
			for (Entity idLight : view) {
				auto& [light, transform] = m_registry.get<LightComponent, TransformComponent>(idLight);
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

		// step 4: render lights
		{
			auto view = m_registry.view<LightComponent>();
			std::vector<LightComponent> lights;
			lights.reserve(view.size());

			for (Entity idLight : view) {
				LightComponent& light = m_registry.get<LightComponent>(idLight);
				lights.push_back(light);
			}

			m_sceneRenderer.RenderLights(lights, m_vAmbient);
		}

		// step 5: draw meshes with transform
		{
			auto view = m_registry.view<MeshComponent, ShaderComponent, MaterialComponent, TransformComponent>();
			for (Entity idMesh : view) {
				auto& [mesh, shader, material, transform] = m_registry.get<MeshComponent, ShaderComponent, MaterialComponent, TransformComponent>(idMesh);
				m_sceneRenderer.RenderMesh(mesh, *pCamera, shader, idMesh, material, transform);
			}
		}

		// step 6: draw meshes without transform but with material
		{
			auto view = m_registry.view<MeshComponent, ShaderComponent, MaterialComponent>(entt::exclude_t<TransformComponent>());
			for (Entity idMesh : view) {
				auto& [mesh, shader, material] = m_registry.get<MeshComponent, ShaderComponent, MaterialComponent>(idMesh);
				m_sceneRenderer.RenderMesh(mesh, *pCamera, shader, idMesh, material);
			}
		}

		// step 7: draw meshes without material but with transform
		{
			auto view = m_registry.view<MeshComponent, ShaderComponent, TransformComponent>(entt::exclude_t<MaterialComponent>());
			for (Entity idMesh : view) {
				auto& [mesh, shader, transform] = m_registry.get<MeshComponent, ShaderComponent, TransformComponent>(idMesh);
				m_sceneRenderer.RenderMesh(mesh, *pCamera, shader, idMesh, MaterialComponent(), transform);
			}
		}

		// step 8: draw meshes without material and transform
		{
			auto view = m_registry.view<MeshComponent, ShaderComponent>(entt::exclude_t<MaterialComponent, TransformComponent>());
			for (Entity idMesh : view) {
				auto& [mesh, shader] = m_registry.get<MeshComponent, ShaderComponent>(idMesh);
				m_sceneRenderer.RenderMesh(mesh, *pCamera, shader, idMesh, MaterialComponent());
			}
		}
	}


	void Scene::DestroyComponents() {
		auto view = m_registry.view<ScriptComponent>();
		for (Entity idScript : view) {
			ScriptComponent& script = m_registry.get<ScriptComponent>(idScript);
			if (script.OnDestroy) {
				script.OnDestroy(script);
			}
		}
	}
}