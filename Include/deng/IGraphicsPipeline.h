// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: IGraphicsPipeline.h - graphics pipeline class implementation
// author: Karl-Mihkel Ott

#pragma once

#include <mutex>
#include <array>
#include <vector>
#include <cvar/SID.h>
#include "deng/Api.h"
#include "deng/ErrorDefinitions.h"
#include "deng/Exceptions.h"
#include "deng/ProgramFilesManager.h"

#include <bitset>

#ifdef GetBinaryType
#undef GetBinaryType
#endif

namespace DENG
{
	enum class ShaderFileType : uint8_t
	{
		Unknown,
		HLSL,
		GLSL
	};

	enum class GraphicsShaderType : uint8_t
	{
		Vertex,
		Geometry,
		Fragment
	};
	
	enum GraphicsShaderFlags_T : uint64_t
	{
		GraphicsShaderFlag_EnableCustomViewport			=	(1 << 0),
		GraphicsShaderFlag_EnableDepthTesting			=	(1 << 1),
		GraphicsShaderFlag_EnableStencilTesting			=	(1 << 2),
		GraphicsShaderFlag_EnableBlend					=   (1 << 3),
		GraphicsShaderFlag_UseSeparateAttributeStriding =	(1 << 4)
	};

	typedef uint64_t GraphicsShaderFlags;
	
	enum class GraphicsPipelineCullMode : uint64_t
	{
		None,
		Clockwise,
		CounterClockwise
	};

	
	enum class GraphicsPipelinePrimitiveMode : uint64_t
	{
		None,
		Points,
		Lines,
		Triangles
	};


	class DENG_API IGraphicsPipeline
	{
		protected:
			static const char* m_szVertexShaderSourcePath;
			static const char* m_szGeometryShaderSourcePath;
			static const char* m_szFragmentShaderSourcePath;

			static const char* m_szVertexShaderBinaryPath;
			static const char* m_szGeometryShaderBinaryPath;
			static const char* m_szFragmentShaderBinaryPath;

			static const char* m_szPipelineCachePath;

			ProgramFilesManager m_programFilesManager;
			std::string m_sVertexShaderName;
			std::string m_sGeometryShaderName;
			std::string m_sFragmentShaderName;

			/* Shader modules are identified with 64 bit crc hash */
			uint64_t m_uCrcHash = 0;
			uint64_t m_uXXHash64 = 0;

			 /* Properties are described by certain bits starting from LSB:
				  * 1 bit - enable custom viewport
				  * 1 bit - enable depth testing
				  * 1 bit - enable stencil testing
				  * 1 bit - enable blend
				  * 1 bit - 0: use interleaved striding; 1 - use separate attribute striding
				  * 2 bits - pipeline cull mode (0: None; 1: Clockwise; 2: CounterClockwise)
				  * 2 bits - primitive mode (0: None; 1: Points; 2: Lines; 3: Triangles)
			  */
			uint64_t m_uProperties = 0;

