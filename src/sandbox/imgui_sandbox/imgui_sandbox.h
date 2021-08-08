/// DENG: dynamic engine - powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: imgui_sandbox.h - imgui layer testing application class header
/// author: Karl-Mihkel Ott


#ifndef __IMGUI_SANDBOX_H
#define __IMGUI_SANDBOX_H


#ifdef __IMGUI_SANDBOX_CPP
    #include <vector>
    #include <array>
    #include <string>
    #include <thread>
    #include <iostream>
    #include <queue>
    #include <memory>

    #include <vulkan/vulkan.h>
    #include <common/hashmap.h>
    #include <common/err_def.h>
    #include <common/uuid.h>
    #include <common/base_types.h>
    #include <data/assets.h>
    #include <data/das_reader.h>
    #include <data/tex_loader.h>
    
    #include <math/deng_math.h>
    #include <deng/window.h>
    #include <utils/font.h>
    #include <deng/cam3d.h>
    #include <deng/registry/registry.h>
    #include <imgui-layer/imgui_entity.h>

    #include <deng/renderer/renderer.h>
    #include <imgui-layer/ui_manager.h>
#endif


namespace deng {

    class ImGUIApplication {
    private:
        Window m_win;
        Camera3D m_cam;
        Renderer m_rend;
        std::unique_ptr<UIManager> m_ui_man;

    public:
        ImGUIApplication();

        /// General run method
        void run();
    };
}


#endif
