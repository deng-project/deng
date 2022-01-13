#define VULKAN_DESCRIPTOR_SETS_CREATOR_CPP
#include <VulkanDescriptorSetsCreator.h>

namespace DENG {

    namespace Vulkan {

        DescriptorSetsCreator::DescriptorSetsCreator(VkDevice _dev, uint32_t _sc_img_count, UniformBufferAllocator *_ubo_allocator, DescriptorPoolCreator *_desc_pool_creator, 
                                                     DescriptorSetLayoutCreator *_desc_set_layout_creator) : m_device(_dev), m_swapchain_images_count(_sc_img_count), mp_ubo_allocator(_ubo_allocator), 
                                                     mp_descriptor_pool_creator(_desc_pool_creator), mp_descriptor_set_layout_creator(_desc_set_layout_creator) 
        {
            _CreateDescriptorSets(Libdas::Point2D<uint32_t>(0, mp_descriptor_pool_creator->GetDescriptorPoolCount()));
        }


        DescriptorSetsCreator::~DescriptorSetsCreator() {
            for(uint32_t i = 0; i < static_cast<uint32_t>(m_descriptor_sets.size()); i++)
                vkFreeDescriptorSets(m_device, mp_descriptor_pool_creator->GetDescriptorPoolById(i).first, m_descriptor_sets[i].size(), m_descriptor_sets[i].data());
        }


        void DescriptorSetsCreator::_CreateDescriptorSets(const Libdas::Point2D<uint32_t> &_bounds) {
            for(uint32_t i = _bounds.x; i < _bounds.y; i++) {
                std::vector<VkDescriptorSetLayout> layouts(m_swapchain_images_count, mp_descriptor_set_layout_creator->GetLayoutSetById(i));

                VkDescriptorSetAllocateInfo allocation_info = {};
                allocation_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                allocation_info.descriptorPool = mp_descriptor_pool_creator->GetDescriptorPoolById(static_cast<uint32_t>(i)).first;
                allocation_info.descriptorSetCount = m_swapchain_images_count;
                allocation_info.pSetLayouts = layouts.data();

                std::vector<VkDescriptorSet> desc_sets(m_swapchain_images_count);
                if(vkAllocateDescriptorSets(m_device, &allocation_info, desc_sets.data()))
                    VK_DESC_ERR("failed to allocate descriptor sets");

                for(uint32_t j = 0; j < m_swapchain_images_count; j++) {
                    VkDescriptorBufferInfo buffer_info = {};
                    buffer_info.buffer = m_uniform_buffer;
                }
            }
        }


        void DescriptorSetsCreator::RecreateDescriptorSets(const Libdas::Point2D<uint32_t> &_bounds) {
            // free all descriptor sets requested
            for(uint32_t i = _bounds.x; i < _bounds.y; i++)
                vkFreeDescriptorSets(m_device, mp_descriptor_pool_creator->GetDescriptorPoolById(i).first, m_descriptor_sets[i].size(), m_descriptor_sets[i].data());

            _CreateDescriptorSets(_bounds);
        }
    }
}
