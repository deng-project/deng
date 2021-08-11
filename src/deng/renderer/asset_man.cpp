/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: asset_man.cpp - asset manager parent class implementation for main renderer
/// author: Karl-Mihkel Ott

#define __ASSET_MAN_CPP
#include <deng/renderer/asset_man.h>

    
namespace deng {

    __AssetManager::__AssetManager (
        std::unique_ptr<vulkan::__vk_Renderer> &vk_rend, 
        std::unique_ptr<vulkan::__vk_ConfigVars> &vk_vars
    ) : m_vk_vars(vk_vars), m_vk_rend(vk_rend) {}


    /// Increment asset type instance count
    void __AssetManager::__assetTypeIncr(das_Asset &asset) {
        // Check the asset type and increment count accordingly
        switch(asset.asset_mode) {
        case DAS_ASSET_MODE_2D_UNMAPPED:
            m_vu2d_c++;
            break;

        case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
            m_vm2d_c++;
            break;

        case DAS_ASSET_MODE_3D_UNMAPPED:
            m_vu3d_c++;
            break;

        case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
            m_vm3d_c++;
            break;

        default: 
            break;
        }
    }


    /// Add texture id to submission queue
    /// PS! Texture UUIDs have to be generated before submitting them
    void __AssetManager::submitTexture(das_Texture &texture) {
        RegType reg_tex = { { 0 } };
        reg_tex.tex = texture;
        m_reg.push(reg_tex.tex.uuid, DENG_SUPPORTED_REG_TYPE_TEXTURE, reg_tex);
        m_texture_queue.push(reg_tex.tex.uuid);
    }


    /// Add asset id to submission queue
    /// PS! Asset UUIDs have to be generated before submitting them
    void __AssetManager::submitAsset(das_Asset &asset) {
        __assetTypeIncr(asset);

        RegType reg_asset = { { 0 } };
        reg_asset.asset = asset;
        m_reg.push(reg_asset.asset.uuid, DENG_SUPPORTED_REG_TYPE_ASSET, reg_asset);
        m_asset_queue.push(reg_asset.asset.uuid);
    }


    /// Submit all assets to in submission queue to renderer
    void __AssetManager::submitAssetQueue() {
        // Check if any assets are available in the queue
        if(m_asset_queue.empty()) 
            return;

        // Keep popping asset ids from the queue
        deng_ui32_t old_size = static_cast<deng_ui32_t>(m_assets.size());
        while(!m_asset_queue.empty()) {
            m_assets.push_back(m_asset_queue.front());
            if(m_win_hints & DENG_WINDOW_HINT_API_VULKAN)
                m_vk_rend->prepareAsset(m_asset_queue.front());

            m_asset_queue.pop();
        }

        // Push data to buffer and allocate descriptor sets
        if(m_win_hints & DENG_WINDOW_HINT_API_VULKAN && m_vk_rend->isInit()) {
            m_vk_rend->updateAssetData({ old_size, static_cast<deng_ui32_t>(m_assets.size()) });
            m_vk_rend->updateAssetDS({ old_size, static_cast<deng_ui32_t>(m_assets.size()) });
            m_vk_rend->updateCmdBuffers(m_vk_vars->background);
        }
    }


    /// Submit all textures in submission queue to renderer
    void __AssetManager::submitTextureQueue() {
        printf("Vulkan renderer pointer value: 0x%p\n", m_vk_rend.get());
        printf("Vulkan config pointer value: 0x%p\n", m_vk_vars.get());
        // Check if any textures are available in the queue
        if(m_texture_queue.empty()) return;

        if((m_win_hints & DENG_WINDOW_HINT_API_VULKAN) && m_vk_rend->isInit())
            m_vk_rend->idle();

        // Pop the queue while not empty
        while(!m_texture_queue.empty()) {
            LOG("Pushed texture to the texture array");
            m_textures.push_back(m_texture_queue.front());

            LOG("Window hint bits are " + std::to_string(m_win_hints) + " but should be " + 
                std::to_string(DENG_WINDOW_HINT_API_VULKAN));
            if(m_win_hints == DENG_WINDOW_HINT_API_VULKAN)
                m_vk_rend->prepareTexture(m_texture_queue.front());

            m_texture_queue.pop();
        }
    }


    /// Push asset to renderer and initialise it, possibly reallocate vertices buffer if needed
    /// PS! Asset UUIDs have to be generated before push and renderer must be setup
    void __AssetManager::pushAsset(das_Asset &asset) {
        submitAsset(asset);
        submitAssetQueue();
    }


    /// Push texture to renderer and initialise it, possibly reallocate texture memory if needed,
    /// PS! Texture UUIDs have to be generated before submitting them and renderer must be setup
    void __AssetManager::pushTexture(das_Texture &texture) {
        submitTexture(texture);
        submitTextureQueue();
    }
}