		protected:
			template <typename Compiler>
			std::vector<uint32_t> _CompileVertexShader(Compiler compiler)
			{
				// full source path
				std::string sSourcePath;
				int iSize = std::snprintf(nullptr, 0, m_szVertexShaderSourcePath, m_sVertexShaderName.c_str(), Compiler::GetSourceCodeExt());
				sSourcePath.resize(static_cast<size_t>(iSize));
				std::snprintf(sSourcePath.data(), sSourcePath.size(), m_szVertexShaderSourcePath, m_sVertexShaderName.c_str(), Compiler::GetSourceCodeExt());

				// throw an exception if source file does not exist
				if (!m_programFilesManager.ExistsFile(sSourcePath))
				{
					throw IOException("Vertex shader source file '" + sSourcePath + "' does not exist");
				}

				// full binary path
				std::string sBinaryPath;
				iSize = std::snprintf(nullptr, 0, m_szVertexShaderBinaryPath, Compiler::GetByteCodeDirectoryName(), m_sVertexShaderName.c_str(), Compiler::GetByteCodeExt());
				sBinaryPath.resize(static_cast<size_t>(iSize));
				std::snprintf(sBinaryPath.data(), sBinaryPath.size(), m_szVertexShaderBinaryPath, Compiler::GetByteCodeDirectoryName(), m_sVertexShaderName.c_str(), Compiler::GetByteCodeExt());

				// check if the binary file should be compiled
				if (!m_programFilesManager.ExistsFile(sBinaryPath) || m_programFilesManager.GetFileTimestamp(sSourcePath) > m_programFilesManager.GetFileTimestamp(sBinaryPath))
				{
					auto bin = std::move(compiler.CompileVertexShaderFile(sSourcePath));
					m_programFilesManager.WriteProgramFile(reinterpret_cast<const char*>(bin.data()), bin.size() * sizeof(uint32_t), sBinaryPath);
					return bin;
				}

				return std::vector<uint32_t>();
			}

			template <typename Compiler>
			std::vector<uint32_t> _CompileGeometryShader(Compiler compiler)
			{
				// full source path
				std::string sSourcePath;
				int iSize = std::snprintf(nullptr, 0, m_szGeometryShaderBinaryPath, m_sGeometryShaderName.c_str(), Compiler::GetSourceCodeExt());
				sSourcePath.resize(static_cast<size_t>(iSize));
				std::snprintf(sSourcePath.data(), sSourcePath.size(), m_szGeometryShaderSourcePath, m_sGeometryShaderName.c_str(), Compiler::GetSourceCodeExt());
				
				// if source exists then continue
				if (!m_programFilesManager.ExistsFile(sSourcePath))
				{
					// full binary path
					std::string sBinaryPath;
					iSize = std::snprintf(nullptr, 0, m_szGeometryShaderBinaryPath, Compiler::GetByteCodeDirectoryName(), m_sGeometryShaderName.c_str(), Compiler::GetByteCodeExt());
					sBinaryPath.resize(static_cast<size_t>(iSize));
					std::snprintf(sBinaryPath.data(), sBinaryPath.size(), m_szGeometryShaderBinaryPath, Compiler::GetByteCodeDirectoryName(), m_sGeometryShaderName.c_str(), Compiler::GetByteCodeExt());

					// check if the binary file should be compiled
					if (!m_programFilesManager.ExistsFile(sBinaryPath) || m_programFilesManager.GetFileTimestamp(sSourcePath) > m_programFilesManager.GetFileTimestamp(sBinaryPath))
					{
						auto bin = std::move(compiler.CompileGeometryShaderFile(sSourcePath));
						m_programFilesManager.WriteProgramFile(reinterpret_cast<const char*>(bin.data()), bin.size() * sizeof(uint32_t), sBinaryPath);
						return bin;
					}
				}

				return std::vector<uint32_t>();
			}

