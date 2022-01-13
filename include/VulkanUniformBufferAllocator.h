// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanUniformBufferAllocator.h - Vulkan uniform buffer allocator and management class header
// author: Karl-Mihkel Ott


#ifndef VULKAN_UNIFORM_BUFFER_ALLOCATOR_H
#define VULKAN_UNIFORM_BUFFER_ALLOCATOR_H

#ifdef VULKAN_UNIFORM_BUFFER_ALLOCATOR_CPP
    #include <string>
    #include <vector>
    #include <vulkan/vulkan.h>

    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
    #include <VulkanHelpers.h>
#endif

namespace DENG {

    namespace Vulkan {

        class UniformBufferAllocator {
            private:
                VkDeviceSize m_offset;
                std::vector<std::vector<VkDeviceSize>> m_area_offsets;
                const uint32_t m_min_align;
                const uint32_t m_swapchain_image_count;

                // vulkan stuff
                VkDevice m_device;
                VkPhysicalDevice m_gpu;
                VkQueue m_graphics_queue;
                VkCommandPool m_cmd_pool;
                VkBuffer m_uniform_buffer;
                VkDeviceMemory m_uniform_buffer_memory;

            private:
                void _FindOffsets(const std::vector<UniformDataLayout> &_layouts);
                void _CreateUniformBuffer();

            public:
                UniformBufferAllocator(VkDevice _dev, VkPhysicalDevice _gpu, VkQueue _graphics_q, VkCommandPool _cmd_pool, 
                                       uint32_t _min_align, uint32_t _sc_img_count, const std::vector<ShaderModule*> &_modules);
                ~UniformBufferAllocator();
                void RecreateUniformBuffer(const std::vector<ShaderModule*> &_modules);


                inline void UpdateDataAtIndex(uint32_t _index, uint32_t _frame, const std::vector<char> &_data) {
                    Vulkan::_CopyToBufferMemory(m_device, _data.size(), _data.data(), m_uniform_buffer_memory, static_cast<VkDeviceSize>(_frame) * _data.size());
                }

                inline VkBuffer GetUniformBuffer() { return m_uniform_buffer; }
        };
    }
}

#endif
