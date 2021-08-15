/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: ubm.cpp - Vulkan uniform buffer manager parent class implementation
/// author: Karl-Mihkel Ott

#define __VK_UBM_CPP
#include <deng/vulkan/ubm.h>


namespace deng {
    namespace vulkan {

        __vk_UniformBufferManager::__vk_UniformBufferManager (
            std::vector<deng_Id> &assets,
            const deng_ui64_t min_align, 
            deng::Registry &reg,
            __vk_BufferData &buf_data,
            void *udata
        ) : m_assets(assets), m_min_align(min_align), m_reg(reg), m_buffer_data(buf_data), m_udata(udata) {}


        /// Create a new uniform buffer instance and allocate memory for it.
        /// WARNING: This method call expects that uniform buffer and its memory
        /// have been freed or are not initialised previously
        void __vk_UniformBufferManager::__mkUniformBuffer (
            VkDevice device, 
            VkPhysicalDevice gpu,
            VkCommandPool cmd_pool, 
            VkQueue g_queue,
            deng_ui64_t asset_cap,
            deng::BufferSectionInfo &buf_sec
        ) {
            buf_sec.ubo_asset_cap = asset_cap;

            // Calculate the chunk size for uniform data
            m_global_ubo_chunk_size = cm_FindChunkSize(m_min_align, sizeof(__UniformObjectTransform)) + 
                                      cm_FindChunkSize(m_min_align, sizeof(__UniformObjectTransform2D)) +
                                      cm_FindChunkSize(m_min_align, sizeof(__UniformLightData));

            buf_sec.ubo_non_asset_size = __max_frame_c * m_global_ubo_chunk_size;
            
            // Calculate the initial asset uniform data size and overall asset data capacity
            buf_sec.ubo_asset_size = __max_frame_c * (asset_cap * cm_FindChunkSize(m_min_align, std::max(sizeof(__UniformAssetData), sizeof(__UniformAssetData2D))));
            buf_sec.ubo_cap = buf_sec.ubo_non_asset_size + buf_sec.ubo_asset_size;
            
            // Create a new uniform buffer instance
            VkMemoryRequirements mem_req = __vk_BufferCreator::makeBuffer(device, gpu, 
                buf_sec.ubo_cap, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                m_buffer_data.uniform_buffer, m_udata);

            // Allocate memory for uniform buffer
            __vk_BufferCreator::allocateMemory(device, gpu, mem_req.size,
                m_buffer_data.uniform_buffer_mem, mem_req.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_udata);

            // Bind the newly created buffer with newly allocated buffer memory
            SET_UDATA("vkBindBufferMemory");
            vkBindBufferMemory(device, m_buffer_data.uniform_buffer, m_buffer_data.uniform_buffer_mem, 0);
        }


        /// Reallocate memory for uniform buffer and copy previous data to
        /// newly allocated buffer instance
        void __vk_UniformBufferManager::__reallocUniformBufferMemory (
            VkDevice device,
            VkPhysicalDevice gpu,
            VkCommandPool cmd_pool,
            VkQueue g_queue,
            deng_ui64_t req_cap,
            deng::BufferSectionInfo &buf_sec
        ) {
            // Create a new staging buffer for the previous uniform data
            VkMemoryRequirements mem_req = __vk_BufferCreator::makeBuffer(device, gpu,
                buf_sec.ubo_cap, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                m_buffer_data.staging_buffer, m_udata);
            
            // Allocate memory for the staging buffer
            __vk_BufferCreator::allocateMemory(device, gpu,
                mem_req.size, m_buffer_data.staging_buffer_memory, 
                mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | 
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, m_udata);

            // Bind the staging buffer with its memory
            vkBindBufferMemory(device, m_buffer_data.staging_buffer, m_buffer_data.staging_buffer_memory, 0);

            // Copy from uniform buffer to staging buffer
            __vk_BufferCreator::cpyBufferToBuffer(device, cmd_pool, g_queue, m_buffer_data.uniform_buffer, 
                m_buffer_data.staging_buffer, buf_sec.ubo_cap, 0, 0, m_udata);

            // Free all resources used in uniform buffers
            SET_UDATA("vkDestroyBuffer");
            vkDestroyBuffer(device, m_buffer_data.uniform_buffer, NULL);
            SET_UDATA("vkFreeMemory");
            vkFreeMemory(device, m_buffer_data.uniform_buffer_mem, NULL);

            // Allocate memory for new uniform buffer instance
            __mkUniformBuffer(device, gpu, cmd_pool, g_queue, req_cap, buf_sec);

            // Copy from staging buffer to the newly created uniform buffer instance
            __vk_BufferCreator::cpyBufferToBuffer(device, cmd_pool, g_queue, m_buffer_data.staging_buffer, 
                m_buffer_data.uniform_buffer, buf_sec.ubo_cap, 0, 0, m_udata);
        }


