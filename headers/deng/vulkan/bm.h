/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: bm.h - Vulkan resource manager parent class header for handling buffer related operations
/// author: Karl-Mihkel Ott


#ifndef __VK_BM_H
#define __VK_BM_H

#ifdef __VK_BM_CPP
    #include <memory>
    #include <vector>
    #include <string>
    #include <mutex>
    #include <queue>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <common/common.h>
    #include <common/hashmap.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
	#include <deng/window.h>
    #include <deng/cam3d.h>
    #include <deng/lighting/light_srcs.h>
    #include <deng/vulkan/resources.h>
    #include <deng/registry/registry.h>
    #include <imgui-layer/imgui_entity.h>
    #include <deng/cross_api/cross_api.h>

    #include <deng/vulkan/ubm.h>
    #include <deng/vulkan/asset_cpy.h>
    #include <deng/vulkan/sd.h>
    #include <deng/vulkan/qm.h>
    #include <deng/vulkan/rend_infos.h>

    extern deng_ui32_t __max_frame_c;       
#endif

namespace deng {
    namespace vulkan {

        class __vk_BufferManager : public __OffsetFinder, public __vk_UniformBufferManager {
        private:
            std::vector<deng_Id> &m_assets;
            const VkPhysicalDeviceLimits &m_gpu_limits;
            __ImGuiData *m_p_imgui_data = NULL;
            deng_ui64_t m_ubo_chunk_size;
            deng::Registry &m_reg;
            void *m_udata;

        protected:
            __vk_BufferData m_buffer_data;

        private:
            /// Create staging buffers for all asset data between bounds
            void __stageAssets(VkDevice device, VkPhysicalDevice gpu, VkCommandPool cmd_pool, VkQueue g_queue, 
                const dengMath::vec2<deng_ui32_t> &bounds, VkDeviceSize cpy_offset);

        protected:
            __vk_BufferManager(VkDevice device, VkPhysicalDevice gpu, const VkPhysicalDeviceLimits &gpu_limits, 
                std::vector<deng_Id> &assets, deng::Registry &reg, void *udata);

        public:
            /// Check if buffer reallocation is needed for assets and gui elements
            deng_bool_t reallocCheck(VkDevice device, VkPhysicalDevice gpu, VkCommandPool cmd_pool, VkQueue g_queue,
                const std::vector<VkFence> &fences);


            /// Allocate the reserved memory
            void allocateMainBufferMemory(VkDevice device, VkPhysicalDevice gpu);


            /// Copy all asset data between given bounds to buffer
            /// NOTE: The asset capacity in buffer has to be larger than required asset size (use assetCapCheck() for this)
            void cpyAssetsToBuffer(VkDevice device, VkPhysicalDevice gpu, 
                VkCommandPool cmd_pool, VkQueue g_queue, deng_bool_t no_offset_calc, const dengMath::vec2<deng_ui32_t> &bounds);


            /// Copy ImGui vertex and indices data to buffer
            /// NOTE: The UI element capacity has to be larger than required UI element size (use uiCapCheck() for this)
            void cpyUIDataToBuffer(VkDevice device, VkPhysicalDevice gpu,
                VkCommandPool cmd_pool, VkQueue g_queue, deng_bool_t no_offset_calc);

        // Setter methods
        public:
            void setUIDataPtr(__ImGuiData *p_gui);
        };
    }
}
#endif
