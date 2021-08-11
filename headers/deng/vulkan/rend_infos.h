/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: rend_infos.h - Vulkan renderer initialiser helper data structures
/// author: Karl-Mihkel Ott


#ifndef __VK_REND_INFOS_H
#define __VK_REND_INFOS_H


namespace deng {
    namespace vulkan {

        /// Struct for storing all vulkan device related information 
        struct __vk_DeviceInfo {
            VkDevice m_device;
            deng::Window &m_win;
            VkPhysicalDevice m_gpu;
            __vk_DeviceInfo(deng::Window &win) : m_win(win) {} ;
        };


        /// Struct for storing all vk_ instance objects 
        struct __vk_InstanceInfo {
            __vk_QueueManager m_qff;
            VkInstance m_instance;
            VkSurfaceKHR m_surface;
            VkDebugUtilsMessengerEXT m_debug_mes;
        };


        /// Struct for storing all vk_ swapchain related objects 
        struct __vk_SwapChainInfo {
            VkRenderPass m_renderpass;
            VkSwapchainKHR m_swapchain;
            VkPresentModeKHR m_present_mode;
            VkExtent2D m_ext;
            VkFormat m_format;
            VkSurfaceFormatKHR m_surface_format;
            std::vector<VkImage> m_swapchain_images;
            std::vector<VkImageView> m_swapchain_image_views;
            __vk_SwapChainDetails *m_p_sc_details;
        };


        /// Struct for storing all vk_ resource allocation related objects 
        struct __vk_ResourceInfo {
            std::vector<VkFramebuffer> m_framebuffers;
            VkImage m_depth_image;
            VkDeviceMemory m_depth_image_mem;
            VkImageView m_depth_image_view;

            VkSampleCountFlagBits m_sample_count;
            VkImage m_color_image;
            VkDeviceMemory m_color_image_mem;
            VkImageView m_color_image_view;

            __vk_BufferData m_buffer_data;
        };
    }
}

#endif
