// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: NodeLoader.h - DAS model node transformation handler implementation
// author: Karl-Mihkel Ott

#define NODE_LOADER_CPP
#include <NodeLoader.h>

namespace DENG {

    uint32_t NodeLoader::m_node_index = 0;

    NodeLoader::NodeLoader(
        Renderer &_rend, 
        const Libdas::DasNode &_node, 
        Libdas::DasParser &_parser, 
        const std::vector<uint32_t> &_main_buffer_offsets, 
        uint32_t _camera_offset, 
        std::vector<Animation> &_animations, 
        const std::string &_framebuffer_id,
        const Libdas::Matrix4<float> &_parent
    ) :
        m_renderer(_rend),
        m_node(_node),
        m_parser(_parser),
        m_parent(_parent),
        m_framebuffer_id(_framebuffer_id)
    {
        // apply the parent transformation
        m_transform = m_parent * m_node.transform;

        // find the maximum node index from
        if(m_node.children_count) {
            m_max_node = *std::max_element(m_node.children, m_node.children + m_node.children_count);
            m_max_node++;

            // fill the lookup table
            m_node_lookup.resize(m_max_node);
            for(uint32_t i = 0; i < m_node.children_count; i++)
                m_node_lookup[m_node.children[i]] = i;

            // create child nodes
            m_child_nodes.reserve(m_node.children_count);
            for(uint32_t i = 0; i < m_node.children_count; i++) {
                const Libdas::DasNode &node = m_parser.AccessNode(m_node.children[i]);
                m_child_nodes.emplace_back(m_renderer, node, m_parser, _main_buffer_offsets, _camera_offset, _animations, _framebuffer_id, m_transform);
            }
        }

        _CreateBoundElementLoaders(_animations, _main_buffer_offsets, _camera_offset);
        _SearchForBoundAnimationSamplers(_animations);

        // give node a name if possible
        if(m_node.name != "")
            m_node_name = m_node.name;
        else m_node_name += std::to_string(m_node_index++);
    }


    NodeLoader::NodeLoader(NodeLoader &&_node) noexcept :
        m_renderer(_node.m_renderer),
        mp_skeleton(_node.mp_skeleton),
        mp_mesh_loader(_node.mp_mesh_loader),
        m_node(_node.m_node),
        m_parser(_node.m_parser),
        m_child_nodes(std::move(_node.m_child_nodes)),
        m_node_lookup(std::move(_node.m_node_lookup)),
        m_animation_samplers(std::move(_node.m_animation_samplers)),
        m_node_name(std::move(_node.m_node_name)),
        m_custom_translation(_node.m_custom_translation),
        m_custom_rotation(_node.m_custom_rotation),
        m_custom_scale(_node.m_custom_scale),
        m_parent(_node.m_parent),
        m_custom(_node.m_custom),
        m_transform(_node.m_transform),
        m_morph_weights(_node.m_morph_weights),
        m_max_node(_node.m_max_node),
        m_framebuffer_id(_node.m_framebuffer_id)
    {
        mp_skeleton = nullptr;
        mp_mesh_loader = nullptr;
    }


    NodeLoader::~NodeLoader() {
        delete mp_skeleton;
        delete mp_mesh_loader;
    }


    void NodeLoader::_CreateBoundElementLoaders(std::vector<Animation> &_animations, const std::vector<uint32_t> &_main_buffer_offsets, uint32_t _camera_offset) {
        if(m_node.skeleton != UINT32_MAX) {
            const Libdas::DasSkeleton &skeleton = m_parser.AccessSkeleton(m_node.skeleton);
            mp_skeleton = new SkeletonDataManager(m_transform, m_parser, skeleton, _animations);

            if(m_node.mesh == UINT32_MAX) {
                WARNME("Unbound skeleton: " + skeleton.name);
            }
        }

        if(m_node.mesh != UINT32_MAX) {
            const Libdas::DasMesh &mesh = m_parser.AccessMesh(m_node.mesh);
            if(mp_skeleton) {
                const uint32_t joint_count = static_cast<uint32_t>(mp_skeleton->GetJointMatrices().size());
                mp_mesh_loader = new MeshLoader(mesh, m_parser, m_renderer, _main_buffer_offsets, _camera_offset, joint_count, m_framebuffer_id);
            }
            else mp_mesh_loader = new MeshLoader(mesh, m_parser, m_renderer, _main_buffer_offsets, _camera_offset, 0, m_framebuffer_id);
            mp_mesh_loader->Attach();
            m_morph_weights = mp_mesh_loader->GetMorphWeights();
        }
    }


