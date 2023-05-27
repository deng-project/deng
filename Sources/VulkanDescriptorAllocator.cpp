// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: DescriptorAllocator.h - Vulkan descriptor allocation management class implementation
// author: Karl-Mihkel Ott

#define VULKAN_DESCRIPTOR_ALLOCATOR_CPP
#include "deng/VulkanDescriptorAllocator.h"


namespace DENG {
    namespace Vulkan {

        DescriptorAllocator::DescriptorAllocator(
            VkDevice _hDevice,
            VkBuffer _hMainBuffer,
            const std::unordered_map<uint32_t, TextureResource>& _textureRegistry,
            const std::vector<UniformDataLayout>& _uniformDataLayouts,
            uint32_t _uBufferingStageCount,
            uint32_t _uMissing2DTextureId,
            uint32_t _uMissing3DTextureId
        ) : m_hDevice(_hDevice),
            m_textureRegistry(_textureRegistry),
            m_uniformDataLayouts(_uniformDataLayouts),
            m_uBufferingStageCount(_uBufferingStageCount),
            m_u2DMissingTextureId(_uMissing2DTextureId),
            m_u3DMissingTextureId(_uMissing3DTextureId)
        {
            try {
                _CreateDescriptorSetLayouts();
                _CreateShaderDescriptorPool();
                _AllocateNewMeshDescriptorPool();
                _AllocateShaderDescriptorSets();
            }
            catch (const RendererException& e) {
                DISPATCH_ERROR_MESSAGE("RendererException", e.what(), ErrorSeverity::CRITICAL);
            }
        }


        DescriptorAllocator::DescriptorAllocator(DescriptorAllocator&& _descriptorAllocator) noexcept :
            m_hDevice(_descriptorAllocator.m_hDevice),
            m_textureRegistry(_descriptorAllocator.m_textureRegistry),
            m_uniformDataLayouts(_descriptorAllocator.m_uniformDataLayouts),
            m_u2DMissingTextureId(_descriptorAllocator.m_u2DMissingTextureId),
            m_u3DMissingTextureId(_descriptorAllocator.m_u3DMissingTextureId),
            m_hShaderDescriptorPool(_descriptorAllocator.m_hShaderDescriptorPool),
            m_shaderDescriptorSets(std::move(_descriptorAllocator.m_shaderDescriptorSets)),
            m_meshDescriptorPools(std::move(_descriptorAllocator.m_meshDescriptorPools)),
            m_meshDescriptorSets(std::move(_descriptorAllocator.m_meshDescriptorSets)),
            m_uMeshCounter(_descriptorAllocator.m_uMeshCounter),
            m_uMeshMasterPoolCapacity(_descriptorAllocator.m_uMeshMasterPoolCapacity),
            m_uBufferingStageCount(_descriptorAllocator.m_uBufferingStageCount),
            m_hShaderDescriptorSetLayout(_descriptorAllocator.m_hShaderDescriptorSetLayout),
            m_hMeshDescriptorSetLayout(_descriptorAllocator.m_hMeshDescriptorSetLayout)
        {
            m_hShaderDescriptorPool = VK_NULL_HANDLE;
            m_hShaderDescriptorSetLayout = VK_NULL_HANDLE;
            m_hMeshDescriptorSetLayout = VK_NULL_HANDLE;
        }



        DescriptorAllocator::~DescriptorAllocator() noexcept {
            if (m_hShaderDescriptorPool != VK_NULL_HANDLE)
                vkDestroyDescriptorPool(m_hDevice, m_hShaderDescriptorPool, nullptr);

            for (VkDescriptorPool descriptorPool : m_meshDescriptorPools) {
                if (descriptorPool != VK_NULL_HANDLE)
                    vkDestroyDescriptorPool(m_hDevice, descriptorPool, nullptr);
            }

            if (m_hShaderDescriptorSetLayout)
                vkDestroyDescriptorSetLayout(m_hDevice, m_hShaderDescriptorSetLayout, nullptr);
            if (m_hMeshDescriptorSetLayout)
                vkDestroyDescriptorSetLayout(m_hDevice, m_hMeshDescriptorSetLayout, nullptr);
        }


