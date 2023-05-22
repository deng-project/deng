// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanPipelineCreator.cpp - Vulkan pipeline creation class implementation
// author: Karl-Mihkel Ott

#define VULKAN_PIPELINE_CREATOR_CPP
#include "deng/VulkanPipelineCreator.h"

using namespace std;

namespace DENG {
    namespace Vulkan {

        PipelineCreator::PipelineCreator(
            VkDevice _hDevice, 
            VkRenderPass _hRenderPass,
            VkDescriptorSetLayout _hShaderDescriptorSetLayout,
            VkDescriptorSetLayout _hMeshDescriptorSetLayout,
            VkExtent2D _extent, 
            VkSampleCountFlagBits _uSampleBits, 
            const Vulkan::PhysicalDeviceInformation& _information, 
            const PipelineModule& _pipeline) :
            m_hDevice(_hDevice),
            m_extent(_extent),
            m_hShaderDescriptorSetLayout(_hShaderDescriptorSetLayout),
            m_hMeshDescriptorSetLayout(_hMeshDescriptorSetLayout),
            m_uSampleBits(_uSampleBits),
            m_hRenderPass(_hRenderPass)
        {
            try {
                _CreatePipelineLayout();
            }
            catch (const RendererException& e) {
                DISPATCH_ERROR_MESSAGE("RendererException", e.what(), CRITICAL);
            }
            
            VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
            pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

            if (_pipeline.pShader->GetPipelineCacheStatus() & VULKAN_CACHE) {
                auto cacheBytes = _pipeline.pShader->GetPipelineCache(RendererType::VULKAN);
                
                VkPipelineCacheHeaderVersionOne header = {};
                memcpy(&header, cacheBytes.data(), sizeof(VkPipelineCacheHeaderVersionOne));

                if (header.deviceID == _information.uDeviceId && header.vendorID == _information.uVendorId) {
                    pipelineCacheCreateInfo.initialDataSize = cacheBytes.size();
                    pipelineCacheCreateInfo.pInitialData = cacheBytes.data();

                    if (vkCreatePipelineCache(m_hDevice, &pipelineCacheCreateInfo, nullptr, &m_hPipelineCache) != VK_SUCCESS)
                        throw RendererException("vkCreatePipelineCache() failed to create pipeline cache");
                }
            }

            if (m_hPipelineCache == VK_NULL_HANDLE) {
                if (vkCreatePipelineCache(m_hDevice, &pipelineCacheCreateInfo, nullptr, &m_hPipelineCache) != VK_SUCCESS)
                    throw RendererException("vkCreatePipelineCache() failed to create pipeline cache");
            }

            VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
            try {
                graphicsPipelineCreateInfo = _GeneratePipelineCreateInfo(_pipeline);
            }
            catch (const ShaderException&) {
                throw RendererException("Could not create PipelineCreator. Shader source code is invalid.");
            }
            catch (const IOException&) {
                throw RendererException("Could not create PipelineCreator. Shader source code is missing.");
            }
            
            // create a pipeline
            if (vkCreateGraphicsPipelines(m_hDevice, m_hPipelineCache, 1, &graphicsPipelineCreateInfo, nullptr, &m_hPipeline) != VK_SUCCESS) {
                throw RendererException("vkCreateGraphicsPipelines() failed to create a graphics pipeline");
            }
        
            if (!pipelineCacheCreateInfo.initialDataSize) {
                size_t uCacheSize = 0;
                vkGetPipelineCacheData(m_hDevice, m_hPipelineCache, &uCacheSize, nullptr);
                vector<char> cacheData(uCacheSize);
                vkGetPipelineCacheData(m_hDevice, m_hPipelineCache, &uCacheSize, cacheData.data());

                _pipeline.pShader->CachePipeline(RendererType::VULKAN, cacheData.data(), cacheData.size());
            }
        }


