// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: IGraphicsShaderSpirvCompiler.h - shader spirv compiler class interface
// author: Karl-Mihkel Ott

/* Shader compilation is abstracted away to support different shader types such as HLSL and GLSL for various backends */
#pragma once

#include "deng/Api.h"
#include <vector>
#include <string>
#include <cstring>
#include <iterator>

namespace DENG {

	class DENG_API IGraphicsShaderSpirvCompiler {
		public:
			// Shader file compilation methods
			virtual std::vector<uint32_t>&& CompileVertexShaderFile(const std::string& _sFileName) const = 0;
			virtual std::vector<uint32_t>&& CompileGeometryShaderFile(const std::string& _sFileName) const = 0;
			virtual std::vector<uint32_t>&& CompileFragmentShaderFile(const std::string& _sFileName) const = 0;
			
			// Shader source compilation methods
			// Using const char* as string type
			inline std::vector<uint32_t>&& CompileVertexShader(const char* _szSource) const {
				return CompileVertexShader(_szSource, std::strlen(_szSource));
			}

			inline std::vector<uint32_t>&& CompileGeometryShader(const char* _szSource) const {
				return CompileGeometryShader(_szSource, std::strlen(_szSource));
			}

			inline std::vector<uint32_t>&& CompileFragmentShader(const char* _szSource) const {
				return CompileFragmentShader(_szSource, std::strlen(_szSource));
			}

			virtual std::vector<uint32_t>&& CompileVertexShader(const char* _szSource, size_t _uLen) const = 0;
			virtual std::vector<uint32_t>&& CompileGeometryShader(const char* _szSource, size_t _uLen) const = 0;
			virtual std::vector<uint32_t>&& CompileFragmentShader(const char* _szSource, size_t _uLen) const = 0;

			// Using std::string as string type
			inline std::vector<uint32_t>&& CompileVertexShader(const std::string& _sSource) const {
				return CompileVertexShader(_sSource.c_str(), _sSource.size());
			}

			inline std::vector<uint32_t>&& CompileGeometryShader(const std::string& _sSource) const {
				return CompileGeometryShader(_sSource.c_str(), _sSource.size());
			}

			inline std::vector<uint32_t>&& CompileFragmentShader(const std::string& _sSource) const {
				return CompileFragmentShader(_sSource.c_str(), _sSource.size());
			}
			
			// Using string iterators
			template<class InputIterator>
			std::vector<uint32_t>&& CompileVertexShader(InputIterator _itBegin, InputIterator _itEnd) const {
				typedef typename std::iterator_traits<InputIterator>::iterator_category category;
				if constexpr (std::is_same_v<category, std::random_access_iterator_tag>) {
					return CompileVertexShader(&(*_itBegin), _itEnd - _itBegin)
				}
				else {
					std::string _sSource;
					for (auto it = _itBegin; it != _itEnd; it++)
						_sSource += *it;
					return CompileVertexShader(_sSource);
				}
			}
	};
}