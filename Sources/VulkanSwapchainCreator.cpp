// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanSwapchainCreator.h - Vulkan swapchain creator class implementation
// author: Karl-Mihkel Ott

#define VULKAN_SWAPCHAIN_CREATOR_CPP
#include "deng/VulkanSwapchainCreator.h"

namespace DENG {
    namespace Vulkan {
        SwapchainCreator::SwapchainCreator(const InstanceCreator* _pInstanceCreator, uint32_t _uWidth, uint32_t _uHeight, VkSampleCountFlagBits _uSampleCountBits) : 
            m_pInstanceCreator(_pInstanceCreator), 
            m_uSampleCountBits(_uSampleCountBits)
        {
            DENG_ASSERT(m_pInstanceCreator);
            try {
                _ConfigureSwapchainSettings();
                _CreateSwapchain(_uWidth, _uHeight);
                _CreateSwapchainImageViews();
            }
            catch (const RendererException& e) {
                DISPATCH_ERROR_MESSAGE("RendererException", e.what(), ErrorSeverity::CRITICAL);
            }
            catch (const HardwareException& e) {
                DISPATCH_ERROR_MESSAGE("HardwareException", e.what(), ErrorSeverity::CRITICAL);
            }
        }


        SwapchainCreator::~SwapchainCreator() {
            for (auto it = m_swapchainImages.begin(); it != m_swapchainImages.end(); it++) {
                vkDestroyImageView(m_pInstanceCreator->GetDevice(), it->hImageView, nullptr);
            }

            vkDestroySwapchainKHR(m_pInstanceCreator->GetDevice(), m_hSwapchain, nullptr);
        }


