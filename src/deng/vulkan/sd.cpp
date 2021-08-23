/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: sd.cpp - Vulkan swapchain detail handler class implementation
/// author: Karl-Mihkel Ott


#define __VK_SD_CPP
#include <deng/vulkan/sd.h>

namespace deng {
    namespace vulkan {

        /*****************************************/
        /********* __vk_SwapChainDetails *********/
        /*****************************************/
        __vk_SwapChainDetails::__vk_SwapChainDetails (
            VkPhysicalDevice gpu, 
            VkSurfaceKHR surface
        ) {
            // Find device capabilities and formats
            VkSurfaceCapabilitiesKHR capabilities;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &capabilities);
            deng_ui32_t format_count;
            vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &format_count, NULL);

            if(!format_count) 
                VK_SWAPCHAIN_ERR("no surface formats available!");

            m_formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &format_count, m_formats.data());
            
            deng_ui32_t present_mode_count;
            vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &present_mode_count, NULL);

            // Check if present modes are available
            if(!present_mode_count) 
                VK_SWAPCHAIN_ERR("No surface present modes available!");

            m_present_modes.resize(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &present_mode_count, m_present_modes.data());
        }


        VkSurfaceCapabilitiesKHR __vk_SwapChainDetails::getCapabilities(VkPhysicalDevice gpu, VkSurfaceKHR surface) { 
            VkSurfaceCapabilitiesKHR capabilities;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &capabilities);
            return capabilities; 
        }


        std::vector<VkSurfaceFormatKHR> __vk_SwapChainDetails::getFormats() { return m_formats; }
        std::vector<VkPresentModeKHR> __vk_SwapChainDetails::getPresentModes() { return m_present_modes; }
    }
}
