// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: HLSLGraphicsShaderSpirvCompiler.h - HLSL graphics shader compiler class header
// author: Karl-Mihkel Ott

#pragma once

#include <atlbase.h>
#include <directx-dxc/dxcapi.h>
#include "deng/IGraphicsShaderCompiler.h"

namespace DENG
{
	class DENG_API HLSLGraphicsShaderSpirvCompiler : public IGraphicsShaderCompiler
	{
		private:
			CComPtr<IDxcLibrary> m_library;
			CComPtr<IDxcCompiler3> m_compiler;
			CComPtr<IDxcUtils> m_utils;

		private:
			std::vector<uint32_t> _Compile(const char* _szSource, size_t _uLen, const wchar_t* _lszProfile, const wchar_t* _lszFileName = L"Unknown_File") const;
			std::wstring _UTF8_Decode(const std::string& _str) const;

		public:
			HLSLGraphicsShaderSpirvCompiler();
			DECLARE_GRAPHICS_SHADER_COMPILER("hlsl", "spv", "Spirv");

			virtual std::vector<uint32_t> CompileVertexShaderFile(const std::string& _sFileName) const override;
			virtual std::vector<uint32_t> CompileGeometryShaderFile(const std::string& _sFileName) const override;
			virtual std::vector<uint32_t> CompileFragmentShaderFile(const std::string& _sFileName) const override;

			virtual std::vector<uint32_t> CompileVertexShader(const char* _szSource, size_t _uLen) const override;
			virtual std::vector<uint32_t> CompileGeometryShader(const char* _szSource, size_t _uLen) const override;
			virtual std::vector<uint32_t> CompileFragmentShader(const char* _szSource, size_t _uLen) const override;
	};
}
