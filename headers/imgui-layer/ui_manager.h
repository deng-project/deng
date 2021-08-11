/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: ui_manager.h - abstracted UI management class header
/// author: Karl-Mihkel Ott

#ifndef __IMGUI_RENDERER_H
#define __IMGUI_RENDERER_H


#ifdef __IMGUI_RENDERER_CPP
    #include <vector>
    #include <array>
    #include <string>
    #include <memory>
    #include <thread>
    #include <queue>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <common/hashmap.h>
    #include <common/uuid.h>
    #include <common/err_def.h>
    #include <data/assets.h>
    #include <data/tex_loader.h>

    #include <math/deng_math.h>
    #include <imgui-layer/imgui_entity.h>

    #include <deng/registry/registry.h>
    #include <deng/window.h>
    #include <deng/cam3d.h>
    #include <deng/renderer/renderer.h>


    #define DENG_IMGUI_ENTITY_RESERV                16
    #define DENG_IMGUI_INITIAL_KEYPRESS_DELAY       0.25f // seconds
    #define DENG_IMGUI_REPEATED_KEYPRESS_DELAY      0.07f // seconds
#endif

namespace deng {


    class UIManager {
    private:
        deng::Renderer &m_rend;
        deng_Id m_imgui_atlas = NULL;
        ImGuiIO *m_p_io;
        deng::__ImGuiData m_gui_data = {};
        char m_prev_in = 0;
        deng_bool_t m_is_first_key_press = false;

        std::chrono::duration<deng_vec_t, std::milli> m_frame_dur = std::chrono::milliseconds(0);
        std::chrono::time_point<std::chrono::high_resolution_clock> m_key_pt = std::chrono::high_resolution_clock::now();

    private:
        /// Map IO keys according to DENG keymap
        void __mapIOKeys();


        /// Update IO key status
        void __updateIOKeys();

    public:
        UIManager(Renderer &rend);

        /// Create a new ui form in ImGui
        //void spawnForm();

        /// Update IO device input
        void updateIO(const deng::Window &win);

        /// Render ImGui data to screen
        void render(ImDrawData *p_draw_data, const Window &win);

    public:
        ImGuiIO *getIO();

        /// Set new time points to measure framerate
        void setTime(std::chrono::time_point<std::chrono::high_resolution_clock> t1, 
            std::chrono::time_point<std::chrono::high_resolution_clock> t2);
    };
}


#endif
