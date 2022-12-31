// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanSwapchainCreator.h - Vulkan swapchain creator class implementation
// author: Karl-Mihkel Ott

#define VULKAN_SWAPCHAIN_CREATOR_CPP
#include "deng/VulkanSwapchainCreator.h"

namespace DENG {
    namespace Vulkan {
        SwapchainCreator::SwapchainCreator(InstanceCreator &_ic, TRS::Point2D<uint32_t> _win_size, VkSampleCountFlagBits _sample_c) : 
            m_instance_creator(_ic), 
            m_window_size(_win_size), 
            m_sample_c(_sample_c)
        {
            _ConfigureSwapchainSettings();
            _CreateSwapchain();
            _CreateSwapchainImageViews();
        }


        SwapchainCreator::~SwapchainCreator() {
            for (auto it = m_swapchain_image_ids.begin(); it != m_swapchain_image_ids.end(); it++) {
                TextureDatabase* db = TextureDatabase::GetInstance();
                Vulkan::TextureData tex_data = std::get<Vulkan::TextureData>(db->GetResource(*it).vendor);
                vkDestroyImageView(m_instance_creator.GetDevice(), tex_data.image_view, nullptr);
            }

            vkDestroySwapchainKHR(m_instance_creator.GetDevice(), m_swapchain, nullptr);
        }


