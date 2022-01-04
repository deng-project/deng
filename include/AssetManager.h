// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: AssetManager.h - Asset handling class header
// author: Karl-Mihkel Ott


#ifndef ASSET_MAN_H
#define ASSET_MAN_H

#ifdef ASSET_MAN_CPP
    #include <stdlib.h>
    #include <string.h>
    #include <vector>
    #include <array>
    #include <mutex>
    #include <string>
    #include <memory>
    #include <queue>
    #include <vulkan/vulkan.h>

    #include <base_types.h>
    #include <err_def.h>
    #include <hashmap.h>
    #include <assets.h>
    #include <deng_math.h>

    #include <window.h>
    #include <cam3d.h>
    #include <registry.h>
    #include <imgui_entity.h>
    
    #include <cross_api.h>
    #include <VulkanRenderer.h>
    #include <OpenGLRenderer.h>
#endif


namespace DENG {

    class AssetManager {
    private:
        vulkan::__vk_ConfigVars &m_vk_vars;
        opengl::__gl_ConfigVars &m_gl_vars;
        std::queue<deng_Id> m_asset_queue;
        std::queue<deng_Id> m_texture_queue;

        // Bookkeeping variables for assets
        deng_ui32_t m_vu2d_c = 0;
        deng_ui32_t m_vm2d_c = 0;
        deng_ui32_t m_vu3d_c = 0;
        deng_ui32_t m_vm3d_c = 0;

    private:
        /// Increment asset type instance count
        void _AssetTypeIncr(das_Asset &asset);

    protected:
        deng_WindowHint m_win_hints = 0;
        Registry m_reg;
        std::vector<deng_Id> m_assets;
        std::vector<deng_Id> m_textures;
        std::shared_ptr<vulkan::__vk_Renderer> m_vk_rend = {};
        std::shared_ptr<opengl::__gl_Renderer> m_gl_rend = {};
        
    public:
        AssetManager(vulkan::__vk_ConfigVars &vk_cfg, opengl::__gl_ConfigVars &gl_cfg);

        /// Add texture id to submission queue
        /// PS! Texture UUIDs have to be generated before submitting them
        void SubmitTexture(das_Texture &texture);


        /// Add asset id to submission queue
        /// PS! Asset UUIDs have to be generated before submitting them
        void SubmitAsset(das_Asset &asset);


        /// Submit all assets to in submission queue to renderer
        void SubmitAssetQueue();


        /// Submit all textures in submission queue to renderer
        void SubmitTextureQueue();


        /// Push asset to renderer and initialise it, possibly reallocate vertices buffer if needed
        /// PS! Asset UUIDs have to be generated before push and renderer must be setup
        void PushAsset(das_Asset &asset);


        /// Push texture to renderer and initialise it, possibly reallocate texture memory if needed,
        /// PS! Texture UUIDs have to be generated before submitting them and renderer must be setup
        void pushTexture(das_Texture &texture);
    };
}

#endif
