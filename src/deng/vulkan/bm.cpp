/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: bm.h - Vulkan resource manager parent class implemenatation for handling buffer related operations
/// author: Karl-Mihkel Ott


#define __VK_BM_CPP
#include <deng/vulkan/bm.h>


namespace deng {
    namespace vulkan {

        __vk_BufferManager::__vk_BufferManager (
            VkDevice device,
            VkPhysicalDevice gpu,
            const VkPhysicalDeviceLimits &gpu_limits,
            std::vector<deng_Id> &assets,
            deng::Registry &reg
        ) : __vk_UniformBufferManager(assets, gpu_limits.minUniformBufferOffsetAlignment,
                reg, m_buffer_data), __OffsetFinder(assets, reg),
            m_assets(assets), m_gpu_limits(gpu_limits), m_reg(reg)
        {
            // Allocate initial amount of memory for the renderer
            allocateMainBufferMemory(device, gpu);
        }


        /// Create staging buffers for all asset data between bounds
        void __vk_BufferManager::__stageAssets (
            VkDevice device, 
            VkPhysicalDevice gpu, 
            VkCommandPool cmd_pool,
            VkQueue g_queue,
            const dengMath::vec2<deng_ui32_t> &bounds,
            VkDeviceSize cpy_offset
        ) {
            // Create and allocate memory for staging buffer
            VkMemoryRequirements mem_req = __vk_BufferCreator::makeBuffer(device, gpu, __OffsetFinder::getSectionInfo().asset_cap, 
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT, m_buffer_data.staging_buffer);

            __vk_BufferCreator::allocateMemory(device, gpu, mem_req.size,
                m_buffer_data.staging_buffer_memory,  mem_req.memoryTypeBits, 
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

            // Bind staging buffer with its memory
            vkBindBufferMemory(device, m_buffer_data.staging_buffer, m_buffer_data.staging_buffer_memory, 0);
            
            // For each given asset populate staging buffer with asset data
            for(size_t i = bounds.first; i < bounds.second; i++) {
                // Retrieve base and Vulkan specific asset instances
                RegData &reg_asset = m_reg.retrieve(m_assets[i], DENG_REGISTRY_TYPE_ASSET, NULL);
                RegData &reg_vk_asset = m_reg.retrieve(reg_asset.asset.vk_id, DENG_REGISTRY_TYPE_VK_ASSET, NULL);
                
                // Adjust offsets to staging buffer
                reg_asset.asset.offsets.pos_offset -= cpy_offset;
                reg_asset.asset.offsets.tex_offset -= cpy_offset;
                reg_asset.asset.offsets.nor_offset -= cpy_offset;
                reg_asset.asset.offsets.ind_offset -= cpy_offset;

                // Populate staging buffer memory with vertices data
                __AssetCpy asset_cpy = {};
                asset_cpy.cpyToBuffer(device, reg_asset.asset, m_buffer_data.staging_buffer_memory);

                // Readjust offsets to main buffer
                reg_asset.asset.offsets.pos_offset += cpy_offset;
                reg_asset.asset.offsets.tex_offset += cpy_offset;
                reg_asset.asset.offsets.nor_offset += cpy_offset;
                reg_asset.asset.offsets.ind_offset += cpy_offset;
                
                // Check if new uniform data memory area needs to be mapped
                if(!reg_vk_asset.vk_asset.is_desc)
                    mapUniformBufferArea(device, gpu, cmd_pool, g_queue, reg_vk_asset.vk_asset, __OffsetFinder::getSectionInfo());
            }
        }


        /// Check if buffer reallocation is needed for assets and gui elements and reallocate if needed
        deng_bool_t __vk_BufferManager::reallocCheck (
            VkDevice device, 
            VkPhysicalDevice gpu, 
            VkCommandPool cmd_pool, 
            VkQueue g_queue,
            const std::vector<VkFence> &fences
        ) {
            __OffsetFinder::__calcOffsets(false);
            const deng_bool_t asset_realloc = __OffsetFinder::assetCapCheck();
            const deng_bool_t ui_realloc = __OffsetFinder::uiCapCheck();

            // Check if any reallocation should be done
            if(asset_realloc || ui_realloc) {
                vkWaitForFences(device, static_cast<deng_ui32_t>(fences.size()), fences.data(), VK_TRUE, UINT64_MAX);

                // Destroy previous buffer instance 
                vkFreeMemory(device, m_buffer_data.main_buffer_memory, NULL);
                vkDestroyBuffer(device, m_buffer_data.main_buffer, NULL);
                
                // Allocate new buffer
                allocateMainBufferMemory(device, gpu);
                cpyAssetsToBuffer(device, gpu, cmd_pool, g_queue, true, { 0, static_cast<deng_ui32_t>(m_assets.size()) });
                cpyUIDataToBuffer(device, gpu, cmd_pool, g_queue, true);
            }

            return asset_realloc || ui_realloc;
        }


        /// Allocate the reserved memory
        void __vk_BufferManager::allocateMainBufferMemory(VkDevice device, VkPhysicalDevice gpu) {
            // Create a new buffer instance
            VkMemoryRequirements mem_req = __vk_BufferCreator::makeBuffer(device, gpu, __OffsetFinder::getSectionInfo().asset_cap + __OffsetFinder::getSectionInfo().ui_cap, 
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                m_buffer_data.main_buffer);

            // Allocate memory for the buffer instance
            __vk_BufferCreator::allocateMemory(device, gpu, mem_req.size, m_buffer_data.main_buffer_memory, 
                mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            
            // Bind the memory with buffer
            vkBindBufferMemory(device, m_buffer_data.main_buffer, m_buffer_data.main_buffer_memory, 0);
        }


        /// Copy all asset data between given bounds to buffer
        /// NOTE: The asset capacity in buffer has to be larger than required asset size (use assetSizeCheck() for this)
        void __vk_BufferManager::cpyAssetsToBuffer (
            VkDevice device, 
            VkPhysicalDevice gpu, 
            VkCommandPool cmd_pool, 
            VkQueue g_queue,
            deng_bool_t no_offset_calc,
            const dengMath::vec2<deng_ui32_t> &bounds
        ) {
            VkMemoryRequirements mem_req = {};

            // Find the total required buffer size and set correct offsets if required
            if(!no_offset_calc) {
                __OffsetFinder::getSectionInfo().asset_size = 0;
                for(size_t i = 0; i < m_assets.size(); i++) {
                    RegData &reg_asset = m_reg.retrieve(m_assets[i], DENG_REGISTRY_TYPE_ASSET, NULL);
                    
                    // Find buffer offsets for the asset
                    __findAssetOffsets(reg_asset.asset);
                }
            }

            // Create asset staging buffers
            __stageAssets(device, gpu, cmd_pool, g_queue, dengMath::vec2<deng_ui32_t>{ bounds.first, bounds.second}, 0);

            // Copy data from staging buffer to main buffer
            __vk_BufferCreator::cpyBufferToBuffer(device, cmd_pool, g_queue, 
                m_buffer_data.staging_buffer, m_buffer_data.main_buffer, 
                __OffsetFinder::getSectionInfo().asset_cap, 0, 0);

            // Perform staging buffer cleanup
            vkDestroyBuffer(device, m_buffer_data.staging_buffer, NULL);
            vkFreeMemory(device, m_buffer_data.staging_buffer_memory, NULL);
        }


        /// Copy ImGui vertex and indices data to buffer
        /// NOTE: The UI element capacity has to be larger than required UI element size (use uiCapCheck() for this)
        void __vk_BufferManager::cpyUIDataToBuffer (
            VkDevice device,
            VkPhysicalDevice gpu,
            VkCommandPool cmd_pool,
            VkQueue g_queue,
            deng_bool_t no_offset_calc
        ) {
            // Check if ImGui data is even present
            if(!m_p_imgui_data || !m_p_imgui_data->cmd_data.size()) return;

            // Calculate offsets and required size 
            if(!no_offset_calc) {
                __OffsetFinder::getSectionInfo().ui_size = 0;
                __findGuiEntitiesOffsets(false);
            }

            // Create staging buffer for UI data
            VkMemoryRequirements mem_req = __vk_BufferCreator::makeBuffer(device, gpu, __OffsetFinder::getSectionInfo().ui_size,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT, m_buffer_data.staging_buffer);

            // Allocate memory for staging buffer
            __vk_BufferCreator::allocateMemory(device, gpu, mem_req.size, m_buffer_data.staging_buffer_memory,
                mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

            // Bind the staging buffer with its memory
            vkBindBufferMemory(device, m_buffer_data.staging_buffer, m_buffer_data.staging_buffer_memory, 0);

            // Copy data to staging buffer
            void *data = NULL;
            vkMapMemory(device, m_buffer_data.staging_buffer_memory, 0, __OffsetFinder::getSectionInfo().ui_size, 0, &data);
                VkDeviceSize offset = 0;
                
                // For each command list copy its vertices and indices to the buffer
                for(size_t i = 0; i < m_p_imgui_data->cmd_data.size(); i++) {
                    // Copy all vertices to buffer
                    memcpy(((char*) data) + offset, m_p_imgui_data->cmd_data[i].verts, m_p_imgui_data->cmd_data[i].vert_c * sizeof(ImDrawVert));
                    offset += m_p_imgui_data->cmd_data[i].vert_c * sizeof(ImDrawVert);

                    // Copy all indices to buffer
                    memcpy(((char*) data) + offset, m_p_imgui_data->cmd_data[i].ind, m_p_imgui_data->cmd_data[i].ind_c * sizeof(ImDrawIdx));
                    offset += m_p_imgui_data->cmd_data[i].ind_c * sizeof(ImDrawIdx);
                }

                // Unmap buffer memory area
            vkUnmapMemory(device, m_buffer_data.staging_buffer_memory);

            // Copy staging buffer to the main buffer
            __vk_BufferCreator::cpyBufferToBuffer(device, cmd_pool, g_queue, m_buffer_data.staging_buffer,
                m_buffer_data.main_buffer, __OffsetFinder::getSectionInfo().ui_size, 0, __OffsetFinder::getSectionInfo().asset_cap);

            // Destroy stagin buffer and free its memory
            vkDestroyBuffer(device, m_buffer_data.staging_buffer, NULL);
            vkFreeMemory(device, m_buffer_data.staging_buffer_memory, NULL);
        }


        void __vk_BufferManager::setUIDataPtr(__ImGuiData *p_gui) { 
            m_p_imgui_data = p_gui; 
            __OffsetFinder::setUIData(p_gui);
        }
    }
}
