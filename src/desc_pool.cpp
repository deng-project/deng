/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: desc_pool.cpp - Vulkan descriptor pool creator class implementation
/// author: Karl-Mihkel Ott


#define __VK_DESC_POOL_C_CPP
#include <deng/vulkan/desc_pool.h>


namespace deng {
    namespace vulkan {


        __vk_DescriptorPoolCreator::__vk_DescriptorPoolCreator(std::vector<deng_Id> &assets, Registry &reg, void *udata) :
            m_assets(assets), m_reg(reg), m_udata(udata) {}


        void __vk_DescriptorPoolCreator::__mk2DUnmappedDescPool(VkDevice device) {
            std::array<VkDescriptorPoolSize, 2> desc_pool_sizes;
            VkDescriptorPoolCreateInfo desc_pool_info{};

            // The first uniform buffer is the 2D transformation uniform
            desc_pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            desc_pool_sizes[0].descriptorCount = m_vu2d_cap * __max_frame_c;

            // The second uniform buffer is the asset color uniform data
            desc_pool_sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            desc_pool_sizes[1].descriptorCount = m_vu2d_cap * __max_frame_c;

            // Set the createinfo
            desc_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            desc_pool_info.poolSizeCount = static_cast<deng_ui32_t>(desc_pool_sizes.size());
            desc_pool_info.pPoolSizes = desc_pool_sizes.data();
            desc_pool_info.maxSets = m_vu2d_cap * __max_frame_c;
            
            // Try to create descriptor pool
            SET_UDATA("vkCreateDescriptorPool");
            if(vkCreateDescriptorPool(device, &desc_pool_info, NULL, 
               &m_vu2d_pool) != VK_SUCCESS)
                VK_DESC_ERR("failed to create descriptor pool for 2D unmapped assets");
        }


        void __vk_DescriptorPoolCreator::__mk2DTexMappedDescPool(VkDevice device) {
            m_vu2d_cap = m_vm2d_cap * __max_frame_c;
            std::array<VkDescriptorPoolSize, 3> desc_pool_sizes;
            VkDescriptorPoolCreateInfo desc_pool_info{};

            // The first uniform binding is the 2D transformation uniform
            desc_pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            desc_pool_sizes[0].descriptorCount = m_vm2d_cap * __max_frame_c;

            // The second uniform binding is the asset color uniform data
            desc_pool_sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            desc_pool_sizes[1].descriptorCount = m_vm2d_cap * __max_frame_c;

            // The third uniform binding is texture image sampler
            desc_pool_sizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            desc_pool_sizes[2].descriptorCount = m_vm2d_cap * __max_frame_c;

            // Set the createinfo
            desc_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            desc_pool_info.poolSizeCount = static_cast<deng_ui32_t>(desc_pool_sizes.size());
            desc_pool_info.pPoolSizes = desc_pool_sizes.data();
            desc_pool_info.maxSets = m_vm2d_cap * __max_frame_c;
            
            // Try to create descriptor pool
            SET_UDATA("vkCreateDescriptorPool");
            if(vkCreateDescriptorPool(device, &desc_pool_info, NULL, 
               &m_vm2d_pool) != VK_SUCCESS)
                VK_DESC_ERR("failed to create descriptor pool for 2D texture mapped assets");
        }


        void __vk_DescriptorPoolCreator::__mk3DUnmappedDescPool(VkDevice device) {
            m_vu2d_cap = m_vu3d_cap * __max_frame_c;
            std::array<VkDescriptorPoolSize, 3> desc_pool_sizes;
            VkDescriptorPoolCreateInfo desc_pool_info{};

            // The first uniform binding is the 2D transformation uniform
            desc_pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            desc_pool_sizes[0].descriptorCount = m_vu3d_cap * __max_frame_c;

            // The second uniform binding is the asset color uniform data
            desc_pool_sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            desc_pool_sizes[1].descriptorCount = m_vu3d_cap * __max_frame_c;

            // The third uniform binding is the lighting uniform data
            desc_pool_sizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            desc_pool_sizes[2].descriptorCount = m_vu3d_cap * __max_frame_c;

            // Set the createinfo
            desc_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            desc_pool_info.poolSizeCount = static_cast<deng_ui32_t>(desc_pool_sizes.size());
            desc_pool_info.pPoolSizes = desc_pool_sizes.data();
            desc_pool_info.maxSets = m_vu3d_cap * __max_frame_c;
            
            // Try to create descriptor pool
            SET_UDATA("vkCreateDescriptorPool");
            if(vkCreateDescriptorPool(device, &desc_pool_info, NULL, 
               &m_vu3d_pool) != VK_SUCCESS)
                VK_DESC_ERR("failed to create descriptor pool for unmapped assets");
        }


