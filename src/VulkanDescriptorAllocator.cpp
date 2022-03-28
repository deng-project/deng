// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: DescriptorAllocator.h - Vulkan descriptor allocation management class implementation
// author: Karl-Mihkel Ott

#define VULKAN_DESCRIPTOR_ALLOCATOR_CPP
#include <VulkanDescriptorAllocator.h>


namespace DENG {
    namespace Vulkan {
    
        DescriptorAllocator::DescriptorAllocator(VkDevice _dev, VkBuffer _u_buffer, VkDescriptorSetLayout _layout, const std::vector<UniformDataLayout> &_ubo_layouts, 
                                                 uint32_t _sc_img_c, const Vulkan::TextureData &_missing, uint32_t _pool_cap) :
            m_device(_dev),
            m_uniform_buffer(_u_buffer),
            m_descriptor_set_layout(_layout),
            m_ubo_layouts(_ubo_layouts),
            m_swapchain_image_count(_sc_img_c),
            m_missing(_missing),
            m_pool_capacity(_pool_cap)
        {
            _AllocateDescriptorPool();
            _CreateDescriptorSets(MISSING_TEXTURE_NAME, _missing);
        }


        DescriptorAllocator::~DescriptorAllocator() {
            vkFreeDescriptorSets(m_device, m_pool, static_cast<uint32_t>(m_descriptor_sets.size()), m_descriptor_sets.data());
            vkDestroyDescriptorPool(m_device, m_pool, nullptr);
        }


        DescriptorAllocator &DescriptorAllocator::operator=(const DescriptorAllocator &_da) {
            this->m_descriptor_sets = _da.m_descriptor_sets;
            return *this;
        }


