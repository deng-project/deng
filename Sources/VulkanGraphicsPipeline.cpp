// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanGraphicsPipeline.cpp - Vulkan graphics pipeline class implementation
// author: Karl-Mihkel Ott

#include <array>
#include "deng/HLSLGraphicsShaderToSpirvCompiler.h"
#include "deng/SpirvByteCodeToolset.h"
#include "deng/VulkanGraphicsPipeline.h"

namespace DENG
{
	namespace Vulkan
	{
		bool GraphicsPipeline::_LoadPipelineCache()
		{
			// check if pipeline hash needs to be calculated
			if (!m_uCrcHash || !m_uXXHash64)
			{
				CalculatePipelineKeyHash();
			}


			std::string sFileName = std::move(_GetCacheFileName());
			LOG("Searching for pipeline cache file " << sFileName);
			

			// check if cache exists in a file
			if (m_programFilesManager.ExistsFile(sFileName))
			{
				LOG("Pipeline cache " << sFileName << " exists, creating a VkPipelineCache object");
				auto cacheBytes = m_programFilesManager.GetProgramFileContent(sFileName);

				VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
				pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
				pipelineCacheCreateInfo.initialDataSize = cacheBytes.size();
				pipelineCacheCreateInfo.pInitialData = cacheBytes.data();

				// perform basic cache validity checks
				VkPipelineCacheHeaderVersionOne pipelineCacheHeader = {};
				std::memcpy(&pipelineCacheHeader, cacheBytes.data(), sizeof(VkPipelineCacheHeaderVersionOne));

				if (pipelineCacheHeader.deviceID == m_uDeviceId && pipelineCacheHeader.vendorID == m_uVendorId)
				{
					if (vkCreatePipelineCache(m_hDevice, &pipelineCacheCreateInfo, nullptr, &m_hPipelineCache) != VK_SUCCESS)
					{
						return false;
					}

					return true;
				}
			}

			return false;
		}

		std::string GraphicsPipeline::_GetCacheFileName()
		{
			const char* szUUID = _CreateUUID();
			int iSize = std::snprintf(nullptr, 0, m_szPipelineCachePath, "Vulkan", szUUID);
			std::string sFileName;
			sFileName.resize(iSize);
			std::snprintf(sFileName.data(), sFileName.size(), m_szPipelineCachePath, "Vulkan", szUUID);
			
			return sFileName;
		}


		void GraphicsPipeline::_CreateDummyPipelineCacheHandle()
		{
			VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
			pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

			if (vkCreatePipelineCache(m_hDevice, &pipelineCacheCreateInfo, nullptr, &m_hPipelineCache) != VK_SUCCESS)
				throw RendererException("Failed to create pipeline cache handle (vkCreatePipelineCache)");
		}


		void GraphicsPipeline::_SerializePipelineCache()
		{
			DENG_ASSERT(m_hPipeline);
			// retrieve the cache
			size_t uCacheSize = 0;
			vkGetPipelineCacheData(m_hDevice, m_hPipelineCache, &uCacheSize, nullptr);
			std::vector<char> cacheData(uCacheSize);
			vkGetPipelineCacheData(m_hDevice, m_hPipelineCache, &uCacheSize, cacheData.data());

			// serialize the cache
			std::string sFileName = std::move(_GetCacheFileName());
			m_programFilesManager.WriteProgramFile(cacheData, sFileName);
		}


		VkShaderModule GraphicsPipeline::_CreateShaderModule(const char* _szBinaryPath, const std::vector<uint32_t>& _spv)
		{
			return VkShaderModule();
		}


		std::vector<VkPipelineShaderStageCreateInfo> GraphicsPipeline::_CreateShaderStageCreateInfos(const std::vector<VkShaderModule>& _shaderModules)
		{
			return std::vector<VkPipelineShaderStageCreateInfo>();
		}


