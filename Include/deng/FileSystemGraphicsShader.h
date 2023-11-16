// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: FileSystemGraphicsShader.h - file system graphics shader class header
// author: Karl-Mihkel Ott

#ifndef FILE_SYSTEM_SHADER_H
#define FILE_SYSTEM_SHADER_H

#include <string>
#include <vector>

#include "deng/Api.h"
#include "deng/ProgramFilesManager.h"
#include "deng/IGraphicsShader.h"
#include "deng/IGraphicsShaderCompiler.h"

#ifdef FILE_SYSTEM_SHADER_CPP
#endif

namespace DENG {
	
	class DENG_API FileSystemGraphicsShader : public IGraphicsShader {
		private:
			const std::string_view m_csVertexShaderSourcePath = "Shaders/Source/Vertex/%s.%s";
			const std::string_view m_csGeometryShaderSourcePath = "Shaders/Source/Geometry/%s.%s";
			const std::string_view m_csFragmentShaderSourcePath = "Shaders/Source/Fragment/%s.%s";

			const std::string_view m_csVertexShaderByteCodePath = "Shaders/ByteCode/%s/Vertex/%s.%s.%s";
			const std::string_view m_csGeometryShaderByteCodePath = "Shaders/ByteCode/%s/Geometry/%s.%s.%s";
			const std::string_view m_csFragmentShaderByteCodePath = "Shaders/ByteCode/%s/Fragment/%s.%s.%s";

			const std::string_view m_csPipelineCachePath = "Shaders/GraphicsPipelineCache/%s/%s.cache";

			// source identifiers
			const std::string m_csVertexShaderSourceName;
			const std::string m_csGeometryShaderSourceName;
			const std::string m_csFragmentShaderSourceName;
			
			// spirv identifiers
			const std::string m_csVertexShaderByteCodeName;
			const std::string m_csGeometryShaderByteCodeName;
			const std::string m_csFragmentShaderByteCodeName;

			IGraphicsShaderCompiler* m_pCompiler = nullptr;
			ProgramFilesManager m_programFilesManager;

			std::vector<std::pair<std::string, std::string>> m_vertexShaderMacros;
			std::vector<std::pair<std::string, std::string>> m_geometryShaderMacros;
			std::vector<std::pair<std::string, std::string>> m_fragmentShaderMacros;

		private:
			void _WriteShaderByteCodeToFile(const std::string& _csPath, const std::vector<uint32_t>& _byteCode) const;

		public:
			// If spirv names are unspecified then source names are going to be used
			FileSystemGraphicsShader(IGraphicsShaderCompiler* _pGraphicsShaderCompiler,
									 const std::string& _sVertexShaderSourceName,
									 const std::string& _sGeometryShaderSourceName,
									 const std::string& _sFragmentShaderSourceName,
									 const std::string& _sVertexShaderSpirvName = "",
									 const std::string& _sGeometryShaderSpirvName = "",
									 const std::string& _sFragmentShaderSprivName = "");
			~FileSystemGraphicsShader();

			virtual std::vector<uint32_t> GetVertexShaderByteCode() const override;
			virtual std::vector<uint32_t> GetGeometryShaderByteCode() const override;
			virtual std::vector<uint32_t> GetFragmentShaderByteCode() const override;

			virtual std::vector<char> GetPipelineCache() const override;
			virtual void CachePipeline(const void* _pData, size_t _uLength) const override;
			virtual PipelineCacheStatus GetPipelineCacheStatus() const override;

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
