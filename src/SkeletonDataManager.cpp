#define SKELETON_DATA_MANAGER_CPP
#include <SkeletonDataManager.h>

namespace DENG {

    uint32_t SkeletonDataManager::m_skeleton_index = 0;

    SkeletonDataManager::SkeletonDataManager(Libdas::DasParser &_parser, const Libdas::DasSkeleton &_skeleton, std::vector<std::vector<AnimationSampler>> &_animation_sampler) : 
        m_parser(_parser),
        m_skeleton(_skeleton)
    {
        m_joint_matrices.reserve(m_skeleton.joint_count);
        m_modified_joint_matrices.reserve(m_skeleton.joint_count);

        // create joint matrices
        uint32_t max_joint = 0;
        for(uint32_t i = 0; i < m_skeleton.joint_count; i++) {
            const Libdas::DasSkeletonJoint &joint = m_parser.AccessSkeletonJoint(m_skeleton.joints[i]);
            Libdas::Matrix4<float> scale_matrix = {
                { joint.scale, 0.0f, 0.0f, 0.0f },
                { 0.0f, joint.scale, 0.0f, 0.0f },
                { 0.0f, 0.0f, joint.scale, 0.0f },
                { 0.0f, 0.0f, 0.0f, 1.0f }
            };

            Libdas::Matrix4<float> rotation_matrix = joint.rotation.ExpandToMatrix4();
            Libdas::Matrix4<float> translation_matrix = {
                { 1.0f, 0.0f, 0.0f, joint.translation.x },
                { 0.0f, 1.0f, 0.0f, joint.translation.y },
                { 0.0f, 0.0f, 1.0f, joint.translation.z },
                { 0.0f, 0.0f, 0.0f, 1.0f }
            };

            m_joint_matrices.push_back((translation_matrix * rotation_matrix * scale_matrix) * joint.inverse_bind_pos);

            if(m_skeleton.joints[i] > max_joint)
                max_joint = m_skeleton.joints[i];
        }

        m_modified_joint_matrices.insert(m_modified_joint_matrices.begin(), m_joint_matrices.begin(), m_joint_matrices.end());
        max_joint++;
        m_joint_lookup.resize(max_joint);

        // fill lookup table
        for(uint32_t i = 0; i < m_skeleton.joint_count; i++)
            m_joint_lookup[m_skeleton.joints[i]] = i;

        // check which animation sampler joint ids are used in current skeleton
        for(auto ani_it = _animation_sampler.begin(); ani_it != _animation_sampler.end(); ani_it++) {
            for(auto smp_it = ani_it->begin(); smp_it != ani_it->end(); smp_it++) {
                for(uint32_t i = 0; i < m_skeleton.joint_count; i++) {
                    if(m_skeleton.joints[i] == smp_it->GetAnimationChannel().joint_id)
                        m_joint_samplers.push_back(&(*smp_it));
                }
            }
        }

        if(m_skeleton.name != "")
            m_skeleton_name = m_skeleton.name;
        else m_skeleton_name += std::to_string(m_skeleton_index++);
    }


    void SkeletonDataManager::Update() {
        m_modified_joint_matrices.insert(m_modified_joint_matrices.begin(), m_joint_matrices.begin(), m_joint_matrices.end());

        // update animation samplers
        for(auto it = m_joint_samplers.begin(); it != m_joint_samplers.end(); it++) {
            (*it)->Update();
            m_modified_joint_matrices[m_joint_lookup[(*it)->GetAnimationChannel().joint_id]] = (*it)->GetAnimationMatrix();
        }
    }
}
