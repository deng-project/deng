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
 *  Name: tm - Texture manager for Vulkan resource manager
 *  Purpose: Provide a parent class for creating resource manager, which
 *  handles all operations done on textures.
 *  Author: Karl-Mihkel Ott
 */ 


#ifndef __TM_H
#define __TM_H


#ifdef __TM_CPP
    #include <vector>
    #include <array>
    #include <queue>
    #include <cmath>
    #include <stdlib.h>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/common.h>
    #include <common/hashmap.h>
    #include <common/uuid.h>
    #include <common/err_def.h>

    #include <data/assets.h>
    #include <math/deng_math.h>

    #include <deng/window.h>
    #include <deng/vulkan/sd.h>
    #include <deng/vulkan/qm.h>
    #include <deng/vulkan/resources.h>
    #include <deng/vulkan/rend_infos.h>
    
    #include <deng/lighting/light_srcs.h>
    #include <deng/registry/registry.h>

    #define __DEFAULT_TEX_WIDTH     512
    #define __DEFAULT_TEX_HEIGHT    512
    #define __DEFAULT_TEX_SIZE      1048576 // bytes
    #define __DEFAULT_TEX_MEM_CAP   8388608 // bytes
#endif


namespace deng {
    namespace vulkan {

        class __vk_TextureManager {
        private:
            std::vector<deng_Id> &m_textures;
            deng_ui32_t m_tex_mem_bits = 0;
            deng_Id m_missing_tex_uuid;
            __vk_BufferData &m_bd;
            deng::__GlobalRegistry &m_reg;

        private:
            /// Create missing texture in case any texture mapped asset
            /// has invalid texture uuid
            void __mkMissingTex(VkDevice device, VkPhysicalDevice gpu,
                VkCommandPool cmd_pool, VkQueue g_queue);

            
            /// Create mipmaps for a texture image
            void __mkMipMaps(VkDevice &device, VkCommandPool &cmd_pool,
                VkImage image, VkQueue g_queue, deng_i32_t width, deng_i32_t height,
                deng_ui32_t mip_levels);


            /// Create texture sampler for texture image
            void __mkTextureSampler(VkDevice &device, VkSampler &sampler,
                deng_ui32_t mip_levels);


            /// Allocate memory for VkImage instances
            void __allocateTexMemory(VkDevice device, VkPhysicalDevice gpu,
                VkCommandPool cmd_pool, VkQueue g_queue, VkDeviceSize size);

            
            /// Calculate miplevels for texture specified with specific width and height
            const deng_ui32_t __findMipLevels(const deng_bool_t is_lf, const deng_ui16_t width, 
                const deng_ui16_t height);

            
            /*
             * Destroy previous texture memory instances, allocate new one and 
             * copy texture data over to the new memory area.
             *
             * Allocated memory size is at least min_size floored up to the nearest base 2 exponant,
             * in case the required allocation size floored to nearest base 2 exponant is smaller 
             * than the double of the previous capacity the latter will be used instead
             */
            void __reallocTexMemory(VkDevice device, VkPhysicalDevice gpu, VkCommandPool cmd_pool,
                VkQueue g_queue, VkDeviceSize min_size, deng_bool_t is_lf, deng_ui32_t mip_levels);

            
            /// Check if linear filtering is requested and if mipmaps should be created
            void __mipmapTransition(VkDevice device, VkCommandPool cmd_pool, VkQueue g_queue, deng_bool_t is_lf,
                deng_ui32_t mip_levels, __vk_Texture &tex);

            
            /// Create VkImageViews for texture images
            void __mkImageView(VkDevice device, __vk_Texture &tex, deng_ui32_t mip_levels);


            /// Create new VkImage and VkImageView instances for a single texture
            void __newVkTexture(VkDevice device, VkPhysicalDevice gpu, VkCommandPool cmd_pool,
                VkQueue g_queue, deng_ui32_t mip_levels, deng_bool_t is_lf, deng_bool_t ignore_mem_check,
                deng_bool_t set_default_mem_req, __vk_Texture &tex);


        public:
            __vk_TextureManager(VkDevice device, VkPhysicalDevice gpu, VkCommandPool cmd_pool,
                VkQueue g_queue, std::vector<deng_Id> &textures, deng::__GlobalRegistry &reg,
                __vk_BufferData &bd);


            /// Create new Vulkan texture from texture specified via id
            void mkTexture(VkDevice device, VkPhysicalDevice gpu, VkCommandPool command_pool,
                deng_bool_t is_lf_supported, deng_Id id, VkQueue g_queue);


            /// Copy all bitmap data to image bitmap buffer
            /// WARNING: This method performs no buffer bounds check and can cause errors!
            /// NOTE: Texture object must have a valid VkImage instance created before calling this 
            /// method
            void cpyBitmap(VkDevice device, VkPhysicalDevice gpu, VkCommandPool cmd_pool,
                VkQueue g_queue, deng_ui32_t mip_levels, __vk_Texture &tex);


            /// Get the id of a missing texture
            deng_Id getMissingTextureUUID();
        };
    }
}

#endif
