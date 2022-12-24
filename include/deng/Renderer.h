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
    #include <queue>

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

#include "deng/Missing.h"

namespace DENG {

    struct DrawCommand {
        uint32_t indices_offset = 0;
        uint32_t draw_count = 0; // if used with indices, this property specifies index count, otherwise it describes attribute count
        std::vector<uint32_t> texture_ids;
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
        std::vector<uint32_t> framebuffer_ids;
        std::vector<DrawCommand> commands;
        std::vector<UniformDataLayout> ubo_data_layouts;
        bool enable = true;
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

    struct FramebufferIndices {
        FramebufferIndices() = default;
        FramebufferIndices(const FramebufferIndices&) = default;
        FramebufferIndices(uint32_t _tid, uint32_t _fid) :
            texture_id(_tid), framebuffer_id(_fid) {}

        uint32_t texture_id;
        uint32_t framebuffer_id;
    };


    class DENG_API Renderer {
        protected:
            const RendererConfig &m_conf;
            std::vector<MeshReference*> m_meshes;
            std::vector<ShaderModule*> m_shader_modules;

            std::queue<MeshReference> m_removed_mesh_queue;
            std::queue<ShaderModule> m_removed_shader_queue;

        public:
            Renderer(const RendererConfig &_conf) : m_conf(_conf) {}
            ~Renderer() {}

            // .x - image id
            // .y - framebuffer id
            virtual FramebufferIndices AddFramebuffer(TRS::Point2D<uint32_t> _extent) = 0;

            inline uint32_t PushMeshReference(const MeshReference &_mesh) {
                m_meshes.push_back(new MeshReference(_mesh));
                return static_cast<uint32_t>(m_meshes.size() - 1);
            }

            inline uint32_t NewMeshReference() {
                m_meshes.push_back(new MeshReference);
                return static_cast<uint32_t>(m_meshes.size() - 1);
            }

            // it doesn't clean renderer resources
            inline void RemoveMeshReference(uint32_t _id) {
                if (_id >= static_cast<uint32_t>(m_meshes.size()))
                    throw std::runtime_error("Cannot remove mesh with " + std::to_string(_id) + " (index out of bounds)");
                else if (m_meshes[_id] == nullptr)
                    throw std::runtime_error("Mesh with id " + std::to_string(_id) + " is already removed");

                m_removed_mesh_queue.push(*m_meshes[_id]);
                delete m_meshes[_id];
                m_meshes[_id] = nullptr;
            }

            inline uint32_t PushShaderModule(const ShaderModule &_module) {
                m_shader_modules.push_back(new ShaderModule(_module));
                return static_cast<uint32_t>(m_shader_modules.size() - 1);
            }

            MeshReference &GetMesh(uint32_t _id) {
                if (_id >= static_cast<uint32_t>(m_meshes.size()))
                    throw std::runtime_error("Cannot get mesh with id " + std::to_string(_id) + " (index out of bounds)");
                else if (m_meshes[_id] == nullptr)
                    throw std::runtime_error("Cannot get mesh with id " + std::to_string(_id) + " (mesh is removed)");
                
                return *m_meshes[_id];
            }

            ShaderModule &GetShaderModule(uint32_t _id) {
                if (_id >= static_cast<uint32_t>(m_shader_modules.size()))
                    throw std::runtime_error("Cannot get shader module with id " + std::to_string(_id) + " (index out of bounds)");
                else if (m_shader_modules[_id] == nullptr)
                    throw std::runtime_error("Cannot get shader module with id " + std::to_string(_id) + " (shader module is removed)");
                
                return *m_shader_modules[_id];
            }

            inline const RendererConfig& GetConfig() {
                return m_conf;
            }

            // slow due to the heap allocation
            virtual std::vector<std::string> GetTextureNames() = 0;

            virtual uint32_t AlignUniformBufferOffset(uint32_t _req) = 0;
            virtual void LoadShaders() = 0;
            virtual void ReloadShaderModule(uint32_t _shader_id) = 0;
            virtual void UpdateUniform(const char *_raw_data, uint32_t _size, uint32_t _offset) = 0;
            virtual void UpdateVertexDataBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset = 0) = 0;
            virtual void ClearFrame() = 0;
            virtual void RenderFrame() = 0;
    };
}

#endif
