// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanFramebuffer.h - Vulkan framebuffer management class header
// author: Karl-Mihkel Ott

#ifndef VULKAN_FRAMEBUFFER_H
#define VULKAN_FRAMEBUFFER_H

#ifdef VULKAN_FRAMEBUFFER_CPP
    #include <vector>
    #include <string>
    #include <array>
    #include <variant>
    #include <queue>
    #include <unordered_map>
    #include <functional>
    #include <sstream>
    #include <iomanip>
#ifdef __DEBUG
    #include <iostream>
#endif
    #include <vulkan/vulkan.h>

    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Quaternion.h"

    #include <cvar/SID.h>

    #include "deng/Api.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/Exceptions.h"
    #include "deng/Components.h"
    #include "deng/IWindowContext.h"
    #include "deng/VulkanHelpers.h"
    #include "deng/RenderResources.h"
    #include "deng/IRenderer.h"
    #include "deng/VulkanInstanceCreator.h"
    #include "deng/VulkanSwapchainCreator.h"
    #include "deng/VulkanPipelineCreator.h"
#endif

namespace DENG {
    namespace Vulkan {

        class Framebuffer : public IFramebuffer {
            private:
                const InstanceCreator* m_pInstanceCreator = nullptr;
                SwapchainCreator* m_pSwapchainCreator = nullptr;

                VkBuffer& m_hMainBuffer;
                VkSampleCountFlagBits m_uSampleCountBits;
                
                Vulkan::TextureData m_framebufferImageHandles;
                Vulkan::TextureData m_depthImageHandles;

                std::unordered_map<cvar::hash_t, Vulkan::PipelineCreator, cvar::NoHash> m_pipelineCreators;

                VkCommandPool m_hCommandPool;
                std::vector<VkCommandBuffer> m_commandBuffers;
                std::vector<VkSemaphore> m_renderFinishedSemaphores;
                std::vector<VkSemaphore> m_imageAvailableSemaphores;
                std::vector<VkFence> m_flightFences;
                std::vector<VkFramebuffer> m_framebuffers;

                VkRenderPass m_hRenderpass;

                uint32_t m_uCurrentSwapchainImageIndex = 0;
                uint32_t m_uCurrentFrameIndex = 0;

            private:
                void _CreateDepthResources();
                void _CreateFramebuffers();
                void _CreateFramebufferImage();
                void _CreateCommandPool();
                void _CreateSynchronisationPrimitives();
                void _AllocateCommandBuffers();
                void _RecreateSwapchain();
                void _DestroyFramebuffer();

            public:
                Framebuffer(
                    const InstanceCreator* _pInstanceCreator,
                    VkBuffer& _hMainBuffer,
                    VkSampleCountFlagBits _uSampleCountBits,
                    TRS::Point2D<uint32_t> _extent,
                    bool _bIsSwapchain = false);
                Framebuffer(Framebuffer &&_fb) noexcept = default;
                ~Framebuffer();

                void RecreateFramebuffer(uint32_t _uWidth, uint32_t _uHeight);

                virtual void BeginCommandBufferRecording(TRS::Vector4<float> _vClearColor) override;
                void Draw(
                    cvar::hash_t _hshMesh, 
                    cvar::hash_t _hshShader, 
                    uint32_t _uInstanceCount,
                    uint32_t _uFirstInstance,
                    VkDescriptorSet _hShaderDescriptorSet, 
                    VkDescriptorSet _hMaterialDescriptorSet,
                    VkDescriptorSetLayout _hShaderDescriptorSetLayout,
                    VkDescriptorSetLayout _hMaterialDescriptorSetLayout);
                virtual void EndCommandBufferRecording() override;
                virtual void RenderToFramebuffer() override;

                inline void UpdatePipelineCreator(cvar::hash_t _hshShader, VkDescriptorSetLayout _hShaderDescriptorSetLayout, VkDescriptorSetLayout _hMaterialDescriptorSetLayout) {
                    if (m_pipelineCreators.find(_hshShader) == m_pipelineCreators.end()) {
                        ResourceManager& resourceManager = ResourceManager::GetInstance();
                        auto pShader = resourceManager.GetGraphicsShader(_hshShader);
                        DENG_ASSERT(pShader);

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
                }

                inline uint32_t GetCurrentFrameIndex() {
                    return m_uCurrentFrameIndex;
                }

                inline SwapchainCreator* GetSwapchainCreator() {
                    return m_pSwapchainCreator;
                }

                inline VkCommandPool GetCommandPool() {
                    return m_hCommandPool;
                }

                inline VkRenderPass GetRenderPass() {
                    return m_hRenderpass;
                }

                // returns null handles if framebuffer is swapchain framebuffer
                inline const Vulkan::TextureData& GetFramebufferImageHandles() const {
                    return m_framebufferImageHandles;
                }

                inline VkFence& GetCurrentFlightFence() {
                    return m_flightFences[m_uCurrentFrameIndex];
                }

                inline VkSemaphore GetSwapchainImageAcquisitionSemaphore() {
                    return m_imageAvailableSemaphores[m_uCurrentFrameIndex];
                }

                inline VkSemaphore GetRenderFinishedSemaphore() {
                    return m_renderFinishedSemaphores[m_uCurrentFrameIndex];
                }
        };
    }
}

#endif
