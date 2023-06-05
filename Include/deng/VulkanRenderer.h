// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanRenderer.h - Vulkan renderer class header
// author: Karl-Mihkel Ott

#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H


#ifdef VULKAN_RENDERER_CPP
    #include <string>
    #include <vector>
    #include <cstring>
    #include <queue>
    #include <list>
    #include <unordered_map>

    #include <algorithm>
#ifdef __DEBUG
    #include <iostream>
#endif
    #include <vulkan/vulkan.h>

    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Points.h"
    #include "trs/Quaternion.h"

    #include "deng/Api.h"
    #include "deng/Exceptions.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/Missing.h"

    #define CALC_MIPLVL(_x, _y) (static_cast<uint32_t>(std::floor(std::log2(std::max(static_cast<double>(_x), static_cast<double>(_y))))))
#endif

#ifndef RESIZE_DEBOUNCE_TIMESTEP
#define RESIZE_DEBOUNCE_TIMESTEP 100.f // ms
#endif

#include "deng/IRenderer.h"
#include "deng/VulkanHelpers.h"
#include "deng/VulkanInstanceCreator.h"
#include "deng/VulkanSwapchainCreator.h"
#include "deng/VulkanPipelineCreator.h"
#include "deng/VulkanDescriptorAllocator.h"
#include "deng/VulkanFramebuffer.h"


namespace DENG {

    class DENG_API VulkanRenderer : public IRenderer {
        private:
            const VkSampleCountFlagBits m_uSampleCountBits = VK_SAMPLE_COUNT_1_BIT;

            Vulkan::InstanceCreator* m_pInstanceCreator = nullptr;
            Vulkan::SwapchainCreator* m_pSwapchainCreator = nullptr;
            std::unordered_map<Shader*, Vulkan::DescriptorAllocator> m_pipelineDescriptorAllocators;

            // locally managed vulkan resources
            VkDeviceSize m_uBufferSize = DEFAULT_BUFFER_SIZE;
            VkDeviceSize m_uStagingBufferSize = DEFAULT_STAGING_BUFFER_SIZE;

            struct _DeletedBuffer {
                VkBuffer hBuffer = VK_NULL_HANDLE;
                VkDeviceMemory hMemory = VK_NULL_HANDLE;
                VkDeviceSize uSize = 0;
            };

            std::vector<_DeletedBuffer> m_deletedBuffers;
            VkBuffer m_hMainBuffer = VK_NULL_HANDLE;
            VkDeviceMemory m_hMainBufferMemory = VK_NULL_HANDLE;

            VkBuffer m_hStagingBuffer = VK_NULL_HANDLE;
            VkDeviceMemory m_hStagingBufferMemory = VK_NULL_HANDLE;

            uint32_t m_uResizedViewportWidth = 0;
            uint32_t m_uResizedViewportHeight = 0;
            bool m_bResizeModeTriggered = false;

            std::chrono::time_point<std::chrono::high_resolution_clock> m_resizeBeginTimestamp =
                std::chrono::high_resolution_clock::now();
            std::chrono::time_point<std::chrono::high_resolution_clock> m_resizeEndTimestamp =
                std::chrono::high_resolution_clock::now();

        private:
            void _CreateApiImageHandles(uint32_t _id);
            void _CheckAndReallocateBufferResources(size_t _uSize, size_t _uOffset);


        public:
            VulkanRenderer() = default;
            VulkanRenderer(VulkanRenderer&&) noexcept = default;
            ~VulkanRenderer();

            virtual void UpdateViewport(uint32_t _uWidth, uint32_t _uHeight) override;
            virtual uint32_t AddTextureResource(const TextureResource& _resource) override;
            virtual void DestroyPipeline(Shader* _pShader) override;
            virtual IFramebuffer* CreateFramebuffer(uint32_t _uWidth, uint32_t _uHeight) override;
            virtual IFramebuffer* CreateContext(IWindowContext* _pWindow) override;
            virtual size_t AllocateMemory(size_t _uSize, BufferDataType _eType) override;
            virtual void DeallocateMemory(size_t _uOffset) override;
            virtual void UpdateBuffer(const void* _pData, size_t _uSize, size_t _uOffset) override;
            virtual bool SetupFrame() override;
            virtual void DrawMesh(const MeshComponent& _mesh, const ShaderComponent& _shader, IFramebuffer* _pFramebuffer, uint32_t _uInstanceCount, const std::vector<uint32_t>& _textureIds = {}) override;
    };
}

#endif
