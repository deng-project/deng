// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: UberShader.cpp - UberShader request class implementation
// author: Karl-Mihkel Ott

#define UBER_SHADER_CPP
#include "deng/UberShader.h"

#define MD5LEN 16

namespace DENG {

	UberShader::UberShader(const Libdas::DasModel& _model, uint32_t _uMeshId) {
		_CreateVertexShaderProperties(_model, _uMeshId);
		_CreateGeometryShaderProperties(_model, _uMeshId);
		_CreateFragmentShaderProperties(_model, _uMeshId);

		_CalculateMD5Hash(reinterpret_cast<char*>(&m_properties), sizeof(m_properties));
	}


	std::vector<uint32_t> UberShader::_CompileVertexShader() {
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		// vertex normals
		if (m_properties.iVertexShaderNormalLocation != -1)
			options.AddMacroDefinition("VERTEX_NORMALS", 
				std::to_string(m_properties.iVertexShaderNormalLocation));
		if (m_properties.iVertexShaderNormalOutputLocation != -1)
			options.AddMacroDefinition("OUTPUT_VERTEX_NORMALS",
				std::to_string(m_properties.iVertexShaderNormalOutputLocation));
		
		// vertex tangents
		if (m_properties.iVertexShaderTangentLocation != -1)
			options.AddMacroDefinition("VERTEX_TANGENTS",
				std::to_string(m_properties.iVertexShaderTangentLocation));
		if (m_properties.iVertexShaderTangentOutputLocation != -1)
			options.AddMacroDefinition("OUTPUT_VERTEX_TANGENTS",
				std::to_string(m_properties.iVertexShaderTangentOutputLocation));

		// uv coordinates
		if (m_properties.iVertexShaderUVCoordinatesLocation != -1)
			options.AddMacroDefinition("UV_COORDINATES",
				std::to_string(m_properties.iVertexShaderUVCoordinatesLocation));
		if (m_properties.iVertexShaderUVAttributeCount)
			options.AddMacroDefinition("UV_ATTRIB_COUNT",
				std::to_string(m_properties.iVertexShaderUVAttributeCount));

		// color multipliers
		if (m_properties.iVertexShaderColorMultiplierLocation != -1)
			options.AddMacroDefinition("COLOR_MULTIPLIERS",
				std::to_string(m_properties.iVertexShaderColorMultiplierLocation));
		if (m_properties.iVertexShaderColorMultiplierAttributeCount)
			options.AddMacroDefinition("COLOR_MULTIPLIER_ATTRIB_COUNT",
				std::to_string(m_properties.iVertexShaderColorMultiplierAttributeCount));

		// joint transforms
		if (m_properties.bVertexShaderUseJointTransforms) {
			options.AddMacroDefinition("USE_JOINT_TRANSFORMS");
			options.AddMacroDefinition("JOINT_INDICES",
				std::to_string(m_properties.iVertexShaderJointIndicesLocation));
			options.AddMacroDefinition("JOINT_WEIGHTS",
				std::to_string(m_properties.iVertexShaderJointWeightsLocation));
			options.AddMacroDefinition("JOINT_SET_COUNT",
				std::to_string(m_properties.iVertexShaderJointSetCount));
			options.AddMacroDefinition("JOINT_COUNT",
				std::to_string(m_properties.iVertexShaderJointCount));
		}

		// morph targets
		if (m_properties.bVertexShaderUseMorphTargets) {
			options.AddMacroDefinition("USE_MORPH_TARGETS");
			options.AddMacroDefinition("MORPH_SET_COUNT",
				std::to_string(m_properties.iVertexShaderMorphSetCount));
			options.AddMacroDefinition("MORPH_TARGET_COUNT",
				std::to_string(m_properties.iVertexShaderMorphTargetCount));
			options.AddMacroDefinition("MORPH_POSITIONS",
				std::to_string(m_properties.iVertexShaderMorphPositionsLocation));
			
			if (m_properties.iVertexShaderMorphNormalsLocation != -1)
				options.AddMacroDefinition("MORPH_NORMALS",
					std::to_string(m_properties.iVertexShaderMorphNormalsLocation));
			if (m_properties.iVertexShaderMorphTangentsLocation != -1)
				options.AddMacroDefinition("MORPH_TANGENTS",
					std::to_string(m_properties.iVertexShaderMorphTangentsLocation));
		}

		const std::vector<char> sSourceCode =
			m_programDataManager.GetProgramFileContent(m_csShaderSourcePath + "\\UberShader.vert");
		shaderc::SpvCompilationResult module =
			compiler.CompileGlslToSpv(sSourceCode.data(), sSourceCode.size(), shaderc_vertex_shader, "UberShader.vert");
		
		if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
			std::cerr << module.GetErrorMessage();
			std::exit(EXIT_FAILURE);
		}

