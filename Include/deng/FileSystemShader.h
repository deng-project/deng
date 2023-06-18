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
			std::vector<uint32_t> _CompileVertexShaderToSpirv() const;
			std::vector<uint32_t> _CompileGeometryShaderToSpirv() const;
			std::vector<uint32_t> _CompileFragmentShaderToSpirv() const;

		public:
			// use relative source path, since ProgramFilesManager is used anyways
			FileSystemShader(const std::string& _sSourceIdentifier, const std::string& _sBinaryIdentifier = "");

			virtual std::vector<uint32_t> GetVertexShaderSpirv() const override;
			virtual std::vector<uint32_t> GetGeometryShaderSpirv() const override;
			virtual std::vector<uint32_t> GetFragmentShaderSpirv() const override;

			virtual std::vector<char> GetPipelineCache(RendererType _eRendererType) const override;
			virtual void CachePipeline(RendererType _eRendererType, const void* _pData, size_t _uLength) const override;
			virtual PipelineCacheStatusBits GetPipelineCacheStatus() const override;

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
}

#endif
