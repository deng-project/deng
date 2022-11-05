// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: DescriptorAllocator.h - Vulkan descriptor allocation management class implementation
// author: Karl-Mihkel Ott

#define VULKAN_DESCRIPTOR_ALLOCATOR_CPP
#include "deng/VulkanDescriptorAllocator.h"


namespace DENG {
    namespace Vulkan {
    
        DescriptorAllocator::DescriptorAllocator(
            VkDevice _dev, 
            VkBuffer _ubo_buffer, 
            VkDescriptorSetLayout _layout, 
            const std::variant<std::vector<UniformDataLayout>, std::vector<UniformBufferBlock>> &_ubo_desc, 
            uint32_t _sc_img_c, 
            const Vulkan::TextureData &_2d_missing,
            const Vulkan::TextureData &_3d_missing,
            uint32_t _pool_cap
        ) :
            m_device(_dev),
            m_uniform_buffer(_ubo_buffer),
            m_descriptor_set_layout(_layout),
            m_ubo_desc(_ubo_desc),
            m_swapchain_image_count(_sc_img_c),
            m_2d_missing(_2d_missing),
            m_3d_missing(_3d_missing),
            m_pool_capacity(_pool_cap)
        {
            _AllocateDescriptorPool(m_primary_pool);
            _CreateDescriptorSets(m_primary_pool);
            m_descriptor_set_pool_flags.resize(m_descriptor_sets.size());
            std::fill(m_descriptor_set_pool_flags.begin(), m_descriptor_set_pool_flags.end(), true);

            // update created descriptor sets
            _FindSamplerCountPerDescription();

            std::vector<Vulkan::TextureData> textures;
            std::vector<std::string> names;
            
            if(m_2d_sampler_count || m_3d_sampler_count) {
                textures.resize(m_2d_sampler_count + m_3d_sampler_count);
                names.resize(m_2d_sampler_count + m_3d_sampler_count);
                std::fill(names.begin(), names.begin() + m_2d_sampler_count, MISSING_TEXTURE_NAME);
                std::fill(names.begin() + m_2d_sampler_count, names.end(), MISSING_3D_TEXTURE_NAME);
                std::fill(textures.begin(), textures.begin() + m_2d_sampler_count, m_2d_missing);
                std::fill(textures.begin() + m_2d_sampler_count, textures.end(), m_3d_missing);

                std::string name = "";
                for(size_t i = 0; i != names.size(); i++) {
                    if(!i) name += names[i];
                    else name += "|" + names[i];
                }
                m_texture_bound_desc_sets[name] = 0;
            }

            _UpdateDescriptorSets(names, textures, 0);
        }


        DescriptorAllocator::DescriptorAllocator(DescriptorAllocator &&_da) noexcept :
            m_device(_da.m_device),
            m_uniform_buffer(_da.m_uniform_buffer),
            m_descriptor_set_layout(_da.m_descriptor_set_layout),
            m_ubo_desc(_da.m_ubo_desc),
            m_swapchain_image_count(_da.m_swapchain_image_count),
            m_2d_missing(_da.m_2d_missing),
            m_3d_missing(_da.m_3d_missing),
            m_is_sampled(_da.m_is_sampled),
            m_is_pool_transfer(_da.m_is_pool_transfer),
            m_primary_pool(_da.m_primary_pool),
            m_secondary_pool(_da.m_secondary_pool),
            m_pool_capacity(_da.m_pool_capacity),
            m_descriptor_sets(std::move(_da.m_descriptor_sets)),
            m_descriptor_set_pool_flags(std::move(_da.m_descriptor_set_pool_flags)),
            m_texture_bound_desc_sets(std::move(_da.m_texture_bound_desc_sets)),
            m_2d_sampler_count(_da.m_2d_sampler_count),
            m_3d_sampler_count(_da.m_3d_sampler_count)
        {
            _da.m_primary_pool = VK_NULL_HANDLE;
            _da.m_secondary_pool = VK_NULL_HANDLE;
        }


        DescriptorAllocator::~DescriptorAllocator() {
            if(m_descriptor_sets.size() && m_primary_pool != VK_NULL_HANDLE) {
                vkFreeDescriptorSets(m_device, m_primary_pool, static_cast<uint32_t>(m_descriptor_sets.size()), m_descriptor_sets.data());
                vkDestroyDescriptorPool(m_device, m_primary_pool, nullptr);
            }
        }


