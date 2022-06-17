// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanPipelineCreator.cpp - Vulkan pipeline creation class implementation
// author: Karl-Mihkel Ott

#define VULKAN_PIPELINE_CREATOR_CPP
#include <VulkanPipelineCreator.h>


namespace DENG {
    namespace Vulkan {

        PipelineCreator::PipelineCreator(VkDevice _dev, VkRenderPass _render_pass, VkExtent2D _ext, VkSampleCountFlagBits _samples, std::array<VkDescriptorSetLayout, 2> _desc_set_layouts, const ShaderModule &_module) :
            m_device(_dev), 
            m_desc_set_layouts(_desc_set_layouts), 
            m_ext(_ext), 
            m_samples(_samples), 
            m_module(_module), 
            m_render_pass(_render_pass)
        {
            _CreatePipelineLayout();

            // for each shader module create a VkPipeline instance
            VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = _GeneratePipelineCreateInfo();
            
            // create a pipeline
            if(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, NULL, &m_pipeline) != VK_SUCCESS)
                VK_PIPELINEC_ERR("failed to create a pipeline");
        }


        PipelineCreator::~PipelineCreator() noexcept {
            for(size_t i = 0; i < m_shader_modules.size(); i++)
                vkDestroyShaderModule(m_device, m_shader_modules[i], NULL);

            vkDestroyPipeline(m_device, m_pipeline, nullptr);
            vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
        } 


        void PipelineCreator::DestroyPipelineData() {
            vkDestroyPipeline(m_device, m_pipeline, nullptr);
            vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
        }


