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
                std::vector<VkDescriptorSet> m_descriptor_sets;

                VkDevice m_device;
                uint32_t m_swapchain_images_count;
                UniformBufferAllocator *mp_ubo_allocator = nullptr;
                std::vector<Vulkan::TextureData> *m_textures = nullptr;
                VkDescriptorPool m_descriptor_pool;
                VkDescriptorSetLayout m_descriptor_set_layout;
                ShaderModule const *m_shader_module;
                uint32_t m_mod_id = 0;

            private:
                void _CreateDescriptorSets();
                std::vector<VkDescriptorBufferInfo> _FindBufferInfos();
                std::vector<VkDescriptorImageInfo> _FindImageInfos();
                std::vector<VkWriteDescriptorSet> _FindWriteDescriptorInfos(const std::vector<VkDescriptorBufferInfo> &_buf_infos, const std::vector<VkDescriptorImageInfo> &_img_infos);

            public:
                DescriptorSetsCreator() = default;
                DescriptorSetsCreator(const DescriptorSetsCreator &_dsc) = default;
                DescriptorSetsCreator(DescriptorSetsCreator &&_dsc);
                DescriptorSetsCreator(VkDevice _dev, uint32_t _sc_img_count, const ShaderModule &_module, uint32_t _mod_id, UniformBufferAllocator *_ubo_allocator, 
                                      VkDescriptorPool _desc_pool, VkDescriptorSetLayout _dest_set_layout, std::vector<Vulkan::TextureData> &_textures);
                ~DescriptorSetsCreator() = default;

                DescriptorSetsCreator &operator=(const DescriptorSetsCreator &_dc);
                DescriptorSetsCreator &operator=(DescriptorSetsCreator &&_dc);

                //void RecreateDescriptorSets(const Libdas::Point2D<uint32_t> &_bounds, std::vector<ShaderModule*> &_modules);
                
                inline VkDescriptorSet GetDescriptorSetById(uint32_t _id) {
                    DENG_ASSERT(_id < m_descriptor_sets.size());
                    return m_descriptor_sets[_id];
                }
        };
    }
}

#endif
