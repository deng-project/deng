// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanUniformBufferAllocator.cpp - Vulkan uniform buffer allocator and management class implementation
// author: Karl-Mihkel Ott

#define VULKAN_UNIFORM_BUFFER_ALLOCATOR_CPP
#include <VulkanUniformBufferAllocator.h>

namespace DENG {

    namespace Vulkan {

        UniformBufferAllocator::UniformBufferAllocator(VkDevice _dev, VkPhysicalDevice _gpu, VkQueue _graphics_q, VkCommandPool _cmd_pool, 
                                                       uint32_t _min_align, uint32_t _sc_img_count, const std::vector<ShaderModule*> &_modules) : 
                                                       m_min_align(_min_align), m_swapchain_image_count(_sc_img_count), m_device(_dev), m_gpu(_gpu), 
                                                       m_graphics_queue(_graphics_q), m_cmd_pool(_cmd_pool) 
        {
            for(size_t i = 0; i < _modules.size(); i++)
                _FindOffsets(_modules[i]->ubo_data_layouts);
            m_offset = m_swapchain_image_count * m_min_align;
            _CreateUniformBuffer();
        }

        
        UniformBufferAllocator::~UniformBufferAllocator() {
            vkFreeMemory(m_device, m_uniform_buffer_memory, NULL);
            vkDestroyBuffer(m_device, m_uniform_buffer, NULL);
        }


        void UniformBufferAllocator::_FindOffsets(const std::vector<UniformDataLayout> &_layouts) {
            m_area_offsets.push_back(std::vector<VkDeviceSize>());
            for(size_t i = 0; i < _layouts.size(); i++) {
                m_area_offsets.back().push_back(m_offset);
                m_offset += m_swapchain_image_count * AlignData(_layouts[i].ubo_size, m_min_align);
            }
        }


        void UniformBufferAllocator::_CreateUniformBuffer() {
            // create buffer object
            VkMemoryRequirements mem_req = _CreateBuffer(m_device, m_offset, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, m_uniform_buffer);
            _AllocateMemory(m_device, m_gpu, mem_req.size, m_uniform_buffer_memory, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            if(vkBindBufferMemory(m_device, m_uniform_buffer, m_uniform_buffer_memory, 0) != VK_SUCCESS)
                VK_BUFFER_ERR("could not bind buffer to its memory");
        }


        void UniformBufferAllocator::RecreateUniformBuffer(const std::vector<ShaderModule*> &_modules) {
            // free used resources
            vkFreeMemory(m_device, m_uniform_buffer_memory, NULL);
            vkDestroyBuffer(m_device, m_uniform_buffer, NULL);

            // find new offsets
            m_area_offsets.clear();
            for(size_t i = 0; i < _modules.size(); i++)
                _FindOffsets(_modules[i]->ubo_data_layouts);

            _CreateUniformBuffer();
        }
    }
}
