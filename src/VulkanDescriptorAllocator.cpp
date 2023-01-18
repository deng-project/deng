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
            const std::vector<UniformDataLayout>& _ubo_layouts, 
            uint32_t _missing_2d,
            uint32_t _missing_3d,
            uint32_t _pool_cap
        ) :
            m_device(_dev),
            m_uniform_buffer(_ubo_buffer),
            m_descriptor_set_layout(_layout),
            m_ubo_layouts(_ubo_layouts),
            m_2d_missing(_missing_2d),
            m_3d_missing(_missing_3d),
            m_pool_capacity(_pool_cap)
        {
            _AllocateDescriptorPool();
            _CreateDescriptorSets();
            UpdateDescriptorSets();
        }


        DescriptorAllocator::~DescriptorAllocator() {
            if(m_descriptor_sets.size() && m_descriptor_pool != VK_NULL_HANDLE) {
                vkFreeDescriptorSets(m_device, m_descriptor_pool, static_cast<uint32_t>(m_descriptor_sets.size()), m_descriptor_sets.data());
                vkDestroyDescriptorPool(m_device, m_descriptor_pool, nullptr);
            }
        }


        void DescriptorAllocator::_AllocateDescriptorPool() {
            std::vector<VkDescriptorPoolSize> desc_sizes;
            desc_sizes.resize(m_ubo_layouts.size());

            for (size_t i = 0; i < desc_sizes.size(); i++) {
                switch (m_ubo_layouts[i].type) {
                    case UNIFORM_DATA_TYPE_BUFFER:
                        desc_sizes[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        break;

                    case UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER:
                    case UNIFORM_DATA_TYPE_3D_IMAGE_SAMPLER:
                        desc_sizes[i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        break;

                    default:
                        break;
                }

                desc_sizes[i].descriptorCount = m_pool_capacity;
            }

            
            VkDescriptorPoolCreateInfo desc_pool_info = {};
            desc_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            desc_pool_info.poolSizeCount = static_cast<uint32_t>(desc_sizes.size());
            desc_pool_info.pPoolSizes = desc_sizes.data();
            desc_pool_info.maxSets = m_pool_capacity;
            desc_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            
            if(vkCreateDescriptorPool(m_device, &desc_pool_info, nullptr, &m_descriptor_pool) != VK_SUCCESS)
                VK_DESC_ERR("Failed to create a descriptor pool");
        }


        void DescriptorAllocator::_CreateDescriptorSets() {
            m_descriptor_sets.resize(m_pool_capacity);

            std::vector<VkDescriptorSetLayout> layouts(m_pool_capacity, m_descriptor_set_layout);
            
            VkDescriptorSetAllocateInfo allocation_info = {};
            allocation_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocation_info.descriptorPool = m_descriptor_pool;
            allocation_info.descriptorSetCount = static_cast<uint32_t>(m_pool_capacity);
            allocation_info.pSetLayouts = layouts.data();

            if(vkAllocateDescriptorSets(m_device, &allocation_info, m_descriptor_sets.data()) != VK_SUCCESS)
                VK_DESC_ERR("failed to allocate descriptor sets");
        }

        
        void DescriptorAllocator::UpdateDescriptorSets(const std::vector<uint32_t>& _textures) {
            std::vector<VkDescriptorBufferInfo> buffer_infos;
            std::vector<VkDescriptorImageInfo> img_infos;
            std::vector<VkWriteDescriptorSet> write_sets;

            buffer_infos.reserve(m_ubo_layouts.size());
            img_infos.reserve(m_ubo_layouts.size());
            write_sets.reserve(m_pool_capacity * m_ubo_layouts.size());

            // for each data layout create appropriate write infos
            TextureDatabase* db = TextureDatabase::GetInstance();
            Vulkan::TextureData missing_2d_data = std::get<Vulkan::TextureData>(db->GetResource(m_2d_missing).vendor);
            Vulkan::TextureData missing_3d_data = std::get<Vulkan::TextureData>(db->GetResource(m_3d_missing).vendor);
            uint32_t texture_count = 0;

            for (size_t i = 0; i < m_ubo_layouts.size(); i++) {
                switch (m_ubo_layouts[i].type) {
                    case UNIFORM_DATA_TYPE_BUFFER:
                        buffer_infos.emplace_back();
                        buffer_infos.back().buffer = m_uniform_buffer;
                        buffer_infos.back().offset = m_ubo_layouts[i].block.offset;
                        buffer_infos.back().range = m_ubo_layouts[i].block.size;
                        break;
                    
                    case UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER:
                    {
                        img_infos.emplace_back();
                        img_infos.back().imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        if (texture_count >= _textures.size() || _textures[texture_count] == UINT32_MAX || 
                            (db->GetResource(_textures[texture_count]).resource_type != TEXTURE_RESOURCE_2D_IMAGE &&
                            db->GetResource(_textures[texture_count]).resource_type != TEXTURE_RESOURCE_INTERNAL_FRAMEBUFFER_2D_IMAGE)) {
                            img_infos.back().sampler = missing_2d_data.sampler;
                            img_infos.back().imageView = missing_2d_data.image_view;
                        }
                        else {
                            Vulkan::TextureData data = std::get<Vulkan::TextureData>(db->GetResource(_textures[texture_count]).vendor);
                            img_infos.back().sampler = data.sampler;
                            img_infos.back().imageView = data.image_view;
                        }
                        texture_count++;
                        break;
                    }
                    case UNIFORM_DATA_TYPE_3D_IMAGE_SAMPLER:
                    {
                        img_infos.emplace_back();
                        img_infos.back().imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        if (texture_count >= _textures.size() || _textures[texture_count] == UINT32_MAX ||
                            (db->GetResource(_textures[texture_count]).resource_type != TEXTURE_RESOURCE_3D_IMAGE &&
                            db->GetResource(_textures[texture_count]).resource_type != TEXTURE_RESOURCE_INTERNAL_FRAMEBUFFER_3D_IMAGE)) {
                            img_infos.back().sampler = missing_3d_data.sampler;
                            img_infos.back().imageView = missing_3d_data.image_view;
                        } else {
                            Vulkan::TextureData data = std::get<Vulkan::TextureData>(db->GetResource(_textures[texture_count]).vendor);
                            img_infos.back().sampler = data.sampler;
                            img_infos.back().imageView = data.image_view;
                        }
                        break;
                    }

                    default:
                        DENG_ASSERT(false);
                        break;
                }
            }

            // create write infos
            for(auto desc_it = m_descriptor_sets.begin(); desc_it < m_descriptor_sets.end(); desc_it++) {
                uint32_t used_buffers = 0;
                uint32_t used_imgs = 0;
                for(auto ubo_it = m_ubo_layouts.begin(); ubo_it != m_ubo_layouts.end(); ubo_it++) {
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

            vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(write_sets.size()), write_sets.data(), 0, nullptr);
        }


        void DescriptorAllocator::RecreateDescriptorSets(VkBuffer _new_ubo, const std::vector<uint32_t>& _textures) {
            m_uniform_buffer = _new_ubo;

            vkDeviceWaitIdle(m_device);
            vkFreeDescriptorSets(m_device, m_descriptor_pool, static_cast<uint32_t>(m_descriptor_sets.size()), m_descriptor_sets.data());
            _CreateDescriptorSets();
            UpdateDescriptorSets(_textures);
        }
    }
}
