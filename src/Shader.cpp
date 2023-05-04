// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: UberShader.cpp - shader requrest classes implementation
// author: Karl-Mihkel Ott

#define SHADER_CPP
#include "deng/Shader.h"

using namespace std;
namespace fs = filesystem;
#define MD5LEN 16

namespace DENG {

	Shader::Shader(const string& _sVertexShaderSourcePath, const string& _sFragmentShaderSourcePath, const string& _sGeometryShaderSourcePath) :
		m_csVertexShaderSourcePath(_sVertexShaderSourcePath),
		m_sGeometryShaderSourcePath(_sGeometryShaderSourcePath),
		m_csFragmentShaderSourcePath(_sFragmentShaderSourcePath)
	{
		string vertexStem = fs::path(m_csVertexShaderSourcePath).stem().u8string();
		string geometryStem = fs::path(m_sGeometryShaderSourcePath).stem().u8string();
		string fragmentStem = fs::path(m_csFragmentShaderSourcePath).stem().u8string();

		if ((geometryStem != "" && vertexStem == geometryStem && vertexStem == fragmentStem) || (vertexStem == fragmentStem))
			m_sGenericShaderIdentifier = vertexStem;
		else {
			stringstream ss;
			ss << "Shader source code naming convention error.\n" <<
				"Cannot determine shader identifier from files " <<
				fs::path(m_csVertexShaderSourcePath).filename().u8string() << ",  " <<
				fs::path(m_sGeometryShaderSourcePath).filename().u8string() << " and " <<
				fs::path(m_csFragmentShaderSourcePath).filename().u8string() <<
				"\nAll shader source files must have the same parent directory and the same stem filenames!";

			throw ConventionException(ss.str());
		}
	}


