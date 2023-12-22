// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanGraphicsPipeline.cpp - Vulkan graphics pipeline class implementation
// author: Karl-Mihkel Ott

#include "deng/HLSLGraphicsShaderSpirvCompiler.h"
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
			_CreatePipelineLayout();
			if (!_LoadPipelineCache())
			{
				_CreateDummyPipelineCacheHandle();
			}

			// compile shaders using HLSLGraphicsShaderSpirvCompiler
			using ByteCodeCompiler = HLSLGraphicsShaderSpirvCompiler;
			auto byteCodes = _CompileByteCode<ByteCodeCompiler>();

			std::vector<VkShaderModule> shaderModules;
			shaderModules.reserve(2);
			
			// vertex shader
			shaderModules.push_back(_CreateShaderModule(m_szVertexShaderBinaryPath, byteCodes[0]));
			// geometry shader
			if (!byteCodes[1].empty())
				shaderModules.push_back(_CreateShaderModule(m_szGeometryShaderBinaryPath, byteCodes[1]));
			// fragment shader
			shaderModules.push_back(_CreateShaderModule(m_szFragmentShaderSourcePath, byteCodes[2]));

			std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos = std::move(_CreateShaderStageCreateInfos());
			
		}
	}
}