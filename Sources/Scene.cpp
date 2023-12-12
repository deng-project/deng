// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Scene.cpp - scene class implementation
// author: Karl-Mihkel Ott

#define SCENE_CPP
#include <queue>
#include "deng/Scene.h"

namespace DENG {

	Scene::Scene(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer) :
		m_sceneRenderer(_pRenderer, _pFramebuffer, m_registry) 
	{
		EventManager& eventManager = EventManager::GetInstance();
		eventManager.AddListener<Scene, ComponentModifiedEvent>(&Scene::OnComponentModifiedEvent, this);
		// eventManager.AddListener<Scene, ResourceModifiedEvent>(&Scene::OnResourceModifiedEvent, this);
	}

	Scene::~Scene() {
		EventManager& eventManager = EventManager::GetInstance();
		eventManager.RemoveListener<Scene, ComponentModifiedEvent>(this);
		eventManager.RemoveListener<Scene, ResourceModifiedEvent>(this);
	}


	// void Scene::_CorrectLightResources() {
	// 	// check if light sources need to be recopied
	// 	if (m_bmCopyFlags & (RendererCopyFlagBit_CopyDirectionalLights | RendererCopyFlagBit_CopySpotLights | RendererCopyFlagBit_CopyPointLights)) {
	// 		_RenderLights();
	// 	}
	// 	else {
	// 		if (m_modifiedDirLights.size()) {
	// 			std::vector<std::pair<std::size_t, std::size_t>> dirLightUpdateAreas =
	// 				_MakeMemoryRegions(m_modifiedDirLights);
	// 
	// 			for (auto it = dirLightUpdateAreas.begin(); it != dirLightUpdateAreas.end(); it++) {
	// 				m_sceneRenderer.UpdateDirLightRegion(
	// 					m_lights.dirLights.data() + it->first,
	// 					it->first,
	// 					it->second - it->first + 1);
	// 			}
	// 		}
	// 
	// 		if (m_modifiedPointLights.size()) {
	// 			std::vector<std::pair<std::size_t, std::size_t>> ptLightUpdateAreas =
	// 				_MakeMemoryRegions(m_modifiedPointLights);
	// 
	// 			for (auto it = ptLightUpdateAreas.begin(); it != ptLightUpdateAreas.end(); it++) {
	// 				m_sceneRenderer.UpdatePointLightRegion(
	// 					m_lights.pointLights.data() + it->first,
	// 					it->first,
	// 					it->second - it->first + 1);
	// 			}
	// 		}
	// 
	// 		if (m_modifiedSpotLights.size()) {
	// 			std::vector<std::pair<std::size_t, std::size_t>> spotLightUpdateAreas =
	// 				_MakeMemoryRegions(m_modifiedSpotLights);
	// 
	// 			for (auto it = spotLightUpdateAreas.begin(); it != spotLightUpdateAreas.end(); it++) {
	// 				m_sceneRenderer.UpdateSpotLightRegion(
	// 					m_lights.spotLights.data() + it->first,
	// 					it->first,
	// 					it->second - it->first + 1);
	// 			}
	// 		}
	// 
	// 		m_modifiedDirLights.clear();
	// 		m_modifiedSpotLights.clear();
	// 		m_modifiedPointLights.clear();
	// 	}
	// }

	void Scene::_SortRenderableGroup() {
		auto group = m_registry.group<MeshPassComponent, MaterialComponent>();

		// assets
		using _Tuple = std::tuple<MeshPassComponent, MaterialComponent>;
		group.sort<MeshPassComponent, MaterialComponent>(
			[](const _Tuple& _t1, const _Tuple& _t2) {
				auto& [mesh1, material1] = _t1;
				auto& [mesh2, material2] = _t2;

				// mesh verification
				if (mesh1.uBatchId != mesh2.uBatchId)
					return mesh1.uBatchId < mesh2.uBatchId;
				else if (mesh1.uRelativeDrawOffset != mesh2.uRelativeDrawOffset)
					return mesh1.uRelativeDrawOffset < mesh2.uRelativeDrawOffset;

				// material verification
				if (material1.hshMaterial != material2.hshMaterial)
					return material1.hshMaterial < material2.hshMaterial;

				return false;
			});
	}

	glm::mat4 Scene::_CalculateTransformMatrix(Entity _ent)
	{
		TransformComponent& transform = m_registry.get<TransformComponent>(_ent);
		glm::mat4 mCustom(1.f);
		mCustom = glm::translate(mCustom, glm::vec3(transform.vTranslation));
		glm::quat qX = { FT::cos(transform.vRotation.x / 2.f), FT::sin(transform.vRotation.x / 2.f), 0.f, 0.f };
		glm::quat qY = { FT::cos(transform.vRotation.y / 2.f), 0.f, FT::sin(transform.vRotation.y / 2.f), 0.f };
		glm::quat qZ = { FT::cos(transform.vRotation.z / 2.f), 0.f, 0.f, FT::sin(transform.vRotation.z / 2.f) };
		mCustom *= (qX * qY * qZ);
		mCustom = glm::scale(mCustom, glm::vec3(transform.vScale));
		return mCustom;
	}


	void Scene::_SortHierarchies() {
		auto group = m_registry.group<HierarchyComponent, TransformComponent>();
		
		using _Tuple = std::tuple<HierarchyComponent>;
		group.sort<HierarchyComponent>(
			[](const HierarchyComponent& _t1, const HierarchyComponent& _t2) {
				// sort by parent such that entities with no parents would be 
				return _t1.idParent < _t2.idParent;
			}
		);
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
		_SortHierarchies();

		if (m_idSkybox != entt::null) {
			auto& skybox = m_registry.get<SkyboxComponent>(m_idSkybox);
			m_sceneRenderer.UpdateSkyboxScale(skybox.vScale);
		}
	}


	void Scene::RenderScene() {
		_UpdateScripts();
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
		// if hierarchies were changed, sort them again
		if (_event.GetComponentType() & ComponentType_Hierarchy) {
			_SortHierarchies();
		}

		// traverse transforms hierarchically
		if ((_event.GetComponentType() & ComponentType_Transform) && m_registry.any_of<HierarchyComponent>(_event.GetEntity())) {
			// traverse hierarchy tree
			std::queue<std::pair<Entity, glm::mat4>> treeQue;
			treeQue.push(std::make_pair(_event.GetEntity(), glm::mat4(1.0f)));

			while (!treeQue.empty()) {
				auto& pair = treeQue.front();
				HierarchyComponent& hier = m_registry.get<HierarchyComponent>(pair.first);

				// check if entity has transform component
				if (m_registry.any_of<TransformComponent>(pair.first)) {
					TransformComponent& transform = m_registry.get<TransformComponent>(pair.first);
					transform.mCustomTransform = pair.second;
					pair.second *= _CalculateTransformMatrix(pair.first);
				}

				// check children
				if (hier.idFirst != entt::null) {
					std::queue<Entity> neighbours;
					neighbours.push(hier.idFirst);
					while (!neighbours.empty()) {
						Entity neighbour = neighbours.front();
						if (m_registry.any_of<HierarchyComponent>(neighbour)) {
							HierarchyComponent& neighbourHier = m_registry.get<HierarchyComponent>(neighbour);
							if (neighbourHier.idNext != entt::null)
								neighbours.push(neighbourHier.idNext);
							treeQue.push(std::make_pair(neighbour, pair.second));
						}
						neighbours.pop();
					}
				}

				treeQue.pop();
			}
		}

		// update light sources
		if (_event.GetComponentType() & ComponentType_Light)
			m_sceneRenderer.UpdateLight(_event.GetEntity());
	}
}