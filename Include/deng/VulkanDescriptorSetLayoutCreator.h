// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanDescriptorSetLayoutCreator.h - Vulkan descriptor set layout creator class header
// author: Karl-Mihkel Ott


#ifndef VULKAN_DESCRIPTOR_SET_LAYOUT_CREATOR_H
#define VULKAN_DESCRIPTOR_SET_LAYOUT_CREATOR_H


#ifdef VULKAN_DESCRIPTOR_SET_LAYOUT_CREATOR_CPP
    #include <vector>
    #include <string>
    #include <cstring>
    #include <unordered_map>
    #include <array>
    #include <queue>
#ifdef __DEBUG
    #include <iostream>
#endif
    #include <vulkan/vulkan.h>

    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Quaternion.h"

    #include "das/Api.h"
    #include "das/DasStructures.h"
    #include "das/TextureReader.h"

    #include "deng/ErrorDefinitions.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Api.h"
    #include "deng/Window.h"
    #include "deng/Renderer.h"
#endif


namespace DENG {

    namespace Vulkan {

        class DescriptorSetLayoutCreator {
            private:
                VkDevice m_device;
                const std::vector<UniformDataLayout>& m_ubo_layouts;
                VkDescriptorSetLayout m_descriptor_set_layout = VK_NULL_HANDLE;

            public:
                DescriptorSetLayoutCreator(VkDevice _dev, const std::vector<UniformDataLayout>& _layouts);
                DescriptorSetLayoutCreator(const DescriptorSetLayoutCreator &_dslc) = delete;
                DescriptorSetLayoutCreator(DescriptorSetLayoutCreator &&_dslc) noexcept;
                ~DescriptorSetLayoutCreator();

                inline VkDescriptorSetLayout GetDescriptorSetLayout() { return m_descriptor_set_layout; }
        };
    }
}

#endif
