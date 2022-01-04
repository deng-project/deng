/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: pipeline_ci_gen.cpp - Vulkan pipeline createinfo generation class implementation
/// author: Karl-Mihkel Ott


#define __VK_PIPELINE_CI_GEN_CPP
#include <deng/vulkan/pipeline_ci_gen.h>

namespace deng {
    namespace vulkan {

        /***************************************************/
        /******** __vk_PipelineCreateInfoGenerator *********/
        /***************************************************/

        __vk_PipelineCreateInfoGenerator::__vk_PipelineCreateInfoGenerator (
            VkDevice device, 
            VkExtent2D ext, 
            VkRenderPass renderpass,
            __vk_PipelineData &pd
        ) : m_pipeline_data(pd) {
            m_device = device;
            m_ext = ext;
            m_renderpass = renderpass;
        }


        /// Read SPIR-V binary data from shader files
        char *__vk_PipelineCreateInfoGenerator::__readBinShader(const char *file_name, size_t &size) {
            FILE *file = fopen(file_name, "rb");

            // Check if the file opening was not successful
            if(!file) FILE_ERR(file_name);

            // Find the file size
            fseek(file, 0, SEEK_END);
            size = ftell(file);
            fseek(file, 0, SEEK_SET);

            printf("Bin shader file size for file %s: %lu\n", file_name, size);

            // Allocate memory for binary data
            char *bin = (char*) calloc(size + 1, sizeof(char));

            // Read the binary file contents
            size_t res = fread(bin, sizeof(char), size, file);
            if(!res) {
                RUN_ERR("__vk_PipelineCreateInfoGenerator::__readBinShader()",
                    "Failed to read from binary shader file: " + std::string(file_name));
            }

            fclose(file);
            return bin;
        }


        /// Read appropriate vertex and fragment shaders according to 
        /// the pipeline mode
        void __vk_PipelineCreateInfoGenerator::__readShaders(deng_PipelineType pt, 
            char **p_vert, size_t &vert_c, char **p_frag, size_t &frag_c) {

            // Check which shaders to read according to the pipeline type
            switch(pt) {
            case DENG_PIPELINE_TYPE_UNMAPPED_2D:
                LOG("Pipeline type unmapped 2d, using files: " + std::string(SPIR_V_UNMAPPED_VERT_SHADER_2D) + " " + 
                    std::string(SPIR_V_UNMAPPED_FRAG_SHADER_2D));
                *p_vert = __readBinShader(SPIR_V_UNMAPPED_VERT_SHADER_2D, vert_c);
                *p_frag = __readBinShader(SPIR_V_UNMAPPED_FRAG_SHADER_2D, frag_c);
                break;

            case DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D:
                LOG("Pipeline type texture mapped 2d, using files: " + std::string(SPIR_V_TEXTURE_MAPPED_VERT_SHADER_2D) + " " + 
                    std::string(SPIR_V_TEXTURE_MAPPED_FRAG_SHADER_2D));
                *p_vert = __readBinShader(SPIR_V_TEXTURE_MAPPED_VERT_SHADER_2D, vert_c);
                *p_frag = __readBinShader(SPIR_V_TEXTURE_MAPPED_FRAG_SHADER_2D, frag_c);
                break;

            case DENG_PIPELINE_TYPE_UNMAPPED_3D:
                LOG("Pipeline type unmapped 3d, using files: " + std::string(SPIR_V_UNMAPPED_VERT_SHADER_3D) + " " + 
                    std::string(SPIR_V_UNMAPPED_FRAG_SHADER_3D));
                *p_vert = __readBinShader(SPIR_V_UNMAPPED_VERT_SHADER_3D, vert_c);
                *p_frag = __readBinShader(SPIR_V_UNMAPPED_FRAG_SHADER_3D, frag_c);
                break;

            case DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D:
                LOG("Pipeline type texture mapped 3d, using files: " + std::string(SPIR_V_TEXTURE_MAPPED_VERT_SHADER_3D) + " " + 
                    std::string(SPIR_V_TEXTURE_MAPPED_FRAG_SHADER_3D));
                *p_vert = __readBinShader(SPIR_V_TEXTURE_MAPPED_VERT_SHADER_3D, vert_c);
                *p_frag = __readBinShader(SPIR_V_TEXTURE_MAPPED_FRAG_SHADER_3D, frag_c);
                break;

            case DENG_PIPELINE_TYPE_UI:
                LOG("Pipeline type ui, using files: " + std::string(SPIR_V_UI_VERT_SHADER) + " " + 
                    std::string(SPIR_V_UI_FRAG_SHADER));
                *p_vert = __readBinShader(SPIR_V_UI_VERT_SHADER, vert_c);
                *p_frag = __readBinShader(SPIR_V_UI_FRAG_SHADER, frag_c);
                break;

            default:
                break;
            }
        }


