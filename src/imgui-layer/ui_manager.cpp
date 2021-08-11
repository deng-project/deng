/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: ui_manager.cpp - abstracted UI management class implementation
/// author: Karl-Mihkel Ott

#define __IMGUI_RENDERER_CPP
#include <imgui-layer/ui_manager.h>

namespace deng {

    UIManager::UIManager(deng::Renderer &rend) : m_rend(rend) {
        // Create new ImGui context
        ImGui::CreateContext();
        m_p_io = &ImGui::GetIO();
        
        // Create a texture atlas for ImGui
        das_Texture imgui_atlas = {};
        imgui_atlas.uuid = uuid_Generate();
        m_imgui_atlas = imgui_atlas.uuid;
        m_gui_data.tex_id = imgui_atlas.uuid;
        imgui_atlas.name = (char*) "#ImGui_atlas";
    
        // Temporary width and height variables
        deng_i32_t width, height;
        m_p_io->Fonts->GetTexDataAsRGBA32(&imgui_atlas.pixel_data.pixel_data, &width, &height);

        // Cast the width and height parameters into texture width and height
        imgui_atlas.pixel_data.width = static_cast<deng_ui16_t>(width);
        imgui_atlas.pixel_data.height = static_cast<deng_ui16_t>(height);

        imgui_atlas.pixel_data.size = width * height * 4;
        imgui_atlas.no_reg_cleanup = true;

        das_RealignPixelData(&imgui_atlas, DAS_PIXEL_FORMAT_R8G8B8A8);
        
        // Set the texture ID
        m_p_io->Fonts->SetTexID((void*) imgui_atlas.uuid);

        m_rend.pushTexture(imgui_atlas);
        m_rend.setUIDataPtr(&m_gui_data);
        __mapIOKeys();
    }


    /// Map IO keys according to DENG keymap
    void UIManager::__mapIOKeys() {
        m_p_io->KeyMap[ImGuiKey_Tab] = NEKO_KEY_TAB;
        m_p_io->KeyMap[ImGuiKey_LeftArrow] = NEKO_KEY_LEFT;
        m_p_io->KeyMap[ImGuiKey_RightArrow] = NEKO_KEY_RIGHT;
        m_p_io->KeyMap[ImGuiKey_UpArrow] = NEKO_KEY_UP;
        m_p_io->KeyMap[ImGuiKey_DownArrow] = NEKO_KEY_DOWN;
        m_p_io->KeyMap[ImGuiKey_PageDown] = NEKO_KEY_PAGE_DOWN;
        m_p_io->KeyMap[ImGuiKey_PageUp] = NEKO_KEY_PAGE_UP;
        m_p_io->KeyMap[ImGuiKey_Home] = NEKO_KEY_HOME;
        m_p_io->KeyMap[ImGuiKey_End] = NEKO_KEY_END;
        m_p_io->KeyMap[ImGuiKey_Delete] = NEKO_KEY_DELETE;
        m_p_io->KeyMap[ImGuiKey_Backspace] = NEKO_KEY_BACKSPACE;
        m_p_io->KeyMap[ImGuiKey_Space] = NEKO_KEY_SPACE;
        m_p_io->KeyMap[ImGuiKey_Enter] = NEKO_KEY_ENTER;
        m_p_io->KeyMap[ImGuiKey_Escape] = NEKO_KEY_ESCAPE;
        m_p_io->KeyMap[ImGuiKey_KeyPadEnter] = NEKO_KEY_NP_ENTER;
        m_p_io->KeyMap[ImGuiKey_A] = NEKO_KEY_A;
        m_p_io->KeyMap[ImGuiKey_C] = NEKO_KEY_C;
        m_p_io->KeyMap[ImGuiKey_V] = NEKO_KEY_V;
        m_p_io->KeyMap[ImGuiKey_X] = NEKO_KEY_X;
        m_p_io->KeyMap[ImGuiKey_Y] = NEKO_KEY_Y;
        m_p_io->KeyMap[ImGuiKey_Z] = NEKO_KEY_Z;
    }


