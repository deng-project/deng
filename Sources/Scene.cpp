// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Scene.cpp - scene class implementation
// author: Karl-Mihkel Ott

#define SCENE_CPP
#include "deng/Scene.h"

namespace DENG {

	Scene::Scene(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer) :
		m_sceneRenderer(_pRenderer, _pFramebuffer) 
	{
		EventManager& eventManager = EventManager::GetInstance();
		eventManager.AddListener<Scene, ComponentModifiedEvent>(&Scene::OnComponentModifiedEvent, this);
		eventManager.AddListener<Scene, ResourceModifiedEvent>(&Scene::OnResourceModifiedEvent, this);
	}

	Scene::~Scene() {
		EventManager& eventManager = EventManager::GetInstance();
		eventManager.RemoveListener<Scene, ComponentModifiedEvent>(this);
		eventManager.RemoveListener<Scene, ResourceModifiedEvent>(this);
	}


	std::vector<std::pair<std::size_t, std::size_t>> Scene::_MakeMemoryRegions(const std::set<std::size_t>& _updateSet) {
		std::vector<std::pair<std::size_t, std::size_t>> memoryRegions;
		std::size_t uPrevTransformId = 0;
		for (auto it = _updateSet.begin(); it != _updateSet.end(); it++) {
			if (it == _updateSet.begin())
				memoryRegions.emplace_back(std::make_pair(*it, *it));
			else if (*it - uPrevTransformId < 3)
				memoryRegions.back().second = *it;
			else
				memoryRegions.emplace_back(std::make_pair(*it, *it));

			uPrevTransformId = *it;
		}

		return memoryRegions;
	}

	void Scene::_CorrectMeshResources() {
		// check if meshes have to be reinstanced
		if (m_bmCopyFlags & RendererCopyFlagBit_Reinstance) {
			_InstanceRenderablesMSM();
		}
		else if (m_modifiedTransforms.size()) {
			std::vector<std::pair<std::size_t, std::size_t>> transformUpdateAreas =
				_MakeMemoryRegions(m_modifiedTransforms);

			for (auto it = transformUpdateAreas.begin(); it != transformUpdateAreas.end(); it++) {
				m_sceneRenderer.UpdateTransformRegion(
					m_instances.transforms.data() + it->first,
					it->first,
					it->second - it->first + 1);
			}

			m_modifiedTransforms.clear();
		}
	}

	void Scene::_CorrectLightResources() {

		// check if light sources need to be recopied
		if (m_bmCopyFlags & (RendererCopyFlagBit_CopyDirectionalLights | RendererCopyFlagBit_CopySpotLights | RendererCopyFlagBit_CopyPointLights)) {
			_RenderLights();
		}
		else {
			if (m_modifiedDirLights.size()) {
				std::vector<std::pair<std::size_t, std::size_t>> dirLightUpdateAreas =
					_MakeMemoryRegions(m_modifiedDirLights);

				for (auto it = dirLightUpdateAreas.begin(); it != dirLightUpdateAreas.end(); it++) {
					m_sceneRenderer.UpdateDirLightRegion(
						m_lights.dirLights.data() + it->first,
						it->first,
						it->second - it->first + 1);
				}
			}

			if (m_modifiedPointLights.size()) {
				std::vector<std::pair<std::size_t, std::size_t>> ptLightUpdateAreas =
					_MakeMemoryRegions(m_modifiedPointLights);

				for (auto it = ptLightUpdateAreas.begin(); it != ptLightUpdateAreas.end(); it++) {
					m_sceneRenderer.UpdatePointLightRegion(
						m_lights.pointLights.data() + it->first,
						it->first,
						it->second - it->first + 1);
				}
			}

			if (m_modifiedSpotLights.size()) {
				std::vector<std::pair<std::size_t, std::size_t>> spotLightUpdateAreas =
					_MakeMemoryRegions(m_modifiedSpotLights);

				for (auto it = spotLightUpdateAreas.begin(); it != spotLightUpdateAreas.end(); it++) {
					m_sceneRenderer.UpdateSpotLightRegion(
						m_lights.spotLights.data() + it->first,
						it->first,
						it->second - it->first + 1);
				}
			}

			m_modifiedDirLights.clear();
			m_modifiedSpotLights.clear();
			m_modifiedPointLights.clear();
		}
	}

