// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SpirvByteCodeToolset.h - ByteCodeToolset implementation class for Spirv bytecode format header
// author: Karl-Mihkel Ott

#pragma once

#include "deng/ErrorDefinitions.h"
#include "deng/Exceptions.h"
#include "deng/IByteCodeToolset.h"
#include <deng/spirv_reflect.h>

namespace DENG
{
	class DENG_API SpirvByteCodeToolset : public IByteCodeToolset
	{
		private:
			SpvReflectShaderModule m_vertexModule = {};
			SpvReflectShaderModule m_geometryModule = {};
			SpvReflectShaderModule m_fragmentModule = {};

		private:
			inline void _EvaluateShaderModuleCreation(SpvReflectResult _res, int _iShaderStage)
			{
				if (_res != SPV_REFLECT_RESULT_SUCCESS)
				{
					switch (_iShaderStage)
					{
						case 1:
							throw ShaderException("SPIR-V Reflect error: failed to create spvReflectShaderModule object for vertex shader");
							break;

						case 2:
							throw ShaderException("SPIR-V Reflect error: failed to create spvReflectShaderModule object for geometry shader");
							break;

						case 3:
							throw ShaderException("SPIR-V Reflect error: failed to create spvReflectShaderModule object for fragment shader");
							break;

						default:
							DENG_ASSERT(false);
							break;
					}
				}
			}

			inline void _EvaluateInputVariableEnumeration(SpvReflectResult _res, int _iShaderStage)
			{
				if (_res != SPV_REFLECT_RESULT_SUCCESS)
				{
					switch (_iShaderStage)
					{
						case 1:
							throw ShaderException("SPIR-V Reflect error: failed to enumerate input variables for vertex shader");
							break;

						case 2:
							throw ShaderException("SPIR-V Reflect error: failed to enumerate input variables for geometry shader");
							break;

						case 3:
							throw ShaderException("SPIR-V Reflect error: failed to enumerate input variables for fragment shader");
							break;

						default:
							DENG_ASSERT(false);
							break;
					}
				}
			}

			inline void _EvaluateDescriptorBindingEnumeration(SpvReflectResult _res, int _iShaderStage)
			{
				if (_res != SPV_REFLECT_RESULT_SUCCESS)
				{
					switch (_iShaderStage)
					{
						case 1:
							throw ShaderException("SPIR-V Reflect error: failed to enumerate descriptor bindings for vertex shader");
							break;

						case 2:
							throw ShaderException("SPIR-V Reflect error: failed to enumerate descriptor bindings for geometry shader");
							break;

						case 3:
							throw ShaderException("SPIR-V Reflect error: failed to enumerate descriptor bindings for fragment shader");
							break;

						default:
							DENG_ASSERT(false);
							break;
					}
				}
			}

			void _CreateShaderModule(const std::vector<uint32_t>& _shaderWords, SpvReflectShaderModule& _shaderModule, int _iShaderType);
			void _FindVertexShaderInputVariables();
			void _FindShaderBindings(SpvReflectShaderModule& _shaderModule, int _iShaderType);

		public:
			SpirvByteCodeToolset(const std::vector<uint32_t>& _vertexBytes, const std::vector<uint32_t>& _geometryBytes, const std::vector<uint32_t>& _fragmentBytes);
	};
}