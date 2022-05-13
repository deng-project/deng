// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: NodeTransformManager.h - DAS model node transformation handler
// author: Karl-Mihkel Ott

#ifndef NODE_TRANSFORM_MANAGER_H
#define NODE_TRANSFORM_MANAGER_H

#ifdef NODE_TRANSFORM_MANAGER_CPP
    #include <cstdint>
    #include <cstring>
    #include <cmath>
    #include <chrono>
#ifdef _DEBUG
    #include <iostream>
#endif
    #include <string>
    #include <vector>
    #include <fstream>
    #include <unordered_map>
    #include <any>
    #include <algorithm>

    #include <libdas/include/Api.h>
    #include <libdas/include/Points.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Quaternion.h>
    #include <libdas/include/ErrorHandlers.h>
    #include <libdas/include/DasStructures.h>
    #include <libdas/include/AsciiStreamReader.h>
    #include <libdas/include/AsciiLineReader.h>
    #include <libdas/include/DasReaderCore.h>
    #include <libdas/include/DasParser.h>

    #include <Api.h>
    #include <ShaderDefinitions.h>
    #include <ErrorDefinitions.h>
    #include <Window.h>
    #include <Renderer.h>
    #include <ModelUniforms.h>
    #include <AnimationSampler.h>
    #include <MeshLoader.h>
    #include <NodeLoader.h>
    #include <SkeletonDataManager.h>
#endif

namespace DENG {

    // recursive class
    class DENG_API NodeLoader {
        private:
            Renderer &m_renderer;
            SkeletonDataManager *mp_skeleton = nullptr;
            MeshLoader *mp_mesh_loader = nullptr;
            const Libdas::DasNode &m_node;
            Libdas::DasParser &m_parser;

            std::vector<float*> m_child_morph_weights;
            std::vector<Libdas::Matrix4<float>> m_child_matrices;
            std::vector<NodeLoader> m_child_nodes;
            std::vector<uint32_t> m_node_lookup;
            std::vector<AnimationSampler*> m_child_samplers;
            static uint32_t m_node_index;
            std::string m_node_name = "Unnamed node";

            // custom transformation properties
            float m_scale = 1.0f;
            Libdas::Vector3<float> m_translation = { 0.0f, 0.0f, 0.0f };
            Libdas::Point3D<float> m_rotation = { 0.0f, 0.0f, 0.0f };        // rotations are in radians

            Libdas::Matrix4<float> m_custom_transform;

        private:
            void _ApplyCustomTransform();

        public:
            NodeLoader(Renderer &_rend, const Libdas::DasNode &_node, Libdas::DasParser &_parser, uint32_t _camera_offset, std::vector<Animation> &_animation_samplers, std::vector<std::string> &_texture_names);
            ~NodeLoader();
            void Update(const Libdas::Matrix4<float> &_parent, float *morph_weights);

            inline const std::string &GetName() const {
                return m_node_name;
            }

            inline MeshLoader *GetMeshLoader() const {
                return mp_mesh_loader;
            }

            inline SkeletonDataManager *GetSkeleton() const {
                return mp_skeleton;
            }

            inline std::vector<NodeLoader> &GetChildNodes() {
                return m_child_nodes;
            }

            // custom transformations
            inline float GetCustomScale() const {
                return m_scale;
            }

            inline Libdas::Vector3<float> GetCustomTranslation() const {
                return m_translation;
            }

            inline Libdas::Point3D<float> GetCustomRotation() const {
                return m_rotation;
            }

            inline void SetCustomScale(float _scale) {
                m_scale = _scale;
                _ApplyCustomTransform();
            }

            inline void SetCustomTranslation(const Libdas::Vector3<float> &_translation) {
                m_translation = _translation;
                _ApplyCustomTransform();
            }

            inline void SetCustomRotation(const Libdas::Point3D<float> &_rot) {
                m_rotation = _rot;
                _ApplyCustomTransform();
            }
    };
}

#endif
