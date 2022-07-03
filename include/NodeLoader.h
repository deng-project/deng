// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: NodeLoader.h - DAS model node transformation handler header
// author: Karl-Mihkel Ott

#ifndef NODE_LOADER_H
#define NODE_LOADER_H

#ifdef NODE_LOADER_CPP
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
    #include <SkeletonLoader.h>
#endif

namespace DENG {

    // recursive class
    class DENG_API NodeLoader {
        private:
            Renderer &m_renderer;
            SkeletonLoader *mp_skeleton = nullptr;
            MeshLoader *mp_mesh_loader = nullptr;
            const Libdas::DasNode &m_node;
            Libdas::DasParser &m_parser;

            std::vector<NodeLoader> m_child_nodes;
            std::vector<uint32_t> m_node_lookup;
            std::vector<std::pair<const bool*, AnimationSampler*>> m_animation_samplers;
            static uint32_t m_node_index;
            std::string m_node_name = "Unnamed node";

            // custom transformation properties
            Libdas::Vector3<float> m_custom_translation = { 0.0f, 0.0f, 0.0f };
            Libdas::Quaternion m_custom_rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
            float m_custom_scale = 1.0f;

            // transformation matrices
            Libdas::Matrix4<float> m_parent;
            Libdas::Matrix4<float> m_custom;
            Libdas::Matrix4<float> m_transform;

            float *m_morph_weights = nullptr;
            uint32_t m_max_node = 0;
            const std::string &m_framebuffer_id;

        private:
            void _CreateBoundElementLoaders(std::vector<Animation> &_animations, const std::vector<uint32_t> &_main_buffer_offsets, uint32_t _camera_offset);
            void _SearchForBoundAnimationSamplers(std::vector<Animation> &_animations);
            void _UpdateTransformTRS(Libdas::Vector3<float> _translation, Libdas::Quaternion _rotation, float _scale);
 
        public:
            NodeLoader(
                Renderer &_rend, 
                const Libdas::DasNode &_node, 
                Libdas::DasParser &_parser, 
                const std::vector<uint32_t> &_main_buffer_offsets, 
                uint32_t _camera_offset, 
                std::vector<Animation> &_animations, 
                const std::string &_framebuffer_id,
                const Libdas::Matrix4<float> &_parent
            );
            NodeLoader(const NodeLoader &) = delete;
            NodeLoader(NodeLoader&& _node) noexcept;
            ~NodeLoader();

            void Update();

            inline void NewParent(const Libdas::Matrix4<float> &_parent) {
                m_parent = _parent;
            }

            inline const std::string &GetName() const {
                return m_node_name;
            }

            inline MeshLoader *GetMeshLoader() const {
                return mp_mesh_loader;
            }

            inline SkeletonLoader *GetSkeleton() const {
                return mp_skeleton;
            }

            inline std::vector<NodeLoader> &GetChildNodes() {
                return m_child_nodes;
            }

            // custom transformations
            inline Libdas::Vector3<float> GetCustomTranslation() const {
                return m_custom_translation;
            }

            inline Libdas::Quaternion GetCustomRotation() const {
                return m_custom_rotation;
            }

            inline float GetCustomScale() const {
                return m_custom_scale;
            }

            inline void SetCustomScale(float _scale) {
                m_custom_scale = _scale;
            }

            inline void SetCustomTranslation(const Libdas::Vector3<float> &_translation) {
                m_custom_translation = _translation;
            }

            inline void SetCustomRotation(const Libdas::Quaternion &_rot) {
                m_custom_rotation = _rot;
            }
    };
}

#endif