        void DescriptorAllocator::_AllocateDescriptorPool() {
            std::vector<VkDescriptorPoolSize> desc_sizes(m_ubo_layouts.size());

            for(size_t i = 0; i < desc_sizes.size(); i++) {
                switch(m_ubo_layouts[i].type) {
                    case UNIFORM_DATA_TYPE_BUFFER:
                        desc_sizes[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        break;

                    case UNIFORM_DATA_TYPE_IMAGE_SAMPLER:
                        desc_sizes[i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        break;

                    default:
                        DENG_ASSERT(false);
                        break;
                }

                desc_sizes[i].descriptorCount = m_pool_capacity * m_swapchain_image_count;
            }

            VkDescriptorPoolCreateInfo desc_pool_info = {};
            desc_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            desc_pool_info.poolSizeCount = static_cast<uint32_t>(desc_sizes.size());
            desc_pool_info.pPoolSizes = desc_sizes.data();
            desc_pool_info.maxSets = m_pool_capacity * m_swapchain_image_count;
            desc_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

            if(vkCreateDescriptorPool(m_device, &desc_pool_info, nullptr, &m_pool) != VK_SUCCESS)
                VK_DESC_ERR("Failed to create a descriptor pool");
        }


        void DescriptorAllocator::_CreateDescriptorSets(const std::string &_name, const Vulkan::TextureData &_tex) {
            const size_t rel_offset = m_descriptor_sets.size();
            m_descriptor_sets.resize(m_descriptor_sets.size() + m_swapchain_image_count);

            std::vector<VkDescriptorSetLayout> layouts(m_swapchain_image_count, m_descriptor_set_layout);
            
            VkDescriptorSetAllocateInfo allocation_info = {};
            allocation_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocation_info.descriptorPool = m_pool;
            allocation_info.descriptorSetCount = static_cast<uint32_t>(m_swapchain_image_count);
            allocation_info.pSetLayouts = layouts.data();

            if(vkAllocateDescriptorSets(m_device, &allocation_info, m_descriptor_sets.data() + rel_offset) != VK_SUCCESS)
                VK_DESC_ERR("failed to allocate descriptor sets");

            // check if textured descriptor set already exists in map
            if(m_texture_bound_desc_sets.find(_name) != m_texture_bound_desc_sets.end()) {
                WARNME("Descriptor for texture with name '" +  _name + "' already exists");
                DENG_ASSERT(false);
            }

            m_texture_bound_desc_sets[_name] = m_descriptor_sets.data() + rel_offset;
            _UpdateDescriptorSets(_tex, rel_offset);
        }

        
        void DescriptorAllocator::_UpdateDescriptorSets(const Vulkan::TextureData &_tex, size_t _beg) {
            std::vector<VkDescriptorBufferInfo> buffer_infos;
            std::vector<VkDescriptorImageInfo> img_infos;
            std::vector<VkWriteDescriptorSet> write_sets;
            buffer_infos.reserve(m_ubo_layouts.size());
            img_infos.reserve(m_ubo_layouts.size());
            write_sets.reserve(m_swapchain_image_count * m_ubo_layouts.size());

            // for each data layout create appropriate write infos
            for(auto it = m_ubo_layouts.begin(); it != m_ubo_layouts.end(); it++) {
                switch(it->type) {
                    case UNIFORM_DATA_TYPE_BUFFER:
                        buffer_infos.emplace_back();
                        buffer_infos.back().buffer = m_uniform_buffer;
                        buffer_infos.back().offset = it->offset;
                        buffer_infos.back().range = it->ubo_size;
                        break;

                    case UNIFORM_DATA_TYPE_IMAGE_SAMPLER:
                        img_infos.emplace_back();
                        img_infos.back().sampler = _tex.sampler;
                        img_infos.back().imageView = _tex.image_view;
                        img_infos.back().imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        break;

                    default:
                        DENG_ASSERT(false);
                        break;
                }
            }

            // create write infos
            for(auto desc_it = m_descriptor_sets.begin() + _beg; desc_it < m_descriptor_sets.end(); desc_it++) {
                uint32_t used_buffers = 0;
                uint32_t used_imgs = 0;
                for(auto ubo_it = m_ubo_layouts.begin(); ubo_it != m_ubo_layouts.end(); ubo_it++) {
                    write_sets.emplace_back();
                    write_sets.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    write_sets.back().dstSet = *desc_it;
                    write_sets.back().dstBinding = ubo_it->binding;
                    write_sets.back().dstArrayElement = 0;
                    write_sets.back().descriptorCount = 1;

                    switch(ubo_it->type) {
                        case UNIFORM_DATA_TYPE_BUFFER:
                            write_sets.back().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                            write_sets.back().pBufferInfo = &buffer_infos[used_buffers++];
                            write_sets.back().pImageInfo = nullptr;
                            break;

                        case UNIFORM_DATA_TYPE_IMAGE_SAMPLER:
                            write_sets.back().descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                            write_sets.back().pImageInfo = &img_infos[used_imgs++];
                            write_sets.back().pBufferInfo = nullptr;
                            break;

                        default:
                            DENG_ASSERT(false);
                            break;
                    }
                }
            }

            vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(write_sets.size()), write_sets.data(), 0, nullptr);
        }


        void DescriptorAllocator::RequestNewDescriptorPool(uint32_t _cap) {
            m_pool_capacity = _cap;

            // free all previous resources
            vkFreeDescriptorSets(m_device, m_pool, static_cast<uint32_t>(m_descriptor_sets.size()), m_descriptor_sets.data());
            vkDestroyDescriptorPool(m_device, m_pool, nullptr);
            m_descriptor_sets.clear();
            m_texture_bound_desc_sets.clear();

            // create new pool and sets
            _AllocateDescriptorPool();
            _CreateDescriptorSets(MISSING_TEXTURE_NAME, m_missing);
        }


        void DescriptorAllocator::RemoveDescriptorSetByTexture(const std::string &_name) {
            if(m_texture_bound_desc_sets.find(_name) == m_texture_bound_desc_sets.end()) {
                WARNME("Descriptor for texture with name '" + _name + "' does not exist");
                DENG_ASSERT(false);
            }

            VkDescriptorSet *sets = m_texture_bound_desc_sets[_name];
            vkFreeDescriptorSets(m_device, m_pool, m_swapchain_image_count, sets);
            m_texture_bound_desc_sets.erase(_name);

            auto f_it = m_descriptor_sets.begin() + (sets - m_descriptor_sets.data());
            auto l_it = m_descriptor_sets.begin() + (sets - m_descriptor_sets.data()) + m_swapchain_image_count;
            m_descriptor_sets.erase(f_it, l_it);

            // perform some pointer arithmetic for the rest of textured descriptors for continuous block of descriptor sets
            for(auto it = m_texture_bound_desc_sets.begin(); it != m_texture_bound_desc_sets.end(); it++) {
                if(it->second > sets)
                    it->second -= m_swapchain_image_count;
            }
        }


        VkDescriptorSet DescriptorAllocator::RequestDescriptorSetByTexture(const std::string &_name, const Vulkan::TextureData &_tex, uint32_t _current_frame) {
            // texture descriptor does not exist in map
            if(m_texture_bound_desc_sets.find(_name) == m_texture_bound_desc_sets.end())
                _CreateDescriptorSets(_name, _tex);

            return m_texture_bound_desc_sets[_name][_current_frame];
        }
    }
}
