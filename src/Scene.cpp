// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Scene.cpp - scene class implementation
// author: Karl-Mihkel Ott

#define SCENE_CPP
#include "deng/Scene.h"

namespace DENG {

	Scene::Scene(Scene&& _scene) noexcept {
		m_sName = std::move(_scene.m_sName);
		m_registry = std::move(_scene.m_registry);
	}


	void Scene::RenderScene(IRenderer& _renderer) {
		auto view = m_registry.view<MaterialComponent>();
	}

}