        void DescriptorAllocator::_CreateDescriptorSetLayouts() {
            std::vector<VkDescriptorSetLayoutBinding> shaderBindings;
            std::vector<VkDescriptorSetLayoutBinding> meshBindings;

            shaderBindings.reserve(m_uniformDataLayouts.size());
            meshBindings.reserve(m_uniformDataLayouts.size());

            for (auto it = m_uniformDataLayouts.begin(); it != m_uniformDataLayouts.end(); it++) {
                VkDescriptorSetLayoutBinding* pBinding = nullptr;
                if (it->eUsage == UNIFORM_USAGE_PER_MESH) {
                    meshBindings.emplace_back();
                    pBinding = &meshBindings.back();
                }
                else {
                    shaderBindings.emplace_back();
                    pBinding = &shaderBindings.back();
                }


                switch (it->eType) {
                    case UNIFORM_DATA_TYPE_BUFFER:
                        pBinding->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        break;

                    case UNIFORM_DATA_TYPE_STORAGE_BUFFER:
                        pBinding->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                        break;

                    case UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER:
                    case UNIFORM_DATA_TYPE_3D_IMAGE_SAMPLER:
                        pBinding->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        break;

                    default:
                        DENG_ASSERT(false);
                        break;
                }

                pBinding->pImmutableSamplers = nullptr;
                pBinding->stageFlags = 0;
                pBinding->binding = it->block.uBinding;
                pBinding->descriptorCount = 1;

                if (it->iStage & SHADER_STAGE_VERTEX)
                    pBinding->stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
                if (it->iStage & SHADER_STAGE_GEOMETRY)
                    pBinding->stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
                if (it->iStage & SHADER_STAGE_FRAGMENT)
                    pBinding->stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
            }

            VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
            descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

            // create per-shader descriptor set layout
            if (shaderBindings.size()) {
                descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(shaderBindings.size());
                descriptorSetLayoutCreateInfo.pBindings = shaderBindings.data();
                if (vkCreateDescriptorSetLayout(m_hDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_hShaderDescriptorSetLayout) != VK_SUCCESS)
                    throw RendererException("vkCreateDescriptorSetLayout() failed to create a per-shader descriptor set layout");
            }

            // create per-shader descriptor set layout
            if (meshBindings.size()) {
                descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(meshBindings.size());
                descriptorSetLayoutCreateInfo.pBindings = meshBindings.data();

                if (vkCreateDescriptorSetLayout(m_hDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_hMeshDescriptorSetLayout) != VK_SUCCESS)
                    throw RendererException("vkCreateDescriptorSetLayout() failed to create a per-mesh descriptor set layout");
            }
        }

        void DescriptorAllocator::_CreateShaderDescriptorPool() {
            std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
            descriptorPoolSizes.reserve(m_uniformDataLayouts.size());

            for (size_t i = 0; i < m_uniformDataLayouts.size(); i++) {
                if (m_uniformDataLayouts[i].eUsage == UNIFORM_USAGE_PER_SHADER) {
                    descriptorPoolSizes.emplace_back();
                    switch (m_uniformDataLayouts[i].eType) {
                        case UNIFORM_DATA_TYPE_BUFFER:
                            descriptorPoolSizes.back().type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                            break;

                        case UNIFORM_DATA_TYPE_STORAGE_BUFFER:
                            descriptorPoolSizes.back().type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                            break;

                        case UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER:
                        case UNIFORM_DATA_TYPE_3D_IMAGE_SAMPLER:
                            descriptorPoolSizes.back().type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                            break;

                        default:
                            DENG_ASSERT(false);
                            break;
                    }

                    descriptorPoolSizes.back().descriptorCount = m_uBufferingStageCount;
                }
            }

            if (descriptorPoolSizes.size()) {
                VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
                descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
                descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
                descriptorPoolCreateInfo.maxSets = m_uBufferingStageCount;

                if (vkCreateDescriptorPool(m_hDevice, &descriptorPoolCreateInfo, nullptr, &m_hShaderDescriptorPool) != VK_SUCCESS)
                    throw RendererException("vkCreateDescriptorPool() failed to create shader descriptor pool");
            }
        }


