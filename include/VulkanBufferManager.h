/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: VulkanBufferManager.h - Vulkan resource manager parent class header for handling buffer related operations
/// author: Karl-Mihkel Ott


#ifndef VULKAN_BUFFER_MANAGER_H
#define VULKAN_BUFFER_MANAGER_H

#ifdef VULKAN_BUFFER_MANAGER_CPP
    #include <string.h>
    #include <memory>
    #include <vector>
    #include <string>
    #include <mutex>
    #include <queue>
    #include <vulkan/vulkan.h>

    #include <base_types.h>
    #include <err_def.h>
    #include <common.h>
    #include <hashmap.h>
    #include <assets.h>

    #include <deng_math.h>
	#include <window.h>
    #include <cam3d.h>
    #include <light_srcs.h>
    #include <resources.h>
    #include <registry.h>
    #include <imgui_entity.h>
    #include <cross_api.h>

    #include <ubm.h>
    #include <asset_cpy.h>
    #include <sd.h>
    #include <qm.h>
    #include <rend_infos.h>

    extern deng_ui32_t __max_frame_c;       
#endif

namespace DENG {
    namespace Vulkan {

        class BufferManager : public OffsetFinder, public UniformBufferManager {
        private:
            std::vector<deng_Id> &m_assets;
            const VkPhysicalDeviceLimits &m_gpu_limits;
            __ImGuiData *m_p_imgui_data = NULL;
            deng_ui64_t m_ubo_chunk_size;
            deng::Registry &m_reg;
            void *m_udata;

        protected:
            BufferData m_buffer_data;

        private:
            /// Create staging buffers for all asset data between bounds
            void _StageAssets(VkDevice device, VkPhysicalDevice gpu, VkCommandPool cmd_pool, VkQueue g_queue, 
                const dengMath::vec2<deng_ui32_t> &bounds, VkDeviceSize cpy_offset);

        protected:
            BufferManager(VkDevice device, VkPhysicalDevice gpu, const VkPhysicalDeviceLimits &gpu_limits, 
                std::vector<deng_Id> &assets, deng::Registry &reg, void *udata);

        public:
            /// Check if buffer reallocation is needed for assets and gui elements
            bool ReallocCheck(VkDevice device, VkPhysicalDevice gpu, VkCommandPool cmd_pool, VkQueue g_queue,
                const std::vector<VkFence> &fences);


            /// Allocate the reserved memory
            void AllocateMainBufferMemory(VkDevice device, VkPhysicalDevice gpu);


            /// Copy all asset data between given bounds to buffer
            /// NOTE: The asset capacity in buffer has to be larger than required asset size (use assetCapCheck() for this)
            void CpyAssetsToBuffer(VkDevice device, VkPhysicalDevice gpu, 
                VkCommandPool cmd_pool, VkQueue g_queue, deng_bool_t no_offset_calc, const dengMath::vec2<deng_ui32_t> &bounds);


            /// Copy ImGui vertex and indices data to buffer
            /// NOTE: The UI element capacity has to be larger than required UI element size (use uiCapCheck() for this)
            void CpyUIDataToBuffer(VkDevice device, VkPhysicalDevice gpu,
                VkCommandPool cmd_pool, VkQueue g_queue, deng_bool_t no_offset_calc);

        // Setter methods
        public:
            void SetUIDataPtr(__ImGuiData *p_gui);
        };
    }
}
#endif
