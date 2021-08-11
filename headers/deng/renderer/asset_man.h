/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: asset_man.h - asset manager parent class header for main renderer
/// author: Karl-Mihkel Ott


#ifndef __ASSET_MAN_H
#define __ASSET_MAN_H

#ifdef __ASSET_MAN_CPP
    #include <stdlib.h>
    #include <vector>
    #include <array>
    #include <mutex>
    #include <string>
    #include <memory>
    #include <queue>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <common/hashmap.h>
    #include <data/assets.h>
    #include <math/deng_math.h>

    #include <deng/window.h>
    #include <deng/cam3d.h>
    #include <deng/registry/registry.h>
    #include <imgui-layer/imgui_entity.h>
    
    #include <deng/cross_api/cross_api.h>
    #include <deng/vulkan/renderer.h>
#endif


namespace deng {

    class __AssetManager {
    private:
        std::unique_ptr<vulkan::__vk_ConfigVars> &m_vk_vars;
        std::unique_ptr<vulkan::__vk_Renderer> &m_vk_rend;
        std::queue<deng_Id> m_asset_queue;
        std::queue<deng_Id> m_texture_queue;

        // Bookkeeping variables for assets
        deng_ui32_t m_vu2d_c = 0;
        deng_ui32_t m_vm2d_c = 0;
        deng_ui32_t m_vu3d_c = 0;
        deng_ui32_t m_vm3d_c = 0;

    private:
        /// Increment asset type instance count
        void __assetTypeIncr(das_Asset &asset);

    protected:
        deng_WindowHint m_win_hints = 0;
        __GlobalRegistry m_reg;
        std::vector<deng_Id> m_assets;
        std::vector<deng_Id> m_textures;
        
    public:
        __AssetManager(std::unique_ptr<vulkan::__vk_Renderer> &vk_rend, std::unique_ptr<vulkan::__vk_ConfigVars> &vk_vars);

        /// Add texture id to submission queue
        /// PS! Texture UUIDs have to be generated before submitting them
        void submitTexture(das_Texture &texture);


        /// Add asset id to submission queue
        /// PS! Asset UUIDs have to be generated before submitting them
        void submitAsset(das_Asset &asset);


        /// Submit all assets to in submission queue to renderer
        void submitAssetQueue();


        /// Submit all textures in submission queue to renderer
        void submitTextureQueue();


        /// Push asset to renderer and initialise it, possibly reallocate vertices buffer if needed
        /// PS! Asset UUIDs have to be generated before push and renderer must be setup
        void pushAsset(das_Asset &asset);


        /// Push texture to renderer and initialise it, possibly reallocate texture memory if needed,
        /// PS! Texture UUIDs have to be generated before submitting them and renderer must be setup
        void pushTexture(das_Texture &texture);
    };
}

#endif
