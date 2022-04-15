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

    #include <Api.h>
    #include <Window.h>
    #include <ShaderDefinitions.h>
    #include <Renderer.h>
    #include <ModelUniforms.h>
    #include <ModelShaderManager.h>
#endif

namespace DENG {

    class MeshLoader {
        private:
            uint32_t m_mesh_ref_id = UINT32_MAX;
            Libdas::DasParser &m_parser;
            const Libdas::DasMesh &m_mesh;
            Renderer &m_renderer;
            const uint32_t m_shader_id;
            const uint32_t m_ubo_offset;
            bool m_is_animation_target = false;
            std::string m_name = "Unnamed mesh";

            // Uniform node data
            Libdas::Matrix4<float> m_node_transform;
            Libdas::Matrix4<float> m_skeleton_transform;
            Libdas::Vector4<float> m_color = { 0.2f, 1.0f, 0.2f, 1.0f };

        public:
            MeshLoader(const Libdas::DasMesh &_mesh, Libdas::DasParser &_parser, Renderer &_renderer, uint32_t _shader_id, uint32_t _base_ubo_offset);
            void Attach();
            void UseTexture(const std::string &_name);

            inline uint32_t GetModelUboOffset() {
                return m_ubo_offset;
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

            // transform setters and getters
            inline Libdas::Matrix4<float> &GetNodeTransform() {
                return m_node_transform;
            }

            inline Libdas::Matrix4<float> &GetSkeletonTransform() {
                return m_skeleton_transform;
            }

            inline Libdas::Vector4<float> GetColor() {
                return m_color;
            }

            inline void SetColor(const Libdas::Vector4<float> &_color) {
                m_color = _color;
            }

            inline void SetNodeTransform(const Libdas::Matrix4<float> &_node) {
                m_node_transform = _node;
            }

            inline void SetSkeletonTransform(const Libdas::Matrix4<float> &_skeleton) {
                m_skeleton_transform = _skeleton;
            }

            inline void SetAnimationTargetFlag(bool _flag) {
                m_is_animation_target = _flag;
            }

            inline bool IsAnimationTarget() {
                return m_is_animation_target;
            }

    };
}

#endif

