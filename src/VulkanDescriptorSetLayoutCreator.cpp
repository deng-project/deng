// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanDescriptorSetLayoutCreator.cpp - Vulkan descriptor set layout creator class implementation
// author: Karl-Mihkel Ott

#define VULKAN_DESCRIPTOR_SET_LAYOUT_CREATOR_CPP
#include <VulkanDescriptorSetLayoutCreator.h>


namespace DENG {

    namespace Vulkan {
        
        DescriptorSetLayoutCreator::DescriptorSetLayoutCreator(VkDevice _dev, ShaderModule const *_module) : m_device(_dev), m_shader_module(_module) {
            _CreateDescriptorSetLayout();
        }


        DescriptorSetLayoutCreator::DescriptorSetLayoutCreator(DescriptorSetLayoutCreator &&_dslc) : m_device(_dslc.m_device),
            m_shader_module(_dslc.m_shader_module), m_descriptor_set_layout(_dslc.m_descriptor_set_layout)
        {
            _dslc.m_descriptor_set_layout = VK_NULL_HANDLE;
        }


        DescriptorSetLayoutCreator::~DescriptorSetLayoutCreator() {
            if(m_descriptor_set_layout != VK_NULL_HANDLE)
                vkDestroyDescriptorSetLayout(m_device, m_descriptor_set_layout, NULL);
        }


        DescriptorSetLayoutCreator &DescriptorSetLayoutCreator::operator=(const DescriptorSetLayoutCreator &_dslc) {
            m_device = _dslc.m_device;
            m_shader_module = _dslc.m_shader_module;
            m_descriptor_set_layout = _dslc.m_descriptor_set_layout;

            return *this;
        }


        DescriptorSetLayoutCreator &DescriptorSetLayoutCreator::operator=(DescriptorSetLayoutCreator &&_dslc) {
            m_device = _dslc.m_device;
            m_shader_module = _dslc.m_shader_module;
            m_descriptor_set_layout = _dslc.m_descriptor_set_layout;

            _dslc.m_descriptor_set_layout = VK_NULL_HANDLE;
            return *this;
        }


        void DescriptorSetLayoutCreator::_CreateDescriptorSetLayout() {
            std::vector<VkDescriptorSetLayoutBinding> bindings(m_shader_module->ubo_data_layouts.size());

            for(size_t i = 0; i < bindings.size(); i++) {
                bindings[i].binding = m_shader_module->ubo_data_layouts[i].binding;
                bindings[i].descriptorCount = 1;

                switch(m_shader_module->ubo_data_layouts[i].type) {
                    case UNIFORM_DATA_TYPE_BUFFER:
                        bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        break;

                    case UNIFORM_DATA_TYPE_IMAGE_SAMPLER:
                        bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        break;
                }

                bindings[i].pImmutableSamplers = NULL;
                bindings[i].stageFlags = 0;

                if(m_shader_module->ubo_data_layouts[i].stage & SHADER_STAGE_VERTEX)
                    bindings[i].stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
                if(m_shader_module->ubo_data_layouts[i].stage & SHADER_STAGE_GEOMETRY)
                    bindings[i].stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
                if(m_shader_module->ubo_data_layouts[i].stage & SHADER_STAGE_FRAGMENT)
                    bindings[i].stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
            }

            VkDescriptorSetLayoutCreateInfo desc_set_layout_info = {};
            desc_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            desc_set_layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
            desc_set_layout_info.pBindings = bindings.data();

            if(vkCreateDescriptorSetLayout(m_device, &desc_set_layout_info, NULL, &m_descriptor_set_layout) != VK_SUCCESS)
                VK_DESC_ERR("failed to create descriptor set layout");
        }
    }
}
