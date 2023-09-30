// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: DasExternalComponentizer.h - implementation for das2 format componentizer class
// author: Karl-Mihkel Ott

#include <deng/DasExternalComponentizer.h>

namespace DENG {
	
	uint32_t DasExternalComponentizer::m_uUnnamedAnimationCount = 0;
	uint32_t DasExternalComponentizer::m_uUnnamedMeshGroupCount = 0;
	uint32_t DasExternalComponentizer::m_uUnnamedNodeCount = 0;
	uint32_t DasExternalComponentizer::m_uUnnamedSceneCount = 0;
	uint32_t DasExternalComponentizer::m_uUnnamedSkeletonCount = 0;
	uint32_t DasExternalComponentizer::m_uUnnamedSkeletonJointCount = 0;

	

	DasExternalComponentizer::DasExternalComponentizer(Scene& _scene, das2::Model&& _model) :
		m_model(_model),
		m_scene(_scene) 
	{
	}

	void DasExternalComponentizer::_CreateEntities(const std::vector<das2::Mesh>& _meshes) {
	}

	void DasExternalComponentizer::_CreateEntities(const std::vector<das2::MorphTarget>& _morphTargets) {

	}

	void DasExternalComponentizer::_CreateEntities(const std::vector<das2::MeshGroup>& _meshGroups) {

	}

	void DasExternalComponentizer::_CreateEntities(const std::vector<das2::Node>& _nodes) {

	}

	void DasExternalComponentizer::_CreateEntities(const std::vector<das2::SkeletonJoint>& _skeletonJoints) {

	}

	void DasExternalComponentizer::_CreateEntities(const std::vector<das2::AnimationChannel>& _animationChannels) {

	}

	Entity DasExternalComponentizer::Componentize() {
		Entity idModel = m_scene.CreateEntity();
		auto& model = m_scene.EmplaceComponent<ModelComponent>(idModel);
		model.sAuthorName = m_model.header.szAuthorName.CString();
		model.sComment = m_model.header.szComment.CString();
		auto& hierarchy = m_scene.EmplaceComponent<HierarchyComponent>(idModel);

		_CreateEntities(m_model.meshes);
		_CreateEntities(m_model.meshGroups);
		_CreateEntities(m_model.skeletonJoints);
		_CreateEntities(m_model.skeletons, idModel);
		_CreateEntities(m_model.nodes);
		_CreateEntities(m_model.scenes, idModel);
		_CreateEntities(m_model.animationChannels);
		_CreateEntities(m_model.animations, idModel);
	}
}