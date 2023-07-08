// licence: Apache, see LICENCE file
// file: VulkanRenderer.cpp - Vulkan renderer class implementation
// author: Karl-Mihkel Ott

#define VULKAN_RENDERER_CPP
#include "deng/VulkanRenderer.h"


namespace DENG {

    VulkanRenderer::~VulkanRenderer() {
        if (m_pInstanceCreator) {
            vkDeviceWaitIdle(m_pInstanceCreator->GetDevice());

            // destroy all framebuffers
            for (auto it = m_framebuffers.begin(); it != m_framebuffers.end(); it++) {
                delete *it;
            }

            // destroy all descriptor pools and descriptor set layouts
            m_fullDescriptorPools.push_back(m_hMainDescriptorPool);
            for (VkDescriptorPool hPool : m_fullDescriptorPools)
                vkDestroyDescriptorPool(m_pInstanceCreator->GetDevice(), hPool, nullptr);

            vkDestroyDescriptorSetLayout(m_pInstanceCreator->GetDevice(), m_hMaterialDescriptorSetLayout, nullptr);
            for (auto it = m_shaderDescriptors.begin(); it != m_shaderDescriptors.end(); it++) {
                vkDestroyDescriptorSetLayout(m_pInstanceCreator->GetDevice(), it->second.hDescriptorSetLayout, nullptr);
            }

            DeleteTextureHandles();

            // free main buffers
            vkDestroyBuffer(m_pInstanceCreator->GetDevice(), m_mainBuffer.hBuffer, NULL);
            vkFreeMemory(m_pInstanceCreator->GetDevice(), m_mainBuffer.hMemory, NULL);

            // free staging buffers
            vkDestroyBuffer(m_pInstanceCreator->GetDevice(), m_stagingBuffer.hBuffer, nullptr);
            vkFreeMemory(m_pInstanceCreator->GetDevice(), m_stagingBuffer.hMemory, nullptr);

            // delete instance creator
            delete m_pInstanceCreator;
            m_pInstanceCreator = nullptr;
        }
    }


