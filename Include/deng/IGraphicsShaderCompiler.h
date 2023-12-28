// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: IGraphicsShaderCompiler.h - graphics shader compiler class interface
// author: Karl-Mihkel Ott

/* Shader compilation is abstracted away to support different shader types such as HLSL and GLSL for various backends */
#pragma once

#include "deng/Api.h"
#include <vector>
#include <string>
#include <string_view>
#include <cstring>
#include <iterator>

#define DECLARE_GRAPHICS_SHADER_COMPILER(sourceExt, byteCodeExt, byteCodeDir) \
		static constexpr const char* GetSourceCodeExt() { return sourceExt; } \
		static constexpr const char* GetByteCodeExt() { return byteCodeExt; } \
		static constexpr const char* GetByteCodeDirectoryName() { return byteCodeDir;  }

namespace DENG
{

	class DENG_API IGraphicsShaderCompiler
	{
		public:
			IGraphicsShaderCompiler() = default;

			// Shader file compilation methods
			virtual std::vector<uint32_t> CompileVertexShaderFile(const char* _szFileName) const = 0;
			virtual std::vector<uint32_t> CompileGeometryShaderFile(const char* _szFileName) const = 0;
			virtual std::vector<uint32_t> CompileFragmentShaderFile(const char* _szFileName) const = 0;

			// Shader source compilation methods
			// Using const char* as string type
			inline std::vector<uint32_t> CompileVertexShader(const char* _szSource) const
			{
				return CompileVertexShader(_szSource, std::strlen(_szSource));
			}

			inline std::vector<uint32_t> CompileGeometryShader(const char* _szSource) const
			{
				return CompileGeometryShader(_szSource, std::strlen(_szSource));
			}

			inline std::vector<uint32_t> CompileFragmentShader(const char* _szSource) const
			{
				return CompileFragmentShader(_szSource, std::strlen(_szSource));
			}

			virtual std::vector<uint32_t> CompileVertexShader(const char* _szSource, size_t _uLen) const = 0;
			virtual std::vector<uint32_t> CompileGeometryShader(const char* _szSource, size_t _uLen) const = 0;
			virtual std::vector<uint32_t> CompileFragmentShader(const char* _szSource, size_t _uLen) const = 0;

			// Using string iterators
			template<class InputIterator>
			std::vector<uint32_t> CompileVertexShader(InputIterator _itBegin, InputIterator _itEnd) const
			{
				typedef typename std::iterator_traits<InputIterator>::iterator_category category;
				if constexpr (std::is_same_v<category, std::random_access_iterator_tag>)
				{
					return std::move(CompileVertexShader(&(*_itBegin), _itEnd - _itBegin));
				}
				else
				{
					std::string _sSource;
					for (auto it = _itBegin; it != _itEnd; it++)
						_sSource += *it;
					return std::move(CompileVertexShader(_sSource.c_str()));
				}
			}
	};
}