        PipelineCreator::PipelineCreator(PipelineCreator &&_pc) noexcept :
            m_hDevice(_pc.m_hDevice),
            m_viewport(_pc.m_viewport),
            m_scissor(_pc.m_scissor),
            m_extent(_pc.m_extent),
            m_uSampleBits(_pc.m_uSampleBits),
            m_shaderStageCreateInfos(move(_pc.m_shaderStageCreateInfos)),
            m_shaderModules(move(_pc.m_shaderModules)),
            m_vertexInputBindingDescriptions(move(_pc.m_vertexInputBindingDescriptions)),
            m_vertexInputAttributeDescriptions(move(_pc.m_vertexInputAttributeDescriptions)),
            m_vertexInputStateCreateInfo(_pc.m_vertexInputStateCreateInfo),
            m_inputAssemblyStateCreateInfo(_pc.m_inputAssemblyStateCreateInfo),
            m_viewportStateCreateInfo(_pc.m_viewportStateCreateInfo),
            m_rasterizationStateCreateInfo(_pc.m_rasterizationStateCreateInfo),
            m_multisampleStateCreateInfo(_pc.m_multisampleStateCreateInfo),
            m_colorBlendAttachmentState(_pc.m_colorBlendAttachmentState),
            m_depthStencilStateCreateInfo(_pc.m_depthStencilStateCreateInfo),
            m_colorBlendStateCreateInfo(_pc.m_colorBlendStateCreateInfo),
            m_dynamicStateCreateInfo(_pc.m_dynamicStateCreateInfo),
            m_dynamicStates(move(_pc.m_dynamicStates)),
            m_hShaderDescriptorSetLayout(_pc.m_hShaderDescriptorSetLayout),
            m_hMeshDescriptorSetLayout(_pc.m_hMeshDescriptorSetLayout),
            m_hPipelineLayout(_pc.m_hPipelineLayout),
            m_hPipeline(_pc.m_hPipeline),
            m_hRenderPass(_pc.m_hRenderPass),
            m_hPipelineCache(_pc.m_hPipelineCache)
        {
            _pc.m_hShaderDescriptorSetLayout = VK_NULL_HANDLE;
            _pc.m_hMeshDescriptorSetLayout = VK_NULL_HANDLE;
            _pc.m_hPipelineLayout = VK_NULL_HANDLE;
            _pc.m_hPipeline = VK_NULL_HANDLE;
            _pc.m_hPipelineCache = VK_NULL_HANDLE;
        }


        PipelineCreator::~PipelineCreator() noexcept {
            if (m_hPipelineCache != VK_NULL_HANDLE)
                vkDestroyPipelineCache(m_hDevice, m_hPipelineCache, nullptr);
            
            for(size_t i = 0; i < m_shaderModules.size(); i++)
                vkDestroyShaderModule(m_hDevice, m_shaderModules[i], NULL);

            if(m_hPipeline != VK_NULL_HANDLE)
                vkDestroyPipeline(m_hDevice, m_hPipeline, nullptr);
            if(m_hPipelineLayout != VK_NULL_HANDLE)
                vkDestroyPipelineLayout(m_hDevice, m_hPipelineLayout, nullptr);
        } 


        void PipelineCreator::DestroyPipelineData() {
            vkDestroyPipeline(m_hDevice, m_hPipeline, nullptr);
            m_hPipeline = VK_NULL_HANDLE;
            vkDestroyPipelineLayout(m_hDevice, m_hPipelineLayout, nullptr);
            m_hPipelineLayout = VK_NULL_HANDLE;
        }


        void PipelineCreator::RecreatePipeline(const PipelineModule& _pipeline, VkRenderPass _hRenderPass, VkExtent2D _extent, bool _bRecompile) {
            m_hRenderPass = _hRenderPass;
            m_extent = _extent;

            _CreatePipelineLayout();
            VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = _GeneratePipelineCreateInfo(_pipeline, _bRecompile);

            if(vkCreateGraphicsPipelines(m_hDevice, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &m_hPipeline) != VK_SUCCESS)
                VK_PIPELINEC_ERR("failed to create a pipeline");
        }


        void PipelineCreator::_FindInputBindingDescriptions(const PipelineModule& _pipeline) {
            DENG_ASSERT(_pipeline.attributes.size() == _pipeline.attributeStrides.size());
            m_vertexInputBindingDescriptions.reserve(_pipeline.attributes.size());

            for (size_t i = 0; i < _pipeline.attributes.size(); i++) {
                m_vertexInputBindingDescriptions.emplace_back();
                m_vertexInputBindingDescriptions.back().binding = static_cast<uint32_t>(i);;
                m_vertexInputBindingDescriptions.back().stride = static_cast<uint32_t>(_pipeline.attributeStrides[i]);
                m_vertexInputBindingDescriptions.back().inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            }
        }


