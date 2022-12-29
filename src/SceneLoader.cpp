// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SceneLoader.cpp - Scene loader class implementation
// author: Karl-Mihkel Ott

#define SCENE_LOADER_CPP
#include "deng/SceneLoader.h"

namespace DENG {

    uint32_t SceneLoader::m_scene_index = 0;

    SceneLoader::SceneLoader(
        Entity *_parent,
        Renderer &_rend, 
        Libdas::DasModel &_model, 
        const Libdas::DasScene &_scene, 
        const std::vector<uint32_t> &_main_buffer_offsets, 
        uint32_t _camera_offset,
        std::vector<Animation> &_animations,
        const std::vector<uint32_t> &_framebuffer_ids
    ) : Entity(_parent, "", ENTITY_TYPE_SCENE) 
    {
        // give the scene a name if possible
        std::string name = "Unnamed scene";
        if (_scene.name != "")
            name = _scene.name;
        else name += std::to_string(m_scene_index++);

        SetName(name);

        m_root_node_loaders.reserve(_scene.node_count);
        for (uint32_t i = 0; i < _scene.node_count; i++) {
            const Libdas::DasNode& node = _model.nodes[_scene.nodes[i]];
            m_root_node_loaders.emplace_back(this, _rend, node, &_model, _main_buffer_offsets, _camera_offset, _animations, _framebuffer_ids, TRS::Matrix4<float>());
        }
    }


    SceneLoader::SceneLoader(SceneLoader&& _sl) noexcept :
        Entity(std::move(_sl)),
        m_root_node_loaders(std::move(_sl.m_root_node_loaders)) {}


    void SceneLoader::_SetModel(Libdas::DasModel &_model) {
        for(auto it = m_root_node_loaders.begin(); it != m_root_node_loaders.end(); it++) {
            it->_SetModel(_model);
        }
    }


    void SceneLoader::Update() {
        for (auto it = m_root_node_loaders.begin(); it != m_root_node_loaders.end(); it++)
            it->Update();
    }
}