        void DescriptorAllocator::_AllocateNewMeshDescriptorPool() {
            m_meshDescriptorPools.emplace_back();

            std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
            descriptorPoolSizes.reserve(m_uniformDataLayouts.size());

            for (size_t i = 0; i < m_uniformDataLayouts.size(); i++) {
                if (m_uniformDataLayouts[i].eUsage == UNIFORM_USAGE_PER_MESH) {
                    descriptorPoolSizes.emplace_back();
                    switch (m_uniformDataLayouts[i].eType) {
                        case UNIFORM_DATA_TYPE_BUFFER:
                            descriptorPoolSizes.back().type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                            break;

                        case UNIFORM_DATA_TYPE_STORAGE_BUFFER:
                            descriptorPoolSizes.back().type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                            break;

                        case UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER:
                        case UNIFORM_DATA_TYPE_3D_IMAGE_SAMPLER:
                            descriptorPoolSizes.back().type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                            break;

                        default:
                            DENG_ASSERT(false);
                            break;
                    }

                    descriptorPoolSizes.back().descriptorCount = m_uMeshMasterPoolCapacity * m_uBufferingStageCount;
                }
            }

            if (descriptorPoolSizes.size()) {
                VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
                descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
                descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
                descriptorPoolCreateInfo.maxSets = m_uMeshMasterPoolCapacity * m_uBufferingStageCount;

                if (vkCreateDescriptorPool(m_hDevice, &descriptorPoolCreateInfo, nullptr, &m_meshDescriptorPools.back()) != VK_SUCCESS)
                    throw RendererException("vkCreateDescriptorPool() failed to create mesh descriptor pool");
            }
        }


        void DescriptorAllocator::_AllocateShaderDescriptorSets() {
            m_shaderDescriptorSets.resize(m_uBufferingStageCount, VK_NULL_HANDLE);

            if (m_hShaderDescriptorPool != VK_NULL_HANDLE) {
                std::vector<VkDescriptorSetLayout> descriptorSetLayouts(m_uBufferingStageCount, m_hShaderDescriptorSetLayout);

                VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
                descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                descriptorSetAllocateInfo.descriptorPool = m_hShaderDescriptorPool;
                descriptorSetAllocateInfo.descriptorSetCount = m_uBufferingStageCount;
                descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

                if (vkAllocateDescriptorSets(m_hDevice, &descriptorSetAllocateInfo, m_shaderDescriptorSets.data()) != VK_SUCCESS)
                    throw RendererException("vkAllocateDescriptorSets() could not allocate shader descriptor sets");
            }
        }


        VkDescriptorSet DescriptorAllocator::RequestMeshDescriptorSet() {
            if (m_hMeshDescriptorSetLayout == VK_NULL_HANDLE)
                return VK_NULL_HANDLE;

            if (m_uMeshCounter >= static_cast<uint32_t>(m_meshDescriptorSets.size())) {

                // check if new pool allocation is required
                if (m_uMeshCounter >= static_cast<size_t>(m_uMeshMasterPoolCapacity * m_uBufferingStageCount) * m_meshDescriptorPools.size())
                    _AllocateNewMeshDescriptorPool();

                VkDescriptorPool descriptorPool = m_meshDescriptorPools.back();
                m_meshDescriptorSets.emplace_back();

                VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
                descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                descriptorSetAllocateInfo.descriptorPool = descriptorPool;
                descriptorSetAllocateInfo.descriptorSetCount = 1;
                descriptorSetAllocateInfo.pSetLayouts = &m_hMeshDescriptorSetLayout;

                if (vkAllocateDescriptorSets(m_hDevice, &descriptorSetAllocateInfo, &m_meshDescriptorSets.back()) != VK_SUCCESS)
                    throw RendererException("vkAllocateDescriptorSets() could not allocate a mesh descriptor set");
            }

            return m_meshDescriptorSets[m_uMeshCounter++];
        }


