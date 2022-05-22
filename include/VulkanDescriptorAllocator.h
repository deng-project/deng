// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: DescriptorAllocator.h - Vulkan descriptor allocation management class header
// author: Karl-Mihkel Ott

#ifndef VULKAN_DESCRIPTOR_ALLOCATOR_H
#define VULKAN_DESCRIPTOR_ALLOCATOR_H

#ifdef VULKAN_DESCRIPTOR_ALLOCATOR_CPP
    #include <string>
    #include <vector>
    #include <algorithm>
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
                VkBuffer m_uniform_buffer;
                const VkDescriptorSetLayout m_descriptor_set_layout;
                const std::variant<std::vector<UniformDataLayout>*, std::vector<UniformBufferBlock>*> m_ubo_desc; // 0 - per shader ubo; 1 - per mesh ubo
                const uint32_t m_swapchain_image_count;
                const Vulkan::TextureData &m_missing;
                bool m_is_sampled = false;
                bool m_is_pool_transfer = false;
                VkDescriptorPool m_primary_pool = VK_NULL_HANDLE;
                VkDescriptorPool m_secondary_pool = VK_NULL_HANDLE;
                uint32_t m_pool_capacity;                               // value of how many in flight descriptor sets can it hold
                std::vector<VkDescriptorSet> m_descriptor_sets; 
                std::vector<bool> m_descriptor_set_pool_flags;          // true represents primary pool and false secondary
                std::unordered_map<std::string, VkDescriptorSet*> m_texture_bound_desc_sets;
                size_t m_sampler_count = 0;

            private:
                void _FindSamplerCountPerDescription();
                void _AllocateDescriptorPool(VkDescriptorPool &_pool);
                size_t _CreateDescriptorSets(VkDescriptorPool &_pool);
                void _UpdateDescriptorSets(const std::vector<std::string> &_names, const std::vector<Vulkan::TextureData> &_textures, size_t _beg);
                void _ReallocateDescriptorPool(); // new pool capacity is m_pool_capacity * m_swapchain_image_count

            public:
                DescriptorAllocator(VkDevice _dev, VkBuffer _u_buffer, VkDescriptorSetLayout _layout, const std::variant<std::vector<UniformDataLayout>*, std::vector<UniformBufferBlock>*> &_ubo_desc, 
                                    uint32_t _sc_img_c, const Vulkan::TextureData &_missing, uint32_t _pool_cap = 1);
                ~DescriptorAllocator();
                DescriptorAllocator &operator=(const DescriptorAllocator &_da);
                void CompletePoolTransfer();

                // whenever new uniform buffer is allocated
                void RecreateDescriptorSets(VkBuffer _new_ubo);
                //void RemoveDescriptorSetByTexture(const std::string &_name);

                // pool reallocation must be done separately if required
                VkDescriptorSet RequestDescriptorSetByTextures(const std::vector<std::string> &_names, const std::vector<Vulkan::TextureData> &_texture_datas, uint32_t _current_frame, uint32_t _reserve_pool_size = 0);
                inline VkDescriptorSet RequestDescriptorSetByFrame(uint32_t _current_frame) {
                    return m_descriptor_sets[_current_frame];
                }

                inline bool IsSampled() {
                    return m_is_sampled;
                }

                inline uint32_t GetPoolCapacity() {
                    return m_pool_capacity;
                }

                inline size_t GetSamplerCount() {
                    return m_sampler_count;
                }

                inline size_t GetBoundTextureCount() {
                    return m_texture_bound_desc_sets.size();
                }
        };
    }
}

#endif
