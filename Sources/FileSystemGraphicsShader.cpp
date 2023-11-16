// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: UberShader.cpp - shader requrest classes implementation
// author: Karl-Mihkel Ott

#define FILE_SYSTEM_SHADER_CPP

#include "deng/FileSystemGraphicsShader.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <chrono>
#include <filesystem>

#include "deng/Api.h"
#include "deng/ErrorDefinitions.h"
#include "deng/Exceptions.h"
#include "deng/Strings.h"

#include <cvar/CVarSystem.h>

using namespace std;
namespace fs = filesystem;


namespace DENG {

	FileSystemGraphicsShader::FileSystemGraphicsShader(
		IGraphicsShaderCompiler* _pGraphicsShaderCompiler,
		const string& _sVertexShaderSourceName,
		const string& _sGeometryShaderSourceName,
		const string& _sFragmentShaderSourceName,
		const string& _sVertexShaderByteCodeName,
		const string& _sGeometryShaderByteCodeName,
		const string& _sFragmentShaderByteCodeName
	) :
		m_csVertexShaderSourceName(_sVertexShaderSourceName),
		m_csGeometryShaderSourceName(_sGeometryShaderSourceName),
		m_csFragmentShaderSourceName(_sFragmentShaderSourceName),
		m_csVertexShaderByteCodeName((_sVertexShaderByteCodeName == "" ? _sVertexShaderSourceName : _sVertexShaderByteCodeName)),
		m_csGeometryShaderByteCodeName((_sGeometryShaderByteCodeName == "" ? _sGeometryShaderSourceName : _sGeometryShaderByteCodeName)),
		m_csFragmentShaderByteCodeName((_sFragmentShaderByteCodeName == "" ? _sFragmentShaderSourceName : _sFragmentShaderByteCodeName)),
		m_pCompiler(_pGraphicsShaderCompiler)
	{
		DENG_ASSERT(m_pCompiler);
		const string sVertexShaderModuleCrc32Src = m_csVertexShaderSourceName + '|' + m_csVertexShaderByteCodeName;
		const string sGeometryShaderModuleCrc32Src = m_csGeometryShaderSourceName + '|' + m_csGeometryShaderByteCodeName;
		const string sFragmentShaderModuleCrc32Src = m_csFragmentShaderSourceName + '|' + m_csFragmentShaderByteCodeName;
	
		const uint32_t uVertexModuleHash = cvar::RuntimeCrc32(sVertexShaderModuleCrc32Src);
		const uint32_t uGeometryModuleHash = cvar::RuntimeCrc32(sGeometryShaderModuleCrc32Src);
		const uint32_t uFragmentModuleHash = cvar::RuntimeCrc32(sFragmentShaderModuleCrc32Src);

		m_bProperties |= (bitset<256>{static_cast<uint64_t>(uVertexModuleHash)} << VertexShaderNameCrc32PropertyOffset);
		m_bProperties |= (bitset<256>{static_cast<uint64_t>(uGeometryModuleHash)} << GeometryShaderNameCrc32PropertyOffset);
		m_bProperties |= (bitset<256>{static_cast<uint64_t>(uFragmentModuleHash)} << FragmentShaderNameCrc32PropertyOffset);
	}

	FileSystemGraphicsShader::~FileSystemGraphicsShader() {
		delete m_pCompiler;
	}

	void FileSystemGraphicsShader::_WriteShaderByteCodeToFile(const string& _csPath, const vector<uint32_t>& _byteCode) const {
		std::vector<char> buf;
		buf.resize(_byteCode.size() * sizeof(uint32_t));
		memcpy(buf.data(), _byteCode.data(), _byteCode.size() * sizeof(uint32_t));
		m_programFilesManager.WriteProgramFile(buf, _csPath);
	}

