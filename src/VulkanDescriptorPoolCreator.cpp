// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanDescriptorPoolCreater.cpp - Vulkan descriptor pool creator class implementation
// author: Karl-Mihkel Ott

#define VULKAN_DESCRIPTOR_POOL_CREATOR_CPP
#include <VulkanDescriptorPoolCreator.h>


namespace DENG {

    namespace Vulkan {

        DescriptorPoolCreator::DescriptorPoolCreator(VkDevice _dev, uint32_t _swapchain_image_c, const std::vector<UniformDataLayout> &_ubo_layouts) : 
            m_device(_dev), m_swapchain_image_count(_swapchain_image_c), m_is_init(true)
        {
            _LoadUniformsForShader(_ubo_layouts);
        }


        DescriptorPoolCreator::~DescriptorPoolCreator() {
            for(std::pair<VkDescriptorPool, uint32_t> &pool : m_descriptor_pools)
                vkDestroyDescriptorPool(m_device, pool.first, NULL);
        }


        std::vector<VkDescriptorPoolSize> DescriptorPoolCreator::_FindDescriptorPoolSizes(const std::vector<UniformDataLayout> &_ubo_layouts, uint32_t _desc_c) {
            std::vector<VkDescriptorPoolSize> desc_sizes(_ubo_layouts.size());

            for(size_t i = 0; i < desc_sizes.size(); i++) {
                switch(_ubo_layouts[i].type) {
                    case UNIFORM_DATA_TYPE_BUFFER:
                        desc_sizes[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        break;

                    case UNIFORM_DATA_TYPE_IMAGE_SAMPLER:
                        desc_sizes[i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        break;
                }

                desc_sizes[i].descriptorCount = _desc_c;
            }

            return desc_sizes;
        }


        void DescriptorPoolCreator::_LoadUniformsForShader(const std::vector<UniformDataLayout> &_ubo_layouts) {
            std::vector<VkDescriptorPoolSize> desc_sizes = _FindDescriptorPoolSizes(_ubo_layouts, DEFAULT_DESCRIPTOR_POOL_CAP * m_swapchain_image_count);

            // set createinfo struct
            VkDescriptorPoolCreateInfo desc_pool_info = {};
            desc_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            desc_pool_info.poolSizeCount = static_cast<uint32_t>(desc_sizes.size());
            desc_pool_info.pPoolSizes = desc_sizes.data();
            // desc_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            desc_pool_info.maxSets = DEFAULT_DESCRIPTOR_POOL_CAP * m_swapchain_image_count;

            VkDescriptorPool desc_pool = VK_NULL_HANDLE;
            if(vkCreateDescriptorPool(m_device, &desc_pool_info, NULL, &desc_pool) != VK_SUCCESS)
                VK_DESC_ERR("failed to create descriptor pool");

            m_descriptor_pools.push_back(std::make_pair(desc_pool, desc_pool_info.maxSets));
        }


        void DescriptorPoolCreator::RecreateDescriptorPool(const std::vector<VkDescriptorSet> &_desc_sets, uint32_t _index, uint32_t _req_cap, 
                                                           const std::vector<UniformDataLayout> &_ubo_layouts) 
        {
            m_descriptor_pools[_index].second = m_descriptor_pools[_index].second * 3 / 2 > _req_cap * 3 / 2 ? 
                m_descriptor_pools[_index].second * 3 / 2 : _req_cap * 3 / 2;

            vkFreeDescriptorSets(m_device, m_descriptor_pools[_index].first, static_cast<uint32_t>(_desc_sets.size()), _desc_sets.data());
            vkDestroyDescriptorPool(m_device, m_descriptor_pools[_index].first, NULL);

            std::vector<VkDescriptorPoolSize> desc_sizes = _FindDescriptorPoolSizes(_ubo_layouts, m_descriptor_pools[_index].second);
            VkDescriptorPoolCreateInfo desc_pool_info = {};

            desc_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            desc_pool_info.poolSizeCount = static_cast<uint32_t>(desc_sizes.size()) * m_swapchain_image_count;
            desc_pool_info.pPoolSizes = desc_sizes.data();
            desc_pool_info.maxSets = m_descriptor_pools[_index].second;

            if(vkCreateDescriptorPool(m_device, &desc_pool_info, NULL, &m_descriptor_pools[_index].first))
                VK_DESC_ERR("failed to create a descriptor pool");
        }
    }
}
