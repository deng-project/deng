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

    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
#endif


namespace DENG {
    namespace Vulkan {
        
        class PipelineCreator {
            private:
                VkDevice m_device = VK_NULL_HANDLE;
                VkDescriptorSetLayout m_desc_set_layout = VK_NULL_HANDLE;
                VkViewport m_viewport = {};
                VkRect2D m_scissor = {};
                VkExtent2D m_ext = {};
                VkSampleCountFlagBits m_samples = {};

                std::vector<VkPipelineShaderStageCreateInfo> m_shader_stage_createinfos;
                std::vector<VkShaderModule> m_shader_modules;
                std::vector<VkVertexInputBindingDescription> m_input_binding_desc;
                std::vector<VkVertexInputAttributeDescription> m_input_attr_descs;

                VkPipelineVertexInputStateCreateInfo    m_vert_input_create_info = {};
                VkPipelineInputAssemblyStateCreateInfo  m_input_asm_createinfo = {};
                VkPipelineViewportStateCreateInfo       m_viewport_state_createinfo = {};
                VkPipelineRasterizationStateCreateInfo  m_rasterization_createinfo = {};
                VkPipelineMultisampleStateCreateInfo    m_multisample_createinfo = {};
                VkPipelineColorBlendAttachmentState     m_colorblend_attachment = {};
                VkPipelineDepthStencilStateCreateInfo   m_depth_stencil = {};
                VkPipelineColorBlendStateCreateInfo     m_colorblend_state_createinfo = {};

                VkPipelineDynamicStateCreateInfo        m_dynamic_state_createinfo = {};
                std::vector<VkDynamicState> m_dynamic_states;
                ShaderModule &m_module;

                std::vector<uint32_t> m_vertex_bin;
                std::vector<uint32_t> m_geometry_bin;
                std::vector<uint32_t> m_fragment_bin;

                VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
                VkPipeline m_pipeline = VK_NULL_HANDLE;
                VkRenderPass m_render_pass = VK_NULL_HANDLE;

            private:
                void _FindInputBindingDescriptions(const ShaderModule &_module);
                void _FindVertexInputAttributeDescriptions(const ShaderModule &_module);
                VkShaderModule _CreateShaderModule(std::vector<uint32_t> &_bin);

                // avoid shaderc includes in redistributable library 
#ifdef VULKAN_PIPELINE_CREATOR_CPP
                static void _CompileShader(std::vector<uint32_t> &_target, const std::string &_src, const std::string &_file_name, shaderc_shader_kind _kind);
#endif
                void _CheckAndCompileShaderSources(ShaderModule &_module);
                std::string _ReadShaderSource(const std::string &_file_name);
                void _CreatePipelineLayout();
                VkGraphicsPipelineCreateInfo _GeneratePipelineCreateInfo(bool _create_shader_modules = true);

            public:
                PipelineCreator(VkDevice _dev, VkRenderPass _render_pass, VkExtent2D _ext, VkSampleCountFlagBits _samples, VkDescriptorSetLayout _desc_set_layout, ShaderModule &_module);
                PipelineCreator(const PipelineCreator &_pc);
                PipelineCreator(PipelineCreator &&_pc);
                ~PipelineCreator();

                PipelineCreator &operator=(const PipelineCreator &_pc);
                PipelineCreator &operator=(PipelineCreator &&_pc);

                void DestroyPipelineData();
                void RecreatePipeline(VkRenderPass _render_pass, VkExtent2D _ext);

                inline VkPipelineLayout GetPipelineLayout() { return m_pipeline_layout; }
                inline VkPipeline GetPipeline() { return m_pipeline; }
        };
    }
}

#endif
