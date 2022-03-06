// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ImGuiTest.h - API neutral ImGuiApp implementation
// author: Karl-Mihkel Ott

#ifndef IMGUI_TEST_H
#define IMGUI_TEST_H

#include "imgui_internal.h"
#if defined(_WIN32) && !defined(_DEBUG)
    #pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif

#include <string>
#include <chrono>
#include <vector>

#ifdef _DEBUG
    #include <iostream>
#endif

#include <libdas/include/Points.h>
#include <libdas/include/Vector.h>
#include <libdas/include/Matrix.h>

#include <Api.h>
#include <ErrorDefinitions.h>
#include <BaseTypes.h>
#include <Window.h>
#include <ShaderDefinitions.h>
#include <Renderer.h>

#if defined(USE_OPENGL)
    #include <OpenGLRenderer.h>
#elif defined(USE_VULKAN)
    #include <VulkanRenderer.h>
#endif

#include <imgui.h>

// Shader definitions
#define VERTEX_SHADER_SRC       "ImGuiAppData/VertexShader.glsl"
#define FRAGMENT_SHADER_SRC     "ImGuiAppData/FragmentShader.glsl"

// Mesh and texture definitions
#define MESH_NAME               "ImGui windows"
#define TEXTURE_NAME            "ImGui window textures"

class ImGuiApp {
    private:
        DENG::Window &m_window;
        DENG::Renderer &m_renderer;
        DENG::TextureReference m_texmap_ref = {};
        ImGuiIO *m_io = nullptr;
        DENG::MeshReference m_window_items = {};

    private:
        void _CreateDrawCommands(ImDrawData *_draw_data) {
            uint32_t vert_offset = 0;
            uint32_t idx_offset = 0;
            DENG::MeshReference &mesh = m_renderer.GetMeshes().front();
            mesh.commands.clear();

            for(int i = 0; i < _draw_data->CmdListsCount; i++) {
                const ImDrawList *cmd_list = _draw_data->CmdLists[i];
                const ImDrawVert *vert_buffer = cmd_list->VtxBuffer.Data;
                const ImDrawIdx *idx_buffer = cmd_list->IdxBuffer.Data;

                for(int j = 0; j < cmd_list->CmdBuffer.Size; j++) {
                    const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[j];
                    if(pcmd->UserCallback)
                        pcmd->UserCallback(cmd_list, pcmd);
                    else {
                        ImVec2 pos = _draw_data->DisplayPos;
                        m_renderer.UpdateVertexBuffer(std::make_pair(reinterpret_cast<const char*>(vert_buffer), static_cast<uint32_t>(cmd_list->VtxBuffer.Size) * sizeof(ImDrawVert)), vert_offset);
                        m_renderer.UpdateIndexBuffer(std::make_pair(reinterpret_cast<const char*>(idx_buffer), static_cast<uint32_t>(cmd_list->IdxBuffer.Size) * sizeof(ImDrawIdx)), idx_offset);
                        vert_offset += static_cast<uint32_t>(cmd_list->VtxBuffer.Size) * sizeof(ImDrawVert);
                        idx_offset += static_cast<uint32_t>(cmd_list->IdxBuffer.Size) * sizeof(ImDrawIdx);

                        // update the mesh instance for new draw calls
                        mesh.commands.emplace_back();
                        mesh.commands.back().vertices_offset = vert_offset;
                    }
                }
            }
        }

    public:
        ImGuiApp(DENG::Window &_win, DENG::Renderer &_rend) : m_window(_win), m_renderer(_rend) {
            ImGui::CreateContext();
            m_io = &ImGui::GetIO();

            unsigned char *pix;
            int width, height;
            m_io->Fonts->GetTexDataAsRGBA32(&pix, &width, &height);

            DENG::ShaderModule module = {};
            module.fragment_shader_file = FRAGMENT_SHADER_SRC;
            module.vertex_shader_file  = VERTEX_SHADER_SRC;
            module.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC2_FLOAT);
            module.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC2_FLOAT);
            module.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC4_SHORT);

            module.ubo_data_layouts.emplace_back();
            module.ubo_data_layouts.back().binding = 0;
            module.ubo_data_layouts.back().stage = SHADER_STAGE_FRAGMENT;
            module.ubo_data_layouts.back().type = DENG::UNIFORM_DATA_TYPE_IMAGE_SAMPLER;

            m_renderer.ClearFrame();
            uint32_t smi = m_renderer.PushShader(module);
            m_texmap_ref.shader_module_id = smi;
            m_texmap_ref.shader_sampler_id = 0;
            m_texmap_ref.name = "Texture map";

            // retrive texture map data
            m_renderer.PushTextureFromMemory(m_texmap_ref, reinterpret_cast<const char*>(pix), static_cast<uint32_t>(width), static_cast<uint32_t>(height), 4);
            m_io->Fonts->SetTexID((void*) &m_texmap_ref);
        }


        void Run() {
            auto beg = std::chrono::steady_clock::now();
            auto end = std::chrono::steady_clock::now();
            while(m_window.IsRunning()) {
                m_io->DeltaTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(end - beg).count()) / 1000;
                m_io->DisplaySize.x = m_window.GetSize().x;
                m_io->DisplaySize.y = m_window.GetSize().y;
                m_io->MousePos.x = static_cast<float>(m_window.GetMousePosition().x);
                m_io->MousePos.y = static_cast<float>(m_window.GetMousePosition().y);
                m_io->MouseDown[0] = m_window.IsKeyPressed(NEKO_MOUSE_BTN_1);
                m_io->MouseDown[1] = m_window.IsKeyPressed(NEKO_MOUSE_BTN_2);

                ImGui::NewFrame();
                ImGui::ShowDemoWindow();
                ImGui::EndFrame();

                ImGui::Render();
                ImDrawData *draw_data = ImGui::GetDrawData();
                _CreateDrawCommands(draw_data);
            }
        }
};

#endif
