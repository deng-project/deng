// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: UberShader.h - UberShader request class header
// author: Karl-Mihkel Ott

#ifndef UBER_SHADER_H
#define UBER_SHADER_H

#ifdef UBER_SHADER_CPP
	#include <string>
	#include <vector>
	#include <iomanip>
	#include <iostream>
	#include <sstream>

	#include <Windows.h>
	#include <Wincrypt.h>

	#include "shaderc/shaderc.hpp"

	#include "trs/Points.h"
	#include "trs/Vector.h"
	#include "trs/Quaternion.h"
	#include "trs/Matrix.h"

	#include "das/DasStructures.h"
	#include "deng/ProgramFilesManager.h"
#endif

namespace DENG {
	// md5 hashable struct
	struct UberShaderProperties {
		// vertex shader properties
		int32_t iVertexShaderNormalLocation = -1;
		int32_t iVertexShaderNormalOutputLocation = -1;
		int32_t iVertexShaderTangentLocation = -1;
		int32_t iVertexShaderTangentOutputLocation = -1;
		int32_t iVertexShaderUVCoordinatesLocation = -1;
		int32_t iVertexShaderUVAttributeCount = 0;
		int32_t iVertexShaderUVCoordinatesOutputLocation = -1;
		int32_t iVertexShaderColorMultiplierLocation = -1;
		int32_t iVertexShaderColorMultiplierAttributeCount = 0;
		int32_t iVertexShaderColorMultiplierOutputLocation = -1;
		bool bVertexShaderUseJointTransforms = false;
		int32_t iVertexShaderJointIndicesLocation = -1;
		int32_t iVertexShaderJointWeightsLocation = -1;
		int32_t iVertexShaderJointSetCount = 0;
		int32_t iVertexShaderJointCount = 0;
		bool bVertexShaderUseMorphTargets = false;
		int32_t iVertexShaderMorphSetCount = 0;
		int32_t iVertexShaderMorphTargetCount = 0;
		int32_t iVertexShaderMorphPositionsLocation = -1;
		int32_t iVertexShaderMorphNormalsLocation = -1;
		int32_t iVertexShaderMorphTangentsLocation = -1;

		// geometry shader properties;
		bool bUseGeometryShader = false;
		int32_t iGeometryShaderUVCoordinatesLocation = -1;
		int32_t iGeometryShaderUVAttributeCount = 0;
		int32_t iGeometryShaderUVCoordinatesOutputLocation = -1;
		int32_t iGeometryShaderColorMultipliersLocation = -1;
		int32_t iGeometryShaderColorMultipliersAttributeCount = 0;
		int32_t iGeometryShaderColorMultipliersOutputLocation = -1;

		// fragment shader
		int32_t iFragmentShaderVertexNormalLocation = -1;
		int32_t iFragmentShaderVertexTangentLocation = -1;
		int32_t iFragmentShaderUVCoordinatesLocation = -1;
		int32_t iFragmentShaderUVAttributeCount = 0;
		int32_t iFragmentShaderColorMultipliersLocation = -1;
		int32_t iFragmentShaderColorMultiplierAttributeCount = 0;
	};

	enum PipelineCacheStatus { UNKNOWN, NO_CACHE, PARTIAL_SPIRV, SPIRV, CACHED_PIPELINE };

	class UberShader {
		private:
			const std::string m_csPipelineCachePath = "Shaders\\PipelineCache\\GenericUberShader";
			const std::string m_csShaderSourcePath = "Shaders\\Source\\GenericUberShader";
			const std::string m_csSpirvBinaryPath = "Shaders\\Spriv\\GenericUberShader";

			ProgramFilesManager m_programDataManager;
			UberShaderProperties m_properties;
			std::string m_sMD5Hash;
			bool m_bSpirvStatus[3] = { false, false, false };

		private:
			std::vector<uint32_t> _CompileVertexShader();
			std::vector<uint32_t> _CompileGeometryShader();
			std::vector<uint32_t> _CompileFragmentShader();

			void _CalculateMD5Hash(const char* _pSrc, size_t _uLen);	// based on wincrypt api
			void _CreateVertexShaderProperties(const Libdas::DasModel& _model, uint32_t _uMeshId);
			void _CreateGeometryShaderProperties(const Libdas::DasModel& _model, uint32_t _uMeshId);
			void _CreateFragmentShaderProperties(const Libdas::DasModel& _model, uint32_t _uMeshId);

		public:
			UberShader(const Libdas::DasModel& _model, uint32_t _uMeshId);
			PipelineCacheStatus PipelineStatus();
			
			inline std::vector<char> GetPipelineCache() {
				return m_programDataManager.GetProgramFileContent(m_csPipelineCachePath + '\\' + m_sMD5Hash + ".cache");
			}
			
			std::vector<uint32_t> GetVertexShaderSpirv();
			std::vector<uint32_t> GetGeometryShaderSpirv();
			std::vector<uint32_t> GetFragmentShaderSpirv();

	};
}

#endif
