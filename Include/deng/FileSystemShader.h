// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Shader.h - file system shader class header
// author: Karl-Mihkel Ott

#ifndef FILE_SYSTEM_SHADER_H
#define FILE_SYSTEM_SHADER_H

#include <string>
#include <vector>

#include "deng/Api.h"
#include "deng/ProgramFilesManager.h"
#include "deng/IShader.h"

#ifdef FILE_SYSTEM_SHADER_CPP
	#include <iomanip>
	#include <iostream>
	#include <sstream>
	#include <chrono>
	#include <filesystem>

	#include "shaderc/shaderc.hpp"

	#include "deng/Api.h"
	#include "deng/ErrorDefinitions.h"
	#include "deng/Exceptions.h"
#endif

namespace DENG {
	
	class DENG_API FileSystemShader : public IShader {
		private:
			const std::string_view m_csVertexShaderSourcePath = "Shaders/Source/Vertex";
			const std::string_view m_csGeometryShaderSourcePath = "Shaders/Source/Geometry";
			const std::string_view m_csFragmentShaderSourcePath = "Shaders/Source/Fragment";

			const std::string_view m_csVertexShaderSpirvPath = "Shaders/SPV/Vertex";
			const std::string_view m_csGeometryShaderSpirvPath = "Shaders/SPV/Geometry";
			const std::string_view m_csFragmentShaderSpirvPath = "Shaders/SPV/Fragment";

			const std::string m_csPipelineCachePath = "Shaders/PipelineCache";

			// source identifiers
			const std::string m_csVertexShaderSourceName;
			const std::string m_csGeometryShaderSourceName;
			const std::string m_csFragmentShaderSourceName;
			
			// spirv identifiers
			const std::string m_csVertexShaderSpirvName;
			const std::string m_csGeometryShaderSpirvName;
			const std::string m_csFragmentShaderSpirvName;

			ProgramFilesManager m_programFilesManager;

			std::vector<std::pair<std::string, std::string>> m_vertexShaderMacros;
			std::vector<std::pair<std::string, std::string>> m_geometryShaderMacros;
			std::vector<std::pair<std::string, std::string>> m_fragmentShaderMacros;

		private:
			std::vector<uint32_t>&& _CompileVertexShaderToSpirv() const;
			std::vector<uint32_t>&& _CompileGeometryShaderToSpirv() const;
			std::vector<uint32_t>&& _CompileFragmentShaderToSpirv() const;

		public:
			// If spirv names are unspecified then source names are going to be used
			FileSystemShader(const std::string& _sVertexShaderSourceName,
							 const std::string& _sGeometryShaderSourceName,
							 const std::string& _sFragmentShaderSourceName,
							 const std::string& _sVertexShaderSpirvName = "",
							 const std::string& _sGeometryShaderSpirvName = "",
							 const std::string& _sFragmentShaderSprivName = "");

			virtual std::vector<uint32_t>&& GetVertexShaderSpirv() const override;
			virtual std::vector<uint32_t>&& GetGeometryShaderSpirv() const override;
			virtual std::vector<uint32_t>&& GetFragmentShaderSpirv() const override;

			virtual std::vector<char>&& GetPipelineCache(RendererType _eRendererType) const override;
			virtual void CachePipeline(RendererType _eRendererType, const void* _pData, size_t _uLength) const override;
			virtual PipelineCacheStatusBits GetPipelineCacheStatus() const override;

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
}

#endif
