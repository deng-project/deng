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

            //  Main memory is usually splitted into vertex regions and index regions, but not always
            //  [ [ VERTICES ] [ INDICES ] ]
            VkBuffer m_hMainBuffer = VK_NULL_HANDLE;
            VkDeviceMemory m_hMainBufferMemory = VK_NULL_HANDLE;

            VkBuffer m_hStagingBuffer = VK_NULL_HANDLE;
            VkDeviceMemory m_hStagingBufferMemory = VK_NULL_HANDLE;

        private:
            void _CreateApiImageHandles(uint32_t _id);
            void _CheckAndReallocateBufferResources(size_t _uSize, size_t _uOffset);


        public:
            VulkanRenderer() = default;
            VulkanRenderer(VulkanRenderer&&) noexcept = default;
            ~VulkanRenderer();

            virtual uint32_t AddTextureResource(const TextureResource& _resource) override;
            virtual void DestroyPipeline(Shader* _pShader) override;
            virtual IFramebuffer* CreateFramebuffer(uint32_t _uWidth, uint32_t _uHeight) override;
            virtual IFramebuffer* CreateContext(IWindowContext* _pWindow) override;
            virtual size_t AllocateMemory(size_t _uSize, BufferDataType _eType) override;
            virtual void DeallocateMemory(size_t _uOffset) override;
            virtual void UpdateBuffer(const void* _pData, size_t _uSize, size_t _uOffset) override;
            virtual void DrawMesh(const MeshComponent& _mesh, const ShaderComponent& _shader, IFramebuffer* _pFramebuffer, const std::vector<uint32_t>& _textureIds = {}) override;
    };
}

#endif
