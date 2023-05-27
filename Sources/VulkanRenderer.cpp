// licence: Apache, see LICENCE file
// file: VulkanRenderer.cpp - Vulkan renderer class implementation
// author: Karl-Mihkel Ott

#define VULKAN_RENDERER_CPP
#include "deng/VulkanRenderer.h"


namespace DENG {

    uint32_t IRenderer::m_uImageCounter = 1;

    VulkanRenderer::~VulkanRenderer() {
        if (m_pInstanceCreator) {
            vkDeviceWaitIdle(m_pInstanceCreator->GetDevice());

            // destroy all framebuffers
            for (auto it = m_framebuffers.begin(); it != m_framebuffers.end(); it++) {
                delete *it;
            }

            // destroy all descriptor allocators
            for (auto it = m_pipelineDescriptorAllocators.begin(); it != m_pipelineDescriptorAllocators.end();)
                it = m_pipelineDescriptorAllocators.erase(it);

            for (auto it = m_textureRegistry.begin(); it != m_textureRegistry.end(); it++) {
                DeleteTextureResource(it->first);
            }

            // destroy texture resources
            while (!m_deletedTextureResourceQueue.empty()) {
                TextureResource textureResource = m_textureRegistry[m_deletedTextureResourceQueue.front()];
                m_deletedTextureResourceQueue.pop();
                Vulkan::TextureData vulkanHandles = std::get<0>(textureResource.apiTextureHandles);

                if (textureResource.eResourceType == TEXTURE_RESOURCE_1D_IMAGE ||
                    textureResource.eResourceType == TEXTURE_RESOURCE_2D_IMAGE ||
                    textureResource.eResourceType == TEXTURE_RESOURCE_3D_IMAGE)
                {
                    vkDestroySampler(m_pInstanceCreator->GetDevice(), vulkanHandles.hSampler, nullptr);
                    vkDestroyImageView(m_pInstanceCreator->GetDevice(), vulkanHandles.hImageView, nullptr);
                    vkDestroyImage(m_pInstanceCreator->GetDevice(), vulkanHandles.hImage, nullptr);
                    vkFreeMemory(m_pInstanceCreator->GetDevice(), vulkanHandles.hMemory, nullptr);

                    if (textureResource.bHeapAllocationFlag)
                        delete[] textureResource.pRGBAData;
                }
            }

            // free main buffers
            vkDestroyBuffer(m_pInstanceCreator->GetDevice(), m_hMainBuffer, NULL);
            vkFreeMemory(m_pInstanceCreator->GetDevice(), m_hMainBufferMemory, NULL);

            // free staging buffers
            vkDestroyBuffer(m_pInstanceCreator->GetDevice(), m_hStagingBuffer, nullptr);
            vkFreeMemory(m_pInstanceCreator->GetDevice(), m_hStagingBufferMemory, nullptr);

            // delete instance creator
            delete m_pInstanceCreator;
            m_pInstanceCreator = nullptr;
        }
    }


