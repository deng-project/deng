/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: data_updater.cpp - update request handling class implementation
/// author: Karl-Mihkel Ott

#define __DATA_UPDATER_CPP
#include <deng/renderer/data_updater.h>


namespace deng {

    __DataUpdater::__DataUpdater (
        Registry &reg, 
        vulkan::__vk_ConfigVars &vk_cfg,
        opengl::__gl_ConfigVars &gl_cfg
    ) : __AssetManager(vk_cfg, gl_cfg), m_vk_cfg(vk_cfg), m_gl_cfg(gl_cfg) {}


    /// Check if the renderer is initialised for update methods and throw error if needed
    void __DataUpdater::__initCheck(const std::string &func_name) {
        if(!m_is_init)
            RUN_ERR(func_name, "DENG renderer is not initialised");
    }


    /// Overwrite asset vertices to main buffer.
    /// Note that this method expects that vertices count hasn't changed,
    /// otherwise weird stuff can happen!
    void __DataUpdater::overwriteAssetData(const dengMath::vec2<deng_ui32_t> &bounds) {
        __initCheck("deng::__DataUpdater::updateAssetVerticesBuffer()");

        // Check the currently used API
        if(m_win_hints & DENG_WINDOW_HINT_API_VULKAN)
            m_vk_rend->updateAssetData(bounds);
        else if(m_win_hints & DENG_WINDOW_HINT_API_OPENGL)
            m_gl_rend->updateAssetData(bounds);
    }


    /// Replace current light sources with new ones
    void __DataUpdater::updateLighting(std::array<deng_Id, __DENG_MAX_LIGHT_SRC_COUNT> &light_srcs) {
        // Check the backend api type
        if(m_win_hints & DENG_WINDOW_HINT_API_VULKAN)
            m_vk_rend->setLighting(light_srcs);
        else if(m_win_hints & DENG_WINDOW_HINT_API_OPENGL)
            m_vk_rend->setLighting(light_srcs);
    }


    /// Check if buffer needs to be reallocated for data
    void __DataUpdater::checkBufferRealloc() {
        // Check the backend api type
        if(m_win_hints & DENG_WINDOW_HINT_API_VULKAN)
            m_vk_rend->checkForBufferReallocation();
        else if(m_win_hints & DENG_WINDOW_HINT_API_OPENGL)
            m_gl_rend->checkForBufferReallocation();
    }


    /// Update ui vertices data in the buffer
    void __DataUpdater::uiUpdateData() {
        // Check the backend api type
        if(m_win_hints & DENG_WINDOW_HINT_API_VULKAN)
            m_vk_rend->updateUIData(m_vk_cfg.background);
        else if(m_win_hints & DENG_WINDOW_HINT_API_OPENGL)
            m_gl_rend->updateUIData();
    }


    void __DataUpdater::setVkRenderer(std::shared_ptr<vulkan::__vk_Renderer> rend) {
        __AssetManager::m_vk_rend = rend;
    }


    void __DataUpdater::setGlRenderer(std::shared_ptr<opengl::__gl_Renderer> rend) {
        __AssetManager::m_gl_rend = rend;
    }
}
