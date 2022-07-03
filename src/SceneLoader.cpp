// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SceneLoader.cpp - Scene loader class implementation
// author: Karl-Mihkel Ott

#define SCENE_LOADER_CPP
#include <SceneLoader.h>

namespace DENG {

    uint32_t SceneLoader::m_scene_index = 0;

    SceneLoader::SceneLoader(
        Renderer &_rend, 
        Libdas::DasParser &_parser, 
        const Libdas::DasScene &_scene, 
        const std::vector<uint32_t> &_main_buffer_offsets, 
        uint32_t _camera_offset, 
        std::vector<Animation> &_animations,
        const std::string &_framebuffer_id
    ) {
        m_root_node_loaders.reserve(_scene.node_count);
        for (uint32_t i = 0; i < _scene.node_count; i++) {
            const Libdas::DasNode& node = _parser.AccessNode(_scene.nodes[i]);
            m_root_node_loaders.emplace_back(_rend, node, &_parser, _main_buffer_offsets, _camera_offset, _animations, _framebuffer_id, Libdas::Matrix4<float>());
        }

        // give scene a name if possible
        if(_scene.name != "")
            m_scene_name = _scene.name;
        else m_scene_name += std::to_string(m_scene_index++);
    }


    SceneLoader::SceneLoader(SceneLoader&& _sl) noexcept :
        m_root_node_loaders(std::move(_sl.m_root_node_loaders)),
        m_scene_name(std::move(_sl.m_scene_name)) {}


    void SceneLoader::_SetParser(Libdas::DasParser &_parser) {
        for(auto it = m_root_node_loaders.begin(); it != m_root_node_loaders.end(); it++) {
            it->_SetParser(_parser);
        }
    }


    void SceneLoader::Update() {
        for (auto it = m_root_node_loaders.begin(); it != m_root_node_loaders.end(); it++)
            it->Update();
    }
}