	void Scene::_InstanceRenderablesMSM() {
		m_instances.instanceInfos.clear();
		m_instances.transforms.clear();
		m_instances.materials.clear();
		m_instances.drawDescriptorIndices.clear();
		m_renderableInstanceLookup.clear();
		std::unordered_map<hash_t, size_t, NoHash> materialIndexLookup;

		ResourceManager& resourceManager = ResourceManager::GetInstance();

		auto group = m_registry.group<MeshComponent, ShaderComponent, MaterialComponent>();
		
		for (auto it = group.begin(); it != group.end(); it++) {
			auto& [mesh, shader, material] = group.get<MeshComponent, ShaderComponent, MaterialComponent>(*it);
			const bool bHasTransform = m_registry.any_of<TransformComponent>(*it);

			if (it != group.begin()) {
				auto itPrev = it - 1;
				auto& [prevMesh, prevShader, prevMaterial] = group.get<MeshComponent, ShaderComponent, MaterialComponent>(*itPrev);

				if (mesh == prevMesh && shader == prevShader && material == prevMaterial) {
					m_renderableInstanceLookup[*it] = std::make_pair(
						m_instances.instanceInfos.size() - 1,
						static_cast<size_t>(m_instances.instanceInfos.back().uInstanceCount));
					m_instances.instanceInfos.back().uInstanceCount++;
				}
				else {
					m_instances.instanceInfos.emplace_back();
					m_instances.instanceInfos.back().hshMesh = mesh.hshMesh;
					m_instances.instanceInfos.back().hshShader = shader.hshShader;
					m_instances.instanceInfos.back().hshMaterial = material.hshMaterial;
					m_instances.instanceInfos.back().uInstanceCount++;
				
					m_renderableInstanceLookup[*it] = std::make_pair(
						m_instances.instanceInfos.size() - 1, 0);
				}
			}
			else {
				m_instances.instanceInfos.emplace_back();
				m_instances.instanceInfos.back().hshMesh = mesh.hshMesh;
				m_instances.instanceInfos.back().hshShader = shader.hshShader;
				m_instances.instanceInfos.back().hshMaterial = material.hshMaterial;
				m_instances.instanceInfos.back().uInstanceCount++;

				m_renderableInstanceLookup[*it] = std::make_pair(
					m_instances.instanceInfos.size() - 1, 0);
			}

			// material indexing
			if (material.hshMaterial && materialIndexLookup.find(material.hshMaterial) == materialIndexLookup.end()) {
				auto pMaterial = resourceManager.GetMaterial(material.hshMaterial);
				DENG_ASSERT(pMaterial);

				m_instances.materials.emplace_back(*pMaterial);
				materialIndexLookup[material.hshMaterial] = m_instances.materials.size() - 1;
			}

			if (bHasTransform) {
				auto& transform = m_registry.get<TransformComponent>(*it);
				transform.CalculateNormalMatrix();
				m_instances.transforms.emplace_back(transform);
				m_instances.drawDescriptorIndices.emplace_back(
					static_cast<int32_t>(m_instances.transforms.size() - 1),
					static_cast<int32_t>(materialIndexLookup[material.hshMaterial]));
			}
			else {
				m_instances.drawDescriptorIndices.emplace_back(-1, static_cast<int32_t>(materialIndexLookup[material.hshMaterial]));
			}
		}

		m_sceneRenderer.UpdateStorageBuffers(m_instances.transforms, m_instances.materials, m_instances.drawDescriptorIndices);
	}

