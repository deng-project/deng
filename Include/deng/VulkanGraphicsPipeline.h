// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanGraphicsPipeline.h - Vulkan graphics pipeline class header
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
				VkDevice m_hDevice;
				VkPipeline m_hPipeline = VK_NULL_HANDLE;
				VkPipelineLayout m_hPipelineLayout = VK_NULL_HANDLE;
				VkDescriptorSetLayout m_hDescriptorSetLayout = VK_NULL_HANDLE;
				VkPipelineCache m_hPipelineCache = VK_NULL_HANDLE;
				uint32_t m_uDeviceId = 0;
				uint32_t m_uVendorId = 0;

			private:
				bool _LoadPipelineCache();
				std::string _GetCacheFileName();
				void _CreateDummyPipelineCacheHandle();
				void _SerializePipelineCache();
				template <typename ByteCodeToolset>
				void _CreatePipelineLayout(const std::array<std::vector<uint32_t>, 3>& _spv)
				{
					static_assert(std::is_base_of<IByteCodeToolset, ByteCodeToolset>::value);
					ByteCodeToolset toolset(_spv[0], _spv[1], _spv[2]);

					// create descriptor set layout
					std::vector<VkDescriptorSetLayoutBinding> bindings;
					bindings.reserve(toolset.GetBoundResources().size());

					for (auto& boundResource : toolset.GetBoundResources())
					{
						bindings.emplace_back();
						switch (boundResource.type)
						{
							case BoundResourceType::Buffer:
								bindings.back().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
								break;

							case BoundResourceType::StorageBuffer:
								bindings.back().descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
								break;

							case BoundResourceType::ImageSampler2D:
							case BoundResourceType::ImageSampler3D:
								bindings.back().descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
								break;

							default:
								DENG_ASSERT(false);
								break;
						}

						bindings.back().binding = boundResource.uBinding;
						bindings.back().descriptorCount = 1;

						if (boundResource.uShaderStage & ShaderStageBit_Vertex)
							bindings.back().stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
						if (boundResource.uShaderStage & ShaderStageBit_Geometry)
							bindings.back().stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
						if (boundResource.uShaderStage & ShaderStageBit_Fragment)
							bindings.back().stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
					}

					VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
					descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
					descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
					descriptorSetLayoutCreateInfo.pBindings = bindings.data();
					if (vkCreateDescriptorSetLayout(m_hDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_hDescriptorSetLayout) != VK_SUCCESS)
						throw RendererException("Failed to create a descriptor set layout for pipeline " << _CreateUUID());
				
					// create pipeline layout
					VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
					pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
					pipelineLayoutCreateInfo.setLayoutCount = 1;
					pipelineLayoutCreateInfo.pSetLayouts = &m_hPipelineLayout;

					// check push constants
					if (toolset.HasPushConstants())
					{
						VkPushConstantRange pushConstantRange = {};
						pushConstantRange.size = toolset.GetEffectivePushConstantLength();
						if (toolset.GetPushConstantStageBits() & ShaderStageBit_Vertex)
							pushConstantRange.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
						if (toolset.GetPushConstantStageBits() & ShaderStageBit_Geometry)
							pushConstantRange.stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
						if (toolset.GetPushConstantStageBits() & ShaderStageBit_Fragment)
							pushConstantRange.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;

						pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
						pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
					}

					if (vkCreatePipelineLayout(m_hDevice, &pipelineLayoutCreateInfo, nullptr, &m_hPipelineLayout) != VK_SUCCESS)
						throw RendererException("Failed to create a pipeline layout (vkCreatePipelineLayout)");
				}

				VkShaderModule _CreateShaderModule(const char* _szBinaryPath, const std::vector<uint32_t>& _spv);
				std::vector<VkPipelineShaderStageCreateInfo> _CreateShaderStageCreateInfos(const std::vector<VkShaderModule>& _shaderModules);

			public:
				GraphicsPipeline(
					VkDevice _hDevice,
					uint32_t _uDeviceId,
					uint32_t _uVendorId,
					const char* _szVertexShader, 
					const char* _szFragmentShader, 
					const char* _szGeometryShader = "") :
					IGraphicsPipeline(_szVertexShader, _szFragmentShader, _szGeometryShader),
					m_hDevice(_hDevice),
					m_uDeviceId(_uDeviceId),
					m_uVendorId(_uVendorId)
				{ 
				}

				virtual void CreatePipeline() override;

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
