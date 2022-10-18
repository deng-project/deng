// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: ImGuiLayer.h - ImGui gui abstraction class implementation
// author: Karl-Mihkel Ott

#define IMGUI_LAYER_CPP
#include "deng/ImGuiLayer.h"

namespace DENG {

    ImGuiLayer::ImGuiLayer(const std::string &_framebuffer_name) : 
        m_gui_texture_name(IMGUI_TEXTURE_NAME),
        m_framebuffer_name(_framebuffer_name)
    { m_beg = std::chrono::system_clock::now(); }


    ImGuiLayer::~ImGuiLayer() {
        // delete reserved memory locations
        GPUMemoryManager *mem_man = GPUMemoryManager::GetInstance();
        mem_man->DeleteUniformMemoryLocation(m_ubo_offset);
        if(m_main_offset != UINT32_MAX)
            mem_man->DeleteMainMemoryLocation(m_main_offset);

        ImGui::SetCurrentContext(m_context);
        ImGui::DestroyContext();
    }


    uint32_t ImGuiLayer::_CalculateUsedMemory(ImDrawData *_draw_data) {
        uint32_t total = 0;
        for(int i = 0; i < _draw_data->CmdListsCount; i++) {
            const ImDrawList *cmd_list = _draw_data->CmdLists[i];
            total += static_cast<uint32_t>(cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
            total += static_cast<uint32_t>(cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        }

        return total;
    }


    ImGuiKey ImGuiLayer::_TranslateKey(neko_HidEvent _key) {
        switch(_key) {
            case NEKO_KEY_TAB:
                return ImGuiKey_Tab;

            case NEKO_KEY_LEFT:
                return ImGuiKey_LeftArrow;

            case NEKO_KEY_RIGHT:
                return ImGuiKey_RightArrow;

            case NEKO_KEY_UP:
                return ImGuiKey_UpArrow;

            case NEKO_KEY_PAGE_UP:
                return ImGuiKey_PageUp;

            case NEKO_KEY_PAGE_DOWN:
                return ImGuiKey_PageDown;

            case NEKO_KEY_HOME:
                return ImGuiKey_Home;

            case NEKO_KEY_END:
                return ImGuiKey_End;

            case NEKO_KEY_INSERT:
                return ImGuiKey_Insert;

            case NEKO_KEY_DELETE:
                return ImGuiKey_Delete;

            case NEKO_KEY_BACKSPACE:
                return ImGuiKey_Backspace;

            case NEKO_KEY_SPACE:
                return ImGuiKey_Space;

            case NEKO_KEY_ENTER:
                return ImGuiKey_Enter;

            case NEKO_KEY_ESCAPE:
                return ImGuiKey_Escape;

            case NEKO_KEY_LEFT_CTRL:
                return ImGuiKey_LeftCtrl;

            case NEKO_KEY_LEFT_SHIFT:
                return ImGuiKey_LeftShift;

            case NEKO_KEY_LEFT_ALT:
                return ImGuiKey_LeftAlt;

            case NEKO_KEY_LEFT_SUPER:
                return ImGuiKey_LeftSuper;

            case NEKO_KEY_RIGHT_CTRL:
                return ImGuiKey_RightCtrl;

            case NEKO_KEY_RIGHT_SHIFT:
                return ImGuiKey_RightShift;

            case NEKO_KEY_RIGHT_ALT:
                return ImGuiKey_RightAlt;

            case NEKO_KEY_RIGHT_SUPER:
                return ImGuiKey_RightSuper;

            case NEKO_KEY_MENU:
                return ImGuiKey_Menu;

            case NEKO_KEY_0:
                return ImGuiKey_0;

            case NEKO_KEY_1:
                return ImGuiKey_1;

            case NEKO_KEY_2:
                return ImGuiKey_2;

            case NEKO_KEY_3:
                return ImGuiKey_3;

            case NEKO_KEY_4:
                return ImGuiKey_4;

            case NEKO_KEY_5:
                return ImGuiKey_5;

            case NEKO_KEY_6:
                return ImGuiKey_6;

            case NEKO_KEY_7:
                return ImGuiKey_7;

            case NEKO_KEY_8:
                return ImGuiKey_8;

            case NEKO_KEY_9:
                return ImGuiKey_9;

            // alphabetical input characters
            case NEKO_KEY_A:
            	return ImGuiKey_A;

            case NEKO_KEY_B:
            	return ImGuiKey_B;

            case NEKO_KEY_C:
            	return ImGuiKey_C;

            case NEKO_KEY_D:
            	return ImGuiKey_D;

            case NEKO_KEY_E:
            	return ImGuiKey_E;

            case NEKO_KEY_F:
            	return ImGuiKey_F;

            case NEKO_KEY_G:
            	return ImGuiKey_G;

            case NEKO_KEY_H:
            	return ImGuiKey_H;

            case NEKO_KEY_I:
            	return ImGuiKey_I;

            case NEKO_KEY_J:
            	return ImGuiKey_J;

            case NEKO_KEY_K:
            	return ImGuiKey_K;

            case NEKO_KEY_L:
            	return ImGuiKey_L;

            case NEKO_KEY_M:
            	return ImGuiKey_M;

            case NEKO_KEY_N:
            	return ImGuiKey_N;

            case NEKO_KEY_O:
            	return ImGuiKey_O;

            case NEKO_KEY_P:
            	return ImGuiKey_P;

            case NEKO_KEY_Q:
            	return ImGuiKey_Q;

            case NEKO_KEY_R:
            	return ImGuiKey_R;

            case NEKO_KEY_S:
            	return ImGuiKey_S;

            case NEKO_KEY_T:
            	return ImGuiKey_T;

            case NEKO_KEY_U:
            	return ImGuiKey_U;

            case NEKO_KEY_V:
            	return ImGuiKey_V;

            case NEKO_KEY_W:
            	return ImGuiKey_W;

            case NEKO_KEY_X:
            	return ImGuiKey_X;

            case NEKO_KEY_Y:
            	return ImGuiKey_Y;

            case NEKO_KEY_Z:
            	return ImGuiKey_Z;

            case NEKO_KEY_F1:
            	return ImGuiKey_F1;

            case NEKO_KEY_F2:
            	return ImGuiKey_F2;

            case NEKO_KEY_F3:
            	return ImGuiKey_F3;

            case NEKO_KEY_F4:
            	return ImGuiKey_F4;

            case NEKO_KEY_F5:
            	return ImGuiKey_F5;

            case NEKO_KEY_F6:
            	return ImGuiKey_F6;

            case NEKO_KEY_F7:
            	return ImGuiKey_F7;

            case NEKO_KEY_F8:
            	return ImGuiKey_F8;

            case NEKO_KEY_F9:
            	return ImGuiKey_F9;

            case NEKO_KEY_F10:
            	return ImGuiKey_F10;

            case NEKO_KEY_F11:
            	return ImGuiKey_F11;

            case NEKO_KEY_F12:
            	return ImGuiKey_F12;

            case NEKO_KEY_APOSTROPHE:
                return ImGuiKey_Apostrophe;

            case NEKO_KEY_COMMA:
                return ImGuiKey_Comma;

            case NEKO_KEY_MINUS:
                return ImGuiKey_Minus;

            case NEKO_KEY_DOT:
                return ImGuiKey_Period;

            case NEKO_KEY_SLASH:
                return ImGuiKey_Slash;

            case NEKO_KEY_SEMICOLON:
                return ImGuiKey_Semicolon;

            case NEKO_KEY_EQUAL:
                return ImGuiKey_Equal;

            // NOTE: missing ImGuiKey_LeftBracket, ImGuiKey_Backslash, ImGuiKey_RightBracket, ImGuiKey_GraveAccent ! ! !
            case NEKO_KEY_CAPS_LOCK:
                return ImGuiKey_CapsLock;

            case NEKO_KEY_SCROLL_LOCK:
                return ImGuiKey_ScrollLock;

            case NEKO_KEY_NUM_LOCK:
                return ImGuiKey_NumLock;

            case NEKO_KEY_PRINT_SCREEN:
                return ImGuiKey_PrintScreen;

            case NEKO_KEY_PAUSE:
                return ImGuiKey_Pause;

            case NEKO_KEY_NP_0:
                return ImGuiKey_Keypad0;

            case NEKO_KEY_NP_1:
                return ImGuiKey_Keypad1;

            case NEKO_KEY_NP_2:
                return ImGuiKey_Keypad2;

            case NEKO_KEY_NP_3:
                return ImGuiKey_Keypad3;

            case NEKO_KEY_NP_4:
                return ImGuiKey_Keypad4;

            case NEKO_KEY_NP_5:
                return ImGuiKey_Keypad5;

            case NEKO_KEY_NP_6:
                return ImGuiKey_Keypad6;

            case NEKO_KEY_NP_7:
                return ImGuiKey_Keypad7;

            case NEKO_KEY_NP_8:
                return ImGuiKey_Keypad8;

            case NEKO_KEY_NP_9:
                return ImGuiKey_Keypad9;

            case NEKO_KEY_NP_DECIMAL:
                return ImGuiKey_KeypadDecimal;

            case NEKO_KEY_NP_DIVIDE:
                return ImGuiKey_KeypadDivide;

            case NEKO_KEY_NP_MULTIPLY:
                return ImGuiKey_KeypadMultiply;

            case NEKO_KEY_NP_SUBTRACT:
                return ImGuiKey_KeypadSubtract;

            case NEKO_KEY_NP_ADD:
                return ImGuiKey_KeypadAdd;

            case NEKO_KEY_NP_ENTER:
                return ImGuiKey_KeypadEnter;

            case NEKO_KEY_NP_EQUAL:
                return ImGuiKey_KeypadEqual;

            default:
                return ImGuiKey_None;
        }
    }

    
    void ImGuiLayer::_CreateDrawCommands(ImDrawData *_draw_data) {
        DENG::MeshReference &mesh = mp_renderer->GetMesh(m_mesh_id, m_framebuffer_name);
        mesh.name = "__imgui__";
        mesh.shader_module_id = m_shader_id;
        mesh.commands.clear();

        GPUMemoryManager *mem_manager = GPUMemoryManager::GetInstance();
        // check if main memory was already requested and if so, delete it
        if(m_main_offset != UINT32_MAX)
            mem_manager->DeleteMainMemoryLocation(m_main_offset);

        const uint32_t total_size = _CalculateUsedMemory(_draw_data);
        m_main_offset = mem_manager->RequestMainMemoryLocationF(static_cast<uint32_t>(sizeof(float)), total_size);

        uint32_t cmd_vert_offset = m_main_offset;
        uint32_t cmd_idx_offset = m_main_offset;

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

                    float x = static_cast<float>(mp_window->GetSize().x);
                    float y = static_cast<float>(mp_window->GetSize().y);

                    if(clip.z > mp_window->GetSize().x) clip.z = x;
                    if(clip.w > mp_window->GetSize().y) clip.w = y;

                    if(clip.z <= clip.x || clip.w <= clip.y)
                        continue;

                    // update the mesh instance for new draw calls
                    mesh.commands.emplace_back();
                    const uint32_t base_vertex_offset = cmd_vert_offset + pcmd->VtxOffset * sizeof(ImDrawVert);
                    mesh.commands.back().attribute_offsets.push_back(base_vertex_offset + offsetof(ImDrawVert, pos));
                    mesh.commands.back().attribute_offsets.push_back(base_vertex_offset + offsetof(ImDrawVert, uv));
                    mesh.commands.back().attribute_offsets.push_back(base_vertex_offset + offsetof(ImDrawVert, col));
                    mesh.commands.back().indices_offset = cmd_idx_offset + pcmd->IdxOffset * sizeof(ImDrawIdx);
                    mesh.commands.back().draw_count = pcmd->ElemCount;
                    mesh.commands.back().texture_names.push_back(std::string(reinterpret_cast<char*>(pcmd->TextureId)));
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
        m_io->DeltaTime = m_delta_time;
        m_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        m_io->DisplaySize.x = static_cast<float>(mp_window->GetSize().x);
        m_io->DisplaySize.y = static_cast<float>(mp_window->GetSize().y);
        m_io->AddMouseButtonEvent(0, mp_window->IsKeyPressed(NEKO_MOUSE_BTN_1));
        m_io->AddMouseButtonEvent(1, mp_window->IsKeyPressed(NEKO_MOUSE_BTN_2));
        m_io->AddMousePosEvent(static_cast<float>(mp_window->GetMousePosition().x), static_cast<float>(mp_window->GetMousePosition().y));

        uint32_t index = 0;
        uint32_t size = 0;
        const EventQueue *active = mp_window->GetActiveEventQueue();
        const EventQueue *released = mp_window->GetReleasedEventQueue();
        
        size = active->size;
        while(size) {
            if(active->events[index] <= NEKO_KEY_MAX) {
                const ImGuiKey key = _TranslateKey(active->events[index]);
                m_io->AddKeyEvent(key, true);
            }
            size--;
            index++;
        }

        size = released->size;
        while(size) {
            if(released->events[index] <= NEKO_KEY_MAX) {
                const ImGuiKey key = _TranslateKey(released->events[index]);
                m_io->AddKeyEvent(key, false);
            }
            size--;
            index++;
        }

        if(mp_window->IsKeyPressed(NEKO_MOUSE_SCROLL_DOWN))
            m_io->MouseWheel = -1;
        else if(mp_window->IsKeyPressed(NEKO_MOUSE_SCROLL_UP))
            m_io->MouseWheel = 1;
        else m_io->MouseWheel = 0;
    }


    void ImGuiLayer::Attach(Window &_win, Renderer &_rend, PFN_ImGuiDrawCallback _callback, void *_user_data) {
        GPUMemoryManager *mem_manager = GPUMemoryManager::GetInstance();
        m_ubo_offset = mem_manager->RequestUniformMemoryLocationP(_rend, static_cast<uint32_t>(sizeof(TRS::Point2D<float>)));
        mp_window = &_win;
        mp_renderer = &_rend;
        m_callback = _callback;
        m_user_data = _user_data;

        m_context = ImGui::CreateContext();
        ImGui::SetCurrentContext(m_context);
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
        module.enable_2d_textures = true;
        module.load_shaders_from_file = false;

        module.ubo_data_layouts.reserve(2);
        module.ubo_data_layouts.emplace_back();
        module.ubo_data_layouts.back().block.binding = 0;
        module.ubo_data_layouts.back().stage = SHADER_STAGE_VERTEX;
        module.ubo_data_layouts.back().type = DENG::UNIFORM_DATA_TYPE_BUFFER;
        module.ubo_data_layouts.back().block.size = static_cast<uint32_t>(sizeof(TRS::Point2D<float>));
        module.ubo_data_layouts.back().block.offset = m_ubo_offset;

        module.ubo_data_layouts.emplace_back();
        module.ubo_data_layouts.back().block.binding = 1;
        module.ubo_data_layouts.back().stage = SHADER_STAGE_FRAGMENT;
        module.ubo_data_layouts.back().type = DENG::UNIFORM_DATA_TYPE_IMAGE_SAMPLER;

        m_shader_id = mp_renderer->PushShader(module, m_framebuffer_name);

        m_mesh_id = mp_renderer->PushMeshReference(DENG::MeshReference(), m_framebuffer_name);
        mp_renderer->PushTextureFromMemory(IMGUI_TEXTURE_NAME, reinterpret_cast<const char*>(pix), static_cast<uint32_t>(width), static_cast<uint32_t>(height), 4);
        m_io->Fonts->SetTexID(const_cast<char*>(m_gui_texture_name.c_str()));
    }


    void ImGuiLayer::Update() {
        m_end = std::chrono::system_clock::now();
        std::chrono::duration<float, std::milli> dur = m_end - m_beg;
        m_delta_time = dur.count() / 1000.f;
        if(m_delta_time < 0.0f)
            m_delta_time = 0.0f;
        m_beg = std::chrono::system_clock::now();

        ImGui::SetCurrentContext(m_context);
        _UpdateIO();
        ImGui::NewFrame();
            m_callback(m_user_data);
        ImGui::EndFrame();

        ImGui::Render();

        ImDrawData *draw_data = ImGui::GetDrawData();
        _CreateDrawCommands(draw_data);

        TRS::Point2D<float> wsize = { static_cast<float>(mp_window->GetSize().x), static_cast<float>(mp_window->GetSize().y) };
        mp_renderer->UpdateUniform(reinterpret_cast<char*>(&wsize), sizeof(TRS::Point2D<float>), m_ubo_offset);
    }
}
