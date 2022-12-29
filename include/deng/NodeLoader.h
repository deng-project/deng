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
    #include <array>
    #include <fstream>
    #include <unordered_map>
    #include <any>
    #include <queue>
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
    #include "das/TextureReader.h"

    #include "deng/Api.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/Window.h"
    #include "deng/Renderer.h"
    #include "deng/Entity.h"
    #include "deng/ModelUniforms.h"
    #include "deng/VertexNormalVisualizer.h"
    #include "deng/AnimationSampler.h"
    #include "deng/MeshLoader.h"
    #include "deng/SkeletonLoader.h"
#endif

namespace DENG {

    // recursive class
    class DENG_API NodeLoader : public Entity {
        friend class SceneLoader;
        private:
            Renderer &m_renderer;
            SkeletonLoader *mp_skeleton = nullptr;
            MeshLoader *mp_mesh_loader = nullptr;
            const Libdas::DasNode &m_node;
            Libdas::DasModel *mp_model;

            std::vector<NodeLoader> m_child_nodes;
            std::vector<uint32_t> m_node_lookup;
            std::vector<std::pair<const bool*, AnimationSampler*>> m_animation_samplers;
            static uint32_t m_node_index;

            // custom transformation properties
            TRS::Vector3<float> m_custom_translation = { 0.0f, 0.0f, 0.0f };
            TRS::Quaternion m_custom_rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
            float m_custom_scale = 1.0f;

            // transformation matrices
            TRS::Matrix4<float> m_parent_matrix;
            TRS::Matrix4<float> m_custom;
            TRS::Matrix4<float> m_transform;

            float *m_morph_weights = nullptr;
            uint32_t m_max_node = 0;
            std::vector<uint32_t> m_framebuffers;

        private:
            void _CreateBoundElementLoaders(std::vector<Animation> &_animations, const std::vector<uint32_t> &_main_buffer_offsets, uint32_t _camera_offset);
            void _SearchForBoundAnimationSamplers(std::vector<Animation> &_animations);
            void _UpdateTransformTRS(TRS::Vector3<float> _translation, TRS::Quaternion _rotation, float _scale);
            void _SetModel(Libdas::DasModel &_model);
 
        public:
            NodeLoader(
                Entity *_parent,
                Renderer &_rend, 
                const Libdas::DasNode &_node, 
                Libdas::DasModel *_p_model, 
                const std::vector<uint32_t> &_main_buffer_offsets, 
                uint32_t _camera_offset, 
                std::vector<Animation> &_animations, 
                const std::vector<uint32_t>& _framebuffer_ids,
                const TRS::Matrix4<float> &_parent_matrix
            );
            NodeLoader(const NodeLoader &) = delete;
            NodeLoader(NodeLoader&& _node) noexcept;
            ~NodeLoader();

            void Update();

            inline void NewParentMatrix(const TRS::Matrix4<float> &_parent_mat) {
                m_parent_matrix = _parent_mat;
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
