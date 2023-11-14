// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: UberShader.cpp - shader requrest classes implementation
// author: Karl-Mihkel Ott

#define FILE_SYSTEM_SHADER_CPP
#include "deng/FileSystemShader.h"

using namespace std;
namespace fs = filesystem;


namespace DENG {

	FileSystemShader::FileSystemShader(
		const string& _sVertexShaderSourceName,
		const string& _sGeometryShaderSourceName,
		const string& _sFragmentShaderSourceName,
		const string& _sVertexShaderSpirvName,
		const string& _sGeometryShaderSpirvName,
		const string& _sFragmentShaderSpirvName
	) :
		m_csVertexShaderSourceName(_sVertexShaderSourceName),
		m_csGeometryShaderSourceName(_sGeometryShaderSourceName),
		m_csFragmentShaderSourceName(_sFragmentShaderSourceName),
		m_csVertexShaderSpirvName((_sVertexShaderSpirvName == "" ? _sVertexShaderSourceName : _sVertexShaderSpirvName)),
		m_csGeometryShaderSpirvName((_sGeometryShaderSpirvName == "" ? _sGeometryShaderSourceName : _sGeometryShaderSpirvName)),
		m_csFragmentShaderSpirvName((_sFragmentShaderSpirvName == "" ? _sFragmentShaderSourceName : _sFragmentShaderSpirvName))
	{
		const string sVertexShaderModuleCrc32Src = m_csVertexShaderSourceName + '|' + m_csVertexShaderSpirvName;
		const string sGeometryShaderModuleCrc32Src = m_csGeometryShaderSourceName + '|' + m_csGeometryShaderSpirvName;
		const string sFragmentShaderModuleCrc32Src = m_csFragmentShaderSourceName + '|' + m_csFragmentShaderSpirvName;
	
		const uint32_t uVertexModuleHash = cvar::RuntimeCrc32(sVertexShaderModuleCrc32Src);
		const uint32_t uGeometryModuleHash = cvar::RuntimeCrc32(sGeometryShaderModuleCrc32Src);
		const uint32_t uFragmentModuleHash = cvar::RuntimeCrc32(sFragmentShaderModuleCrc32Src);

		m_bProperties |= (bitset<256>{static_cast<uint64_t>(uVertexModuleHash)} << VertexShaderNameCrc32PropertyOffset);
		m_bProperties |= (bitset<256>{static_cast<uint64_t>(uGeometryModuleHash)} << GeometryShaderNameCrc32PropertyOffset);
		m_bProperties |= (bitset<256>{static_cast<uint64_t>(uFragmentModuleHash)} << FragmentShaderNameCrc32PropertyOffset);
	}


	vector<uint32_t>&& FileSystemShader::_CompileVertexShaderToSpirv() const {
		return move(vector<uint32_t>());
	}


	vector<uint32_t>&& FileSystemShader::_CompileGeometryShaderToSpirv() const {
		return move(vector<uint32_t>());
	}


	vector<uint32_t>&& FileSystemShader::_CompileFragmentShaderToSpirv() const {
		return move(vector<uint32_t>());
	}


	vector<uint32_t>&& FileSystemShader::GetVertexShaderSpirv() const {
		const string csVertexShaderSpirvFilePath = 
			string{ m_csVertexShaderSpirvPath } + '/' + m_csVertexShaderSpirvName + ".vert.spv";
		const string csVertexShaderSourceFilePath = 
			string{ m_csVertexShaderSourcePath } + '/' + m_csVertexShaderSourceName + ".vert";
		
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
			
			return move(output);
		}
		else {
			return _CompileVertexShaderToSpirv();
		}
	}
	
	
	vector<uint32_t>&& FileSystemShader::GetGeometryShaderSpirv() const {
		const string csGeometryShaderSpirvFilePath =
			string{ m_csGeometryShaderSpirvPath } + '/' + m_csGeometryShaderSpirvName + ".geom.spv";
		const string csGeometryShaderSourceFilePath =
			string{ m_csGeometryShaderSourcePath } + '/' + m_csGeometryShaderSourceName + ".geom";

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
	
			return move(output);
		}
		else {
			return _CompileGeometryShaderToSpirv();
		}
	}
	
	
	vector<uint32_t>&& FileSystemShader::GetFragmentShaderSpirv() const {
		const string csFragmentShaderSpirvFilePath =
			string{ m_csFragmentShaderSpirvPath } + '/' + m_csFragmentShaderSourceName + ".frag.spv";
		const string csFragmentShaderSourceFilePath =
			string{ m_csFragmentShaderSourcePath } + '/' + m_csFragmentShaderSourceName + ".frag";
	
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
	
			return move(output);
		}
		else {
			return _CompileFragmentShaderToSpirv();
		}
	}


	vector<char>&& FileSystemShader::GetPipelineCache(RendererType _eRendererType) const {
		string sPipelineCacheFilePath = m_csPipelineCachePath;

		switch (_eRendererType) {
			case RendererType::OpenGL:
				sPipelineCacheFilePath += "/OpenGL";
				break;

			case RendererType::Vulkan:
				sPipelineCacheFilePath += "/Vulkan";
				break;

			case RendererType::DirectX:
				sPipelineCacheFilePath += "/DirectX";
				break;

			default:
				return move(vector<char>());
		}

		sPipelineCacheFilePath += '/' + m_bProperties.to_string() + ".cache";

		if (!m_programFilesManager.ExistsFile(sPipelineCacheFilePath)) {
			stringstream ss;
			ss << "Pipeline cache for shader module '" << _Props2HexString() << "' does not exist";
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

		sPipelineCacheFilePath += '/' + string{ _Props2HexString() } + ".cache";

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
		const char* szHexProps = _Props2HexString();
		
		const string sDirectXPipelineCacheFilePath = m_csPipelineCachePath + "/DirectX/" + szHexProps + ".cache";
		const string sOpenGLPipelineCacheFilePath = m_csPipelineCachePath + "/OpenGL/" + szHexProps + ".cache";
		const string sVulkanPipelineCacheFilePath = m_csPipelineCachePath + "/Vulkan/" + szHexProps + ".cache";
		
		if (m_programFilesManager.ExistsFile(sOpenGLPipelineCacheFilePath))
			uMask |= PipelineCacheStatusBit_OpenGLCache;
		else if (m_programFilesManager.ExistsFile(sVulkanPipelineCacheFilePath))
			uMask |= PipelineCacheStatusBit_VulkanCache;
		else if (m_programFilesManager.ExistsFile(sDirectXPipelineCacheFilePath))
			uMask |= PipelineCacheStatusBit_DirectXCache;

		if (uMask) return uMask;
		
		// check if spirv exists
		const string sVertexShaderSpirvPath = 
			string{ m_csVertexShaderSpirvPath } + '/' + m_csVertexShaderSpirvName + ".vert.spv";
		const string sGeometryShaderSourcePath =
			string{ m_csGeometryShaderSourcePath } + '/' + m_csGeometryShaderSourceName + ".geom";
		const string sGeometryShaderSpirvPath = 
			string{ m_csGeometryShaderSpirvPath } + '/' + m_csGeometryShaderSpirvName + ".geom.spv";
		const string sFragmentShaderSpirvPath =
			string{ m_csFragmentShaderSpirvPath } + '/' + m_csFragmentShaderSpirvName + ".frag.spv";
		
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