			template <typename Compiler>
			std::vector<uint32_t> _CompileFragmentShader(Compiler compiler)
			{
				// full source path
				std::string sSourcePath;
				int iSize = std::snprintf(nullptr, 0, m_szFragmentShaderSourcePath, m_sFragmentShaderName.c_str(), Compiler::GetSourceCodeExt());
				sSourcePath.resize(static_cast<size_t>(iSize));
				std::snprintf(sSourcePath.data(), sSourcePath.size(), m_szFragmentShaderSourcePath, m_sFragmentShaderName.c_str(), Compiler::GetSourceCodeExt());

				// throw an exception if source file does not exist
				if (!m_programFilesManager.ExistsFile(sSourcePath))
				{
					throw IOException("Fragment shader source file '" + sSourcePath + "' does not exist");
				}

				// full binary path
				std::string sBinaryPath;
				iSize = std::snprintf(nullptr, 0, m_szFragmentShaderBinaryPath, Compiler::GetByteCodeDirectoryName(), m_sFragmentShaderName.c_str(), Compiler::GetByteCodeExt());
				sBinaryPath.resize(static_cast<size_t>(iSize));
				std::snprintf(sBinaryPath.data(), sBinaryPath.size(), m_szFragmentShaderBinaryPath, Compiler::GetByteCodeDirectoryName(), m_sFragmentShaderName.c_str(), Compiler::GetByteCodeExt());

				// check if the binary file should be compiled
				if (!m_programFilesManager.ExistsFile(sBinaryPath) || m_programFilesManager.GetFileTimestamp(sSourcePath) > m_programFilesManager.GetFileTimestamp(sBinaryPath))
				{
					auto bin = std::move(compiler.CompileFragmentShaderFile(sSourcePath));
					m_programFilesManager.WriteProgramFile(reinterpret_cast<const char*>(bin.data()), bin.size() * sizeof(uint32_t), sBinaryPath);
					return bin;
				}

				return std::vector<uint32_t>();
			}

			template<typename Compiler>
			std::array<std::vector<uint32_t>, 3> _CompileByteCode()
			{
				static_assert(std::is_base_of<IGraphicsShaderCompiler, Compiler>::value,
							  "Compiler template must be derived from IGraphicsShaderCompiler");

				Compiler compiler;
				return std::array<std::vector<uint32_t>, 3>{
					std::move(_CompileVertexShader(compiler)),
					std::move(_CompileGeometryShader(compiler)),
					std::move(_CompileFragmentShader(compiler))
				};
			}

			const char* _CreateUUID();

		public:
			IGraphicsPipeline(const char* _szVertexShaderName, const char* _szFragmentShaderName, const char* _szGeometryShaderName = "");
			void CalculatePipelineKeyHash();
			virtual void CreatePipeline() = 0;

			inline uint64_t GetPipelineCacheKey() 
			{ 
				return m_uCrcHash; 
			}

			inline const std::string& GetVertexShaderName()
			{
				return m_sVertexShaderName;
			}

			inline const std::string& GetGeometryShaderName()
			{
				return m_sGeometryShaderName;
			}

			inline const std::string& GetFragmentShaderName()
			{
				return m_sFragmentShaderName;
			}

			inline bool IsFlagSet(GraphicsShaderFlags _flags)
			{
				return (m_uProperties & _flags) == _flags;
			}

			
			inline void SetFlags(GraphicsShaderFlags _flags, bool _bEnable = true)
			{
				if (_bEnable)
				{
					m_uCrcHash |= _flags;
				}
				else
				{
					m_uCrcHash &= ~_flags;
				}
			}

			
			inline GraphicsPipelineCullMode GetPipelineCullMode()
			{
				uint64_t uCullMode = (m_uCrcHash >> 5 & 0xff);
				return static_cast<GraphicsPipelineCullMode>(uCullMode);
			}

			
			inline void SetPipelineCullMode(GraphicsPipelineCullMode _pipelineCullMode)
			{
				// disable previous pipeline cull mode
				m_uCrcHash &= ~(static_cast<uint64_t>(_pipelineCullMode) << 5);
				m_uCrcHash |= (static_cast<uint64_t>(_pipelineCullMode) << 5);
			}

			
			inline GraphicsPipelinePrimitiveMode GetPipelinePrimitiveMode()
			{
				uint64_t uPrimitiveMode = (m_uCrcHash >> 7 & 0xff);
				return static_cast<GraphicsPipelinePrimitiveMode>(uPrimitiveMode);
			}


			inline void SetPipelinePrimitiveMode(GraphicsPipelinePrimitiveMode _pipelinePrimitiveMode)
			{
				m_uCrcHash &= ~(static_cast<uint64_t>(_pipelinePrimitiveMode) << 7);
				m_uCrcHash |= (static_cast<uint64_t>(_pipelinePrimitiveMode) << 7);
			}
	};
}