    void NodeLoader::_SearchForBoundAnimationSamplers(std::vector<Animation> &_animations) {
        // for each animation
        for(auto ani_it = _animations.begin(); ani_it != _animations.end(); ani_it++) {
            // for each sampler
            for(auto smp_it = ani_it->samplers.begin(); smp_it != ani_it->samplers.end(); smp_it++) {
                const uint32_t index = static_cast<uint32_t>(&m_node - &m_parser.AccessNode(0));
                if(smp_it->GetAnimationChannel().node_id == index)
                    m_animation_samplers.push_back(std::make_pair(&ani_it->is_bound, &(*smp_it)));
            }
        }
    }


    void NodeLoader::_UpdateTransformTRS(Libdas::Vector3<float> _translation, Libdas::Quaternion _rotation, float _scale) {
        const Libdas::Matrix4<float> t = {
            { 1.0f, 0.0f, 0.0f, _translation.first },
            { 0.0f, 1.0f, 0.0f, _translation.second },
            { 0.0f, 0.0f, 1.0f, _translation.third },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        };

        const Libdas::Matrix4<float> r = _rotation.ExpandToMatrix4();
        const Libdas::Matrix4<float> s = {
            { _scale, 0.0f, 0.0f, 0.0f },
            { 0.0f, _scale, 0.0f, 0.0f },
            { 0.0f, 0.0f, _scale, 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        };

        m_transform *= t * r * s;
    }

       
    void NodeLoader::Update() {
        bool parent_update_flag = false;

        // update animation samplers if required
        m_transform = m_parent;
        if(m_animation_samplers.size()) {
            for(auto it = m_animation_samplers.begin(); it != m_animation_samplers.end(); it++) {
                if(*it->first) {
                    it->second->Update();
                    Libdas::Vector3<float> translation = { 0.0f, 0.0f, 0.0f };
                    Libdas::Quaternion rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
                    float scale = 1.0f;

                    switch(it->second->GetAnimationTarget()) {
                        case LIBDAS_ANIMATION_TARGET_WEIGHTS:
                            m_morph_weights = it->second->GetMorphWeights();
                            break;

                        case LIBDAS_ANIMATION_TARGET_TRANSLATION:
                            translation = it->second->GetTranslation();
                            break;

                        case LIBDAS_ANIMATION_TARGET_ROTATION:
                            rotation = it->second->GetRotation();
                            break;

                        case LIBDAS_ANIMATION_TARGET_SCALE:
                            scale = it->second->GetScale();
                            break;

                        default:
                            DENG_ASSERT(false);
                            break;
                    }

                    _UpdateTransformTRS(translation, rotation, scale);
                    parent_update_flag = true;
                }
            }
        } else {
            m_transform *= m_node.transform;
        }

        _UpdateTransformTRS(m_custom_translation, m_custom_rotation, m_custom_scale);

        // update parent transformation for child nodes
        if(parent_update_flag) {
            for(uint32_t i = 0; i < m_node.children_count; i++)
                m_child_nodes[i].NewParent(m_transform);
        }

        // update children
        for(auto it = m_child_nodes.begin(); it != m_child_nodes.end(); it++)
            it->Update();

        // update mesh specific ubo if required
        if(mp_mesh_loader) {
            uint32_t offset = mp_mesh_loader->GetMeshUboOffset();
            ModelUbo ubo;
            ubo.node_transform = m_transform.Transpose();
            ubo.use_color = mp_mesh_loader->GetUseColor();
            ubo.color = mp_mesh_loader->GetColor();

            // check if morph weights are given
            if(m_morph_weights) std::memcpy(ubo.morph_weights, m_morph_weights, sizeof(float[MAX_MORPH_TARGETS]));
            else std::memset(ubo.morph_weights, 0, sizeof(float[MAX_MORPH_TARGETS]));
            m_renderer.UpdateUniform(reinterpret_cast<const char*>(&ubo), sizeof(ModelUbo), offset);
        }

        // update skeleton if required
        if(mp_skeleton) {
            mp_skeleton->Update();
            auto joints = mp_skeleton->GetJointMatrices();
            if(mp_mesh_loader)
                mp_mesh_loader->UpdateJointMatrices(joints);
        }
    }
}
