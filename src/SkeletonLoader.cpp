// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SkeletonLoader.h - DAS skeleton transformation data loader class implementation
// author: Karl-Mihkel Ott

#define SKELETON_LOADER_CPP
#include "deng/SkeletonLoader.h"

namespace DENG {

    uint32_t SkeletonLoader::m_skeleton_index = 0;

    SkeletonLoader::SkeletonLoader(
        const TRS::Matrix4<float> &_node, 
        Libdas::DasParser *_p_parser, 
        const Libdas::DasSkeleton &_skeleton, 
        std::vector<Animation> &_animations
    ) : 
        mp_parser(_p_parser),
        m_skeleton(_skeleton),
        m_node_transform(_node),
        m_inv_node_transform(_node.Inverse())
    {
        m_joint_transforms.resize(m_skeleton.joint_count);
        m_joint_world_transforms.resize(m_skeleton.joint_count);
        m_joint_matrices.resize(m_skeleton.joint_count);
        m_joint_animated_properties.resize(m_skeleton.joint_count);
        m_animated_trs_values.resize(m_skeleton.joint_count);

        // find the maximum joint id
        for(uint32_t i = 0; i < m_skeleton.joint_count; i++) {
            if(m_skeleton.joints[i] > m_max_joint)
                m_max_joint = m_skeleton.joints[i];
        }

        m_max_joint++;
        m_joint_lookup.resize(m_max_joint);

        // fill lookup table
        for(uint32_t i = 0; i < m_skeleton.joint_count; i++)
            m_joint_lookup[m_skeleton.joints[i]] = i;

        _CalculateJointWorldTransforms();

        // check which animation sampler joint ids are used in current skeleton
        for(auto ani_it = _animations.begin(); ani_it != _animations.end(); ani_it++) {
            for(auto smp_it = ani_it->samplers.begin(); smp_it != ani_it->samplers.end(); smp_it++) {
                for(uint32_t i = 0; i < m_skeleton.joint_count; i++) {
                    if(m_skeleton.joints[i] == smp_it->GetAnimationChannel().joint_id)
                        m_joint_samplers.push_back(std::make_pair(&ani_it->is_unbound, &(*smp_it)));
                }
            }
        }

        _FindJointAnimatedProperties();

        if(m_skeleton.name != "")
            m_skeleton_name = m_skeleton.name;
        else m_skeleton_name += std::to_string(m_skeleton_index++);
    }


    SkeletonLoader::SkeletonLoader(SkeletonLoader &&_sm) noexcept :
        mp_parser(_sm.mp_parser),
        m_skeleton(_sm.m_skeleton),
        m_node_transform(_sm.m_node_transform),
        m_inv_node_transform(_sm.m_inv_node_transform),
        m_joint_transforms(std::move(_sm.m_joint_transforms)),
        m_joint_world_transforms(std::move(_sm.m_joint_world_transforms)),
        m_joint_matrices(std::move(_sm.m_joint_matrices)),
        m_joint_lookup(std::move(_sm.m_joint_lookup)),
        m_joint_samplers(std::move(_sm.m_joint_samplers)),
        m_skeleton_name(std::move(_sm.m_skeleton_name)),
        m_max_joint(_sm.m_max_joint),
        m_is_bound(_sm.m_is_bound) {}


    void SkeletonLoader::_FindJointAnimatedProperties() {
        for(auto it = m_joint_samplers.begin(); it != m_joint_samplers.end(); it++) {
            const uint32_t joint_id_l = m_joint_lookup[it->second->GetAnimationChannel().joint_id];

            switch(it->second->GetAnimationTarget()) {
                case LIBDAS_ANIMATION_TARGET_TRANSLATION:
                    m_joint_animated_properties[joint_id_l].is_t = true;
                    break;

                case LIBDAS_ANIMATION_TARGET_ROTATION:
                    m_joint_animated_properties[joint_id_l].is_r = true;
                    break;

                case LIBDAS_ANIMATION_TARGET_SCALE:
                    m_joint_animated_properties[joint_id_l].is_s = true;
                    break;

                default:
                    DENG_ASSERT(false);
                    break;
            }
        }
    }


    void SkeletonLoader::_CalculateJointWorldTransforms() {
        // first - index of the joint to be calculated
        // second - index of parent joint (UINT32_MAX for none)
        std::queue<std::pair<uint32_t, uint32_t>> children;
        children.push(std::make_pair(m_skeleton.parent, UINT32_MAX));

        while(!children.empty()) {
            const std::pair<uint32_t, uint32_t> child = children.front();
            children.pop();

            const Libdas::DasSkeletonJoint &joint = mp_parser->AccessSkeletonJoint(child.first);
            const TRS::Matrix4<float> &trs = m_joint_transforms[m_joint_lookup[child.first]];

            if(child.second == UINT32_MAX) {
                m_joint_world_transforms[m_joint_lookup[child.first]] = m_node_transform * trs;
            } else {
                m_joint_world_transforms[m_joint_lookup[child.first]] = m_joint_world_transforms[m_joint_lookup[child.second]] * trs;
            }

            // for each child joint
            for(uint32_t i = 0; i < joint.children_count; i++)
                children.push(std::make_pair(joint.children[i], child.first));
        }
    }


