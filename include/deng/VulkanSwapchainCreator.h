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
    #include <vulkan/vulkan.h>
    #include <string>
    #include <unordered_map>
#ifdef __DEBUG
    #include <iostream>
#endif

    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Matrix.h"

    #include "deng/Api.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/BaseTypes.h"
    #include "deng/Window.h"
    #include "deng/HardwareInfo.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Renderer.h"
    #include "deng/VulkanInstanceCreator.h"
    #include "deng/VulkanHelpers.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Renderer.h"
#endif

namespace DENG {
    namespace Vulkan {

        class SwapchainCreator {
            private:
                InstanceCreator &m_instance_creator;
                TRS::Point2D<uint32_t> m_window_size;

                VkSwapchainKHR m_swapchain;
                std::vector<TextureData> m_swapchain_images;
                VkSurfaceFormatKHR m_selected_surface_format;
                VkPresentModeKHR m_selected_present_mode;
                VkExtent2D m_extent;
                VkSampleCountFlagBits m_sample_c;

                const RendererConfig &m_config;
            
            private:
                void _ConfigureSwapchainSettings();
                void _CreateSwapchain();
                void _CreateSwapchainImageViews();
            
            public:
                static VkRenderPass CreateRenderPass(VkDevice _dev, VkFormat _format, VkSampleCountFlagBits _sample_c, bool _use_non_default_fb = false);

                SwapchainCreator(InstanceCreator &_ic, TRS::Point2D<uint32_t> _win_size, VkSampleCountFlagBits _sample_c, const RendererConfig &_conf);
                ~SwapchainCreator();

                /**
                 * Create new swapchain according to specified new window size
                 * NOTE: Pipelines, pipeline layouts and renderpasses must be destroyed beforehand
                 **/
                void RecreateSwapchain(TRS::Point2D<uint32_t> _new_win_size);

                inline VkSwapchainKHR &GetSwapchain() { return m_swapchain; }
                inline VkFormat GetSwapchainFormat() const { return m_selected_surface_format.format; }
                inline VkExtent2D GetExtent() const { return m_extent; }
                inline std::vector<TextureData> &GetSwapchainImages() { return m_swapchain_images; }
        };  
    }
}
#endif
