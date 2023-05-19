// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanSwapchainCreator.h - Vulkan swapchain creator class header
// author: Karl-Mihkel Ott


#ifndef VULKAN_SWAPCHAIN_CREATOR_H
#define VULKAN_SWAPCHAIN_CREATOR_H

#ifdef VULKAN_SWAPCHAIN_CREATOR_CPP
    #include <vector>
    #include <cstring>
    #include <array>
    #include <queue>
    #include <vulkan/vulkan.h>
    #include <string>
    #include <variant>
    #include <unordered_map>
#ifdef __DEBUG
    #include <iostream>
#endif

    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Quaternion.h"

    #include "das/Api.h"
    #include "das/DasStructures.h"

    #include "deng/Api.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/Exceptions.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/IWindowContext.h"
    #include "deng/VulkanHelpers.h"
    #include "deng/VulkanInstanceCreator.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/VulkanInstanceCreator.h"
#endif

namespace DENG {
    namespace Vulkan {

        class SwapchainCreator {
            private:
                const InstanceCreator *m_pInstanceCreator;
                VkSwapchainKHR m_hSwapchain = VK_NULL_HANDLE;
                std::vector<Vulkan::TextureData> m_swapchainImages;
                VkSurfaceFormatKHR m_selectedSurfaceFormat = {};
                VkPresentModeKHR m_eSelectedPresentMode = {};
                VkSampleCountFlagBits m_uSampleCountBits;

            private:
                void _ConfigureSwapchainSettings();
                void _CreateSwapchain(uint32_t _uWidth, uint32_t _uHeight);
                void _CreateSwapchainImageViews();
            
            public:
                static VkRenderPass CreateRenderPass(VkDevice _dev, VkFormat _format, VkSampleCountFlagBits _sample_c, bool _use_non_default_fb = false);

                SwapchainCreator(const InstanceCreator* _pInstanceCreator, uint32_t _uWidth, uint32_t _uHeight, VkSampleCountFlagBits _uSampleCountBits);
                ~SwapchainCreator();

                /**
                 * Create new swapchain according to specified new window size
                 * NOTE: Pipelines, pipeline layouts and renderpasses must be destroyed beforehand
                 **/
                void RecreateSwapchain(uint32_t _uWidth, uint32_t _uHeight);

                inline const std::vector<Vulkan::TextureData>& GetSwapchainImageHandles() const { return m_swapchainImages; }
                inline VkSwapchainKHR GetSwapchain() const { return m_hSwapchain; }
                inline VkFormat GetSwapchainFormat() const { return m_selectedSurfaceFormat.format; }
        };  
    }
}
#endif
