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

    #include "das/Api.h"
    #include "das/DasStructures.h"
    #include "das/TextureReader.h"

    #include "deng/Api.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/Exceptions.h"
    #include "deng/Components.h"
    #include "deng/IWindowContext.h"
    #include "deng/VulkanHelpers.h"
    #include "deng/IRenderer.h"
    #include "deng/VulkanInstanceCreator.h"
    #include "deng/VulkanSwapchainCreator.h"
    #include "deng/VulkanPipelineCreator.h"
    #include "deng/VulkanDescriptorAllocator.h"
#endif

namespace DENG {
    namespace Vulkan {

        class Framebuffer : public IFramebuffer {
            private:
                const InstanceCreator* m_pInstanceCreator = nullptr;
                SwapchainCreator* m_pSwapchainCreator = nullptr;
                std::unordered_map<uint32_t, TextureResource>& m_textureRegistry;
                uint32_t(*m_pfnGetNewImageId)();

                VkBuffer& m_hMainBuffer;
                VkSampleCountFlagBits m_uSampleCountBits;
                
                std::unordered_map<Shader*, PipelineCreator> m_pipelineCreators;

                VkCommandPool m_hCommandPool;
                std::vector<VkCommandBuffer> m_commandBuffers;
                std::vector<VkSemaphore> m_renderFinishedSemaphores;
                std::vector<VkSemaphore> m_imageAvailableSemaphores;
                std::vector<VkFence> m_flightFences;
                std::vector<VkFramebuffer> m_framebuffers;

                // color resolve and depth image resources
                uint32_t m_uDepthImageId = UINT32_MAX;
                std::vector<uint32_t> m_framebufferImageIds;

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
                void _DestroyFramebuffer(bool _bRemoveImageResources = true);

            public:
                Framebuffer(
                    const InstanceCreator* _pInstanceCreator,
                    VkBuffer& _hMainBuffer,
                    VkSampleCountFlagBits _uSampleCountBits,
                    TRS::Point2D<uint32_t> _extent,
                    std::unordered_map<uint32_t, TextureResource>& _textureRegistry,
                    uint32_t(*_pfnGetNewImageId)(),
                    bool _bIsSwapchain = false
                );
                Framebuffer(Framebuffer &&_fb) noexcept = default;
                ~Framebuffer();

                void RecreateFramebuffer(uint32_t _uWidth, uint32_t _uHeight);

                inline void DestroyPipelineAllocator(Shader* _pShader) {
                    m_pipelineCreators.erase(_pShader);
                }

                virtual void BeginCommandBufferRecording(TRS::Vector4<float> _vClearColor) override;
                void Draw(const MeshComponent& _mesh, const ShaderComponent& _shaderComponent, DescriptorAllocator* _pDescriptorAllocator, const std::vector<uint32_t>& _textureIds = {});
                virtual void EndCommandBufferRecording() override;
                virtual void RenderToFramebuffer() override;

                inline SwapchainCreator* GetSwapchainCreator() {
                    return m_pSwapchainCreator;
                }

                inline VkCommandPool GetCommandPool() {
                    return m_hCommandPool;
                }

                inline VkRenderPass GetRenderPass() {
                    return m_hRenderpass;
                }

                inline const std::vector<uint32_t> &GetFramebufferImageIds() const {
                    return m_framebufferImageIds;
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
