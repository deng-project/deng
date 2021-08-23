/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: rm.cpp - Vulkan resource manager class implementation
/// author: Karl-Mihkel Ott

#define __VK_RM_CPP
#include <deng/vulkan/rm.h>

namespace deng {
    namespace vulkan {

        /*********************************************************/
        /********** __vk_ResourceManager class methods ***********/
        /*********************************************************/

        __vk_ResourceManager::__vk_ResourceManager (
            VkDevice device, 
            VkPhysicalDevice gpu, 
            VkExtent2D extent,
            VkSampleCountFlagBits sample_c, 
            VkRenderPass renderpass, 
            VkCommandPool cmd_pool,
            VkQueue g_queue,
            const std::vector<VkImageView> &sc_img_views,
            deng::Registry &reg,
            std::vector<deng_Id> &assets,
            std::vector<deng_Id> &textures,
            VkFormat sc_color_format,
            const VkPhysicalDeviceLimits &gpu_limits, 
            void *udata) : 
            __vk_BufferManager(device, gpu, gpu_limits, assets, reg, udata),
            __vk_TextureManager(device, gpu, cmd_pool, g_queue, textures, reg, m_buffer_data, __vk_BufferManager::__OffsetFinder::getSectionInfo(), udata),
            m_udata(udata)
        {
            m_sample_c = sample_c;
            // Allocate memory for new uniform buffers
            initUniformBuffer(device, gpu, cmd_pool, g_queue, __vk_BufferManager::getSectionInfo());

            // Create new color and depth resources
            __mkColorResources(device, gpu, extent, sc_color_format);
            __mkDepthResources(device, gpu, extent); 

            // Allocate frame buffers
            __mkFrameBuffers(device, renderpass, extent, sc_img_views);
        }


        void __vk_ResourceManager::remkFrameBuffers (
            VkDevice device, 
            VkPhysicalDevice gpu,
            VkRenderPass rp,
            VkExtent2D ext,
            VkFormat sc_color_format,
            const std::vector<VkImageView> &sc_img_views
        ) {
            vkDestroyImageView(device, m_color_image_view, NULL);
            vkFreeMemory(device, m_color_image_mem, NULL);
            vkDestroyImage(device, m_color_image, NULL);

            vkDestroyImageView(device, m_depth_image_view, NULL);
            vkFreeMemory(device, m_depth_image_mem, NULL);
            vkDestroyImage(device, m_depth_image, NULL);

            for(size_t i = 0; i < m_framebuffers.size(); i++)
                vkDestroyFramebuffer(device, m_framebuffers[i], NULL);

            __mkColorResources(device, gpu, ext, sc_color_format);
            __mkDepthResources(device, gpu, ext);
            __mkFrameBuffers(device, rp, ext, sc_img_views);
        }


        /// Create new framebuffers 
        void __vk_ResourceManager::__mkFrameBuffers (
            VkDevice device, 
            VkRenderPass renderpass, 
            VkExtent2D extent, 
            const std::vector<VkImageView> &sc_img_views
        ) {
            size_t index;
            m_framebuffers.resize(sc_img_views.size());
            std::array<VkImageView, 3> attachments;

            for(index = 0; index < sc_img_views.size(); index++) {
                attachments = { m_color_image_view, m_depth_image_view, sc_img_views[index] };

                VkFramebufferCreateInfo framebuffer_createinfo{};
                framebuffer_createinfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebuffer_createinfo.renderPass = renderpass;
                framebuffer_createinfo.attachmentCount = static_cast<deng_ui32_t>(attachments.size());
                framebuffer_createinfo.pAttachments = attachments.data();
                framebuffer_createinfo.width = extent.width;
                framebuffer_createinfo.height = extent.height;
                framebuffer_createinfo.layers = 1;

                SET_UDATA("vkCreateFramebuffer");
                if(vkCreateFramebuffer(device, &framebuffer_createinfo, NULL, &m_framebuffers[index]) != VK_SUCCESS)
                    VK_RES_ERR("failed to create framebuffer!");
                
                else LOG("Framebuffer successfully created");
            }
        }


        /// Create color resources for multisampling 
        void __vk_ResourceManager::__mkColorResources (
            VkDevice device,
            VkPhysicalDevice gpu,
            VkExtent2D extent,
            VkFormat sc_color_format
        ) {
            // Create a new Vulkan image handle
            VkMemoryRequirements mem_req = __vk_ImageCreator::makeImage(device, gpu, m_color_image, extent.width, 
                extent.height, 1, sc_color_format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | 
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_sample_c, m_udata);

            // Allocate memory for the image handle
            __vk_BufferCreator::allocateMemory(device, gpu, mem_req.size, m_color_image_mem,
                mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_udata);

            // Bind the memory with the image handle
            SET_UDATA("vkBindImageMemory");
            vkBindImageMemory(device, m_color_image, m_color_image_mem, 0);

            // Create image view
            VkImageViewCreateInfo image_view_createinfo = __vk_ImageCreator::getImageViewInfo (
                m_color_image, sc_color_format, VK_IMAGE_ASPECT_COLOR_BIT, 1, m_udata);

            SET_UDATA("vkCreateImageView");
            if(vkCreateImageView(device, &image_view_createinfo, NULL, &m_color_image_view) != VK_SUCCESS)
                VK_RES_ERR("failed to create color image view");
        }


        /// Create depth image buffers for depth buffering 
        void __vk_ResourceManager::__mkDepthResources (
            VkDevice device, 
            VkPhysicalDevice gpu, 
            VkExtent2D extent
        ) {
            // Create an VkImage instance for depth buffers
            VkMemoryRequirements mem_req = __vk_ImageCreator::makeImage (
                device, gpu, m_depth_image, extent.width, extent.height, 
                1, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, 
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, m_sample_c, m_udata);
            
            // Allocate memory for image buffer
            __vk_BufferCreator::allocateMemory(device, gpu, 
                mem_req.size, m_depth_image_mem, mem_req.memoryTypeBits, 
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_udata);

            // Bind the image buffer with its memory
            SET_UDATA("vkBindImageMemory");
            vkBindImageMemory(device, m_depth_image, 
                m_depth_image_mem, 0);

            // Create image view createinfo
            VkImageViewCreateInfo image_view_createinfo = __vk_ImageCreator::getImageViewInfo (
                m_depth_image, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT, 1, m_udata);

            // Create new image view
            SET_UDATA("vkCreateImageView");
            if(vkCreateImageView(device, &image_view_createinfo, NULL, &m_depth_image_view) != VK_SUCCESS)
                VK_RES_ERR("failed to create depth image view!");
        }


        /// __vk_ResourceManager class getters
        __vk_BufferData &__vk_ResourceManager::getBD() { return m_buffer_data; }
        std::vector<VkFramebuffer> &__vk_ResourceManager::getFB() { return m_framebuffers; }
        VkImage __vk_ResourceManager::getDepImg() { return m_depth_image; }
        VkDeviceMemory __vk_ResourceManager::getDepImgMem() { return m_depth_image_mem; }
        VkImageView __vk_ResourceManager::getDepImgView() { return m_depth_image_view; }
        VkImage __vk_ResourceManager::getColorImg() { return m_color_image; }
        VkDeviceMemory __vk_ResourceManager::getColorImgMem() { return m_color_image_mem; }
        VkImageView __vk_ResourceManager::getColorImgView() { return m_color_image_view; }

        void __vk_ResourceManager::setUIDataPtr(__ImGuiData *p_gui) { __vk_BufferManager::setUIDataPtr(p_gui); };
    }
}
