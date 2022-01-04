// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: ImGuiEntity.h - ImGui entity data structures
// author: Karl-Mihkel Ott

#ifndef __IMGUI_ENTITY_H
#define __IMGUI_ENTITY_H

#if defined(NO_IMGUI)
    typedef struct ImDrawVert {
        dengMath::vec2<deng_vec_t> pos;
        dengMath::vec2<deng_vec_t> uv;
        deng_ui32_t col;
    } ImDrawVert;


    typedef deng_ui32_t ImDrawIdx;
#else 
    #include <imgui.h>
#endif

namespace deng {
    // Check if ImGui vertex and index data should be defined


    /// All possible floating UI forms
    enum UIForm {
        DENG_UI_FORM_SYS_INFO       = 0,
        DENG_UI_FORM_TEXTURE_VIEW   = 1,
        DENG_UI_FORM_ASSET_VIEW     = 2
    };


    /// Structure for storing data about ImGui draw operations
    struct __ImGuiCmdData {
        size_t voffset;                  // Vertex offset from the beginning of the ui memory area
        size_t ioffset;
        const ImDrawVert *verts = NULL;
        size_t vert_c = 0;
        const ImDrawIdx *ind = NULL;
        size_t ind_c = 0;
    };


    /// Structure used to pass ImGui entities
    struct __ImGuiEntity {
        size_t buf_offset;                          // Indices offset from the beginning of the ui memory area
        const ImDrawIdx *ind;
        size_t ind_c;                               // Total count of indices
        size_t cmd_list_ind;                        // Index of the used command list
        dengMath::vec2<deng_i32_t> sc_rec_offset;   // Scissor rectangle offset
        dengMath::vec2<deng_ui32_t> sc_rec_size;    // Scissor rectangle size
    };

    
    /// Data structure used for passing UI data between UI handler classes and the renderer 
    struct __ImGuiData {
        deng_Id tex_id;
        std::vector<__ImGuiCmdData> cmd_data;   // Data required for creating entity commandbuffers
        std::vector<__ImGuiEntity> entities;    // All entities related to ImGui
    };
}
#endif
