// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanDescriptorSetsCreator.h - Vulkan descriptor set creator class header
// author: Karl-Mihkel Ott


#ifndef VULKAN_DESCRIPTOR_SETS_CREATOR_H
#define VULKAN_DESCRIPTOR_SETS_CREATOR_H

#ifdef VULKAN_DESCRIPTOR_SETS_CREATOR_CPP
    #include <vector>
    #include <string>
    #include <vulkan/vulkan.h>

    #include <libdas/include/Points.h>

    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
    #include <VulkanHelpers.h>
    #include <VulkanUniformBufferAllocator.h>
    #include <VulkanDescriptorPoolCreator.h>
    #include <VulkanDescriptorSetLayoutCreator.h>
#endif

namespace DENG {

    namespace Vulkan {

        class DescriptorSetsCreator {
            private:
                typedef std::vector<VkDescriptorSet> DescriptorSetsPerSwapchainImage;
                std::vector<DescriptorSetsPerSwapchainImage> m_descriptor_sets;

                VkDevice m_device;
                VkBuffer m_uniform_buffer;
                uint32_t m_swapchain_images_count;
                UniformBufferAllocator *mp_ubo_allocator;
                DescriptorPoolCreator *mp_descriptor_pool_creator;
                DescriptorSetLayoutCreator *mp_descriptor_set_layout_creator;
                bool m_is_init = false;

            private:
                void _CreateDescriptorSets(const Libdas::Point2D<uint32_t> &_bounds);

            public:
                DescriptorSetsCreator() = default;
                DescriptorSetsCreator(const DescriptorSetsCreator &_dc) = default;
                DescriptorSetsCreator(VkDevice _dev, uint32_t _sc_img_count, UniformBufferAllocator *_ubo_allocator, DescriptorPoolCreator &_desc_pool_creator, 
                                      DescriptorSetLayoutCreator *_desc_set_layout_creator);
                ~DescriptorSetsCreator();

                void RecreateDescriptorSets(const Libdas::Point2D<uint32_t> &_bounds);

                inline DescriptorSetsPerSwapchainImage &GetDescriptorSetsById(uint32_t _id) {
                    return m_descriptor_sets[_id];
                }

                inline bool IsInit() {
                    return m_is_init;
                }
        };

#if 0
        /// Abstract class for creating descriptor descriptor sets and pipelines
        class DescriptorSetsCreator : public DescriptorPoolCreator, 
                                       public DescriptorSetLayoutCreator {   
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
            void *m_udata;

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
                VkSampleCountFlagBits sample_c,
                void *udata
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
#endif
    }
}

#endif
