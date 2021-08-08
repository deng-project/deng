/// DENG: dynamic engine - powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: rm.h - Vulkan resource manager class header
/// author: Karl-Mihkel Ott


#ifndef __VK_RM_H
#define __VK_RM_H

#ifdef __VK_RM_CPP
    #include <memory>
    #include <vector>
    #include <array>
    #include <queue>
    #include <mutex>
    #include <cmath>
	#include <algorithm>

    #include <vulkan/vulkan.h>
    #include <common/base_types.h>
    #include <common/hashmap.h>
    #include <common/uuid.h>
    #include <common/common.h>
    #include <common/err_def.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
    #include <deng/cross_api/gpu_mem.h>
    #include <deng/window.h>
    #include <deng/cam3d.h>

    #include <deng/vulkan/asset_cpy.h>
    #include <deng/vulkan/sd.h>
    #include <deng/vulkan/qm.h>
    #include <deng/vulkan/resources.h>
    #include <deng/vulkan/rend_infos.h>

    #include <imgui-layer/imgui_entity.h>
    #include <deng/lighting/light_srcs.h>
    #include <deng/registry/registry.h>
    #include <deng/cross_api/offset_finder.h>
    #include <imgui-layer/imgui_entity.h>

    #include <deng/vulkan/ubm.h>
    #include <deng/vulkan/bm.h>
    #include <deng/vulkan/tm.h>

    extern deng_ui32_t __max_frame_c;       
#endif


namespace deng {
    namespace vulkan {

        class __vk_ResourceManager : public __vk_BufferManager, public __vk_TextureManager {
        private:
            // Color image instances
            VkImage m_color_image;
            VkDeviceMemory m_color_image_mem;
            VkImageView m_color_image_view;

            // Depth image instances
            VkImage m_depth_image;
            VkDeviceMemory m_depth_image_mem;
            VkImageView m_depth_image_view;

            std::vector<VkFramebuffer> m_framebuffers;
            VkSampleCountFlagBits m_sample_c;
            deng_ui32_t m_tex_mem_bits = 0;

        private:

            void __mkFrameBuffers(VkDevice &device, VkRenderPass &renderpass, 
                VkExtent2D &ext, const std::vector<VkImageView> &sc_img_views);

            void __mkColorResources(VkDevice &device, VkPhysicalDevice &gpu,
                VkExtent2D &ext, VkFormat sc_color_format);

            void __mkDepthResources(VkDevice &device, VkPhysicalDevice &gpu, VkExtent2D &ext);

            
        public:
            __vk_ResourceManager(VkDevice device, VkPhysicalDevice gpu, VkExtent2D ext, VkSampleCountFlagBits sample_c,
                VkRenderPass renderpass, VkCommandPool cmd_pool, VkQueue g_queue, const std::vector<VkImageView> &sc_img_views,
                deng::__GlobalRegistry &reg, std::vector<deng_Id> &assets, std::vector<deng_Id> &textures, VkFormat sc_color_format,
                const VkPhysicalDeviceLimits &gpu_limits);


        // Getter and setter methods
        public:
            __vk_BufferData &getBD();
            std::vector<VkFramebuffer> getFB();
            VkImage getDepImg();
            VkDeviceMemory getDepImgMem();
            VkImageView getDepImgView();
            VkImage getColorImg();
            VkDeviceMemory getColorImgMem();
            VkImageView getColorImgView();

            void setUIDataPtr(__ImGuiData *p_gui);
        };
    }
}

#endif