        /// Create new shader module from SPIR-V binaries
        VkShaderModule __vk_PipelineCreateInfoGenerator::__mkShaderModule(char *bin, size_t bin_c) {
            VkShaderModuleCreateInfo createinfo{};
            createinfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createinfo.codeSize = static_cast<deng_ui32_t>(bin_c);
            createinfo.pCode = reinterpret_cast<const deng_ui32_t*>(bin);
            VkShaderModule shader_module;

            if(vkCreateShaderModule(m_device, &createinfo, NULL, &shader_module) != VK_SUCCESS)
                VK_PIPELINEC_ERR("failed to create shader module!");

            return shader_module;
        }

        
        /// Get binding description info in VkVertexInputBindingDescription instance
        std::vector<VkVertexInputBindingDescription> __vk_PipelineCreateInfoGenerator::__getBindingDesc() {
            std::vector<VkVertexInputBindingDescription> input_binding_desc;

            switch (m_pipeline_data.pipeline_type) { 
            case DENG_PIPELINE_TYPE_UNMAPPED_2D:
                input_binding_desc.resize(1);
                // Set the position vertex input bindings
                input_binding_desc[0].binding = 0;
                input_binding_desc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                input_binding_desc[0].stride = sizeof(das_ObjPosData2D);
                break; 

            case DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D:
                input_binding_desc.resize(2);
                input_binding_desc[0].binding = 0;
                input_binding_desc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                input_binding_desc[0].stride = sizeof(das_ObjPosData2D);

                input_binding_desc[1].binding = 1;
                input_binding_desc[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                input_binding_desc[1].stride = sizeof(das_ObjTextureData);
                break;

            case DENG_PIPELINE_TYPE_UNMAPPED_3D:
                input_binding_desc.resize(2);
                input_binding_desc[0].binding = 0;
                input_binding_desc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                input_binding_desc[0].stride = sizeof(das_ObjPosData);

                input_binding_desc[1].binding = 1;
                input_binding_desc[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                input_binding_desc[1].stride = sizeof(das_ObjNormalData);
                break;

            case DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D:
                input_binding_desc.resize(3);
                input_binding_desc[0].binding = 0;
                input_binding_desc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                input_binding_desc[0].stride = sizeof(das_ObjPosData);

                input_binding_desc[1].binding = 1;
                input_binding_desc[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                input_binding_desc[1].stride = sizeof(das_ObjTextureData);

                input_binding_desc[2].binding = 2;
                input_binding_desc[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                input_binding_desc[2].stride = sizeof(das_ObjNormalData);
                break;

            case DENG_PIPELINE_TYPE_UI:
                input_binding_desc.resize(1);
                input_binding_desc[0].binding = 0;
                input_binding_desc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                input_binding_desc[0].stride = sizeof(ImDrawVert);
                break;
                
            default:
                break;
            }

            return input_binding_desc;
        } 


        /// Get attribute description info in VkVertexInputAttributeDescription instances
        std::vector<VkVertexInputAttributeDescription> __vk_PipelineCreateInfoGenerator::__getAttributeDescs() {
            std::vector<VkVertexInputAttributeDescription> input_attr_desc{};

            switch (m_pipeline_data.pipeline_type) {
            case DENG_PIPELINE_TYPE_UNMAPPED_2D:
                input_attr_desc.resize(1);
                input_attr_desc[0].binding = 0;
                input_attr_desc[0].location = 0;
                input_attr_desc[0].format = VK_FORMAT_R32G32_SFLOAT;
                input_attr_desc[0].offset = 0;
                break;

            case DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D:
                input_attr_desc.resize(2);
                input_attr_desc[0].binding = 0;
                input_attr_desc[0].location = 0;
                input_attr_desc[0].format = VK_FORMAT_R32G32_SFLOAT;
                input_attr_desc[0].offset = 0;

                input_attr_desc[1].binding = 1;
                input_attr_desc[1].location = 1;
                input_attr_desc[1].format = VK_FORMAT_R32G32_SFLOAT;
                input_attr_desc[1].offset = 0;
                break;

            case DENG_PIPELINE_TYPE_UNMAPPED_3D:
                input_attr_desc.resize(2);
                input_attr_desc[0].binding = 0;
                input_attr_desc[0].location = 0;
                input_attr_desc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
                input_attr_desc[0].offset = 0;

                input_attr_desc[1].binding = 1;
                input_attr_desc[1].location = 1;
                input_attr_desc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                input_attr_desc[1].offset = 0;
                break;

            case DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D:
                input_attr_desc.resize(3);
                input_attr_desc[0].binding = 0;
                input_attr_desc[0].location = 0;
                input_attr_desc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
                input_attr_desc[0].offset = 0;

                input_attr_desc[1].binding = 1;
                input_attr_desc[1].location = 1;
                input_attr_desc[1].format = VK_FORMAT_R32G32_SFLOAT;
                input_attr_desc[1].offset = 0;

                input_attr_desc[2].binding = 2;
                input_attr_desc[2].location = 2;
                input_attr_desc[2].format = VK_FORMAT_R32G32B32_SFLOAT;
                input_attr_desc[2].offset = 0;
                break;

            case DENG_PIPELINE_TYPE_UI:
                input_attr_desc.resize(3);
                input_attr_desc[0].binding = 0;
                input_attr_desc[0].location = 0;
                input_attr_desc[0].format = VK_FORMAT_R32G32_SFLOAT;
                input_attr_desc[0].offset = offsetof(ImDrawVert, pos);

                input_attr_desc[1].binding = 0;
                input_attr_desc[1].location = 1;
                input_attr_desc[1].format = VK_FORMAT_R32G32_SFLOAT;
                input_attr_desc[1].offset = offsetof(ImDrawVert, uv);

                input_attr_desc[2].binding = 0;
                input_attr_desc[2].location = 2;
                input_attr_desc[2].format = VK_FORMAT_R8G8B8A8_UNORM;
                input_attr_desc[2].offset = offsetof(ImDrawVert, col);
                break;

            default:
                break;
            }

            return input_attr_desc;
        }


        /// Make createinfo instance for graphics pipeline
        /// This method is used to set up VkGraphicsPipelineCreateInfo, while defaulting
        /// some options that are not needed to be customised by different DENG pipelines
        VkGraphicsPipelineCreateInfo __vk_PipelineCreateInfoGenerator::mkGraphicsPipelineInfo(const __vk_PipelineCreationFlags &pl_flags) {
            // Binary shader data pointers
            char *vert_bin = NULL;
            size_t vert_c = 0;
            char *frag_bin = NULL;
            size_t frag_c = 0;
            
            // Read binary data from compiled shaders
            __readShaders(m_pipeline_data.pipeline_type, &vert_bin, vert_c, &frag_bin, frag_c);

            // Create vertex and fragment shader modules
            m_shader_modules[0] = __mkShaderModule(vert_bin, vert_c);
            m_shader_modules[1] = __mkShaderModule(frag_bin, frag_c);

            // Create vertex shader stage createinfo
            VkPipelineShaderStageCreateInfo vertex_shader_stage_createinfo{};
            vertex_shader_stage_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertex_shader_stage_createinfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertex_shader_stage_createinfo.module = m_shader_modules[0];
            vertex_shader_stage_createinfo.pName = "main";

            // Create fragment shader stage createinfo
            VkPipelineShaderStageCreateInfo frag_shader_stage_createinfo{};
            frag_shader_stage_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            frag_shader_stage_createinfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            frag_shader_stage_createinfo.module = m_shader_modules[1];
            frag_shader_stage_createinfo.pName = "main";
            
            m_shader_stage_createinfos = { vertex_shader_stage_createinfo, frag_shader_stage_createinfo };

            // Clean the shader data from memory
            free(frag_bin);
            free(vert_bin);

            // Bind get binding descriptors and attribute descriptors for the current pipeline type
            m_input_binding_desc = __getBindingDesc();
            m_input_attr_descs = __getAttributeDescs();

            // Set up vertex input createinfo object 
            m_vert_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            m_vert_input_create_info.vertexBindingDescriptionCount = (deng_ui32_t) m_input_binding_desc.size();
            m_vert_input_create_info.vertexAttributeDescriptionCount = (deng_ui32_t) m_input_attr_descs.size();
            m_vert_input_create_info.pVertexBindingDescriptions = m_input_binding_desc.data();
            m_vert_input_create_info.pVertexAttributeDescriptions = m_input_attr_descs.data();

            // Set up input assembly createinfo object
            m_input_asm_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            m_input_asm_createinfo.topology = pl_flags.primitive_topology;
            m_input_asm_createinfo.primitiveRestartEnable = VK_FALSE;

            // Set viewport values
            m_viewport.x = 0.0f;
            m_viewport.y = 0.0f;
            m_viewport.width = static_cast<deng_vec_t>(m_ext.width);
            m_viewport.height = static_cast<deng_vec_t>(m_ext.height);
            m_viewport.minDepth = 0.0f;
            m_viewport.maxDepth = 1.0f;

            // Set scissor offset and extent values
            m_scissor.offset = {0, 0};
            m_scissor.extent = m_ext;

            // Set up viewport state createinfo object
            m_viewport_state_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            m_viewport_state_createinfo.viewportCount = 1;
            m_viewport_state_createinfo.pViewports = &m_viewport;
            m_viewport_state_createinfo.scissorCount = 1;
            m_viewport_state_createinfo.pScissors = &m_scissor;

            // Set up rasterization create info
            m_rasterization_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            m_rasterization_createinfo.depthClampEnable = VK_FALSE;
            m_rasterization_createinfo.rasterizerDiscardEnable = VK_FALSE;
            m_rasterization_createinfo.polygonMode = pl_flags.polygon_mode;
            m_rasterization_createinfo.lineWidth = 1.0f;
            m_rasterization_createinfo.cullMode = pl_flags.cull_mode;

            if(pl_flags.cull_mode != VK_CULL_MODE_NONE) {
                m_rasterization_createinfo.frontFace = pl_flags.front_face;
                m_rasterization_createinfo.depthBiasEnable = VK_TRUE;
            }


            // Set up multisampling createinfo
            m_multisample_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            m_multisample_createinfo.sampleShadingEnable = VK_FALSE;
            m_multisample_createinfo.rasterizationSamples = pl_flags.sample_c;

            // Set colorblend options
            m_colorblend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            if(pl_flags.add_color_blend) m_colorblend_attachment.blendEnable = VK_TRUE;
            else m_colorblend_attachment.blendEnable = VK_FALSE;
            m_colorblend_attachment.alphaBlendOp = VK_BLEND_OP_SUBTRACT;
            m_colorblend_attachment.blendEnable = VK_TRUE;
            m_colorblend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            m_colorblend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            m_colorblend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
            m_colorblend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            m_colorblend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            m_colorblend_attachment.alphaBlendOp = VK_BLEND_OP_SUBTRACT;
            
            // Check if depth stencil is enabled and if it is set the createinfo accordingly
            m_depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            if(pl_flags.add_depth_stencil) {
                m_depth_stencil.depthTestEnable = VK_TRUE;
                m_depth_stencil.depthWriteEnable = VK_TRUE;
                m_depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
                m_depth_stencil.depthBoundsTestEnable = VK_FALSE;
                m_depth_stencil.stencilTestEnable = VK_FALSE;
            }
            else {
                m_depth_stencil.depthTestEnable = VK_FALSE;
                m_depth_stencil.depthWriteEnable = VK_FALSE;
                m_depth_stencil.depthBoundsTestEnable = VK_FALSE;
                m_depth_stencil.stencilTestEnable = VK_FALSE;
            }

            // Check if the dynamic pipeline state createinfo should be created
            VkPipelineDynamicStateCreateInfo *p_dyn_info = NULL;
            if(pl_flags.add_scissoring) {
                m_dyn_state_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
                m_dynamic_states.push_back(VK_DYNAMIC_STATE_SCISSOR);
                m_dyn_state_createinfo.dynamicStateCount = static_cast<deng_ui32_t>(m_dynamic_states.size());
                m_dyn_state_createinfo.pDynamicStates = m_dynamic_states.data();
                p_dyn_info = &m_dyn_state_createinfo;
            }
            
            // Set up colorblend state createinfo
            m_colorblend_state_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            m_colorblend_state_createinfo.logicOpEnable = VK_FALSE;
            m_colorblend_state_createinfo.attachmentCount = 1;
            m_colorblend_state_createinfo.pAttachments = &m_colorblend_attachment;

            // Set up graphics pipeline createinfo
            VkGraphicsPipelineCreateInfo graphics_pipeline_createinfo{};
            graphics_pipeline_createinfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            graphics_pipeline_createinfo.stageCount = (deng_ui32_t) m_shader_stage_createinfos.size();
            graphics_pipeline_createinfo.pStages = m_shader_stage_createinfos.data();
            graphics_pipeline_createinfo.pVertexInputState = &m_vert_input_create_info;
            graphics_pipeline_createinfo.pInputAssemblyState = &m_input_asm_createinfo;
            graphics_pipeline_createinfo.pViewportState = &m_viewport_state_createinfo;
            graphics_pipeline_createinfo.pColorBlendState = &m_colorblend_state_createinfo;
            graphics_pipeline_createinfo.pRasterizationState = &m_rasterization_createinfo;
            graphics_pipeline_createinfo.pMultisampleState = &m_multisample_createinfo;
            graphics_pipeline_createinfo.pDepthStencilState = &m_depth_stencil;
            graphics_pipeline_createinfo.pDynamicState = p_dyn_info;
            graphics_pipeline_createinfo.layout = *m_pipeline_data.p_pipeline_layout;

            graphics_pipeline_createinfo.renderPass = m_renderpass;
            graphics_pipeline_createinfo.subpass = pl_flags.subpass_index;
            graphics_pipeline_createinfo.basePipelineHandle = VK_NULL_HANDLE;

            return graphics_pipeline_createinfo;
        }


        __vk_PipelineCreateInfoGenerator::~__vk_PipelineCreateInfoGenerator() {
            vkDestroyShaderModule(m_device, 
                m_shader_modules[0], NULL);
            
            vkDestroyShaderModule(m_device, 
                m_shader_modules[1], NULL);
        }
    }
}
