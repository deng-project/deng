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
		if (m_idMainCamera != entt::null) {
			CameraComponent& camera = m_registry.get<CameraComponent>(m_idMainCamera);
			m_sceneRenderer.RenderCamera(camera);
		}

		// step 3: render lights
		{
			auto view = m_registry.view<LightComponent>();
			std::vector<LightComponent> lights;
			lights.reserve(view.size());

			for (Entity idLight : view) {
				LightComponent& light = m_registry.get<LightComponent>(idLight);
				lights.push_back(light);
			}

			m_sceneRenderer.RenderLights(lights);
		}

		// step 4: draw meshes with transform
		{
			auto view = m_registry.view<MeshComponent, ShaderComponent, TransformComponent>();
			for (Entity idMesh : view) {
				auto& [mesh, shader, transform] = m_registry.get<MeshComponent, ShaderComponent, TransformComponent>(idMesh);
				m_sceneRenderer.RenderMesh(mesh, shader, idMesh, transform);
			}
		}

		// step 5: draw meshes without transform
		{
			auto view = m_registry.view<MeshComponent, ShaderComponent>(entt::exclude_t<TransformComponent>());
			for (Entity idMesh : view) {
				auto& [mesh, shader] = m_registry.get<MeshComponent, ShaderComponent>(idMesh);
				m_sceneRenderer.RenderMesh(mesh, shader, idMesh);
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