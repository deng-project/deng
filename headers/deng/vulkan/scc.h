/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: scc.h - Vulkan swapchain creator class header
/// author: Karl-Mihkel Ott


#ifndef __VK_SCC_H
#define __VK_SCC_H

#ifdef __VK_SCC_CPP
    #include <vector>
    #include <array>

    #include <vulkan/vulkan.h>
    #include <common/base_types.h>   
    #include <common/hashmap.h>
    #include <common/err_def.h>
    #include <data/assets.h>
    
    #include <math/deng_math.h>
    #include <deng/window.h>
    #include <deng/vulkan/sd.h>
    #include <deng/vulkan/qm.h>
    #include <deng/vulkan/resources.h>
    #include <deng/vulkan/rend_infos.h>

    deng_ui32_t __max_frame_c;
#endif

namespace deng {
    namespace vulkan {

        class __vk_SwapChainCreator : private __vk_SwapChainInfo, private __vk_DeviceInfo {
        private:
            __vk_QueueManager m_qff;
            VkSampleCountFlagBits m_msaa_sample_c;
            void *m_udata;
        
        private:
#ifdef __DEBUG
            #define db_SampleCount() _db_SampleCount()
            // DEBUGGING HELPERS
            void _db_SampleCount();
#else
            #define db_SampleCount()
#endif

            void __mkSwapChainSettings();
            void __mkSwapChain(VkSurfaceKHR &surface, VkSurfaceCapabilitiesKHR &surface_cap, deng_ui32_t g_queue_i, deng_ui32_t p_queue_i);
            void __mkRenderPass();
            void __mkSCImageViews();
        
        public:
            __vk_SwapChainCreator(VkDevice device, Window &win, VkPhysicalDevice gpu, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR &&surface_cap,
                                  __vk_QueueManager qff, VkSampleCountFlagBits sample_c, void *udata);
            ~__vk_SwapChainCreator();

            void SCCleanup();

            /// System that checks for window resizing should be made
            void remkSwapChain (
                VkDevice device,
                Window *p_win,
                VkSurfaceKHR surface, 
                VkSurfaceCapabilitiesKHR &surface_cap,
                VkSurfaceFormatKHR s_format
            );

        /// Getters
        public:
            VkRenderPass getRp();
            VkExtent2D getExt();
            VkSwapchainKHR getSC();
            VkFormat getSF();
            std::vector<VkImage> getSCImg();
            std::vector<VkImageView> getSCImgViews();
        };  
    }
}
#endif