        void __vk_DescriptorPoolCreator::__mk3DTexMappedDescPool(VkDevice device) {
            m_vu2d_cap = m_vm3d_cap * __max_frame_c;
            std::array<VkDescriptorPoolSize, 4> desc_pool_sizes;
            VkDescriptorPoolCreateInfo desc_pool_info{};

            // The first uniform binding is the 2D transformation uniform
            desc_pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            desc_pool_sizes[0].descriptorCount = m_vm3d_cap * __max_frame_c;

            // The second uniform binding is the asset color uniform data
            desc_pool_sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            desc_pool_sizes[1].descriptorCount = m_vm3d_cap * __max_frame_c;

            // The third uniform binding is texture image sampler
            desc_pool_sizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            desc_pool_sizes[2].descriptorCount = m_vm3d_cap * __max_frame_c;

            // The fourth uniform binding is lighting uniform data
            desc_pool_sizes[3].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            desc_pool_sizes[3].descriptorCount = m_vm3d_cap * __max_frame_c;

            // Set the createinfo
            desc_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            desc_pool_info.poolSizeCount = static_cast<deng_ui32_t>(desc_pool_sizes.size());
            desc_pool_info.pPoolSizes = desc_pool_sizes.data();
            desc_pool_info.maxSets = m_vm3d_cap * __max_frame_c;
            
            // Try to create descriptor pool
            SET_UDATA("vkCreateDescriptorPool");
            if(vkCreateDescriptorPool(device, &desc_pool_info, NULL, 
               &m_vm3d_pool) != VK_SUCCESS)
                VK_DESC_ERR("failed to create descriptor pool for unmapped assets");
        }


        void __vk_DescriptorPoolCreator::__mkUIPool(VkDevice device) {
            m_vu2d_cap = m_vm3d_cap * __max_frame_c;
            VkDescriptorPoolSize desc_pool_size;
            VkDescriptorPoolCreateInfo desc_pool_info{};

            // The first uniform binding is the 2D transformation uniform
            desc_pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            desc_pool_size.descriptorCount = __max_frame_c;

            // Set the createinfo
            desc_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            desc_pool_info.poolSizeCount = 1;
            desc_pool_info.pPoolSizes = &desc_pool_size;
            desc_pool_info.maxSets = __max_frame_c;
            
            // Try to create descriptor pool
            SET_UDATA("vkCreateDescriptorPool");
            if(vkCreateDescriptorPool(device, &desc_pool_info, NULL, 
               &m_ui_pool) != VK_SUCCESS)
                VK_DESC_ERR("failed to create descriptor pool for ImGui elements");
        }


        /// Destroy assets with specific asset mode
        void __vk_DescriptorPoolCreator::__destroyAssetDescriptorSets ( 
            VkDevice device, 
            std::vector<deng_Id> &destroyed, 
            das_AssetMode asset_mode
        ) {
            // Iterate through every asset and check if they need to have 
            // descriptor sets freed
            for(size_t i = 0; i < m_assets.size(); i++) {
                // Retrieve the asset from registry
                RegData &vk_asset = m_reg.retrieve(m_assets[i], DENG_REGISTRY_TYPE_VK_ASSET, NULL);
                RegData &reg_asset = m_reg.retrieve(vk_asset.vk_asset.base_id, DENG_REGISTRY_TYPE_VK_ASSET, NULL);

                // Check if the asset has descriptor sets allocated
                if(vk_asset.vk_asset.is_desc && reg_asset.asset.asset_mode == asset_mode) {
                    // Check the asset mode and free descriptor sets
                    switch(asset_mode) {
                    case DAS_ASSET_MODE_2D_UNMAPPED:
                        SET_UDATA("vkFreeDescriptorSets");
                        vkFreeDescriptorSets(device, m_vu2d_pool, 
                            vk_asset.vk_asset.desc_c, vk_asset.vk_asset.desc_sets);
                        break;

                    case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                        SET_UDATA("vkFreeDescriptorSets");
                        vkFreeDescriptorSets(device, m_vm2d_pool, 
                            vk_asset.vk_asset.desc_c, vk_asset.vk_asset.desc_sets);
                        break;

                    case DAS_ASSET_MODE_3D_UNMAPPED:
                        SET_UDATA("vkFreeDescriptorSets");
                        vkFreeDescriptorSets(device, m_vu2d_pool, 
                            vk_asset.vk_asset.desc_c, vk_asset.vk_asset.desc_sets);
                        break;

                    case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                        SET_UDATA("vkFreeDescriptorSets");
                        vkFreeDescriptorSets(device, m_vu2d_pool, 
                            vk_asset.vk_asset.desc_c, vk_asset.vk_asset.desc_sets);
                        break;
                    
                    default:
                        break;
                    }

                    // Free memory allocated for descriptor set instances
                    free(vk_asset.vk_asset.desc_sets);
                    vk_asset.vk_asset.is_desc = false;
                    vk_asset.vk_asset.desc_c = 0;
                    
                    // Push the current id to destroyed data vector
                    destroyed.push_back(m_assets[i]);
                }
            }
        }


