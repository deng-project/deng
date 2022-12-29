// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: MeshLoader.h - DAS data MeshLoader class header
// author: Karl-Mihkel Ott

#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#ifdef MESH_LOADER_CPP
    #include <any>
    #include <vector>
    #include <string>
    #include <fstream>
#ifdef __DEBUG
    #include <iostream>
#endif
    #include <cstdint>
    #include <cstring>
    #include <cmath>
    #include <array>
    #include <unordered_map>
    #include <queue>
    #include <vulkan/vulkan.h>

    #include "trs/Vector.h"
    #include "trs/Points.h"
    #include "trs/Matrix.h"
    #include "trs/Quaternion.h"

    #include "mar/AsciiStreamReader.h"
    #include "mar/AsciiLineReader.h"

    #include "das/Api.h"
    #include "das/ErrorHandlers.h"
    #include "das/DasStructures.h"
    #include "das/DasReaderCore.h"
    #include "das/DasParser.h"
    #include "das/Hash.h"
    #include "das/TextureReader.h"

    #include "deng/Api.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/Window.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Renderer.h"
    #include "deng/Entity.h"
    #include "deng/GPUMemoryManager.h"
    #include "deng/ModelUniforms.h"
    #include "deng/ModelShaderGenerator.h"
    #include "deng/ModelShaderManager.h"
    #include "deng/ModelUniforms.h"
    #include "deng/VertexNormalVisualizer.h"
#endif

namespace DENG {

    class DENG_API MeshLoader : public Entity {
        friend class NodeLoader;
        private:
            uint32_t m_mesh_ref_id = UINT32_MAX;
            Libdas::DasModel *mp_model;
            const Libdas::DasMesh &m_mesh;
            Renderer &m_renderer;
            static uint32_t m_mesh_index;
            bool m_is_animation_target = false;
            uint32_t m_shader_id = UINT32_MAX;

            uint32_t m_main_memory_offset = 0;
            uint32_t m_mesh_ubo_offset = 0;
            uint32_t m_mesh_joints_ubo_offset = UINT32_MAX;

            // singleton instance of ubo offset
            const std::vector<uint32_t> &m_mesh_buffer_offsets;
            const uint32_t m_skeleton_joint_count;
            const Libdas::DasMeshPrimitive *mp_prim = nullptr;
            bool m_disable_joint_transforms = false;
            uint32_t m_supported_texture_count = 0;

            std::array<float, MAX_MORPH_TARGETS> m_morph_weights = {};

            // Uniform node data
            TRS::Vector4<float> m_color = { 0.2f, 1.0f, 0.2f, 1.0f };

            // framebuffer ids
            std::vector<uint32_t> m_framebuffer_ids;

            // mesh properties
            uint32_t m_environment_map = 0;
            bool m_is_colored = true;
            bool m_use_environment_mapping = false;

            std::vector<VertexNormalVisualizer> m_normal_visualizers;

        private:
            void _CheckMeshPrimitives();

        public:
            MeshLoader(
                Entity* _parent,
                const Libdas::DasMesh& _mesh,
                Libdas::DasModel* _model,
                Renderer& _renderer,
                const std::vector<uint32_t>& _main_buffer_offsets,
                uint32_t _camera_offset,
                uint32_t _skeleton_joint_count,
                const std::vector<uint32_t>& _framebuffers
            );
            MeshLoader(const MeshLoader &_ml) = delete;
            MeshLoader(MeshLoader&& _ml) noexcept;
            ~MeshLoader();

            void Attach();
            void UseTextures(const std::vector<uint32_t>& _handles);
            void UpdateJointMatrices(const std::vector<TRS::Matrix4<float>> &_matrices);

            inline std::array<float, MAX_MORPH_TARGETS> &GetMorphWeights() {
                return m_morph_weights;
            }

            inline uint32_t GetMeshUboOffset() {
                return m_mesh_ubo_offset;
            }

            inline uint32_t GetSupportedTextureCount() {
                return m_supported_texture_count;
            };

            inline const Libdas::DasMeshPrimitive *GetSamplePrimitive() {
                return mp_prim;
            }

            inline uint32_t &GetMeshReferenceId() {
                return m_mesh_ref_id;
            }

            inline std::vector<VertexNormalVisualizer>& GetNormalVisualizers() {
                return m_normal_visualizers;
            }

            inline const Libdas::DasMesh& GetDasMesh() {
                return m_mesh;
            }

            inline uint32_t GetShaderId() {
                return m_shader_id;
            }

            inline bool &GetUseColorBit() {
                return m_is_colored;
            }

            inline bool GetUseEnvironmentMapBit() {
                return m_use_environment_mapping;
            }

            inline void SetUseColorBit(bool _use_color) {
                m_is_colored = _use_color;
            }

            inline void SetUseEnvironmentMapBit(bool _use_env) {
                m_use_environment_mapping = _use_env;
            }

            void SetEnvironmentMap(uint32_t _handle);

            inline bool GetDisableJointTransforms() const {
                return m_disable_joint_transforms;
            }

            inline void SetDisableJointTransforms(bool _disable) {
                m_disable_joint_transforms = _disable;
            }

            inline TRS::Vector4<float> GetColor() const {
                return m_color;
            }

            inline void SetColor(const TRS::Vector4<float> &_color) {
                m_color = _color;
            }
    };
}

#endif