    void SkeletonLoader::_ApplyJointTransforms(uint32_t _joint_id) {
        std::queue<uint32_t> children;
        children.push(_joint_id);

        while(!children.empty()) {
            uint32_t curr_id = children.front();
            children.pop();

            const Libdas::DasSkeletonJoint &joint = mp_parser->AccessSkeletonJoint(curr_id);
            if(m_is_bound) {
                m_joint_matrices[m_joint_lookup[curr_id]] = m_inv_node_transform * m_joint_world_transforms[m_joint_lookup[curr_id]] * joint.inverse_bind_pos;
            } else {
                m_joint_matrices[m_joint_lookup[curr_id]] = TRS::Matrix4<float>();
            }

            for(uint32_t i = 0; i < joint.children_count; i++)
                children.push(joint.children[i]);
        }
    }


    void SkeletonLoader::Update() {
        m_is_bound = false;
        std::fill(m_joint_transforms.begin(), m_joint_transforms.end(), TRS::Matrix4<float>());
        std::fill(m_joint_world_transforms.begin(), m_joint_world_transforms.end(), TRS::Matrix4<float>());

        // update animation samplers
        for(auto it = m_joint_samplers.begin(); it != m_joint_samplers.end(); it++) {
            if(!*it->first) {
                m_is_bound = true;
                it->second->Update();
                const uint32_t ani_joint = it->second->GetAnimationChannel().joint_id;
                TRS::Matrix4<float> ani_mat;

                switch(it->second->GetAnimationTarget()) {
                    case LIBDAS_ANIMATION_TARGET_TRANSLATION:
                        m_animated_trs_values[m_joint_lookup[ani_joint]].t = it->second->GetTranslation();
                        break;

                    case LIBDAS_ANIMATION_TARGET_ROTATION:
                        m_animated_trs_values[m_joint_lookup[ani_joint]].r = it->second->GetRotation();
                        break;

                    case LIBDAS_ANIMATION_TARGET_SCALE:
                        m_animated_trs_values[m_joint_lookup[ani_joint]].s = it->second->GetScale();
                        break;

                    default:
                        DENG_ASSERT(false);
                        break;
                }
            }
        }

        // update local joint transformations
        for(uint32_t i = 0; i < m_skeleton.joint_count; i++) {
            const Libdas::DasSkeletonJoint &joint = mp_parser->AccessSkeletonJoint(m_skeleton.joints[i]);
            if(m_joint_animated_properties[m_joint_lookup[i]].is_t) {
                const TRS::Matrix4<float> t = {
                    { 1.0f, 0.0f, 0.0f, m_animated_trs_values[m_joint_lookup[i]].t.first },
                    { 0.0f, 1.0f, 0.0f, m_animated_trs_values[m_joint_lookup[i]].t.second },
                    { 0.0f, 0.0f, 1.0f, m_animated_trs_values[m_joint_lookup[i]].t.third },
                    { 0.0f, 0.0f, 0.0f, 1.0f },
                };
                m_joint_transforms[m_joint_lookup[i]] *= t;
            } else {
                const TRS::Matrix4<float> t = {
                    { 1.0f, 0.0f, 0.0f, joint.translation.x },
                    { 0.0f, 1.0f, 0.0f, joint.translation.y },
                    { 0.0f, 0.0f, 1.0f, joint.translation.z },
                    { 0.0f, 0.0f, 0.0f, 1.0f }
                };
                m_joint_transforms[m_joint_lookup[i]] *= t;
            }

            if(m_joint_animated_properties[m_joint_lookup[i]].is_r) {
                const TRS::Matrix4<float> r = m_animated_trs_values[m_joint_lookup[i]].r.ExpandToMatrix4();
                m_joint_transforms[m_joint_lookup[i]] *= r;
            } else {
                const TRS::Matrix4<float> r = joint.rotation.ExpandToMatrix4();
                m_joint_transforms[m_joint_lookup[i]] *= r;
            }

            if(m_joint_animated_properties[m_joint_lookup[i]].is_s) {
                const TRS::Matrix4<float> s = {
                    { m_animated_trs_values[m_joint_lookup[i]].s, 0.0f, 0.0f, 0.0f },
                    { 0.0f, m_animated_trs_values[m_joint_lookup[i]].s, 0.0f, 0.0f },
                    { 0.0f, 0.0f, m_animated_trs_values[m_joint_lookup[i]].s, 0.0f },
                    { 0.0f, 0.0f, 0.0f, 1.0f },
                };
                m_joint_transforms[m_joint_lookup[i]] *= s;
            } else {
                const TRS::Matrix4<float> s = {
                    { joint.scale, 0.0f, 0.0f, 0.0f },
                    { 0.0f, joint.scale, 0.0f, 0.0f },
                    { 0.0f, 0.0f, joint.scale, 0.0f },
                    { 0.0f, 0.0f, 0.0f, 1.0f },
                };
                m_joint_transforms[m_joint_lookup[i]] *= s;
            }
        }

        _CalculateJointWorldTransforms();
        _ApplyJointTransforms(m_skeleton.parent);

        for(auto it = m_joint_matrices.begin(); it != m_joint_matrices.end(); it++)
            (*it) = it->Transpose();
    }
}
