// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanSwapchainCreator.h - Vulkan swapchain creator class header
// author: Karl-Mihkel Ott


#ifndef VULKAN_SWAPCHAIN_CREATOR_H
#define VULKAN_SWAPCHAIN_CREATOR_H

#ifdef VULKAN_SWAPCHAIN_CREATOR_CPP
    #include <vector>
    #include <array>
    #include <vulkan/vulkan.h>

    #include <libdas/include/Points.h>

    #include <Api.h>
    #include <ErrorDefinitions.h>
    #include <BaseTypes.h>
    #include <Window.h>
    #include <VulkanInstanceCreator.h>
    #include <VulkanHelpers.h>
#endif

namespace DENG {
    namespace Vulkan {

        class SwapchainCreator {
            private:
                InstanceCreator *mp_instance_creator = nullptr;
                Libdas::Point2D<int32_t> m_window_size;


                VkSwapchainKHR m_swapchain;
                std::vector<VkImage> m_swapchain_images;
                std::vector<VkImageView> m_swapchain_imageviews;
                VkSurfaceFormatKHR m_selected_surface_format;
                VkPresentModeKHR m_selected_present_mode;
                VkExtent2D m_extent;
                VkSampleCountFlagBits m_sample_c;
                VkRenderPass m_renderpass;
            
            private:
                void _ConfigureSwapchainSettings();
                void _CreateSwapchain();
                void _CreateSwapchainImageViews();
                void _CreateRenderPass();
            
            public:
                SwapchainCreator(InstanceCreator *_instance_creator, Libdas::Point2D<int32_t> _win_size, VkSampleCountFlagBits _sample_c);
                ~SwapchainCreator();

                /**
                 * Create new swapchain according to specified new window size
                 * NOTE: Pipelines, pipeline layouts and renderpasses must be destroyed beforehand
                 **/
                void RecreateSwapchain(Libdas::Point2D<int32_t> _new_win_size);

                inline VkRenderPass GetRenderPass() { return m_renderpass; }
                inline VkSwapchainKHR &GetSwapchain() { return m_swapchain; }
                inline VkFormat GetSwapchainFormat() { return m_selected_surface_format.format; }
                inline VkExtent2D GetExtent() { return m_extent; }
                inline std::vector<VkImage> &GetSwapchainImages() { return m_swapchain_images; }
                inline std::vector<VkImageView> &GetSwapchainImageViews() { return m_swapchain_imageviews; }
        };  
    }
}
#endif
