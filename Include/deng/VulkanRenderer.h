// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanRenderer.h - Vulkan renderer class header
// author: Karl-Mihkel Ott

#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H


#ifdef VULKAN_RENDERER_CPP
    #include <string>
    #include <vector>
    #include <array>
    #include <cstring>
    #include <queue>
    #include <list>
    #include <unordered_map>

    #include <algorithm>
    #include <iostream>
    #include <iomanip>
    #include <vulkan/vulkan.h>

    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Points.h"
    #include "trs/Quaternion.h"

    #include "deng/Api.h"
    #include "deng/Exceptions.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/Missing.h"
    #include "deng/RenderResources.h"
    #include "deng/MissingTextureBuilder.h"

    #define CALC_MIPLVL(_x, _y) (static_cast<uint32_t>(std::floor(std::log2(std::max(static_cast<double>(_x), static_cast<double>(_y))))))
#endif

#ifndef RESIZE_DEBOUNCE_TIMESTEP
#define RESIZE_DEBOUNCE_TIMESTEP 100.f // ms
#endif

#include "deng/SID.h"
#include "deng/ErrorDefinitions.h"
#include "deng/IRenderer.h"
#include "deng/VulkanHelpers.h"
#include "deng/VulkanInstanceCreator.h"
#include "deng/VulkanSwapchainCreator.h"
#include "deng/VulkanPipelineCreator.h"
#include "deng/VulkanDescriptorAllocator.h"
#include "deng/VulkanFramebuffer.h"
#include "deng/ResourceEvents.h"


namespace DENG {

    class DENG_API VulkanRenderer : public IRenderer {
        private:
            const VkSampleCountFlagBits m_uSampleCountBits = VK_SAMPLE_COUNT_1_BIT;

            Vulkan::InstanceCreator* m_pInstanceCreator = nullptr;
            std::unordered_map<hash_t, Vulkan::PipelineCreator, NoHash> m_pipelineCreators;
            std::unordered_map<hash_t, Vulkan::TextureData, NoHash> m_textureHandles;

            Vulkan::BufferData m_mainBuffer;
            Vulkan::BufferData m_stagingBuffer;

            VkDescriptorPool m_hMainDescriptorPool = VK_NULL_HANDLE;
            uint32_t m_uDescriptorPoolUsage = 0;
            uint32_t m_uDescriptorPoolCapacity = 100 * MAX_FRAMES_IN_FLIGHT;   // arbitrary number rn
            
            std::unordered_map<hash_t, Vulkan::ShaderDescriptorData, NoHash> m_shaderDescriptors;

            std::unordered_map<size_t, VkDescriptorSetLayout> m_materialDescriptorSetLayouts;
            std::unordered_map<hash_t, std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT>, NoHash> m_materialDescriptors;
            std::unordered_map<hash_t, bool, NoHash> m_shaderDescriptorUpdateTable;

            std::vector<VkDescriptorPool> m_fullDescriptorPools;
            std::vector<Vulkan::BufferData> m_deletedBuffers;

            uint32_t m_uResizedViewportWidth = 0;
            uint32_t m_uResizedViewportHeight = 0;
            bool m_bResizeModeTriggered = false;

            std::chrono::time_point<std::chrono::high_resolution_clock> m_resizeBeginTimestamp =
                std::chrono::high_resolution_clock::now();
            std::chrono::time_point<std::chrono::high_resolution_clock> m_resizeEndTimestamp =
                std::chrono::high_resolution_clock::now();

        private:
            void _CreateApiImageHandles(hash_t _id);
            void _CheckAndReallocateBufferResources(size_t _uSize, size_t _uOffset);
            void _CreateMaterialDescriptorSetLayout(size_t _uCount);
            void _CreateShaderDescriptorSetLayout(VkDescriptorSetLayout* _pDescriptorSetLayout, hash_t _hshShader);
            void _AllocateShaderDescriptors(hash_t _hshShader);