        void DescriptorAllocator::_FindSamplerCountPerDescription() {
            switch(m_ubo_desc.index()) {
                case 0:
                {
                    const std::vector<UniformDataLayout> &desc = std::get<std::vector<UniformDataLayout>>(m_ubo_desc);
                    for(auto it = desc.begin(); it != desc.end(); it++) {
                        if(it->type == UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER) {
                            m_2d_sampler_count++;
                        } else if(it->type == UNIFORM_DATA_TYPE_3D_IMAGE_SAMPLER) {
                            m_3d_sampler_count++;
                        }
                    }
                    break;
                }

                default:
                    break;
            }
        }


        void DescriptorAllocator::_AllocateDescriptorPool(VkDescriptorPool &_pool) {
            std::vector<VkDescriptorPoolSize> desc_sizes;

            // check ubo desc index
            switch(m_ubo_desc.index()) {
                // per shader descriptor
                case 0:
                    {
                        const std::vector<UniformDataLayout> &layouts = std::get<std::vector<UniformDataLayout>>(m_ubo_desc);
                        desc_sizes.reserve(layouts.size());

                        for(size_t i = 0; i < layouts.size(); i++) {
                            if(layouts.at(i).usage != UNIFORM_USAGE_PER_MESH) {
                                desc_sizes.emplace_back();
                                switch(layouts.at(i).type) {
                                    case UNIFORM_DATA_TYPE_BUFFER:
                                        desc_sizes.back().type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                                        break;

                                    case UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER:
                                    case UNIFORM_DATA_TYPE_3D_IMAGE_SAMPLER:
                                        desc_sizes.back().type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                                        break;

                                    default:
                                        DENG_ASSERT(false);
                                        break;
                                }

                                desc_sizes.back().descriptorCount = m_pool_capacity * m_swapchain_image_count;
                            }
                        }
                    }
                    break;

                // per mesh descriptor
                case 1:
                    {
                        const std::vector<UniformBufferBlock> &block = std::get<std::vector<UniformBufferBlock>>(m_ubo_desc);
                        desc_sizes.resize(block.size());

                        for(auto it = desc_sizes.begin(); it != desc_sizes.end(); it++) {
                            it->type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;       // per mesh descriptor is always uniform buffer type
                            it->descriptorCount = m_pool_capacity * m_swapchain_image_count;
                        }
                    }
                    break;

                default:
                    DENG_ASSERT(false);
                    break;
            }

            VkDescriptorPoolCreateInfo desc_pool_info = {};
            desc_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            desc_pool_info.poolSizeCount = static_cast<uint32_t>(desc_sizes.size());
            desc_pool_info.pPoolSizes = desc_sizes.data();
            desc_pool_info.maxSets = m_pool_capacity * m_swapchain_image_count;
            desc_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

            if(vkCreateDescriptorPool(m_device, &desc_pool_info, nullptr, &_pool) != VK_SUCCESS)
                VK_DESC_ERR("Failed to create a descriptor pool");
        }


