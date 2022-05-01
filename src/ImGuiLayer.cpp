// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: ImGuiLayer.h - ImGui gui abstraction class implementation
// author: Karl-Mihkel Ott

#define IMGUI_LAYER_CPP
#include <ImGuiLayer.h>

namespace DENG {

    ImGuiLayer::ImGuiLayer() {
        m_beg = std::chrono::system_clock::now();
        m_end = std::chrono::system_clock::now();
    }

    ImGuiLayer::~ImGuiLayer() {
        ImGui::DestroyContext();
    }
    
    void ImGuiLayer::_CreateDrawCommands(ImDrawData *_draw_data, uint32_t _combined_offset) {
        DENG::MeshReference &mesh = mp_renderer->GetMeshes()[m_mesh_id];
        mesh.shader_module_id = m_shader_id;
        mesh.commands.clear();

        uint32_t cmd_vert_offset = _combined_offset;
        uint32_t cmd_idx_offset = _combined_offset;

        for(int i = 0; i < _draw_data->CmdListsCount; i++) {

            const ImDrawList *cmd_list = _draw_data->CmdLists[i];
            const ImDrawVert *vert_buffer = cmd_list->VtxBuffer.Data;
            const ImDrawIdx *idx_buffer = cmd_list->IdxBuffer.Data;

            cmd_idx_offset += static_cast<uint32_t>(cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));

            mp_renderer->UpdateVertexDataBuffer(std::make_pair(reinterpret_cast<const char*>(vert_buffer), static_cast<uint32_t>(cmd_list->VtxBuffer.Size * sizeof(ImDrawVert))), cmd_vert_offset);
            mp_renderer->UpdateVertexDataBuffer(std::make_pair(reinterpret_cast<const char*>(idx_buffer), static_cast<uint32_t>(cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx))), cmd_idx_offset);

