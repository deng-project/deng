/*    ____         ________     __      _        ______  
 *   |  _ \_      |  ______|   |  \    | |     _/  ____\
 *   | | \_ \_    |  |         |   \   | |    /  _/   
 *   | |   \  |   |  |_____    | |\ \  | |   |  |   _____ 
 *   | |    | |   |  ______|   | | \ \ | |   |  |  |___  | 
 *   | |  _/ _|   |  |         | |  \ \| |    \  \    / /
 *   | |_/ _/     |  |_____    | |   \   |     \_ \__/ /
 *   |____/       |________|   |_|    \__|       \____/
 *               
 * __________________________________________________________
 * __________________________________________________________
 *                       Project DENG 
 *
 *
 * Copyright (C) 2020 - 2021
 * This Software is licensed under Apache License as described 
 * in the LICENSE.md file, which you should have recieved with 
 * this distribution.
 * 
 * You may reproduce and distribute copies of the
 * Work or Derivative Works thereof in any medium, with or without
 * modifications, and in Source or Object form, provided that You
 * meet the following conditions:
 *
 * (a) You must give any other recipients of the Work or
 *     Derivative Works a copy of this License; and
 *
 * (b) You must cause any modified files to carry prominent notices
 *     stating that You changed the files; and
 *
 * (c) You must retain, in the Source form of any Derivative Works
 *     that You distribute, all copyright, patent, trademark, and
 *     attribution notices from the Source form of the Work,
 *     excluding those notices that do not pertain to any part of
 *     the Derivative Works; and
 *
 * (d) If the Work includes a "NOTICE" text file as part of its
 *     distribution, then any Derivative Works that You distribute must
 *     include a readable copy of the attribution notices contained
 *     within such NOTICE file, excluding those notices that do not
 *     pertain to any part of the Derivative Works, in at least one
 *     of the following places: within a NOTICE text file distributed
 *     as part of the Derivative Works; within the Source form or
 *     documentation, if provided along with the Derivative Works; or,
 *     within a display generated by the Derivative Works, if and
 *     wherever such third-party notices normally appear. The contents
 *     of the NOTICE file are for informational purposes only and
 *     do not modify the License. You may add Your own attribution
 *     notices within Derivative Works that You distribute, alongside
 *     or as an addendum to the NOTICE text from the Work, provided
 *     that such additional attribution notices cannot be construed
 *     as modifying the License.
 *
 * You may add Your own copyright statement to Your modifications and
 * may provide additional or different license terms and conditions
 * for use, reproduction, or distribution of Your modifications, or
 * for any such Derivative Works as a whole, provided Your use,
 * reproduction, and distribution of the Work otherwise complies with
 * the conditions stated in this License.
 * ----------------------------------------------------------------
 *  Name: asset_man - Asset manager parent for Renderer class
 *  Purpose: Provide interface for interacting with assets and textures
 *  Author: Karl-Mihkel Ott
 */ 


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
    #include <deng/camera.h>
    #include <deng/registry/registry.h>
    #include <imgui-layer/imgui_entity.h>

    #include <utils/font.h>
    #include <deng/vulkan/renderer.h>
#endif


namespace deng {

    class __AssetManager {
    private:
        std::shared_ptr<vulkan::__vk_ConfigVars> &m_vk_vars;
        std::shared_ptr<vulkan::__vk_Renderer> &m_vk_rend;
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
        deng_RendererHintBits &m_api_bits;

        __GlobalRegistry m_reg;
        std::vector<deng_Id> m_assets;
        std::vector<deng_Id> m_textures;
        
    public:
        __AssetManager(std::shared_ptr<vulkan::__vk_Renderer> &vk_rend,
            std::shared_ptr<vulkan::__vk_ConfigVars> &vk_vars, deng_RendererHintBits &api);

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
