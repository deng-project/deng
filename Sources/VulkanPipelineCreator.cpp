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
            VkDescriptorSetLayout _hMaterialDescriptorSetLayout,
            VkSampleCountFlagBits _uSampleBits, 
            const Vulkan::PhysicalDeviceInformation& _information,
            const IGraphicsShader* _pShader) :
            m_hDevice(_hDevice),
            m_hShaderDescriptorSetLayout(_hShaderDescriptorSetLayout),
            m_hMaterialDescriptorSetLayout(_hMaterialDescriptorSetLayout),
            m_uSampleBits(_uSampleBits),
            m_hRenderPass(_hRenderPass)
        {
            try {
                _CreatePipelineLayout(_pShader);
            }
            catch (const RendererException& e) {
                DISPATCH_ERROR_MESSAGE("RendererException", e.what(), CRITICAL);
            }
            
            VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
            pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

            if (_pShader->GetPipelineCacheStatus() == PipelineCacheStatus_CachedPipeline) {
                auto cacheBytes = _pShader->GetPipelineCache();
                
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

            try {
                _GeneratePipelineCreateInfo(_pShader);
            }
            catch (const ShaderException&) {
                throw RendererException("Could not create PipelineCreator. Shader source code is invalid.");
            }
            catch (const IOException&) {
                throw RendererException("Could not create PipelineCreator. Shader source code is missing.");
            }
            
            // create a pipeline
            if (vkCreateGraphicsPipelines(m_hDevice, m_hPipelineCache, 1, &m_graphicsPipelineCreateInfo, nullptr, &m_hPipeline) != VK_SUCCESS) {
                throw RendererException("vkCreateGraphicsPipelines() failed to create a graphics pipeline");
            }
        
            if (!pipelineCacheCreateInfo.initialDataSize) {
                size_t uCacheSize = 0;
                vkGetPipelineCacheData(m_hDevice, m_hPipelineCache, &uCacheSize, nullptr);
                vector<char> cacheData(uCacheSize);
                vkGetPipelineCacheData(m_hDevice, m_hPipelineCache, &uCacheSize, cacheData.data());

                _pShader->CachePipeline(cacheData.data(), cacheData.size());
            }
        }


        PipelineCreator::PipelineCreator(PipelineCreator &&_pc) noexcept :
            m_hDevice(_pc.m_hDevice),
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
            m_hMaterialDescriptorSetLayout(_pc.m_hMaterialDescriptorSetLayout),
            m_hPipelineLayout(_pc.m_hPipelineLayout),
            m_hPipeline(_pc.m_hPipeline),
            m_hRenderPass(_pc.m_hRenderPass),
            m_hPipelineCache(_pc.m_hPipelineCache)
        {
            _pc.m_hShaderDescriptorSetLayout = VK_NULL_HANDLE;
            _pc.m_hMaterialDescriptorSetLayout = VK_NULL_HANDLE;
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

        
        void PipelineCreator::_FindInputBindingDescriptions(const IGraphicsShader* _pShader) {
            DENG_ASSERT(_pShader->GetAttributeTypes().size() == _pShader->GetAttributeStrides().size());
            m_vertexInputBindingDescriptions.reserve(_pShader->GetAttributeTypes().size());

            for (size_t i = 0; i < _pShader->GetAttributeTypes().size(); i++) {
                m_vertexInputBindingDescriptions.emplace_back();
                m_vertexInputBindingDescriptions.back().binding = static_cast<uint32_t>(i);
                m_vertexInputBindingDescriptions.back().stride = static_cast<uint32_t>(_pShader->GetAttributeStrides()[i]);
                m_vertexInputBindingDescriptions.back().inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            }
        }


        void PipelineCreator::_FindVertexInputAttributeDescriptions(const IGraphicsShader* _pShader) {
            for(uint32_t i = 0; i < static_cast<uint32_t>(_pShader->GetAttributeTypes().size()); i++) {
                m_vertexInputAttributeDescriptions.push_back(VkVertexInputAttributeDescription{});
                m_vertexInputAttributeDescriptions.back().binding = i;
                m_vertexInputAttributeDescriptions.back().location = i;

                switch(_pShader->GetAttributeTypes()[i]) {
                    // single element attributes
                    case VertexAttributeType::Float:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32_SFLOAT;
                        break;

                    case VertexAttributeType::Double:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R64_SFLOAT;
                        break;

                    case VertexAttributeType::Byte:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R8_SINT;
                        break;

                    case VertexAttributeType::UnsignedByte:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R8_UINT;
                        break;

                    case VertexAttributeType::Short:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R16_SINT;
                        break;
                    
                    case VertexAttributeType::UnsignedShort:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R16_UINT;
                        break;

                    case VertexAttributeType::UnsignedInt:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32_UINT;
                        break;
                    
                    case VertexAttributeType::Int:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32_SINT;
                        break;

                    // two element attributes
                    case VertexAttributeType::Vec2_Float:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32G32_SFLOAT;
                        break;

                    case VertexAttributeType::Vec2_Double:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R64G64_SFLOAT;
                        break;

                    case VertexAttributeType::Vec2_Byte:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R8G8_SNORM;
                        break;

                    case VertexAttributeType::Vec2_UnsignedByte:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R8G8_UNORM;
                        break;

                    case VertexAttributeType::Vec2_Short:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R16G16_SINT;
                        break;
                    
                    case VertexAttributeType::Vec2_UnsignedShort:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R16G16_UINT;
                        break;

                    case VertexAttributeType::Vec2_UnsignedInt:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32G32_UINT;
                        break;
                    
                    case VertexAttributeType::Vec2_Int:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32G32_SINT;
                        break;

                    // three element attributes
                    case VertexAttributeType::Vec3_Float:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32G32B32_SFLOAT;
                        break;

                    case VertexAttributeType::Vec3_Double:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R64G64B64_SFLOAT;
                        break;

                    case VertexAttributeType::Vec3_Byte:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R8G8B8_SNORM;
                        break;

                    case VertexAttributeType::Vec3_UnsignedByte:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R8G8B8_UNORM;
                        break;

                    case VertexAttributeType::Vec3_Short:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R16G16B16_SINT;
                        break;
                    
                    case VertexAttributeType::Vec3_UnsignedShort:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R16G16B16_UINT;
                        break;

                    case VertexAttributeType::Vec3_UnsignedInt:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32G32B32_UINT;
                        break;
                    
                    case VertexAttributeType::Vec3_Int:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32G32B32_SINT;
                        break;

                    // four element attributes
                    case VertexAttributeType::Vec4_Float:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32G32B32A32_SFLOAT;
                        break;

                    case VertexAttributeType::Vec4_Double:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R64G64B64A64_SFLOAT;
                        break;

                    case VertexAttributeType::Vec4_Byte:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R8G8B8A8_SNORM;
                        break;

                    case VertexAttributeType::Vec4_UnsignedByte:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R8G8B8A8_UNORM;
                        break;

                    case VertexAttributeType::Vec4_Short:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R16G16B16A16_SINT;
                        break;
                    
                    case VertexAttributeType::Vec4_UnsignedShort:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R16G16B16A16_UINT;
                        break;

                    case VertexAttributeType::Vec4_UnsignedInt:
                        m_vertexInputAttributeDescriptions.back().format = VK_FORMAT_R32G32B32A32_UINT;
                        break;
                    
                    case VertexAttributeType::Vec4_Int:
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


        void PipelineCreator::_CreatePipelineLayout(const IGraphicsShader* _pShader) {
            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
            pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            
            // 1. only per shader descriptor set layouts are present
            // 2. only per mesh descriptor set layouts are present
            // 3. all descriptor set layouts are present
            VkDescriptorSetLayout descriptorSetLayouts[2] = { 
                m_hShaderDescriptorSetLayout, 
                m_hMaterialDescriptorSetLayout 
            };

            if(m_hShaderDescriptorSetLayout != VK_NULL_HANDLE && m_hMaterialDescriptorSetLayout == VK_NULL_HANDLE) {
                pipelineLayoutCreateInfo.setLayoutCount = 1;
                pipelineLayoutCreateInfo.pSetLayouts = &m_hShaderDescriptorSetLayout;
            } else if(m_hShaderDescriptorSetLayout == VK_NULL_HANDLE && m_hMaterialDescriptorSetLayout != VK_NULL_HANDLE) {
                pipelineLayoutCreateInfo.setLayoutCount = 1;
                pipelineLayoutCreateInfo.pSetLayouts = &m_hMaterialDescriptorSetLayout;
            } else if (m_hShaderDescriptorSetLayout != VK_NULL_HANDLE && m_hMaterialDescriptorSetLayout != VK_NULL_HANDLE) {
                pipelineLayoutCreateInfo.setLayoutCount = 2;
                pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts;
            }

            VkPushConstantRange pushConstantRange = {};
            if (_pShader->IsPropertySet(ShaderPropertyBit_EnablePushConstants)) {
                pushConstantRange.size = _pShader->GetPushConstant().uLength;
                if (_pShader->GetPushConstant().bmShaderStage & ShaderStageBit_Vertex)
                    pushConstantRange.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
                if (_pShader->GetPushConstant().bmShaderStage & ShaderStageBit_Fragment)
                    pushConstantRange.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
                if (_pShader->GetPushConstant().bmShaderStage & ShaderStageBit_Geometry)
                    pushConstantRange.stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;

                pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
                pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
            }

            if(vkCreatePipelineLayout(m_hDevice, &pipelineLayoutCreateInfo, NULL, &m_hPipelineLayout) != VK_SUCCESS)
                VK_PIPELINEC_ERR("failed to create a pipeline layout");
        }


        // pipelines should be created from skratch only when there is no cache available
        void PipelineCreator::_GeneratePipelineCreateInfo(const IGraphicsShader* _pShader, bool _bCreateShaderModules) {
            if(_bCreateShaderModules) {
                // Create vertex and fragment shader modules
                m_shaderModules.clear();
                m_shaderModules.reserve(3);
                
                std::vector<uint32_t> vertexShaderSpirv, geometryShaderSpirv, fragmentShaderSpirv;
                try {
                    vertexShaderSpirv = _pShader->GetVertexShaderByteCode();
                    fragmentShaderSpirv = _pShader->GetFragmentShaderByteCode();
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
                    geometryShaderSpirv = _pShader->GetGeometryShaderByteCode();
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
                _FindInputBindingDescriptions(_pShader);
                _FindVertexInputAttributeDescriptions(_pShader);
            }

            // Set up vertex input create_info object 
            m_vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            m_vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(m_vertexInputBindingDescriptions.size());
            m_vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_vertexInputAttributeDescriptions.size());
            m_vertexInputStateCreateInfo.pVertexBindingDescriptions = m_vertexInputBindingDescriptions.data();
            m_vertexInputStateCreateInfo.pVertexAttributeDescriptions = m_vertexInputAttributeDescriptions.data();

            // Set up input assembly create_info object
            m_inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            switch(_pShader->GetPrimitiveMode()) {
                case PrimitiveMode::Triangles:
                    m_inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                    break;

                case PrimitiveMode::Lines:
                    m_inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
                    break;

                case PrimitiveMode::Points:
                    m_inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
                    break;

                default:
                    break;
            }
            m_inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

            // Set up viewport state create_info object
            m_viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

            // Set up rasterization create info
            m_rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            m_rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
            m_rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
            m_rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
            m_rasterizationStateCreateInfo.lineWidth = 1.0f;

            // set up culling mode
            switch(_pShader->GetPipelineCullMode()) {
                case PipelineCullMode::None:
                    m_rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
                    break;

                case PipelineCullMode::CounterClockwise:
                    m_rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
                    m_rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
                    break;

                case PipelineCullMode::Clockwise:
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
            if(_pShader->IsPropertySet(ShaderPropertyBit_EnableBlend)) {
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
            if(_pShader->IsPropertySet(ShaderPropertyBit_EnableDepthTesting)) {
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
            m_dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
            m_dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

            m_dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            m_dynamicStateCreateInfo.dynamicStateCount = 2;
            m_dynamicStateCreateInfo.pDynamicStates = m_dynamicStates.data();

            // Set up colorblend state create_info
            m_colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            m_colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
            m_colorBlendStateCreateInfo.attachmentCount = 1;
            m_colorBlendStateCreateInfo.pAttachments = &m_colorBlendAttachmentState;

            m_viewportStateCreateInfo = {};
            m_viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            m_viewportStateCreateInfo.scissorCount = 1;
            m_viewportStateCreateInfo.viewportCount = 1;

            // Set up graphics pipeline create_info
            m_graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            m_graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(m_shaderStageCreateInfos.size());
            m_graphicsPipelineCreateInfo.pStages = m_shaderStageCreateInfos.data();
            m_graphicsPipelineCreateInfo.pVertexInputState = &m_vertexInputStateCreateInfo;
            m_graphicsPipelineCreateInfo.pInputAssemblyState = &m_inputAssemblyStateCreateInfo;
            m_graphicsPipelineCreateInfo.pViewportState = &m_viewportStateCreateInfo;
            m_graphicsPipelineCreateInfo.pColorBlendState = &m_colorBlendStateCreateInfo;
            m_graphicsPipelineCreateInfo.pRasterizationState = &m_rasterizationStateCreateInfo;
            m_graphicsPipelineCreateInfo.pMultisampleState = &m_multisampleStateCreateInfo;
            m_graphicsPipelineCreateInfo.pDepthStencilState = &m_depthStencilStateCreateInfo;
            m_graphicsPipelineCreateInfo.pDynamicState = &m_dynamicStateCreateInfo;
            m_graphicsPipelineCreateInfo.pViewportState = &m_viewportStateCreateInfo;
            m_graphicsPipelineCreateInfo.layout = m_hPipelineLayout;
            m_graphicsPipelineCreateInfo.renderPass = m_hRenderPass;

            // tmp
            m_graphicsPipelineCreateInfo.subpass = 0;
            m_graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        }
    }
}