		void GraphicsPipeline::CreatePipeline()
		{
			using ByteCodeToolset = SpirvByteCodeToolset;
			using ByteCodeCompiler = HLSLGraphicsShaderToSpirvCompiler;
			
			auto byteCodes = std::move(_CompileByteCode<ByteCodeCompiler>());
			_CreatePipelineLayout<ByteCodeToolset>(byteCodes);

			if (!_LoadPipelineCache())
			{
				_CreateDummyPipelineCacheHandle();
			}


			std::vector<VkShaderModule> shaderModules;
			shaderModules.reserve(2);
			
			// vertex shader
			shaderModules.push_back(_CreateShaderModule(m_szVertexShaderBinaryPath, byteCodes[0]));
			// geometry shader
			if (!byteCodes[1].empty())
				shaderModules.push_back(_CreateShaderModule(m_szGeometryShaderBinaryPath, byteCodes[1]));
			// fragment shader
			shaderModules.push_back(_CreateShaderModule(m_szFragmentShaderSourcePath, byteCodes[2]));

			std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos = std::move(_CreateShaderStageCreateInfos(shaderModules));
			std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions = std::move(_FindInputBindingDescriptions<ByteCodeToolset>(byteCodes));
			std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions = std::move(_FindInputAttributeDescriptions<ByteCodeToolset>(byteCodes));
			
			VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
			pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindingDescriptions.size());
			pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vertexInputBindingDescriptions.data();
			pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributeDescriptions.size());
			pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();
		
			VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
			pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

			switch (GetPipelinePrimitiveMode())
			{
				case GraphicsPipelinePrimitiveMode::Points:
					pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
					break;

				case GraphicsPipelinePrimitiveMode::Lines:
					pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
					break;

				case GraphicsPipelinePrimitiveMode::Triangles:
					pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
					break;

				default:
					throw RendererException("Cannot create a pipeline with no primitive mode specified");
					break;
			}

			VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
			pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
			pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
			pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
			pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

			// setup graphics pipeline culling mode
			switch (GetPipelineCullMode())
			{
				case GraphicsPipelineCullMode::None:
					pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
					break;

				case GraphicsPipelineCullMode::CounterClockwise:
					pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
					pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
					break;

				case GraphicsPipelineCullMode::Clockwise:
					pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
					pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
					break;
			}

			VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
			pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
			pipelineMultisampleStateCreateInfo.rasterizationSamples = m_uSampleCountFlagBits;

			VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {};
			if (this->IsFlagSet(GraphicsShaderFlag_EnableBlend))
			{
				pipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
				pipelineColorBlendAttachmentState.blendEnable = VK_TRUE;
				pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
				pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
				pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
				pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
				pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			}
			else
			{
				pipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
				pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
			}

			VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
			pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			
			// check if depth testing is enabled
			if (this->IsFlagSet(GraphicsShaderFlag_EnableDepthTesting))
			{
				pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
				pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
				pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
				pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
			}
			else
			{
				pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
				pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
			}

			VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {};
			std::array<VkDynamicState, 3> dynamicStates = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR,
				VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE
			};

			pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
			pipelineDynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

			VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
			pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
			pipelineColorBlendStateCreateInfo.attachmentCount = 1;
			pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;

			VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
			pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			pipelineViewportStateCreateInfo.scissorCount = 1;
			pipelineViewportStateCreateInfo.viewportCount = 1;

			VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
			graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStageCreateInfos.size());
			graphicsPipelineCreateInfo.pStages = shaderStageCreateInfos.data();
			graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
			graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
			graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
			graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
			graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
			graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
			graphicsPipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
			graphicsPipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
			graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
			graphicsPipelineCreateInfo.layout = m_hPipelineLayout;
			graphicsPipelineCreateInfo.renderPass = m_hRenderPass;
		
			if (vkCreateGraphicsPipelines(m_hDevice, m_hPipelineCache, 1, &graphicsPipelineCreateInfo, nullptr, &m_hPipeline) != VK_SUCCESS)
				throw RendererException("vkCreateGraphicsPipelines(): could not create graphics pipeline");

			_SerializePipelineCache();
		}
	}
}