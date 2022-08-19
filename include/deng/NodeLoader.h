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
#ifdef __DEBUG
    #include <iostream>
#endif
    #include <string>
    #include <vector>
    #include <fstream>
    #include <unordered_map>
    #include <any>
    #include <algorithm>

    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Quaternion.h"

    #include "mar/AsciiStreamReader.h"
    #include "mar/AsciiLineReader.h"

    #include "das/Api.h"
    #include "das/ErrorHandlers.h"
    #include "das/DasStructures.h"
    #include "das/DasReaderCore.h"
    #include "das/DasParser.h"

    #include "deng/Api.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/Window.h"
    #include "deng/Renderer.h"
    #include "deng/ModelUniforms.h"
    #include "deng/AnimationSampler.h"
    #include "deng/MeshLoader.h"
    #include "deng/SkeletonLoader.h"
#endif

namespace DENG {

    // recursive class
    class DENG_API NodeLoader {
        friend class SceneLoader;
        private:
            Renderer &m_renderer;
            SkeletonLoader *mp_skeleton = nullptr;
            MeshLoader *mp_mesh_loader = nullptr;
            const Libdas::DasNode &m_node;
            Libdas::DasParser *mp_parser;

            std::vector<NodeLoader> m_child_nodes;
            std::vector<uint32_t> m_node_lookup;
            std::vector<std::pair<const bool*, AnimationSampler*>> m_animation_samplers;
            static uint32_t m_node_index;
            std::string m_node_name = "Unnamed node";

            // custom transformation properties
            TRS::Vector3<float> m_custom_translation = { 0.0f, 0.0f, 0.0f };
            TRS::Quaternion m_custom_rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
            float m_custom_scale = 1.0f;

            // transformation matrices
            TRS::Matrix4<float> m_parent;
            TRS::Matrix4<float> m_custom;
            TRS::Matrix4<float> m_transform;

            float *m_morph_weights = nullptr;
            uint32_t m_max_node = 0;
            const std::string &m_framebuffer_id;

        private:
            void _CreateBoundElementLoaders(std::vector<Animation> &_animations, const std::vector<uint32_t> &_main_buffer_offsets, uint32_t _camera_offset);
            void _SearchForBoundAnimationSamplers(std::vector<Animation> &_animations);
            void _UpdateTransformTRS(TRS::Vector3<float> _translation, TRS::Quaternion _rotation, float _scale);
            void _SetParser(Libdas::DasParser &_parser);
 
        public:
            NodeLoader(
                Renderer &_rend, 
                const Libdas::DasNode &_node, 
                Libdas::DasParser *_p_parser, 
                const std::vector<uint32_t> &_main_buffer_offsets, 
                uint32_t _camera_offset, 
                std::vector<Animation> &_animations, 
                const std::string &_framebuffer_id,
                const TRS::Matrix4<float> &_parent
            );
            NodeLoader(const NodeLoader &) = delete;
            NodeLoader(NodeLoader&& _node) noexcept;
            ~NodeLoader();

            void Update();

            inline void NewParent(const TRS::Matrix4<float> &_parent) {
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
            inline TRS::Vector3<float> &GetCustomTranslation() {
                return m_custom_translation;
            }

            inline TRS::Quaternion &GetCustomRotation() {
                return m_custom_rotation;
            }

            inline float &GetCustomScale() {
                return m_custom_scale;
            }

            inline void SetCustomScale(float _scale) {
                m_custom_scale = _scale;
            }

            inline void SetCustomTranslation(const TRS::Vector3<float> &_translation) {
                m_custom_translation = _translation;
            }

            inline void SetCustomRotation(const TRS::Quaternion &_rot) {
                m_custom_rotation = _rot;
            }
    };
}

#endif
