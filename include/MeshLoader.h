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
            static uint32_t m_ubo_offset;
            static uint32_t m_main_buffer_offset;
            uint32_t m_mesh_ubo_offset;
            const uint32_t m_skeleton_joint_count;
            const Libdas::DasMeshPrimitive *mp_prim = nullptr;
            bool m_use_color = true;
#ifdef _DEBUG
            bool m_disable_joint_transforms = false;
#endif

            // Uniform node data
            Libdas::Vector4<float> m_color = { 0.2f, 1.0f, 0.2f, 1.0f };

        private:
            void _CheckMeshPrimitives();

        public:
            MeshLoader(const Libdas::DasMesh &_mesh, Libdas::DasParser &_parser, Renderer &_renderer, uint32_t _camera_offset, uint32_t _skeleton_joint_count);
            void Attach();
            void UseTextures(const std::vector<std::string> &_names);
            void UpdateJointMatrices(const std::vector<Libdas::Matrix4<float>> &_matrices);

            // static member getters / setters
            static void SetUniformBufferOffset(uint32_t _ubo_offset);
            static uint32_t GetUboOffset();
            static void SetMainBufferOffset(uint32_t _offset);
            static uint32_t GetMainBufferOffset();

            inline const Libdas::DasMeshPrimitive *GetSamplePrimitive() {
                return mp_prim;
            }

            inline uint32_t GetMeshUboOffset() {
                return m_mesh_ubo_offset;
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

            static uint32_t CalculateAbsoluteOffset(const Libdas::DasParser &_parser, uint32_t _buffer_id, uint32_t _buffer_offset);

            inline bool GetUseColor() const {
                return m_use_color;
            }

            inline void SetUseColor(bool _use_color) {
                m_use_color = _use_color;
            }

#ifdef _DEBUG
            inline bool GetDisableJointTransforms() const {
                return m_disable_joint_transforms;
            }

            inline void SetDisableJointTransforms(bool _disable) {
                m_disable_joint_transforms = _disable;
            }
#endif

            inline Libdas::Vector4<float> GetColor() const {
                return m_color;
            }

            inline void SetColor(const Libdas::Vector4<float> &_color) {
                m_color = _color;
            }
    };
}

#endif

