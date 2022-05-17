// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SceneLoader.cpp - Scene loader class implementation
// author: Karl-Mihkel Ott

#define SCENE_LOADER_CPP
#include <SceneLoader.h>

namespace DENG {

    uint32_t SceneLoader::m_scene_index = 0;

    SceneLoader::SceneLoader(Renderer &_rend, Libdas::DasParser &_parser, const Libdas::DasScene &_scene, const std::vector<uint32_t> &_main_buffer_offsets, uint32_t _camera_offset, std::vector<Animation> &_animations) : 
        m_renderer(_rend),
        m_parser(_parser),
        m_scene(_scene)
    {
        uint32_t root = _FindRootNode();
        DENG_ASSERT(root != UINT32_MAX);
        const Libdas::DasNode &node = m_parser.AccessNode(root);
        mp_root_node_loader = new NodeLoader(_rend, node, _parser, _main_buffer_offsets, _camera_offset, _animations, Libdas::Matrix4<float>());

        // give scene a name if possible
        if(m_scene.name != "")
            m_scene_name = m_scene.name;
        else m_scene_name += std::to_string(m_scene_index++);
    }


    SceneLoader::~SceneLoader() {
        delete mp_root_node_loader;
    }


    uint32_t SceneLoader::_FindRootNode() {
        uint32_t max = *std::max_element(m_scene.nodes, m_scene.nodes + m_scene.node_count);
        max++;
        std::vector<bool> child_node_table(max);
        for(uint32_t i = 0; i < m_scene.node_count; i++) {
            const Libdas::DasNode &node = m_parser.AccessNode(m_scene.nodes[i]);
            for(uint32_t j = 0; j < node.children_count; j++)
                child_node_table[node.children[j]] = true;
        }

        // find the nearest non-child node
        for(uint32_t i = 0; i < m_scene.node_count; i++) {
            if(!child_node_table[m_scene.nodes[i]])
                return m_scene.nodes[i];
        }

        return UINT32_MAX;
    }
}
