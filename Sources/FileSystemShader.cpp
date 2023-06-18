// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: UberShader.cpp - shader requrest classes implementation
// author: Karl-Mihkel Ott

#define FILE_SYSTEM_SHADER_CPP
#include "deng/FileSystemShader.h"

using namespace std;
namespace fs = filesystem;


namespace DENG {

	FileSystemShader::FileSystemShader(const string& _sSourceIdentifier, const string& _sBinaryIdentifier) :
		m_csSourceIdentifier(_sSourceIdentifier),
		m_sBinaryIdentifier(_sBinaryIdentifier)
	{
		if (m_sBinaryIdentifier.empty())
			m_sBinaryIdentifier = m_csSourceIdentifier;
	}


	vector<uint32_t> FileSystemShader::_CompileVertexShaderToSpirv() const {
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		for (auto it = m_vertexShaderMacros.begin(); it != m_vertexShaderMacros.end(); it++) {
			if (it->second != "")
				options.AddMacroDefinition(it->first, it->second);
			else options.AddMacroDefinition(it->first);
		}

		const string csVertexShaderSourcePath =
			m_csShaderSourcePath + '\\' + m_csSourceIdentifier + '\\' + m_csSourceIdentifier + ".vert";
		
		vector<char> sSourceCode = m_programFilesManager.GetProgramFileContent(csVertexShaderSourcePath);
		shaderc::CompilationResult module = compiler.CompileGlslToSpv(
			sSourceCode.data(), 
			sSourceCode.size(), 
			shaderc_vertex_shader, 
			fs::path(csVertexShaderSourcePath).filename().u8string().c_str(),
			options);

		if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
			throw ShaderException(module.GetErrorMessage());
			return {};
		}

		vector<uint32_t> spirv(module.cbegin(), module.cend());
	
