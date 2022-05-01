// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SkeletonDataManager.h - DAS skeleton transformation data manager
// author: Karl-Mihkel Ott

#ifndef SKELETON_DATA_MANAGER_H
#define SKELETON_DATA_MANAGER_H

#ifdef SKELETON_DATA_MANAGER_CPP
    #include <vector>
    #include <cstring>
    #include <string>
    #include <chrono>
    #include <fstream>
    #include <any>
    #include <unordered_map>
    #include <cmath>
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
    #include <ModelUniforms.h>
    #include <AnimationSampler.h>
#endif

namespace DENG {

    class SkeletonDataManager {
        private:
            Libdas::DasParser &m_parser;
            const Libdas::DasSkeleton &m_skeleton;
            std::vector<Libdas::Matrix4<float>> m_joint_matrices;
            std::vector<Libdas::Matrix4<float>> m_modified_joint_matrices;
            std::vector<uint32_t> m_joint_lookup;
            std::vector<AnimationSampler*> m_joint_samplers;
            static uint32_t m_skeleton_index;
            std::string m_skeleton_name = "Unnamed skeleton";

        public:
            SkeletonDataManager(Libdas::DasParser &_parser, const Libdas::DasSkeleton &_skeleton, std::vector<std::vector<AnimationSampler>> &_animation_samplers);
            void Update();

            inline std::vector<Libdas::Matrix4<float>> GetJointMatrices() {
                return m_modified_joint_matrices;
            }

            inline const Libdas::DasSkeleton &GetSkeleton() {
                return m_skeleton;
            }
    };
}

#endif
