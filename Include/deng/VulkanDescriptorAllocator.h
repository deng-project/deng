// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: DescriptorAllocator.h - Vulkan descriptor allocation management class header
// author: Karl-Mihkel Ott

#ifndef VULKAN_DESCRIPTOR_ALLOCATOR_H
#define VULKAN_DESCRIPTOR_ALLOCATOR_H

#ifdef VULKAN_DESCRIPTOR_ALLOCATOR_CPP
    #include <string>
    #include <vector>
    #include <queue>
    #include <variant>
    #include <algorithm>
    #include <unordered_map>
    #include <variant>

    #include "deng/Api.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/Exceptions.h"
    #include "deng/ShaderComponent.h"
    #include "deng/IRenderer.h"
    #include "deng/VulkanHelpers.h"
    #include "deng/Missing.h"
#endif


namespace DENG {
    namespace Vulkan {

        class DescriptorAllocator {
            private:
                VkDevice m_hDevice = VK_NULL_HANDLE;
                const std::unordered_map<uint32_t, TextureResource>& m_textureRegistry;
                uint32_t m_u2DMissingTextureId;
                uint32_t m_u3DMissingTextureId;

                VkDescriptorPool m_hShaderDescriptorPool = VK_NULL_HANDLE;
                std::vector<VkDescriptorSet> m_shaderDescriptorSets;

                // first: descriptor pool handle
                // second: descriptor pool size
                std::vector<VkDescriptorPool> m_meshDescriptorPools;
                std::vector<VkDescriptorSet> m_meshDescriptorSets;
                uint32_t m_uCurrentFrame = 0;
                uint32_t m_uMeshCounter = 0;
                uint32_t m_uMeshMasterPoolCapacity = 100; // the real pool size is m_uMeshMasterPoolCapacity * m_uBufferingStageCount
                uint32_t m_uBufferingStageCount = MAX_FRAMES_IN_FLIGHT;

                VkDescriptorSetLayout m_hShaderDescriptorSetLayout = VK_NULL_HANDLE;
                VkDescriptorSetLayout m_hMeshDescriptorSetLayout = VK_NULL_HANDLE;

            private:
                void _CreateDescriptorSetLayouts(const std::vector<UniformDataLayout>& _uniformDataLayouts);
                void _CreateShaderDescriptorPool(const std::vector<UniformDataLayout>& _uniformDataLayouts);
                void _AllocateNewMeshDescriptorPool(const std::vector<UniformDataLayout>& _uniformDataLayouts);
                void _AllocateShaderDescriptorSets(const std::vector<UniformDataLayout>& _uniformDataLayouts);

            public:
                DescriptorAllocator(
                    VkDevice _hDevice,
                    VkBuffer _hMainBuffer,
                    const std::unordered_map<uint32_t, TextureResource>& _textureRegistry,
                    const std::vector<UniformDataLayout> &_uniformDataLayouts, 
                    uint32_t _uFrameCount,
                    uint32_t _uMissing2DTextureId,
                    uint32_t _uMissing3DTextureId);
                DescriptorAllocator(DescriptorAllocator&& _da) noexcept;
                DescriptorAllocator(const DescriptorAllocator& _da) = delete;
                ~DescriptorAllocator() noexcept;

                // whenever new uniform buffer is allocated
                VkDescriptorSet RequestMeshDescriptorSet(const std::vector<UniformDataLayout>& _uniformDataLayouts);
                void UpdateDescriptorSet(
                    VkBuffer _hMainBuffer, 
                    VkDescriptorSet _hDescriptorSet, 
                    UniformUsage _eUsage, 
                    const std::vector<UniformDataLayout>& _uniformDataLayouts,
                    size_t _uFrameIndex = 0,
                    const std::vector<uint32_t>& _textures = {});
                void MergeMeshDescriptorPools(const std::vector<UniformDataLayout>& _uniformDataLayouts);

                inline void ResetMeshCounter() {
                    m_uCurrentFrame = (m_uCurrentFrame + 1) % m_uBufferingStageCount;
                    if (m_uCurrentFrame == 0)
                        m_uMeshCounter = 0;
                }

                inline VkDescriptorSet RequestShaderDescriptorSet(uint32_t _uCurrentFrame) const {
                    return m_shaderDescriptorSets[_uCurrentFrame];
                }

                inline VkDescriptorSetLayout GetShaderDescriptorSetLayout() const {
                    return m_hShaderDescriptorSetLayout;
                }

                inline VkDescriptorSetLayout GetMeshDescriptorSetLayout() const {
                    return m_hMeshDescriptorSetLayout;
                }
        };
    }
}

#endif
