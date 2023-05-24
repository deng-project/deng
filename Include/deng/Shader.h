// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Shader.h - shader request class header
// author: Karl-Mihkel Ott

#ifndef SHADER_H
#define SHADER_H


#include <string>
#include <vector>
#include <unordered_map>

#include "trs/Points.h"
#include "trs/Vector.h"
#include "trs/Quaternion.h"
#include "trs/Matrix.h"

#include "das/DasStructures.h"

#include "deng/Api.h"
#include "deng/ProgramFilesManager.h"

#ifdef SHADER_CPP
	#include <iomanip>
	#include <iostream>
	#include <sstream>
	#include <chrono>
	#include <filesystem>

	#include <Windows.h>
	#include <Wincrypt.h>

	#include "shaderc/shaderc.hpp"

	#include "deng/Api.h"
	#include "deng/ErrorDefinitions.h"
	#include "deng/Exceptions.h"
#endif

namespace DENG {

	enum class RendererType { OPENGL, VULKAN, DIRECTX };

	enum PipelineCacheStatus { 
		UNKNOWN			=	0x0000, 
		NO_CACHE		=	0x0001, 
		PARTIAL_SPIRV	=	0x0002, 
		SPIRV			=	0x0004,
		VULKAN_CACHE	=	0x0008, 
		OPENGL_CACHE	=	0x0010,
		DIRECTX_CACHE	=	0x0020,
		CACHED_PIPELINE	=	0x0038,
		MAX_CACHE_ENUM	=	0xffff
	};

	class DENG_API Shader {
		private:
			const std::string m_csShaderSourcePath = "Shaders\\Source";
			const std::string m_csSpirvBinaryPath = "Shaders\\Binary";
			const std::string m_csPipelineCachePath = "Shaders\\PipelineCache";

			const std::string m_csSourceIdentifier;
			std::string m_sBinaryIdentifier;

			ProgramFilesManager m_programFilesManager;

			std::vector<std::pair<std::string, std::string>> m_vertexShaderMacros;
			std::vector<std::pair<std::string, std::string>> m_geometryShaderMacros;
			std::vector<std::pair<std::string, std::string>> m_fragmentShaderMacros;

		private:
			std::vector<uint32_t> _CompileVertexShaderToSpirv();
			std::vector<uint32_t> _CompileGeometryShaderToSpirv();
			std::vector<uint32_t> _CompileFragmentShaderToSpirv();

		public:
			// use relative source path, since ProgramFilesManager is used anyways
			Shader(const std::string& _sSourceIdentifier, const std::string& _sBinaryIdentifier = "");

			std::vector<uint32_t> ReadVertexShaderSpirv();
			std::vector<uint32_t> ReadGeometryShaderSpirv();
			std::vector<uint32_t> ReadFragmentShaderSpirv();

			std::vector<char> GetPipelineCache(RendererType _eRendererType);
			void CachePipeline(RendererType _eRendererType, const void* _pData, size_t _uLength);
			uint16_t GetPipelineCacheStatus();

			inline void SetBinaryIdentifier(const std::string& _sBinaryIdentifier) {
				m_sBinaryIdentifier = _sBinaryIdentifier;
			}

			inline void AddVertexShaderMacroDefinition(const std::string& _sDefinition) {
				m_vertexShaderMacros.push_back(std::make_pair(_sDefinition, ""));
			}

			inline void AddVertexShaderMacroDefinition(const std::string& _sDefinition, const std::string& _sValue) {
				m_vertexShaderMacros.push_back(std::make_pair(_sDefinition, _sValue));
			}

			inline void AddGeometryShaderMacroDefinition(const std::string& _sDefinition) {
				m_geometryShaderMacros.push_back(std::make_pair(_sDefinition, ""));
			}

			inline void AddGeometryShaderMacroDefinition(const std::string& _sDefinition, const std::string& _sValue) {
				m_geometryShaderMacros.push_back(std::make_pair(_sDefinition, _sValue));
			}

			inline void AddFragmentShaderMacroDefinition(const std::string& _sDefinition) {
				m_fragmentShaderMacros.push_back(std::make_pair(_sDefinition, ""));
			}

			inline void AddFragmentShaderMacroDefinition(const std::string& _sDefinition, const std::string& _sValue) {
				m_fragmentShaderMacros.push_back(std::make_pair(_sDefinition, _sValue));
			}
	};

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

	
	class UberShader : public Shader {
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
