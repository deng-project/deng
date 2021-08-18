/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: renderer.cpp - abstracted renderer class implementation
/// author: Karl-Mihkel Ott


#define __RENDERER_CPP
#include <deng/renderer/renderer.h>

namespace deng {

    Renderer::Renderer(deng_RendererHintBits hints, const dengMath::vec4<deng_vec_t> &env_color) :
        __DataUpdater(m_reg, m_vk_rend, m_vk_vars) {
        m_hints = hints;
        m_env_color = env_color;
    }

    
    /// Setup graphics api specific renderer instance from the hints given
    void Renderer::setup(Camera3D &main_cam, Window &main_win) {
        m_p_cam = &main_cam;
        m_p_win = &main_win;

        // Check the current window api
        m_win_hints = m_p_win->getHints();
        if(m_win_hints & DENG_WINDOW_HINT_API_VULKAN) {
            // Setup Vulkan renderer configuration values
            m_vk_vars = std::make_unique<vulkan::__vk_ConfigVars>();
            m_vk_vars->background = m_env_color,
            m_vk_vars->p_cam = &main_cam;
            m_vk_vars->p_win = &main_win;
            m_vk_vars->enable_validation_layers = m_hints & DENG_RENDERER_HINT_ENABLE_API_DEBUGGING;

            // Find the sample count
            if(m_hints & DENG_RENDERER_HINT_MSAA_64)
                m_vk_vars->msaa_sample_count = VK_SAMPLE_COUNT_64_BIT;
            else if(m_hints & DENG_RENDERER_HINT_MSAA_32)
                m_vk_vars->msaa_sample_count = VK_SAMPLE_COUNT_32_BIT;
            else if(m_hints & DENG_RENDERER_HINT_MSAA_16)
                m_vk_vars->msaa_sample_count = VK_SAMPLE_COUNT_16_BIT;
            else if(m_hints & DENG_RENDERER_HINT_MSAA_8)
                m_vk_vars->msaa_sample_count = VK_SAMPLE_COUNT_8_BIT;
            else if(m_hints & DENG_RENDERER_HINT_MSAA_4)
                m_vk_vars->msaa_sample_count = VK_SAMPLE_COUNT_4_BIT;
            else if(m_hints & DENG_RENDERER_HINT_MSAA_2)
                m_vk_vars->msaa_sample_count = VK_SAMPLE_COUNT_2_BIT;
            else m_vk_vars->msaa_sample_count = VK_SAMPLE_COUNT_1_BIT;


            // Create a new renderer instance
            m_vk_rend = std::make_unique<vulkan::__vk_Renderer>(*m_vk_vars.get(), Renderer::m_reg, m_assets, m_textures);

            // Tell Vulkan renderer to initialise Vulkan specific asset and texture instances 
            __AssetManager::submitTextureQueue();
            __AssetManager::submitAssetQueue();

            // Setup the renderer
            m_vk_rend->setup();
        }
        else RUN_ERR("deng::Renderer::setup()", "OpenGL is not supported by DENG default renderer :(");

        m_is_init = true;
    }


    /// Update data before creating new frame
    void Renderer::update() {
        m_p_win->update();
        m_p_cam->update();

        // Render new frame according to the renderer api used
        m_win_hints = m_p_win->getHints();
        if(m_win_hints & DENG_WINDOW_HINT_API_VULKAN)
            m_vk_rend->makeFrame();
    }


    /// Begin the rendering loop
    void Renderer::run() {
        while(m_p_win->isRunning()) {
            update();
        }
        
        // Idle the renderer for destruction
        if(m_win_hints & DENG_WINDOW_HINT_API_VULKAN)
            m_vk_rend->idle();
    }


    /// idle the renderer 
    void Renderer::idle() {
        if(m_p_win->getHints() & DENG_WINDOW_HINT_API_VULKAN)
            m_vk_rend->idle();
    }
    

    void Renderer::setUIDataPtr(__ImGuiData *p_data) {
        // Check the backend renderer and call the correct ui data setter method accordingly
        if(m_p_win->getHints() & DENG_WINDOW_HINT_API_VULKAN)
            m_vk_rend->setUIDataPtr(p_data);
    }


    const std::vector<deng_Id> &Renderer::getAssets() { return m_assets; }
    const std::vector<deng_Id> &Renderer::getTextures() { return m_textures; }
    Registry &Renderer::getRegistry() { return m_reg; }
} 