        size_t DescriptorAllocator::_CreateDescriptorSets(VkDescriptorPool &_pool) {
            const size_t rel_offset = m_descriptor_sets.size();
            m_descriptor_sets.resize(m_descriptor_sets.size() + m_swapchain_image_count);

            std::vector<VkDescriptorSetLayout> layouts(m_swapchain_image_count, m_descriptor_set_layout);
            
            VkDescriptorSetAllocateInfo allocation_info = {};
            allocation_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocation_info.descriptorPool = _pool;
            allocation_info.descriptorSetCount = static_cast<uint32_t>(m_swapchain_image_count);
            allocation_info.pSetLayouts = layouts.data();

            if(vkAllocateDescriptorSets(m_device, &allocation_info, m_descriptor_sets.data() + rel_offset) != VK_SUCCESS)
                VK_DESC_ERR("failed to allocate descriptor sets");

            return rel_offset;
        }

        
        void DescriptorAllocator::_UpdateDescriptorSets(const std::vector<std::string> &_names, const std::vector<Vulkan::TextureData> &_textures, size_t _rel_offset) {
            std::vector<VkDescriptorBufferInfo> buffer_infos;
            std::vector<VkDescriptorImageInfo> img_infos;
            std::vector<VkWriteDescriptorSet> write_sets;

            DENG_ASSERT(_names.size() == _textures.size());
            switch(m_ubo_desc.index()) {
                // per shader uniform
                case 0:
                    {
                        const std::vector<UniformDataLayout> &layout = std::get<std::vector<UniformDataLayout>>(m_ubo_desc);
                        size_t texture_index = 0;
                        std::string bound_texture_name = "";

                        buffer_infos.reserve(layout.size());
                        img_infos.reserve(layout.size());
                        write_sets.reserve(m_swapchain_image_count * layout.size());

                        // for each data layout create appropriate write infos
                        for(auto it = layout.begin(); it != layout.end(); it++) {
                            if(it->usage == UNIFORM_USAGE_PER_SHADER) {
                                switch(it->type) {
                                    case UNIFORM_DATA_TYPE_BUFFER:
                                        buffer_infos.emplace_back();
                                        buffer_infos.back().buffer = m_uniform_buffer;
                                        buffer_infos.back().offset = it->block.offset;
                                        buffer_infos.back().range = it->block.size;
                                        break;

                                    case UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER:
                                    case UNIFORM_DATA_TYPE_3D_IMAGE_SAMPLER:
                                        DENG_ASSERT(texture_index < _textures.size());
                                        img_infos.emplace_back();

                                        if(!texture_index)
                                            bound_texture_name += _names[texture_index];
                                        else bound_texture_name += "|" + _names[texture_index];

                                        img_infos.back().sampler = _textures[texture_index].sampler;
                                        img_infos.back().imageView = _textures[texture_index].image_view;
                                        img_infos.back().imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                        texture_index++;
                                        break;

                                    default:
                                        DENG_ASSERT(false);
                                        break;
                                }
                            }
                        }

                        // create write infos
                        for(auto desc_it = m_descriptor_sets.begin() + _rel_offset; desc_it < m_descriptor_sets.end(); desc_it++) {
                            uint32_t used_buffers = 0;
                            uint32_t used_imgs = 0;
                            for(auto ubo_it = layout.begin(); ubo_it != layout.end(); ubo_it++) {
                                if(ubo_it->usage == UNIFORM_USAGE_PER_SHADER) {
                                    write_sets.emplace_back();
                                    write_sets.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                                    write_sets.back().dstSet = *desc_it;
                                    write_sets.back().dstBinding = ubo_it->block.binding;
                                    write_sets.back().dstArrayElement = 0;
                                    write_sets.back().descriptorCount = 1;

                                    switch(ubo_it->type) {
                                        case UNIFORM_DATA_TYPE_BUFFER:
                                            write_sets.back().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                                            write_sets.back().pBufferInfo = &buffer_infos[used_buffers++];
                                            write_sets.back().pImageInfo = nullptr;
                                            break;

                                        case UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER:
                                        case UNIFORM_DATA_TYPE_3D_IMAGE_SAMPLER:
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
                        }
                    }
                    break;

                // per mesh uniform
                case 1:
                    {
                        const std::vector<UniformBufferBlock> &block = std::get<std::vector<UniformBufferBlock>>(m_ubo_desc);
                        buffer_infos.reserve(block.size());

                        for(auto it = block.begin(); it != block.end(); it++) {
                            buffer_infos.emplace_back();
                            buffer_infos.back().buffer = m_uniform_buffer;
                            buffer_infos.back().offset = it->offset;
                            buffer_infos.back().range = it->size;
                        }

                        for(auto desc_it = m_descriptor_sets.begin() + _rel_offset; desc_it < m_descriptor_sets.end(); desc_it++) {
                            uint32_t used_buffers = 0;
                            for(auto ubo_it = block.begin(); ubo_it != block.end(); ubo_it++) {
                                write_sets.emplace_back();
                                write_sets.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                                write_sets.back().dstSet = *desc_it;
                                write_sets.back().dstBinding = ubo_it->binding;
                                write_sets.back().dstArrayElement = 0;
                                write_sets.back().descriptorCount = 1;

                                write_sets.back().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                                write_sets.back().pBufferInfo = &buffer_infos[used_buffers++];
                                write_sets.back().pImageInfo = nullptr;
                            }
                        }
                    }
                    break;
            }

            vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(write_sets.size()), write_sets.data(), 0, nullptr);
        }


        void DescriptorAllocator::CompletePoolTransfer() {
            // check if pool transfer is active
            if(m_is_pool_transfer) {
                vkDeviceWaitIdle(m_device);

                // extract and destroy all primary pool descriptors
                std::vector<VkDescriptorSet> primary_pool_descriptors;
                std::vector<VkDescriptorSet> secondary_pool_descriptors;
                primary_pool_descriptors.reserve(m_descriptor_sets.size());
                secondary_pool_descriptors.reserve(m_descriptor_sets.size());
                for(size_t i = 0; i < m_descriptor_set_pool_flags.size(); i++) {
                    if(m_descriptor_set_pool_flags[i]) {
                        primary_pool_descriptors.push_back(m_descriptor_sets[i]);
                    } else {
                        secondary_pool_descriptors.push_back(m_descriptor_sets[i]);
                    }
                }

                // create a vector to use for sorting
                std::vector<std::pair<std::string, uint32_t>> sorted_texture_bounds;
                sorted_texture_bounds.reserve(m_texture_bound_desc_sets.size());
                for(auto it = m_texture_bound_desc_sets.begin(); it != m_texture_bound_desc_sets.end(); it++) {
                    sorted_texture_bounds.push_back(*it);
                }
                m_texture_bound_desc_sets.clear();
                std::sort(sorted_texture_bounds.begin(), sorted_texture_bounds.end(), [](auto &_p1, auto &_p2){ return _p1.second < _p2.second; });


                // remove all texture bound descriptor sets that were using primary pool
                // texture bound descriptor sets that are using secondary pool are going to be shifted accordingly
                uint32_t shift = 0;
                for(auto it = sorted_texture_bounds.begin(); it != sorted_texture_bounds.end(); it++) {
                    const uint32_t index = it->second;
                    if(m_descriptor_set_pool_flags[index]) {
                        shift += m_swapchain_image_count;
                    } else {
                        it->second -= shift;
                        m_texture_bound_desc_sets[it->first] = it->second;
                    }
                }

                // free all primary descriptor sets
                vkFreeDescriptorSets(m_device, m_primary_pool, static_cast<uint32_t>(primary_pool_descriptors.size()), primary_pool_descriptors.data());
                vkDestroyDescriptorPool(m_device, m_primary_pool, nullptr);
                m_descriptor_set_pool_flags.resize(secondary_pool_descriptors.size());
                std::fill(m_descriptor_set_pool_flags.begin(), m_descriptor_set_pool_flags.end(), true);
                m_descriptor_sets = secondary_pool_descriptors;
                m_primary_pool = m_secondary_pool;
                m_secondary_pool = VK_NULL_HANDLE;
                m_is_pool_transfer = false;
            }
        }


        void DescriptorAllocator::RecreateDescriptorSets(VkBuffer _new_ubo) {
            m_uniform_buffer = _new_ubo;

            // find all descriptor sets that are using primary pool
            std::vector<VkDescriptorSet> primary_pool_sets;
            primary_pool_sets.reserve(m_descriptor_sets.size());
            for(auto it = m_descriptor_set_pool_flags.begin(); it != m_descriptor_set_pool_flags.end(); it++) {
                const size_t index = it - m_descriptor_set_pool_flags.begin();
                if(*it) primary_pool_sets.push_back(m_descriptor_sets[index]);
            }

            // find all descriptor sets that are using secondary pool
            std::vector<VkDescriptorSet> secondary_pool_sets;
            secondary_pool_sets.reserve(m_descriptor_sets.size());
            for(auto it = m_descriptor_set_pool_flags.begin(); it != m_descriptor_set_pool_flags.end(); it++) {
                const size_t index = it - m_descriptor_set_pool_flags.begin();
                if(!*it) secondary_pool_sets.push_back(m_descriptor_sets[index]);
            }

            vkDeviceWaitIdle(m_device);
            std::fill(m_descriptor_set_pool_flags.begin(), m_descriptor_set_pool_flags.end(), true);

            // free primary pool and its descriptor sets
            if(primary_pool_sets.size()) {
                vkFreeDescriptorSets(m_device, m_primary_pool, static_cast<uint32_t>(primary_pool_sets.size()), primary_pool_sets.data());
            } 

            if(secondary_pool_sets.size()) {
                vkFreeDescriptorSets(m_device, m_secondary_pool, static_cast<uint32_t>(secondary_pool_sets.size()), secondary_pool_sets.data());
                if(primary_pool_sets.size())
                    vkDestroyDescriptorPool(m_device, m_primary_pool, nullptr);

                // set secondary pool as primary
                m_primary_pool = m_secondary_pool;
            }

            m_descriptor_sets.clear();
            m_texture_bound_desc_sets.clear();
            _CreateDescriptorSets(m_primary_pool);

            std::vector<std::string> names;
            std::vector<Vulkan::TextureData> textures;
            if(m_2d_sampler_count + m_3d_sampler_count) {
                names.resize(m_2d_sampler_count + m_3d_sampler_count);
                textures.resize(m_2d_sampler_count + m_3d_sampler_count);
                std::fill(names.begin(), names.begin() + m_2d_sampler_count, MISSING_TEXTURE_NAME);
                std::fill(names.begin() + m_2d_sampler_count, names.end(), MISSING_3D_TEXTURE_NAME);
                std::fill(textures.begin(), textures.begin() + m_2d_sampler_count, m_2d_missing);
                std::fill(textures.begin() + m_2d_sampler_count, textures.end(), m_3d_missing);

                std::string name = "";
                for(size_t i = 0; i < names.size(); i++) {
                    if(!i) name += names[i];
                    else name += "|" + names[i];
                }

                m_texture_bound_desc_sets[name] = 0;
            }

            _UpdateDescriptorSets(names, textures, 0);
        }


        VkDescriptorSet DescriptorAllocator::RequestDescriptorSetByTextures(const std::vector<std::string> &_names, const std::vector<Vulkan::TextureData> &_textures, uint32_t _current_frame, uint32_t _reserve_pool_size) {
            DENG_ASSERT(m_2d_sampler_count + m_3d_sampler_count);
            DENG_ASSERT(_names.size() == _textures.size());
            DENG_ASSERT(_names.size() == m_2d_sampler_count + m_3d_sampler_count);

            std::string name = "";

            // create a string key for descriptor sets
            for(uint32_t i = 0; i < _names.size(); i++) {
                if(!i) name += _names[i];
                else name += "|" + _names[i];
            }

            // texture descriptor does not exist in map
            if(m_texture_bound_desc_sets.find(name) == m_texture_bound_desc_sets.end()) {
                const size_t old_size = m_descriptor_set_pool_flags.size();

                // check if descriptor pool reallocation is required
                if(m_texture_bound_desc_sets.size() + _reserve_pool_size + 1 > m_pool_capacity) {
                    m_pool_capacity += static_cast<uint32_t>(m_texture_bound_desc_sets.size()) + _reserve_pool_size + 1;
                    _AllocateDescriptorPool(m_secondary_pool);
                    const size_t rel_offset = _CreateDescriptorSets(m_secondary_pool);
                    m_descriptor_set_pool_flags.resize(old_size + m_swapchain_image_count);
                    std::fill(m_descriptor_set_pool_flags.begin() + old_size, m_descriptor_set_pool_flags.end(), false);
                    _UpdateDescriptorSets(_names, _textures, rel_offset);
                    m_texture_bound_desc_sets[name] = static_cast<uint32_t>(rel_offset);
                    m_is_pool_transfer = true;
                } else if(m_is_pool_transfer) {         // pool transfer is active
                    const size_t rel_offset = _CreateDescriptorSets(m_secondary_pool);
                    _UpdateDescriptorSets(_names, _textures, rel_offset);
                    std::fill(m_descriptor_set_pool_flags.begin() + old_size, m_descriptor_set_pool_flags.end(), false);
                    m_texture_bound_desc_sets[name] = static_cast<uint32_t>(rel_offset);
                } else {                                // pool transfer is not active
                    const size_t rel_offset = _CreateDescriptorSets(m_primary_pool);
                    _UpdateDescriptorSets(_names, _textures, rel_offset);
                    std::fill(m_descriptor_set_pool_flags.begin() + old_size, m_descriptor_set_pool_flags.end(), true);
                    m_texture_bound_desc_sets[name] = static_cast<uint32_t>(rel_offset);
                }
            }

            return m_descriptor_sets[m_texture_bound_desc_sets[name] + _current_frame];
        }
    }
}
