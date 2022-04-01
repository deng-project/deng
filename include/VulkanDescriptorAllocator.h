// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: DescriptorAllocator.h - Vulkan descriptor allocation management class header
// author: Karl-Mihkel Ott

#ifndef VULKAN_DESCRIPTOR_ALLOCATOR_H
#define VULKAN_DESCRIPTOR_ALLOCATOR_H

#ifdef VULKAN_DESCRIPTOR_ALLOCATOR_CPP
    #include <string>
    #include <vector>
    #include <unordered_map>
    #include <variant>
    #include <vulkan/vulkan.h>

    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
    #include <VulkanHelpers.h>
    #include <Missing.h>
#endif

namespace DENG {
    namespace Vulkan {

        class DescriptorAllocator {
            private:
                const VkDevice m_device;
                const VkBuffer m_uniform_buffer;
                const VkDescriptorSetLayout m_descriptor_set_layout;
                const std::variant<std::vector<UniformDataLayout>*, std::vector<UniformBufferBlock>*> m_ubo_desc;
                const uint32_t m_swapchain_image_count;
                const Vulkan::TextureData &m_missing;
                bool m_is_sampled = false;
                VkDescriptorPool m_pool = VK_NULL_HANDLE;
                uint32_t m_pool_capacity;   // value of how many in flight descriptor sets can it hold
                std::vector<VkDescriptorSet> m_descriptor_sets; 
                std::unordered_map<std::string, VkDescriptorSet*> m_texture_bound_desc_sets;

            private:
                void _AllocateDescriptorPool();
                void _CreateDescriptorSets(const std::string &_name, const Vulkan::TextureData &_tex);
                void _UpdateDescriptorSets(const Vulkan::TextureData &_tex, size_t _beg);

            public:
                DescriptorAllocator(VkDevice _dev, VkBuffer _u_buffer, VkDescriptorSetLayout _layout, const std::variant<std::vector<UniformDataLayout>*, std::vector<UniformBufferBlock>*> &_ubo_desc, 
                                    uint32_t _sc_img_c, const Vulkan::TextureData &_missing, uint32_t _pool_cap = 1);
                ~DescriptorAllocator();
                DescriptorAllocator &operator=(const DescriptorAllocator &_da);

                void RequestNewDescriptorPool(uint32_t _cap);
                void RemoveDescriptorSetByTexture(const std::string &_name);

                // pool reallocation must be done separately if required
                VkDescriptorSet RequestDescriptorSetByTexture(const std::string &_name, const Vulkan::TextureData &_tex, uint32_t _current_frame);
                VkDescriptorSet RequestDescriptorSetByFrame(uint32_t _current_frame) {
                    return m_descriptor_sets[_current_frame];
                }

                inline bool IsSampled() {
                    return m_is_sampled;
                }

                inline uint32_t GetPoolCapacity() {
                    return m_pool_capacity;
                }

        };
    }
}

#endif