    void VulkanRenderer::_CreateApiImageHandles(uint32_t _id) {
        DENG_ASSERT(m_textureRegistry.find(_id) != m_textureRegistry.end());
        TextureResource& textureResource = m_textureRegistry[_id];
        DENG_ASSERT(textureResource.pRGBAData);
        Vulkan::TextureData vulkanTextureData;

        VkFormat eFormat = VK_FORMAT_DEFAULT_IMAGE;
        if (textureResource.uBitDepth == 3)
            eFormat = VK_FORMAT_R8G8B8_UNORM;

        VkDeviceSize uSize = 0;
        VkImageCreateFlagBits bImageBits = (VkImageCreateFlagBits)0;
        const uint32_t uMipLevels = 1; // temporary
        uint32_t uArrayCount = 0;

        switch (textureResource.eResourceType) {
            case TEXTURE_RESOURCE_2D_IMAGE:
                uSize = static_cast<VkDeviceSize>(textureResource.uWidth * textureResource.uHeight * textureResource.uBitDepth);
                uArrayCount = 1;
                break;

            case TEXTURE_RESOURCE_3D_IMAGE:
                uSize = static_cast<VkDeviceSize>(textureResource.uWidth * textureResource.uHeight * textureResource.uBitDepth * 6u);
                uArrayCount = 6;
                bImageBits = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
                break;

            default:
                return;
        }

        _CheckAndReallocateBufferResources(uSize, 0);
        Vulkan::_CopyToBufferMemory(m_pInstanceCreator->GetDevice(), uSize, textureResource.pRGBAData, m_hStagingBufferMemory, 0);
        
        VkMemoryRequirements memoryRequirements = Vulkan::_CreateImage(
            m_pInstanceCreator->GetDevice(),
            vulkanTextureData.hImage,
            textureResource.uWidth,
            textureResource.uHeight,
            uMipLevels,
            uArrayCount,
            eFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
            VK_SAMPLE_COUNT_1_BIT,
            bImageBits);

        Vulkan::_AllocateMemory(
            m_pInstanceCreator->GetDevice(),
            m_pInstanceCreator->GetPhysicalDevice(),
            memoryRequirements.size,
            vulkanTextureData.hMemory,
            memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vkBindImageMemory(m_pInstanceCreator->GetDevice(), vulkanTextureData.hImage, vulkanTextureData.hMemory, 0);
    
        VkCommandPool hCommandPool = static_cast<Vulkan::Framebuffer*>(m_framebuffers[0])->GetCommandPool();
        Vulkan::_TransitionImageLayout(
            m_pInstanceCreator->GetDevice(),
            vulkanTextureData.hImage,
            hCommandPool,
            m_pInstanceCreator->GetGraphicsQueue(),
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            uMipLevels,
            uArrayCount);

        // copy from staging buffer to image memory
        Vulkan::_CopyBufferToImage(
            m_pInstanceCreator->GetDevice(),
            hCommandPool,
            m_pInstanceCreator->GetGraphicsQueue(),
            m_hStagingBuffer,
            vulkanTextureData.hImage,
            textureResource.uWidth,
            textureResource.uHeight,
            uArrayCount);

        Vulkan::_TransitionImageLayout(
            m_pInstanceCreator->GetDevice(),
            vulkanTextureData.hImage,
            hCommandPool,
            m_pInstanceCreator->GetGraphicsQueue(),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            uMipLevels,
            uArrayCount);

        // create image view
        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = vulkanTextureData.hImage;
        if (textureResource.eResourceType == TEXTURE_RESOURCE_2D_IMAGE)
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        else imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        imageViewCreateInfo.format = eFormat;
        //imageViewCreateInfo.components = {
        //    VK_COMPONENT_SWIZZLE_R,
        //    VK_COMPONENT_SWIZZLE_G,
        //    VK_COMPONENT_SWIZZLE_B,
        //    VK_COMPONENT_SWIZZLE_A
        //};
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = uMipLevels;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = uArrayCount;
        
        if (vkCreateImageView(m_pInstanceCreator->GetDevice(), &imageViewCreateInfo, nullptr, &vulkanTextureData.hImageView) != VK_SUCCESS)
            throw RendererException("Cannot create VkImageView object");

        vulkanTextureData.hSampler = Vulkan::_CreateTextureSampler(
            m_pInstanceCreator->GetDevice(),
            m_pInstanceCreator->GetPhysicalDeviceInformation().fMaxSamplerAnisotropy,
            uMipLevels);

        textureResource.apiTextureHandles = vulkanTextureData;

        if (textureResource.bHeapAllocationFlag) {
            delete[] textureResource.pRGBAData;
            textureResource.bHeapAllocationFlag = false;
            textureResource.pRGBAData = nullptr;
        }
    }


    void VulkanRenderer::_CheckAndReallocateBufferResources(size_t _uSize, size_t _uOffset) {
        VkCommandPool hCommandPool = static_cast<Vulkan::Framebuffer*>(m_framebuffers[0])->GetCommandPool();

        // check if staging buffer reallocation is required
        if (static_cast<VkDeviceSize>(_uSize) > m_uStagingBufferSize) {
            vkFreeMemory(m_pInstanceCreator->GetDevice(), m_hStagingBufferMemory, nullptr);
            vkDestroyBuffer(m_pInstanceCreator->GetDevice(), m_hStagingBuffer, nullptr);

            m_uStagingBufferSize = std::max((m_uStagingBufferSize * 3) >> 1, _uSize);
            const VkDeviceSize uMinUniformBufferAlignment = static_cast<VkDeviceSize>(m_pInstanceCreator->GetPhysicalDeviceInformation().uMinimalUniformBufferAlignment);

            m_uStagingBufferSize = (m_uStagingBufferSize + uMinUniformBufferAlignment - 1) & ~(uMinUniformBufferAlignment - 1);

            VkMemoryRequirements memoryRequirements = Vulkan::_CreateBuffer(
                m_pInstanceCreator->GetDevice(),
                m_uStagingBufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                m_hStagingBuffer);

            Vulkan::_AllocateMemory(
                m_pInstanceCreator->GetDevice(),
                m_pInstanceCreator->GetPhysicalDevice(),
                memoryRequirements.size,
                m_hStagingBufferMemory,
                memoryRequirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            vkBindBufferMemory(m_pInstanceCreator->GetDevice(), m_hStagingBuffer, m_hStagingBufferMemory, 0);
        }

        // check if main buffer reallocation is required
        if (static_cast<VkDeviceSize>(_uSize + _uOffset) > m_uBufferSize) {
            // allocate a temporary memory buffer to hold data in
            VkBuffer hTemporaryBuffer = VK_NULL_HANDLE;
            VkDeviceMemory hTemporaryBufferMemory = VK_NULL_HANDLE;
            const VkDeviceSize cuOldBufferSize = m_uBufferSize;

            VkMemoryRequirements memoryRequirements = Vulkan::_CreateBuffer(
                m_pInstanceCreator->GetDevice(),
                m_uBufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                hTemporaryBuffer);

            Vulkan::_AllocateMemory(
                m_pInstanceCreator->GetDevice(),
                m_pInstanceCreator->GetPhysicalDevice(),
                memoryRequirements.size,
                m_hStagingBufferMemory,
                memoryRequirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            vkBindBufferMemory(m_pInstanceCreator->GetDevice(), hTemporaryBuffer, hTemporaryBufferMemory, 0);

            // copy data from main buffer to temporary buffer
            Vulkan::_CopyBufferToBuffer(
                m_pInstanceCreator->GetDevice(),
                hCommandPool,
                m_pInstanceCreator->GetGraphicsQueue(),
                m_hMainBuffer,
                hTemporaryBuffer,
                m_uBufferSize,
                0, 0);

            // free old main buffer resources and allocate new larger buffer
            vkFreeMemory(m_pInstanceCreator->GetDevice(), m_hMainBufferMemory, nullptr);
            vkDestroyBuffer(m_pInstanceCreator->GetDevice(), m_hMainBuffer, nullptr);

            m_uBufferSize = std::max(static_cast<VkDeviceSize>(_uSize + _uOffset), (m_uBufferSize * 3) >> 1);

            memoryRequirements = Vulkan::_CreateBuffer(
                m_pInstanceCreator->GetDevice(),
                m_uBufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                m_hMainBuffer);

            Vulkan::_AllocateMemory(
                m_pInstanceCreator->GetDevice(),
                m_pInstanceCreator->GetPhysicalDevice(),
                memoryRequirements.size,
                m_hMainBufferMemory,
                memoryRequirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            vkBindBufferMemory(m_pInstanceCreator->GetDevice(), m_hMainBuffer, m_hMainBufferMemory, 0);

            Vulkan::_CopyBufferToBuffer(
                m_pInstanceCreator->GetDevice(),
                hCommandPool,
                m_pInstanceCreator->GetGraphicsQueue(),
                hTemporaryBuffer,
                m_hMainBuffer,
                cuOldBufferSize,
                0, 0);
        }
    }


    uint32_t VulkanRenderer::AddTextureResource(const TextureResource& _resource) {
        DENG_ASSERT(m_pInstanceCreator);
        uint32_t uImageId = m_uImageCounter++;
        m_textureRegistry[uImageId] = _resource;

        _CreateApiImageHandles(uImageId);
        return uImageId;
    }


    void VulkanRenderer::DestroyPipeline(Shader* _pShader) {
        m_pipelineDescriptorAllocators.erase(_pShader);
        
        for (IFramebuffer* pFramebuffer : m_framebuffers) {
            Vulkan::Framebuffer* pVulkanFramebuffer = static_cast<Vulkan::Framebuffer*>(pFramebuffer);
            pVulkanFramebuffer->DestroyPipelineAllocator(_pShader);
        }
    }

    IFramebuffer* VulkanRenderer::CreateFramebuffer(uint32_t _uWidth, uint32_t _uHeight) {
        Vulkan::Framebuffer* pFramebuffer = new Vulkan::Framebuffer(
            m_pInstanceCreator, 
            m_hMainBuffer, 
            m_uSampleCountBits, 
            TRS::Point2D<uint32_t>(_uWidth, _uHeight),
            m_textureRegistry,
            []() { return m_uImageCounter++; },
            false);

        return pFramebuffer;
    }

    IFramebuffer* VulkanRenderer::CreateContext(IWindowContext* _pWindow) {
        DENG_ASSERT(_pWindow);

        m_pInstanceCreator = new Vulkan::InstanceCreator(*_pWindow);

        // allocate staging buffer memory
        VkMemoryRequirements memoryRequirements = Vulkan::_CreateBuffer(
            m_pInstanceCreator->GetDevice(),
            m_uStagingBufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            m_hStagingBuffer);

        Vulkan::_AllocateMemory(
            m_pInstanceCreator->GetDevice(),
            m_pInstanceCreator->GetPhysicalDevice(),
            memoryRequirements.size,
            m_hStagingBufferMemory,
            memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        
        vkBindBufferMemory(m_pInstanceCreator->GetDevice(), m_hStagingBuffer, m_hStagingBufferMemory, 0);

        // allocate main buffer memory
        memoryRequirements = Vulkan::_CreateBuffer(
            m_pInstanceCreator->GetDevice(),
            m_uBufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            m_hMainBuffer);

        Vulkan::_AllocateMemory(
            m_pInstanceCreator->GetDevice(),
            m_pInstanceCreator->GetPhysicalDevice(),
            memoryRequirements.size,
            m_hMainBufferMemory,
            memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vkBindBufferMemory(m_pInstanceCreator->GetDevice(), m_hMainBuffer, m_hMainBufferMemory, 0);

        Vulkan::Framebuffer* pFramebuffer = new Vulkan::Framebuffer(
            m_pInstanceCreator,
            m_hMainBuffer,
            m_uSampleCountBits,
            TRS::Point2D<uint32_t>(_pWindow->GetWidth(), _pWindow->GetHeight()),
            m_textureRegistry,
            []() { return m_uImageCounter++; },
            true);

        m_framebuffers.push_back(pFramebuffer);

        // add missing textures
        m_uMissing2DTextureId = m_uImageCounter++;
        m_uMissing3DTextureId = m_uImageCounter++;

        int x, y, size;
        char* data = GetMissingTextureRGBA(x, y, size);

        TextureResource textureResource;
        textureResource.eLoadType = TEXTURE_RESOURCE_LOAD_TYPE_EMBEDDED;
        textureResource.eResourceType = TEXTURE_RESOURCE_2D_IMAGE;
        textureResource.pRGBAData = data;
        textureResource.uWidth = static_cast<uint32_t>(x);
        textureResource.uHeight = static_cast<uint32_t>(y);
        textureResource.uBitDepth = 4;

        m_textureRegistry.insert(std::make_pair(m_uMissing2DTextureId, textureResource));
        _CreateApiImageHandles(m_uMissing2DTextureId);

        textureResource.eResourceType = TEXTURE_RESOURCE_3D_IMAGE;
        textureResource.pRGBAData = new char[x * y * 24] {};
        textureResource.bHeapAllocationFlag = true;
        for (int i = 0; i < 6; i++)
            std::memcpy(textureResource.pRGBAData + i * x * y * 4, data, x * y * 4);

        m_textureRegistry.insert(std::make_pair(m_uMissing3DTextureId, textureResource));
        _CreateApiImageHandles(m_uMissing3DTextureId);
        return pFramebuffer;
    }

    size_t VulkanRenderer::AllocateMemory(size_t _uSize, BufferDataType _eType) {
        DENG_ASSERT(m_pInstanceCreator);

        switch (_eType) {
            case BufferDataType::VERTEX:
                return m_gpuMemoryAllocator.RequestMemory(_uSize, sizeof(uint32_t)).uOffset;

            case BufferDataType::INDEX:
                return m_gpuMemoryAllocator.RequestMemory(_uSize, sizeof(uint32_t)).uOffset;

            case BufferDataType::UNIFORM:
                return m_gpuMemoryAllocator.RequestMemory(
                    _uSize,
                    m_pInstanceCreator->GetPhysicalDeviceInformation().uMinimalUniformBufferAlignment).uOffset;
        }
        
        return 0;
    }


    void VulkanRenderer::DeallocateMemory(size_t _uOffset) {
        m_gpuMemoryAllocator.FreeMemory(_uOffset);
    }


    void VulkanRenderer::UpdateBuffer(const void* _pData, size_t _uSize, size_t _uOffset) {
        DENG_ASSERT(m_pInstanceCreator);
        _CheckAndReallocateBufferResources(_uSize, _uOffset);
        VkCommandPool hCommandPool = static_cast<Vulkan::Framebuffer*>(m_framebuffers[0])->GetCommandPool();

        // staging
        Vulkan::_CopyToBufferMemory(
            m_pInstanceCreator->GetDevice(),
            _uSize,
            _pData,
            m_hStagingBufferMemory, 0);

        // staging to main
        Vulkan::_CopyBufferToBuffer(
            m_pInstanceCreator->GetDevice(),
            hCommandPool,
            m_pInstanceCreator->GetGraphicsQueue(),
            m_hStagingBuffer,
            m_hMainBuffer,
            _uSize,
            0, _uOffset);
    }


    void VulkanRenderer::SetupFrame() {
        for (auto it = m_pipelineDescriptorAllocators.begin(); it != m_pipelineDescriptorAllocators.end(); it++) {
            it->second.ResetMeshCounter();
        }

        // check if there are any textures that could be removed
        while (!m_deletedTextureResourceQueue.empty()) {
            uint32_t hTexture = m_deletedTextureResourceQueue.front();
            m_deletedTextureResourceQueue.pop();

            auto itTexture = m_textureRegistry.find(hTexture);
            
            // just in case
            if (itTexture == m_textureRegistry.end())
                continue;
        
            Vulkan::TextureData textureData = std::get<0>(itTexture->second.apiTextureHandles);
            vkDestroySampler(m_pInstanceCreator->GetDevice(), textureData.hSampler, nullptr);
            vkDestroyImageView(m_pInstanceCreator->GetDevice(), textureData.hImageView, nullptr);
            vkDestroyImage(m_pInstanceCreator->GetDevice(), textureData.hImage, nullptr);
            vkFreeMemory(m_pInstanceCreator->GetDevice(), textureData.hMemory, nullptr);
        }

        // check if there are any textures that could be added
        while (!m_addedTextureResourceQueue.empty()) {
            uint32_t hTexture = m_addedTextureResourceQueue.front();
            m_addedTextureResourceQueue.pop();

            auto itTexture = m_textureRegistry.find(hTexture);

            if (itTexture == m_textureRegistry.end())
                continue;

            _CreateApiImageHandles(hTexture);
        }
    }


    void VulkanRenderer::DrawMesh(const MeshComponent& _mesh, const ShaderComponent& _shaderComponent, IFramebuffer* _pFramebuffer, const std::vector<uint32_t>& _textureIds) {
        DENG_ASSERT(_pFramebuffer);
        Vulkan::Framebuffer* vulkanFramebuffer = static_cast<Vulkan::Framebuffer*>(_pFramebuffer);
        // descriptor allocators do not exist
        if (_shaderComponent.uboDataLayouts.size() && m_pipelineDescriptorAllocators.find(_shaderComponent.pShader) == m_pipelineDescriptorAllocators.end()) {
            m_pipelineDescriptorAllocators.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(_shaderComponent.pShader),
                std::forward_as_tuple(
                    m_pInstanceCreator->GetDevice(),
                    m_hMainBuffer,
                    m_textureRegistry,
                    _shaderComponent.uboDataLayouts,
                    MAX_FRAMES_IN_FLIGHT,
                    m_uMissing2DTextureId,
                    m_uMissing3DTextureId));
        }

        auto itDescriptorAllocator = m_pipelineDescriptorAllocators.find(_shaderComponent.pShader);
        
        if (itDescriptorAllocator != m_pipelineDescriptorAllocators.end()) {
            vulkanFramebuffer->Draw(_mesh, _shaderComponent, &itDescriptorAllocator->second, _textureIds);
        }
        else {
            vulkanFramebuffer->Draw(_mesh, _shaderComponent, nullptr, _textureIds);
        }
    }
}