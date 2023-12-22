// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SpirvByteCodeToolset.h - ByteCodeToolset implementation class for Spirv bytecode format implementation
// author: Karl-Mihkel Ott

#include <vector>
#include <ostream>
#include <sstream>
#include "deng/ErrorDefinitions.h"
#include "deng/Exceptions.h"
#include "deng/SpirvByteCodeToolset.h"
#include "deng/spirv_reflect.h"

#define SPV_MAGIC 0x07230203
#define SPV_ENDIANNESS_CONFLICT_MAGIC 0x03022307
#define SPV_VERSION(major, minor) ((static_cast<uint32_t>(major%256) << 16) | (static_cast<uint32_t>(minor%256) << 8))
#define SPV_VERSION_MAJOR(x) ((x >> 16) % 256)
#define SPV_VERSION_MINOR(x) ((x >> 8) % 256)
#define SPV_FIRST_WORD_INDEX 5

namespace DENG
{
	SpirvByteCodeToolset::SpirvByteCodeToolset(const std::vector<uint32_t>& _vertexBytes, const std::vector<uint32_t>& _geometryBytes, const std::vector<uint32_t>& _fragmentBytes) :
		IByteCodeToolset(_vertexBytes, _geometryBytes, _fragmentBytes)
	{
		SpvReflectShaderModule module = {};
		SpvReflectResult result = spvReflectCreateShaderModule(_vertexBytes.size() * sizeof(uint32_t), _vertexBytes.data(), &module);
		if (result != SPV_REFLECT_RESULT_SUCCESS)
		{
			throw ShaderException("Failed to create spvReflectShaderModule object");
		}

		// enumerate and extract shader's input variables
		uint32_t uVariableCount = 0;
		result = spvReflectEnumerateInputVariables(&module, &uVariableCount, nullptr);
		if (result != SPV_REFLECT_RESULT_SUCCESS)
		{
			throw ShaderException("Failed to enumerate SPIR-V input variables");
		}
		std::vector<SpvReflectInterfaceVariable*> inputVariables(uVariableCount);
		result = spvReflectEnumerateInputVariables(&module, &uVariableCount, inputVariables.data());
		if (result != SPV_REFLECT_RESULT_SUCCESS)
		{
			throw ShaderException("Failed to enumerate SPIR-V input variables");
		}

		for (SpvReflectInterfaceVariable* pVar : inputVariables)
		{
			// check if the variable is input
			if (pVar->storage_class == SpvStorageClassInput)
			{
				// check if the location is the size of m_inputFormats
				if (pVar->location != m_inputFormats.size())
				{
					throw ShaderException("SPIR-V Reflect error: vertex shader input locations must be continuous");
				}

				m_inputFormats.push_back(static_cast<ShaderInputFormat>(pVar->format));
			}
		}

		spvReflectDestroyShaderModule(&module);
	}
}