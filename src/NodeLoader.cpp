#define NODE_TRANSFORM_MANAGER_CPP
#include <NodeLoader.h>

namespace DENG {

    uint32_t NodeLoader::m_node_index = 0;

    NodeLoader::NodeLoader(Renderer &_rend, const Libdas::DasNode &_node, Libdas::DasParser &_parser, const std::vector<uint32_t> &_main_buffer_offsets, uint32_t _camera_offset, 
                           std::vector<Animation> &_animation_samplers, std::vector<std::string> &_texture_names, const Libdas::Matrix4<float> &_parent) :
        m_renderer(_rend),
        m_node(_node),
        m_parser(_parser),
        m_parent_transform(_parent)
    {
        // for each node traverse its children
        m_child_nodes.reserve(m_node.children_count);
        m_child_morph_weights.resize(m_node.children_count);
        m_child_matrices.resize(m_node.children_count);

        uint32_t max_node = 0;
        m_custom_transform = _parent * m_node.transform;
        for(uint32_t i = 0; i < m_node.children_count; i++) {
            const Libdas::DasNode &child_node = m_parser.AccessNode(m_node.children[i]);
            m_child_nodes.emplace_back(m_renderer, child_node, m_parser, _main_buffer_offsets, _camera_offset, _animation_samplers, _texture_names, m_custom_transform);

            if(m_node.children[i] > max_node)
                max_node = m_node.children[i];
        }

        // fill the lookup table
        max_node++;
        m_node_lookup.resize(max_node);
        for(uint32_t i = 0; i < m_node.children_count; i++)
            m_node_lookup[m_node.children[i]] = i;

        if(m_node.skeleton != UINT32_MAX) {
            const Libdas::DasSkeleton &skeleton = m_parser.AccessSkeleton(m_node.skeleton);
            mp_skeleton = new SkeletonDataManager(m_custom_transform, m_parser, skeleton, _animation_samplers);

            if(m_node.mesh == UINT32_MAX) {
                const Libdas::DasSkeleton &skeleton = mp_skeleton->GetSkeleton();
                WARNME("Unbound skeleton: " + skeleton.name);
            }
        }

        if(m_node.mesh != UINT32_MAX) {
            const Libdas::DasMesh &mesh = m_parser.AccessMesh(m_node.mesh);
            if(mp_skeleton) {
                const uint32_t joint_count = static_cast<uint32_t>(mp_skeleton->GetJointMatrices().size());
                mp_mesh_loader = new MeshLoader(mesh, m_parser, m_renderer, _main_buffer_offsets, _camera_offset, joint_count);
            }
            else mp_mesh_loader = new MeshLoader(mesh, m_parser, m_renderer, _main_buffer_offsets, _camera_offset, 0);
            mp_mesh_loader->Attach();
        }

        // search for animation samplers whose nodes are current node's children
        for(auto ani_it = _animation_samplers.begin(); ani_it != _animation_samplers.end(); ani_it++) {
            for(auto smp_it = ani_it->second.begin(); smp_it != ani_it->second.end(); smp_it++) {
                for(uint32_t i = 0; i < m_node.children_count; i++) {
                    if(m_node.children[i] == smp_it->GetAnimationChannel().node_id) {
                        m_child_samplers.push_back(&(*smp_it));
                        break;
                    }
                }
            }
        }


        // give node a name if possible
        if(m_node.name != "")
            m_node_name = m_node.name;
        else m_node_name += std::to_string(m_node_index++);
    }


    NodeLoader::~NodeLoader() {
        delete mp_skeleton;
        delete mp_mesh_loader;
    }


    void NodeLoader::ApplyCustomTransform(const Libdas::Matrix4<float> &_parent) {
        m_parent_transform = _parent;

        // TRS properties
        const Libdas::Matrix4<float> t = {
            { 1.0f, 0.0f, 0.0f, m_translation.first },
            { 0.0f, 1.0f, 0.0f, m_translation.second },
            { 0.0f, 0.0f, 1.0f, m_translation.third },
            { 0.0f, 0.0f, 0.0f, 1.0f },
        };

        const Libdas::Quaternion x(sinf(m_rotation.x / 2), 0, 0, cosf(m_rotation.x / 2));
        const Libdas::Quaternion y(0, sinf(m_rotation.y / 2), 0, cosf(m_rotation.y / 2));
        const Libdas::Quaternion z(0, 0, sinf(m_rotation.z / 2), cosf(m_rotation.z / 2));
        const Libdas::Matrix4<float> r = z.ExpandToMatrix4() * y.ExpandToMatrix4() * x.ExpandToMatrix4();

        const Libdas::Matrix4<float> s = {
            { m_scale, 0.0f, 0.0f, 0.0f },
            { 0.0f, m_scale, 0.0f, 0.0f },
            { 0.0f, 0.0f, m_scale, 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f },
        };

        m_custom_transform = m_parent_transform * m_node.transform * t * r * s;

        // for each child node apply transformations
        for(auto it = m_child_nodes.begin(); it != m_child_nodes.end(); it++)
            it->ApplyCustomTransform(m_custom_transform);
    }


    void NodeLoader::Update(const Libdas::Matrix4<float> &_parent, float *_morph_weights) {
        const Libdas::Matrix4<float> new_parent = m_custom_transform * m_node.transform * _parent;
        std::fill(m_child_matrices.begin(), m_child_matrices.end(), Libdas::Matrix4<float>());
        std::fill(m_child_morph_weights.begin(), m_child_morph_weights.end(), nullptr);

        for(auto it = m_child_samplers.begin(); it != m_child_samplers.end(); it++) {
            (*it)->Update();
            m_child_matrices[m_node_lookup[(*it)->GetAnimationChannel().node_id]] *= (*it)->GetAnimationMatrix();
            m_child_morph_weights[m_node_lookup[(*it)->GetAnimationChannel().node_id]] = (*it)->GetMorphWeights();
        }

        for(auto it = m_child_nodes.begin(); it != m_child_nodes.end(); it++) {
            const size_t index = it - m_child_nodes.begin();
            it->Update(m_child_matrices[index] * new_parent, m_child_morph_weights[index]);
        }

        // update mesh specific ubo if required
        if(mp_mesh_loader) {
            uint32_t offset = mp_mesh_loader->GetMeshUboOffset();
            ModelUbo ubo;
            ubo.node_transform = new_parent;
            ubo.use_color = mp_mesh_loader->GetUseColor();
            ubo.color = mp_mesh_loader->GetColor();

            // check if morph weights are given
            if(_morph_weights) std::memcpy(ubo.morph_weights, _morph_weights, sizeof(float[MAX_MORPH_TARGETS]));
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
