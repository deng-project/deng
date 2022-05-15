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
#ifdef _DEBUG
    #include <iostream>
#endif
    #include <cstdint>
    #include <cstring>
    #include <cmath>
    #include <unordered_map>
    #include <vulkan/vulkan.h>

    #include <libdas/include/Vector.h>
    #include <libdas/include/Points.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Quaternion.h>
    #include <libdas/include/Api.h>
    #include <libdas/include/ErrorHandlers.h>
    #include <libdas/include/AsciiStreamReader.h>
    #include <libdas/include/AsciiLineReader.h>
    #include <libdas/include/DasStructures.h>
    #include <libdas/include/DasReaderCore.h>
    #include <libdas/include/DasParser.h>
    #include <libdas/include/Hash.h>

    #include <Api.h>
    #include <ErrorDefinitions.h>
    #include <Window.h>
    #include <ShaderDefinitions.h>
    #include <Renderer.h>
    #include <GPUMemoryManager.h>
    #include <ModelUniforms.h>
    #include <ModelShaderGenerator.h>
    #include <ModelShaderManager.h>
#endif

namespace DENG {

    class DENG_API MeshLoader {
        private:
            uint32_t m_mesh_ref_id = UINT32_MAX;
            Libdas::DasParser &m_parser;
            const Libdas::DasMesh &m_mesh;
            Renderer &m_renderer;
            static uint32_t m_mesh_index;
            std::string m_name = "Unnamed mesh";
            bool m_is_animation_target = false;
            uint32_t m_shader_id = UINT32_MAX;

            // singleton instance of ubo offset
            uint32_t m_mesh_ubo_offset = 0;
            uint32_t m_mesh_joints_ubo_offset = UINT32_MAX;
            const std::vector<uint32_t> &m_mesh_buffer_offsets;
            const uint32_t m_skeleton_joint_count;
            const Libdas::DasMeshPrimitive *mp_prim = nullptr;
            bool m_use_color = true;
            bool m_disable_joint_transforms = false;
            uint32_t m_supported_texture_count = 0;

            // Uniform node data
            Libdas::Vector4<float> m_color = { 0.2f, 1.0f, 0.2f, 1.0f };

        private:
            void _CheckMeshPrimitives();

        public:
            MeshLoader(const Libdas::DasMesh &_mesh, Libdas::DasParser &_parser, Renderer &_renderer, const std::vector<uint32_t> &_main_buffer_offsets, uint32_t _camera_offset, uint32_t _skeleton_joint_count);
            void Attach();
            void UseTextures(const std::vector<std::string> &_names);
            void UpdateJointMatrices(const std::vector<Libdas::Matrix4<float>> &_matrices);

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

            inline uint32_t GetShaderId() {
                return m_shader_id;
            }

            inline const std::string &GetName() const {
                return m_name;
            }

            inline bool GetUseColor() const {
                return m_use_color;
            }

            inline void SetUseColor(bool _use_color) {
                m_use_color = _use_color;
            }

            inline bool GetDisableJointTransforms() const {
                return m_disable_joint_transforms;
            }

            inline void SetDisableJointTransforms(bool _disable) {
                m_disable_joint_transforms = _disable;
            }

            inline Libdas::Vector4<float> GetColor() const {
                return m_color;
            }

            inline void SetColor(const Libdas::Vector4<float> &_color) {
                m_color = _color;
            }
    };
}

#endif

