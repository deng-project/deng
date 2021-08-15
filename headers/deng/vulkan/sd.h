/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: sd.h - Vulkan swapchain detail handler class header
/// author: Karl-Mihkel Ott


#ifndef __VK_SD_H
#define __VK_SD_H

#ifdef __VK_SD_CPP
    #include <vector>
    #include <stdexcept>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/err_def.h>
#endif

namespace deng {
    namespace vulkan {

        class __vk_SwapChainDetails {
        private:
            VkSurfaceCapabilitiesKHR m_capabilities;
            std::vector<VkSurfaceFormatKHR> m_formats;
            std::vector<VkPresentModeKHR> m_present_modes;

        public:
            __vk_SwapChainDetails (
                VkPhysicalDevice &gpu, 
                VkSurfaceKHR &surface
            );

            VkSurfaceCapabilitiesKHR getCapabilities();
            std::vector<VkSurfaceFormatKHR> getFormats();
            std::vector<VkPresentModeKHR> getPresentModes();
        };
    }
}

#endif