	vector<uint32_t> Shader::_CompileVertexShaderToSpirv() {
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		for (auto it = m_vertexShaderMacros.begin(); it != m_vertexShaderMacros.end(); it++) {
			if (it->second != "")
				options.AddMacroDefinition(it->first, it->second);
			else options.AddMacroDefinition(it->first);
		}

		const string csVertexShaderSourcePath =
			m_csShaderSourcePath + '\\' + m_csVertexShaderSourcePath;
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
				m_csSpirvBinaryPath + '\\' + m_sGenericShaderIdentifier + '\\' + m_sGenericShaderIdentifier + ".vert.spv");
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), NON_CRITICAL);
		}

		return spirv;
	}


	vector<uint32_t> Shader::_CompileGeometryShaderToSpirv() {
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		for (auto it = m_geometryShaderMacros.begin(); it != m_geometryShaderMacros.end(); it++) {
			if (it->second != "")
				options.AddMacroDefinition(it->first, it->second);
			else options.AddMacroDefinition(it->first);
		}

		const string csGeometryShaderSourcePath =
			m_csShaderSourcePath + '\\' + m_sGeometryShaderSourcePath;

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
				m_csSpirvBinaryPath + '\\' + m_sGenericShaderIdentifier + '\\' + m_sGenericShaderIdentifier + ".geom.spv");
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), NON_CRITICAL);
		}

		return spirv;
	}


	vector<uint32_t> Shader::_CompileFragmentShaderToSpirv() {
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		for (auto it = m_fragmentShaderMacros.begin(); it != m_geometryShaderMacros.end(); it++) {
			if (it->second != "")
				options.AddMacroDefinition(it->first, it->second);
			else options.AddMacroDefinition(it->first);
		}

		const string csFragmentShaderSourcePath =
			m_csShaderSourcePath + '\\' + m_csFragmentShaderSourcePath;
		vector<char> sSourceCode = m_programFilesManager.GetProgramFileContent(csFragmentShaderSourcePath);
		shaderc::CompilationResult module = compiler.CompileGlslToSpv(
			sSourceCode.data(),
			sSourceCode.size(),
			shaderc_geometry_shader,
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
				m_csSpirvBinaryPath + '\\' + m_sGenericShaderIdentifier + '\\' + m_sGenericShaderIdentifier + ".frag.spv");
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), NON_CRITICAL);
		}

		return spirv;
	}


	vector<uint32_t> Shader::ReadVertexShaderSpirv() {
		const string csVertexShaderSpirvFilePath = m_csSpirvBinaryPath + '\\' + m_sGenericShaderIdentifier + ".vert.spv";
		const string csVertexShaderSourceFilePath = m_csShaderSourcePath + '\\' + m_csVertexShaderSourcePath;
		
		time_t tSpvWriteTimestamp = 0, tSourceWriteTimestamp = 0;

		try {
			tSourceWriteTimestamp = m_programFilesManager.GetFileTimestamp(csVertexShaderSpirvFilePath);
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), NON_CRITICAL);
			return {};
		}

		try {
			tSpvWriteTimestamp = m_programFilesManager.GetFileTimestamp(csVertexShaderSpirvFilePath);
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), WARNING);
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
	
	
	vector<uint32_t> Shader::ReadGeometryShaderSpirv() {
		const string csGeometryShaderSpirvFilePath =
			m_csSpirvBinaryPath + '\\' + m_sGenericShaderIdentifier + ".geom.spv";
		const string csGeometryShaderSourceFilePath =
			m_csShaderSourcePath + '\\' + m_sGeometryShaderSourcePath;

		time_t tSpvWriteTimestamp = 0, tSourceWriteTimestamp = 0;
		try {
			tSourceWriteTimestamp = m_programFilesManager.GetFileTimestamp(csGeometryShaderSourceFilePath);
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), WARNING);
			return {};
		}

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
	
	
	vector<uint32_t> Shader::ReadFragmentShaderSpirv() {
		const string csFragmentShaderSpirvFilePath =
			m_csSpirvBinaryPath + '\\' + m_sGenericShaderIdentifier + ".frag.spv";
		const string csFragmentShaderSourceFilePath =
			m_csShaderSourcePath + '\\' + m_csVertexShaderSourcePath;
	
		time_t tSpvWriteTimestamp = 0, tSourceWriteTimestamp = 0;

		try {
			tSourceWriteTimestamp = m_programFilesManager.GetFileTimestamp(csFragmentShaderSourceFilePath);
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), NON_CRITICAL);
			return {};
		}

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


	vector<char> Shader::GetPipelineCache(RendererType _eRendererType) {
		string sPipelineCacheFilePath = m_csPipelineCachePath;

		switch (_eRendererType) {
			case RENDERER_TYPE_OPENGL:
				sPipelineCacheFilePath += "\\OpenGL";
				break;

			case RENDERER_TYPE_VULKAN:
				sPipelineCacheFilePath += "\\Vulkan";
				break;

			default:
				return {};
		}

		sPipelineCacheFilePath += '\\' + m_sGenericShaderIdentifier + ".cache";

		if (!m_programFilesManager.ExistsFile(sPipelineCacheFilePath)) {
			stringstream ss;
			ss << "Pipeline cache for shader module '" << m_sGenericShaderIdentifier << "' does not exist";
			throw ShaderException(ss.str());
			return {};
		}

		return m_programFilesManager.GetProgramFileContent(sPipelineCacheFilePath);
	}


	void Shader::CachePipeline(RendererType _eRendererType, const void* _pData, size_t _uLength) {
		DENG_ASSERT(_eRendererType == RENDERER_TYPE_VULKAN || _eRendererType == RENDERER_TYPE_OPENGL);
		string sPipelineCacheFilePath = m_csPipelineCachePath;

		switch (_eRendererType) {
			case RENDERER_TYPE_VULKAN:
				sPipelineCacheFilePath += "\\Vulkan";
				break;

			case RENDERER_TYPE_OPENGL:
				sPipelineCacheFilePath += "\\OpenGL";
				break;
		}

		sPipelineCacheFilePath += "\\" + m_sGenericShaderIdentifier + ".cache";

		try {
			m_programFilesManager.WriteProgramFile((const char*)_pData, _uLength, sPipelineCacheFilePath);
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), NON_CRITICAL);
		}
	}


	PipelineCacheStatus Shader::GetPipelineCacheStatus() {
		// check if pipeline cache exists
		const string sOpenGLPipelineCacheFilePath = m_csPipelineCachePath + "\\OpenGL\\" + m_sGenericShaderIdentifier + ".cache";
		const string sVulkanPipelineCacheFilePath = m_csPipelineCachePath + "\\Vulkan\\" + m_sGenericShaderIdentifier + ".cache";
		
		if (m_programFilesManager.ExistsFile(sOpenGLPipelineCacheFilePath) && m_programFilesManager.ExistsFile(sVulkanPipelineCacheFilePath))
			return CACHED_PIPELINE;
		else if (m_programFilesManager.ExistsFile(sOpenGLPipelineCacheFilePath))
			return OPENGL_CACHE;
		else if (m_programFilesManager.ExistsFile(sVulkanPipelineCacheFilePath))
			return VULKAN_CACHE;

		// check if spirv exists
		const string sVertexShaderSpirvPath = 
			m_csSpirvBinaryPath + '\\' + m_sGenericShaderIdentifier + '\\' + m_sGenericShaderIdentifier + ".vert.spv";
		const string sGeometryShaderSourcePath =
			m_csShaderSourcePath + '\\' + m_sGeometryShaderSourcePath;
		const string sGeometryShaderSpirvPath = 
			m_csSpirvBinaryPath + '\\' + m_sGenericShaderIdentifier + '\\' + m_sGenericShaderIdentifier + ".geom.spv";
		const string sFragmentShaderSpirvPath =
			m_csSpirvBinaryPath + '\\' + m_sGenericShaderIdentifier + '\\' + m_sGenericShaderIdentifier + ".frag.spv";
		
		if (m_programFilesManager.ExistsFile(sVertexShaderSpirvPath) &&
			(!m_programFilesManager.ExistsFile(sGeometryShaderSourcePath) || m_programFilesManager.ExistsFile(sGeometryShaderSpirvPath)) &&
			m_programFilesManager.ExistsFile(sFragmentShaderSpirvPath))
		{
			return SPIRV;
		}
		else if (m_programFilesManager.ExistsFile(sVertexShaderSpirvPath) ||
			(!m_programFilesManager.ExistsFile(sGeometryShaderSourcePath) || m_programFilesManager.ExistsFile(sGeometryShaderSpirvPath)) ||
			m_programFilesManager.ExistsFile(sFragmentShaderSpirvPath))
		{
			return PARTIAL_SPIRV;
		}
		else return NO_CACHE;
	}


	UberShader::UberShader(const Libdas::DasModel& _model, uint32_t _uMeshId) :
		Shader("UberShader\\UberShader.vert", "UberShader\\UberShader.frag", "UberShader\\UberShader.geom")
	{
		_CreateVertexShaderProperties(_model, _uMeshId);
		_CreateGeometryShaderProperties(_model, _uMeshId);
		_CreateFragmentShaderProperties(_model, _uMeshId);

		if (!m_properties.bUseGeometryShader)
			SetGeometryShaderSourcePath("");

		_CalculateMD5Hash(reinterpret_cast<char*>(&m_properties), sizeof(m_properties));
		SetGenericShaderIdentifier(m_sMD5Hash);

		_AddVertexShaderMacroDefinitions();
		if (m_properties.bUseGeometryShader)
			_AddGeometryShaderMacroDefinitions();
		_AddFragmentShaderMacroDefinitions();
	}


	void UberShader::_CalculateMD5Hash(const char* _pSrc, size_t _uLen) {
		HCRYPTPROV hProv = 0;
		HCRYPTHASH hHash = 0;
		BYTE rgbHash[MD5LEN]{};
		CHAR rgbDigits[] = "0123456789abcdef";

		if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
			DWORD dwStatus = GetLastError();
			cerr << "CryptAcquireContext failed: 0x" << setfill('0') << setw(8) <<
				hex << dwStatus << endl; cerr << "CryptAcquireContext failed: 0x" << hex << dwStatus << endl;
			return;
		}

		if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
			DWORD dwStatus = GetLastError();
			cerr << "CryptCreateHash failed: 0x" << setfill('0') << setw(8) <<
				hex << dwStatus << endl;
			return;
		}

		if (!CryptHashData(hHash, (const BYTE*)_pSrc, (DWORD)_uLen, 0)) {
			DWORD dwStatus = GetLastError();
			cerr << "CryptHashData failed: 0x" << setfill('0') << setw(8) <<
				hex << dwStatus << endl;
		}

		DWORD cbHash = MD5LEN; // md5 hash length in bytes
		if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
			stringstream ss;
			for (DWORD i = 0; i < cbHash; i++) {
				ss << rgbDigits[rgbHash[i] >> 4] << rgbDigits[rgbHash[i] & 0xff];
			}
			m_sMD5Hash = ss.str();
		}
		else {
			DWORD dwStatus = GetLastError();
			cerr << "CryptGetHashParam failed: " << setfill('0') << setw(8) <<
				hex << dwStatus << endl;
		}

		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
	}


	void UberShader::_CreateVertexShaderProperties(const Libdas::DasModel& _model, uint32_t _uMeshId) {
		const Libdas::DasMeshPrimitive& samplePrimitive =
			_model.mesh_primitives[_model.meshes[_uMeshId].primitives[0]];

		int32_t uLocationCounter = 1;
		int32_t uOutputLocationCounter = 1;

		if (samplePrimitive.vertex_buffer_id != UINT32_MAX) {
			m_properties.iVertexShaderNormalLocation = uLocationCounter++;
			m_properties.iVertexShaderNormalOutputLocation = uOutputLocationCounter++;
		}
		if (samplePrimitive.vertex_tangent_buffer_id != UINT32_MAX) {
			m_properties.iVertexShaderTangentLocation = uLocationCounter++;
			m_properties.iVertexShaderTangentOutputLocation = uOutputLocationCounter++;
		}
		if (samplePrimitive.texture_count) {
			m_properties.iVertexShaderUVCoordinatesLocation = uLocationCounter;
			uLocationCounter += samplePrimitive.texture_count;
			m_properties.iVertexShaderUVCoordinatesOutputLocation = uOutputLocationCounter;
			uOutputLocationCounter += samplePrimitive.texture_count;
			m_properties.iVertexShaderUVAttributeCount = samplePrimitive.texture_count;
		}
		if (samplePrimitive.color_mul_count) {
			m_properties.iVertexShaderColorMultiplierLocation = uLocationCounter;
			uLocationCounter += samplePrimitive.color_mul_count;
			m_properties.iVertexShaderColorMultiplierOutputLocation = uOutputLocationCounter;
			uOutputLocationCounter += samplePrimitive.color_mul_count;
			m_properties.iVertexShaderColorMultiplierAttributeCount = samplePrimitive.color_mul_count;
		}
		if (samplePrimitive.joint_set_count) {
			m_properties.bVertexShaderUseJointTransforms = true;
			m_properties.iVertexShaderJointIndicesLocation = uLocationCounter;
			uLocationCounter += samplePrimitive.joint_set_count;
			m_properties.iVertexShaderJointWeightsLocation = uLocationCounter;
			uLocationCounter += samplePrimitive.joint_set_count;
			m_properties.iVertexShaderJointSetCount = samplePrimitive.joint_set_count;
			m_properties.iVertexShaderJointCount = static_cast<int32_t>(_model.joints.size());
		}
		if (samplePrimitive.morph_target_count) {
			m_properties.bVertexShaderUseMorphTargets = true;
			m_properties.iVertexShaderMorphSetCount = samplePrimitive.morph_target_count % 4 ? samplePrimitive.morph_target_count / 4 + 1 : samplePrimitive.morph_target_count / 4;
			m_properties.iVertexShaderMorphTargetCount = samplePrimitive.morph_target_count;

			if (_model.morph_targets[samplePrimitive.morph_targets[0]].vertex_normal_buffer_id != UINT32_MAX) {
				m_properties.iVertexShaderMorphNormalsLocation = uLocationCounter;
				uLocationCounter += samplePrimitive.morph_target_count;
			}
			if (_model.morph_targets[samplePrimitive.morph_targets[0]].vertex_tangent_buffer_id != UINT32_MAX) {
				m_properties.iVertexShaderMorphTangentsLocation = uLocationCounter;
				uLocationCounter += samplePrimitive.morph_target_count;
			}

			// uv and color multiplier properties are not yet implemented
			// temporarily
		}
	}


	void UberShader::_CreateGeometryShaderProperties(const Libdas::DasModel& _model, uint32_t _uMeshId) {
		const Libdas::DasMeshPrimitive& samplePrimitive =
			_model.mesh_primitives[_model.meshes[_uMeshId].primitives[0]];

		uint32_t uLocationCounter = 1;
		// 1: Position vertices
		// 2: Vertex normals
		uint32_t uOutputLocationCounter = 2;

		// NOTE: geometry shader is only used when vertex normals are missing
		if (samplePrimitive.vertex_normal_buffer_id == UINT32_MAX) {
			m_properties.bUseGeometryShader = true;
			if (samplePrimitive.texture_count) {
				m_properties.iGeometryShaderUVCoordinatesLocation = uLocationCounter;
				uLocationCounter += samplePrimitive.texture_count;
				m_properties.iGeometryShaderUVCoordinatesOutputLocation = uOutputLocationCounter;
				uOutputLocationCounter += samplePrimitive.texture_count;
				m_properties.iGeometryShaderUVAttributeCount = samplePrimitive.texture_count;
			}
			if (samplePrimitive.color_mul_count) {
				m_properties.iGeometryShaderColorMultipliersLocation = uLocationCounter;
				uLocationCounter += samplePrimitive.color_mul_count;
				m_properties.iGeometryShaderColorMultipliersOutputLocation = uOutputLocationCounter;
				uOutputLocationCounter += samplePrimitive.color_mul_count;
				m_properties.iGeometryShaderColorMultipliersAttributeCount = samplePrimitive.color_mul_count;
			}
		}
	}


	void UberShader::_CreateFragmentShaderProperties(const Libdas::DasModel& _model, uint32_t _uMeshId) {
		const Libdas::DasMeshPrimitive& samplePrimitive =
			_model.mesh_primitives[_model.meshes[_uMeshId].primitives[0]];

		uint32_t uLocationCounter = 1;
		m_properties.iFragmentShaderVertexNormalLocation = uLocationCounter++;

		if (samplePrimitive.vertex_tangent_buffer_id != UINT32_MAX)
			m_properties.iFragmentShaderVertexTangentLocation = uLocationCounter++;
		if (samplePrimitive.texture_count) {
			m_properties.iFragmentShaderUVCoordinatesLocation = uLocationCounter;
			uLocationCounter += samplePrimitive.texture_count;
			m_properties.iFragmentShaderUVAttributeCount = samplePrimitive.texture_count;
		}
		if (samplePrimitive.color_mul_count) {
			m_properties.iFragmentShaderColorMultipliersLocation = uLocationCounter;
			uLocationCounter += samplePrimitive.color_mul_count;
			m_properties.iFragmentShaderColorMultiplierAttributeCount = samplePrimitive.color_mul_count;
		}
	}


	void UberShader::_AddVertexShaderMacroDefinitions() {
		// vertex normals
		if (m_properties.iVertexShaderNormalLocation != -1)
			AddVertexShaderMacroDefinition("VERTEX_NORMALS", 
				to_string(m_properties.iVertexShaderNormalLocation));
		if (m_properties.iVertexShaderNormalOutputLocation != -1)
			AddVertexShaderMacroDefinition("OUTPUT_VERTEX_NORMALS",
				to_string(m_properties.iVertexShaderNormalOutputLocation));
		
		// vertex tangents
		if (m_properties.iVertexShaderTangentLocation != -1)
			AddVertexShaderMacroDefinition("VERTEX_TANGENTS",
				to_string(m_properties.iVertexShaderTangentLocation));
		if (m_properties.iVertexShaderTangentOutputLocation != -1)
			AddVertexShaderMacroDefinition("OUTPUT_VERTEX_TANGENTS",
				to_string(m_properties.iVertexShaderTangentOutputLocation));
		
		// uv coordinates
		if (m_properties.iVertexShaderUVCoordinatesLocation != -1)
			AddVertexShaderMacroDefinition("UV_COORDINATES",
				to_string(m_properties.iVertexShaderUVCoordinatesLocation));
		if (m_properties.iVertexShaderUVAttributeCount)
			AddVertexShaderMacroDefinition("UV_ATTRIB_COUNT",
				to_string(m_properties.iVertexShaderUVAttributeCount));
		
		// color multipliers
		if (m_properties.iVertexShaderColorMultiplierLocation != -1)
			AddVertexShaderMacroDefinition("COLOR_MULTIPLIERS",
				to_string(m_properties.iVertexShaderColorMultiplierLocation));
		if (m_properties.iVertexShaderColorMultiplierAttributeCount)
			AddVertexShaderMacroDefinition("COLOR_MULTIPLIER_ATTRIB_COUNT",
				to_string(m_properties.iVertexShaderColorMultiplierAttributeCount));
		
		// joint transforms
		if (m_properties.bVertexShaderUseJointTransforms) {
			AddVertexShaderMacroDefinition("USE_JOINT_TRANSFORMS");
			AddVertexShaderMacroDefinition("JOINT_INDICES",
				to_string(m_properties.iVertexShaderJointIndicesLocation));
			AddVertexShaderMacroDefinition("JOINT_WEIGHTS",
				to_string(m_properties.iVertexShaderJointWeightsLocation));
			AddVertexShaderMacroDefinition("JOINT_SET_COUNT",
				to_string(m_properties.iVertexShaderJointSetCount));
			AddVertexShaderMacroDefinition("JOINT_COUNT",
				to_string(m_properties.iVertexShaderJointCount));
		}
		
		// morph targets
		if (m_properties.bVertexShaderUseMorphTargets) {
			AddVertexShaderMacroDefinition("USE_MORPH_TARGETS");
			AddVertexShaderMacroDefinition("MORPH_SET_COUNT",
				to_string(m_properties.iVertexShaderMorphSetCount));
			AddVertexShaderMacroDefinition("MORPH_TARGET_COUNT",
				to_string(m_properties.iVertexShaderMorphTargetCount));
			AddVertexShaderMacroDefinition("MORPH_POSITIONS",
				to_string(m_properties.iVertexShaderMorphPositionsLocation));
			
			if (m_properties.iVertexShaderMorphNormalsLocation != -1)
				AddVertexShaderMacroDefinition("MORPH_NORMALS",
					to_string(m_properties.iVertexShaderMorphNormalsLocation));
			if (m_properties.iVertexShaderMorphTangentsLocation != -1)
				AddVertexShaderMacroDefinition("MORPH_TANGENTS",
					to_string(m_properties.iVertexShaderMorphTangentsLocation));
		}
	}


	void UberShader::_AddGeometryShaderMacroDefinitions() {
		// uv coordinates
		if (m_properties.iGeometryShaderUVCoordinatesLocation != -1)
			AddGeometryShaderMacroDefinition("UV_COORDINATES",
				to_string(m_properties.iGeometryShaderUVCoordinatesLocation));
		if (m_properties.iGeometryShaderUVAttributeCount)
			AddGeometryShaderMacroDefinition("UV_ATTRIB_COUNT",
				to_string(m_properties.iGeometryShaderUVAttributeCount));
		if (m_properties.iGeometryShaderUVCoordinatesOutputLocation != -1)
			AddGeometryShaderMacroDefinition("OUTPUT_UV_COORDINATES",
				to_string(m_properties.iGeometryShaderUVCoordinatesOutputLocation));
	
		// color multipliers
		if (m_properties.iGeometryShaderColorMultipliersLocation != -1)
			AddGeometryShaderMacroDefinition("COLOR_MULTIPLIERS",
				to_string(m_properties.iGeometryShaderColorMultipliersLocation));
		if (m_properties.iGeometryShaderColorMultipliersAttributeCount)
			AddGeometryShaderMacroDefinition("COLOR_MULTIPLIER_ATTRIB_COUNT",
				to_string(m_properties.iGeometryShaderColorMultipliersAttributeCount));
		if (m_properties.iGeometryShaderColorMultipliersOutputLocation != -1)
			AddGeometryShaderMacroDefinition("OUTPUT_COLOR_MULTIPLIERS",
				to_string(m_properties.iGeometryShaderColorMultipliersOutputLocation));
	}


	void UberShader::_AddFragmentShaderMacroDefinitions() {
		// vertex normals
		if (m_properties.iFragmentShaderVertexNormalLocation != -1)
			AddFragmentShaderMacroDefinition("VERTEX_NORMALS",
				to_string(m_properties.iFragmentShaderVertexNormalLocation));
		
		// vertex tangents
		if (m_properties.iFragmentShaderVertexTangentLocation != -1)
			AddFragmentShaderMacroDefinition("VERTEX_TANGENTS",
				to_string(m_properties.iFragmentShaderVertexTangentLocation));
		
		// uv coordinates
		if (m_properties.iFragmentShaderUVCoordinatesLocation != -1)
			AddFragmentShaderMacroDefinition("UV_COORDINATES",
				to_string(m_properties.iFragmentShaderUVCoordinatesLocation));
		if (m_properties.iFragmentShaderUVAttributeCount)
			AddFragmentShaderMacroDefinition("UV_ATTRIB_COUNT",
				to_string(m_properties.iFragmentShaderUVAttributeCount));
		
		// color multipliers
		if (m_properties.iFragmentShaderColorMultipliersLocation != -1)
			AddFragmentShaderMacroDefinition("COLOR_MULTIPLIERS",
				to_string(m_properties.iFragmentShaderColorMultipliersLocation));
		if (m_properties.iFragmentShaderColorMultiplierAttributeCount)
			AddFragmentShaderMacroDefinition("COLOR_MULTIPLIER_ATTRIB_COUNT",
				to_string(m_properties.iFragmentShaderColorMultiplierAttributeCount));
	}
}