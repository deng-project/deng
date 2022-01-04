// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanSwapchainDetails.h - Vulkan swapchain detail class header
// author: Karl-Mihkel Ott


#ifndef VULKAN_SWAPCHAIN_DETAILS_H
#define VULKAN_SWAPCHAIN_DETAILS_H

#ifdef VULKAN_SWAPCHAIN_DETAILS_CPP
    #include <vector>
    #include <stdexcept>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/err_def.h>
#endif

namespace deng {
    namespace vulkan {

        class SwapChainDetails {
        private:
            std::vector<VkSurfaceFormatKHR> m_formats;
            std::vector<VkPresentModeKHR> m_present_modes;

        public:
            __vk_SwapChainDetails(VkPhysicalDevice gpu, VkSurfaceKHR surface);
            VkSurfaceCapabilitiesKHR getCapabilities(VkPhysicalDevice gpu, VkSurfaceKHR surface);
            std::vector<VkSurfaceFormatKHR> getFormats();
            std::vector<VkPresentModeKHR> getPresentModes();
        };
    }
}

#endif
