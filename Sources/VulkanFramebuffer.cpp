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
            std::unordered_map<uint32_t, TextureResource>& _textureRegistry,
            uint32_t(*_pfnGetNewImageId)(),
            bool _bIsSwapchain) :
            IFramebuffer(_extent.x, _extent.y),
            m_pInstanceCreator(_pInstanceCreator),
            m_textureRegistry(_textureRegistry),
            m_pfnGetNewImageId(_pfnGetNewImageId),
            m_hMainBuffer(_hMainBuffer),
            m_uSampleCountBits(_uSampleCountBits)
        {
            DENG_ASSERT(m_pfnGetNewImageId);

            try {
                m_hRenderpass = Vulkan::SwapchainCreator::CreateRenderPass(m_pInstanceCreator->GetDevice(), VK_FORMAT_DEFAULT_IMAGE, VK_SAMPLE_COUNT_1_BIT, !_bIsSwapchain);

                if (!_bIsSwapchain) {
                    _CreateFramebufferImage();
                }
                else {
                    m_pSwapchainCreator = new SwapchainCreator(m_pInstanceCreator, m_uWidth, m_uHeight, m_uSampleCountBits);
                    const std::vector<Vulkan::TextureData>& swapchainImageResources =
                        m_pSwapchainCreator->GetSwapchainImageHandles();

                    for (auto it = swapchainImageResources.begin(); it != swapchainImageResources.end(); it++) {
                        TextureResource textureResource;
                        textureResource.uWidth = m_uWidth;
                        textureResource.uHeight = m_uHeight;
                        textureResource.uBitDepth = 4;
                        textureResource.eLoadType = TEXTURE_RESOURCE_LOAD_TYPE_EMBEDDED;
                        textureResource.eResourceType = TEXTURE_RESOURCE_INTERNAL_FRAMEBUFFER_2D_IMAGE;
                        textureResource.apiTextureHandles = *it;

                        m_framebufferImageIds.push_back(m_pfnGetNewImageId());
                        m_textureRegistry[m_framebufferImageIds.back()] = textureResource;
                    }
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
            _DestroyFramebuffer(true);
            const VkDevice hDevice = m_pInstanceCreator->GetDevice();

            // free commandbuffers and -pools
            vkFreeCommandBuffers(hDevice, m_hCommandPool, static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
            vkDestroyCommandPool(hDevice, m_hCommandPool, nullptr);

            // destroy synchronization primitives
            for (size_t i = 0; i < m_framebufferImageIds.size(); i++) {
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

            Vulkan::TextureData vkTextureData = {};
            TextureResource textureResource;
            textureResource.uWidth = m_uWidth;
            textureResource.uHeight = m_uHeight;
            textureResource.uBitDepth = 1;
            textureResource.eLoadType = TEXTURE_RESOURCE_LOAD_TYPE_EMBEDDED;
            textureResource.eResourceType = TEXTURE_RESOURCE_INTERNAL_FRAMEBUFFER_2D_IMAGE;
            
            VkMemoryRequirements memoryRequirements = Vulkan::_CreateImage(
                hDevice, 
                vkTextureData.hImage,
                m_uWidth, 
                m_uHeight, 
                1,
                1,
                VK_FORMAT_D32_SFLOAT, 
                VK_IMAGE_TILING_OPTIMAL, 
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
                m_uSampleCountBits,
                0);

            Vulkan::_AllocateMemory(hDevice, hPhysicalDevice, memoryRequirements.size, vkTextureData.hMemory, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            vkBindImageMemory(hDevice, vkTextureData.hImage, vkTextureData.hMemory, 0);

            // create image view info
            VkImageViewCreateInfo imageViewCreateInfo = {};
            imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.image = vkTextureData.hImage;
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            imageViewCreateInfo.format = VK_FORMAT_D32_SFLOAT;

            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            imageViewCreateInfo.subresourceRange.levelCount = 1;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageViewCreateInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(hDevice, &imageViewCreateInfo, NULL, &vkTextureData.hImageView) != VK_SUCCESS)
                throw RendererException("vkCreateImageView() could not create depth image view");
            
            textureResource.apiTextureHandles = vkTextureData;

            if (m_textureRegistry.find(m_uDepthImageId) == m_textureRegistry.end())
                m_uDepthImageId = m_pfnGetNewImageId();

            m_textureRegistry[m_uDepthImageId] = textureResource;
        }


        void Framebuffer::_CreateFramebuffers() {
            m_framebuffers.resize(m_framebufferImageIds.size());
            std::array<VkImageView, 2> imageAttachments = {};

            Vulkan::TextureData vkDepthImageData = std::get<0>(m_textureRegistry[m_uDepthImageId].apiTextureHandles);

            for(size_t i = 0; i < m_framebufferImageIds.size(); i++) {
                Vulkan::TextureData vkFramebufferImageData = std::get<0>(m_textureRegistry[m_framebufferImageIds[i]].apiTextureHandles);
                
                imageAttachments = { vkFramebufferImageData.hImageView, vkDepthImageData.hImageView };
                
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
            // create a framebuffer image
            TextureResource textureResource;
            textureResource.uWidth = m_uWidth;
            textureResource.uHeight = m_uHeight;
            textureResource.uBitDepth = 4;
            textureResource.eLoadType = TEXTURE_RESOURCE_LOAD_TYPE_EMBEDDED;
            textureResource.eResourceType = TEXTURE_RESOURCE_INTERNAL_FRAMEBUFFER_2D_IMAGE;

            Vulkan::TextureData vkTextureData = {};

            VkMemoryRequirements memoryRequirements = Vulkan::_CreateImage(
                m_pInstanceCreator->GetDevice(),
                vkTextureData.hImage,
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
                vkTextureData.hMemory,
                memoryRequirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            vkBindImageMemory(m_pInstanceCreator->GetDevice(), vkTextureData.hImage, vkTextureData.hMemory, 0);

            // create image view
            VkImageViewCreateInfo imageViewCreateInfo = Vulkan::_GetImageViewInfo(vkTextureData.hImage, VK_FORMAT_DEFAULT_IMAGE, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1);
            if (vkCreateImageView(m_pInstanceCreator->GetDevice(), &imageViewCreateInfo, nullptr, &vkTextureData.hImageView) != VK_SUCCESS)
                throw RendererException("vkCreateImageView() could not create a Vulkan image view for custom framebuffer");

            // image sampler
            vkTextureData.hSampler = Vulkan::_CreateTextureSampler(
                m_pInstanceCreator->GetDevice(), 
                m_pInstanceCreator->GetPhysicalDeviceInformation().fMaxSamplerAnisotropy, 
                1);

            textureResource.apiTextureHandles = vkTextureData;
            m_framebufferImageIds.push_back(m_pfnGetNewImageId());
            m_textureRegistry[m_framebufferImageIds.back()] = textureResource;
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
            m_commandBuffers.resize(m_framebufferImageIds.size());

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


        void Framebuffer::_DestroyFramebuffer(bool _bRemoveImageResources) {
            // destroy depth image resources
            DENG_ASSERT(m_textureRegistry.find(m_uDepthImageId) != m_textureRegistry.end());

            Vulkan::TextureData vkImageData = std::get<0>(m_textureRegistry[m_uDepthImageId].apiTextureHandles);
            vkDestroyImageView(m_pInstanceCreator->GetDevice(), vkImageData.hImageView, nullptr);
            vkDestroyImage(m_pInstanceCreator->GetDevice(), vkImageData.hImage, nullptr);
            vkFreeMemory(m_pInstanceCreator->GetDevice(), vkImageData.hMemory, nullptr);

            // reset command pool, destroy framebuffers and pipelines
            vkResetCommandPool(m_pInstanceCreator->GetDevice(), m_hCommandPool, 0);
            for (auto& fb : m_framebuffers)
                vkDestroyFramebuffer(m_pInstanceCreator->GetDevice(), fb, nullptr);
            for (auto& pipelineCreator : m_pipelineCreators)
                pipelineCreator.second.DestroyPipelineData();

            // destroy framebuffer images if possible
            if (!m_pSwapchainCreator) {
                for (size_t i = 0; i < m_framebufferImageIds.size(); i++) {
                    vkImageData = std::get<Vulkan::TextureData>(m_textureRegistry[m_framebufferImageIds[i]].apiTextureHandles);
                    vkDestroySampler(m_pInstanceCreator->GetDevice(), vkImageData.hSampler, nullptr);
                    vkDestroyImageView(m_pInstanceCreator->GetDevice(), vkImageData.hImageView, nullptr);
                    vkDestroyImage(m_pInstanceCreator->GetDevice(), vkImageData.hImage, nullptr);
                    vkFreeMemory(m_pInstanceCreator->GetDevice(), vkImageData.hMemory, nullptr);
                }

                // check if registry removal is required
                if (_bRemoveImageResources) {
                    for (auto it = m_framebufferImageIds.begin(); it != m_framebufferImageIds.end(); it++) {
                        m_textureRegistry.erase(*it);
                    }

                    m_framebufferImageIds.clear();
                }
            }

            if (_bRemoveImageResources) {
                m_textureRegistry.erase(m_uDepthImageId);
                m_uDepthImageId = UINT32_MAX;
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


        void Framebuffer::Draw(const MeshComponent& _mesh, const ShaderComponent& _shader, DescriptorAllocator* _pDescriptorAllocator, const std::vector<uint32_t>& _textureIds) {
            // check if pipeline creator exists for requested mesh
            if (m_pipelineCreators.find(_shader.pShader) == m_pipelineCreators.end()) {
                if (_pDescriptorAllocator) {
                    m_pipelineCreators.emplace(
                        std::piecewise_construct,
                        std::forward_as_tuple(_shader.pShader),
                        std::forward_as_tuple(
                            m_pInstanceCreator->GetDevice(),
                            m_hRenderpass,
                            _pDescriptorAllocator->GetShaderDescriptorSetLayout(),
                            _pDescriptorAllocator->GetMeshDescriptorSetLayout(),
                            VkExtent2D{ m_uWidth, m_uHeight },
                            m_uSampleCountBits,
                            m_pInstanceCreator->GetPhysicalDeviceInformation(),
                            _shader));
                }
                else {
                    m_pipelineCreators.emplace(
                        std::piecewise_construct,
                        std::forward_as_tuple(_shader.pShader),
                        std::forward_as_tuple(
                            m_pInstanceCreator->GetDevice(),
                            m_hRenderpass,
                            VK_NULL_HANDLE,
                            VK_NULL_HANDLE,
                            VkExtent2D{ m_uWidth, m_uHeight },
                            m_uSampleCountBits,
                            m_pInstanceCreator->GetPhysicalDeviceInformation(),
                            _shader));
                }
            }

            // check if custom viewport should be used
            if (_shader.bEnableCustomViewport) {
                VkViewport vp = {};
                vp.x = static_cast<float>(_shader.viewport.uX);
                vp.y = static_cast<float>(_shader.viewport.uY);
                vp.width = static_cast<float>(_shader.viewport.uWidth);
                vp.height = static_cast<float>(_shader.viewport.uHeight);
                vp.minDepth = 0.f;
                vp.maxDepth = 1.f;
                vkCmdSetViewport(m_commandBuffers[m_uCurrentFrameIndex], 0, 1, &vp);
            }

            std::array<VkDescriptorSet, 2> descriptorSets = { VK_NULL_HANDLE, VK_NULL_HANDLE };
            if (_pDescriptorAllocator) {
                descriptorSets[0] = _pDescriptorAllocator->RequestShaderDescriptorSet(m_uCurrentFrameIndex);
                descriptorSets[1] = _pDescriptorAllocator->RequestMeshDescriptorSet();
            }

            // check if textures should be bound
            if (descriptorSets[0]) {
                _pDescriptorAllocator->UpdateDescriptorSet(
                    m_hMainBuffer, 
                    descriptorSets[0], 
                    UNIFORM_USAGE_PER_SHADER, 
                    m_uCurrentFrameIndex,
                    _textureIds);
            }
            if (descriptorSets[1]) {
                _pDescriptorAllocator->UpdateDescriptorSet(
                    m_hMainBuffer, 
                    descriptorSets[1],
                    UNIFORM_USAGE_PER_MESH,
                    m_uCurrentFrameIndex,
                    _textureIds);
            }

            // submit each draw command in mesh
            for (auto itCmd = _mesh.drawCommands.begin(); itCmd != _mesh.drawCommands.end(); itCmd++) {
                vkCmdBindPipeline(
                    m_commandBuffers[m_uCurrentFrameIndex], 
                    VK_PIPELINE_BIND_POINT_GRAPHICS, 
                    m_pipelineCreators.find(_shader.pShader)->second.GetPipeline());

                std::vector<VkBuffer> buffers(_shader.attributes.size(), m_hMainBuffer);
                
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
                        m_pipelineCreators.find(_shader.pShader)->second.GetPipelineLayout(),
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
                        m_pipelineCreators.find(_shader.pShader)->second.GetPipelineLayout(),
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
                        m_pipelineCreators.find(_shader.pShader)->second.GetPipelineLayout(),
                        0,
                        1,
                        &descriptorSets[1],
                        0,
                        nullptr);
                }
                    
                // check if scissor technique should be used
                if (_shader.bEnableScissor) {
                    VkRect2D rect = {};
                    rect.offset = VkOffset2D{ 
                        itCmd->scissor.offset.x, 
                        itCmd->scissor.offset.y 
                    };

                    rect.extent = VkExtent2D{ 
                        itCmd->scissor.extent.x, 
                        itCmd->scissor.extent.y 
                    };

                    vkCmdSetScissor(m_commandBuffers[m_uCurrentFrameIndex], 0, 1, &rect);
                }

                // check if push constants should be bound
                if (_shader.bEnablePushConstants) {
                    auto itPipelineCreator = m_pipelineCreators.find(_shader.pShader);
                    VkShaderStageFlags bShaderStage = (VkShaderStageFlags)0;

                    if (_shader.iPushConstantShaderStage & SHADER_STAGE_VERTEX)
                        bShaderStage |= VK_SHADER_STAGE_VERTEX_BIT;
                    if (_shader.iPushConstantShaderStage & SHADER_STAGE_FRAGMENT)
                        bShaderStage |= VK_SHADER_STAGE_FRAGMENT_BIT;
                    if (_shader.iPushConstantShaderStage & SHADER_STAGE_GEOMETRY)
                        bShaderStage |= VK_SHADER_STAGE_GEOMETRY_BIT;

                    vkCmdPushConstants(
                        m_commandBuffers[m_uCurrentFrameIndex],
                        itPipelineCreator->second.GetPipelineLayout(),
                        bShaderStage,
                        0,
                        _shader.uPushConstantDataLength,
                        _shader.pPushConstantData);
                }

                // check if indexed or unindexed draw call should be submitted
                if (_shader.bEnableIndexing) {
                    vkCmdBindIndexBuffer(m_commandBuffers[m_uCurrentFrameIndex], m_hMainBuffer, static_cast<VkDeviceSize>(itCmd->uIndicesOffset), VK_INDEX_TYPE_UINT32);
                    vkCmdDrawIndexed(m_commandBuffers[m_uCurrentFrameIndex], itCmd->uDrawCount, 1, 0, 0, 0);
                } else {
                    vkCmdDraw(m_commandBuffers[m_uCurrentFrameIndex], itCmd->uDrawCount, 1, 0, 0);
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

        void Framebuffer::RecreateFramebuffer() {
            // destroy previous resources
            _DestroyFramebuffer(false);
            _CreateDepthResources();

            if (!m_pSwapchainCreator) {
                _CreateFramebufferImage();
            }

            m_hRenderpass = Vulkan::SwapchainCreator::CreateRenderPass(m_pInstanceCreator->GetDevice(), VK_FORMAT_DEFAULT_IMAGE, m_uSampleCountBits, !m_pSwapchainCreator);

            if (!m_pSwapchainCreator) {
                _CreateFramebufferImage();
            }
            else {
                m_pSwapchainCreator = new SwapchainCreator(m_pInstanceCreator, m_uWidth, m_uHeight, m_uSampleCountBits);
                const std::vector<Vulkan::TextureData>& swapchainImageResources =
                    m_pSwapchainCreator->GetSwapchainImageHandles();

                DENG_ASSERT(swapchainImageResources.size() == m_framebufferImageIds.size());

                for (size_t i = 0; i < swapchainImageResources.size(); i++) {
                    TextureResource textureResource;
                    textureResource.uWidth = m_uWidth;
                    textureResource.uHeight = m_uHeight;
                    textureResource.uBitDepth = 4;
                    textureResource.eLoadType = TEXTURE_RESOURCE_LOAD_TYPE_EMBEDDED;
                    textureResource.eResourceType = TEXTURE_RESOURCE_INTERNAL_FRAMEBUFFER_2D_IMAGE;
                    textureResource.apiTextureHandles = swapchainImageResources[i];

                    m_textureRegistry[m_framebufferImageIds[i]] = textureResource;
                }
            }

            _CreateFramebuffers();

            if (m_pSwapchainCreator) {
                for (auto& pipelineCreator : m_pipelineCreators) {
                    const VkExtent2D extent = { m_uWidth, m_uHeight };

                    //pipelineCreator.second.RecreatePipeline(
                    //    pipelineCreator.second,
                    //    m_hRenderpass,
                    //    extent,
                    //    false);
                }
            }
        }
    }
}