        VkRenderPass SwapchainCreator::CreateRenderPass(VkDevice _dev, VkFormat _format, VkSampleCountFlagBits _msaa_samples, bool _use_non_default_fb) {
            std::array<VkAttachmentDescription, 2> attachments = {};
            // color attachment
            attachments[0].format = _format;
            attachments[0].samples = _msaa_samples;
            attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            if(!_use_non_default_fb) {
                attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            } else {
                attachments[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }

            // depth attachment
            attachments[1].format = VK_FORMAT_D32_SFLOAT;
            attachments[1].samples = _msaa_samples;
            attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            // color attachment resolver attachment
            //attachments[2].format = _format;
            //attachments[2].samples = VK_SAMPLE_COUNT_1_BIT;
            //attachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            //attachments[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            //attachments[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            //attachments[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            //attachments[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            //attachments[2].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            std::array<VkAttachmentReference, 2> references = {};
            // color attachment reference
            references[0].attachment = 0;
            references[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            // depth attachment reference
            references[1].attachment = 1;
            references[1].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            // color attachment resolver attachment reference
            //references[2].attachment = 2;
            //references[2].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass_desc = {};
            subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass_desc.colorAttachmentCount = 1;
            subpass_desc.pColorAttachments = &references[0];
            subpass_desc.pDepthStencilAttachment = &references[1];
            //subpass_desc.pResolveAttachments = &references[2];

            std::array<VkSubpassDependency, 2> subpass_dependencies = {};
            if(_use_non_default_fb) {
                // transform the image into VK_PIPELINE_STAGE_COLOR_OUTPUT_ATTACHMENT_BIT since the original stage is VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
                subpass_dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
                subpass_dependencies[0].dstSubpass = 0;
                subpass_dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                subpass_dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                subpass_dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                subpass_dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                subpass_dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

                // transform the image back VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT so it could be used as a texture
                subpass_dependencies[1].srcSubpass = 0;
                subpass_dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
                subpass_dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                subpass_dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                subpass_dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                subpass_dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            } else {
                subpass_dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
                subpass_dependencies[0].dstSubpass = 0;
                subpass_dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                subpass_dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                subpass_dependencies[0].srcAccessMask = 0;
                subpass_dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            }

            VkRenderPassCreateInfo renderpass_createinfo = {};
            renderpass_createinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderpass_createinfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            renderpass_createinfo.pAttachments = attachments.data();
            renderpass_createinfo.subpassCount = 1;
            renderpass_createinfo.pSubpasses = &subpass_desc;
            if(_use_non_default_fb) renderpass_createinfo.dependencyCount = 2;
            else renderpass_createinfo.dependencyCount = 0;
            renderpass_createinfo.pDependencies = subpass_dependencies.data();

            VkRenderPass renderpass = VK_NULL_HANDLE;
            if(vkCreateRenderPass(_dev, &renderpass_createinfo, nullptr, &renderpass) != VK_SUCCESS)
                VK_SWAPCHAIN_ERR("failed to create render pass!");

            return renderpass;
        }


        void SwapchainCreator::RecreateSwapchain(TRS::Point2D<uint32_t> _new_win_size) {
            const VkDevice device = m_instance_creator.GetDevice();
            m_window_size = _new_win_size;

            // cleanup the previous swapchain
            TextureDatabase* db = TextureDatabase::GetInstance();
            for (auto it = m_swapchain_image_ids.begin(); it != m_swapchain_image_ids.end(); it++) {
                Vulkan::TextureData& data = std::get<Vulkan::TextureData>(db->GetResource(*it).vendor);
                vkDestroyImageView(m_instance_creator.GetDevice(), data.image_view, nullptr);
            }

            vkDestroySwapchainKHR(device, m_swapchain, nullptr);

            m_instance_creator.UpdateSurfaceProperties();
            _CreateSwapchain();
            _CreateSwapchainImageViews();
        }


        void SwapchainCreator::_ConfigureSwapchainSettings() {
            bool found_suitable_format = false;

            // check if found surface formats support SRGB coloring and nonlinear color space
            for(const VkSurfaceFormatKHR &sformat : m_instance_creator.GetSurfaceFormats()) {
                if(sformat.format == VK_FORMAT_DEFAULT_IMAGE && sformat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    m_selected_surface_format = sformat;
                    found_suitable_format = true;
                    break;
                }
            }

            if(!found_suitable_format) {
                WARNME("Could not find suitable surface format! Trying to use first one available!");
                m_selected_surface_format = m_instance_creator.GetSurfaceFormats()[0];
            }


            bool found_presentation_mode = false;
            bool found_vsync = false;
            for(const VkPresentModeKHR presentation_mode : m_instance_creator.GetPresentationModes()) {
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
                        found_vsync = true;
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
                }
            }

            if(!found_presentation_mode) {
                m_selected_present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            } 
        }


        void SwapchainCreator::_CreateSwapchain() {
            uint32_t min_image_count = m_instance_creator.GetSurfaceCapabilities().minImageCount + 1;

            // Verify that the maximum image count is not exceeded
            if(m_instance_creator.GetSurfaceCapabilities().maxImageCount > 0 && min_image_count > m_instance_creator.GetSurfaceCapabilities().maxImageCount) 
                min_image_count = m_instance_creator.GetSurfaceCapabilities().maxImageCount;


            VkSwapchainCreateInfoKHR swapchain_createinfo = {};
            swapchain_createinfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchain_createinfo.surface = m_instance_creator.GetSurface();
            swapchain_createinfo.minImageCount = min_image_count;
            swapchain_createinfo.imageFormat = m_selected_surface_format.format;
            swapchain_createinfo.imageColorSpace = m_selected_surface_format.colorSpace;

            VkExtent2D ext = m_instance_creator.GetSurfaceCapabilities().currentExtent;
            if(ext.width != UINT32_MAX && ext.height != UINT32_MAX)
                swapchain_createinfo.imageExtent = ext;
            else {
                swapchain_createinfo.imageExtent.width = m_window_size.x;
                swapchain_createinfo.imageExtent.height = m_window_size.y;
            }

            ext = { swapchain_createinfo.imageExtent.width, swapchain_createinfo.imageExtent.height };
            swapchain_createinfo.imageArrayLayers = 1;
            swapchain_createinfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            // Check if present queue and graphics queue are the same and necessary synchronise the image sharing mode
            std::array<uint32_t, 2> queue_families;
            queue_families[0] = m_instance_creator.GetGraphicsFamilyIndex();
            queue_families[1] = m_instance_creator.GetPresentationFamilyIndex();
            if(queue_families[0] != queue_families[1]) {
                swapchain_createinfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                swapchain_createinfo.queueFamilyIndexCount = static_cast<uint32_t>(queue_families.size());
                swapchain_createinfo.pQueueFamilyIndices = queue_families.data();
            } else {
                swapchain_createinfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            }
            

            swapchain_createinfo.preTransform = m_instance_creator.GetSurfaceCapabilities().currentTransform;

            // Check, which composite alpha mode is supported
            if(m_instance_creator.GetSurfaceCapabilities().supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) 
                swapchain_createinfo.compositeAlpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
            else if(m_instance_creator.GetSurfaceCapabilities().supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
                swapchain_createinfo.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
            else if(m_instance_creator.GetSurfaceCapabilities().supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
                swapchain_createinfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            else RUN_ERR("_CreateSwapchain", "Could not select correct VK_COMPOSITE_ALPHA_BIT that is supported by DENG");

            swapchain_createinfo.presentMode = m_selected_present_mode;
            swapchain_createinfo.clipped = VK_TRUE;

            if(vkCreateSwapchainKHR(m_instance_creator.GetDevice(), &swapchain_createinfo, nullptr, &m_swapchain) != VK_SUCCESS)
                VK_SWAPCHAIN_ERR("failed to create create a swap chain!");

            uint32_t image_count = UINT32_MAX;
            vkGetSwapchainImagesKHR(m_instance_creator.GetDevice(), m_swapchain, &image_count, nullptr);
            std::vector<VkImage> images(image_count);
            vkGetSwapchainImagesKHR(m_instance_creator.GetDevice(), m_swapchain, &image_count, images.data());

            // copy images to TextureDatabase
            TextureDatabase* db = TextureDatabase::GetInstance();
            if (m_swapchain_image_ids.size() != image_count) {
                m_swapchain_image_ids.resize(image_count);
                for (size_t i = 0; i < images.size(); i++) {
                    Vulkan::TextureData tex_data;
                    tex_data.image = images[i];

                    TextureResource res;
                    res.width = ext.width;
                    res.height = ext.height;
                    res.bit_depth = 4;
                    res.load_type = TEXTURE_RESOURCE_LOAD_TYPE_EMBEDDED;
                    res.resource_type = TEXTURE_RESOURCE_INTERNAL_FRAMEBUFFER_2D_IMAGE;
                    res.vendor = tex_data;

                    m_swapchain_image_ids[i] = db->AddResource(res);
                }
            }
            else {
                for (size_t i = 0; i < images.size(); i++) {
                    Vulkan::TextureData& data = std::get<Vulkan::TextureData>(db->GetResource(m_swapchain_image_ids[i]).vendor);
                    data.image = images[i];
                }
            }
        }


        void SwapchainCreator::_CreateSwapchainImageViews() {
            TextureDatabase* db = TextureDatabase::GetInstance();

            for(uint32_t i = 0; i < static_cast<uint32_t>(m_swapchain_image_ids.size()); i++) {
                Vulkan::TextureData &data = std::get<Vulkan::TextureData>(db->GetResource(m_swapchain_image_ids[i]).vendor);

                VkImageViewCreateInfo image_view_info = {};
                image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                image_view_info.image = data.image;
                image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                image_view_info.format = m_selected_surface_format.format;
                
                image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                image_view_info.subresourceRange.baseMipLevel = 0;
                image_view_info.subresourceRange.levelCount = 1;
                image_view_info.subresourceRange.baseArrayLayer = 0;
                image_view_info.subresourceRange.layerCount = 1;

                if(vkCreateImageView(m_instance_creator.GetDevice(), &image_view_info, nullptr, &data.image_view) != VK_SUCCESS)
                    VK_SWAPCHAIN_ERR("failed to create image views!");
            }
        }
    }
}
