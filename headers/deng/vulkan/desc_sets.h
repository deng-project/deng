/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: desc_sets.cpp - Vulkan descriptor set creator class implementation
/// author: Karl-Mihkel Ott


#ifndef __VK_DESC_SETS_H
#define __VK_DESC_SETS_H

#ifdef __VK_DESC_SETS_CPP
    #include <vector>
    #include <string>
    #include <array>

    #include <vulkan/vulkan.h>
    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <common/hashmap.h>
    #include <common/common.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
    #include <deng/window.h>
    #include <deng/cross_api/shader_def.h>
    #include <deng/vulkan/sd.h>
    #include <deng/vulkan/qm.h>
    #include <deng/vulkan/resources.h>

    #include <deng/lighting/light_srcs.h>
    #include <deng/cross_api/ubo.h>
    #include <deng/registry/registry.h>

    #include <deng/vulkan/pipeline_data.h>
    #include <deng/vulkan/pipelines.h>
    #include <deng/vulkan/desc_set_layout.h>
    #include <deng/vulkan/desc_pool.h>


    // Default capacity for descriptor pool
    #define __DEFAULT_DESC_POOL_CAP         32
    extern deng_ui32_t __max_frame_c;
#endif

namespace deng {
    namespace vulkan {

        /// Abstract class for creating descriptor descriptor sets and pipelines
        class __vk_DescriptorSetsCreator : public __vk_DescriptorPoolCreator, 
                                       public __vk_DescriptorSetLayoutCreator {   
        private:
            std::vector<deng_Id> &m_assets;
            std::vector<deng_Id> &m_textures;
            
            // Registered asset count
            deng_ui64_t m_2d_unmapped_asset_c = 0;
            deng_ui64_t m_2d_mapped_asset_c = 0;
            deng_ui64_t m_3d_unmapped_asset_c = 0;
            deng_ui64_t m_3d_mapped_asset_c = 0;

            deng::Registry &m_reg;
            std::vector<VkDescriptorSet> m_ui_desc_sets;

        private:
            /// Check if the pool was reallocated and if it was reallocate 
            /// descriptor sets for each asset that had them destroyed
            void __reallocCheck(VkDevice device, __vk_BufferData &bd, deng_Id missing_tex_id,
                deng_ui64_t min_ubo_align, deng_ui64_t ubo_chunk_size);

            /// Find the bufferinfos based on the asset mode used
            std::vector<VkDescriptorBufferInfo> __findBufferInfos(das_Asset &asset,
                __vk_BufferData &bd, deng_ui64_t cur_frame, deng_ui64_t frame_c, 
                deng_ui64_t min_align, deng_ui64_t ubo_chunk_size);


            /// Create write descriptors for texture mapped assets
            const std::vector<VkWriteDescriptorSet> __mkMappedWriteDescInfos(das_AssetMode asset_mode, 
                std::vector<VkDescriptorBufferInfo> &buffer_infos, VkDescriptorSet &desc_set,
                VkDescriptorImageInfo &img_info);


            /// Create write descriptors for unmapped assets
            const std::vector<VkWriteDescriptorSet> __mkUnmappedWriteDescInfos(das_AssetMode asset_mode,
                std::vector<VkDescriptorBufferInfo> &buffer_info, VkDescriptorSet &desc_set);


            /// Create write descriptors for ImGui elements
            const VkWriteDescriptorSet __mkUIWriteDescInfo(const VkDescriptorSet &desc_set, 
                const VkDescriptorImageInfo &img_info);


            /// Create descriptor sets for unmapped assets
            void __mkUnmappedDS(VkDevice device, __vk_Asset &asset, __vk_BufferData &bd,
                deng_ui64_t min_align, deng_ui64_t ubo_chunk_size);
            

            /// Create descriptor sets for texture mapped assets
            void __mkTexMappedDS(VkDevice device, __vk_Asset &asset, __vk_BufferData &bd,
                deng_Id missing_tex_uuid, deng_ui64_t min_align, deng_ui64_t ubo_chunk_size);

            
            /// Reallocate previously destroyed descriptor set instances
            void __restoreDestroyedDescriptorSets();

        public:
            __vk_DescriptorSetsCreator (
                VkDevice device,
                VkExtent2D ext,
                VkRenderPass renderpass, 
                deng::Registry &reg,
                std::vector<deng_Id> &assets,
                std::vector<deng_Id> &textures,
                VkSampleCountFlagBits sample_c
            );


            /// Abstracted method for creating descriptor sets for each asset
            /// for each asset
            void mkAssetDS(VkDevice device, __vk_BufferData &bd, deng_Id missing_tex_uuid,
                const dengMath::vec2<deng_ui32_t> &asset_bounds, deng_ui64_t ubo_chunk_size,
                deng_ui64_t min_ubo_align);


            /// Create descriptor sets for ImGui elements
            void mkUIDS(VkDevice device, __vk_BufferData &bd, deng_Id texture_atlas);

        // Getter methods
        public:
            const std::vector<VkDescriptorSet> &getUIDS();
        };
    }
}

#endif