		return std::vector<uint32_t>(module.cbegin(), module.cend());
	}


	std::vector<uint32_t> UberShader::_CompileGeometryShader() {
		// geometry shader is not needed when vertex normals are present
		if (!m_properties.bUseGeometryShader)
			return std::vector<uint32_t>();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		// uv coordinates
		if (m_properties.iGeometryShaderUVCoordinatesLocation != -1)
			options.AddMacroDefinition("UV_COORDINATES",
				std::to_string(m_properties.iGeometryShaderUVCoordinatesLocation));
		if (m_properties.iGeometryShaderUVAttributeCount)
			options.AddMacroDefinition("UV_ATTRIB_COUNT",
				std::to_string(m_properties.iGeometryShaderUVAttributeCount));
		if (m_properties.iGeometryShaderUVCoordinatesOutputLocation != -1)
			options.AddMacroDefinition("OUTPUT_UV_COORDINATES",
				std::to_string(m_properties.iGeometryShaderUVCoordinatesOutputLocation));

		// color multipliers
		if (m_properties.iGeometryShaderColorMultipliersLocation != -1)
			options.AddMacroDefinition("COLOR_MULTIPLIERS",
				std::to_string(m_properties.iGeometryShaderColorMultipliersLocation));
		if (m_properties.iGeometryShaderColorMultipliersAttributeCount)
			options.AddMacroDefinition("COLOR_MULTIPLIER_ATTRIB_COUNT",
				std::to_string(m_properties.iGeometryShaderColorMultipliersAttributeCount));
		if (m_properties.iGeometryShaderColorMultipliersOutputLocation != -1)
			options.AddMacroDefinition("OUTPUT_COLOR_MULTIPLIERS",
				std::to_string(m_properties.iGeometryShaderColorMultipliersOutputLocation));
	
		const std::vector<char> sSourceCode =
			m_programDataManager.GetProgramFileContent(m_csShaderSourcePath + "\\UberShader.geom");
		shaderc::SpvCompilationResult module =
			compiler.CompileGlslToSpv(sSourceCode.data(), sSourceCode.size(), shaderc_vertex_shader, "UberShader.geom", options);
		
		if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
			std::cerr << module.GetErrorMessage();
			std::exit(EXIT_FAILURE);
		}

		return std::vector<uint32_t>(module.cbegin(), module.cend());
	}


	std::vector<uint32_t> UberShader::_CompileFragmentShader() {
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		// vertex normals
		if (m_properties.iFragmentShaderVertexNormalLocation != -1)
			options.AddMacroDefinition("VERTEX_NORMALS",
				std::to_string(m_properties.iFragmentShaderVertexNormalLocation));
		
		// vertex tangents
		if (m_properties.iFragmentShaderVertexTangentLocation != -1)
			options.AddMacroDefinition("VERTEX_TANGENTS",
				std::to_string(m_properties.iFragmentShaderVertexTangentLocation));
		
		// uv coordinates
		if (m_properties.iFragmentShaderUVCoordinatesLocation != -1)
			options.AddMacroDefinition("UV_COORDINATES",
				std::to_string(m_properties.iFragmentShaderUVCoordinatesLocation));
		if (m_properties.iFragmentShaderUVAttributeCount)
			options.AddMacroDefinition("UV_ATTRIB_COUNT",
				std::to_string(m_properties.iFragmentShaderUVAttributeCount));

		// color multipliers
		if (m_properties.iFragmentShaderColorMultipliersLocation != -1)
			options.AddMacroDefinition("COLOR_MULTIPLIERS",
				std::to_string(m_properties.iFragmentShaderColorMultipliersLocation));
		if (m_properties.iFragmentShaderColorMultiplierAttributeCount)
			options.AddMacroDefinition("COLOR_MULTIPLIER_ATTRIB_COUNT",
				std::to_string(m_properties.iFragmentShaderColorMultiplierAttributeCount));

		const std::vector<char> sSourceCode =
			m_programDataManager.GetProgramFileContent(m_csShaderSourcePath + "\\UberShader.frag");

		shaderc::SpvCompilationResult module =
			compiler.CompileGlslToSpv(sSourceCode.data(), sSourceCode.size(), shaderc_fragment_shader, "UberShader.frag", options);

		if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
			std::cerr << module.GetErrorMessage();
			std::exit(EXIT_FAILURE);
		}

		return std::vector<uint32_t>(module.cbegin(), module.cend());
	}


	void UberShader::_CalculateMD5Hash(const char* _pSrc, size_t _uLen) {
		HCRYPTPROV hProv = 0;
		HCRYPTHASH hHash = 0;
		BYTE rgbHash[MD5LEN]{};
		const CHAR rgbDigits[] = "0123456789abcdef";

		if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
			DWORD dwStatus = GetLastError();
			std::cerr << "CryptAcquireContext failed: 0x" << std::setfill('0') << std::setw(8) <<
				std::hex << dwStatus << std::endl; std::cerr << "CryptAcquireContext failed: 0x" << std::hex << dwStatus << std::endl;
			return;
		}

		if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
			DWORD dwStatus = GetLastError();
			std::cerr << "CryptCreateHash failed: 0x" << std::setfill('0') << std::setw(8) << 
				std::hex << dwStatus << std::endl;
			return;
		}

		if (!CryptHashData(hHash, (const BYTE*)_pSrc, (DWORD)_uLen, 0)) {
			DWORD dwStatus = GetLastError();
			std::cerr << "CryptHashData failed: 0x" << std::setfill('0') << std::setw(8) <<
				std::hex << dwStatus << std::endl;
		}

		DWORD cbHash = MD5LEN; // md5 hash length in bytes
		if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
			std::stringstream ss;
			for (DWORD i = 0; i < cbHash; i++) {
				ss << rgbDigits[rgbHash[i] >> 4] << rgbDigits[rgbHash[i] & 0xff];
			}
			m_sMD5Hash = ss.str();
		}
		else {
			DWORD dwStatus = GetLastError();
			std::cerr << "CryptGetHashParam failed: " << std::setfill('0') << std::setw(8) <<
				std::hex << dwStatus << std::endl;
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


	PipelineCacheStatus UberShader::PipelineStatus() {
		const std::string sPipelineCacheFilePath =
			m_csPipelineCachePath + '\\' + m_sMD5Hash + ".cache";
		const std::string sVertexShaderSpirvFilePath =
			m_csSpirvBinaryPath + '\\' + m_sMD5Hash + ".vert";
		const std::string sGeometryShaderSpirvFilePath =
			m_csSpirvBinaryPath + '\\' + m_sMD5Hash + ".geom";
		const std::string sFragmentShaderSpirvFilePath =
			m_csSpirvBinaryPath + '\\' + m_sMD5Hash + ".frag";

		if (m_programDataManager.ExistsFile(sPipelineCacheFilePath))
			return CACHED_PIPELINE;

		// check if spir-v binaries exist
		// vertex shader
		if (m_programDataManager.ExistsFile(sVertexShaderSpirvFilePath) &&
			m_programDataManager.FileSize(sVertexShaderSpirvFilePath) % sizeof(uint32_t) == 0) 
		{
			m_bSpirvStatus[0] = true;

			if (m_properties.iVertexShaderNormalLocation != -1) {
				// geometry shader
				if (m_programDataManager.ExistsFile(sGeometryShaderSpirvFilePath) &&
					m_programDataManager.FileSize(sGeometryShaderSpirvFilePath) % sizeof(uint32_t) == 0)
				{
					m_bSpirvStatus[1] = true;
				}
			}
			else {
				m_bSpirvStatus[1] = true;
			}

			// fragment shader
			if (m_programDataManager.ExistsFile(sFragmentShaderSpirvFilePath) &&
				m_programDataManager.FileSize(sFragmentShaderSpirvFilePath) % sizeof(uint32_t) == 0)
			{
				m_bSpirvStatus[2] = true;
			}
		}

		if (m_bSpirvStatus[0] && m_bSpirvStatus[1] && m_bSpirvStatus[2])
			return SPIRV;
		else if (m_bSpirvStatus[0] || m_bSpirvStatus[1] || m_bSpirvStatus[2])
			return PARTIAL_SPIRV;
		else return NO_CACHE;
	}


	std::vector<uint32_t> UberShader::GetVertexShaderSpirv() {
		const std::string sVertexShaderFilePath =
			m_csSpirvBinaryPath + '\\' + m_sMD5Hash + ".vert";

		if (m_bSpirvStatus[0]) {
			auto input = m_programDataManager.GetProgramFileContent(sVertexShaderFilePath);
			std::vector<uint32_t> output(input.size() / sizeof(uint32_t));
			for (size_t i = 0; i < output.size(); i++)
				output[i] = reinterpret_cast<uint32_t*>(input.data())[i];
			
			return output;
		}
		else {
			return _CompileVertexShader();
		}
	}


	std::vector<uint32_t> UberShader::GetGeometryShaderSpirv() {
		const std::string sGeometryShaderFilePath =
			m_csSpirvBinaryPath + '\\' + m_sMD5Hash + ".geom";

		if (m_bSpirvStatus[1] && m_programDataManager.ExistsFile(sGeometryShaderFilePath)) {
			auto input = m_programDataManager.GetProgramFileContent(sGeometryShaderFilePath);
			std::vector<uint32_t> output(input.size() / sizeof(uint32_t));
			for (size_t i = 0; i < output.size(); i++)
				output[i] = reinterpret_cast<uint32_t*>(input.data())[i];

			return output;
		}
		else {
			return _CompileGeometryShader();
		}
	}


	std::vector<uint32_t> UberShader::GetFragmentShaderSpirv() {
		const std::string sFragmentShaderFilePath =
			m_csSpirvBinaryPath + '\\' + m_sMD5Hash + ".frag";

		if (m_bSpirvStatus[2]) {
			auto input = m_programDataManager.GetProgramFileContent(sFragmentShaderFilePath);
			std::vector<uint32_t> output(input.size() / sizeof(uint32_t));
			for (size_t i = 0; i < output.size(); i++)
				output[i] = reinterpret_cast<uint32_t*>(input.data())[i];

			return output;
		}
		else {
			return _CompileFragmentShader();
		}
	}
}