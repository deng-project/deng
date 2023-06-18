// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanFramebuffer.cpp - Vulkan framebuffer management class implementation
// author: Karl-Mihkel Ott

#define VULKAN_FRAMEBUFFER_CPP
#include "deng/VulkanFramebuffer.h"


namespace DENG {
    namespace Vulkan {

        Framebuffer::Framebuffer(
            const InstanceCreator* _pInstanceCreator, 
            VkBuffer& _hMainBuffer, 
            VkSampleCountFlagBits _uSampleCountBits,
            TRS::Point2D<uint32_t> _extent,
            bool _bIsSwapchain) :
            IFramebuffer(_extent.x, _extent.y),
            m_pInstanceCreator(_pInstanceCreator),
            m_hMainBuffer(_hMainBuffer),
            m_uSampleCountBits(_uSampleCountBits)
        {
            try {
                m_hRenderpass = Vulkan::SwapchainCreator::CreateRenderPass(m_pInstanceCreator->GetDevice(), VK_FORMAT_DEFAULT_IMAGE, VK_SAMPLE_COUNT_1_BIT, !_bIsSwapchain);

                if (!_bIsSwapchain) {
                    _CreateFramebufferImage();
                }
                else {
                    m_pSwapchainCreator = new SwapchainCreator(m_pInstanceCreator, m_uWidth, m_uHeight, m_uSampleCountBits);
                }

                _CreateDepthResources();
                _CreateFramebuffers();
                _CreateCommandPool();
                _AllocateCommandBuffers();
                _CreateSynchronisationPrimitives();
            }
            catch (const RendererException& e) {
                DISPATCH_ERROR_MESSAGE("RendererException", e.what(), ErrorSeverity::CRITICAL);
            }
            catch (const HardwareException& e) {
                DISPATCH_ERROR_MESSAGE("HardwareException", e.what(), ErrorSeverity::CRITICAL);
            }
        }