            const uint32_t used_size = cmd_idx_offset + cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);

            for(int j = 0; j < cmd_list->CmdBuffer.Size; j++) {
                const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[j];
                if(pcmd->UserCallback)
                    pcmd->UserCallback(cmd_list, pcmd);
                else {
                    ImVec2 pos = _draw_data->DisplayPos;
                    ImVec4 clip = pcmd->ClipRect;
                    clip.x -= pos.x; clip.y -= pos.y;
                    clip.z -= pos.x; clip.w -= pos.y;

                    // clamp to viewport
                    if(clip.x < 0.0f) clip.x = 0.0f;
                    if(clip.y < 0.0f) clip.y = 0.0f;
                    if(clip.z > mp_window->GetSize().x) clip.z = mp_window->GetSize().x;
                    if(clip.w > mp_window->GetSize().y) clip.w = mp_window->GetSize().y;

                    if(clip.z <= clip.x || clip.w <= clip.y)
                        continue;

                    // update the mesh instance for new draw calls
                    mesh.commands.emplace_back();
                    const uint32_t base_vertex_offset = cmd_vert_offset + pcmd->VtxOffset * sizeof(ImDrawVert);
                    mesh.commands.back().attribute_offsets.push_back(base_vertex_offset + offsetof(ImDrawVert, pos));
                    mesh.commands.back().attribute_offsets.push_back(base_vertex_offset + offsetof(ImDrawVert, uv));
                    mesh.commands.back().attribute_offsets.push_back(base_vertex_offset + offsetof(ImDrawVert, col));
                    mesh.commands.back().indices_offset = cmd_idx_offset + pcmd->IdxOffset * sizeof(ImDrawIdx);
                    mesh.commands.back().indices_count = pcmd->ElemCount;
                    mesh.commands.back().texture_names.push_back(IMGUI_TEXTURE_NAME);
                    mesh.commands.back().scissor.offset = { static_cast<int32_t>(clip.x), static_cast<int32_t>(clip.y) };
                    mesh.commands.back().scissor.ext = { static_cast<uint32_t>(clip.z - clip.x), static_cast<uint32_t>(clip.w - clip.y) };
                    mesh.commands.back().scissor.enabled = true;
                }
            }

            cmd_vert_offset += used_size;
            cmd_idx_offset += used_size;
        }
    }


    void ImGuiLayer::_UpdateIO() {
        m_io->DeltaTime = 1.0f;
        m_io->DisplaySize.x = mp_window->GetSize().x;
        m_io->DisplaySize.y = mp_window->GetSize().y;
        m_io->MousePos.x = static_cast<float>(mp_window->GetMousePosition().x);
        m_io->MousePos.y = static_cast<float>(mp_window->GetMousePosition().y);
        m_io->MouseDown[0] = mp_window->IsKeyPressed(NEKO_MOUSE_BTN_1);
        m_io->MouseDown[1] = mp_window->IsKeyPressed(NEKO_MOUSE_BTN_2);

        if(mp_window->IsKeyPressed(NEKO_MOUSE_SCROLL_DOWN))
            m_io->MouseWheel = -1;
        else if(mp_window->IsKeyPressed(NEKO_MOUSE_SCROLL_UP))
            m_io->MouseWheel = 1;
        else m_io->MouseWheel = 0;
    }


    void ImGuiLayer::Attach(Window &_win, Renderer &_rend, uint32_t _ubo_offset, PFN_ImGuiDrawCallback _callback, void *_user_data) {
        m_ubo_offset = _ubo_offset;
        mp_window = &_win;
        mp_renderer = &_rend;
        m_callback = _callback;
        m_user_data = _user_data;

        ImGui::CreateContext();
        m_io = &ImGui::GetIO();

        // retrieve texture atlas data
        unsigned char *pix;
        int width, height;
        m_io->Fonts->GetTexDataAsRGBA32(&pix, &width, &height);

        DENG::ShaderModule module;
        module.fragment_shader_src = FRAG_SHADER_SRC;
        module.vertex_shader_src = VERT_SHADER_SRC;
        module.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC2_FLOAT);
        module.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC2_FLOAT);
        module.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC4_UBYTE);
        module.attribute_strides.push_back(sizeof(ImDrawVert));
        module.attribute_strides.push_back(sizeof(ImDrawVert));
        module.attribute_strides.push_back(sizeof(ImDrawVert));
        module.enable_blend = true;
        module.enable_scissor = true;
        module.load_shaders_from_file = false;
        module.use_texture_mapping = true;

        module.ubo_data_layouts.reserve(2);
        module.ubo_data_layouts.emplace_back();
        module.ubo_data_layouts.back().block.binding = 0;
        module.ubo_data_layouts.back().stage = SHADER_STAGE_VERTEX;
        module.ubo_data_layouts.back().type = DENG::UNIFORM_DATA_TYPE_BUFFER;
        module.ubo_data_layouts.back().block.size = static_cast<uint32_t>(sizeof(Libdas::Point2D<float>));
        module.ubo_data_layouts.back().block.offset = m_ubo_offset;

        module.ubo_data_layouts.emplace_back();
        module.ubo_data_layouts.back().block.binding = 1;
        module.ubo_data_layouts.back().stage = SHADER_STAGE_FRAGMENT;
        module.ubo_data_layouts.back().type = DENG::UNIFORM_DATA_TYPE_IMAGE_SAMPLER;

        m_shader_id = mp_renderer->PushShader(module);

        m_mesh_id = mp_renderer->PushMeshReference(DENG::MeshReference());
        mp_renderer->PushTextureFromMemory(IMGUI_TEXTURE_NAME, reinterpret_cast<const char*>(pix), static_cast<uint32_t>(width), static_cast<uint32_t>(height), 4);
        m_io->Fonts->SetTexID(reinterpret_cast<void*>(0));
    }


    void ImGuiLayer::Update(uint32_t _combined_offset) {
        m_end = std::chrono::system_clock::now();
        m_delta_time = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(m_end - m_beg).count());
        m_beg = std::chrono::system_clock::now();
        _UpdateIO();
        ImGui::NewFrame();
            m_callback(m_user_data);
        ImGui::EndFrame();

        ImGui::Render();

        ImDrawData *draw_data = ImGui::GetDrawData();
        _CreateDrawCommands(draw_data, _combined_offset);

        Libdas::Point2D<float> wsize = { static_cast<float>(mp_window->GetSize().x), static_cast<float>(mp_window->GetSize().y) };
        mp_renderer->UpdateUniform(reinterpret_cast<char*>(&wsize), sizeof(Libdas::Point2D<float>), m_ubo_offset);
    }
}
