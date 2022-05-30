#define SKELETON_DATA_MANAGER_CPP
#include <SkeletonDataManager.h>

namespace DENG {

    uint32_t SkeletonDataManager::m_skeleton_index = 0;

    SkeletonDataManager::SkeletonDataManager(const Libdas::Matrix4<float> &_node, Libdas::DasParser &_parser, const Libdas::DasSkeleton &_skeleton, std::vector<Animation> &_animations) : 
        m_parser(_parser),
        m_skeleton(_skeleton),
        m_node_transform(_node),
        m_inv_node_transform(_node.Inverse())
    {
        m_joint_matrices.resize(m_skeleton.joint_count);
        m_joint_world_transforms.resize(m_skeleton.joint_count);

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

        _FillJointTransformTableTRS();
        _CalculateJointWorldTransforms();

        // check which animation sampler joint ids are used in current skeleton
        for(auto ani_it = _animations.begin(); ani_it != _animations.end(); ani_it++) {
            for(auto smp_it = ani_it->samplers.begin(); smp_it != ani_it->samplers.end(); smp_it++) {
                for(uint32_t i = 0; i < m_skeleton.joint_count; i++) {
                    if(m_skeleton.joints[i] == smp_it->GetAnimationChannel().joint_id)
                        m_joint_samplers.push_back(std::make_pair(&ani_it->is_bound, &(*smp_it)));
                }
            }
        }

        if(m_skeleton.name != "")
            m_skeleton_name = m_skeleton.name;
        else m_skeleton_name += std::to_string(m_skeleton_index++);
    }


    SkeletonDataManager::SkeletonDataManager(const SkeletonDataManager &_sdm) noexcept :
        m_parser(_sdm.m_parser),
        m_skeleton(_sdm.m_skeleton),
        m_node_transform(_sdm.m_node_transform),
        m_inv_node_transform(_sdm.m_inv_node_transform),
        m_joint_matrices(_sdm.m_joint_matrices),
        m_joint_world_transforms(_sdm.m_joint_world_transforms),
        m_joint_trs_transforms(_sdm.m_joint_trs_transforms),
        m_inverse_bind_matrices(_sdm.m_inverse_bind_matrices),
        m_joint_lookup(_sdm.m_joint_lookup),
        m_joint_samplers(_sdm.m_joint_samplers),
        m_skeleton_name(_sdm.m_skeleton_name),
        m_max_joint(_sdm.m_max_joint),
        m_is_bound(_sdm.m_is_bound) {}


    SkeletonDataManager::SkeletonDataManager(SkeletonDataManager &&_sdm) noexcept :
        m_parser(_sdm.m_parser),
        m_skeleton(_sdm.m_skeleton),
        m_node_transform(_sdm.m_node_transform),
        m_inv_node_transform(_sdm.m_inv_node_transform),
        m_joint_matrices(std::move(_sdm.m_joint_matrices)),
        m_joint_world_transforms(std::move(_sdm.m_joint_world_transforms)),
        m_joint_trs_transforms(std::move(_sdm.m_joint_trs_transforms)),
        m_inverse_bind_matrices(std::move(_sdm.m_inverse_bind_matrices)),
        m_joint_lookup(std::move(_sdm.m_joint_lookup)),
        m_joint_samplers(std::move(_sdm.m_joint_samplers)),
        m_skeleton_name(std::move(_sdm.m_skeleton_name)),
        m_max_joint(_sdm.m_max_joint),
        m_is_bound(_sdm.m_is_bound) {}


    void SkeletonDataManager::_FillJointTransformTableTRS() {
        m_joint_trs_transforms.resize(m_max_joint);

        // fill the TRS table
        for(uint32_t i = 0; i < m_skeleton.joint_count; i++) {
            const Libdas::DasSkeletonJoint &joint = m_parser.AccessSkeletonJoint(m_skeleton.joints[i]);
            JointTransformation &trs = m_joint_trs_transforms[m_joint_lookup[m_skeleton.joints[i]]];
            trs.t = { joint.translation.x, joint.translation.y, joint.translation.z };
            trs.r = joint.rotation;
            trs.s = joint.scale;
        }
    }


