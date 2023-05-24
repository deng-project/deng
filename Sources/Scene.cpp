// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Scene.cpp - scene class implementation
// author: Karl-Mihkel Ott

#define SCENE_CPP
#include "deng/Scene.h"

namespace DENG {

	Scene::Scene(IFramebuffer* _pFramebuffer) :
		m_pFramebuffer(_pFramebuffer) {}


	void Scene::AttachComponents() {
		auto view = m_registry.view<ScriptComponent>();

		for (Entity idScriptEntity : view) {
			ScriptComponent& scriptComponent = m_registry.get<ScriptComponent>(idScriptEntity);
			if (scriptComponent.OnAttach)
				scriptComponent.OnAttach(scriptComponent);
		}
	}


	void Scene::RenderScene(IRenderer* _pRenderer) {
		std::chrono::duration<float, std::milli> frametime;
		// step 1: update scripts
		{
			auto view = m_registry.view<ScriptComponent>();
			for (Entity idScript : view) {
				ScriptComponent& script = m_registry.get<ScriptComponent>(idScript);
				if (script.OnUpdate) {
					m_tpEnd = std::chrono::high_resolution_clock::now();
					frametime = m_tpEnd - m_tpBegin;
					script.OnUpdate(script, frametime.count());
				}
			}
		}

		// step 2: render meshes
		{
			auto view = m_registry.view<MeshComponent, ShaderComponent>();
			for (Entity idMesh : view) {
				auto [mesh, shader] = m_registry.get<MeshComponent, ShaderComponent>(idMesh);
				_pRenderer->DrawMesh(mesh, shader, m_pFramebuffer, {});
			}
		}
		m_tpBegin = std::chrono::high_resolution_clock::now();
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