        Framebuffer::~Framebuffer() {
            _DestroyFramebuffer();
            const VkDevice hDevice = m_pInstanceCreator->GetDevice();

            // free commandbuffers and -pools
            vkFreeCommandBuffers(hDevice, m_hCommandPool, static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
            vkDestroyCommandPool(hDevice, m_hCommandPool, nullptr);

            // destroy synchronization primitives
            for (size_t i = 0; i < m_flightFences.size(); i++) {
                if (m_pSwapchainCreator) {
                    vkDestroySemaphore(hDevice, m_renderFinishedSemaphores[i], nullptr);
                    vkDestroySemaphore(hDevice, m_imageAvailableSemaphores[i], nullptr);
                }

                vkDestroyFence(hDevice, m_flightFences[i], nullptr);
            }

            vkDestroyRenderPass(m_pInstanceCreator->GetDevice(), m_hRenderpass, nullptr);

            if (m_pSwapchainCreator) {
                delete m_pSwapchainCreator;
                m_pSwapchainCreator = nullptr;
            }
        }


        void Framebuffer::_CreateDepthResources() {
            const VkDevice hDevice = m_pInstanceCreator->GetDevice();
            const VkPhysicalDevice hPhysicalDevice = m_pInstanceCreator->GetPhysicalDevice();

            VkMemoryRequirements memoryRequirements = Vulkan::_CreateImage(
                hDevice, 
                m_depthImageHandles.hImage,
                m_uWidth,
                m_uHeight,
                1,
                1,
                VK_FORMAT_D32_SFLOAT, 
                VK_IMAGE_TILING_OPTIMAL, 
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
                m_uSampleCountBits,
                0);

            Vulkan::_AllocateMemory(
                hDevice, 
                hPhysicalDevice, 
                memoryRequirements.size, 
                m_depthImageHandles.hMemory, 
                memoryRequirements.memoryTypeBits, 
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            vkBindImageMemory(hDevice, m_depthImageHandles.hImage, m_depthImageHandles.hMemory, 0);

            // create image view info
            VkImageViewCreateInfo imageViewCreateInfo = {};
            imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.image = m_depthImageHandles.hImage;
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            imageViewCreateInfo.format = VK_FORMAT_D32_SFLOAT;

            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            imageViewCreateInfo.subresourceRange.levelCount = 1;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageViewCreateInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(hDevice, &imageViewCreateInfo, NULL, &m_depthImageHandles.hImageView) != VK_SUCCESS)
                throw RendererException("vkCreateImageView() could not create depth image view");
        }


        void Framebuffer::_CreateFramebuffers() {
            if (m_pSwapchainCreator)
                m_framebuffers.resize(m_pSwapchainCreator->GetSwapchainImageHandles().size());
            else m_framebuffers.resize(1);

            std::array<VkImageView, 2> imageAttachments = {};

            for(size_t i = 0; i < m_framebuffers.size(); i++) {
                Vulkan::TextureData framebufferImageData;
                if (m_pSwapchainCreator)
                    framebufferImageData = m_pSwapchainCreator->GetSwapchainImageHandles()[i];
                else framebufferImageData = m_framebufferImageHandles;
                
                imageAttachments = { framebufferImageData.hImageView, m_depthImageHandles.hImageView };
                
                VkFramebufferCreateInfo framebufferCreateInfo = {};
                framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferCreateInfo.renderPass = m_hRenderpass;
                framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(imageAttachments.size());
                framebufferCreateInfo.pAttachments = imageAttachments.data();
                framebufferCreateInfo.width = m_uWidth;
                framebufferCreateInfo.height = m_uHeight;
                framebufferCreateInfo.layers = 1;

                if(vkCreateFramebuffer(m_pInstanceCreator->GetDevice(), &framebufferCreateInfo, NULL, &m_framebuffers[i]) != VK_SUCCESS) {
                    VK_RES_ERR("failed to create framebuffer!");
                } else {
                    std::stringstream ss;
                    ss << "Framebuffer 0x" << std::setfill('0') << std::setw(sizeof(this)) << std::hex << this <<
                        " successfully created";
                    LOG(ss.str());
                }
            }
        }


        void Framebuffer::_CreateFramebufferImage() {
            VkMemoryRequirements memoryRequirements = Vulkan::_CreateImage(
                m_pInstanceCreator->GetDevice(),
                m_framebufferImageHandles.hImage,
                m_uWidth,
                m_uHeight,
                1,
                1,
                VK_FORMAT_DEFAULT_IMAGE,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_SAMPLE_COUNT_1_BIT,
                0);

            Vulkan::_AllocateMemory(
                m_pInstanceCreator->GetDevice(),
                m_pInstanceCreator->GetPhysicalDevice(),
                memoryRequirements.size,
                m_framebufferImageHandles.hMemory,
                memoryRequirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            vkBindImageMemory(m_pInstanceCreator->GetDevice(), m_framebufferImageHandles.hImage, m_framebufferImageHandles.hMemory, 0);

            // create image view
            VkImageViewCreateInfo imageViewCreateInfo = Vulkan::_GetImageViewInfo(m_framebufferImageHandles.hImage, VK_FORMAT_DEFAULT_IMAGE, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1);
            if (vkCreateImageView(m_pInstanceCreator->GetDevice(), &imageViewCreateInfo, nullptr, &m_framebufferImageHandles.hImageView) != VK_SUCCESS)
                throw RendererException("vkCreateImageView() could not create a Vulkan image view for custom framebuffer");

            // image sampler
            m_framebufferImageHandles.hSampler = Vulkan::_CreateTextureSampler(
                m_pInstanceCreator->GetDevice(), 
                m_pInstanceCreator->GetPhysicalDeviceInformation().fMaxSamplerAnisotropy, 
                1);
        }


        void Framebuffer::_CreateCommandPool() {
            VkCommandPoolCreateInfo commandPoolCreateInfo = {};
            commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            commandPoolCreateInfo.queueFamilyIndex = m_pInstanceCreator->GetGraphicsFamilyIndex();
            commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

            if (vkCreateCommandPool(m_pInstanceCreator->GetDevice(), &commandPoolCreateInfo, nullptr, &m_hCommandPool) != VK_SUCCESS) {
                std::stringstream ss;
                ss << "vkCreateCommandPool() could not create a command pool for framebuffer 0x" <<
                    std::setfill('0') << std::setw(sizeof(this)) << std::hex << this;
                throw RendererException(ss.str());
            }
        }


        void Framebuffer::_CreateSynchronisationPrimitives() {
            // swapchain image acquisition and presentation semaphores
            VkSemaphoreCreateInfo semaphoreCreateInfo = {};
            semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fenceCreateInfo = {};
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            
            if (m_pSwapchainCreator) {
                m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
                m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
                m_flightFences.resize(MAX_FRAMES_IN_FLIGHT);

                for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                    const VkDevice hDevice = m_pInstanceCreator->GetDevice();
                    if (vkCreateSemaphore(hDevice, &semaphoreCreateInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS) {
                        std::stringstream ss;
                        ss << "vkCreateSemaphore() could not create image available semaphores for framebuffer 0x" <<
                            std::setfill('0') << std::setw(sizeof(this)) << std::hex << this;
                        throw RendererException(ss.str());
                    }
                    if (vkCreateSemaphore(hDevice, &semaphoreCreateInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS) {
                        std::stringstream ss;
                        ss << "vkCreateSemaphore() could not create render finished semaphores for framebuffer 0x" <<
                            std::setfill('0') << std::setw(sizeof(this)) << std::hex << this;
                        throw RendererException(ss.str());
                    }
                    if (vkCreateFence(hDevice, &fenceCreateInfo, nullptr, &m_flightFences[i]) != VK_SUCCESS) {
                        std::stringstream ss;
                        ss << "vkCreateFence() could not create a frame flight fence for framebuffer 0x" <<
                            std::setfill('0') << std::setw(sizeof(this)) << std::hex << this;
                        throw RendererException(ss.str());
                    }
                }
            }
            else {
                m_flightFences.resize(1);

                if (vkCreateFence(m_pInstanceCreator->GetDevice(), &fenceCreateInfo, nullptr, &m_flightFences[0]) != VK_SUCCESS) {
                    std::stringstream ss;
                    ss << "vkCreateFence() could not create a frame flight fence for framebuffer 0x" <<
                        std::setfill('0') << std::setw(sizeof(this)) << std::hex << this;
                    throw RendererException(ss.str());
                }
            }

            LOG("Successfully created synchronisation primitives");
        }


        void Framebuffer::_AllocateCommandBuffers() {
            m_commandBuffers.resize(m_framebuffers.size());

            // Set up command buffer allocation info
            VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
            commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            commandBufferAllocateInfo.commandPool = m_hCommandPool;
            commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

            // Allocate command buffers
            if (vkAllocateCommandBuffers(m_pInstanceCreator->GetDevice(), &commandBufferAllocateInfo, m_commandBuffers.data()) != VK_SUCCESS) {
                std::stringstream ss;
                ss << "vkAllocateCommandBuffers() could not create command buffers for framebuffer 0x" <<
                    std::setfill('0') << std::setw(sizeof(this)) << std::hex << this;
                throw RendererException(ss.str());
            }
        }

        void Framebuffer::_RecreateSwapchain() {
            vkDeviceWaitIdle(m_pInstanceCreator->GetDevice());
        }


        void Framebuffer::_DestroyFramebuffer() {
            // destroy depth image resources
            vkDestroyImageView(m_pInstanceCreator->GetDevice(), m_depthImageHandles.hImageView, nullptr);
            vkDestroyImage(m_pInstanceCreator->GetDevice(), m_depthImageHandles.hImage, nullptr);
            vkFreeMemory(m_pInstanceCreator->GetDevice(), m_depthImageHandles.hMemory, nullptr);

            // reset command pool, destroy framebuffers and pipelines
            vkResetCommandPool(m_pInstanceCreator->GetDevice(), m_hCommandPool, 0);
            for (auto& fb : m_framebuffers)
                vkDestroyFramebuffer(m_pInstanceCreator->GetDevice(), fb, nullptr);

            // destroy framebuffer images if possible
            if (!m_pSwapchainCreator) {
                for (size_t i = 0; i < m_framebuffers.size(); i++) {
                    vkDestroySampler(m_pInstanceCreator->GetDevice(), m_framebufferImageHandles.hSampler, nullptr);
                    vkDestroyImageView(m_pInstanceCreator->GetDevice(), m_framebufferImageHandles.hImageView, nullptr);
                    vkDestroyImage(m_pInstanceCreator->GetDevice(), m_framebufferImageHandles.hImage, nullptr);
                    vkFreeMemory(m_pInstanceCreator->GetDevice(), m_framebufferImageHandles.hMemory, nullptr);
                }
            }
        }


        void Framebuffer::BeginCommandBufferRecording(TRS::Vector4<float> _vClearColor) {
            if (m_pSwapchainCreator) {
                vkWaitForFences(m_pInstanceCreator->GetDevice(), 1, &m_flightFences[m_uCurrentFrameIndex], VK_TRUE, UINT64_MAX);
                vkResetFences(m_pInstanceCreator->GetDevice(), 1, &m_flightFences[m_uCurrentFrameIndex]);

                vkAcquireNextImageKHR(
                    m_pInstanceCreator->GetDevice(), 
                    m_pSwapchainCreator->GetSwapchain(), 
                    UINT64_MAX, 
                    m_imageAvailableSemaphores[m_uCurrentFrameIndex], 
                    VK_NULL_HANDLE,
                    &m_uCurrentSwapchainImageIndex);
            }

            if (vkResetCommandBuffer(m_commandBuffers[m_uCurrentFrameIndex], 0) != VK_SUCCESS) {
                throw RendererException("vkResetCommandBuffer() could not reset a command buffer");
            }
            
            VkCommandBufferBeginInfo commandBufferBeginInfo = {};
            commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(m_commandBuffers[m_uCurrentFrameIndex], &commandBufferBeginInfo) != VK_SUCCESS)
                throw RendererException("vkBeginCommandBuffer() could not begin command buffer recording");

            VkRenderPassBeginInfo renderPassBeginInfo = {};
            renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassBeginInfo.renderPass = m_hRenderpass;
            renderPassBeginInfo.framebuffer = m_framebuffers[m_uCurrentSwapchainImageIndex];
            renderPassBeginInfo.renderArea.offset = { 0, 0 };
            renderPassBeginInfo.renderArea.extent = { m_uWidth, m_uHeight };

            std::array<VkClearValue, 2> clearValues;
            clearValues[0].color = { { 
                _vClearColor.first,
                _vClearColor.second,
                _vClearColor.third,
                _vClearColor.fourth
            } };
            clearValues[1].depthStencil = { 1.f, 0 };

            renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassBeginInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(m_commandBuffers[m_uCurrentFrameIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        }


        void Framebuffer::Draw(
            hash_t _hshMesh, 
            hash_t _hshShader, 
            uint32_t _uInstanceCount,
            uint32_t _uFirstInstance,
            VkDescriptorSet _hShaderDescriptorSet, 
            VkDescriptorSet _hMaterialDescriptorSet,
            VkDescriptorSetLayout _hShaderDescriptorSetLayout,
            VkDescriptorSetLayout _hMaterialDescriptorSetLayout) 
        {
            ResourceManager& resourceManager = ResourceManager::GetInstance();
            const IShader* pShader = resourceManager.GetShader(_hshShader);
            const MeshCommands* pMesh = resourceManager.GetMesh(_hshMesh);

            // check if pipeline creator needs to be created
            if (m_pipelineCreators.find(_hshShader) == m_pipelineCreators.end()) {
                m_pipelineCreators.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(_hshShader),
                    std::forward_as_tuple(
                        m_pInstanceCreator->GetDevice(),
                        m_hRenderpass,
                        _hShaderDescriptorSetLayout,
                        _hMaterialDescriptorSetLayout,
                        m_uSampleCountBits,
                        m_pInstanceCreator->GetPhysicalDeviceInformation(),
                        pShader
                    ));
            }

            // check if custom viewport should be used
            if (pShader->IsPropertySet(ShaderPropertyBit_EnableCustomViewport)) {
                VkViewport viewport = {};
                viewport.x = static_cast<float>(pShader->GetViewport().uX);
                viewport.y = static_cast<float>(pShader->GetViewport().uY);
                viewport.width = static_cast<float>(pShader->GetViewport().uWidth);
                viewport.height = static_cast<float>(pShader->GetViewport().uHeight);
                viewport.minDepth = 0.f;
                viewport.maxDepth = 1.f;
                vkCmdSetViewport(m_commandBuffers[m_uCurrentFrameIndex], 0, 1, &viewport);
            }
            else {
                VkViewport viewport = {};
                viewport.x = 0;
                viewport.y = static_cast<float>(m_uHeight);
                viewport.width = static_cast<float>(m_uWidth);
                viewport.height = -static_cast<float>(m_uHeight);
                viewport.minDepth = 0.f;
                viewport.maxDepth = 1.f;
                vkCmdSetViewport(m_commandBuffers[m_uCurrentFrameIndex], 0, 1, &viewport);
            }

            std::array<VkDescriptorSet, 2> descriptorSets = { _hShaderDescriptorSet, _hMaterialDescriptorSet };
            
            // submit each draw command in mesh
            for (auto itCmd = pMesh->drawCommands.begin(); itCmd != pMesh->drawCommands.end(); itCmd++) {
                auto itPipelineCreator = m_pipelineCreators.find(_hshShader);
                vkCmdBindPipeline(
                    m_commandBuffers[m_uCurrentFrameIndex], 
                    VK_PIPELINE_BIND_POINT_GRAPHICS, 
                    itPipelineCreator->second.GetPipeline());

                std::vector<VkBuffer> buffers(pShader->GetAttributeTypes().size(), m_hMainBuffer);
                
                vkCmdBindVertexBuffers(
                    m_commandBuffers[m_uCurrentFrameIndex],
                    0,
                    static_cast<uint32_t>(buffers.size()),
                    buffers.data(),
                    itCmd->attributeOffsets.data());

                // bind both descriptor sets
                if (descriptorSets[0] && descriptorSets[1]) {
                    vkCmdBindDescriptorSets(
                        m_commandBuffers[m_uCurrentFrameIndex],
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        itPipelineCreator->second.GetPipelineLayout(),
                        0,
                        2,
                        descriptorSets.data(),
                        0,
                        nullptr);
                }
                else if (descriptorSets[0]) {
                    vkCmdBindDescriptorSets(
                        m_commandBuffers[m_uCurrentFrameIndex],
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        itPipelineCreator->second.GetPipelineLayout(),
                        0,
                        1,
                        &descriptorSets[0],
                        0,
                        nullptr);
                }
                else if (descriptorSets[1]) {
                    vkCmdBindDescriptorSets(
                        m_commandBuffers[m_uCurrentFrameIndex],
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        itPipelineCreator->second.GetPipelineLayout(),
                        0,
                        1,
                        &descriptorSets[1],
                        0,
                        nullptr);
                }
                    
                // check if scissor technique should be used
                if (pShader->IsPropertySet(ShaderPropertyBit_EnableScissor)) {
                    VkRect2D rect = {};
                    rect.offset = VkOffset2D { 
                        itCmd->scissor.offset.x, 
                        itCmd->scissor.offset.y 
                    };

                    rect.extent = VkExtent2D{ 
                        itCmd->scissor.extent.x, 
                        itCmd->scissor.extent.y 
                    };

                    vkCmdSetScissor(m_commandBuffers[m_uCurrentFrameIndex], 0, 1, &rect);
                }
                else {
                    VkRect2D rect = {};
                    rect.offset = { 0, 0 };
                    rect.extent = { m_uWidth, m_uHeight };
                    vkCmdSetScissor(m_commandBuffers[m_uCurrentFrameIndex], 0, 1, &rect);
                }

                // check if push constants should be bound
                if (pShader->IsPropertySet(ShaderPropertyBit_EnablePushConstants)) {
                    VkShaderStageFlags bShaderStage = (VkShaderStageFlags)0;

                    if (pShader->GetPushConstant().bmShaderStage & ShaderStageBit_Vertex)
                        bShaderStage |= VK_SHADER_STAGE_VERTEX_BIT;
                    if (pShader->GetPushConstant().bmShaderStage & ShaderStageBit_Fragment)
                        bShaderStage |= VK_SHADER_STAGE_FRAGMENT_BIT;
                    if (pShader->GetPushConstant().bmShaderStage & ShaderStageBit_Geometry)
                        bShaderStage |= VK_SHADER_STAGE_GEOMETRY_BIT;

                    vkCmdPushConstants(
                        m_commandBuffers[m_uCurrentFrameIndex],
                        itPipelineCreator->second.GetPipelineLayout(),
                        bShaderStage,
                        0,
                        pShader->GetPushConstant().uLength,
                        pShader->GetPushConstant().pPushConstantData);
                }

                // check if indexed or unindexed draw call should be submitted
                if (pShader->IsPropertySet(ShaderPropertyBit_EnableIndexing)) {
                    vkCmdBindIndexBuffer(m_commandBuffers[m_uCurrentFrameIndex], m_hMainBuffer, static_cast<VkDeviceSize>(itCmd->uIndicesOffset), VK_INDEX_TYPE_UINT32);
                    vkCmdDrawIndexed(m_commandBuffers[m_uCurrentFrameIndex], itCmd->uDrawCount, _uInstanceCount, _uFirstInstance, 0, 0);
                } else {
                    vkCmdDraw(m_commandBuffers[m_uCurrentFrameIndex], itCmd->uDrawCount, _uInstanceCount, _uFirstInstance, 0);
                }
           }
        }


        void Framebuffer::EndCommandBufferRecording() {
            vkCmdEndRenderPass(m_commandBuffers[m_uCurrentFrameIndex]);
            if (vkEndCommandBuffer(m_commandBuffers[m_uCurrentFrameIndex]) != VK_SUCCESS)
                throw RendererException("vkEndCommandBuffer() could not end command buffer recording");
        }


        void Framebuffer::RenderToFramebuffer() {
            VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            if (m_pSwapchainCreator) {
                submitInfo.waitSemaphoreCount = 1;
                submitInfo.pWaitSemaphores = &m_imageAvailableSemaphores[m_uCurrentFrameIndex];
                submitInfo.pWaitDstStageMask = wait_stages;
                submitInfo.signalSemaphoreCount = 1;
                submitInfo.pSignalSemaphores = &m_renderFinishedSemaphores[m_uCurrentFrameIndex];
            }
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &m_commandBuffers[m_uCurrentFrameIndex];

            // submit the graphics queue
            if (vkQueueSubmit(m_pInstanceCreator->GetGraphicsQueue(), 1, &submitInfo, m_flightFences[m_uCurrentFrameIndex]) != VK_SUCCESS) {
                throw RendererException("vkQueueSubmit() failed to submit a command buffer to render queue");
            }

            if (m_pSwapchainCreator) {
                VkPresentInfoKHR presentInfo = {};
                presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                presentInfo.waitSemaphoreCount = 1;
                presentInfo.pWaitSemaphores = &m_renderFinishedSemaphores[m_uCurrentFrameIndex];
                presentInfo.swapchainCount = 1;
                presentInfo.pImageIndices = &m_uCurrentSwapchainImageIndex;

                VkSwapchainKHR hSwapchain = m_pSwapchainCreator->GetSwapchain();
                presentInfo.pSwapchains = &hSwapchain;

                m_uCurrentFrameIndex = (m_uCurrentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
                
                VkResult eResult = vkQueuePresentKHR(m_pInstanceCreator->GetPresentationQueue(), &presentInfo);
            
                if (eResult == VK_ERROR_OUT_OF_DATE_KHR || eResult == VK_SUBOPTIMAL_KHR)
                    _RecreateSwapchain();
                else if (eResult != VK_SUCCESS) {
                    throw RendererException("vkQueuePresentKHR() queue presentation failed");
                }
            }
        }

        void Framebuffer::RecreateFramebuffer(uint32_t _uWidth, uint32_t _uHeight) {
            m_uWidth = _uWidth;
            m_uHeight = _uHeight;
            
            // destroy previous resources
            _DestroyFramebuffer();
            _CreateDepthResources();

            if (!m_pSwapchainCreator) {
                _CreateFramebufferImage();
            }

            if (!m_pSwapchainCreator) {
                _CreateFramebufferImage();
            }
            else {
                m_pSwapchainCreator->RecreateSwapchain(m_uWidth, m_uHeight);
                const std::vector<Vulkan::TextureData>& swapchainImageResources =
                    m_pSwapchainCreator->GetSwapchainImageHandles();

                DENG_ASSERT(swapchainImageResources.size() == m_framebuffers.size());
            }

            _CreateFramebuffers();
        }
    }
}