        /// Reallocate specific descriptor pool for specific asset type
        void __vk_DescriptorPoolCreator::__reallocDescPool(
            VkDevice device,
            std::vector<deng_Id> &destroyed_assets,    
            das_AssetMode asset_mode, 
            deng_ui32_t req_c
        ) {
            // Check the asset mode and reallocate descriptor pool accordingly
            switch(asset_mode) {
            case DAS_ASSET_MODE_2D_UNMAPPED:
                m_vu2d_cap = m_vu2d_cap << 1 < cm_ToPow2I32(req_c * 2) ? 
                             cm_ToPow2I32(req_c * 2) : m_vu2d_cap << 1;
                
                // Destroy previous descriptor sets and pools
                __destroyAssetDescriptorSets(device, destroyed_assets, DAS_ASSET_MODE_2D_UNMAPPED);
                SET_UDATA("vkDestroyDescriptorPool");
                vkDestroyDescriptorPool(device, m_vu2d_pool, NULL);
                mkDescPool(device, m_vu2d_cap, DENG_PIPELINE_TYPE_UNMAPPED_2D);
                break;

            case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                m_vm2d_cap = m_vm2d_cap << 1 < cm_ToPow2I32(req_c * 2) ? 
                             cm_ToPow2I32(req_c * 2) : m_vm2d_cap << 1;
                
                // Destroy previous descriptor sets and pools
                __destroyAssetDescriptorSets(device, destroyed_assets, DAS_ASSET_MODE_2D_TEXTURE_MAPPED);
                SET_UDATA("vkDestroyDescriptorPool");
                vkDestroyDescriptorPool(device, m_vm2d_pool, NULL);
                mkDescPool(device, m_vm2d_cap, DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D);
                break;

            case DAS_ASSET_MODE_3D_UNMAPPED:
                m_vu3d_cap = m_vu3d_cap << 1 < cm_ToPow2I32(req_c * 2) ? 
                             cm_ToPow2I32(req_c * 2) : m_vu3d_cap << 1;
                
                // Destroy previous descriptor sets and pools
                __destroyAssetDescriptorSets(device, destroyed_assets, DAS_ASSET_MODE_3D_UNMAPPED);
                SET_UDATA("vkDestroyDescriptorPool");
                vkDestroyDescriptorPool(device, m_vu3d_pool, NULL);
                mkDescPool(device, m_vu3d_cap, DENG_PIPELINE_TYPE_UNMAPPED_3D);
                break;

            case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                m_vm3d_cap = m_vm3d_cap << 1 < cm_ToPow2I32(req_c * 2) ? 
                             cm_ToPow2I32(req_c * 2) : m_vm3d_cap << 1;
                
                // Destroy previous descriptor sets and pools
                __destroyAssetDescriptorSets(device, destroyed_assets, DAS_ASSET_MODE_3D_TEXTURE_MAPPED);
                SET_UDATA("vkDestroyDescriptorPool");
                vkDestroyDescriptorPool(device, m_vm3d_pool, NULL);
                mkDescPool(device, m_vm3d_cap, DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D);
                break;

            default:
                break;
            }
        }


