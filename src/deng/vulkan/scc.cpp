/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: scc.cpp - Vulkan swapchain creator class implementation
/// author: Karl-Mihkel Ott


#define __VK_SCC_CPP
#include <deng/vulkan/scc.h>

namespace deng {
    namespace vulkan {

        __vk_SwapChainCreator::__vk_SwapChainCreator (
            VkDevice device,
            deng::Window &win,
            VkPhysicalDevice gpu, 
            VkSurfaceKHR surface, 
            VkSurfaceCapabilitiesKHR &&surface_cap,
            __vk_QueueManager qff,
            VkSampleCountFlagBits sample_c,
            void *p_udata
        ) : __vk_DeviceInfo(win), m_udata(p_udata) { 
            m_msaa_sample_c = sample_c;       
            db_SampleCount();
            m_device = device;
            m_gpu = gpu;
            m_qff = qff;
            
            m_p_sc_details = new __vk_SwapChainDetails(gpu, surface);

            __mkSwapChainSettings();
            __mkSwapChain(surface, surface_cap, m_qff.getGraphicsQFIndex(), m_qff.getPresentQFIndex());
            __mkSCImageViews();
            __mkRenderPass();
        }

        /// DEBUGGING
#ifdef __DEBUG
        void __vk_SwapChainCreator::_db_SampleCount() {
            switch(m_msaa_sample_c) {
            case VK_SAMPLE_COUNT_64_BIT:
                LOG("Using VK_SAMPLE_COUNT_64_BIT for sample count");
                break;

            case VK_SAMPLE_COUNT_32_BIT:
                LOG("Using VK_SAMPLE_COUNT_32_BIT for sample count");
                break;

            case VK_SAMPLE_COUNT_16_BIT:
                LOG("Using VK_SAMPLE_COUNT_16_BIT for sample count");
                break;

            case VK_SAMPLE_COUNT_8_BIT:
                LOG("Using VK_SAMPLE_COUNT_8_BIT for sample count");
                break;

            case VK_SAMPLE_COUNT_4_BIT:
                LOG("Using VK_SAMPLE_COUNT_4_BIT for sample count");
                break;

            case VK_SAMPLE_COUNT_2_BIT:
                LOG("Using VK_SAMPLE_COUNT_2_BIT for sample count");
                break;

            case VK_SAMPLE_COUNT_1_BIT:
                LOG("Using VK_SAMPLE_COUNT_1_BIT for sample count");
                break;

            default:
                return;
            }
        }
#endif


        /// Initialise swapchain settings in order to create swapchain 
        void __vk_SwapChainCreator::__mkSwapChainSettings() {
            deng_bool_t found_suitable_format = false;
            for(const VkSurfaceFormatKHR &surface_format : m_p_sc_details->getFormats()) {
                if(surface_format.format == VK_FORMAT_B8G8R8A8_SRGB && surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    m_surface_format = surface_format;
                    found_suitable_format = true;
                }
            }
            
            if(!found_suitable_format) {
                WARNME("Didn't find suitable surface format! Trying to use first available one!");
                m_surface_format = m_p_sc_details->getFormats()[0];
            }

            deng_bool_t foundSuitablePresentMode = false;
            for(const VkPresentModeKHR &presentFormat : m_p_sc_details->getPresentModes()) {
                // Check which present modes are available
                switch (presentFormat) {
                case VK_PRESENT_MODE_IMMEDIATE_KHR:
                    LOG("VK_PRESENT_MODE_IMMEDIATE_KHR is available!");
                    break;

                case VK_PRESENT_MODE_MAILBOX_KHR:
                    LOG("VK_PRESENT_MODE_MAILBOX_KHR is available!");
                    break;

                case VK_PRESENT_MODE_FIFO_KHR:
                    LOG("VK_PRESENT_MODE_FIFO_KHR is available!");
                    break;

                case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
                    LOG("VK_PRESENT_MODE_FIFO_KHR is available!");
                    break;

                case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
                    LOG("VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR is available!");
                    break;

                case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
                    LOG("VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR is available!");
                    break;

                default:
                    break;
                }

                if(presentFormat == VK_PRESENT_MODE_MAILBOX_KHR) {
                    m_present_mode = presentFormat;
                    foundSuitablePresentMode = true;
                    LOG("Found suitable present mode for triple buffering!");
                }
            }

            if(!foundSuitablePresentMode) {
                WARNME("Didn't find suitable present mode! Using vSync instead!");
                m_present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            }

            if(m_p_sc_details->getCapabilities().currentExtent.width != UINT32_MAX && 
               m_p_sc_details->getCapabilities().currentExtent.height != UINT32_MAX) 
                m_ext = m_p_sc_details->getCapabilities().currentExtent;

            else {
                m_ext.width = m_win.getSize().first;
                m_ext.height = m_win.getSize().second;
            }
        }


