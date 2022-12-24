// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanDescriptorSetLayoutCreator.cpp - Vulkan descriptor set layout creator class implementation
// author: Karl-Mihkel Ott

#define VULKAN_DESCRIPTOR_SET_LAYOUT_CREATOR_CPP
#include "deng/VulkanDescriptorSetLayoutCreator.h"


namespace DENG {

    namespace Vulkan {
        
        DescriptorSetLayoutCreator::DescriptorSetLayoutCreator(VkDevice _dev, const std::vector<UniformDataLayout>& _layouts) : 
            m_device(_dev), 
            m_ubo_layouts(_layouts)
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings;
            bindings.reserve(m_ubo_layouts.size());

            for (auto it = m_ubo_layouts.begin(); it != m_ubo_layouts.end(); it++) {
                bindings.emplace_back();
                bindings.back().binding = it->block.binding;
                bindings.back().descriptorCount = 1;

                switch (it->type) {
                    case UNIFORM_DATA_TYPE_BUFFER:
                        bindings.back().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        break;

                    case UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER:
                    case UNIFORM_DATA_TYPE_3D_IMAGE_SAMPLER:
                        bindings.back().descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        break;

                    default:
                        DENG_ASSERT(false);
                        break;
                }

                bindings.back().pImmutableSamplers = nullptr;
                bindings.back().stageFlags = 0;

                if (it->stage & SHADER_STAGE_VERTEX)
                    bindings.back().stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
                if (it->stage & SHADER_STAGE_GEOMETRY)
                    bindings.back().stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
                if (it->stage & SHADER_STAGE_FRAGMENT)
                    bindings.back().stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
            }

            VkDescriptorSetLayoutCreateInfo dsl_create_info = {};
            dsl_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            dsl_create_info.bindingCount = static_cast<uint32_t>(bindings.size());
            dsl_create_info.pBindings = bindings.data();
            if (vkCreateDescriptorSetLayout(m_device, &dsl_create_info, NULL, &m_descriptor_set_layout) != VK_SUCCESS)
                VK_DESC_ERR("Failed to create a descriptor set layout.");
        }


        DescriptorSetLayoutCreator::DescriptorSetLayoutCreator(DescriptorSetLayoutCreator &&_dslc) noexcept : 
            m_device(_dslc.m_device),
            m_ubo_layouts(std::move(_dslc.m_ubo_layouts)), 
            m_descriptor_set_layout(_dslc.m_descriptor_set_layout)
        {
            _dslc.m_descriptor_set_layout = VK_NULL_HANDLE;
        }


        DescriptorSetLayoutCreator::~DescriptorSetLayoutCreator() {
            if(m_descriptor_set_layout != VK_NULL_HANDLE)
                vkDestroyDescriptorSetLayout(m_device, m_descriptor_set_layout, NULL);
        }
    }
}