        /// Create a new descriptor pool based on the asset mode
        void __vk_DescriptorPoolCreator::mkDescPool (
            VkDevice device, 
            deng_ui32_t cap, 
            deng_PipelineType mode
        ) {
            // Check for the given asset mode
            switch(mode) {
            case DENG_PIPELINE_TYPE_UNMAPPED_2D:
                m_vu2d_cap = cap;
                __mk2DUnmappedDescPool(device);
                break;

            case DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D:
                m_vm2d_cap = cap;
                __mk2DTexMappedDescPool(device);
                break;

            case DENG_PIPELINE_TYPE_UNMAPPED_3D:
                m_vu3d_cap = cap;
                __mk3DUnmappedDescPool(device);
                break;

            case DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D:
                m_vm3d_cap = cap;
                __mk3DTexMappedDescPool(device);
                break;

            case DENG_PIPELINE_TYPE_UI:
                __mkUIPool(device);
                break;

            default:
                // Throw runtime error, because asset mode given was invalid
                RUN_ERR("__vk_DescriptorPoolCreator::mkDescPool()", 
                    "Invalid given asset mode");
                break;
            }
        }


        /// Create new descriptor pools for all asset types
        void __vk_DescriptorPoolCreator::mkDescPools(VkDevice device, deng_ui32_t cap) {
            mkDescPool(device, cap, DENG_PIPELINE_TYPE_UNMAPPED_2D);
            mkDescPool(device, cap, DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D);
            mkDescPool(device, cap, DENG_PIPELINE_TYPE_UNMAPPED_3D);
            mkDescPool(device, cap, DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D);
            mkDescPool(device, cap, DENG_PIPELINE_TYPE_UI);
        }


        /// Check if the descriptor pools need to be reallocated and reallocate
        /// the pool if needed
        __vk_DescriptorPoolReallocBits __vk_DescriptorPoolCreator::checkPoolReallocation (
            VkDevice device,
            std::vector<deng_Id> &destroyed_assets,
            deng_ui32_t vu2d_c, 
            deng_ui32_t vm2d_c, 
            deng_ui32_t vu3d_c,
            deng_ui32_t vm3d_c
        ) {
            __vk_DescriptorPoolReallocBits res = DENG_VK_DESCRIPTOR_POOL_REALLOC_RESULT_NO_REALLOC;

            // Check if unmapped 2D asset pool needs to be reallocated
            if(vu2d_c >= m_vu2d_cap) {
                __reallocDescPool(device, destroyed_assets, DAS_ASSET_MODE_2D_UNMAPPED,
                    vu2d_c);
                res |= DENG_VK_DESCRIPTOR_POOL_REALLOC_RESULT_REALLOC_VU2D;
            }

            // Check if texture mapped 2D asset pool needs to be reallocated
            if(vu2d_c >= m_vu2d_cap) {
                __reallocDescPool(device, destroyed_assets, DAS_ASSET_MODE_2D_TEXTURE_MAPPED,
                    vm2d_c);
                res |= DENG_VK_DESCRIPTOR_POOL_REALLOC_RESULT_REALLOC_VM2D;
            }

            // Check if unmapped 2D asset pool needs to be reallocated
            if(vu2d_c >= m_vu2d_cap) {
                __reallocDescPool(device, destroyed_assets, DAS_ASSET_MODE_3D_UNMAPPED,
                    vm2d_c);
                res |= DENG_VK_DESCRIPTOR_POOL_REALLOC_RESULT_REALLOC_VU3D;
            }

            // Check if texture mapped 3D asset pool needs to be reallocated
            if(vu2d_c >= m_vu2d_cap) {
                __reallocDescPool(device, destroyed_assets, DAS_ASSET_MODE_3D_UNMAPPED,
                    vm2d_c);
                res |= DENG_VK_DESCRIPTOR_POOL_REALLOC_RESULT_REALLOC_VU3D;
            }

            return res;
        }


        /// Get the descriptor pool according to the given method
        VkDescriptorPool &__vk_DescriptorPoolCreator::getDescPool(deng_PipelineType type) {
            // Check for the asset mode given
            switch(type) {
            case DENG_PIPELINE_TYPE_UNMAPPED_2D: 
                return m_vu2d_pool;

            case DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D: 
                return m_vm2d_pool;

            case DENG_PIPELINE_TYPE_UNMAPPED_3D: 
                return m_vu3d_pool;

            case DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D: 
                return m_vm3d_pool;

            case DENG_PIPELINE_TYPE_UI:
                return m_ui_pool;

            default:
                // Throw runtime error, because asset mode given was invalid
                RUN_ERR("__vk_DescriptorPoolCreator::getDescPool()", 
                    "Invalid given asset mode");
                break;
            }
        }
    }
}