        /// Reset uniform buffer size to first asset color data instance
        void __vk_UniformBufferManager::__resetUboBufferSize(deng::BufferSectionInfo &buf_sec) {
            buf_sec.ubo_asset_size = __max_frame_c * m_global_ubo_chunk_size;
        }


        /// Copy asset uniform data to uniform buffer memory
        void __vk_UniformBufferManager::mapUniformBufferArea (
            VkDevice device,
            VkPhysicalDevice gpu,
            VkCommandPool cmd_pool,
            VkQueue g_queue,
            __vk_Asset &asset,
            deng::BufferSectionInfo &buf_sec
        ) {
            // Retrieve the base asset and set its ubo offset
            RegData &reg_asset = m_reg.retrieve(asset.base_id, 
                DENG_REGISTRY_TYPE_ASSET, NULL);
            reg_asset.asset.offsets.ubo_offset = buf_sec.ubo_asset_size;

            // Increase the used size by required margin 
            if(reg_asset.asset.asset_mode == DAS_ASSET_MODE_2D_UNMAPPED || 
               reg_asset.asset.asset_mode == DAS_ASSET_MODE_2D_TEXTURE_MAPPED) {
                buf_sec.ubo_asset_size += __max_frame_c * cm_FindChunkSize(m_min_align,
                    sizeof(__UniformAssetData2D));
            }

            else {
                buf_sec.ubo_asset_size += __max_frame_c * cm_FindChunkSize(m_min_align,
                    sizeof(__UniformAssetData));
            }

            // Check if buffer reallocation is needed 
            if(buf_sec.ubo_asset_size > buf_sec.ubo_asset_cap) {
                // Capcity is either 1.5x the current capacity or 1.5x the required capacity
                const deng_ui64_t req_size = std::max(buf_sec.ubo_asset_cap * 3 / 2, buf_sec.ubo_asset_size * 3 / 2) + buf_sec.ubo_non_asset_size;
                __reallocUniformBufferMemory(device, gpu, cmd_pool, g_queue, req_size, buf_sec);
            }

            // Copy the data to reserved memory area in the buffer
            for(deng_ui32_t i = 0; i < __max_frame_c; i++)
                updateAssetUboData(device, i, asset);
        }


        /// Update transfrom uniform data in the frame according to updated camera
        /// and view matrix
        void __vk_UniformBufferManager::updateUboTransform3D(
            VkDevice device, 
            deng_ui32_t current_image, 
            Camera3D *p_cam
        ) {
            __UniformObjectTransform ubo;
            ubo.transform = p_cam->getCameraMat();
            ubo.view = p_cam->getViewMat();

            __vk_BufferCreator::cpyToBufferMem(device, sizeof(__UniformObjectTransform),
                &ubo, m_buffer_data.uniform_buffer_mem, current_image * m_global_ubo_chunk_size, m_udata);
        }