    void SkeletonDataManager::_CalculateJointWorldTransforms() {
        // first - index of the joint to be calculated
        // second - index of parent joint (UINT32_MAX for none)
        std::queue<std::pair<uint32_t, uint32_t>> children;
        children.push(std::make_pair(m_skeleton.parent, UINT32_MAX));

        while(!children.empty()) {
            const std::pair<uint32_t, uint32_t> child = children.front();
            children.pop();

            const Libdas::DasSkeletonJoint &joint = m_parser.AccessSkeletonJoint(child.first);
            const JointTransformation &trs = m_joint_trs_transforms[m_joint_lookup[child.first]];

            const Libdas::Matrix4<float> t = {
                { 1.0f, 0.0f, 0.0f, trs.t.first },
                { 0.0f, 1.0f, 0.0f, trs.t.second },
                { 0.0f, 0.0f, 1.0f, trs.t.third },
                { 0.0f, 0.0f, 0.0f, 1.0f }
            };

            const Libdas::Matrix4<float> r = trs.r.ExpandToMatrix4();
            const Libdas::Matrix4<float> s = {
                { trs.s, 0.0f, 0.0f, 0.0f },
                { 0.0f, trs.s, 0.0f, 0.0f },
                { 0.0f, 0.0f, trs.s, 0.0f },
                { 0.0f, 0.0f, 0.0f, 1.0f },
            };

            if(child.second == UINT32_MAX) {
                m_joint_world_transforms[m_joint_lookup[child.first]] = m_node_transform * (t * r * s);
            } else {
                m_joint_world_transforms[m_joint_lookup[child.first]] = m_joint_world_transforms[m_joint_lookup[child.second]] * (t * r * s);
            }

            // for each child joint
            for(uint32_t i = 0; i < joint.children_count; i++)
                children.push(std::make_pair(joint.children[i], child.first));
        }
    }


    void SkeletonDataManager::_ApplyJointTransforms(uint32_t _joint_id) {
        std::queue<uint32_t> children;
        children.push(_joint_id);

        while(!children.empty()) {
            uint32_t curr_id = children.front();
            children.pop();

            const Libdas::DasSkeletonJoint &joint = m_parser.AccessSkeletonJoint(curr_id);
            if(m_is_bound)
                m_joint_matrices[m_joint_lookup[curr_id]] = m_inv_node_transform * m_joint_world_transforms[m_joint_lookup[curr_id]] * /* m_inverse_bind_matrices[m_joint_lookup[curr_id]]; */joint.inverse_bind_pos;
            else m_joint_matrices[m_joint_lookup[curr_id]] = Libdas::Matrix4<float>();

            for(uint32_t i = 0; i < joint.children_count; i++)
                children.push(joint.children[i]);
        }
    }


    void SkeletonDataManager::Update() {
        m_is_bound = false;
        std::fill(m_joint_trs_transforms.begin(), m_joint_trs_transforms.end(), JointTransformation());
        std::fill(m_joint_world_transforms.begin(), m_joint_world_transforms.end(), Libdas::Matrix4<float>());

        // update animation samplers
        for(auto it = m_joint_samplers.begin(); it != m_joint_samplers.end(); it++) {
            if(*it->first) {
                m_is_bound = true;
                it->second->Update();
                const uint32_t ani_joint = it->second->GetAnimationChannel().joint_id;

                switch(it->second->GetAnimationTarget()) {
                    case LIBDAS_ANIMATION_TARGET_TRANSLATION:
                        m_joint_trs_transforms[m_joint_lookup[ani_joint]].t = it->second->GetTranslation();
                        break;

                    case LIBDAS_ANIMATION_TARGET_ROTATION:
                        m_joint_trs_transforms[m_joint_lookup[ani_joint]].r = it->second->GetRotation();
                        break;

                    case LIBDAS_ANIMATION_TARGET_SCALE:
                        m_joint_trs_transforms[m_joint_lookup[ani_joint]].s = it->second->GetScale();
                        break;

                    default:
                        DENG_ASSERT(false);
                        break;
                }
            }
        }

        _CalculateJointWorldTransforms();
        _ApplyJointTransforms(m_skeleton.parent);

        for(auto it = m_joint_matrices.begin(); it != m_joint_matrices.end(); it++)
            (*it) = it->Transpose();
    }
}
