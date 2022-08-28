// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanDescriptorSetLayoutCreator.cpp - Vulkan descriptor set layout creator class implementation
// author: Karl-Mihkel Ott

#define VULKAN_DESCRIPTOR_SET_LAYOUT_CREATOR_CPP
#include "deng/VulkanDescriptorSetLayoutCreator.h"


namespace DENG {

    namespace Vulkan {
        
        DescriptorSetLayoutCreator::DescriptorSetLayoutCreator(VkDevice _dev, const ShaderModule &_module) : 
            m_device(_dev), 
            m_shader_module(&_module)
        {
            _CreatePerShaderDescriptorSetLayout();
            _CreatePerMeshDescriptorSetLayout();
        }


        DescriptorSetLayoutCreator::DescriptorSetLayoutCreator(DescriptorSetLayoutCreator &&_dslc) noexcept : 
            m_device(_dslc.m_device),
            m_shader_module(_dslc.m_shader_module), 
            m_per_shader_descriptor_set_layout(_dslc.m_per_shader_descriptor_set_layout),
            m_per_mesh_descriptor_set_layout(_dslc.m_per_mesh_descriptor_set_layout)
        {
            _dslc.m_per_shader_descriptor_set_layout = VK_NULL_HANDLE;
            _dslc.m_per_mesh_descriptor_set_layout = VK_NULL_HANDLE;
        }


        DescriptorSetLayoutCreator::~DescriptorSetLayoutCreator() {
            if(m_per_shader_descriptor_set_layout != VK_NULL_HANDLE)
                vkDestroyDescriptorSetLayout(m_device, m_per_shader_descriptor_set_layout, NULL);
            if(m_per_mesh_descriptor_set_layout != VK_NULL_HANDLE)
                vkDestroyDescriptorSetLayout(m_device, m_per_mesh_descriptor_set_layout, NULL);
        }


        DescriptorSetLayoutCreator &DescriptorSetLayoutCreator::operator=(const DescriptorSetLayoutCreator &_dslc) noexcept {
            m_device = _dslc.m_device;
            m_shader_module = _dslc.m_shader_module;
            m_per_shader_descriptor_set_layout = _dslc.m_per_shader_descriptor_set_layout;
            m_per_mesh_descriptor_set_layout = _dslc.m_per_mesh_descriptor_set_layout;

            return *this;
        }


        DescriptorSetLayoutCreator &DescriptorSetLayoutCreator::operator=(DescriptorSetLayoutCreator &&_dslc) noexcept {
            m_device = _dslc.m_device;
            m_shader_module = _dslc.m_shader_module;
            m_per_shader_descriptor_set_layout = _dslc.m_per_shader_descriptor_set_layout;
            m_per_mesh_descriptor_set_layout = _dslc.m_per_mesh_descriptor_set_layout;

            _dslc.m_per_shader_descriptor_set_layout = VK_NULL_HANDLE;
            _dslc.m_per_mesh_descriptor_set_layout = VK_NULL_HANDLE;
            return *this;
        }


        void DescriptorSetLayoutCreator::_CreatePerShaderDescriptorSetLayout() {
            std::vector<VkDescriptorSetLayoutBinding> bindings;
            bindings.reserve(m_shader_module->ubo_data_layouts.size());

            // for each shader ubo layout
            for(auto it = m_shader_module->ubo_data_layouts.begin(); it != m_shader_module->ubo_data_layouts.end(); it++) {
                if(it->usage == UNIFORM_USAGE_PER_SHADER) {
                    bindings.emplace_back();
                    bindings.back().binding = it->block.binding;
                    bindings.back().descriptorCount = 1;

                    switch(it->type) {
                        case UNIFORM_DATA_TYPE_BUFFER:
                            bindings.back().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                            break;

                        case UNIFORM_DATA_TYPE_IMAGE_SAMPLER:
                            bindings.back().descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                            break;

                        default:
                            DENG_ASSERT(false);
                            break;
                    }

                    bindings.back().pImmutableSamplers = nullptr;
                    bindings.back().stageFlags = 0;

                    if(it->stage & SHADER_STAGE_VERTEX)
                        bindings.back().stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
                    if(it->stage & SHADER_STAGE_GEOMETRY)
                        bindings.back().stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
                    if(it->stage & SHADER_STAGE_FRAGMENT)
                        bindings.back().stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
                }
            }

            if(bindings.size()) {
                VkDescriptorSetLayoutCreateInfo desc_set_layout_info = {};
                desc_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                desc_set_layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
                desc_set_layout_info.pBindings = bindings.data();
                if(vkCreateDescriptorSetLayout(m_device, &desc_set_layout_info, NULL, &m_per_shader_descriptor_set_layout) != VK_SUCCESS)
                    VK_DESC_ERR("failed to create descriptor set layout");
            }
        }

        void DescriptorSetLayoutCreator::_CreatePerMeshDescriptorSetLayout() {
            std::vector<VkDescriptorSetLayoutBinding> bindings;
            bindings.reserve(m_shader_module->ubo_data_layouts.size());

            // for each mesh ubo layout
            for(auto it = m_shader_module->ubo_data_layouts.begin(); it != m_shader_module->ubo_data_layouts.end(); it++) {
                if(it->usage == UNIFORM_USAGE_PER_MESH) {
                    bindings.emplace_back();
                    bindings.back().binding = it->block.binding;
                    bindings.back().descriptorCount = 1;

                    switch(it->type) {
                        case UNIFORM_DATA_TYPE_BUFFER:
                            bindings.back().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                            break;

                        case UNIFORM_DATA_TYPE_IMAGE_SAMPLER:
                            bindings.back().descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                            break;

                        default:
                            DENG_ASSERT(false);
                            break;
                    }

                    bindings.back().pImmutableSamplers = nullptr;
                    bindings.back().stageFlags = 0;

                    if(it->stage & SHADER_STAGE_VERTEX)
                        bindings.back().stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
                    if(it->stage & SHADER_STAGE_GEOMETRY)
                        bindings.back().stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
                    if(it->stage & SHADER_STAGE_FRAGMENT)
                        bindings.back().stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
                }
            }

            if(bindings.size()) {
                VkDescriptorSetLayoutCreateInfo desc_set_layout_info = {};
                desc_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                desc_set_layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
                desc_set_layout_info.pBindings = bindings.data();
                if(vkCreateDescriptorSetLayout(m_device, &desc_set_layout_info, NULL, &m_per_mesh_descriptor_set_layout) != VK_SUCCESS)
                    VK_DESC_ERR("failed to create descriptor set layout");
            }
        }
    }
}