        void PipelineCreator::_FindVertexInputAttributeDescriptions(const PipelineModule &_pipeline) {
            for(uint32_t i = 0; i < static_cast<uint32_t>(_pipeline.attributes.size()); i++) {
                m_vertexInputAttributeDescriptions.push_back(VkVertexInputAttributeDescription{});
                m_vertexInputAttributeDescriptions.back().binding = i;
                m_vertexInputAttributeDescriptions.back().location = i;

                switch(_pipeline.attributes[i]) {
                    // single element attributes
                    case ATTRIBUTE_TYPE_FLOAT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_DOUBLE:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R64_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_BYTE:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R8_SINT;
                        break;

                    case ATTRIBUTE_TYPE_UBYTE:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R8_UINT;
                        break;

                    case ATTRIBUTE_TYPE_SHORT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R16_SINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_USHORT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R16_UINT;
                        break;

                    case ATTRIBUTE_TYPE_UINT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32_UINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_INT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32_SINT;
                        break;

                    // two element attributes
                    case ATTRIBUTE_TYPE_VEC2_FLOAT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32G32_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC2_DOUBLE:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R64G64_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC2_BYTE:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R8G8_SNORM;
                        break;

                    case ATTRIBUTE_TYPE_VEC2_UBYTE:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R8G8_UNORM;
                        break;

                    case ATTRIBUTE_TYPE_VEC2_SHORT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R16G16_SINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_VEC2_USHORT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R16G16_UINT;
                        break;

                    case ATTRIBUTE_TYPE_VEC2_UINT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32G32_UINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_VEC2_INT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32G32_SINT;
                        break;

                    // three element attributes
                    case ATTRIBUTE_TYPE_VEC3_FLOAT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32G32B32_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC3_DOUBLE:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R64G64B64_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC3_BYTE:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R8G8B8_SNORM;
                        break;

                    case ATTRIBUTE_TYPE_VEC3_UBYTE:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R8G8B8_UNORM;
                        break;

                    case ATTRIBUTE_TYPE_VEC3_SHORT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R16G16B16_SINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_VEC3_USHORT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R16G16B16_UINT;
                        break;

                    case ATTRIBUTE_TYPE_VEC3_UINT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32G32B32_UINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_VEC3_INT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32G32B32_SINT;
                        break;

                    // four element attributes
                    case ATTRIBUTE_TYPE_VEC4_FLOAT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32G32B32A32_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC4_DOUBLE:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R64G64B64A64_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC4_BYTE:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R8G8B8A8_SNORM;
                        break;

                    case ATTRIBUTE_TYPE_VEC4_UBYTE:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R8G8B8A8_UNORM;
                        break;

                    case ATTRIBUTE_TYPE_VEC4_SHORT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R16G16B16A16_SINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_VEC4_USHORT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R16G16B16A16_UINT;
                        break;

                    case ATTRIBUTE_TYPE_VEC4_UINT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32G32B32A32_UINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_VEC4_INT:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32G32B32A32_SINT;
                        break;
                }

                m_vertexInputAttributeDescriptions.back().offset = 0;
            }
        }


        VkShaderModule PipelineCreator::_CreateShaderModule(std::vector<uint32_t> &_bin) {
            VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
            shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shaderModuleCreateInfo.codeSize = static_cast<uint32_t>(_bin.size() * sizeof(uint32_t));
            shaderModuleCreateInfo.pCode = _bin.data();

            VkShaderModule shader_module;
            if (vkCreateShaderModule(m_hDevice, &shaderModuleCreateInfo, NULL, &shader_module) != VK_SUCCESS)
                throw ShaderException("vkCreateShaderModule() failed to create a shader module");

            return shader_module;
        }


        void PipelineCreator::_CreatePipelineLayout() {
            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
            pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
            
            // 1. only per shader descriptor set layouts are present
            // 2. only per mesh descriptor set layouts are present
            // 3. all descriptor set layouts are present
            if(m_hShaderDescriptorSetLayout != VK_NULL_HANDLE && m_hMeshDescriptorSetLayout == VK_NULL_HANDLE) {
                pipelineLayoutCreateInfo.setLayoutCount = 1;
                pipelineLayoutCreateInfo.pSetLayouts = &m_hShaderDescriptorSetLayout;
            } else if(m_hShaderDescriptorSetLayout == VK_NULL_HANDLE && m_hMeshDescriptorSetLayout != VK_NULL_HANDLE) {
                pipelineLayoutCreateInfo.setLayoutCount = 1;
                pipelineLayoutCreateInfo.pSetLayouts = &m_hMeshDescriptorSetLayout;
            } else if (m_hShaderDescriptorSetLayout != VK_NULL_HANDLE && m_hMeshDescriptorSetLayout != VK_NULL_HANDLE) {
                pipelineLayoutCreateInfo.setLayoutCount = 2;
                VkDescriptorSetLayout descriptorSetLayouts[2] = { m_hShaderDescriptorSetLayout, m_hMeshDescriptorSetLayout };
                pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts;
            }

            if(vkCreatePipelineLayout(m_hDevice, &pipelineLayoutCreateInfo, NULL, &m_hPipelineLayout) != VK_SUCCESS)
                VK_PIPELINEC_ERR("failed to create a pipeline layout");
        }


