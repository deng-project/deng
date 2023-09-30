// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: DasExternalComponentizer.h - header for das2 format componentizer class
// author: Karl-Mihkel Ott

#pragma once

#include "deng/Api.h"
#include "deng/Scene.h"
#include <das2/DasStructures.h>

namespace DENG {

	enum StructureArrayId {
		StructureArrayId_Meshes				= 0,
		StructureArrayId_MeshGroups			= 1,
		StructureArrayId_Nodes				= 2,
		StructureArrayId_Scenes				= 3,
		StructureArrayId_SkeletonJoints		= 4,
		StructureArrayId_Skeletons			= 5,
		StructureArrayId_Animations			= 6,
		StructureArrayId_AnimationChannels	= 7,
		StructureArrayId_PhongMaterials		= 8,
		StructureArrayId_PbrMaterials		= 9,
		StructureArrayId_Count				= 10
	};

	class DENG_API DasExternalComponentizer {
		private:
			static uint32_t m_uUnnamedSceneCount;
			static uint32_t m_uUnnamedNodeCount;
			static uint32_t m_uUnnamedMeshGroupCount;
			static uint32_t m_uUnnamedSkeletonJointCount;
			static uint32_t m_uUnnamedSkeletonCount;
			static uint32_t m_uUnnamedAnimationCount;

			das2::Model m_model;
			Scene& m_scene;

			std::array<std::vector<Entity>, StructureArrayId_Count> m_structureIdTranslationTable;

		private:
			void _CreateEntities(const std::vector<das2::Mesh>& _meshes);
			void _CreateEntities(const std::vector<das2::MorphTarget>& _morphTargets);
			void _CreateEntities(const std::vector<das2::MeshGroup>& _meshGroups);
			void _CreateEntities(const std::vector<das2::Node>& _nodes);
			void _CreateEntities(const std::vector<das2::SkeletonJoint>& _joints);

			template <typename T>
			void _CreateEntities(const std::vector<T>& _roots, Entity _idModel) {
				for (auto it = _roots.begin(); it != _roots.end(); it++) {
					Entity idRoot = m_scene.CreateEntity();
					constexpr StructureArrayId eCurrentTableId;
					constexpr StructureArrayId eChildrenTableId;
					std::size_t uChildrenCount = 0;
					uint32_t* pChildren = nullptr;

					if constexpr (std::is_same<T, das2::Scene>::value) {
						m_scene.EmplaceComponent<NameComponent>().sName =
							(it->szName.Length() ? it->szName.CString() : "Unnamed scene " + std::to_string(m_uUnnamedSceneCount++);
						uChildrenCount = it->rootNodes.size();
						pChildren = it->rootNodes.data();

						eCurrentTableId = StructureArrayId_Scenes;
						eChildrenTableId = StructureArrayId_Nodes;
					}
					else if constexpr (std::is_same<T, das2::Skeleton>::value) {
						m_scene.EmplaceComponent<NameComponent>().sName =
							(it->szName.Length() ? it->szName.CString() : "Unnamed skeleton " + std::to_string(m_uUnnamedSkeletonCount++));
						uChildrenCount = 1;
						pChildren = &it->uParent;

						eCurrentTableId = StructureArrayId_Skeletons;
						eChildrenTableId = StructureArrayId_SkeletonJoints;
					}
					else if constexpr (std::is_same<T, das2::Animation>::value) {
						m_scene.EmplaceComponent<NameComponent>().sName =
							(it->szName.Length() ? it->szName.CString() : "Unnamed animation " + std::to_string(m_uUnnamedAnimationCount++));
						uChildrenCount = it->animationChannels.size();
						pChildren = it->animationChannels.data();

						eCurrentTableId = StructureArrayId_Animations;
						eChildrenTableId = StructureArrayId_AnimationChannels;
					}
					else {
						static_assert(true, "Invalid type to create entities based on [NameComponent, HierarchyComponent] grouping");
					}

					auto& hierarchy = m_scene.EmplaceComponent<HierarchyComponent>(idRoot);
					hierarchy.uChildrenCount = uChildrenCount;

					for (std::size_t i = 0; i < uChildrenCount; i++) {
						if (i == 0) {
							hierarchy.idFirst = m_structureIdTranslationTable[eCurrentTableId][pChildren[i]];
						}
						else {
							auto& childHierarchy = m_scene.GetComponent<HierarchyComponent>(m_structureIdTranslationTable[eChildrenTableId][pChildren[i]]);
							childHierarchy.idPrev = m_structureIdTranslationTable[eChildrenTableId][pChildren[i - 1]];
							childHierarchy.idParent = idRoot;
							auto& prevChildHierarchy = m_scene.GetComponent<HierarchyComponent>(m_structureIdTranslationTable[eChildrenTableId][pChildren[i - 1]]);
							prevChildHierarchy.idNext = m_structureIdTranslationTable[pChildren[i]];
						}
					}

					m_structureIdTranslationTable[eCurrentTableId].push_back(idRoot);
				}
			}

			void _CreateEntities(const std::vector<das2::AnimationChannel>& _animationChannels);

		public:
			DasExternalComponentizer(Scene& _scene, das2::Model&& _model);
			Entity Componentize();
	};
}