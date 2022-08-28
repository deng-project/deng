// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: ImGuiLayer.h - ImGui gui abstraction class header
// author: Karl-Mihkel Ott

#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#ifdef IMGUI_LAYER_CPP
    #include <cstring>
    #include <string>
    #include <mutex>
    #include <vector>
    #include <chrono>
    #include <unordered_map>
#ifdef __DEBUG
    #include <iostream>
#endif

    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Matrix.h"

    #include "deng/Api.h"
    #include "deng/Window.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Renderer.h"
    #include "deng/GPUMemoryManager.h"
    #define MESH_NAME       "ImGui windows"
    #define TEXTURE_NAME    "ImGui texture atlas"

    #define VERT_SHADER_SRC "#version 450\n" \
                            "#extension GL_ARB_separate_shader_objects : enable\n" \
                            "layout(location = 0) in vec2 i_pos;\n" \
                            "layout(location = 1) in vec2 i_uv;\n" \
                            "layout(location = 2) in vec4 i_mul;\n" \
                            "layout(binding = 0) uniform UniformBufferData { \n" \
                            "    vec2 size; \n" \
                            "} ubo;\n" \
                            "" \
                            "layout(location = 0) out vec2 o_uv;\n" \
                            "layout(location = 1) out vec4 o_mul;\n" \
                            "" \
                            "void main() {\n" \
                            "    gl_Position = vec4((i_pos.x * 2 / ubo.size.x) - 1.0f, -(i_pos.y * 2 / ubo.size.y) + 1.0f, 0.0f, 1.0f);\n" \
                            "    o_uv = i_uv;\n" \
                            "    o_mul = i_mul;\n" \
                            "}\n"

    #define FRAG_SHADER_SRC "#version 450\n" \
                            "#extension GL_ARB_separate_shader_objects : enable\n" \
                            "layout(binding = 1) uniform sampler2D tex_sampler; \n" \
                            "layout(location = 0) in vec2 uv;\n" \
                            "layout(location = 1) in vec4 col_mul;\n" \
                            "layout(location = 0) out vec4 color;\n" \
                            "void main() {\n" \
                            "    color = col_mul * texture(tex_sampler, uv);\n" \
                            "}\n"

    #define IMGUI_TEXTURE_NAME "__imgui__"
#endif

#define IMGUI_CONTEXT_WINDOW_FLAGS ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground

#include <imgui.h>
#include <imgui_internal.h>

namespace DENG {

    typedef void(*PFN_ImGuiDrawCallback)(void *_user_data);

    class DENG_API ImGuiLayer {
        private:
            Window *mp_window = nullptr;
            Renderer *mp_renderer = nullptr;
            ImGuiIO *m_io = nullptr;
            ImGuiContext *m_context = nullptr;

            PFN_ImGuiDrawCallback m_callback = nullptr;
            uint32_t m_mesh_id = UINT32_MAX;
            uint32_t m_shader_id = UINT32_MAX;
            void *m_user_data = nullptr;
            uint32_t m_ubo_offset = UINT32_MAX;
            uint32_t m_main_offset = UINT32_MAX;
            TRS::Point2D<float> m_ubo;
            float m_delta_time = 1.0f;
            std::chrono::time_point<std::chrono::system_clock> m_beg;
            std::chrono::time_point<std::chrono::system_clock> m_end;
            const std::string m_gui_texture_name;
            std::pair<uint32_t, uint32_t> m_main_region = std::make_pair(UINT32_MAX, UINT32_MAX);
            const std::string m_framebuffer_name;

        private:
            uint32_t _CalculateUsedMemory(ImDrawData *_draw_data);
            ImGuiKey _TranslateKey(neko_HidEvent _key);
            void _CreateDrawCommands(ImDrawData *_draw_data);
            void _UpdateIO();

        public:
            ImGuiLayer(const std::string &_framebuffer_name = MAIN_FRAMEBUFFER_NAME);
            ~ImGuiLayer();
            void Attach(Window &_win, Renderer &_rend, PFN_ImGuiDrawCallback _callback, void *_user_data);
            void Update();

            inline ImGuiContext *GetContext() {
                return m_context;
            }

            inline void SetContext(ImGuiContext *_ctx) {
                ImGui::SetCurrentContext(_ctx);
            }
    };
}

#endif