	vector<uint32_t> FileSystemGraphicsShader::GetVertexShaderByteCode() const {
		const string csVertexShaderByteCodeFilePath =
			FormatString(m_csVertexShaderByteCodePath.data(), m_pCompiler->GetByteCodeType(), m_csVertexShaderSourceName.c_str(), m_pCompiler->GetSourceType().data(), m_pCompiler->GetByteCodeType().data());
		const string csVertexShaderSourceFilePath =
			FormatString(m_csVertexShaderSourcePath.data(), m_csVertexShaderSourceName.c_str(), m_pCompiler->GetSourceType().data());

		time_t tSpvWriteTimestamp = 0, tSourceWriteTimestamp = 0;
		tSourceWriteTimestamp = m_programFilesManager.GetFileTimestamp(csVertexShaderSourceFilePath);

		try {
			tSpvWriteTimestamp = m_programFilesManager.GetFileTimestamp(csVertexShaderByteCodeFilePath);
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), ErrorSeverity::WARNING);
		}

		if (tSpvWriteTimestamp > tSourceWriteTimestamp) {
			auto input = m_programFilesManager.GetProgramFileContent(csVertexShaderByteCodeFilePath);
			vector<uint32_t> output(input.size() / sizeof(uint32_t));
			for (size_t i = 0; i < output.size(); i++)
				output[i] = reinterpret_cast<uint32_t*>(input.data())[i];
			
			return output;
		}
		
		auto byteCode = m_pCompiler->CompileVertexShaderFile(csVertexShaderSourceFilePath);
		_WriteShaderByteCodeToFile(csVertexShaderSourceFilePath, byteCode);
		return m_pCompiler->CompileVertexShaderFile(csVertexShaderSourceFilePath);
	}
	
	
	vector<uint32_t> FileSystemGraphicsShader::GetGeometryShaderByteCode() const {
		const string csGeometryShaderByteCodeFilePath =
			FormatString(m_csGeometryShaderByteCodePath.data(), m_pCompiler->GetByteCodeType(), m_csGeometryShaderSourceName.c_str(), m_pCompiler->GetSourceType().data(), m_pCompiler->GetByteCodeType().data());
		const string csGeometryShaderSourceFilePath =
			FormatString(m_csGeometryShaderSourcePath.data(), m_csGeometryShaderSourceName.c_str(), m_pCompiler->GetSourceType().data());

		time_t tSpvWriteTimestamp = 0, tSourceWriteTimestamp = 0;
		tSourceWriteTimestamp = m_programFilesManager.GetFileTimestamp(csGeometryShaderSourceFilePath);

		try {
			tSpvWriteTimestamp = m_programFilesManager.GetFileTimestamp(csGeometryShaderByteCodeFilePath);
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), WARNING);
		}
	
		if (tSpvWriteTimestamp > tSourceWriteTimestamp) {
			auto input = m_programFilesManager.GetProgramFileContent(csGeometryShaderByteCodeFilePath);
			vector<uint32_t> output(input.size() / sizeof(uint32_t));
			for (size_t i = 0; i < output.size(); i++)
				output[i] = reinterpret_cast<uint32_t*>(input.data())[i];
	
			return output;
		}
		
		auto byteCode = m_pCompiler->CompileGeometryShaderFile(csGeometryShaderSourceFilePath);
		_WriteShaderByteCodeToFile(csGeometryShaderByteCodeFilePath, byteCode);
		return byteCode;
	}
	
	
	vector<uint32_t> FileSystemGraphicsShader::GetFragmentShaderByteCode() const {
		const string csFragmentShaderByteCodeFilePath =
			FormatString(m_csFragmentShaderByteCodePath.data(), m_pCompiler->GetByteCodeType(), m_csFragmentShaderSourceName.c_str(), m_pCompiler->GetSourceType(), m_pCompiler->GetByteCodeType());
		const string csFragmentShaderSourceFilePath =
			FormatString(m_csFragmentShaderSourcePath.data(), m_csFragmentShaderSourceName.c_str(), m_pCompiler->GetSourceType());

		time_t tSpvWriteTimestamp = 0, tSourceWriteTimestamp = 0;
		tSourceWriteTimestamp = m_programFilesManager.GetFileTimestamp(csFragmentShaderSourceFilePath);

		try {
			tSpvWriteTimestamp = m_programFilesManager.GetFileTimestamp(csFragmentShaderByteCodeFilePath);
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), WARNING);
		}

		if (tSpvWriteTimestamp > tSourceWriteTimestamp) {
			auto input = m_programFilesManager.GetProgramFileContent(csFragmentShaderByteCodeFilePath);
			vector<uint32_t> output(input.size() / sizeof(uint32_t));
			for (size_t i = 0; i < output.size(); i++)
				output[i] = reinterpret_cast<uint32_t*>(input.data())[i];
	
			return output;
		}
		
		auto byteCode = m_pCompiler->CompileFragmentShaderFile(csFragmentShaderSourceFilePath);
		_WriteShaderByteCodeToFile(csFragmentShaderByteCodeFilePath, byteCode);
		return byteCode;
	}


	vector<char> FileSystemGraphicsShader::GetPipelineCache() const {
		auto cvarSystem = cvar::CVarSystem::GetInstance();
		auto pBackend = cvarSystem.Get<cvar::String>("runtime.graphics.activeBackend");
		DENG_ASSERT(pBackend);

		const string csBitsetHexString = BitsetToHexString(m_bProperties);
		const string csPipelineCacheFilePath =
			FormatString(m_csPipelineCachePath.data(), pBackend->GetSTDString().c_str(), csBitsetHexString.c_str());

		if (!m_programFilesManager.ExistsFile(csPipelineCacheFilePath)) {
			stringstream ss;
			ss << "Pipeline cache for shader module '" << csBitsetHexString << "' does not exist";
			throw ShaderException(ss.str());
			return {};
		}

		return m_programFilesManager.GetProgramFileContent(csPipelineCacheFilePath);
	}


	void FileSystemGraphicsShader::CachePipeline(const void* _pData, size_t _uLength) const {
		auto cvarSystem = cvar::CVarSystem::GetInstance();
		auto pBackend = cvarSystem.Get<cvar::String>("runtime.graphics.activeBackend");
		DENG_ASSERT(pBackend);

		const string csBitsetHexString = BitsetToHexString(m_bProperties);
		const string csPipelineCacheFilePath =
			FormatString(m_csPipelineCachePath.data(), pBackend->GetSTDString().c_str(), csBitsetHexString.c_str());

		try {
			m_programFilesManager.WriteProgramFile((const char*)_pData, _uLength, csPipelineCacheFilePath);
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), NON_CRITICAL);
		}
	}


	PipelineCacheStatus FileSystemGraphicsShader::GetPipelineCacheStatus() const {
		PipelineCacheStatus eStatus = PipelineCacheStatus_NoCache;

		// check if pipeline cache exists
		auto cvarSystem = cvar::CVarSystem::GetInstance();
		auto pBackend = cvarSystem.Get<cvar::String>("runtime.graphics.activeBackend");
		DENG_ASSERT(pBackend);

		const string csHexProps = BitsetToHexString(m_bProperties);
		const string csPipelineCacheFilePath =
			FormatString(m_csPipelineCachePath.data(), pBackend->GetSTDString().c_str(), csHexProps);
		
		if (m_programFilesManager.ExistsFile(csPipelineCacheFilePath))
			return PipelineCacheStatus_CachedPipeline;

		// check if ByteCode exists
		const char* const szByteCodeType = m_pCompiler->GetByteCodeType().data();
		const char* const szSourceType = m_pCompiler->GetSourceType().data();

		const string csVertexShaderByteCodePath =
			FormatString(m_csVertexShaderByteCodePath.data(), szByteCodeType, m_csVertexShaderSourceName.c_str(), szSourceType, szByteCodeType);
		const string csGeometryShaderSourcePath =
			FormatString(m_csGeometryShaderSourcePath.data(), m_csGeometryShaderSourceName.c_str(), szSourceType);
		const string csGeometryShaderByteCodePath =
			FormatString(m_csGeometryShaderSourcePath.data(), szByteCodeType, m_csGeometryShaderSourceName.c_str(), szSourceType, szByteCodeType);
		const string csFragmentShaderByteCodePath =
			FormatString(m_csFragmentShaderSourcePath.data(), szByteCodeType, m_csFragmentShaderSourceName.c_str(), szSourceType, szByteCodeType);
		
		if (m_programFilesManager.ExistsFile(csVertexShaderByteCodePath) &&
			(!m_programFilesManager.ExistsFile(csGeometryShaderSourcePath) || m_programFilesManager.ExistsFile(csGeometryShaderByteCodePath)) &&
			m_programFilesManager.ExistsFile(csFragmentShaderByteCodePath))
		{
			return PipelineCacheStatus_ByteCode;
		}
		else if (m_programFilesManager.ExistsFile(csVertexShaderByteCodePath) ||
			(!m_programFilesManager.ExistsFile(csGeometryShaderSourcePath) || m_programFilesManager.ExistsFile(csGeometryShaderByteCodePath)) ||
			m_programFilesManager.ExistsFile(csFragmentShaderByteCodePath))
		{
			return PipelineCacheStatus_PartialByteCode;
		}
		else return PipelineCacheStatus_NoCache;
	}
}