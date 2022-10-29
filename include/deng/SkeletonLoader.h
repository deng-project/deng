// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SkeletonLoader.h - DAS skeleton transformation data loader class header
// author: Karl-Mihkel Ott
//
#ifndef SKELETON_LOADER_H
#define SKELETON_LOADER_H

#ifdef SKELETON_LOADER_CPP
    #include <vector>
    #include <array>
    #include <cstring>
    #include <string>
    #include <chrono>
    #include <fstream>
    #include <any>
    #include <unordered_map>
    #include <cmath>
    #include <queue>
    #include <algorithm>
#ifdef __DEBUG
    #include <iostream>
#endif

    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Quaternion.h"

    #include "mar/AsciiStreamReader.h"
    #include "mar/AsciiLineReader.h"

    #include "das/Api.h"
    #include "das/DasStructures.h"
    #include "das/ErrorHandlers.h"
    #include "das/DasReaderCore.h"
    #include "das/DasParser.h"

    #include "deng/Api.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/ModelUniforms.h"
    #include "deng/Entity.h"
    #include "deng/AnimationSampler.h"
#endif

namespace DENG {

    class SkeletonLoader : public Entity {
        friend class NodeLoader;
        private:
            struct JointAnimatedProperties {
                bool is_t = false;
                bool is_r = false;
                bool is_s = false;
            };

            struct TRSProperties {
                TRS::Vector3<float> t;
                TRS::Quaternion r;
                float s;
            };

            Libdas::DasModel *mp_model;
            const Libdas::DasSkeleton &m_skeleton;
            const TRS::Matrix4<float> m_node_transform;
            const TRS::Matrix4<float> m_inv_node_transform;
            std::vector<TRS::Matrix4<float>> m_joint_transforms;
            std::vector<TRS::Matrix4<float>> m_joint_world_transforms;
            std::vector<TRS::Matrix4<float>> m_joint_matrices;
            std::vector<JointAnimatedProperties> m_joint_animated_properties;
            std::vector<TRSProperties> m_animated_trs_values;
            std::vector<uint32_t> m_joint_lookup;
            std::vector<std::pair<const bool*, AnimationSampler*>> m_joint_samplers;
            static uint32_t m_skeleton_index;
            uint32_t m_max_joint = 0;
            bool m_is_bound = false;

        private:
            void _FindJointAnimatedProperties();
            void _FillJointTransformTableTRS();
            void _CalculateJointWorldTransforms();
            void _ApplyJointTransforms(uint32_t _joint_id);

        public:
            SkeletonLoader(
                Entity *_ent,
                const TRS::Matrix4<float> &_node, 
                Libdas::DasModel *_model, 
                const Libdas::DasSkeleton &_skeleton, 
                std::vector<Animation> &_animations
            );
            SkeletonLoader(const SkeletonLoader&) = delete;
            SkeletonLoader(SkeletonLoader&& _sm) noexcept;
            void Update();
            void SetNewParentTransform(const TRS::Matrix4<float> &_parent);

            inline std::vector<TRS::Matrix4<float>> &GetJointMatrices() {
                return m_joint_matrices;
            }

            inline const Libdas::DasSkeleton &GetSkeleton() {
                return m_skeleton;
            }
    };
}

#endif
