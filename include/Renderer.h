// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Renderer.h - Base renderer class header
// author: Karl-Mihkel Ott

#ifndef RENDERER_H
#define RENDERER_H

#ifdef RENDERER_CPP
    #include <string>
    #include <cstring>
    #include <vector>
#ifdef _DEBUG
    #include <iostream>
#endif

    #include <libdas/include/Points.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>

    #include <Api.h>
    #include <ShaderDefinitions.h>
    #include <Window.h>
#endif

#define MAIN_FRAMEBUFFER_NAME   "__main_fb__"
#include <Missing.h>

namespace DENG {

    struct DrawCommand {
        uint32_t indices_offset = 0;
        uint32_t draw_count = 0; // if used with indices, this property specifies index count, otherwise it describes attribute count
        std::vector<std::string> texture_names;
        std::vector<std::size_t> attribute_offsets;

        struct {
            Libdas::Point2D<int32_t> offset = { INT32_MAX, INT32_MAX };
            Libdas::Point2D<uint32_t> ext = { INT32_MAX, INT32_MAX };
            bool enabled = false;
        } scissor;
    };


    struct MeshReference {
        std::string name = "Mesh";
        uint32_t shader_module_id = 0;
        std::vector<DrawCommand> commands;
        std::vector<UniformBufferBlock> ubo_blocks;
    };


    struct FramebufferDrawData {
        std::string image_name;
        std::vector<MeshReference> meshes;
        std::vector<ShaderModule> shaders;
        Libdas::Point2D<uint32_t> extent;
    };


    struct RendererConfig {
        bool enable_vsync = false;
        Libdas::Vector4<float> clear_color = { 0.0f, 0.0f, 0.0f, 0.0f };
    };


    class DENG_API Renderer {
        protected:
            uint32_t m_id = UINT32_MAX;
            Window &m_window;
            std::unordered_map<std::string, FramebufferDrawData> m_framebuffer_draws;
            const RendererConfig &m_conf;

        public:
            Renderer(Window &_win, const RendererConfig &_conf) : m_window(_win), m_conf(_conf) {}
            ~Renderer() {}

            virtual void PushFramebuffer(const FramebufferDrawData &_fb) = 0;

            inline uint32_t PushMeshReference(const MeshReference &_mesh, const std::string &_framebuffer = MAIN_FRAMEBUFFER_NAME) {
                DENG_ASSERT(m_framebuffer_draws.find(_framebuffer) != m_framebuffer_draws.end());
                m_framebuffer_draws[_framebuffer].meshes.push_back(_mesh);
                return static_cast<uint32_t>(m_framebuffer_draws[_framebuffer].meshes.size() - 1);
            }

            inline uint32_t NewMeshReference(const std::string &_framebuffer = MAIN_FRAMEBUFFER_NAME) {
                DENG_ASSERT(m_framebuffer_draws.find(_framebuffer) != m_framebuffer_draws.end());
                m_framebuffer_draws[_framebuffer].meshes.emplace_back();
                return static_cast<uint32_t>(m_framebuffer_draws[_framebuffer].meshes.size() - 1);
            }

            inline MeshReference RemoveMeshReference(uint32_t _id, const std::string &_framebuffer = MAIN_FRAMEBUFFER_NAME) {
                DENG_ASSERT(m_framebuffer_draws.find(_framebuffer) != m_framebuffer_draws.end());
                DENG_ASSERT(_id < static_cast<uint32_t>(m_framebuffer_draws[_framebuffer].meshes.size()));
                MeshReference mesh = m_framebuffer_draws[_framebuffer].meshes[_id];
                m_framebuffer_draws[_framebuffer].meshes.erase(m_framebuffer_draws[_framebuffer].meshes.begin() + _id);
                return mesh;
            }

            inline uint32_t PushShader(const ShaderModule &_module, const std::string &_framebuffer = MAIN_FRAMEBUFFER_NAME) {
                DENG_ASSERT(m_framebuffer_draws.find(_framebuffer) != m_framebuffer_draws.end());
                m_framebuffer_draws[_framebuffer].shaders.push_back(_module);
                return static_cast<uint32_t>(m_framebuffer_draws[_framebuffer].shaders.size() - 1);
            }

            virtual void PushTextureFromFile(const std::string &_name, const std::string &_file_name) = 0;
            virtual void PushTextureFromMemory(const std::string &_name, const char* _raw_data, uint32_t _width, uint32_t _height, uint32_t _bit_depth) = 0;
            virtual void RemoveTexture(const std::string &_name) = 0;

            std::vector<MeshReference> &GetMeshes(const std::string &_framebuffer = MAIN_FRAMEBUFFER_NAME) {
                DENG_ASSERT(m_framebuffer_draws.find(_framebuffer) != m_framebuffer_draws.end());
                return m_framebuffer_draws[_framebuffer].meshes;
            }

            std::vector<ShaderModule> &GetShaderModules(const std::string &_framebuffer = MAIN_FRAMEBUFFER_NAME) {
                DENG_ASSERT(m_framebuffer_draws.find(_framebuffer) != m_framebuffer_draws.end());
                return m_framebuffer_draws[_framebuffer].shaders;
            }

            // slow due to the heap allocation
            virtual std::vector<std::string> GetTextureNames() = 0;

            virtual uint32_t AlignUniformBufferOffset(uint32_t _req) = 0;
            virtual void LoadShaders(uint32_t _ubo_size = 0) = 0;
            virtual void UpdateUniform(const char *_raw_data, uint32_t _size, uint32_t _offset) = 0;
            virtual void UpdateVertexDataBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset = 0) = 0;
            virtual void ClearFrame() = 0;
            virtual void RenderFrame() = 0;
    };
}

#endif
