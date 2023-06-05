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
			
			// every drawable mesh needs to have following components:
			// MeshComponent, ShaderComponent, MaterialComponent, TransformComponent
			auto group = m_registry.group<MeshComponent, ShaderComponent, MaterialComponent, TransformComponent>();

			std::vector<MaterialComponent> materials;
			std::vector<TransformComponent> transforms;

			for (auto it = group.begin(); it != group.end(); it++) {
				auto& [mesh, shader, transform, material] = group.get<MeshComponent, ShaderComponent, TransformComponent, MaterialComponent>(*it);

				if (it != group.begin()) {
					auto itPrev = it - 1;
					auto& [prevMesh, prevShader] = group.get<MeshComponent, ShaderComponent>(*itPrev);

					if (prevMesh == mesh && prevShader == shader) {
						materials.push_back(material);
						transforms.push_back(transform);
					}
					else {
						// render the previous batch
						m_sceneRenderer.RenderBatch(prevMesh, prevShader, materials, transforms, camera);
						materials.clear();
						transforms.clear();
						materials.push_back(material);
						transforms.push_back(transform);
					}
				}
				else {
					materials.push_back(material);
					transforms.push_back(transform);
				}
			}

			if (materials.size() && transforms.size()) {
				auto it = group.rbegin();
				auto& [mesh, shader] = group.get<MeshComponent, ShaderComponent>(*it);
				m_sceneRenderer.RenderBatch(mesh, shader, materials, transforms, camera);
			}

			m_sceneRenderer.Finalize();
		}
	}

	void Scene::AttachComponents() {
		// calculate checksums for all mesh components
		{
			auto group = m_registry.group<MeshComponent, ShaderComponent>();
			group.sort<MeshComponent, ShaderComponent>(
				[](std::tuple<MeshComponent&, ShaderComponent&> t1, std::tuple<MeshComponent&, ShaderComponent&> t2) {
					auto& mesh1 = std::get<MeshComponent&>(t1);
					auto& mesh2 = std::get<MeshComponent&>(t2);
					auto& shader1 = std::get<ShaderComponent&>(t1);
					auto& shader2 = std::get<ShaderComponent&>(t2);

					// mesh verification
					if (mesh1.uDrawCommandHash != mesh2.uDrawCommandHash)
						return mesh1.uDrawCommandHash < mesh2.uDrawCommandHash;

					// shader verification
					if (shader1.pShader != shader2.pShader)
						return shader1.pShader < shader2.pShader;

					return false;
				});

			auto view = m_registry.view<MeshComponent>();
			for (Entity idMesh : view) {
				MeshComponent& mesh = m_registry.get<MeshComponent>(idMesh);
				mesh.uDrawCommandHash = MeshComponent::_Hash()(mesh);
			}
		}

		{
			// initialise all ScriptComponents
			auto view = m_registry.view<ScriptComponent>();
			for (Entity idScriptEntity : view) {
				ScriptComponent& scriptComponent = m_registry.get<ScriptComponent>(idScriptEntity);
				if (scriptComponent.OnAttach)
					scriptComponent.OnAttach(scriptComponent);
			}
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