// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: UberShader.h - uber shader class header
// author: Karl-Mihkel Ott

#ifndef UBER_SHADER_H
#define UBER_SHADER_H

#include <iostream>
#include <string>
#include <vector>

#include "trs/Points.h"
#include "trs/Vector.h"
#include "trs/Matrix.h"
#include "trs/Quaternion.h"

#include "das/DasStructures.h"
#include "deng/FileSystemShader.h"

#ifdef UBER_SHADER_CPP
	#include <sstream>
	#include <iomanip>

	#include "deng/Exceptions.h"
	#define MD5LEN 16

	#include <Windows.h>
	#include <Wincrypt.h>
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


	class UberShader : public FileSystemShader {
		private:
			const std::string m_csPipelineCachePath = "Shaders\\PipelineCache\\GenericUberShader";
			const std::string m_csShaderSourcePath = "Shaders\\Source\\GenericUberShader";
			const std::string m_csSpirvBinaryPath = "Shaders\\Spriv\\GenericUberShader";

			UberShaderProperties m_properties;
			std::string m_sMD5Hash;
			bool m_bSpirvStatus[3] = { false, false, false };

		private:
			void _CalculateMD5Hash(const char* _pSrc, size_t _uLen);	// based on wincrypt api
			void _CreateVertexShaderProperties(const Libdas::DasModel& _model, uint32_t _uMeshId);
			void _CreateGeometryShaderProperties(const Libdas::DasModel& _model, uint32_t _uMeshId);
			void _CreateFragmentShaderProperties(const Libdas::DasModel& _model, uint32_t _uMeshId);

			void _AddVertexShaderMacroDefinitions();
			void _AddGeometryShaderMacroDefinitions();
			void _AddFragmentShaderMacroDefinitions();

		public:
			UberShader(const Libdas::DasModel& _model, uint32_t _uMeshId);
	};
}

#endif
