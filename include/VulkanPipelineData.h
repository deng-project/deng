// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanPipelineData.h - Header containing data structures needed for Vulkan pipelines
// author: Karl-Mihkel Ott


#ifndef VULKAN_PIPELINE_DATA_H
#define VULKAN_PIPELINE_DATA_H

namespace DENG {
    namespace Vulkan {

        /// Structure for all pipeline related data required by Vulkan renderer
        struct PipelineData {
            VkPipeline pipeline;
            deng_PipelineType pipeline_type;
            VkPipelineLayout *p_pipeline_layout;
        };


        /// Parent structure for pipeline createinfo generation class
        struct PipelineCreateInfoSpecifiers {
            std::array<VkPipelineShaderStageCreateInfo, 2> m_shader_stage_createinfos = {};
            std::array<VkShaderModule, 2> m_shader_modules = {};
            std::vector<VkVertexInputBindingDescription> m_input_binding_desc = {};
            std::vector<VkVertexInputAttributeDescription> m_input_attr_descs = {};

            VkViewport m_viewport = {};
            VkRect2D m_scissor = {};

            VkPipelineVertexInputStateCreateInfo    m_vert_input_create_info = {};
            VkPipelineInputAssemblyStateCreateInfo  m_input_asm_createinfo = {};
            VkPipelineViewportStateCreateInfo       m_viewport_state_createinfo = {};
            VkPipelineRasterizationStateCreateInfo  m_rasterization_createinfo = {};
            VkPipelineMultisampleStateCreateInfo    m_multisample_createinfo = {};
            VkPipelineColorBlendAttachmentState     m_colorblend_attachment = {};
            VkPipelineDepthStencilStateCreateInfo   m_depth_stencil = {};
            VkPipelineColorBlendStateCreateInfo     m_colorblend_state_createinfo = {};

            VkPipelineDynamicStateCreateInfo        m_dyn_state_createinfo = {};
            std::vector<VkDynamicState> m_dynamic_states;
        };
    }
}

#endif