        // pipelines should be created from skratch only when there is no cache available
        VkGraphicsPipelineCreateInfo PipelineCreator::_GeneratePipelineCreateInfo(const PipelineModule& _pipeline, bool _bCreateShaderModules) {
            if(_bCreateShaderModules) {
                // Create vertex and fragment shader modules
                m_shaderModules.clear();
                m_shaderModules.reserve(3);
                
                std::vector<uint32_t> vertexShaderSpirv, geometryShaderSpirv, fragmentShaderSpirv;
                try {
                    vertexShaderSpirv = _pipeline.pShader->ReadVertexShaderSpirv();
                    fragmentShaderSpirv = _pipeline.pShader->ReadFragmentShaderSpirv();
                }
                catch (const IOException& e) {
                    DISPATCH_ERROR_MESSAGE("IOException", e.what(), ErrorSeverity::NON_CRITICAL);
                    throw;
                }
                catch (const ShaderException& e) {
                    DISPATCH_ERROR_MESSAGE("ShaderException", e.what(), ErrorSeverity::NON_CRITICAL);
                    throw;
                }

                try {
                    geometryShaderSpirv = _pipeline.pShader->ReadGeometryShaderSpirv();
                }
                catch (const IOException& e) {
                    DISPATCH_ERROR_MESSAGE("IOException", e.what(), ErrorSeverity::NON_CRITICAL);
                }
                catch (const ShaderException& e) {
                    DISPATCH_ERROR_MESSAGE("ShaderException", e.what(), ErrorSeverity::NON_CRITICAL);
                }

                m_shaderModules.push_back(_CreateShaderModule(vertexShaderSpirv));
                if(geometryShaderSpirv.size())
                    m_shaderModules.push_back(_CreateShaderModule(geometryShaderSpirv));
                m_shaderModules.push_back(_CreateShaderModule(fragmentShaderSpirv));

                // Create vertex shader stage create_info
                m_shaderStageCreateInfos.emplace_back();
                m_shaderStageCreateInfos.back().sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                m_shaderStageCreateInfos.back().stage = VK_SHADER_STAGE_VERTEX_BIT;
                m_shaderStageCreateInfos.back().module = m_shaderModules[0];
                m_shaderStageCreateInfos.back().pName = "main";

                // check if geomety shader stage create info needs to be 
                if(geometryShaderSpirv.size()) {
                    m_shaderStageCreateInfos.emplace_back();
                    m_shaderStageCreateInfos.back().sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                    m_shaderStageCreateInfos.back().stage = VK_SHADER_STAGE_GEOMETRY_BIT;
                    m_shaderStageCreateInfos.back().module = m_shaderModules[1];
                    m_shaderStageCreateInfos.back().pName = "main";

                    m_shaderStageCreateInfos.emplace_back();
                    m_shaderStageCreateInfos.back().module = m_shaderModules[2];
                }
                else {
                    m_shaderStageCreateInfos.emplace_back();
                    m_shaderStageCreateInfos.back().module = m_shaderModules[1];
                }

                // Create fragment shader stage create_info
                m_shaderStageCreateInfos.back().sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                m_shaderStageCreateInfos.back().stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                m_shaderStageCreateInfos.back().pName = "main";

                // Bind get binding descriptors and attribute descriptors for the current pipeline type
                _FindInputBindingDescriptions(_pipeline);
                _FindVertexInputAttributeDescriptions(_pipeline);
            }

            // Set up vertex input create_info object 
            m_vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            m_vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(m_vertexInputBindingDescriptions.size());
            m_vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_vertexInputAttributeDescriptions.size());
            m_vertexInputStateCreateInfo.pVertexBindingDescriptions = m_vertexInputBindingDescriptions.data();
            m_vertexInputStateCreateInfo.pVertexAttributeDescriptions = m_vertexInputAttributeDescriptions.data();

            // Set up input assembly create_info object
            m_inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            switch(_pipeline.ePrimitiveMode) {
                case PRIMITIVE_MODE_TRIANGLES:
                    m_inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                    break;

                case PRIMITIVE_MODE_LINES:
                    m_inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
                    break;

                case PRIMITIVE_MODE_POINTS:
                    m_inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
                    break;
            }
            m_inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

