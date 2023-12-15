// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanGraphicsPipeline.h - Vulkan graphics pipeline header
// author: Karl-Mihkel Ott

#pragma once

#include <vulkan/vulkan.h>
#include "deng/IGraphicsPipeline.h"

namespace DENG
{
	namespace Vulkan
	{
		class DENG_API GraphicsPipeline : public IGraphicsPipeline
		{
			private:
				VkPipeline m_hPipeline = VK_NULL_HANDLE;
				VkPipelineLayout m_hPipelineLayout = VK_NULL_HANDLE;
				VkPipelineCache m_hPipelineCache = VK_NULL_HANDLE;

			public:
				GraphicsPipeline(const char* _szVertexShader, const char* _szFragmentShader, const char* _szGeometryShader = "") :
					IGraphicsPipeline(_szVertexShader, _szFragmentShader, _szGeometryShader)
				{ 
				}

				inline void SetPipeline(VkPipeline _hPipeline)
				{
					m_hPipeline = _hPipeline;
				}

				inline VkPipeline GetPipeline()
				{
					return m_hPipeline;
				}

				inline void SetPipelineLayout(VkPipelineLayout _hPipelineLayout)
				{
					m_hPipelineLayout = _hPipelineLayout;
				}

				inline VkPipelineLayout GetPipelineLayout()
				{
					return m_hPipelineLayout;
				}

				inline VkPipelineCache GetPipelineCache()
				{
					return m_hPipelineCache;
				}
		};

	}
}