        void DescriptorAllocator::UpdateDescriptorSet(
            VkBuffer _hMainBuffer, 
            VkDescriptorSet _hDescriptorSet, 
            UniformUsage _eUsage, 
            size_t _uFrameIndex, 
            const std::vector<uint32_t>& _textures) 
        {
            std::vector<VkDescriptorBufferInfo> descriptorBufferInfos;
            std::vector<VkDescriptorImageInfo> descriptorImageInfos;
            std::vector<VkWriteDescriptorSet> writeDescriptorSets;

            descriptorBufferInfos.reserve(m_uniformDataLayouts.size());
            descriptorImageInfos.reserve(m_uniformDataLayouts.size());
            writeDescriptorSets.reserve(m_uniformDataLayouts.size());

            auto m_missing2DTexture = m_textureRegistry.find(m_u2DMissingTextureId);
            auto m_missing3DTexture = m_textureRegistry.find(m_u3DMissingTextureId);

            DENG_ASSERT(m_missing2DTexture != m_textureRegistry.end());
            DENG_ASSERT(m_missing3DTexture != m_textureRegistry.end());

            uint32_t uTextureCounter = 0;
            for (size_t i = 0; i < m_uniformDataLayouts.size(); i++) {
                if (m_uniformDataLayouts[i].eUsage == _eUsage) {
                    switch (m_uniformDataLayouts[i].eType) {
                        case UNIFORM_DATA_TYPE_BUFFER:
                            descriptorBufferInfos.emplace_back();
                            descriptorBufferInfos.back().buffer = _hMainBuffer;
                            descriptorBufferInfos.back().offset = m_uniformDataLayouts[i].block.uOffset;
                            descriptorBufferInfos.back().range = m_uniformDataLayouts[i].block.uSize;
                            break;

                        case UNIFORM_DATA_TYPE_STORAGE_BUFFER:
                            descriptorBufferInfos.emplace_back();
                            descriptorBufferInfos.back().buffer = _hMainBuffer;
                            descriptorBufferInfos.back().offset = m_uniformDataLayouts[i].block.uOffset + _uFrameIndex * m_uniformDataLayouts[i].block.uSize;
                            descriptorBufferInfos.back().range = m_uniformDataLayouts[i].block.uSize;
                            break;

                        case UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER:
                        {
                            descriptorImageInfos.emplace_back();
                            descriptorImageInfos.back().imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                            Vulkan::TextureData vkTextureData;

                            if (uTextureCounter >= static_cast<uint32_t>(_textures.size()) ||
                                m_textureRegistry.find(_textures[uTextureCounter]) == m_textureRegistry.end() ||
                                (m_textureRegistry.find(_textures[uTextureCounter])->second.eResourceType != TEXTURE_RESOURCE_2D_IMAGE &&
                                    m_textureRegistry.find(_textures[uTextureCounter])->second.eResourceType != TEXTURE_RESOURCE_INTERNAL_FRAMEBUFFER_2D_IMAGE))
                            {
                                vkTextureData = std::get<0>(m_missing2DTexture->second.apiTextureHandles);
                            }
                            else {
                                vkTextureData = std::get<0>(m_textureRegistry.find(_textures[uTextureCounter])->second.apiTextureHandles);
                            }

                            descriptorImageInfos.back().imageView = vkTextureData.hImageView;
                            descriptorImageInfos.back().sampler = vkTextureData.hSampler;
                            uTextureCounter++;
                            break;
                        }

                        case UNIFORM_DATA_TYPE_3D_IMAGE_SAMPLER:
                        {
                            descriptorImageInfos.emplace_back();
                            descriptorImageInfos.back().imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                            Vulkan::TextureData vkTextureData;

                            if (uTextureCounter >= static_cast<uint32_t>(_textures.size()) ||
                                m_textureRegistry.find(_textures[uTextureCounter]) == m_textureRegistry.end() ||
                                (m_textureRegistry.find(_textures[uTextureCounter])->second.eResourceType != TEXTURE_RESOURCE_3D_IMAGE &&
                                    m_textureRegistry.find(_textures[uTextureCounter])->second.eResourceType != TEXTURE_RESOURCE_INTERNAL_FRAMEBUFFER_3D_IMAGE))
                            {
                                vkTextureData = std::get<0>(m_missing3DTexture->second.apiTextureHandles);
                            }
                            else {
                                vkTextureData = std::get<0>(m_textureRegistry.find(_textures[uTextureCounter])->second.apiTextureHandles);
                            }

                            descriptorImageInfos.back().imageView = vkTextureData.hImageView;
                            descriptorImageInfos.back().sampler = vkTextureData.hSampler;
                            uTextureCounter++;
                            break;
                        }

                        default:
                            DENG_ASSERT(false);
                            break;
                    }
                }
            }

            // create write infos
            uint32_t uUsedBufferCount = 0;
            uint32_t uUsedImageCount = 0;

            for (auto it = m_uniformDataLayouts.begin(); it != m_uniformDataLayouts.end(); it++) {
                if (it->eUsage == _eUsage) {
                    writeDescriptorSets.emplace_back();
                    writeDescriptorSets.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    writeDescriptorSets.back().dstSet = _hDescriptorSet;
                    writeDescriptorSets.back().dstBinding = it->block.uBinding;
                    writeDescriptorSets.back().dstArrayElement = 0;
                    writeDescriptorSets.back().descriptorCount = 1;

                    switch (it->eType) {
                        case UNIFORM_DATA_TYPE_BUFFER:
                            writeDescriptorSets.back().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                            writeDescriptorSets.back().pBufferInfo = &descriptorBufferInfos[uUsedBufferCount++];
                            writeDescriptorSets.back().pImageInfo = nullptr;
                            break;

                        case UNIFORM_DATA_TYPE_STORAGE_BUFFER:
                            writeDescriptorSets.back().descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                            writeDescriptorSets.back().pBufferInfo = &descriptorBufferInfos[uUsedBufferCount++];
                            writeDescriptorSets.back().pImageInfo = nullptr;
                            break;

                        case UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER:
                        case UNIFORM_DATA_TYPE_3D_IMAGE_SAMPLER:
                            writeDescriptorSets.back().descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                            writeDescriptorSets.back().pImageInfo = &descriptorImageInfos[uUsedImageCount++];
                            writeDescriptorSets.back().pBufferInfo = nullptr;
                            break;

                        default:
                            DENG_ASSERT(false);
                            break;
                    }
                }
            }

            vkUpdateDescriptorSets(m_hDevice, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
        }


        void DescriptorAllocator::MergeMeshDescriptorPools() {
            if (m_meshDescriptorPools.size() > 1) {
                vkDeviceWaitIdle(m_hDevice);

                m_uMeshMasterPoolCapacity *= (static_cast<uint32_t>(m_meshDescriptorPools.size()) * 3) >> 1;

                // destroy all previous mesh descriptor pools
                for (size_t i = 0; i < m_meshDescriptorPools.size(); i++) {
                    vkDestroyDescriptorPool(m_hDevice, m_meshDescriptorPools[i], nullptr);
                }

                m_meshDescriptorPools.clear();
                _AllocateNewMeshDescriptorPool();
            }
        }
    }
}