        /// Update asset uniform buffer data
        void __vk_UniformBufferManager::updateAssetUboData (
            VkDevice device,
            deng_ui32_t current_image, 
            __vk_Asset &asset
        ) {
            // Retrieve base asset from the registry
            RegData &reg_asset = m_reg.retrieve(asset.base_id, 
                DENG_REGISTRY_TYPE_ASSET, NULL);

            // Check the asset type and copy appopriate data structure to buffer accordingly
            if(reg_asset.asset.asset_mode == DAS_ASSET_MODE_2D_UNMAPPED || 
               reg_asset.asset.asset_mode == DAS_ASSET_MODE_2D_TEXTURE_MAPPED) {
                __UniformAssetData2D ubo = {};

                // Set all properties accordingly
                ubo.color = reg_asset.asset.diffuse;
                ubo.ignore_transform = reg_asset.asset.ignore_transform;
                ubo.is_unmapped = reg_asset.asset.force_unmap;

                // Copy the constructed uniform data structure to uniform buffer
                __vk_BufferCreator::cpyToBufferMem(device, sizeof(__UniformAssetData2D), 
                    &ubo, m_buffer_data.uniform_buffer_mem, reg_asset.asset.offsets.ubo_offset + 
                    current_image * cm_FindChunkSize(m_min_align, sizeof(__UniformAssetData2D)), m_udata);
            }

            else {
                __UniformAssetData ubo = {};

                // Set all color / material properties
                ubo.ambient = reg_asset.asset.ambient;
                ubo.diffuse = reg_asset.asset.diffuse;
                ubo.specular = reg_asset.asset.specular;
                ubo.phong_exp = reg_asset.asset.phong_exp;

                // Set additional asset properties
                ubo.ignore_transform = static_cast<deng_ui32_t>(reg_asset.asset.ignore_transform);
                ubo.is_unmapped = static_cast<deng_ui32_t>(reg_asset.asset.force_unmap);

                // Copy the constructed uniform buffer data to uniform buffer
                __vk_BufferCreator::cpyToBufferMem(device, sizeof(__UniformAssetData), 
                    &ubo, m_buffer_data.uniform_buffer_mem, reg_asset.asset.offsets.ubo_offset + 
                    current_image * cm_FindChunkSize(m_min_align, sizeof(__UniformAssetData)), m_udata);
            }
        }


        /// Update lighting uniform data
        void __vk_UniformBufferManager::updateUboLighting (
            VkDevice device,
            std::array<deng_Id, __DENG_MAX_LIGHT_SRC_COUNT> &light_srcs,
            deng_ui32_t current_image
        ) {
            __UniformLightData ubo = { { { { 0 } } } };

            // For each possible light source copy the data to ubo structure
            for(deng_ui64_t i = 0; i < light_srcs.size(); i++) {
                // Quit if no light sources were found
                if(!light_srcs[i]) break;

                deng_RegistryType type = {};
                // Retrieve the current light source
                RegData &reg_light = m_reg.retrieve(light_srcs[i],
                    DENG_REGISTRY_TYPE_PT_LIGHT | DENG_REGISTRY_TYPE_SUN_LIGHT |
                    DENG_REGISTRY_TYPE_DIR_LIGHT, &type);

                // Check the current light source type and copy its data to ubo struct
                switch(type) {
                case DENG_REGISTRY_TYPE_PT_LIGHT:
                    ubo.light_srcs[i].intensity = reg_light.pt_light.intensity;
                    ubo.light_srcs[i].pos = reg_light.pt_light.pos;
                    ubo.light_src_c++;
                    break;

                default:
                    goto CPY;
                }
            }

            CPY:
            __vk_BufferCreator::cpyToBufferMem(device, sizeof(__UniformLightData),
                &ubo, m_buffer_data.uniform_buffer_mem, current_image * m_global_ubo_chunk_size +
                cm_FindChunkSize(m_min_align, sizeof(__UniformObjectTransform)) + 
                cm_FindChunkSize(m_min_align, sizeof(__UniformObjectTransform2D)), m_udata);

        }


        /// Initialise uniform buffer for storing data
        void __vk_UniformBufferManager::initUniformBuffer (
            VkDevice device, 
            VkPhysicalDevice gpu,
            VkCommandPool cmd_pool, 
            VkQueue g_queue,
            deng::BufferSectionInfo &buf_sec
        ) {
            __mkUniformBuffer(device, gpu, cmd_pool, g_queue, DENG_DEFAULT_ASSET_CAP, buf_sec);
            __resetUboBufferSize(buf_sec);
        }

        
        /// Getter methods
        deng_ui64_t __vk_UniformBufferManager::getUboChunkSize() { return m_global_ubo_chunk_size; }
    }
}