        void PipelineCreator::RecreatePipeline(VkRenderPass _render_pass, VkExtent2D _ext) {
            m_render_pass = _render_pass;
            m_ext = _ext;

            _CreatePipelineLayout();
            VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = _GeneratePipelineCreateInfo(false);

            if(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &m_pipeline) != VK_SUCCESS)
                VK_PIPELINEC_ERR("failed to create a pipeline");
        }


        std::string PipelineCreator::_ReadShaderSource(const std::string &_file_name) {
            std::ifstream file(_file_name, std::ios_base::binary);
            if(!file) FILE_ERR(_file_name);

            file.seekg(0, std::ios_base::end);
            size_t len = file.tellg();
            file.seekg(0, std::ios_base::beg);

            std::vector<char> data(len);
            file.read(data.data(), data.size());
            file.close();

            return std::string(data.data(), data.size());
        }


        void PipelineCreator::_FindInputBindingDescriptions(const ShaderModule &_module) {
            DENG_ASSERT(_module.attributes.size() == _module.attribute_strides.size());
            m_input_binding_desc.reserve(_module.attributes.size());

            for(auto it = _module.attributes.begin(); it != _module.attributes.end(); it++) {
                uint32_t i = static_cast<uint32_t>(it - _module.attributes.begin());
                m_input_binding_desc.emplace_back();
                m_input_binding_desc.back().binding = i;
                m_input_binding_desc.back().stride = static_cast<uint32_t>(_module.attribute_strides[i]);
                m_input_binding_desc.back().inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            }
        }


        void PipelineCreator::_FindVertexInputAttributeDescriptions(const ShaderModule &_module) {
            for(uint32_t i = 0; i < static_cast<uint32_t>(_module.attributes.size()); i++) {
                m_input_attr_descs.push_back(VkVertexInputAttributeDescription{});
                m_input_attr_descs.back().binding = i;
                m_input_attr_descs.back().location = i;

                switch(_module.attributes[i]) {
                    // single element attributes
                    case ATTRIBUTE_TYPE_FLOAT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_DOUBLE:
                        m_input_attr_descs.back().format = VK_FORMAT_R64_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_BYTE:
                        m_input_attr_descs.back().format = VK_FORMAT_R8_SINT;
                        break;

                    case ATTRIBUTE_TYPE_UBYTE:
                        m_input_attr_descs.back().format = VK_FORMAT_R8_UINT;
                        break;

                    case ATTRIBUTE_TYPE_SHORT:
                        m_input_attr_descs.back().format = VK_FORMAT_R16_SINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_USHORT:
                        m_input_attr_descs.back().format = VK_FORMAT_R16_UINT;
                        break;

                    case ATTRIBUTE_TYPE_UINT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32_UINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_INT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32_SINT;
                        break;

                    // two element attributes
                    case ATTRIBUTE_TYPE_VEC2_FLOAT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC2_DOUBLE:
                        m_input_attr_descs.back().format = VK_FORMAT_R64G64_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC2_BYTE:
                        m_input_attr_descs.back().format = VK_FORMAT_R8G8_SNORM;
                        break;

                    case ATTRIBUTE_TYPE_VEC2_UBYTE:
                        m_input_attr_descs.back().format = VK_FORMAT_R8G8_UNORM;
                        break;

                    case ATTRIBUTE_TYPE_VEC2_SHORT:
                        m_input_attr_descs.back().format = VK_FORMAT_R16G16_SINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_VEC2_USHORT:
                        m_input_attr_descs.back().format = VK_FORMAT_R16G16_UINT;
                        break;

                    case ATTRIBUTE_TYPE_VEC2_UINT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32_UINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_VEC2_INT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32_SINT;
                        break;

                    // three element attributes
                    case ATTRIBUTE_TYPE_VEC3_FLOAT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32B32_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC3_DOUBLE:
                        m_input_attr_descs.back().format = VK_FORMAT_R64G64B64_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC3_BYTE:
                        m_input_attr_descs.back().format = VK_FORMAT_R8G8B8_SNORM;
                        break;

                    case ATTRIBUTE_TYPE_VEC3_UBYTE:
                        m_input_attr_descs.back().format = VK_FORMAT_R8G8B8_UNORM;
                        break;

                    case ATTRIBUTE_TYPE_VEC3_SHORT:
                        m_input_attr_descs.back().format = VK_FORMAT_R16G16B16_SINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_VEC3_USHORT:
                        m_input_attr_descs.back().format = VK_FORMAT_R16G16B16_UINT;
                        break;

                    case ATTRIBUTE_TYPE_VEC3_UINT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32B32_UINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_VEC3_INT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32B32_SINT;
                        break;

                    // four element attributes
                    case ATTRIBUTE_TYPE_VEC4_FLOAT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32B32A32_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC4_DOUBLE:
                        m_input_attr_descs.back().format = VK_FORMAT_R64G64B64A64_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC4_BYTE:
                        m_input_attr_descs.back().format = VK_FORMAT_R8G8B8A8_SNORM;
                        break;

                    case ATTRIBUTE_TYPE_VEC4_UBYTE:
                        m_input_attr_descs.back().format = VK_FORMAT_R8G8B8A8_UNORM;
                        break;

                    case ATTRIBUTE_TYPE_VEC4_SHORT:
                        m_input_attr_descs.back().format = VK_FORMAT_R16G16B16A16_SINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_VEC4_USHORT:
                        m_input_attr_descs.back().format = VK_FORMAT_R16G16B16A16_UINT;
                        break;

                    case ATTRIBUTE_TYPE_VEC4_UINT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32B32A32_UINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_VEC4_INT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32B32A32_SINT;
                        break;
                }

                m_input_attr_descs.back().offset = 0;
            }
        }


        VkShaderModule PipelineCreator::_CreateShaderModule(std::vector<uint32_t> &_bin) {
            VkShaderModuleCreateInfo module_create_info = {};
            module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            module_create_info.codeSize = static_cast<uint32_t>(_bin.size() * sizeof(uint32_t));
            module_create_info.pCode = _bin.data();

            VkShaderModule shader_module;
            if(vkCreateShaderModule(m_device, &module_create_info, NULL, &shader_module) != VK_SUCCESS)
                VK_PIPELINEC_ERR("failed to create a shader module");

            return shader_module;
        }


        void PipelineCreator::_CompileShader(std::vector<uint32_t> &_target, const std::string &_src, const std::string &_file_name, shaderc_shader_kind _kind) {
            shaderc::Compiler compiler;
            shaderc::CompileOptions options;

            options.SetOptimizationLevel(shaderc_optimization_level_size);
            shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(_src, _kind, _file_name.c_str(), options);

            if(module.GetCompilationStatus() != shaderc_compilation_status_success) {
                std::cerr << module.GetErrorMessage();
                std::exit(EXIT_FAILURE);
            }

            _target = std::vector<uint32_t>(module.cbegin(), module.cend());
        }


        void PipelineCreator::_CheckAndCompileShaderSources() {
            std::string vertex_shader_src;
            std::string geometry_shader_src;
            std::string fragment_shader_src;
            if(m_module.load_shaders_from_file) {
                vertex_shader_src = _ReadShaderSource(m_module.vertex_shader_file);
                if(m_module.geometry_shader_file != "")
                    geometry_shader_src = _ReadShaderSource(m_module.geometry_shader_file);
                fragment_shader_src = _ReadShaderSource(m_module.fragment_shader_file);
            } else {
                vertex_shader_src = m_module.vertex_shader_src;
                geometry_shader_src = m_module.geometry_shader_src;
                fragment_shader_src = m_module.fragment_shader_src;
            }

            _CompileShader(m_vertex_bin, vertex_shader_src, m_module.vertex_shader_file, shaderc_glsl_vertex_shader);
            if(m_module.geometry_shader_src.size())
                _CompileShader(m_geometry_bin, geometry_shader_src, m_module.geometry_shader_file, shaderc_glsl_geometry_shader);
            _CompileShader(m_fragment_bin, fragment_shader_src, m_module.fragment_shader_file, shaderc_glsl_fragment_shader);
        }


        void PipelineCreator::_CreatePipelineLayout() {
            VkPipelineLayoutCreateInfo pipeline_layout_createinfo = {};
            pipeline_layout_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_createinfo.pushConstantRangeCount = 0;
            
            // 1. only per shader descriptor set layouts are present
            // 2. only per mesh descriptor set layouts are present
            // 3. all descriptor set layouts are present
            if(m_desc_set_layouts[0] != VK_NULL_HANDLE && m_desc_set_layouts[1] == VK_NULL_HANDLE) {
                pipeline_layout_createinfo.setLayoutCount = 1;
                pipeline_layout_createinfo.pSetLayouts = m_desc_set_layouts.data();
            } else if(m_desc_set_layouts[0] == VK_NULL_HANDLE && m_desc_set_layouts[1] != VK_NULL_HANDLE) {
                pipeline_layout_createinfo.setLayoutCount = 1;
                pipeline_layout_createinfo.pSetLayouts = &m_desc_set_layouts[1];
            } else {
                pipeline_layout_createinfo.setLayoutCount = 2;
                pipeline_layout_createinfo.pSetLayouts = m_desc_set_layouts.data();
            }

            if(vkCreatePipelineLayout(m_device, &pipeline_layout_createinfo, NULL, &m_pipeline_layout) != VK_SUCCESS)
                VK_PIPELINEC_ERR("failed to create a pipeline layout");
        }


        VkGraphicsPipelineCreateInfo PipelineCreator::_GeneratePipelineCreateInfo(bool _create_shader_modules) {
            if(_create_shader_modules) {
                _CheckAndCompileShaderSources();

                // Create vertex and fragment shader modules
                m_shader_modules.reserve(3);
                m_shader_modules.push_back(_CreateShaderModule(m_vertex_bin)); 
                if(m_module.geometry_shader_src.size())
                    m_shader_modules.push_back(_CreateShaderModule(m_geometry_bin));
                m_shader_modules.push_back(_CreateShaderModule(m_fragment_bin));

                // Create vertex shader stage createinfo
                m_shader_stage_createinfos.emplace_back();
                m_shader_stage_createinfos.back().sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                m_shader_stage_createinfos.back().stage = VK_SHADER_STAGE_VERTEX_BIT;
                m_shader_stage_createinfos.back().module = m_shader_modules[0];
                m_shader_stage_createinfos.back().pName = "main";

                // check if geomety shader stage create info needs to be 
                if(m_module.geometry_shader_src.size()) {
                    m_shader_stage_createinfos.emplace_back();
                    m_shader_stage_createinfos.back().sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                    m_shader_stage_createinfos.back().stage = VK_SHADER_STAGE_GEOMETRY_BIT;
                    m_shader_stage_createinfos.back().module = m_shader_modules[1];
                    m_shader_stage_createinfos.back().pName = "main";

                    m_shader_stage_createinfos.emplace_back();
                    m_shader_stage_createinfos.back().module = m_shader_modules[2];
                }
                else {
                    m_shader_stage_createinfos.emplace_back();
                    m_shader_stage_createinfos.back().module = m_shader_modules[1];
                }

                // Create fragment shader stage createinfo
                m_shader_stage_createinfos.back().sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                m_shader_stage_createinfos.back().stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                m_shader_stage_createinfos.back().pName = "main";

                // Bind get binding descriptors and attribute descriptors for the current pipeline type
                _FindInputBindingDescriptions(m_module);
                _FindVertexInputAttributeDescriptions(m_module);
            }

            // Set up vertex input createinfo object 
            m_vert_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            m_vert_input_create_info.vertexBindingDescriptionCount = static_cast<uint32_t>(m_input_binding_desc.size());
            m_vert_input_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_input_attr_descs.size());
            m_vert_input_create_info.pVertexBindingDescriptions = m_input_binding_desc.data();
            m_vert_input_create_info.pVertexAttributeDescriptions = m_input_attr_descs.data();

            // Set up input assembly createinfo object
            m_input_asm_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            switch(m_module.prim_mode) {
                case PRIMITIVE_MODE_TRIANGLES:
                    m_input_asm_createinfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                    break;

                case PRIMITIVE_MODE_LINES:
                    m_input_asm_createinfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
                    break;

                case PRIMITIVE_MODE_POINTS:
                    m_input_asm_createinfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
                    break;
            }
            m_input_asm_createinfo.primitiveRestartEnable = VK_FALSE;

            // Set viewport values
            m_viewport.width = static_cast<float>(m_ext.width);
            m_viewport.height = -static_cast<float>(m_ext.height);
            m_viewport.x = 0.0f;
            m_viewport.y = -m_viewport.height;
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
            m_rasterization_createinfo.polygonMode = VK_POLYGON_MODE_FILL;
            m_rasterization_createinfo.lineWidth = 1.0f;

            // set up culling mode
            switch(m_module.cull_mode) {
                case CULL_MODE_NONE:
                    m_rasterization_createinfo.cullMode = VK_CULL_MODE_NONE;
                    break;

                case CULL_MODE_COUNTER_CLOCKWISE:
                    m_rasterization_createinfo.cullMode = VK_CULL_MODE_BACK_BIT;
                    m_rasterization_createinfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
                    break;

                case CULL_MODE_CLOCKWISE:
                    m_rasterization_createinfo.cullMode = VK_CULL_MODE_BACK_BIT;
                    m_rasterization_createinfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
                    break;

                default:
                    break;
            }

            // Set up multisampling createinfo
            m_multisample_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            m_multisample_createinfo.sampleShadingEnable = VK_FALSE;
            m_multisample_createinfo.rasterizationSamples = m_samples;

            // Set colorblend options if required
            if(m_module.enable_blend) {
                m_colorblend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
                m_colorblend_attachment.blendEnable = VK_TRUE;
                m_colorblend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
                m_colorblend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                m_colorblend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                m_colorblend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
                m_colorblend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                m_colorblend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            } else {
                m_colorblend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
                m_colorblend_attachment.blendEnable = VK_FALSE;
            }
            
            // Check if depth stencil is enabled and if it is set the createinfo accordingly
            if(m_module.enable_depth_testing) {
                m_depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                m_depth_stencil.depthTestEnable = VK_TRUE;
                m_depth_stencil.depthWriteEnable = VK_TRUE;
                m_depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
                m_depth_stencil.depthBoundsTestEnable = VK_FALSE;
                m_depth_stencil.stencilTestEnable = VK_FALSE;
            }
            else {
                m_depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                m_depth_stencil.depthTestEnable = VK_FALSE;
                m_depth_stencil.depthWriteEnable = VK_FALSE;
                m_depth_stencil.depthBoundsTestEnable = VK_FALSE;
                m_depth_stencil.stencilTestEnable = VK_FALSE;
            }

            m_dynamic_state_createinfo = {};
            m_dynamic_states.clear();
            m_dynamic_state_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

            // check if scissor technique is enabled
            if(m_module.enable_scissor) {
                m_dynamic_states.push_back(VK_DYNAMIC_STATE_SCISSOR);
                m_dynamic_state_createinfo.dynamicStateCount++;
                m_dynamic_state_createinfo.pDynamicStates = m_dynamic_states.data();
            } 

            // check if custom viewports are enabled
            if(m_module.enable_custom_viewport) {
                m_dynamic_states.push_back(VK_DYNAMIC_STATE_VIEWPORT);
                m_dynamic_state_createinfo.dynamicStateCount++;
                m_dynamic_state_createinfo.pDynamicStates = m_dynamic_states.data();
            }

            // Set up colorblend state createinfo
            m_colorblend_state_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            m_colorblend_state_createinfo.logicOpEnable = VK_FALSE;
            m_colorblend_state_createinfo.attachmentCount = 1;
            m_colorblend_state_createinfo.pAttachments = &m_colorblend_attachment;

            // Set up graphics pipeline createinfo
            VkGraphicsPipelineCreateInfo graphics_pipeline_createinfo = {};
            graphics_pipeline_createinfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            graphics_pipeline_createinfo.stageCount = static_cast<uint32_t>(m_shader_stage_createinfos.size());
            graphics_pipeline_createinfo.pStages = m_shader_stage_createinfos.data();
            graphics_pipeline_createinfo.pVertexInputState = &m_vert_input_create_info;
            graphics_pipeline_createinfo.pInputAssemblyState = &m_input_asm_createinfo;
            graphics_pipeline_createinfo.pViewportState = &m_viewport_state_createinfo;
            graphics_pipeline_createinfo.pColorBlendState = &m_colorblend_state_createinfo;
            graphics_pipeline_createinfo.pRasterizationState = &m_rasterization_createinfo;
            graphics_pipeline_createinfo.pMultisampleState = &m_multisample_createinfo;
            graphics_pipeline_createinfo.pDepthStencilState = &m_depth_stencil;
            
            if(m_module.enable_scissor || m_module.enable_custom_viewport)
                graphics_pipeline_createinfo.pDynamicState = &m_dynamic_state_createinfo;
            else graphics_pipeline_createinfo.pDynamicState = nullptr;

            graphics_pipeline_createinfo.layout = m_pipeline_layout;

            graphics_pipeline_createinfo.renderPass = m_render_pass;
            // tmp
            graphics_pipeline_createinfo.subpass = 0;
            graphics_pipeline_createinfo.basePipelineHandle = VK_NULL_HANDLE;

            return graphics_pipeline_createinfo;
        }
    }
}