    /// Update IO key status
    void UIManager::__updateIOKeys() {
        m_p_io->KeysDown[NEKO_KEY_TAB]          = neko_FindKeyStatus(NEKO_KEY_TAB, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_LEFT]         = neko_FindKeyStatus(NEKO_KEY_LEFT, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_RIGHT]        = neko_FindKeyStatus(NEKO_KEY_RIGHT, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_UP]           = neko_FindKeyStatus(NEKO_KEY_UP, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_DOWN]         = neko_FindKeyStatus(NEKO_KEY_DOWN, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_PAGE_DOWN]    = neko_FindKeyStatus(NEKO_KEY_PAGE_DOWN, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_PAGE_UP]      = neko_FindKeyStatus(NEKO_KEY_PAGE_UP, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_HOME]         = neko_FindKeyStatus(NEKO_KEY_HOME, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_END]          = neko_FindKeyStatus(NEKO_KEY_END, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_DELETE]       = neko_FindKeyStatus(NEKO_KEY_DELETE, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_BACKSPACE]    = neko_FindKeyStatus(NEKO_KEY_BACKSPACE, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_SPACE]        = neko_FindKeyStatus(NEKO_KEY_SPACE, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_ENTER]        = neko_FindKeyStatus(NEKO_KEY_ENTER, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_ESCAPE]       = neko_FindKeyStatus(NEKO_KEY_ESCAPE, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_NP_ENTER]     = neko_FindKeyStatus(NEKO_KEY_NP_ENTER, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_A]            = neko_FindKeyStatus(NEKO_KEY_A, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_C]            = neko_FindKeyStatus(NEKO_KEY_C, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_V]            = neko_FindKeyStatus(NEKO_KEY_V, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_X]            = neko_FindKeyStatus(NEKO_KEY_X, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_Y]            = neko_FindKeyStatus(NEKO_KEY_Y, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->KeysDown[NEKO_KEY_Z]            = neko_FindKeyStatus(NEKO_KEY_Z, NEKO_INPUT_EVENT_TYPE_ACTIVE);
    }


    // Update IO device input
    void UIManager::updateIO(const deng::Window &win) {
        m_p_io->DeltaTime = m_frame_dur.count() / 1000;
        m_p_io->DisplaySize.x = static_cast<deng_vec_t>(win.getSize().first);
        m_p_io->DisplaySize.y = static_cast<deng_vec_t>(win.getSize().second);
        m_p_io->MousePos.x = static_cast<deng_vec_t>(win.getMPos().first);
        m_p_io->MousePos.y = static_cast<deng_vec_t>(win.getMPos().second);
        m_p_io->WantCaptureMouse = true;

        // Check if mouse wheel event should be recorded
        if(neko_FindKeyStatus(NEKO_MOUSE_SCROLL_UP, NEKO_INPUT_EVENT_TYPE_ACTIVE))
            m_p_io->MouseWheel = 0.5f;
        else if(neko_FindKeyStatus(NEKO_MOUSE_SCROLL_DOWN, NEKO_INPUT_EVENT_TYPE_ACTIVE))
            m_p_io->MouseWheel = -0.5f;
        
        
        // Check if any input characters should be added
        char in;
        auto tp = std::chrono::high_resolution_clock::now();
        std::chrono::duration<deng_vec_t> delta_time = tp - m_key_pt;
        if((in = neko_GetActiveInput()) && (m_prev_in != in || (m_is_first_key_press ? delta_time.count() > DENG_IMGUI_INITIAL_KEYPRESS_DELAY :
           delta_time.count() > DENG_IMGUI_REPEATED_KEYPRESS_DELAY))) {
            m_p_io->AddInputCharacter(static_cast<deng_ui32_t>(in));
            m_key_pt = std::chrono::high_resolution_clock::now();

            // Check if the initial keystroke was made
            if(m_prev_in != in) m_is_first_key_press = true;
            else m_is_first_key_press = false;

            m_prev_in = in;
        }

        // Update all keyboard key events 
        __updateIOKeys();

        // Check modifier key presses
        m_p_io->KeyCtrl =  neko_FindKeyStatus(NEKO_KEY_LEFT_CTRL, NEKO_INPUT_EVENT_TYPE_ACTIVE) || 
            neko_FindKeyStatus(NEKO_KEY_RIGHT_CTRL, NEKO_INPUT_EVENT_TYPE_ACTIVE);

        m_p_io->KeyShift =  neko_FindKeyStatus(NEKO_KEY_LEFT_SHIFT, NEKO_INPUT_EVENT_TYPE_ACTIVE) || 
            neko_FindKeyStatus(NEKO_KEY_RIGHT_SHIFT, NEKO_INPUT_EVENT_TYPE_ACTIVE);

        m_p_io->KeyAlt =  neko_FindKeyStatus(NEKO_KEY_LEFT_ALT, NEKO_INPUT_EVENT_TYPE_ACTIVE) || 
            neko_FindKeyStatus(NEKO_KEY_RIGHT_ALT, NEKO_INPUT_EVENT_TYPE_ACTIVE);

        m_p_io->KeySuper =  neko_FindKeyStatus(NEKO_KEY_LEFT_SUPER, NEKO_INPUT_EVENT_TYPE_ACTIVE) || 
            neko_FindKeyStatus(NEKO_KEY_RIGHT_SUPER, NEKO_INPUT_EVENT_TYPE_ACTIVE);

        m_p_io->MouseDown[0] = m_p_io->MouseClicked[0] = neko_FindKeyStatus(NEKO_MOUSE_BTN_1, NEKO_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->MouseDown[1] = m_p_io->MouseClicked[1] = neko_FindKeyStatus(NEKO_MOUSE_BTN_2, NEKO_INPUT_EVENT_TYPE_ACTIVE);
    }


    /// Update ImGui draw content according to the changed data
    void UIManager::render(ImDrawData *p_draw_data, const Window &win) {
        size_t list_offset = 0;
        m_gui_data.cmd_data.resize(p_draw_data->CmdListsCount);
        m_gui_data.entities.clear();
        std::this_thread::sleep_for(std::chrono::microseconds(1000));

        // For each command list in the list output its vertices to the renderer
        for(deng_ui32_t i = 0; i < p_draw_data->CmdListsCount; i++) {
            const ImDrawList *draw_list = p_draw_data->CmdLists[i];
            ImDrawVert *verts = draw_list->VtxBuffer.Data;
            ImDrawIdx *inds = draw_list->IdxBuffer.Data;

            // For each vertex, convert its pixel size to vector size
            for(deng_ui32_t j = 0; j < draw_list->VtxBuffer.Size; j++) {
                verts[j].pos.x = dengMath::Conversion::mouseCoordToVecCoord(verts[j].pos.x, static_cast<deng_px_t>(win.getSize().first));
                verts[j].pos.y = dengMath::Conversion::mouseCoordToVecCoord(verts[j].pos.y, static_cast<deng_px_t>(win.getSize().second));
            }

            m_gui_data.cmd_data[i].verts = verts;
            m_gui_data.cmd_data[i].vert_c = draw_list->VtxBuffer.Size;

            m_gui_data.cmd_data[i].ind = inds;
            m_gui_data.cmd_data[i].ind_c = draw_list->IdxBuffer.Size;
            
            // For each command buffer in the current command list set its vertices and indices
            for(deng_ui32_t j = 0; j < draw_list->CmdBuffer.Size; j++) {
                const ImDrawCmd *p_cmd = &draw_list->CmdBuffer[j];

                // Check if user callback should be respected
                if(p_cmd->UserCallback) 
                    p_cmd->UserCallback(draw_list, p_cmd);

                // Perform buffer data update 
                else {
                    __ImGuiEntity ent = {};
                    ent.buf_offset = list_offset + draw_list->VtxBuffer.Size * sizeof(ImDrawVert) + 
                        p_cmd->IdxOffset * sizeof(ImDrawIdx);
                    ent.ind = inds + p_cmd->IdxOffset;
                    ent.ind_c = p_cmd->ElemCount;

                    // Set up clipping rectangle parameters
                    ent.sc_rec_offset.first = static_cast<deng_i32_t>(p_cmd->ClipRect.x - p_draw_data->DisplayPos.x);
                    ent.sc_rec_offset.second = static_cast<deng_i32_t>(p_cmd->ClipRect.y - p_draw_data->DisplayPos.y);
                    ent.sc_rec_size.first = static_cast<deng_ui32_t>(p_cmd->ClipRect.z - p_cmd->ClipRect.x);
                    ent.sc_rec_size.second = static_cast<deng_ui32_t>(p_cmd->ClipRect.w - p_cmd->ClipRect.y);
                    ent.cmd_list_ind = i;

                    m_gui_data.entities.push_back(ent);
                }
            }

            // Increment the list offset accordingly
            list_offset += draw_list->VtxBuffer.Size * sizeof(ImDrawVert) + draw_list->IdxBuffer.Size * sizeof(ImDrawIdx);
        }
        
        // Update vertices data
        m_rend.uiUpdateData();
    }


    ImGuiIO *UIManager::getIO() { return m_p_io; }


    /// Set new time points to measure framerate
    void UIManager::setTime (
        std::chrono::time_point<std::chrono::high_resolution_clock> t1, 
        std::chrono::time_point<std::chrono::high_resolution_clock> t2
    ) { m_frame_dur = t2 - t1; }
}
