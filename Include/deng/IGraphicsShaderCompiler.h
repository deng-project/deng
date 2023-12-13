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

namespace DENG
{

	class DENG_API IGraphicsShaderCompiler
	{
		protected:
			const std::string_view m_svSourceType;
			const std::string_view m_svByteCodeType;

			const std::string_view m_svVertexShaderSourcePath = "Shaders/Source/Vertex";
			const std::string_view m_svGeometryShaderSourcePath = "Shaders/Source/Geometry";
			const std::string_view m_svFragmentShaderSourcePath = "Shaders/Source/Fragment";

			const std::string_view m_svVertexShaderBinaryPath = "Shaders/Binary/Vertex";
			const std::string_view m_svGeometryShaderBinaryPath = "Shaders/Binary/Geometry";
			const std::string_view m_svFragmentShaderBinaryPath = "Shaders/Binary/Fragment";

		public:
			IGraphicsShaderCompiler(const std::string_view& _svSourceType, const std::string_view& _svByteCodeType) :
				m_svSourceType(_svSourceType),
				m_svByteCodeType(_svByteCodeType)
			{
			}

			// Return the shader source type
			// for instance HLSL shaders would be "hlsl" and GLSL shaders would be "glsl"
			inline const std::string_view& GetSourceType()
			{
				return m_svSourceType;
			}

			inline const std::string_view& GetByteCodeType()
			{
				return m_svByteCodeType;
			}

			// Shader file compilation methods
			virtual std::vector<uint32_t> CompileVertexShaderFile(const std::string& _sFileName) const = 0;
			virtual std::vector<uint32_t> CompileGeometryShaderFile(const std::string& _sFileName) const = 0;
			virtual std::vector<uint32_t> CompileFragmentShaderFile(const std::string& _sFileName) const = 0;

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

			// Using std::string as string type
			inline std::vector<uint32_t> CompileVertexShader(const std::string& _sSource) const
			{
				return CompileVertexShader(_sSource.c_str(), _sSource.size());
			}

			inline std::vector<uint32_t> CompileGeometryShader(const std::string& _sSource) const
			{
				return CompileGeometryShader(_sSource.c_str(), _sSource.size());
			}

			inline std::vector<uint32_t> CompileFragmentShader(const std::string& _sSource) const
			{
				return CompileFragmentShader(_sSource.c_str(), _sSource.size());
			}

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
					return std::move(CompileVertexShader(_sSource));
				}
			}
	};
}