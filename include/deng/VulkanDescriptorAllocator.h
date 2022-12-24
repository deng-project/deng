// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: DescriptorAllocator.h - Vulkan descriptor allocation management class header
// author: Karl-Mihkel Ott

#ifndef VULKAN_DESCRIPTOR_ALLOCATOR_H
#define VULKAN_DESCRIPTOR_ALLOCATOR_H

#ifdef VULKAN_DESCRIPTOR_ALLOCATOR_CPP
    #include <string>
    #include <vector>
    #include <queue>
    #include <algorithm>
    #include <unordered_map>
    #include <variant>
    #include <vulkan/vulkan.h>
    #include "nwin/glad/glad.h"

    #include "deng/Api.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/VulkanHelpers.h"
    #include "deng/Missing.h"
    #include "deng/TextureDatabase.h"
#endif

namespace DENG {
    namespace Vulkan {

        class DescriptorAllocator {
            private:
                const VkDevice m_device;
                VkBuffer m_uniform_buffer;
                const VkDescriptorSetLayout m_descriptor_set_layout;
                const std::vector<UniformDataLayout> m_ubo_layouts;
                uint32_t m_2d_missing;
                uint32_t m_3d_missing;
                VkDescriptorPool m_descriptor_pool = VK_NULL_HANDLE;
                const uint32_t m_pool_capacity;
                std::vector<VkDescriptorSet> m_descriptor_sets; 

            private:
                void _AllocateDescriptorPool();
                void _CreateDescriptorSets();

            public:
                DescriptorAllocator(
                    VkDevice _dev, 
                    VkBuffer _u_buffer, 
                    VkDescriptorSetLayout _layout, 
                    const std::vector<UniformDataLayout> &_ubo_layout, 
                    uint32_t _missing_2d,
                    uint32_t _missing_3d,
                    uint32_t _pool_cap
                );
                DescriptorAllocator(DescriptorAllocator&& _da) noexcept = default;
                DescriptorAllocator(const DescriptorAllocator&) = delete;
                ~DescriptorAllocator();

                // whenever new uniform buffer is allocated
                void RecreateDescriptorSets(VkBuffer _new_ubo, const std::vector<uint32_t>& _textures);
                void UpdateDescriptorSets(const std::vector<uint32_t>& _textures = {});
                
                inline VkDescriptorSet RequestDescriptorSetByFrame(uint32_t _current_frame) {
                    return m_descriptor_sets[_current_frame];
                }
        };
    }
}

#endif