            template<typename T, size_t N>
            void _AllocateMaterialDescriptors(hash_t _hshMaterial, const Material<T, N>& _material) {
                m_materialDescriptors.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(_hshMaterial),
                    std::forward_as_tuple());

                auto& descriptorSets = m_materialDescriptors[_hshMaterial];
                if (m_materialDescriptorSetLayouts.find(N) == m_materialDescriptorSetLayouts.end()) {
                    _CreateMaterialDescriptorSetLayout(N);
                }

                // create api texture handles if necessary
                for (auto it = _material.textures.begin(); it != _material.textures.end(); it++) {
                    if (m_textureHandles.find(*it) == m_textureHandles.end())
                        _CreateApiImageHandles(*it);
                }

                std::array<VkDescriptorSetLayout, MAX_FRAMES_IN_FLIGHT> descriptorSetLayouts = {};
                for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
                    descriptorSetLayouts[i] = m_materialDescriptorSetLayouts[N];

                VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
                descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                descriptorSetAllocateInfo.descriptorPool = m_hMainDescriptorPool;
                descriptorSetAllocateInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
                descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

                if (vkAllocateDescriptorSets(m_pInstanceCreator->GetDevice(), &descriptorSetAllocateInfo, descriptorSets.data()))
                    throw RendererException("vkAllocateDescriptorSets() failed to allocate material descriptor sets");
            
                std::array<VkDescriptorImageInfo, N> descriptorImageInfos = {};
                for (size_t i = 0; i < N; i++) {
                    Vulkan::TextureData& texture = m_textureHandles[_material.textures[i]];

                    descriptorImageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    descriptorImageInfos[i].imageView = texture.hImageView;
                    descriptorImageInfos[i].sampler = texture.hSampler;
                }

                std::array<VkWriteDescriptorSet, N* MAX_FRAMES_IN_FLIGHT> writeDescriptors = {};
                for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                    for (size_t j = 0; j < N; j++) {
                        writeDescriptors[i * N + j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        writeDescriptors[i * N + j].dstSet = descriptorSets[i];
                        writeDescriptors[i * N + j].dstBinding = static_cast<uint32_t>(j);
                        writeDescriptors[i * N + j].descriptorCount = 1;
                        writeDescriptors[i * N + j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        writeDescriptors[i * N + j].pImageInfo = &descriptorImageInfos[j];
                    }
                }

                vkUpdateDescriptorSets(
                    m_pInstanceCreator->GetDevice(),
                    static_cast<uint32_t>(writeDescriptors.size()),
                    writeDescriptors.data(),
                    0, nullptr);
            }
            
            void _UpdateShaderDescriptorSet(VkDescriptorSet _hDescriptorSet, const IShader* _pShader);
            void _AllocateDescriptorPool();
            void _CheckAndAllocateDescriptorPool(size_t _uRequest);

        public:
            VulkanRenderer() = default;
            VulkanRenderer(VulkanRenderer&&) noexcept = default;
            ~VulkanRenderer();

            virtual void DeleteTextureHandles() override;
            virtual void UpdateViewport(uint32_t _uWidth, uint32_t _uHeight) override;
            virtual void DestroyPipeline(hash_t _hshShader) override;
            virtual IFramebuffer* CreateFramebuffer(uint32_t _uWidth, uint32_t _uHeight) override;
            virtual IFramebuffer* CreateContext(IWindowContext* _pWindow) override;
            virtual size_t AllocateMemory(size_t _uSize, BufferDataType _eType) override;
            virtual void DeallocateMemory(size_t _uOffset) override;
            virtual void UpdateBuffer(const void* _pData, size_t _uSize, size_t _uOffset) override;
            virtual bool SetupFrame() override;
            virtual void DrawInstance(
                hash_t _hshMesh,
                hash_t _hshShader,
                IFramebuffer* _pFramebuffer,
                uint32_t _uInstanceCount,
                uint32_t _uFirstInstance,
                hash_t _hshMaterialDescriptor = 0) override;
    };
}

#endif
