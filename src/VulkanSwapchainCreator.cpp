// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanSwapchainCreator.h - Vulkan swapchain creator class implementation
// author: Karl-Mihkel Ott

#define VULKAN_SWAPCHAIN_CREATOR_CPP
#include <VulkanSwapchainCreator.h>

namespace DENG {
    
    namespace Vulkan {

        SwapchainCreator::SwapchainCreator(InstanceCreator *_instance_creator, Libdas::Point2D<int32_t> _win_size, VkSampleCountFlagBits _sample_c) : 
            m_instance_creator(_instance_creator), m_window_size(_win_size), m_sample_c(_sample_c)
        {
            _ConfigureSwapchainSettings();
            _CreateSwapchain();
            _CreateSwapchainImageViews();
            _CreateRenderPass();
        }


        SwapchainCreator::~SwapchainCreator() {
            for(auto it = m_swapchain_imageviews.begin(); it != m_swapchain_imageviews.end(); it++)
                vkDestroyImageView(m_instance_creator->GetDevice(), *it, NULL);

            vkDestroySwapchainKHR(m_instance_creator->GetDevice(), m_swapchain, NULL);
        }


        void SwapchainCreator::_ConfigureSwapchainSettings() {
            bool found_suitable_format = false;

            // check if found surface formats support SRGB coloring and nonlinear color space
            for(const VkSurfaceFormatKHR &sformat : m_instance_creator->GetSurfaceFormats()) {
                if(sformat.format == VK_FORMAT_B8G8R8A8_SRGB && sformat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    m_selected_surface_format = sformat;
                    found_suitable_format = true;
                    break;
                }
            }

            if(!found_suitable_format) {
                WARNME("Could not find suitable surface format! Trying to use first one available!");
                m_selected_surface_format = m_instance_creator->GetSurfaceFormats()[0];
            }


            bool found_presentation_mode = false;
            for(const VkPresentModeKHR &presentation_mode : m_instance_creator->GetPresentationModes()) {
                // Check which present modes are available
                switch (presentation_mode) {
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

                if(presentation_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                    m_selected_present_mode = presentation_mode;
                    found_presentation_mode = true;
                    break;
                }
            }

            if(!found_presentation_mode)
                m_selected_present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }


        void SwapchainCreator::_CreateSwapchain() {
            uint32_t min_image_count = m_instance_creator->GetSurfaceCapabilities().minImageCount + 1;
            
            // Verify that the maximum image count is not exceeded
            if(m_instance_creator->GetSurfaceCapabilities().maxImageCount > 0 && 
               min_image_count > m_instance_creator->GetSurfaceCapabilities().maxImageCount) 
                min_image_count = m_instance_creator->GetSurfaceCapabilities().maxImageCount;


            VkSwapchainCreateInfoKHR swapchain_createinfo = {};
            swapchain_createinfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchain_createinfo.surface = m_instance_creator->GetSurface();
            swapchain_createinfo.minImageCount = min_image_count;
            swapchain_createinfo.imageFormat = m_selected_surface_format.format;
            swapchain_createinfo.imageColorSpace = m_selected_surface_format.colorSpace;

            // Check if the retrieved extent is valid
            VkExtent2D ext = m_instance_creator->GetSurfaceCapabilities().currentExtent;
            if(ext.width != UINT32_MAX && ext.height != UINT32_MAX)
                swapchain_createinfo.imageExtent = ext;
            else {
                swapchain_createinfo.imageExtent.width = m_window_size.x;
                swapchain_createinfo.imageExtent.height = m_window_size.y;
            }

            m_extent = VkExtent2D{ swapchain_createinfo.imageExtent.width, swapchain_createinfo.imageExtent.height };
            swapchain_createinfo.imageArrayLayers = 1;
            swapchain_createinfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            // Check if present queue and graphics queue are the same and necessary synchronise the image sharing mode
            std::array<uint32_t, 2> queue_families = { m_instance_creator->GetGraphicsFamilyIndex(), m_instance_creator->GetPresentationFamilyIndex() };
            if(queue_families[0] != queue_families[1]) {
                swapchain_createinfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                swapchain_createinfo.queueFamilyIndexCount = static_cast<uint32_t>(queue_families.size());
                swapchain_createinfo.pQueueFamilyIndices = queue_families.data();
            }

            else swapchain_createinfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            

            swapchain_createinfo.preTransform = m_instance_creator->GetSurfaceCapabilities().currentTransform;

            // Check, which composite alpha mode is supported
            if(m_instance_creator->GetSurfaceCapabilities().supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) 
                swapchain_createinfo.compositeAlpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
            else if(m_instance_creator->GetSurfaceCapabilities().supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
                swapchain_createinfo.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
            else if(m_instance_creator->GetSurfaceCapabilities().supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
                swapchain_createinfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            else RUN_ERR("__mkSwapChain", "Could not select correct VK_COMPOSITE_ALPHA_BIT that is supported by DENG");

            swapchain_createinfo.presentMode = m_selected_present_mode;
            swapchain_createinfo.clipped = VK_TRUE;

            if(vkCreateSwapchainKHR(m_instance_creator->GetDevice(), &swapchain_createinfo, NULL, &m_swapchain) != VK_SUCCESS)
                VK_SWAPCHAIN_ERR("failed to create create a swap chain!");

            uint32_t image_count;
            vkGetSwapchainImagesKHR(m_instance_creator->GetDevice(), m_swapchain, &image_count, NULL);
            m_swapchain_images.resize(image_count);
            vkGetSwapchainImagesKHR(m_instance_creator->GetDevice(), m_swapchain, &image_count, m_swapchain_images.data());
        }


        void SwapchainCreator::_CreateSwapchainImageViews() {
            m_swapchain_imageviews.resize(m_swapchain_images.size());

            for(uint32_t i = 0; i < static_cast<uint32_t>(m_swapchain_imageviews.size()); i++) {
                VkImageViewCreateInfo iview_info = _GetImageViewInfo(m_swapchain_images[i], m_selected_surface_format.format, VK_IMAGE_ASPECT_COLOR_BIT, 1);

                if(vkCreateImageView(m_instance_creator->GetDevice(), &iview_info, NULL, &m_swapchain_imageviews[i]) != VK_SUCCESS)
                    VK_SWAPCHAIN_ERR("failed to create image views!");
            }
        }


        void SwapchainCreator::_CreateRenderPass() {
            VkAttachmentDescription color_attachment{};
            color_attachment.format = m_selected_surface_format.format;
            color_attachment.samples = m_sample_c;
            color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkAttachmentDescription depth_attachment{};
            depth_attachment.format = VK_FORMAT_D32_SFLOAT;
            depth_attachment.samples = m_sample_c;
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
            color_attachment_resolve.format = m_selected_surface_format.format;
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
            renderpass_createinfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            renderpass_createinfo.pAttachments = attachments.data();
            renderpass_createinfo.subpassCount = 1;
            renderpass_createinfo.pSubpasses = &subpass_desc;
            renderpass_createinfo.dependencyCount = 1;
            renderpass_createinfo.pDependencies = &subpass_dependency;


            if(vkCreateRenderPass(m_instance_creator->GetDevice(), &renderpass_createinfo, NULL, &m_renderpass) != VK_SUCCESS)
                VK_SWAPCHAIN_ERR("failed to create render pass!");
        }
    }
}
