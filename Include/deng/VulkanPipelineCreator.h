// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanPipelineCreator.h - Vulkan pipeline creation class header
// author: Karl-Mihkel Ott


#ifndef VULKAN_PIPELINE_CREATOR_H
#define VULKAN_PIPELINE_CREATOR_H

#ifdef VULKAN_PIPELINE_CREATOR_CPP
    #include <string>
    #include <fstream>
    #include <vector>
    #include <array>
    #include <vulkan/vulkan.h>
    #include <shaderc/shaderc.hpp>

    #include "deng/ErrorDefinitions.h"
    #include "deng/Exceptions.h"
    #include "deng/ShaderComponent.h"
    #include "deng/VulkanHelpers.h"
#endif


namespace DENG {
    namespace Vulkan {
        
        class PipelineCreator {
            private:
                VkDevice m_hDevice = VK_NULL_HANDLE;
                VkViewport m_viewport = {};
                VkRect2D m_scissor = {};
                VkExtent2D m_extent = {};
                VkSampleCountFlagBits m_uSampleBits = {};

                std::vector<VkPipelineShaderStageCreateInfo> m_shaderStageCreateInfos;
                std::vector<VkShaderModule> m_shaderModules;
                std::vector<VkVertexInputBindingDescription> m_vertexInputBindingDescriptions;
                std::vector<VkVertexInputAttributeDescription> m_vertexInputAttributeDescriptions;

                VkPipelineVertexInputStateCreateInfo    m_vertexInputStateCreateInfo = {};
                VkPipelineInputAssemblyStateCreateInfo  m_inputAssemblyStateCreateInfo = {};
                VkPipelineViewportStateCreateInfo       m_viewportStateCreateInfo = {};
                VkPipelineRasterizationStateCreateInfo  m_rasterizationStateCreateInfo = {};
                VkPipelineMultisampleStateCreateInfo    m_multisampleStateCreateInfo = {};
                VkPipelineColorBlendAttachmentState     m_colorBlendAttachmentState = {};
                VkPipelineDepthStencilStateCreateInfo   m_depthStencilStateCreateInfo = {};
                VkPipelineColorBlendStateCreateInfo     m_colorBlendStateCreateInfo = {};

                VkPipelineDynamicStateCreateInfo        m_dynamicStateCreateInfo = {};
                std::vector<VkDynamicState> m_dynamicStates;
                VkGraphicsPipelineCreateInfo m_graphicsPipelineCreateInfo = {};

                VkDescriptorSetLayout m_hShaderDescriptorSetLayout = VK_NULL_HANDLE;
                VkDescriptorSetLayout m_hMeshDescriptorSetLayout = VK_NULL_HANDLE;
                VkPipelineLayout m_hPipelineLayout = VK_NULL_HANDLE;
                VkPipeline m_hPipeline = VK_NULL_HANDLE;
                VkRenderPass m_hRenderPass = VK_NULL_HANDLE;

                VkPipelineCache m_hPipelineCache = VK_NULL_HANDLE;

            private:
                void _FindInputBindingDescriptions(const ShaderComponent& _shader);
                void _FindVertexInputAttributeDescriptions(const ShaderComponent& _shader);
                VkShaderModule _CreateShaderModule(std::vector<uint32_t> &_bin);

                void _CreatePipelineLayout(const ShaderComponent& _shader);
                void _GeneratePipelineCreateInfo(const ShaderComponent& _shader, bool _bCreateShaderModules = true);

            public:
                PipelineCreator(
                    VkDevice _hDevice, 
                    VkRenderPass _hRenderPass,
                    VkDescriptorSetLayout _hShaderDescriptorSetLayout,
                    VkDescriptorSetLayout _hMeshDescriptorSetLayout,
                    VkExtent2D _extent, 
                    VkSampleCountFlagBits _uSampleBits,
                    const PhysicalDeviceInformation& _info,
                    const ShaderComponent& _module);
                PipelineCreator(PipelineCreator &&_pc) noexcept;
                PipelineCreator(const PipelineCreator &_pc) = delete;
                ~PipelineCreator() noexcept;

                void DestroyPipelineData();
                void RecreatePipeline(const ShaderComponent& _shader, VkRenderPass _hRenderPass, VkExtent2D _extent, bool _bRecompile = false);

                inline VkPipelineCache GetPipelineCache() { return m_hPipelineCache; }
                inline VkPipelineLayout GetPipelineLayout() { return m_hPipelineLayout; }
                inline VkPipeline GetPipeline() { return m_hPipeline; }
        };
    }
}

#endif