        VkRenderPass SwapchainCreator::CreateRenderPass(VkDevice _hDevice, VkFormat _eFormat, VkSampleCountFlagBits _uMSAASamples, bool _bUseNonDefaultFramebuffer) {
            std::array<VkAttachmentDescription, 2> attachments = {};
            // color attachment
            attachments[0].format = _eFormat;
            attachments[0].samples = _uMSAASamples;
            attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            if(!_bUseNonDefaultFramebuffer) {
                attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            } else {
                attachments[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }

            // depth attachment
            attachments[1].format = VK_FORMAT_D32_SFLOAT;
            attachments[1].samples = _uMSAASamples;
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

            VkSubpassDescription subpassDescription = {};
            subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassDescription.colorAttachmentCount = 1;
            subpassDescription.pColorAttachments = &references[0];
            subpassDescription.pDepthStencilAttachment = &references[1];
            //subpassDescription.pResolveAttachments = &references[2];

            std::array<VkSubpassDependency, 2> subpassDependencies = {};
            if(_bUseNonDefaultFramebuffer) {
                // transform the image into VK_PIPELINE_STAGE_COLOR_OUTPUT_ATTACHMENT_BIT since the original stage is VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
                subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
                subpassDependencies[0].dstSubpass = 0;
                subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                subpassDependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

                // transform the image back VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT so it could be used as a texture
                subpassDependencies[1].srcSubpass = 0;
                subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
                subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                subpassDependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            } else {
                subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
                subpassDependencies[0].dstSubpass = 0;
                subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                subpassDependencies[0].srcAccessMask = 0;
                subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            }

            VkRenderPassCreateInfo renderPassCreateInfo = {};
            renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            renderPassCreateInfo.pAttachments = attachments.data();
            renderPassCreateInfo.subpassCount = 1;
            renderPassCreateInfo.pSubpasses = &subpassDescription;
            if(_bUseNonDefaultFramebuffer) 
                renderPassCreateInfo.dependencyCount = 2;
            else renderPassCreateInfo.dependencyCount = 0;
            renderPassCreateInfo.pDependencies = subpassDependencies.data();

            VkRenderPass renderpass = VK_NULL_HANDLE;
            if (vkCreateRenderPass(_hDevice, &renderPassCreateInfo, nullptr, &renderpass) != VK_SUCCESS)
                throw RendererException("vkCreateRenderPass() could not create a render pass");

            return renderpass;
        }


        void SwapchainCreator::RecreateSwapchain(uint32_t _uWidth, uint32_t _uHeight) {
            // cleanup the previous swapchain
            for (auto it = m_swapchainImages.begin(); it != m_swapchainImages.end(); it++) {
                vkDestroyImageView(m_pInstanceCreator->GetDevice(), it->hImageView, nullptr);
            }

            vkDestroySwapchainKHR(m_pInstanceCreator->GetDevice(), m_hSwapchain, nullptr);

            _CreateSwapchain(_uWidth, _uHeight);
            _CreateSwapchainImageViews();
        }


        void SwapchainCreator::_ConfigureSwapchainSettings() {
            bool bFoundSuitableFormat = false;

            // check if found surface formats support SRGB coloring and nonlinear color space
            for(const VkSurfaceFormatKHR &surfaceFormat : m_pInstanceCreator->GetSurfaceFormats()) {
                if(surfaceFormat.format == VK_FORMAT_DEFAULT_IMAGE && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    m_selectedSurfaceFormat = surfaceFormat;
                    bFoundSuitableFormat = true;
                    break;
                }
            }

            if(!bFoundSuitableFormat) {
                WARNME("Could not find suitable surface format! Trying to use first one available");
                if (!m_pInstanceCreator->GetSurfaceFormats().size())
                    throw HardwareException("Physical device does not support any Vulkan surface formats");
                
                m_selectedSurfaceFormat = m_pInstanceCreator->GetSurfaceFormats()[0];
            }


            bool bFoundPresentationMode = false;
            bool bFoundVsync = false;
            for(const VkPresentModeKHR ePresentMode : m_pInstanceCreator->GetPresentationModes()) {
                // Check which present modes are available
                switch (ePresentMode) {
                    case VK_PRESENT_MODE_IMMEDIATE_KHR:
                        LOG("VK_PRESENT_MODE_IMMEDIATE_KHR is available!");
                        break;

                    case VK_PRESENT_MODE_MAILBOX_KHR:
                        LOG("VK_PRESENT_MODE_MAILBOX_KHR is available!");
                        m_eSelectedPresentMode = ePresentMode;
                        bFoundPresentationMode = true;
                        break;

                    case VK_PRESENT_MODE_FIFO_KHR:
                        LOG("VK_PRESENT_MODE_FIFO_KHR is available!");
                        bFoundVsync = true;
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
            }

            if(!bFoundPresentationMode) {
                m_eSelectedPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            } 
        }


        void SwapchainCreator::_CreateSwapchain(uint32_t _uWidth, uint32_t _uHeight) {
            uint32_t uMinimumImageCount = m_pInstanceCreator->GetSurfaceCapabilities().minImageCount + 1;

            // Verify that the maximum image count is not exceeded
            if(m_pInstanceCreator->GetSurfaceCapabilities().maxImageCount > 0 && uMinimumImageCount > m_pInstanceCreator->GetSurfaceCapabilities().maxImageCount) 
                uMinimumImageCount = m_pInstanceCreator->GetSurfaceCapabilities().maxImageCount;


            VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
            swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchainCreateInfo.surface = m_pInstanceCreator->GetSurface();
            swapchainCreateInfo.minImageCount = uMinimumImageCount;
            swapchainCreateInfo.imageFormat = m_selectedSurfaceFormat.format;
            swapchainCreateInfo.imageColorSpace = m_selectedSurfaceFormat.colorSpace;

            swapchainCreateInfo.imageExtent.width = _uWidth;
            swapchainCreateInfo.imageExtent.height = _uHeight;

            swapchainCreateInfo.imageArrayLayers = 1;
            swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            // Check if present queue and graphics queue are the same and necessary synchronise the image sharing mode
            std::array<uint32_t, 2> queueFamilies;
            queueFamilies[0] = m_pInstanceCreator->GetGraphicsFamilyIndex();
            queueFamilies[1] = m_pInstanceCreator->GetPresentationFamilyIndex();
            if(queueFamilies[0] != queueFamilies[1]) {
                swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilies.size());
                swapchainCreateInfo.pQueueFamilyIndices = queueFamilies.data();
            } else {
                swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            }
            

            swapchainCreateInfo.preTransform = m_pInstanceCreator->GetSurfaceCapabilities().currentTransform;

            // Check, which composite alpha mode is supported
            if(m_pInstanceCreator->GetSurfaceCapabilities().supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) 
                swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
            else if(m_pInstanceCreator->GetSurfaceCapabilities().supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
                swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
            else if(m_pInstanceCreator->GetSurfaceCapabilities().supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
                swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            else {
                throw HardwareException("Could not select valid VkCompositeAlphaFlagBitsKHR supported by DENG");
            }

            swapchainCreateInfo.presentMode = m_eSelectedPresentMode;
            swapchainCreateInfo.clipped = VK_TRUE;

            if (vkCreateSwapchainKHR(m_pInstanceCreator->GetDevice(), &swapchainCreateInfo, nullptr, &m_hSwapchain) != VK_SUCCESS)
                throw RendererException("vkCreateSwapchainKHR() could not create a swapchain");

            uint32_t uImageCount = 0;
            vkGetSwapchainImagesKHR(m_pInstanceCreator->GetDevice(), m_hSwapchain, &uImageCount, nullptr);
            std::vector<VkImage> images(uImageCount);
            if (vkGetSwapchainImagesKHR(m_pInstanceCreator->GetDevice(), m_hSwapchain, &uImageCount, images.data()) != VK_SUCCESS)
                throw RendererException("vkGetSwapchainImagesKHR() could not get swapchain images");

            m_swapchainImages.resize(uImageCount);
            for (size_t i = 0; i < m_swapchainImages.size(); i++) {
                m_swapchainImages[i].hImage = images[i];
            }
        }


        void SwapchainCreator::_CreateSwapchainImageViews() {
            for(size_t i = 0; i < m_swapchainImages.size(); i++) {
                VkImageViewCreateInfo imageViewCreateInfo = {};
                imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                imageViewCreateInfo.image = m_swapchainImages[i].hImage;
                imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                imageViewCreateInfo.format = m_selectedSurfaceFormat.format;
                
                imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
                imageViewCreateInfo.subresourceRange.levelCount = 1;
                imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
                imageViewCreateInfo.subresourceRange.layerCount = 1;

                if (vkCreateImageView(m_pInstanceCreator->GetDevice(), &imageViewCreateInfo, nullptr, &m_swapchainImages[i].hImageView) != VK_SUCCESS)
                    throw RendererException("vkCreateImageView() could not create swapchain image view");
            }
        }
    }
}
