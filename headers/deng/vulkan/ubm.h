/// DENG: dynamic engine - powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: ubm.h - Vulkan uniform buffer manager parent class header
/// author: Karl-Mihkel Ott

#ifndef __VK_UBM_H
#define __VK_UBM_H


#ifdef __VK_UBM_CPP
    #include <array>
    #include <memory>
    #include <vector>
    #include <mutex>
    #include <string>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/hashmap.h>
    #include <common/common.h>
    #include <common/err_def.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
    #include <deng/cross_api/gpu_mem.h>
    #include <deng/window.h>
    #include <deng/cam3d.h>

    #include <deng/lighting/light_srcs.h>
    #include <deng/vulkan/resources.h>
    #include <deng/cross_api/ubo.h>
    #include <deng/lighting/light_srcs.h>
    #include <deng/registry/registry.h>

    #define __DEFAULT_ASSET_CAP             32

    // This is a fucking war crime
    extern deng_ui32_t __max_frame_c;
#endif


namespace deng {
    namespace vulkan {

        class __vk_UniformBufferManager {
        private:
            std::vector<deng_Id> &m_assets;
            const deng_ui64_t m_min_align;
            deng_ui64_t m_global_ubo_chunk_size;
            deng::__GlobalRegistry &m_reg;
            __vk_BufferData &m_buffer_data;

        private:
            /// Create new uniform buffer instance and allocate memory for it.
            /// WARNING: This method call expects that uniform buffer and its memory
            /// have been freed or are not initialised previously
            void __mkUniformBuffer(VkDevice device, VkPhysicalDevice gpu,
                VkCommandPool cmd_pool, VkQueue g_queue, deng_ui64_t asset_cap, deng::BufferSectionInfo &buf_sec);

            
            /// Reallocate memory for uniform buffer and copy previous data to
            /// newly allocated buffer instance
            void __reallocUniformBufferMemory(VkDevice device, VkPhysicalDevice gpu,
                VkCommandPool cmd_pool, VkQueue g_queue, deng_ui64_t req_cap, deng::BufferSectionInfo &buf_sec);

            
            /// Reset uniform buffer size to first asset color data instance
            void __resetUboBufferSize(deng::BufferSectionInfo &buf_sec);

        public:
            __vk_UniformBufferManager(std::vector<deng_Id> &assets,
                const deng_ui64_t min_align, deng::__GlobalRegistry &reg,
                __vk_BufferData &buf_data);


            /// Reserve a uniform data memory location for Vulkan asset
            void mapUniformBufferArea(VkDevice device, VkPhysicalDevice gpu,
                VkCommandPool cmd_pool, VkQueue g_queue, __vk_Asset &asset, deng::BufferSectionInfo &buf_sec);
            

            /// Update uniform transformation data for the frame according to 
            /// camera's view and projection matrix
            void updateUboTransform3D(VkDevice device, 
                deng_ui32_t current_image, Camera3D *p_cam);

            
            /// Update asset uniform buffer data
            void updateAssetUboData(VkDevice device, 
                deng_ui32_t current_image, __vk_Asset &asset);

            
            /// Update lighting uniform data
            void updateUboLighting(VkDevice device,
                std::array<deng_Id, __DENG_MAX_LIGHT_SRC_COUNT> &light_srcs,
                deng_ui32_t current_image);


            /// Initialise uniform buffer for storing data
            void initUniformBuffer(VkDevice device, VkPhysicalDevice gpu,
                VkCommandPool cmd_pool, VkQueue g_queue, deng::BufferSectionInfo &buf_sec);

        /// Getter methods
        public:
            deng_ui64_t getUboChunkSize();
        };
    }
}


#endif
