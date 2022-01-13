// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanDescriptorSetLayoutCreator.cpp - Vulkan descriptor set layout creator class implementation
// author: Karl-Mihkel Ott

#define VULKAN_DESCRIPTOR_SET_LAYOUT_CREATOR_CPP
#include <VulkanDescriptorSetLayoutCreator.h>


namespace DENG {

    namespace Vulkan {
        
        DescriptorSetLayoutCreator::DescriptorSetLayoutCreator(VkDevice _dev, const std::vector<ShaderModule*> &_modules) : m_device(_dev) {
            for(size_t i = 0; i < _modules.size(); i++)
                _CreateDescriptorSetLayout(_modules[i]->ubo_data_layouts);
        }


        DescriptorSetLayoutCreator::~DescriptorSetLayoutCreator() {
            for(VkDescriptorSetLayout &layout : m_descriptor_set_layouts)
                vkDestroyDescriptorSetLayout(m_device, layout, NULL);
        }


        void DescriptorSetLayoutCreator::_CreateDescriptorSetLayout(const std::vector<UniformDataLayout> &_layouts) {
            std::vector<VkDescriptorSetLayoutBinding> bindings(_layouts.size());

            for(size_t i = 0; i < bindings.size(); i++) {
                bindings[i].binding = _layouts[i].binding;
                bindings[i].descriptorCount = 1;

                switch(_layouts[i].type) {
                    case UNIFORM_DATA_TYPE_BUFFER:
                        bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        break;

                    case UNIFORM_DATA_TYPE_IMAGE_SAMPLER:
                        bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        break;
                }

                bindings[i].pImmutableSamplers = NULL;
                bindings[i].stageFlags = 0;

                if(_layouts[i].stage & SHADER_STAGE_VERTEX)
                    bindings[i].stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
                if(_layouts[i].stage & SHADER_STAGE_GEOMETRY)
                    bindings[i].stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
                if(_layouts[i].stage & SHADER_STAGE_FRAGMENT)
                    bindings[i].stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
            }

            VkDescriptorSetLayoutCreateInfo desc_set_layout_info = {};
            desc_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            desc_set_layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
            desc_set_layout_info.pBindings = bindings.data();

            VkDescriptorSetLayout desc_set_layout;
            if(vkCreateDescriptorSetLayout(m_device, &desc_set_layout_info, NULL, &desc_set_layout) != VK_SUCCESS)
                VK_DESC_ERR("failed to create descriptor set layout");

            m_descriptor_set_layouts.push_back(desc_set_layout);
        }
    }
}
