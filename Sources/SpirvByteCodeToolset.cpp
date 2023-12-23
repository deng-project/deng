// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SpirvByteCodeToolset.h - ByteCodeToolset implementation class for Spirv bytecode format implementation
// author: Karl-Mihkel Ott

#include <vector>
#include <ostream>
#include <sstream>
#include "deng/SpirvByteCodeToolset.h"
#include "deng/spirv_reflect.h"

namespace DENG
{
	SpirvByteCodeToolset::SpirvByteCodeToolset(const std::vector<uint32_t>& _vertexWords, const std::vector<uint32_t>& _geometryWords, const std::vector<uint32_t>& _fragmentWords) :
		IByteCodeToolset(_vertexWords, _geometryWords, _fragmentWords)
	{
		// create vertex and fragment reflect shader modules
		_CreateShaderModule(_vertexWords, m_vertexModule, 1);
		_CreateShaderModule(_fragmentWords, m_fragmentModule, 3);

		// enumerate and extract vertex shader's input variables
		_FindVertexShaderInputVariables();

		// enumerate descriptor binding
		_FindShaderBindings(m_vertexModule, 1);
		_FindShaderBindings(m_fragmentModule, 3);

		if (_geometryWords.size())
		{
			_CreateShaderModule(_geometryWords, m_geometryModule, 2);
			_FindShaderBindings(m_geometryModule, 2);
			spvReflectDestroyShaderModule(&m_geometryModule);
		}

		spvReflectDestroyShaderModule(&m_vertexModule);
		spvReflectDestroyShaderModule(&m_fragmentModule);

		_CalculateInputStrides();
	}

	
	void SpirvByteCodeToolset::_CreateShaderModule(const std::vector<uint32_t>& _shaderWords, SpvReflectShaderModule& _shaderModule, int _iShaderType)
	{
		SpvReflectResult result = spvReflectCreateShaderModule(_shaderWords.size() * sizeof(uint32_t), _shaderWords.data(), &_shaderModule);
		_EvaluateShaderModuleCreation(result, _iShaderType);
	}


	void SpirvByteCodeToolset::_FindVertexShaderInputVariables()
	{
		uint32_t uVariableCount = 0;
		SpvReflectResult result = spvReflectEnumerateInputVariables(&m_vertexModule, &uVariableCount, nullptr);
		_EvaluateInputVariableEnumeration(result, 1);

		std::vector<SpvReflectInterfaceVariable*> inputVariables(uVariableCount);
		result = spvReflectEnumerateInputVariables(&m_vertexModule, &uVariableCount, inputVariables.data());
		_EvaluateInputVariableEnumeration(result, 1);

		for (SpvReflectInterfaceVariable* pVar : inputVariables)
		{
			// check if the variable is input
			if (pVar->storage_class == SpvStorageClassInput)
			{
				if (pVar->location != m_inputFormats.size())
				{
					throw ShaderException("SPIR-V Reflect error: vertex shader input locations are not continuous");
				}

				m_inputFormats.push_back(static_cast<ShaderInputFormat>(pVar->format));
			}
		}
	}


	void SpirvByteCodeToolset::_FindShaderBindings(SpvReflectShaderModule& _shaderModule, int _iShaderType)
	{
		uint32_t uVariableCount = 0;
		SpvReflectResult result = spvReflectEnumerateDescriptorBindings(&_shaderModule, &uVariableCount, nullptr);
		_EvaluateDescriptorBindingEnumeration(result, 1);

		ShaderStageBits uShaderStage = ShaderStageBit_None;
		switch (_iShaderType)
		{
			case 1:
				uShaderStage |= ShaderStageBit_Vertex;
				break;

			case 2:
				uShaderStage |= ShaderStageBit_Geometry;
				break;

			case 3:
				uShaderStage |= ShaderStageBit_Fragment;
				break;

			default:
				DENG_ASSERT(false);
				break;
		}

		std::vector<SpvReflectDescriptorBinding*> vertexDescriptorBindings(uVariableCount);
		result = spvReflectEnumerateDescriptorBindings(&_shaderModule, &uVariableCount, vertexDescriptorBindings.data());

		for (SpvReflectDescriptorBinding* pBinding : vertexDescriptorBindings)
		{
			if (pBinding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
				pBinding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER)
			{
				BoundResourceType eResourceType = BoundResourceType::None;
				if (pBinding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
					eResourceType = BoundResourceType::Buffer;
				else
					eResourceType = BoundResourceType::StorageBuffer;

				if (pBinding->binding >= m_boundResources.size())
				{
					m_boundResources.resize(pBinding->binding + 1);
				}
				else if (m_boundResources[pBinding->binding].type != eResourceType)
				{
					// throw an exception
					throw ShaderException("SPIR-V Reflect error: matching descriptor bindings have different resource types");
				}
				else if (m_boundResources[pBinding->binding].uSize != pBinding->block.padded_size)
				{
					throw ShaderException("SPIR-V Reflect error: matching descriptor bindings have different padded sizes");
				}

				m_boundResources[pBinding->binding].type = BoundResourceType::Buffer;
				m_boundResources[pBinding->binding].uShaderStage |= ShaderStageBit_Vertex;
				m_boundResources[pBinding->binding].uSize = pBinding->block.padded_size;
			}
			else if (pBinding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
					 pBinding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
			{
				if (pBinding->binding >= m_boundResources.size())
				{
					m_boundResources.resize(pBinding->binding + 1);
				}
				else if (m_boundResources[pBinding->binding].type != BoundResourceType::ImageSampler)
				{
					throw ShaderException("SPIR-V Reflect error: matching descriptor bindings have different resource types");
				}

				m_boundResources[pBinding->binding].type = BoundResourceType::ImageSampler;
				m_boundResources[pBinding->binding].uShaderStage |= uShaderStage;
			}
		}
	}
}