/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: data_updater.h - update request handling class header
/// author: Karl-Mihkel Ott


#ifndef __DATA_UPDATER_H
#define __DATA_UPDATER_H


#ifdef __DATA_UPDATER_CPP
    #include <array>
    #include <vector>
    #include <memory>
    #include <chrono>
    #include <queue>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/hashmap.h>
    #include <common/err_def.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
    #include <deng/window.h>
    #include <deng/cam3d.h>
    #include <deng/registry/registry.h>
    #include <imgui-layer/imgui_entity.h>

    // Api specific renderer instances
    #include <deng/cross_api/cross_api.h>
    #include <deng/vulkan/renderer.h>
    #include <deng/renderer/asset_man.h>
#endif

namespace deng {

    class __DataUpdater : public __AssetManager {
    private:
        std::unique_ptr<vulkan::__vk_Renderer> &m_vk_rend;
        std::unique_ptr<vulkan::__vk_ConfigVars> &m_vk_cfg;

    protected:
        deng_bool_t m_is_init = false;

    private:
        /// Check if the renderer is initialised for update methods and throw error if needed
        void __initCheck(const std::string &func_name);

    public:
        __DataUpdater(__GlobalRegistry &reg, std::unique_ptr<vulkan::__vk_Renderer> &vk_rend, std::unique_ptr<vulkan::__vk_ConfigVars> &vk_cfg);

        /// Overwrite asset vertices to main buffer.
        /// Note that this method expects that vertices count hasn't changed,
        /// otherwise memory corruption can happen!
        void overwriteAssetData(const dengMath::vec2<deng_ui32_t> &bounds);


        /// Replace current light sources with new ones
        void updateLighting(std::array<deng_Id, __DENG_MAX_LIGHT_SRC_COUNT> &light_srcs);


        /// Check if the main buffer needs to be reallocated for incoming data
        void checkBufferRealloc();


        /// Update ui vertices data in the buffer
        void uiUpdateData();
    };
}

#endif
