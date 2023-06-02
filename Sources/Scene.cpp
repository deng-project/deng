// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Scene.cpp - scene class implementation
// author: Karl-Mihkel Ott

#define SCENE_CPP
#include "deng/Scene.h"

namespace DENG {

	Scene::Scene(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer) :
		m_sceneRenderer(_pRenderer, _pFramebuffer) {}


	void Scene::_UpdateScripts() {
		std::chrono::duration<float, std::milli> frametime;
		
		auto view = m_registry.view<ScriptComponent>();
		for (Entity idScript : view) {
			ScriptComponent& script = m_registry.get<ScriptComponent>(idScript);
			if (script.OnUpdate) {
				m_tpEnd = std::chrono::high_resolution_clock::now();
				frametime = m_tpEnd - m_tpBegin;
				script.OnUpdate(script, frametime.count() / 1000.f);
			}
		}

		m_tpBegin = std::chrono::high_resolution_clock::now();
	}

	void Scene::_RenderLights() {
		std::vector<PointLightComponent> pointLights;
		std::vector<DirectionalLightComponent> dirLights;
		std::vector<SpotlightComponent> spotLights;

		_ReadLightsToVector(pointLights);
		_ReadLightsToVector(dirLights);
		_ReadLightsToVector(spotLights);

		m_sceneRenderer.RenderLights(pointLights, dirLights, spotLights, m_vAmbient);
	}

	void Scene::_DrawMeshes() {
		if (m_idMainCamera != entt::null) {
			CameraComponent& camera = m_registry.get<CameraComponent>(m_idMainCamera);
			// draw meshes with transform and material
			{
				auto view = m_registry.view<MeshComponent, ShaderComponent, MaterialComponent, TransformComponent>();
				for (Entity idMesh : view) {
					auto& [mesh, shader, material, transform] = m_registry.get<MeshComponent, ShaderComponent, MaterialComponent, TransformComponent>(idMesh);
					m_sceneRenderer.RenderMesh(mesh, shader, idMesh, material, transform, camera);
				}
			}

			// draw meshes without transform but with material
			{
				auto view = m_registry.view<MeshComponent, ShaderComponent, MaterialComponent>(entt::exclude_t<TransformComponent>());
				for (Entity idMesh : view) {
					auto& [mesh, shader, material] = m_registry.get<MeshComponent, ShaderComponent, MaterialComponent>(idMesh);
					m_sceneRenderer.RenderMesh(mesh, shader, idMesh, material, TransformComponent(), camera);
				}
			}

			// draw meshes without material but with transform
			{
				auto view = m_registry.view<MeshComponent, ShaderComponent, TransformComponent>(entt::exclude_t<MaterialComponent>());
				for (Entity idMesh : view) {
					auto& [mesh, shader, transform] = m_registry.get<MeshComponent, ShaderComponent, TransformComponent>(idMesh);
					m_sceneRenderer.RenderMesh(mesh, shader, idMesh, MaterialComponent(), transform, camera);
				}
			}

			// draw meshes without material nor transform
			{
				auto view = m_registry.view<MeshComponent, ShaderComponent>(entt::exclude_t<MaterialComponent, TransformComponent>());
				for (Entity idMesh : view) {
					auto& [mesh, shader] = m_registry.get<MeshComponent, ShaderComponent>(idMesh);
					m_sceneRenderer.RenderMesh(mesh, shader, idMesh, MaterialComponent(), TransformComponent(), camera);
				}
			}
		}
	}

	void Scene::AttachComponents() {
		// initialise all ScriptComponents
		auto view = m_registry.view<ScriptComponent>();
		for (Entity idScriptEntity : view) {
			ScriptComponent& scriptComponent = m_registry.get<ScriptComponent>(idScriptEntity);
			if (scriptComponent.OnAttach)
				scriptComponent.OnAttach(scriptComponent);
		}
	}


	void Scene::RenderScene() {
		_UpdateScripts();
		_ApplyLightSourceTransforms<PointLightComponent>();
		_ApplyLightSourceTransforms<SpotlightComponent>();
		_RenderLights();
		_DrawMeshes();
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