		try {
			m_programFilesManager.WriteProgramFile(
				(const char*)spirv.data(),
				spirv.size() * sizeof(uint32_t),
				m_csSpirvBinaryPath + '\\' + m_sBinaryIdentifier + '\\' + m_sBinaryIdentifier + ".vert.spv");
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), ErrorSeverity::WARNING);
		}

		return spirv;
	}


	vector<uint32_t> FileSystemShader::_CompileGeometryShaderToSpirv() const {
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		for (auto it = m_geometryShaderMacros.begin(); it != m_geometryShaderMacros.end(); it++) {
			if (it->second != "")
				options.AddMacroDefinition(it->first, it->second);
			else options.AddMacroDefinition(it->first);
		}

		const string csGeometryShaderSourcePath =
			m_csShaderSourcePath + '\\' + m_csSourceIdentifier + '\\' + m_csSourceIdentifier + ".geom";

		vector<char> sSourceCode = m_programFilesManager.GetProgramFileContent(csGeometryShaderSourcePath);
		shaderc::CompilationResult module = compiler.CompileGlslToSpv(
			sSourceCode.data(),
			sSourceCode.size(),
			shaderc_geometry_shader,
			fs::path(csGeometryShaderSourcePath).filename().u8string().c_str(),
			options);

		if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
			throw ShaderException(module.GetErrorMessage());
			return {};
		}

		vector<uint32_t> spirv(module.cbegin(), module.cend());

		try {
			m_programFilesManager.WriteProgramFile(
				(const char*)spirv.data(),
				spirv.size() * sizeof(uint32_t),
				m_csSpirvBinaryPath + '\\' + m_sBinaryIdentifier + '\\' + m_sBinaryIdentifier + ".geom.spv");
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), NON_CRITICAL);
		}

		return spirv;
	}


	vector<uint32_t> FileSystemShader::_CompileFragmentShaderToSpirv() const {
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		for (auto it = m_fragmentShaderMacros.begin(); it != m_fragmentShaderMacros.end(); it++) {
			if (it->second != "")
				options.AddMacroDefinition(it->first, it->second);
			else options.AddMacroDefinition(it->first);
		}

		const string csFragmentShaderSourcePath =
			m_csShaderSourcePath + '\\' + m_csSourceIdentifier + '\\' + m_csSourceIdentifier + ".frag";
		vector<char> sSourceCode = m_programFilesManager.GetProgramFileContent(csFragmentShaderSourcePath);
		shaderc::CompilationResult module = compiler.CompileGlslToSpv(
			sSourceCode.data(),
			sSourceCode.size(),
			shaderc_fragment_shader,
			fs::path(csFragmentShaderSourcePath).filename().u8string().c_str(),
			options);

		if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
			throw ShaderException(module.GetErrorMessage());
			return {};
		}

		vector<uint32_t> spirv(module.cbegin(), module.cend());

		try {
			m_programFilesManager.WriteProgramFile(
				(const char*)spirv.data(),
				spirv.size() * sizeof(uint32_t),
				m_csSpirvBinaryPath + '\\' + m_sBinaryIdentifier + '\\' + m_sBinaryIdentifier + ".frag.spv");
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), NON_CRITICAL);
		}

		return spirv;
	}


	vector<uint32_t> FileSystemShader::GetVertexShaderSpirv() const {
		const string csVertexShaderSpirvFilePath = 
			m_csSpirvBinaryPath + '\\' + m_sBinaryIdentifier + '\\' + m_sBinaryIdentifier + ".vert.spv";
		const string csVertexShaderSourceFilePath = 
			m_csShaderSourcePath + '\\' + m_csSourceIdentifier + '\\' + m_csSourceIdentifier + ".vert";
		
		time_t tSpvWriteTimestamp = 0, tSourceWriteTimestamp = 0;
		tSourceWriteTimestamp = m_programFilesManager.GetFileTimestamp(csVertexShaderSourceFilePath);

		try {
			tSpvWriteTimestamp = m_programFilesManager.GetFileTimestamp(csVertexShaderSpirvFilePath);
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), ErrorSeverity::WARNING);
		}

		if (tSpvWriteTimestamp > tSourceWriteTimestamp) {
			auto input = m_programFilesManager.GetProgramFileContent(csVertexShaderSpirvFilePath);
			vector<uint32_t> output(input.size() / sizeof(uint32_t));
			for (size_t i = 0; i < output.size(); i++)
				output[i] = reinterpret_cast<uint32_t*>(input.data())[i];
			
			return output;
		}
		else {
			return _CompileVertexShaderToSpirv();
		}
	}
	
	
	vector<uint32_t> FileSystemShader::GetGeometryShaderSpirv() const {
		const string csGeometryShaderSpirvFilePath =
			m_csSpirvBinaryPath + '\\' + m_sBinaryIdentifier + '\\' + m_sBinaryIdentifier + ".geom.spv";
		const string csGeometryShaderSourceFilePath =
			m_csShaderSourcePath + '\\' + m_csSourceIdentifier + '\\' + m_csSourceIdentifier + ".geom";

		time_t tSpvWriteTimestamp = 0, tSourceWriteTimestamp = 0;
		tSourceWriteTimestamp = m_programFilesManager.GetFileTimestamp(csGeometryShaderSourceFilePath);

		try {
			tSpvWriteTimestamp = m_programFilesManager.GetFileTimestamp(csGeometryShaderSpirvFilePath);
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), WARNING);
		}
	
		if (tSpvWriteTimestamp > tSourceWriteTimestamp) {
			auto input = m_programFilesManager.GetProgramFileContent(csGeometryShaderSpirvFilePath);
			vector<uint32_t> output(input.size() / sizeof(uint32_t));
			for (size_t i = 0; i < output.size(); i++)
				output[i] = reinterpret_cast<uint32_t*>(input.data())[i];
	
			return output;
		}
		else {
			return _CompileGeometryShaderToSpirv();
		}
	}
	
	
	vector<uint32_t> FileSystemShader::GetFragmentShaderSpirv() const {
		const string csFragmentShaderSpirvFilePath =
			m_csSpirvBinaryPath + '\\' + m_sBinaryIdentifier + '\\' + m_sBinaryIdentifier + ".frag.spv";
		const string csFragmentShaderSourceFilePath =
			m_csShaderSourcePath + '\\' + m_csSourceIdentifier + '\\' + m_csSourceIdentifier + ".frag";
	
		time_t tSpvWriteTimestamp = 0, tSourceWriteTimestamp = 0;
		tSourceWriteTimestamp = m_programFilesManager.GetFileTimestamp(csFragmentShaderSourceFilePath);

		try {
			tSpvWriteTimestamp = m_programFilesManager.GetFileTimestamp(csFragmentShaderSpirvFilePath);
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), WARNING);
		}

		if (tSpvWriteTimestamp > tSourceWriteTimestamp) {
			auto input = m_programFilesManager.GetProgramFileContent(csFragmentShaderSpirvFilePath);
			vector<uint32_t> output(input.size() / sizeof(uint32_t));
			for (size_t i = 0; i < output.size(); i++)
				output[i] = reinterpret_cast<uint32_t*>(input.data())[i];
	
			return output;
		}
		else {
			return _CompileFragmentShaderToSpirv();
		}
	}


	vector<char> FileSystemShader::GetPipelineCache(RendererType _eRendererType) const {
		string sPipelineCacheFilePath = m_csPipelineCachePath;

		switch (_eRendererType) {
			case RendererType::OpenGL:
				sPipelineCacheFilePath += "\\OpenGL";
				break;

			case RendererType::Vulkan:
				sPipelineCacheFilePath += "\\Vulkan";
				break;

			case RendererType::DirectX:
				sPipelineCacheFilePath += "\\DirectX";
				break;

			default:
				return {};
		}

		sPipelineCacheFilePath += '\\' + m_sBinaryIdentifier + ".cache";

		if (!m_programFilesManager.ExistsFile(sPipelineCacheFilePath)) {
			stringstream ss;
			ss << "Pipeline cache for shader module '" << m_sBinaryIdentifier << "' does not exist";
			throw ShaderException(ss.str());
			return {};
		}

		return m_programFilesManager.GetProgramFileContent(sPipelineCacheFilePath);
	}


	void FileSystemShader::CachePipeline(RendererType _eRendererType, const void* _pData, size_t _uLength) const {
		DENG_ASSERT(_eRendererType == RendererType::Vulkan || _eRendererType == RendererType::OpenGL || _eRendererType == RendererType::DirectX);
		string sPipelineCacheFilePath = m_csPipelineCachePath;

		switch (_eRendererType) {
			case RendererType::Vulkan:
				sPipelineCacheFilePath += "\\Vulkan";
				break;

			case RendererType::OpenGL:
				sPipelineCacheFilePath += "\\OpenGL";
				break;

			case RendererType::DirectX:
				sPipelineCacheFilePath += "\\DirectX";
				break;
		}

		sPipelineCacheFilePath += "\\" + m_sBinaryIdentifier + ".cache";

		try {
			m_programFilesManager.WriteProgramFile((const char*)_pData, _uLength, sPipelineCacheFilePath);
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), NON_CRITICAL);
		}
	}


	PipelineCacheStatusBits FileSystemShader::GetPipelineCacheStatus() const {
		PipelineCacheStatusBits uMask = 0;
		// check if pipeline cache exists
		const string sDirectXPipelineCacheFilePath = m_csPipelineCachePath + "\\DirectX\\" + m_sBinaryIdentifier + ".cache";
		const string sOpenGLPipelineCacheFilePath = m_csPipelineCachePath + "\\OpenGL\\" + m_sBinaryIdentifier + ".cache";
		const string sVulkanPipelineCacheFilePath = m_csPipelineCachePath + "\\Vulkan\\" + m_sBinaryIdentifier + ".cache";
		
		if (m_programFilesManager.ExistsFile(sOpenGLPipelineCacheFilePath))
			uMask |= PipelineCacheStatusBit_OpenGLCache;
		else if (m_programFilesManager.ExistsFile(sVulkanPipelineCacheFilePath))
			uMask |= PipelineCacheStatusBit_VulkanCache;
		else if (m_programFilesManager.ExistsFile(sDirectXPipelineCacheFilePath))
			uMask |= PipelineCacheStatusBit_DirectXCache;

		if (uMask) return uMask;
		
		// check if spirv exists
		const string sVertexShaderSpirvPath = 
			m_csSpirvBinaryPath + '\\' + m_sBinaryIdentifier + '\\' + m_sBinaryIdentifier + ".vert.spv";
		const string sGeometryShaderSourcePath =
			m_csShaderSourcePath + '\\' + m_csSourceIdentifier + '\\' + m_csSourceIdentifier + ".geom";
		const string sGeometryShaderSpirvPath = 
			m_csSpirvBinaryPath + '\\' + m_sBinaryIdentifier + '\\' + m_sBinaryIdentifier + ".geom.spv";
		const string sFragmentShaderSpirvPath =
			m_csSpirvBinaryPath + '\\' + m_sBinaryIdentifier + '\\' + m_sBinaryIdentifier + ".frag.spv";
		
		if (m_programFilesManager.ExistsFile(sVertexShaderSpirvPath) &&
			(!m_programFilesManager.ExistsFile(sGeometryShaderSourcePath) || m_programFilesManager.ExistsFile(sGeometryShaderSpirvPath)) &&
			m_programFilesManager.ExistsFile(sFragmentShaderSpirvPath))
		{
			return PipelineCacheStatusBit_Spirv;
		}
		else if (m_programFilesManager.ExistsFile(sVertexShaderSpirvPath) ||
			(!m_programFilesManager.ExistsFile(sGeometryShaderSourcePath) || m_programFilesManager.ExistsFile(sGeometryShaderSpirvPath)) ||
			m_programFilesManager.ExistsFile(sFragmentShaderSpirvPath))
		{
			return PipelineCacheStatusBit_PartialSpirv;
		}
		else return PipelineCacheStatusBit_NoCache;
	}
}