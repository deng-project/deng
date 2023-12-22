// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: HLSLGraphicsShaderSpirvCompiler.cpp - HLSL graphics shader compiler class implementation
// author: Karl-Mihkel Ott

#include "deng/ProgramFilesManager.h"
#include "deng/HLSLGraphicsShaderSpirvCompiler.h"
#include "deng/Exceptions.h"

#include <stringapiset.h>
#include <sstream>

namespace DENG
{
	static ProgramFilesManager g_programFilesManager;

	HLSLGraphicsShaderSpirvCompiler::HLSLGraphicsShaderSpirvCompiler()
	{
		HRESULT hres = 0;

		// Initialize DXC Library
		hres = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&m_library));
		if (FAILED(hres))
		{
			throw ShaderException("Could not initialize DXC library");
		}

		// Initialize DXC Compiler
		hres = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_compiler));
		if (FAILED(hres))
		{
			throw ShaderException("Could not initialize DXC compiler");
		}

		// Initialize DXC utility
		hres = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_utils));
		if (FAILED(hres))
		{
			throw ShaderException("Could not initialize DXC utility");
		}
	}

	std::vector<uint32_t> HLSLGraphicsShaderSpirvCompiler::_Compile(const char* _szSource, size_t _uLen, const wchar_t* _lszProfile, const wchar_t* _lszFileName) const
	{
		LPCWSTR arguments[] = {
			// name of the shader file to be displayed e.g. in an error message
			_lszFileName,
			// shader main entry point
			L"-E", L"main",
			// shader target profile
			L"-T", _lszProfile,
			// optimization
			L"-O3",
			// compile to SPIRV
			L"-spirv"
		};

		DxcBuffer buffer = {};
		buffer.Encoding = DXC_CP_ACP;
		buffer.Ptr = _szSource;
		buffer.Size = _uLen;

		// Perform compilation
		CComPtr<IDxcResult> result{ nullptr };
		HRESULT hres = m_compiler->Compile(
			&buffer,
			arguments,
			sizeof(arguments) / sizeof(LPCWSTR),
			nullptr,
			IID_PPV_ARGS(&result));

		if (SUCCEEDED(hres))
		{
			result->GetStatus(&hres);
		}

		// throw ShaderException if compilation failed
		if (FAILED(hres) && (result))
		{
			CComPtr<IDxcBlobEncoding> errorBlob;
			hres = result->GetErrorBuffer(&errorBlob);
			if (SUCCEEDED(hres) && errorBlob)
			{
				std::stringstream ss;
				ss << "Shader compilation failed: " << (const char*)errorBlob->GetBufferPointer();
				throw ShaderException(ss.str());
			}
		}

		CComPtr<IDxcBlob> code;
		result->GetResult(&code);

		// put content into std::vector
		std::vector<uint32_t> spirvBlob;
		spirvBlob.reserve(code->GetBufferSize() / sizeof(uint32_t));

		for (size_t i = 0; i < code->GetBufferSize() / sizeof(uint32_t); i++)
		{
			uint32_t uSymbol = static_cast<uint32_t*>(code->GetBufferPointer())[i];
			spirvBlob.push_back(uSymbol);
		}

		return spirvBlob;
	}

	std::wstring HLSLGraphicsShaderSpirvCompiler::_UTF8_Decode(const std::string& _str) const
	{
		if (_str.empty()) return std::wstring();
		int iSizeRequired = MultiByteToWideChar(CP_UTF8, 0, _str.c_str(), (int)_str.size(), nullptr, 0);
		std::wstring wstr(iSizeRequired, 0);
		MultiByteToWideChar(CP_UTF8, 0, _str.c_str(), (int)_str.size(), wstr.data(), (int)wstr.size());
		return wstr;
	}


	std::vector<uint32_t> HLSLGraphicsShaderSpirvCompiler::CompileVertexShaderFile(const std::string& _sFileName) const
	{
		auto vertexSource = g_programFilesManager.GetProgramFileContent(_sFileName);
		
		if (vertexSource.empty())
		{
			throw IOException("Could not find vertex shader source for module " + _sFileName);
		}

		auto wsFileName = std::move(_UTF8_Decode(_sFileName));
		return _Compile(vertexSource.data(), vertexSource.size(), L"vs_6_1", wsFileName.c_str());
	}

	std::vector<uint32_t> HLSLGraphicsShaderSpirvCompiler::CompileGeometryShaderFile(const std::string& _sFileName) const
	{
		auto geomSource = g_programFilesManager.GetProgramFileContent(_sFileName);

		if (geomSource.empty())
		{
			throw IOException("Could not find geometry shader source for module " + _sFileName);
		}

		auto wsFileName = std::move(_UTF8_Decode(_sFileName));
		return _Compile(geomSource.data(), geomSource.size(), L"gs_6_1", wsFileName.c_str());
	}

	std::vector<uint32_t> HLSLGraphicsShaderSpirvCompiler::CompileFragmentShaderFile(const std::string& _sFileName) const
	{
		auto fragSource = g_programFilesManager.GetProgramFileContent(_sFileName);

		if (fragSource.empty())
		{
			throw IOException("Could not find fragment shader source for module " + _sFileName);
		}

		auto wsFileName = std::move(_UTF8_Decode(_sFileName));
		return _Compile(fragSource.data(), fragSource.size(), L"ps_6_1", wsFileName.c_str());
	}

	std::vector<uint32_t> HLSLGraphicsShaderSpirvCompiler::CompileVertexShader(const char* _szSource, size_t _uLen) const
	{
		return _Compile(_szSource, _uLen, L"vs_6_1", L"Unknown_VS_Source");
	}

	std::vector<uint32_t> HLSLGraphicsShaderSpirvCompiler::CompileGeometryShader(const char* _szSource, size_t _uLen) const
	{
		return _Compile(_szSource, _uLen, L"gs_6_1", L"Unknown_GS_Source");
	}

	std::vector<uint32_t> HLSLGraphicsShaderSpirvCompiler::CompileFragmentShader(const char* _szSource, size_t _uLen) const
	{
		return _Compile(_szSource, _uLen, L"ps_6_1", L"Unknown_FS_Source");
	}
}