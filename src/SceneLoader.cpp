// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SceneLoader.cpp - Scene loader class implementation
// author: Karl-Mihkel Ott

#define SCENE_LOADER_CPP
#include <SceneLoader.h>

namespace DENG {

    uint32_t SceneLoader::m_scene_index = 0;

    SceneLoader::SceneLoader(Renderer &_rend, Libdas::DasParser &_parser, const Libdas::DasScene &_scene, const std::vector<uint32_t> &_main_buffer_offsets, uint32_t _camera_offset, std::vector<Animation> &_animation_samplers, std::vector<std::string> &_texture_names) : 
        m_renderer(_rend),
        m_parser(_parser),
        m_scene(_scene)
    {
        m_node_loaders.reserve(m_scene.node_count);
        uint32_t max_node = 0;
        for(uint32_t i = 0; i < m_scene.node_count; i++) {
            const Libdas::DasNode &node = m_parser.AccessNode(m_scene.nodes[i]);
            m_node_loaders.emplace_back(m_renderer, node, m_parser, _main_buffer_offsets, _camera_offset, _animation_samplers, _texture_names, Libdas::Matrix4<float>());
            if(m_scene.nodes[i] > max_node)
                max_node = m_scene.nodes[i];
        }

        max_node++;
        m_node_id_lookup.resize(max_node);
        // fill the lookup table
        for(uint32_t i = 0; i < m_scene.node_count; i++)
            m_node_id_lookup[m_scene.nodes[i]] = i;

        // check and select all animation samplers which target scene root nodes
        for(auto ani_it = _animation_samplers.begin(); ani_it != _animation_samplers.end(); ani_it++) {
            for(auto smp_it = ani_it->second.begin(); smp_it != ani_it->second.end(); smp_it++) {
                for(uint32_t i = 0; i < m_scene.node_count; i++) {
                    if(smp_it->GetAnimationChannel().node_id == m_scene.nodes[i]) {
                        m_scene_root_node_animation_samplers.push_back(&(*smp_it));
                        break;
                    }
                }
            }
        }

        m_node_root_transforms.resize(m_scene.node_count);
        m_morph_weights.resize(m_scene.node_count);

        // give scene a name if possible
        if(m_scene.name != "")
            m_scene_name = m_scene.name;
        else m_scene_name += std::to_string(m_scene_index++);
    }

    void SceneLoader::Update() {
        for(auto it = m_scene_root_node_animation_samplers.begin(); it != m_scene_root_node_animation_samplers.end(); it++) {
            (*it)->Update();
            m_node_root_transforms[m_node_id_lookup[(*it)->GetAnimationChannel().node_id]] = (*it)->GetAnimationMatrix();
            m_morph_weights[m_node_id_lookup[(*it)->GetAnimationChannel().node_id]] = (*it)->GetMorphWeights();
        }

        for(auto it = m_node_loaders.begin(); it != m_node_loaders.end(); it++) {
            const size_t index = it - m_node_loaders.begin();
            it->Update(m_node_root_transforms[index], m_morph_weights[index]);
        }
    }
}
