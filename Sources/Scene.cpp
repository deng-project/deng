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
			
			_ViewAndRender<MeshComponent>();
			_ViewAndRender<Ref<MeshComponent>>();

			m_sceneRenderer.SubmitBatches(camera);
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