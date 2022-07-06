// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SkeletonLoader.h - DAS skeleton transformation data loader class header
// author: Karl-Mihkel Ott
//
#ifndef SKELETON_LOADER_H
#define SKELETON_LOADER_H

#ifdef SKELETON_LOADER_CPP
    #include <vector>
    #include <cstring>
    #include <string>
    #include <chrono>
    #include <fstream>
    #include <any>
    #include <unordered_map>
    #include <cmath>
    #include <queue>
    #include <algorithm>
#ifdef _DEBUG
    #include <iostream>
#endif

    #include <libdas/include/Points.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Quaternion.h>
    #include <libdas/include/DasStructures.h>
    #include <libdas/include/Api.h>
    #include <libdas/include/ErrorHandlers.h>
    #include <libdas/include/AsciiStreamReader.h>
    #include <libdas/include/AsciiLineReader.h>
    #include <libdas/include/DasReaderCore.h>
    #include <libdas/include/DasParser.h>

    #include <Api.h>
    #include <ErrorDefinitions.h>
    #include <ModelUniforms.h>
    #include <AnimationSampler.h>
#endif

namespace DENG {

    class SkeletonLoader {
        friend class NodeLoader;
        private:
            struct JointAnimatedProperties {
                bool is_t = false;
                bool is_r = false;
                bool is_s = false;
            };

            struct TRS {
                Libdas::Vector3<float> t;
                Libdas::Quaternion r;
                float s;
            };

            Libdas::DasParser *mp_parser;
            const Libdas::DasSkeleton &m_skeleton;
            const Libdas::Matrix4<float> m_node_transform;
            const Libdas::Matrix4<float> m_inv_node_transform;
            std::vector<Libdas::Matrix4<float>> m_joint_transforms;
            std::vector<Libdas::Matrix4<float>> m_joint_world_transforms;
            std::vector<Libdas::Matrix4<float>> m_joint_matrices;
            std::vector<JointAnimatedProperties> m_joint_animated_properties;
            std::vector<TRS> m_animated_trs_values;
            std::vector<uint32_t> m_joint_lookup;
            std::vector<std::pair<const bool*, AnimationSampler*>> m_joint_samplers;
            static uint32_t m_skeleton_index;
            std::string m_skeleton_name = "Unnamed skeleton";
            uint32_t m_max_joint = 0;
            bool m_is_bound = false;

        private:
            void _FindJointAnimatedProperties();
            void _FillJointTransformTableTRS();
            void _CalculateJointWorldTransforms();
            void _ApplyJointTransforms(uint32_t _joint_id);

        public:
            SkeletonLoader(
                const Libdas::Matrix4<float> &_node, 
                Libdas::DasParser *_p_parser, 
                const Libdas::DasSkeleton &_skeleton, 
                std::vector<Animation> &_animations
            );
            SkeletonLoader(const SkeletonLoader&) = delete;
            SkeletonLoader(SkeletonLoader&& _sm) noexcept;
            void Update();
            void SetNewParentTransform(const Libdas::Matrix4<float> &_parent);

            inline std::vector<Libdas::Matrix4<float>> &GetJointMatrices() {
                return m_joint_matrices;
            }

            inline const Libdas::DasSkeleton &GetSkeleton() {
                return m_skeleton;
            }
    };
}

#endif
