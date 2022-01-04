/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: VulkanSwapchainCreator.h - Vulkan swapchain creator class header
/// author: Karl-Mihkel Ott


#ifndef VULKAN_SWAPCHAIN_CREATOR_H
#define VULKAN_SWAPCHAIN_CREATOR_H

#ifdef VULKAN_SWAPCHAIN_CREATOR_CPP
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

namespace DENG {
    namespace Vulkan {

        class SwapChainCreator : private DeviceInfo {
        private:
            VkRenderPass m_renderpass;
            VkSwapchainKHR m_swapchain;
            VkExtent2D m_extent;
            VkPresentModeKHR m_present_mode;
            VkFormat m_format;
            VkSurfaceFormatKHR m_surface_format;
            std::vector<VkImage> m_swapchain_images;
            std::vector<VkImageView> m_swapchain_image_views;
            __vk_SwapChainDetails m_sc_details;
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
            void __mkSwapChain(VkPhysicalDevice gpu, VkSurfaceKHR surface, const VkSurfaceCapabilitiesKHR &surface_cap, 
                               deng_ui32_t g_queue_i, deng_ui32_t p_queue_i);
            void __mkRenderPass();
            void __mkSCImageViews();
        
        public:
            __vk_SwapChainCreator(VkDevice device, Window &win, VkPhysicalDevice gpu, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR &&surface_cap,
                                  __vk_QueueManager qff, VkSampleCountFlagBits sample_c, void *udata);

            void SCCleanup();

            /// System that checks for window resizing should be made
            void remkSwapChain (
                VkDevice device,
                VkPhysicalDevice gpu,
                Window *p_win,
                VkSurfaceKHR surface, 
                const VkSurfaceCapabilitiesKHR &surface_cap
            );

        /// Getters
        public:
            VkRenderPass getRp();
            VkExtent2D getExt(VkPhysicalDevice gpu, VkSurfaceKHR surface);
            VkSwapchainKHR getSC();
            VkFormat getSF();
            std::vector<VkImage> getSCImg();
            std::vector<VkImageView> getSCImgViews();
        };  
    }
}
#endif