    void VulkanRenderer::_CreateApiImageHandles(hash_t _hshImage) {
        ResourceManager& resourceManager = ResourceManager::GetInstance();
        auto pImage = resourceManager.GetTexture(_hshImage);
        DENG_ASSERT(pImage);
        DENG_ASSERT(pImage->pRGBAData);

        Vulkan::TextureData vulkanTextureData;
        VkFormat eFormat = VK_FORMAT_UNDEFINED;
        switch (pImage->uBitDepth) {
            case 1:
                eFormat = VK_FORMAT_R8_UNORM;
                break;

            case 2:
                eFormat = VK_FORMAT_R8G8_UNORM;
                break;

            case 3:
                eFormat = VK_FORMAT_R8G8B8_UNORM;
                break;

            case 4:
                eFormat = VK_FORMAT_R8G8B8A8_UNORM;
                break;

            default:
                DENG_ASSERT(false);
                break;
        }

        VkDeviceSize uSize = 0;
        VkImageCreateFlagBits bImageBits = (VkImageCreateFlagBits)0;
        const uint32_t uMipLevels = 1; // temporary
        uint32_t uArrayCount = 0;

        switch (pImage->eResourceType) {
            case TextureType::Image_2D:
                uSize = static_cast<VkDeviceSize>(pImage->uWidth * pImage->uHeight * pImage->uBitDepth);
                uArrayCount = 1;
                break;

            case TextureType::Image_3D:
                uSize = static_cast<VkDeviceSize>(pImage->uWidth * pImage->uHeight * pImage->uBitDepth);
                uArrayCount = 6;
                bImageBits = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
                break;

            case TextureType::Image_3D_Array:
                uSize = static_cast<VkDeviceSize>(pImage->uWidth * pImage->uHeight * pImage->uBitDepth * 6u);
                uArrayCount = 6;
                bImageBits = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
                break;

            default:
                return;
        }

        _CheckAndReallocateBufferResources(uSize, 0);
        Vulkan::_CopyToBufferMemory(m_pInstanceCreator->GetDevice(), uSize, pImage->pRGBAData, m_stagingBuffer.hMemory, 0);
        
        VkMemoryRequirements memoryRequirements = Vulkan::_CreateImage(
            m_pInstanceCreator->GetDevice(),
            vulkanTextureData.hImage,
            pImage->uWidth,
            pImage->uHeight,
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
            m_stagingBuffer.hBuffer,
            vulkanTextureData.hImage,
            pImage->uWidth,
            pImage->uHeight,
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
        if (pImage->eResourceType == TextureType::Image_2D)
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        else imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        imageViewCreateInfo.format = eFormat;
        imageViewCreateInfo.components = {
            VK_COMPONENT_SWIZZLE_R,
            VK_COMPONENT_SWIZZLE_G,
            VK_COMPONENT_SWIZZLE_B,
            VK_COMPONENT_SWIZZLE_A
        };
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

        m_textureHandles[_hshImage] = vulkanTextureData;

        if (pImage->bHeapAllocationFlag)
            resourceManager.FreeTextureHeapData(_hshImage);
    }


    void VulkanRenderer::_CheckAndReallocateBufferResources(size_t _uSize, size_t _uOffset) {
        VkCommandPool hCommandPool = static_cast<Vulkan::Framebuffer*>(m_framebuffers[0])->GetCommandPool();

        // check if device should be on idle
        if (static_cast<VkDeviceSize>(_uSize) > m_stagingBuffer.uSize || static_cast<VkDeviceSize>(_uSize + _uOffset) > m_mainBuffer.uSize)
            vkDeviceWaitIdle(m_pInstanceCreator->GetDevice());

        // check if staging buffer reallocation is required
        if (static_cast<VkDeviceSize>(_uSize) > m_stagingBuffer.uSize) {
            vkFreeMemory(m_pInstanceCreator->GetDevice(), m_stagingBuffer.hMemory, nullptr);
            vkDestroyBuffer(m_pInstanceCreator->GetDevice(), m_stagingBuffer.hBuffer, nullptr);

            m_stagingBuffer.uSize = (_uSize * 3) >> 1;
            const VkDeviceSize uMinUniformBufferAlignment = static_cast<VkDeviceSize>(m_pInstanceCreator->GetPhysicalDeviceInformation().uMinimalUniformBufferAlignment);
            m_stagingBuffer.uSize = (m_stagingBuffer.uSize + uMinUniformBufferAlignment - 1) & ~(uMinUniformBufferAlignment - 1);

            VkMemoryRequirements memoryRequirements = Vulkan::_CreateBuffer(
                m_pInstanceCreator->GetDevice(),
                m_stagingBuffer.uSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                m_stagingBuffer.hBuffer);

            Vulkan::_AllocateMemory(
                m_pInstanceCreator->GetDevice(),
                m_pInstanceCreator->GetPhysicalDevice(),
                memoryRequirements.size,
                m_stagingBuffer.hMemory,
                memoryRequirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            vkBindBufferMemory(m_pInstanceCreator->GetDevice(), m_stagingBuffer.hBuffer, m_stagingBuffer.hMemory, 0);
        }

        // check if main buffer reallocation is required
        if (static_cast<VkDeviceSize>(_uSize + _uOffset) > m_mainBuffer.uSize) {
            // allocate a temporary memory buffer to hold data in
            VkBuffer hTemporaryBuffer = VK_NULL_HANDLE;
            VkDeviceMemory hTemporaryBufferMemory = VK_NULL_HANDLE;
            const VkDeviceSize cuOldBufferSize = m_mainBuffer.uSize;

            VkMemoryRequirements memoryRequirements = Vulkan::_CreateBuffer(
                m_pInstanceCreator->GetDevice(),
                cuOldBufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                hTemporaryBuffer);

            Vulkan::_AllocateMemory(
                m_pInstanceCreator->GetDevice(),
                m_pInstanceCreator->GetPhysicalDevice(),
                memoryRequirements.size,
                hTemporaryBufferMemory,
                memoryRequirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            vkBindBufferMemory(m_pInstanceCreator->GetDevice(), hTemporaryBuffer, hTemporaryBufferMemory, 0);

            // copy data from main buffer to temporary buffer
            Vulkan::_CopyBufferToBuffer(
                m_pInstanceCreator->GetDevice(),
                hCommandPool,
                m_pInstanceCreator->GetGraphicsQueue(),
                m_mainBuffer.hBuffer,
                hTemporaryBuffer,
                cuOldBufferSize,
                0, 0);

            // free old main buffer resources and allocate new larger buffer
            m_deletedBuffers.push_back(m_mainBuffer);
            m_mainBuffer.uSize = (static_cast<VkDeviceSize>(_uSize + _uOffset) * 3) >> 1;

            memoryRequirements = Vulkan::_CreateBuffer(
                m_pInstanceCreator->GetDevice(),
                m_mainBuffer.uSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                m_mainBuffer.hBuffer);

            Vulkan::_AllocateMemory(
                m_pInstanceCreator->GetDevice(),
                m_pInstanceCreator->GetPhysicalDevice(),
                memoryRequirements.size,
                m_mainBuffer.hMemory,
                memoryRequirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            vkBindBufferMemory(m_pInstanceCreator->GetDevice(), m_mainBuffer.hBuffer, m_mainBuffer.hMemory, 0);

            Vulkan::_CopyBufferToBuffer(
                m_pInstanceCreator->GetDevice(),
                hCommandPool,
                m_pInstanceCreator->GetGraphicsQueue(),
                hTemporaryBuffer,
                m_mainBuffer.hBuffer,
                cuOldBufferSize,
                0, 0);

            vkFreeMemory(m_pInstanceCreator->GetDevice(), hTemporaryBufferMemory, nullptr);
            vkDestroyBuffer(m_pInstanceCreator->GetDevice(), hTemporaryBuffer, nullptr);
        }
    }


    void VulkanRenderer::_CreateMaterialDescriptorSetLayout() {
        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {};

        // diffuse map binding
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bindings[0].binding = 0;
        bindings[0].descriptorCount = 1;
        bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        // specular map binding
        bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bindings[1].binding = 1;
        bindings[1].descriptorCount = 1;
        bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = 2;
        descriptorSetLayoutCreateInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(m_pInstanceCreator->GetDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_hMaterialDescriptorSetLayout) != VK_SUCCESS)
            throw RendererException("vkCreateDescriptorSetLayout() failed to create a material descriptor set layout");
        LOG("Created material descriptor set layout");
    }


    void VulkanRenderer::_CreateShaderDescriptorSetLayout(VkDescriptorSetLayout* _pDescriptorSetLayout, hash_t _hshShader) {
        ResourceManager& resourceManager = ResourceManager::GetInstance();
        auto pShader = resourceManager.GetShader(_hshShader);
        DENG_ASSERT(pShader);

        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.reserve(pShader->GetUniformDataLayouts().size());

        for (const UniformDataLayout& _uniformDataLayout : pShader->GetUniformDataLayouts()) {
            bindings.emplace_back();
            switch (_uniformDataLayout.eType) {
                case UniformDataType::Buffer:
                    bindings.back().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    break;

                case UniformDataType::StorageBuffer:
                    bindings.back().descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    break;

                case UniformDataType::ImageSampler2D:
                case UniformDataType::ImageSampler3D:
                    bindings.back().descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    break;

                default:
                    DENG_ASSERT(false);
                    break;
            }
            bindings.back().binding = _uniformDataLayout.block.uBinding;
            bindings.back().descriptorCount = 1;

            if (_uniformDataLayout.bmShaderStage & ShaderStageBit_Vertex)
                bindings.back().stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
            if (_uniformDataLayout.bmShaderStage & ShaderStageBit_Fragment)
                bindings.back().stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
            if (_uniformDataLayout.bmShaderStage & ShaderStageBit_Geometry)
                bindings.back().stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        descriptorSetLayoutCreateInfo.pBindings = bindings.data();
        if (vkCreateDescriptorSetLayout(m_pInstanceCreator->GetDevice(), &descriptorSetLayoutCreateInfo, nullptr, _pDescriptorSetLayout) != VK_SUCCESS)
            throw RendererException("vkCreateDescriptorSetLayout() failed to create a descriptor set layout for shader");
        
        LOG("Created descriptor set layout for shader 0x" << std::setw(8) << std::hex << _hshShader);
    }


    void VulkanRenderer::_AllocateShaderDescriptors(hash_t _hshShader) {
        m_shaderDescriptors.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(_hshShader),
            std::forward_as_tuple());

        Vulkan::ShaderDescriptorData& descriptorData = m_shaderDescriptors[_hshShader];
        DENG_ASSERT(descriptorData.hDescriptorSetLayout != VK_NULL_HANDLE);

        _CheckAndAllocateDescriptorPool(MAX_FRAMES_IN_FLIGHT);

        std::array<VkDescriptorSetLayout, MAX_FRAMES_IN_FLIGHT> descriptorSetLayouts;
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            descriptorSetLayouts[i] = descriptorData.hDescriptorSetLayout;

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = m_hMainDescriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
        descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

        if (vkAllocateDescriptorSets(m_pInstanceCreator->GetDevice(), &descriptorSetAllocateInfo, descriptorData.descriptorSets.data()) != VK_SUCCESS)
            throw RendererException("vkAllocateDescriptorSets() failed to allocate shader descriptor sets");
    }


    //void VulkanRenderer::_AllocateMaterialDescriptors(hash_t _hshMaterial) {
    //    m_materialDescriptors.emplace(
    //        std::piecewise_construct,
    //        std::forward_as_tuple(_hshMaterial),
    //        std::forward_as_tuple());
    //
    //    auto& descriptorSets = m_materialDescriptors[_hshMaterial];
    //    DENG_ASSERT(m_hMaterialDescriptorSetLayout != VK_NULL_HANDLE);
    //
    //    _CheckAndAllocateDescriptorPool(MAX_FRAMES_IN_FLIGHT);
    //
    //    // check if diffuse and specular maps have image handles
    //    ResourceManager& resourceManager = ResourceManager::GetInstance();
    //    auto pMaterial = resourceManager.GetMaterial(_hshMaterial);
    //    DENG_ASSERT(pMaterial);
    //
    //    if (pMaterial->hshDiffuseMap && m_textureHandles.find(pMaterial->hshDiffuseMap) == m_textureHandles.end())
    //        _CreateApiImageHandles(pMaterial->hshDiffuseMap);
    //    if (pMaterial->hshSpecularMap && m_textureHandles.find(pMaterial->hshSpecularMap) == m_textureHandles.end())
    //        _CreateApiImageHandles(pMaterial->hshSpecularMap);
    //
    //    std::array<VkDescriptorSetLayout, MAX_FRAMES_IN_FLIGHT> descriptorSetLayouts;
    //    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    //        descriptorSetLayouts[i] = m_hMaterialDescriptorSetLayout;
    //
    //    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    //    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    //    descriptorSetAllocateInfo.descriptorPool = m_hMainDescriptorPool;
    //    descriptorSetAllocateInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
    //    descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();
    //
    //    if (vkAllocateDescriptorSets(m_pInstanceCreator->GetDevice(), &descriptorSetAllocateInfo, descriptorSets.data()) != VK_SUCCESS)
    //        throw RendererException("vkAllocateDescriptorSets() failed to allocate material descriptor sets");
    //
    //    // query texture resources
    //    Vulkan::TextureData& diffuse = pMaterial->hshDiffuseMap ? m_textureHandles[pMaterial->hshDiffuseMap] : m_textureHandles[m_hshMissing2DTexture];
    //    Vulkan::TextureData& specular = pMaterial->hshSpecularMap ? m_textureHandles[pMaterial->hshSpecularMap] : m_textureHandles[m_hshMissing2DTexture];
    //
    //    std::array<VkDescriptorImageInfo, 2> descriptorImageInfos = {};
    //    descriptorImageInfos[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //    descriptorImageInfos[0].imageView = diffuse.hImageView;
    //    descriptorImageInfos[0].sampler = diffuse.hSampler;
    //
    //    descriptorImageInfos[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //    descriptorImageInfos[1].imageView = specular.hImageView;
    //    descriptorImageInfos[1].sampler = diffuse.hSampler;
    //
    //    std::array<VkWriteDescriptorSet, 2 * MAX_FRAMES_IN_FLIGHT> writeDescriptors = {};
    //    for (size_t i = 0; i < m_materialDescriptors[_hshMaterial].size(); i++) {
    //        writeDescriptors[i * 2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //        writeDescriptors[i * 2].dstSet = m_materialDescriptors[_hshMaterial][i];
    //        writeDescriptors[i * 2].dstBinding = 0;
    //        writeDescriptors[i * 2].descriptorCount = 1;
    //        writeDescriptors[i * 2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    //        writeDescriptors[i * 2].pImageInfo = &descriptorImageInfos[0];
    //
    //        writeDescriptors[i * 2 + 1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //        writeDescriptors[i * 2 + 1].dstSet = m_materialDescriptors[_hshMaterial][i];
    //        writeDescriptors[i * 2 + 1].dstBinding = 1;
    //        writeDescriptors[i * 2 + 1].descriptorCount = 1;
    //        writeDescriptors[i * 2 + 1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    //        writeDescriptors[i * 2 + 1].pImageInfo = &descriptorImageInfos[1];
    //    }
    //
    //    vkUpdateDescriptorSets(
    //        m_pInstanceCreator->GetDevice(),
    //        static_cast<uint32_t>(writeDescriptors.size()),
    //        writeDescriptors.data(),
    //        0, nullptr);
    //}

    void VulkanRenderer::_UpdateShaderDescriptorSet(
        VkDescriptorSet _hDescriptorSet, 
        const IShader* _pShader) 
    {
        std::vector<VkDescriptorBufferInfo> descriptorBufferInfos;
        std::vector<VkDescriptorImageInfo> descriptorImageInfos;
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;

        descriptorBufferInfos.reserve(_pShader->GetUniformDataLayouts().size());
        descriptorImageInfos.reserve(_pShader->GetUniformDataLayouts().size());
        writeDescriptorSets.reserve(_pShader->GetUniformDataLayouts().size());

        auto missing2DTextureHandles = m_textureHandles.find(m_hshMissing2DTexture);
        auto missing3DTextureHandles = m_textureHandles.find(m_hshMissing3DTexture);

        DENG_ASSERT(missing2DTextureHandles != m_textureHandles.end());
        DENG_ASSERT(missing3DTextureHandles != m_textureHandles.end());

        size_t uTextureCounter = 0;
        for (size_t i = 0; i < _pShader->GetUniformDataLayouts().size(); i++) {
            switch (_pShader->GetUniformDataLayouts()[i].eType) {
                case UniformDataType::Buffer:
                case UniformDataType::StorageBuffer:
                    descriptorBufferInfos.emplace_back();
                    descriptorBufferInfos.back().buffer = m_mainBuffer.hBuffer;
                    descriptorBufferInfos.back().offset = _pShader->GetUniformDataLayouts()[i].block.uOffset;
                    descriptorBufferInfos.back().range = _pShader->GetUniformDataLayouts()[i].block.uSize;
                    break;

                case UniformDataType::ImageSampler2D:
                {
                    descriptorImageInfos.emplace_back();
                    descriptorImageInfos.back().imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    
                    if (!_pShader->GetTextureHash(uTextureCounter) || m_textureHandles.find(_pShader->GetTextureHash(uTextureCounter)) == m_textureHandles.end()) {
                        descriptorImageInfos.back().imageView = missing2DTextureHandles->second.hImageView;
                        descriptorImageInfos.back().sampler = missing2DTextureHandles->second.hSampler;
                    }
                    else {
                        auto textureHandles = m_textureHandles.find(_pShader->GetTextureHash(uTextureCounter));
                        descriptorImageInfos.back().imageView = textureHandles->second.hImageView;
                        descriptorImageInfos.back().sampler = textureHandles->second.hSampler;
                    }
                    ++uTextureCounter;
                    break;
                }

                case UniformDataType::ImageSampler3D:
                {
                    descriptorImageInfos.emplace_back();
                    descriptorImageInfos.back().imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                    if (!_pShader->GetTextureHash(uTextureCounter) || m_textureHandles.find(_pShader->GetTextureHash(uTextureCounter)) == m_textureHandles.end()) {
                        descriptorImageInfos.back().imageView = missing3DTextureHandles->second.hImageView;
                        descriptorImageInfos.back().sampler = missing3DTextureHandles->second.hSampler;
                    }
                    else {
                        auto textureHandles = m_textureHandles.find(_pShader->GetTextureHash(uTextureCounter));
                        descriptorImageInfos.back().imageView = textureHandles->second.hImageView;
                        descriptorImageInfos.back().sampler = textureHandles->second.hSampler;
                    }

                    ++uTextureCounter;
                    break;
                }

                default:
                    DENG_ASSERT(false);
                    break;
            }
        }

        size_t uBufferCounter = 0;
        uTextureCounter = 0;

        for (size_t i = 0; i < _pShader->GetUniformDataLayouts().size(); i++) {
            writeDescriptorSets.emplace_back();
            writeDescriptorSets.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets.back().dstSet = _hDescriptorSet;
            writeDescriptorSets.back().dstBinding = _pShader->GetUniformDataLayouts()[i].block.uBinding;
            writeDescriptorSets.back().dstArrayElement = 0;
            writeDescriptorSets.back().descriptorCount = 1;

            switch (_pShader->GetUniformDataLayouts()[i].eType) {
                case UniformDataType::Buffer:
                    writeDescriptorSets.back().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    writeDescriptorSets.back().pBufferInfo = &descriptorBufferInfos[uBufferCounter++];
                    break;

                case UniformDataType::StorageBuffer:
                    writeDescriptorSets.back().descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    writeDescriptorSets.back().pBufferInfo = &descriptorBufferInfos[uBufferCounter++];
                    break;

                case UniformDataType::ImageSampler2D:
                case UniformDataType::ImageSampler3D:
                    writeDescriptorSets.back().descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    writeDescriptorSets.back().pImageInfo = &descriptorImageInfos[uTextureCounter++];
                    break;

                default:
                    DENG_ASSERT(false);
                    break;
            }
        }

        vkUpdateDescriptorSets(m_pInstanceCreator->GetDevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
    }


    void VulkanRenderer::_AllocateDescriptorPool() {
        std::array<VkDescriptorPoolSize, 3> descriptorPoolSizes = {};
        descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorPoolSizes[0].descriptorCount = m_uDescriptorPoolCapacity;
        descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorPoolSizes[1].descriptorCount = m_uDescriptorPoolCapacity;
        descriptorPoolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorPoolSizes[2].descriptorCount = m_uDescriptorPoolCapacity;

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
        descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
        descriptorPoolCreateInfo.maxSets = m_uDescriptorPoolCapacity;

        if (vkCreateDescriptorPool(m_pInstanceCreator->GetDevice(), &descriptorPoolCreateInfo, nullptr, &m_hMainDescriptorPool) != VK_SUCCESS)
            throw RendererException("vkCreateDescriptorPool() failed to create descriptor pool");
    }


    void VulkanRenderer::_CheckAndAllocateDescriptorPool(size_t _uRequest) {
        if (m_uDescriptorPoolUsage + _uRequest >= m_uDescriptorPoolCapacity) {
            m_fullDescriptorPools.push_back(m_hMainDescriptorPool);
            m_uDescriptorPoolCapacity = ((m_uDescriptorPoolUsage + static_cast<uint32_t>(_uRequest)) * 3) >> 1;
            
            _AllocateDescriptorPool();
        }
    }


    void VulkanRenderer::DeleteTextureHandles() {
        for (auto it = m_textureHandles.begin(); it != m_textureHandles.end(); it++) {
            vkDestroySampler(m_pInstanceCreator->GetDevice(), it->second.hSampler, nullptr);
            vkDestroyImageView(m_pInstanceCreator->GetDevice(), it->second.hImageView, nullptr);
            vkDestroyImage(m_pInstanceCreator->GetDevice(), it->second.hImage, nullptr);
            vkFreeMemory(m_pInstanceCreator->GetDevice(), it->second.hMemory, nullptr);
        }

        m_textureHandles.clear();
    }


    void VulkanRenderer::UpdateViewport(uint32_t _uWidth, uint32_t _uHeight) {
        if (!m_bResizeModeTriggered) {
            m_resizeBeginTimestamp = std::chrono::high_resolution_clock::now();
            m_bResizeModeTriggered = true;
        }

        m_uResizedViewportWidth = _uWidth;
        m_uResizedViewportHeight = _uHeight;
        LOG("VulkanRenderer::UpdateViewport(): new viewport dimentions are " << std::dec << m_uResizedViewportWidth << 'x' << std::dec << m_uResizedViewportHeight);
    }

    
    void VulkanRenderer::DestroyPipeline(hash_t _hshShader) {
        m_pipelineCreators.erase(_hshShader);
    }

    IFramebuffer* VulkanRenderer::CreateFramebuffer(uint32_t _uWidth, uint32_t _uHeight) {
        Vulkan::Framebuffer* pFramebuffer = new Vulkan::Framebuffer(
            m_pInstanceCreator, 
            m_mainBuffer.hBuffer,
            m_uSampleCountBits, 
            TRS::Point2D<uint32_t>(_uWidth, _uHeight),
            false);

        return pFramebuffer;
    }

    IFramebuffer* VulkanRenderer::CreateContext(IWindowContext* _pWindow) {
        DENG_ASSERT(_pWindow);

        m_pInstanceCreator = new Vulkan::InstanceCreator(*_pWindow);

        m_mainBuffer.uSize = DEFAULT_BUFFER_SIZE;
        m_stagingBuffer.uSize = DEFAULT_STAGING_BUFFER_SIZE;

        // allocate staging buffer memory
        VkMemoryRequirements memoryRequirements = Vulkan::_CreateBuffer(
            m_pInstanceCreator->GetDevice(),
            m_stagingBuffer.uSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            m_stagingBuffer.hBuffer);

        Vulkan::_AllocateMemory(
            m_pInstanceCreator->GetDevice(),
            m_pInstanceCreator->GetPhysicalDevice(),
            memoryRequirements.size,
            m_stagingBuffer.hMemory,
            memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        
        vkBindBufferMemory(m_pInstanceCreator->GetDevice(), m_stagingBuffer.hBuffer, m_stagingBuffer.hMemory, 0);

        // allocate main buffer memory
        memoryRequirements = Vulkan::_CreateBuffer(
            m_pInstanceCreator->GetDevice(),
            m_mainBuffer.uSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            m_mainBuffer.hBuffer);

        Vulkan::_AllocateMemory(
            m_pInstanceCreator->GetDevice(),
            m_pInstanceCreator->GetPhysicalDevice(),
            memoryRequirements.size,
            m_mainBuffer.hMemory,
            memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vkBindBufferMemory(m_pInstanceCreator->GetDevice(), m_mainBuffer.hBuffer, m_mainBuffer.hMemory, 0);

        Vulkan::Framebuffer* pFramebuffer = new Vulkan::Framebuffer(
            m_pInstanceCreator,
            m_mainBuffer.hBuffer,
            m_uSampleCountBits,
            TRS::Point2D<uint32_t>(_pWindow->GetWidth(), _pWindow->GetHeight()),
            true);

        m_framebuffers.push_back(pFramebuffer);

        // add missing textures
        m_hshMissing2DTexture = SID("__MissingTexture2D__");
        m_hshMissing3DTexture = SID("__MissingTexture3D__");

        ResourceManager& resourceManager = ResourceManager::GetInstance();
        resourceManager.AddTexture<MissingTextureBuilder2D>(m_hshMissing2DTexture);
        resourceManager.AddTexture<MissingTextureBuilder3D>(m_hshMissing3DTexture);
        _CreateApiImageHandles(m_hshMissing2DTexture);
        _CreateApiImageHandles(m_hshMissing3DTexture);

        // create material descriptor set layout
        _CreateMaterialDescriptorSetLayout();
        _AllocateDescriptorPool();

        return pFramebuffer;
    }

    size_t VulkanRenderer::AllocateMemory(size_t _uSize, BufferDataType _eType) {
        DENG_ASSERT(m_pInstanceCreator);

        switch (_eType) {
            case BufferDataType::Vertex:
                return m_gpuMemoryAllocator.RequestMemory(_uSize, sizeof(uint32_t)).uOffset;

            case BufferDataType::Index:
                return m_gpuMemoryAllocator.RequestMemory(_uSize, sizeof(uint32_t)).uOffset;

            case BufferDataType::Uniform:
                return m_gpuMemoryAllocator.RequestMemory(_uSize, m_pInstanceCreator->GetPhysicalDeviceInformation().uMinimalUniformBufferAlignment).uOffset;
        
            default:
                break;
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
            m_stagingBuffer.hMemory, 0);

        // staging to main
        Vulkan::_CopyBufferToBuffer(
            m_pInstanceCreator->GetDevice(),
            hCommandPool,
            m_pInstanceCreator->GetGraphicsQueue(),
            m_stagingBuffer.hBuffer,
            m_mainBuffer.hBuffer,
            _uSize,
            0, _uOffset);
    }


    bool VulkanRenderer::SetupFrame() {
        // check if resize mode is active
        if (m_bResizeModeTriggered) {
            m_resizeEndTimestamp = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float, std::milli> timestep = m_resizeEndTimestamp - m_resizeBeginTimestamp;
            
            if (timestep.count() >= RESIZE_DEBOUNCE_TIMESTEP && m_uResizedViewportWidth && m_uResizedViewportHeight) {
                vkDeviceWaitIdle(m_pInstanceCreator->GetDevice());
                m_bResizeModeTriggered = false;
                LOG("Resized viewport dimentions: " << m_uResizedViewportWidth << 'x' << m_uResizedViewportHeight);
                m_pInstanceCreator->UpdateSurfaceProperties();
                static_cast<Vulkan::Framebuffer*>(m_framebuffers[0])->RecreateFramebuffer(m_uResizedViewportWidth, m_uResizedViewportHeight);
            }
            else {
                return false;
            }
        }

        // delete all previously reallocated buffers
        if (m_deletedBuffers.size()) {
            vkDeviceWaitIdle(m_pInstanceCreator->GetDevice());
            for (auto it = m_deletedBuffers.begin(); it != m_deletedBuffers.end(); it++) {
                vkFreeMemory(m_pInstanceCreator->GetDevice(), it->hMemory, nullptr);
                vkDestroyBuffer(m_pInstanceCreator->GetDevice(), it->hBuffer, nullptr);
            }

            m_deletedBuffers.clear();
        }

        return true;
    }


    void VulkanRenderer::DrawInstance(
        hash_t _hshMesh, 
        hash_t _hshShader,
        IFramebuffer* _pFramebuffer, 
        uint32_t _uInstanceCount,
        uint32_t _uFirstInstance,
        hash_t _hshMaterial) 
    {
        DENG_ASSERT(_pFramebuffer);
        ResourceManager& resourceManager = ResourceManager::GetInstance();
        auto pShader = resourceManager.GetShader(_hshShader);
        DENG_ASSERT(pShader);

        // check if any requested textures are not handles
        for (size_t i = 0; pShader->GetTextureHash(i) != 0; i++) {
            const Texture* texture = resourceManager.GetTexture(pShader->GetTextureHash(i));
            DENG_ASSERT(texture);

            if ((texture->eResourceType == TextureType::Image_2D || texture->eResourceType == TextureType::Image_3D || texture->eResourceType == TextureType::Image_3D_Array) &&
                m_textureHandles.find(pShader->GetTextureHash(i)) == m_textureHandles.end())
            {
                _CreateApiImageHandles(pShader->GetTextureHash(i));
            }
        }

        if (m_bResizeModeTriggered)
            return;
        
        Vulkan::Framebuffer* vulkanFramebuffer = static_cast<Vulkan::Framebuffer*>(_pFramebuffer);

        // shader descriptor sets are required but do not exist
        if (pShader->GetUniformDataLayouts().size() && m_shaderDescriptors.find(_hshShader) == m_shaderDescriptors.end()) {
            _CreateShaderDescriptorSetLayout(&m_shaderDescriptors[_hshShader].hDescriptorSetLayout, _hshShader);
            _AllocateShaderDescriptors(_hshShader);
        }

        if (pShader->GetUniformDataLayouts().size()) {
            _UpdateShaderDescriptorSet(m_shaderDescriptors[_hshShader].descriptorSets[vulkanFramebuffer->GetCurrentFrameIndex()], pShader);
        }

        // check if material should be added
        //if (_hshMaterial && m_materialDescriptors.find(_hshMaterial) == m_materialDescriptors.end()) {
        //    _AllocateMaterialDescriptors(_hshMaterial);
        //}

        
        if (m_shaderDescriptors.find(_hshShader) != m_shaderDescriptors.end() && _hshMaterial) {
            vulkanFramebuffer->Draw(
                _hshMesh, 
                _hshShader, 
                _uInstanceCount,
                _uFirstInstance,
                m_shaderDescriptors[_hshShader].descriptorSets[vulkanFramebuffer->GetCurrentFrameIndex()], 
                //m_materialDescriptors[_hshMaterial][vulkanFramebuffer->GetCurrentFrameIndex()],
                VK_NULL_HANDLE,
                m_shaderDescriptors[_hshShader].hDescriptorSetLayout,
                m_hMaterialDescriptorSetLayout);
        }
        else if (m_shaderDescriptors.find(_hshShader) != m_shaderDescriptors.end()) {
            vulkanFramebuffer->Draw(
                _hshMesh, 
                _hshShader, 
                _uInstanceCount,
                _uFirstInstance,
                m_shaderDescriptors[_hshShader].descriptorSets[vulkanFramebuffer->GetCurrentFrameIndex()],
                VK_NULL_HANDLE,
                m_shaderDescriptors[_hshShader].hDescriptorSetLayout,
                m_hMaterialDescriptorSetLayout);
        }
        else if (m_shaderDescriptors.find(_hshShader) == m_shaderDescriptors.end() && _hshMaterial) {
            vulkanFramebuffer->Draw(
                _hshMesh,
                _hshShader,
                _uInstanceCount,
                _uFirstInstance,
                VK_NULL_HANDLE,
                VK_NULL_HANDLE,
                //m_materialDescriptors[_hshMaterial][vulkanFramebuffer->GetCurrentFrameIndex()],
                m_shaderDescriptors[_hshShader].hDescriptorSetLayout,
                m_hMaterialDescriptorSetLayout);
        }
        else {
            vulkanFramebuffer->Draw(
                _hshMesh,
                _hshShader,
                _uInstanceCount,
                _uFirstInstance,
                VK_NULL_HANDLE,
                VK_NULL_HANDLE,
                m_shaderDescriptors[_hshShader].hDescriptorSetLayout,
                m_hMaterialDescriptorSetLayout);
        }
    }
}