            // Set viewport values
            m_viewport.width = static_cast<float>(m_extent.width);
            m_viewport.height = -static_cast<float>(m_extent.height);
            m_viewport.x = 0.0f;
            m_viewport.y = -m_viewport.height;
            m_viewport.minDepth = 0.0f;
            m_viewport.maxDepth = 1.0f;

            // Set scissor offset and extent values
            m_scissor.offset = {0, 0};
            m_scissor.extent = m_extent;

            // Set up viewport state create_info object
            m_viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            m_viewportStateCreateInfo.viewportCount = 1;
            m_viewportStateCreateInfo.pViewports = &m_viewport;
            m_viewportStateCreateInfo.scissorCount = 1;
            m_viewportStateCreateInfo.pScissors = &m_scissor;

            // Set up rasterization create info
            m_rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            m_rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
            m_rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
            m_rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
            m_rasterizationStateCreateInfo.lineWidth = 1.0f;

            // set up culling mode
            switch(_pipeline.eCullMode) {
                case CULL_MODE_NONE:
                    m_rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
                    break;

                case CULL_MODE_COUNTER_CLOCKWISE:
                    m_rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
                    m_rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
                    break;

                case CULL_MODE_CLOCKWISE:
                    m_rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
                    m_rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
                    break;

                default:
                    break;
            }

            // Set up multisampling create_info
            m_multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            m_multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
            m_multisampleStateCreateInfo.rasterizationSamples = m_uSampleBits;

            // Set colorblend options if required
            if(_pipeline.bEnableBlend) {
                m_colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
                m_colorBlendAttachmentState.blendEnable = VK_TRUE;
                m_colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
                m_colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                m_colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                m_colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
                m_colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                m_colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            } else {
                m_colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
                m_colorBlendAttachmentState.blendEnable = VK_FALSE;
            }
            
            // Check if depth stencil is enabled and if it is set the create_info accordingly
            if(_pipeline.bEnableDepthTesting) {
                m_depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                m_depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
                m_depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
                m_depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
                m_depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
                m_depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
            }
            else {
                m_depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                m_depthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
                m_depthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
                m_depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
                m_depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
            }

            m_dynamicStateCreateInfo = {};
            m_dynamicStates.clear();
            m_dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

            if (_pipeline.bEnableCustomViewport) {
                m_dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
                m_dynamicStateCreateInfo.dynamicStateCount++;
            }

            if (_pipeline.bEnableScissor) {
                m_dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
                m_dynamicStateCreateInfo.dynamicStateCount++;
            }
            m_dynamicStateCreateInfo.pDynamicStates = m_dynamicStates.data();

            // Set up colorblend state create_info
            m_colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            m_colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
            m_colorBlendStateCreateInfo.attachmentCount = 1;
            m_colorBlendStateCreateInfo.pAttachments = &m_colorBlendAttachmentState;

            // Set up graphics pipeline create_info
            VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
            graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(m_shaderStageCreateInfos.size());
            graphicsPipelineCreateInfo.pStages = m_shaderStageCreateInfos.data();
            graphicsPipelineCreateInfo.pVertexInputState = &m_vertexInputStateCreateInfo;
            graphicsPipelineCreateInfo.pInputAssemblyState = &m_inputAssemblyStateCreateInfo;
            graphicsPipelineCreateInfo.pViewportState = &m_viewportStateCreateInfo;
            graphicsPipelineCreateInfo.pColorBlendState = &m_colorBlendStateCreateInfo;
            graphicsPipelineCreateInfo.pRasterizationState = &m_rasterizationStateCreateInfo;
            graphicsPipelineCreateInfo.pMultisampleState = &m_multisampleStateCreateInfo;
            graphicsPipelineCreateInfo.pDepthStencilState = &m_depthStencilStateCreateInfo;

            if(_pipeline.bEnableScissor || _pipeline.bEnableCustomViewport)
                graphicsPipelineCreateInfo.pDynamicState = &m_dynamicStateCreateInfo;
            else graphicsPipelineCreateInfo.pDynamicState = nullptr;

            graphicsPipelineCreateInfo.layout = m_hPipelineLayout;

            graphicsPipelineCreateInfo.renderPass = m_hRenderPass;

            // tmp
            graphicsPipelineCreateInfo.subpass = 0;
            graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

            return graphicsPipelineCreateInfo;
        }
    }
}