        /// Create new swapchain for the surface
        void __vk_SwapChainCreator::__mkSwapChain (
            VkSurfaceKHR &surface, 
            VkSurfaceCapabilitiesKHR &surface_cap,
            deng_ui32_t g_queue_i, 
            deng_ui32_t p_queue_i
        ) {
            deng_ui32_t min_image_count = m_p_sc_details->getCapabilities().minImageCount + 1;
            
            // Verify that the maximum image count is not exceeded
            if(m_p_sc_details->getCapabilities().maxImageCount > 0 && min_image_count > m_p_sc_details->getCapabilities().maxImageCount) 
                min_image_count = m_p_sc_details->getCapabilities().maxImageCount;


            VkSwapchainCreateInfoKHR swapchain_createinfo = {};
            swapchain_createinfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchain_createinfo.surface = surface;
            swapchain_createinfo.minImageCount = min_image_count;
            swapchain_createinfo.imageFormat = m_surface_format.format;
            swapchain_createinfo.imageColorSpace = m_surface_format.colorSpace;
            swapchain_createinfo.imageExtent = m_ext;
            swapchain_createinfo.imageArrayLayers = 1;
            swapchain_createinfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            // Check if present queue and graphics queue are the same and if needed then synchronise the image sharing mode
            std::array<deng_ui32_t, 2> queue_family_indices = {g_queue_i, p_queue_i};
            if(g_queue_i != p_queue_i) {
                swapchain_createinfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                swapchain_createinfo.queueFamilyIndexCount = 2;
                swapchain_createinfo.pQueueFamilyIndices = queue_family_indices.data();
            }

            else swapchain_createinfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            

            swapchain_createinfo.preTransform = m_p_sc_details->getCapabilities().currentTransform;

            // Check, which composite alpha mode is supported
            if(surface_cap.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) 
                swapchain_createinfo.compositeAlpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
            else if(surface_cap.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
                swapchain_createinfo.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
            else if(surface_cap.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
                swapchain_createinfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            else RUN_ERR("__mkSwapChain", "Could not select correct VK_COMPOSITE_ALPHA_BIT that is supported by DENG");

            swapchain_createinfo.presentMode = m_present_mode;
            swapchain_createinfo.clipped = VK_TRUE;

            SET_UDATA("vkCreateSwapchainKHR");
            if(vkCreateSwapchainKHR(m_device, &swapchain_createinfo, NULL, &m_swapchain) != VK_SUCCESS)
                VK_SWAPCHAIN_ERR("failed to create create a swap chain!");

            else {
                deng_ui32_t image_count;
                SET_UDATA("vkGetSwapchainImagesKHR");
                vkGetSwapchainImagesKHR(m_device, m_swapchain, 
                    &image_count, NULL);
                m_swapchain_images.resize(image_count);
                __max_frame_c = image_count;
                SET_UDATA("vkGetSwapchainImagesKHR");
                vkGetSwapchainImagesKHR(m_device, m_swapchain, 
                    &image_count, m_swapchain_images.data());
            }
        }


        /// Create render pass
        void __vk_SwapChainCreator::__mkRenderPass() {
            VkAttachmentDescription color_attachment{};
            color_attachment.format = m_surface_format.format;
            color_attachment.samples = m_msaa_sample_c;
            color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkAttachmentDescription depth_attachment{};
            depth_attachment.format = VK_FORMAT_D32_SFLOAT;
            depth_attachment.samples = m_msaa_sample_c;
            depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            VkAttachmentReference color_attachment_reference{};
            color_attachment_reference.attachment = 0;
            color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            VkAttachmentReference depth_attachment_reference{};
            depth_attachment_reference.attachment = 1;
            depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            // Color attachment description for resolving multisampled images
            VkAttachmentDescription color_attachment_resolve{};
            color_attachment_resolve.format = m_surface_format.format;
            color_attachment_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
            color_attachment_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            color_attachment_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            color_attachment_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            color_attachment_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            color_attachment_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference color_attachment_resolve_ref{};
            color_attachment_resolve_ref.attachment = 2;
            color_attachment_resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass_desc{};
            subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass_desc.colorAttachmentCount = 1;
            subpass_desc.pColorAttachments = &color_attachment_reference;
            subpass_desc.pDepthStencilAttachment = &depth_attachment_reference;
            subpass_desc.pResolveAttachments = &color_attachment_resolve_ref;

            VkSubpassDependency subpass_dependency{};
            subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpass_dependency.srcAccessMask = 0;

            subpass_dependency.dstSubpass = 0;
            subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            std::array<VkAttachmentDescription, 3> attachments = { color_attachment, depth_attachment, color_attachment_resolve };
            VkRenderPassCreateInfo renderpass_createinfo{};
            renderpass_createinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderpass_createinfo.attachmentCount = static_cast<deng_ui32_t>(attachments.size());
            renderpass_createinfo.pAttachments = attachments.data();
            renderpass_createinfo.subpassCount = 1;
            renderpass_createinfo.pSubpasses = &subpass_desc;
            renderpass_createinfo.dependencyCount = 1;
            renderpass_createinfo.pDependencies = &subpass_dependency;


            SET_UDATA("vkCreateRenderPass");
            if(vkCreateRenderPass(m_device, &renderpass_createinfo, NULL, &m_renderpass) != VK_SUCCESS)
                VK_SWAPCHAIN_ERR("failed to create render pass!");
        }


        /// Create swapchain image views 
        void __vk_SwapChainCreator::__mkSCImageViews() {
            m_swapchain_image_views.resize(m_swapchain_images.size());

            for(deng_ui32_t i = 0; i < m_swapchain_image_views.size(); i++) {
                VkImageViewCreateInfo createInfo = 
                __vk_ImageCreator::getImageViewInfo ( m_swapchain_images[i], m_surface_format.format, VK_IMAGE_ASPECT_COLOR_BIT, 1, m_udata);
                SET_UDATA("vkCreateImageView");
                if(vkCreateImageView(m_device, &createInfo, NULL, &m_swapchain_image_views[i]) != VK_SUCCESS)
                    VK_SWAPCHAIN_ERR("failed to create image views!");
            }
        }


        /// Remake the swapchain (needed for resizing the window) 
        void __vk_SwapChainCreator::remkSwapChain (
            VkDevice device,
            deng::Window *p_win,
            VkSurfaceKHR surface,
            VkSurfaceCapabilitiesKHR &surface_cap,
            VkSurfaceFormatKHR s_format
        ) {
            SET_UDATA("vkDeviceWaitIdle");
            vkDeviceWaitIdle(m_device);

            SCCleanup();
            __mkSwapChainSettings();
            __mkSwapChain(surface, surface_cap, m_qff.getGraphicsQFIndex(), m_qff.getPresentQFIndex());
            __mkRenderPass();
            __mkSCImageViews();
        }


        /// Clean all swap chain related structs 
        void __vk_SwapChainCreator::SCCleanup() {
            size_t index = 0;
            for(index = 0; index < m_swapchain_images.size(); index++) {
                SET_UDATA("vkDestroyImageView");
                vkDestroyImageView(m_device, m_swapchain_image_views[index], NULL);
            }

            SET_UDATA("vkDestroySwapchainKHR");
            vkDestroySwapchainKHR(m_device, m_swapchain, NULL);
        }


        __vk_SwapChainCreator::~__vk_SwapChainCreator() {
            delete m_p_sc_details;
        }

        
        /// Getters
        VkRenderPass __vk_SwapChainCreator::getRp() { return m_renderpass; }
        VkExtent2D __vk_SwapChainCreator::getExt() { return m_ext; }
        VkSwapchainKHR __vk_SwapChainCreator::getSC() { return m_swapchain; }
        VkFormat __vk_SwapChainCreator::getSF() { return m_surface_format.format; }
        std::vector<VkImage> __vk_SwapChainCreator::getSCImg() { return m_swapchain_images; }
        std::vector<VkImageView> __vk_SwapChainCreator::getSCImgViews() { return m_swapchain_image_views; }
    }
}
