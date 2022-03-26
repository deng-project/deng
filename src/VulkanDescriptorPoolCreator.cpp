// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanDescriptorPoolCreater.cpp - Vulkan descriptor pool creator class implementation
// author: Karl-Mihkel Ott

#define VULKAN_DESCRIPTOR_POOL_CREATOR_CPP
#include <VulkanDescriptorPoolCreator.h>


namespace DENG {

    namespace Vulkan {

        DescriptorPoolCreator::DescriptorPoolCreator(VkDevice _dev, uint32_t _swapchain_image_c, uint32_t _texture_count, const std::vector<UniformDataLayout> &_ubo_layouts) : 
            m_device(_dev), m_swapchain_image_count(_swapchain_image_c), m_texture_count(_texture_count)
        {
            _LoadUniformsForShader(_ubo_layouts);
        }


        DescriptorPoolCreator::DescriptorPoolCreator(DescriptorPoolCreator && _dpc) : m_device(_dpc.m_device), m_swapchain_image_count(_dpc.m_swapchain_image_count), 
            m_texture_count(_dpc.m_texture_count), m_descriptor_pool(_dpc.m_descriptor_pool)
        {
            m_descriptor_pool = VK_NULL_HANDLE;
        }


        DescriptorPoolCreator::~DescriptorPoolCreator() {
            if(m_descriptor_pool != VK_NULL_HANDLE)
                vkDestroyDescriptorPool(m_device, m_descriptor_pool, NULL);
        }


        DescriptorPoolCreator &DescriptorPoolCreator::operator=(const DescriptorPoolCreator &_dpc) {
            m_device = _dpc.m_device;
            m_swapchain_image_count = _dpc.m_swapchain_image_count;
            m_texture_count = _dpc.m_texture_count;
            m_descriptor_pool = _dpc.m_descriptor_pool;

            return *this;
        }


        DescriptorPoolCreator &DescriptorPoolCreator::operator=(DescriptorPoolCreator &&_dpc) {
            m_device = _dpc.m_device;
            m_swapchain_image_count = _dpc.m_swapchain_image_count;
            m_texture_count = _dpc.m_texture_count;
            m_descriptor_pool = _dpc.m_descriptor_pool;

            _dpc.m_descriptor_pool = VK_NULL_HANDLE;
            
            return *this;
        }


        std::vector<VkDescriptorPoolSize> DescriptorPoolCreator::_FindDescriptorPoolSizes(const std::vector<UniformDataLayout> &_ubo_layouts) {
            std::vector<VkDescriptorPoolSize> desc_sizes(_ubo_layouts.size());
            const uint32_t desc_count = (m_texture_count == 0 ? 1 : m_texture_count) * m_swapchain_image_count;

            for(size_t i = 0; i < desc_sizes.size(); i++) {
                switch(_ubo_layouts[i].type) {
                    case UNIFORM_DATA_TYPE_BUFFER:
                        desc_sizes[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        break;

                    case UNIFORM_DATA_TYPE_IMAGE_SAMPLER:
                        desc_sizes[i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        break;
                }

                desc_sizes[i].descriptorCount = desc_count;
            }

            return desc_sizes;
        }


        void DescriptorPoolCreator::_LoadUniformsForShader(const std::vector<UniformDataLayout> &_ubo_layouts) {
            std::vector<VkDescriptorPoolSize> desc_sizes = _FindDescriptorPoolSizes(_ubo_layouts);

            // set createinfo struct
            VkDescriptorPoolCreateInfo desc_pool_info = {};
            desc_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            desc_pool_info.poolSizeCount = static_cast<uint32_t>(desc_sizes.size());
            desc_pool_info.pPoolSizes = desc_sizes.data();
            desc_pool_info.maxSets = (m_texture_count == 0 ? 1 : m_texture_count) * m_swapchain_image_count;
            desc_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

            if(vkCreateDescriptorPool(m_device, &desc_pool_info, NULL, &m_descriptor_pool) != VK_SUCCESS)
                VK_DESC_ERR("failed to create descriptor pool");
        }
    }
}
