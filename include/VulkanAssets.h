// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanAssets.h - Provide structures for Vulkan assets and textures
// author: Karl-Mihkel Ott


#ifndef VULKAN_ASSETS_H
#define VULKAN_ASSETS_H

namespace DENG {
    namespace Vulkan {

        /// Struct for assets in vulkan renderer
        struct Asset {
            deng_Id base_id;
            deng_Id tex_uuid;
            deng_Id uuid;

            // Boolean flag for checking if descriptor sets have been created
            deng_bool_t is_desc;
            VkDescriptorSet *desc_sets;
            uint32_t desc_c;
        };

        
        /// Struct for texture images
        /// This struct contains textures and their Vulkan image buffers, views, samplers 
        /// and memory objects. Additionally each texture image instance in DENG contains 
        /// data to uniform color data in case texture mapping is disabled for certain assets
        struct Texture {
            deng_Id base_id;
            deng_Id uuid;
            bool is_buffered;
            VkImage image;
            VkImageView image_view;
            VkSampler sampler;
        };
    }
}

#endif
