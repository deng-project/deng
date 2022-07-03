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
#ifdef DENG_EDITOR
    #include <algorithm>
#endif

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
    #include <ModelUniforms.h>
#endif

namespace DENG {

    class DENG_API MeshLoader {
        private:
            uint32_t m_mesh_ref_id = UINT32_MAX;
            Libdas::DasParser &m_parser;
            const Libdas::DasMesh &m_mesh;
            Renderer &m_renderer;
            static uint32_t m_mesh_index;
            std::string m_name = "Unnamed mesh" + std::to_string(m_mesh_index++);
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

            float m_morph_weights[MAX_MORPH_TARGETS] = {};
            //bool m_is_attached = false;

            // Uniform node data
            Libdas::Vector4<float> m_color = { 0.2f, 1.0f, 0.2f, 1.0f };

            // framebuffer id
            const std::string m_framebuffer_id;

            bool m_is_colored = true;
#ifdef DENG_EDITOR
            uint32_t m_used_textures = 0;
            std::string m_inspector_name;
            std::string m_color_checkbox_id;
            std::string m_color_picker_id;
            std::string m_texture_button_id;
            std::string m_texture_picker_id;
            std::vector<bool> m_texture_table;
            std::string m_texture_save_id;
#endif

        private:
            void _CheckMeshPrimitives();

        public:
            MeshLoader(
                const Libdas::DasMesh &_mesh, 
                Libdas::DasParser &_parser, 
                Renderer &_renderer, 
                const std::vector<uint32_t> &_main_buffer_offsets, 
                uint32_t _camera_offset, 
                uint32_t _skeleton_joint_count,
                const std::string &_framebuffer_name
            );
            MeshLoader(const MeshLoader &_ml) = delete;
            MeshLoader(MeshLoader&& _ml) noexcept;
            ~MeshLoader();

            void Attach();
            void UseTextures(const std::vector<std::string> &_names);
            void UpdateJointMatrices(const std::vector<Libdas::Matrix4<float>> &_matrices);

            inline float *GetMorphWeights() {
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

            inline uint32_t GetShaderId() {
                return m_shader_id;
            }

            inline const std::string &GetName() const {
                return m_name;
            }

            inline bool &GetUseColor() {
                return m_is_colored;
            }

            inline void SetUseColor(bool _use_color) {
                m_is_colored = _use_color;
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

            // editor getters
#ifdef DENG_EDITOR
            inline uint32_t &GetUsedTextureCount() {
                return m_used_textures;
            }

            inline std::string &GetInspectorName() {
                return m_inspector_name;
            }

            inline std::string &GetColorCheckboxId() {
                return m_color_checkbox_id;
            }

            inline std::string &GetColorPickerId() {
                return m_color_picker_id;
            }

            inline std::string &GetTextureButtonId() {
                return m_texture_button_id;
            }

            inline std::string &GetTexturePickerId() {
                return m_texture_picker_id;
            }

            inline std::vector<bool> &GetTextureTable() {
                return m_texture_table;
            }

            inline std::string &GetTextureSaveId() {
                return m_texture_save_id;
            }
#endif
    };
}

#endif

