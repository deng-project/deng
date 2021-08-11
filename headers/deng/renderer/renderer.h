/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: renderer.h - abstracted renderer class header
/// author: Karl-Mihkel Ott


#ifndef __RENDERER_H
#define __RENDERER_H

#ifdef __RENDERER_CPP
    #include <vector>
    #include <thread>
    #include <chrono>
    #include <mutex>
    #include <queue>
    #include <string>
    #include <memory>
    #include <array>
    
    #include <vulkan/vulkan.h>
    #include <common/base_types.h>
    #include <common/hashmap.h>
    #include <common/uuid.h>
    #include <common/err_def.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
    #include <deng/window.h>
    #include <deng/cam3d.h>

    #include <deng/registry/registry.h>
#endif

#include <imgui-layer/imgui_entity.h>
#include <deng/cross_api/cross_api.h>
#include <deng/vulkan/renderer.h>
#include <deng/renderer/asset_man.h>
#include <deng/renderer/data_updater.h>

#define DENG_DEFAULT_NEAR_PLANE 0.1f
#define DENG_DEFAULT_FAR_PLANE 25.0f
#define DENG_FRAME_INTERVAL 50 // microseconds

namespace deng {

    // Frame renderer type
    class Renderer;
    typedef void(*PCustomRunFunc)(Renderer&);


    /// Main renderer class
    class Renderer : public __DataUpdater {   
    private:
        std::unique_ptr<vulkan::__vk_ConfigVars> m_vk_vars;
        std::unique_ptr<vulkan::__vk_Renderer> m_vk_rend;   
        dengMath::vec4<deng_vec_t> m_env_color;
        deng_RendererHintBits m_hints;
        deng_bool_t m_is_init;

        Window *m_p_win;
        Camera3D *m_p_cam;

    public:
        Renderer(deng_RendererHintBits hints, const dengMath::vec4<deng_vec_t> &env_color);

        /// Setup graphics api specific renderer from the hints given in the constructor
        void setup(Camera3D &main_cam, Window &main_win);


        /// Update data, before creating new frame
        void update();


        /// Begin the rendering loop
        void run();        


        /// idle the renderer 
        void idle();

    // Render backend getter and setter methods
    public:
        void setUIDataPtr(__ImGuiData *p_data);
        const std::vector<deng_Id> &getAssets();
        const std::vector<deng_Id> &getTextures();
        deng::__GlobalRegistry &getRegistry();
    };
}


#endif