	void Scene::_SortRenderableGroup() {
		auto group = m_registry.group<MeshComponent, ShaderComponent, MaterialComponent>();

		// assets
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
			m_sceneRenderer.RenderInstances(m_instances.instanceInfos, m_instances.transforms, m_instances.materials, m_instances.drawDescriptorIndices, camera);
			
			if (m_idSkybox != entt::null) {
				auto& skybox = m_registry.get<SkyboxComponent>(m_idSkybox);
				m_sceneRenderer.RenderSkybox(camera, skybox);
			}
		}
	}

	void Scene::AttachComponents() {
		{
			// initialise all ScriptComponents
			auto view = m_registry.view<ScriptComponent>();
			for (Entity idScriptEntity : view) {
				ScriptComponent& scriptComponent = m_registry.get<ScriptComponent>(idScriptEntity);
				if (scriptComponent.OnAttach)
					scriptComponent.OnAttach(scriptComponent);
			}
		}

		_SortRenderableGroup();
		_RenderLights();
		_InstanceRenderablesMSM();

		if (m_idSkybox != entt::null) {
			auto& skybox = m_registry.get<SkyboxComponent>(m_idSkybox);
			m_sceneRenderer.UpdateSkyboxScale(skybox.vScale);
		}
	}


	void Scene::RenderScene() {
		_UpdateScripts();
		_CorrectMeshResources();
		_CorrectLightResources();
		_DrawMeshes();
		m_bmCopyFlags = RendererCopyFlagBit_None;
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

	bool Scene::OnComponentModifiedEvent(ComponentModifiedEvent& _event) {
		// renderable transform modified
		if ((_event.GetComponentType() & ComponentType_Transform) && 
			!m_registry.any_of<DirectionalLightComponent, PointLightComponent, SpotlightComponent>(_event.GetEntity())) 
		{
			DENG_ASSERT(m_renderableInstanceLookup.find(_event.GetEntity()) != m_renderableInstanceLookup.end());
		
			const std::size_t uDrawCallId = m_renderableInstanceLookup[_event.GetEntity()].first;
			std::size_t uInstanceId = m_renderableInstanceLookup[_event.GetEntity()].second;
		
			for (std::size_t i = 0; i < uDrawCallId; i++) {
				uInstanceId += static_cast<std::size_t>(m_instances.instanceInfos[i].uInstanceCount);
			}

			auto& transform = m_registry.get<TransformComponent>(_event.GetEntity());
			transform.CalculateNormalMatrix();
			m_instances.transforms[uInstanceId] = transform;
			m_modifiedTransforms.insert(uInstanceId);
		}
		// other renderable component modified
		else if (_event.GetComponentType() & ComponentType_Renderable) {
			m_bmCopyFlags |= RendererCopyFlagBit_Reinstance;
		}
		// light modified
		else if (_event.GetComponentType() & ComponentType_Light) {
			DENG_ASSERT(m_lightLookup.find(_event.GetEntity()) != m_lightLookup.end());
			
			if (_event.GetComponentType() & ComponentType_DirectionalLight)
				m_modifiedDirLights.insert(m_lightLookup[_event.GetEntity()]);
			if (_event.GetComponentType() & ComponentType_PointLight)
				m_modifiedPointLights.insert(m_lightLookup[_event.GetEntity()]);
			if (_event.GetComponentType() & ComponentType_SpotLight)
				m_modifiedSpotLights.insert(m_lightLookup[_event.GetEntity()]);
		}
		// skybox modified
		else if (_event.GetComponentType() & ComponentType_Skybox) {
			auto& skybox = m_registry.get<SkyboxComponent>(_event.GetEntity());
			m_sceneRenderer.UpdateSkyboxScale(skybox.vScale);
		}

		return true;
	}

	bool Scene::OnResourceModifiedEvent(ResourceModifiedEvent& _event) {
		m_bmCopyFlags |= RendererCopyFlagBit_Reinstance;
		return true;
	}
}