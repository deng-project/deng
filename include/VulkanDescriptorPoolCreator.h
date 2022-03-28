// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanDescriptorPoolCreater.h - Vulkan descriptor pool creator class header
// author: Karl-Mihkel Ott


#ifndef VULKAN_DESCRIPTOR_POOL_CREATOR_H
#define VULKAN_DESCRIPTOR_POOL_CREATOR_H


#ifdef VULKAN_DESCRIPTOR_POOL_CREATOR_CPP
    #include <vector>
    #include <string>
    #include <vulkan/vulkan.h>

    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>

    //#define DEFAULT_DESCRIPTOR_POOL_CAP     16
#endif


namespace DENG {
    namespace Vulkan {

        class DescriptorPoolCreator {
            private:
                VkDevice m_device;
                uint32_t m_swapchain_image_count;
                VkDescriptorPool m_descriptor_pool = VK_NULL_HANDLE;

            private:
                std::vector<VkDescriptorPoolSize> _FindDescriptorPoolSizes(const std::vector<UniformDataLayout> &_ubo_layouts);
                void _CreateDescriptorPools(const std::vector<UniformDataLayout> &_ubo_layouts);

            public:
                DescriptorPoolCreator() = default;
                DescriptorPoolCreator(const DescriptorPoolCreator &_dpc) = default;
                DescriptorPoolCreator(DescriptorPoolCreator &&_dpc);
                DescriptorPoolCreator(VkDevice _dev, uint32_t _swapchain_image_c, const std::vector<UniformDataLayout> &_ubo_layouts);
                ~DescriptorPoolCreator();

                DescriptorPoolCreator &operator=(const DescriptorPoolCreator &_dpc);
                DescriptorPoolCreator &operator=(DescriptorPoolCreator &&_dpc);

                /// NOTE: recreating descriptor pool frees all associated descriptor sets
                //void RecreateDescriptorPool(const std::vector<VkDescriptorSet> &_desc_sets, uint32_t _index, uint32_t _req_cap, 
                                            //const std::vector<UniformDataLayout> &_ubo_layouts);

                inline VkDescriptorPool GetDescriptorPool() {
                    return m_descriptor_pool;
                }
        };
    }
}

#endif
