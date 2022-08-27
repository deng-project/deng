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

#ifdef __DEBUG
    #include <iostream>
#endif

    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Matrix.h"

    #include "deng/Api.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Window.h"
#endif

#define MAIN_FRAMEBUFFER_NAME   "__main_fb__"
#include "deng/Missing.h"

namespace DENG {

    struct DrawCommand {
        uint32_t indices_offset = 0;
        uint32_t draw_count = 0; // if used with indices, this property specifies index count, otherwise it describes attribute count
        std::vector<std::string> texture_names;
        std::vector<std::size_t> attribute_offsets;

        struct {
            TRS::Point2D<int32_t> offset = { INT32_MAX, INT32_MAX };
            TRS::Point2D<uint32_t> ext = { INT32_MAX, INT32_MAX };
            bool enabled = false;
        } scissor;
    };


    struct MeshReference {
        std::string name = "Mesh";
        uint32_t shader_module_id = 0;
        std::vector<DrawCommand> commands;
        std::vector<UniformBufferBlock> ubo_blocks;
    };

    enum ResourceState {
        RESOURCE_ADDED,
        RESOURCE_LOADED,
        RESOURCE_DESTROYED
    };

    struct FramebufferDrawData {
        std::string image_name;
        std::vector<std::pair<MeshReference, ResourceState>> meshes;
        std::vector<std::pair<ShaderModule, ResourceState>> shaders;
        TRS::Point2D<uint32_t> extent;
    };


    struct RendererConfig {
#ifdef _WIN32
        HINSTANCE win32_instance;
        HWND win32_hwnd;
#else
        Display* xlib_dpy;
        Window xlib_win;
#endif
        std::string title;
        TRS::Point2D<uint32_t> canvas_size;
        bool enable_vsync = false;
        TRS::Vector4<float> clear_color = { 0.0f, 0.0f, 0.0f, 0.0f };
    };


    class DENG_API Renderer {
        protected:
            uint32_t m_id = UINT32_MAX;
            std::unordered_map<std::string, FramebufferDrawData> m_framebuffer_draws;
            const RendererConfig &m_conf;

        public:
            Renderer(const RendererConfig &_conf) : m_conf(_conf) {}
            ~Renderer() {}

            virtual void PushFramebuffer(const FramebufferDrawData &_fb) = 0;

            inline uint32_t PushMeshReference(const MeshReference &_mesh, const std::string &_framebuffer = MAIN_FRAMEBUFFER_NAME) {
                DENG_ASSERT(m_framebuffer_draws.find(_framebuffer) != m_framebuffer_draws.end());
                m_framebuffer_draws[_framebuffer].meshes.push_back(std::make_pair(_mesh, RESOURCE_ADDED));
                return static_cast<uint32_t>(m_framebuffer_draws[_framebuffer].meshes.size() - 1);
            }

            inline uint32_t NewMeshReference(const std::string &_framebuffer = MAIN_FRAMEBUFFER_NAME) {
                DENG_ASSERT(m_framebuffer_draws.find(_framebuffer) != m_framebuffer_draws.end());
                m_framebuffer_draws[_framebuffer].meshes.emplace_back();
                m_framebuffer_draws[_framebuffer].meshes.back().second = RESOURCE_ADDED;
                return static_cast<uint32_t>(m_framebuffer_draws[_framebuffer].meshes.size() - 1);
            }

            // it doesn't clean renderer resources
            inline MeshReference RemoveMeshReference(uint32_t _id, const std::string &_framebuffer = MAIN_FRAMEBUFFER_NAME) {
                DENG_ASSERT(m_framebuffer_draws.find(_framebuffer) != m_framebuffer_draws.end());
                DENG_ASSERT(_id < static_cast<uint32_t>(m_framebuffer_draws[_framebuffer].meshes.size()));
                MeshReference mesh = m_framebuffer_draws[_framebuffer].meshes[_id].first;
                m_framebuffer_draws[_framebuffer].meshes.erase(m_framebuffer_draws[_framebuffer].meshes.begin() + _id);
                return mesh;
            }

            inline uint32_t PushShader(const ShaderModule &_module, const std::string &_framebuffer = MAIN_FRAMEBUFFER_NAME) {
                DENG_ASSERT(m_framebuffer_draws.find(_framebuffer) != m_framebuffer_draws.end());
                m_framebuffer_draws[_framebuffer].shaders.push_back(std::make_pair(_module, RESOURCE_ADDED));
                return static_cast<uint32_t>(m_framebuffer_draws[_framebuffer].shaders.size() - 1);
            }

            virtual void PushTextureFromFile(const std::string &_name, const std::string &_file_name) = 0;
            virtual void PushTextureFromMemory(const std::string &_name, const char* _raw_data, uint32_t _width, uint32_t _height, uint32_t _bit_depth) = 0;
            virtual void RemoveTexture(const std::string &_name) = 0;

            MeshReference &GetMesh(uint32_t _id, const std::string &_framebuffer = MAIN_FRAMEBUFFER_NAME) {
                DENG_ASSERT(m_framebuffer_draws.find(_framebuffer) != m_framebuffer_draws.end());
                DENG_ASSERT(_id < static_cast<uint32_t>(m_framebuffer_draws.find(_framebuffer)->second.meshes.size()));
                return m_framebuffer_draws.find(_framebuffer)->second.meshes[_id].first;
            }

            ShaderModule &GetShaderModule(uint32_t _id, const std::string &_framebuffer = MAIN_FRAMEBUFFER_NAME) {
                DENG_ASSERT(m_framebuffer_draws.find(_framebuffer) != m_framebuffer_draws.end());
                DENG_ASSERT(_id < static_cast<uint32_t>(m_framebuffer_draws.find(_framebuffer)->second.shaders.size()));
                return m_framebuffer_draws.find(_framebuffer)->second.shaders[_id].first;
            }

            inline const RendererConfig& GetConfig() {
                return m_conf;
            }

            // slow due to the heap allocation
            virtual std::vector<std::string> GetTextureNames() = 0;

            virtual uint32_t AlignUniformBufferOffset(uint32_t _req) = 0;
            virtual void LoadShaders() = 0;
            virtual void UpdateUniform(const char *_raw_data, uint32_t _size, uint32_t _offset) = 0;
            virtual void UpdateVertexDataBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset = 0) = 0;
            virtual void ClearFrame() = 0;
            virtual void RenderFrame() = 0;
    };
}

#endif
