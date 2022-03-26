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
    #include <cstring>
#ifdef _DEBUG
    #include <iostream>
#endif

    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Points.h>

    #include <Api.h>
    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
    #include <VulkanHelpers.h>
    #include <VulkanDescriptorPoolCreator.h>
    #include <VulkanDescriptorSetLayoutCreator.h>
    #include <Window.h>
    #include <Renderer.h>
#endif

namespace DENG {

    namespace Vulkan {

        class DescriptorSetsCreator {
            private:
                std::vector<VkDescriptorSet> m_descriptor_sets;

                VkDevice m_device;
                VkBuffer &m_uniform_buffer;
                std::vector<Vulkan::TextureData> *m_textures = nullptr;
                VkDescriptorPool m_descriptor_pool;
                VkDescriptorSetLayout m_descriptor_set_layout;
                ShaderModule const *m_shader_module = nullptr;
                MeshReference const *m_mesh_module = nullptr;
                uint32_t m_swapchain_images_count;

            private:
                void _CreateDescriptorSets();
                std::vector<VkDescriptorBufferInfo> _FindBufferInfos();
                std::vector<VkDescriptorImageInfo> _FindImageInfos();
                std::vector<VkWriteDescriptorSet> _FindWriteDescriptorInfos(const std::vector<VkDescriptorBufferInfo> &_buf_infos, const std::vector<VkDescriptorImageInfo> &_img_infos);

            public:
                DescriptorSetsCreator() = default;
                DescriptorSetsCreator(const DescriptorSetsCreator &_dsc) = default;
                DescriptorSetsCreator(DescriptorSetsCreator &&_dsc);

                // shader descriptor set creator
                DescriptorSetsCreator(VkDevice _dev, uint32_t _sc_img_count, const ShaderModule &_module, VkBuffer &_ubo_buffer, VkDescriptorPool _desc_pool, VkDescriptorSetLayout _dest_set_layout, std::vector<Vulkan::TextureData> &_textures);

                // mesh descriptor set creator
                DescriptorSetsCreator(VkDevice _dev, uint32_t _sc_img_count, const MeshReference &_mesh, VkBuffer &_ubo_buffer, VkDescriptorPool _desc_pool, VkDescriptorSetLayout _desc_set_layout);
                ~DescriptorSetsCreator() = default;

                DescriptorSetsCreator &operator=(const DescriptorSetsCreator &_dc);
                DescriptorSetsCreator &operator=(DescriptorSetsCreator &&_dc);

                void UpdateDescriptorSets();

                //void RecreateDescriptorSets(const Libdas::Point2D<uint32_t> &_bounds, std::vector<ShaderModule*> &_modules);
                
                inline VkDescriptorSet GetDescriptorSetById(uint32_t _id) {
                    DENG_ASSERT(_id < m_descriptor_sets.size());
                    return m_descriptor_sets[_id];
                }
        };
    }
}

#endif
