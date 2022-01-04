/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: tm.h - Vulkan texture manager class header
/// author: Karl-Mihkel Ott

#ifndef __VK_TM_H
#define __VK_TM_H


#ifdef __VK_TM_CPP
    #include <vector>
    #include <array>
    #include <queue>
    #include <cmath>
    #include <stdlib.h>
    #include <string.h>
	#include <string>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/common.h>
    #include <common/hashmap.h>
    #include <common/uuid.h>
    #include <common/err_def.h>

    #include <data/assets.h>
    #include <math/deng_math.h>

    #include <deng/cross_api/gpu_mem.h>
    #include <deng/window.h>
    #include <deng/vulkan/sd.h>
    #include <deng/vulkan/qm.h>
    #include <deng/vulkan/resources.h>
    #include <deng/vulkan/rend_infos.h>
    
    #include <deng/lighting/light_srcs.h>
    #include <deng/registry/registry.h>
#endif


namespace deng {
    namespace vulkan {

        class __vk_TextureManager {
        private:
            std::vector<deng_Id> &m_textures;
            deng_ui32_t m_tex_mem_bits = 0;
            deng_Id m_missing_tex_uuid;
            __vk_BufferData &m_bd;
            deng::BufferSectionInfo &m_buf_sec;
            deng::Registry &m_reg;
            void *m_udata;

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
                VkQueue g_queue, std::vector<deng_Id> &textures, deng::Registry &reg,
                __vk_BufferData &bd, deng::BufferSectionInfo &buf_sec, void *udata);


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
