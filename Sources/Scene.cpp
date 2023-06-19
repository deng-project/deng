// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Scene.cpp - scene class implementation
// author: Karl-Mihkel Ott

#define SCENE_CPP
#include "deng/Scene.h"

namespace DENG {

	Scene::Scene(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer) :
		m_sceneRenderer(_pRenderer, _pFramebuffer) {}


	void Scene::_InstanceRenderablesMSM() {
		m_instanceInfos.clear();
		m_transforms.clear();
		m_materials.clear();
		m_drawDescriptorIndices.clear();
		std::unordered_map<hash_t, size_t, NoHash> materialIndexLookup;

		ResourceManager& resourceManager = ResourceManager::GetInstance();

		auto group = m_registry.group<MeshComponent, ShaderComponent, MaterialComponent>();
		
		for (auto it = group.begin(); it != group.end(); it++) {
			auto& [mesh, shader, material] = group.get<MeshComponent, ShaderComponent, MaterialComponent>(*it);
			const bool bHasTransform = m_registry.any_of<TransformComponent>(*it);

			if (it != group.begin()) {
				auto itPrev = it - 1;
				auto& [prevMesh, prevShader, prevMaterial] = group.get<MeshComponent, ShaderComponent, MaterialComponent>(*itPrev);

				if (mesh == prevMesh && shader == prevShader && material == prevMaterial)
					m_instanceInfos.back().uInstanceCount++;
				else {
					m_instanceInfos.emplace_back();
					m_instanceInfos.back().hshMesh = mesh.hshMesh;
					m_instanceInfos.back().hshShader = shader.hshShader;
					m_instanceInfos.back().hshMaterial = material.hshMaterial;
				}
			}
			else {
				m_instanceInfos.emplace_back();
				m_instanceInfos.back().hshMesh = mesh.hshMesh;
				m_instanceInfos.back().hshShader = shader.hshShader;
				m_instanceInfos.back().hshMaterial = material.hshMaterial;
			}

			// material indexing
			if (materialIndexLookup.find(material.hshMaterial) == materialIndexLookup.end()) {
				auto pMaterial = resourceManager.GetMaterial(material.hshMaterial);
				DENG_ASSERT(pMaterial);

				m_materials.emplace_back(*pMaterial);
				materialIndexLookup[material.hshMaterial] = m_materials.size() - 1;
			}


			if (bHasTransform) {
				auto& transform = m_registry.get<TransformComponent>(*it);
				m_transforms.emplace_back(transform);
				m_drawDescriptorIndices.emplace_back(
					static_cast<int32_t>(m_transforms.size() - 1),
					static_cast<int32_t>(materialIndexLookup[material.hshMaterial]));
			}
			else {
				m_drawDescriptorIndices.emplace_back(-1, static_cast<int32_t>(materialIndexLookup[material.hshMaterial]));
			}
		}
	}

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
			
			_InstanceRenderablesMSM();
			m_sceneRenderer.RenderInstances(m_instanceInfos, m_transforms, m_materials, m_drawDescriptorIndices, camera);
		}
	}

	void Scene::AttachComponents() {
		// sort entities that have MeshComponent, ShaderComponent and MaterialComponent
		{
			auto group = m_registry.group<MeshComponent, ShaderComponent, MaterialComponent>();
			
			using _Tuple = std::tuple<MeshComponent, ShaderComponent, MaterialComponent>;
			group.sort<MeshComponent, ShaderComponent, MaterialComponent>(
				[](const _Tuple& _t1, const _Tuple& _t2) {
					auto& [mesh1, shader1, material1] = _t1;
					auto& [mesh2, shader2, material2] = _t2;

					// mesh verification
					if (mesh1.hshMesh != mesh2.hshMesh)
						return mesh1.hshMesh < mesh2.hshMesh;

					// shader verification
					if (shader1.hshShader != shader2.hshShader)
						return shader1.hshShader < shader2.hshShader;

					// material verification
					if (material1.hshMaterial != material2.hshMaterial)
						return material1.hshMaterial < material2.hshMaterial;

					return false;
				});
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

		_InstanceRenderablesMSM();
		m_sceneRenderer.UpdateStorageBuffers(m_transforms, m_materials, m_drawDescriptorIndices);
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