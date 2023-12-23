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
				VkDevice m_hDevice = VK_NULL_HANDLE;
				VkRenderPass m_hRenderPass = VK_NULL_HANDLE;
				VkSampleCountFlagBits m_uSampleCountFlagBits = (VkSampleCountFlagBits)0;
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
				std::vector<VkVertexInputBindingDescription> _FindInputBindingDescriptions(const std::array<std::vector<uint32_t>, 3>& _spv)
				{
					static_assert(std::is_base_of<IByteCodeToolset, ByteCodeToolset>::value);
					ByteCodeToolset toolset(_spv[0], _spv[1], _spv[2]);
					
					auto& shaderInputFormats = toolset.GetShaderInputFormats();
					auto& shaderInputStrides = toolset.GetShaderInputStrides();

					DENG_ASSERT(shaderInputFormats.size() == shaderInputStrides.size());
					uint32_t uCombinedStride = 0;
					for (uint32_t uStride : shaderInputStrides)
					{
						uCombinedStride += uStride;
					}

					std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
					vertexInputBindingDescriptions.reserve(shaderInputFormats.size());
				
					for (size_t i = 0; i < shaderInputFormats.size(); i++)
					{
						vertexInputBindingDescriptions.emplace_back();
						vertexInputBindingDescriptions.back().binding = static_cast<uint32_t>(i);
						vertexInputBindingDescriptions.back().inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

						if (this->IsFlagSet(GraphicsShaderFlag_UseSeparateAttributeStriding))
						{
							vertexInputBindingDescriptions.back().stride = shaderInputStrides[i];
						}
						else
						{
							vertexInputBindingDescriptions.back().stride = uCombinedStride;
						}
					}

					return vertexInputBindingDescriptions;
				}

				template <typename ByteCodeToolset>
				std::vector<VkVertexInputAttributeDescription> _FindInputAttributeDescriptions(const std::array<std::vector<uint32_t>, 3>& _spv)
				{
					static_assert(std::is_base_of<IByteCodeToolset, ByteCodeToolset>::value);
					ByteCodeToolset toolset(_spv[0], _spv[1], _spv[2]);

					auto& shaderInputFormats = toolset.GetShaderInputFormats();
					std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions = {};
					vertexInputAttributeDescriptions.reserve(shaderInputFormats.size());

					for (size_t i = 0; i < shaderInputFormats.size(); i++)
					{
						vertexInputAttributeDescriptions.emplace_back();
						vertexInputAttributeDescriptions.back().binding = static_cast<uint32_t>(i);
						vertexInputAttributeDescriptions.back().location = static_cast<uint32_t>(i);
						vertexInputAttributeDescriptions.back().format = static_cast<VkFormat>(shaderInputFormats[i]);
					}

					return vertexInputAttributeDescriptions;
				}
				
				template <typename ByteCodeToolset>
				void _CreatePipelineLayout(const std::array<std::vector<uint32_t>, 3>& _spv)
				{
					static_assert(std::is_base_of<IByteCodeToolset, ByteCodeToolset>::value);
					ByteCodeToolset toolset(_spv[0], _spv[1], _spv[2]);

					// create descriptor set layout
					std::vector<VkDescriptorSetLayoutBinding> bindings;
					bindings.reserve(toolset.GetBoundResources().size());
					auto& boundResources = toolset.GetBoundResources();

					for (size_t i = 0; i < boundResources.size(); i++)
					{
						if (boundResources[i].type == BoundResourceType::None)
							break;

						bindings.emplace_back();
						switch (boundResources[i].type)
						{
							case BoundResourceType::Buffer:
								bindings.back().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
								break;

							case BoundResourceType::StorageBuffer:
								bindings.back().descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
								break;

							case BoundResourceType::ImageSampler:
								bindings.back().descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
								break;

							default:
								DENG_ASSERT(false);
								break;
						}

						bindings.back().binding = static_cast<uint32_t>(i);
						bindings.back().descriptorCount = 1;

						if (boundResources[i].uShaderStage & ShaderStageBit_Vertex)
							bindings.back().stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
						if (boundResources[i].uShaderStage & ShaderStageBit_Geometry)
							bindings.back().stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
						if (boundResources[i].uShaderStage & ShaderStageBit_Fragment)
							bindings.back().stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
					}

					VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
					descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
					descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
					descriptorSetLayoutCreateInfo.pBindings = bindings.data();
					if (vkCreateDescriptorSetLayout(m_hDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_hDescriptorSetLayout) != VK_SUCCESS)
						throw RendererException("Failed to create a descriptor set layout for pipeline " + std::string(_CreateUUID()));
				
					// create pipeline layout
					VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
					pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
					pipelineLayoutCreateInfo.setLayoutCount = 1;
					pipelineLayoutCreateInfo.pSetLayouts = &m_hDescriptorSetLayout;

					if (vkCreatePipelineLayout(m_hDevice, &pipelineLayoutCreateInfo, nullptr, &m_hPipelineLayout) != VK_SUCCESS)
						throw RendererException("Failed to create a pipeline layout (vkCreatePipelineLayout)");
				}

				VkShaderModule _CreateShaderModule(const char* _szBinaryPath, const std::vector<uint32_t>& _spv);
				std::vector<VkPipelineShaderStageCreateInfo> _CreateShaderStageCreateInfos(const std::vector<VkShaderModule>& _shaderModules);

			public:
				GraphicsPipeline(
					VkDevice _hDevice,
					VkRenderPass _hRenderPass,
					VkSampleCountFlagBits _uSampleCountFlagBits,
					uint32_t _uDeviceId,
					uint32_t _uVendorId,
					const char* _szVertexShader, 
					const char* _szFragmentShader, 
					const char* _szGeometryShader = "") :
					IGraphicsPipeline(_szVertexShader, _szFragmentShader, _szGeometryShader),
					m_hDevice(_hDevice),
					m_hRenderPass(_hRenderPass),
					m_uSampleCountFlagBits(_uSampleCountFlagBits),
					m_uDeviceId(_uDeviceId),
					m_uVendorId(_uVendorId)
				{ 
				}

				virtual void CreatePipeline() override;

				inline VkPipeline GetPipeline()
				{
					return m_hPipeline;
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
