// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: IGraphicsPipeline.h - graphics pipeline class implementation
// author: Karl-Mihkel Ott

#pragma once

#include <mutex>
#include <vector>
#include <cvar/SID.h>
#include "deng/Api.h"
#include "deng/ErrorDefinitions.h"

#include <bitset>

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
		GraphicsShaderFlag_EnablePushConstants			=	(1 << 0),
		GraphicsShaderFlag_EnableCustomViewport			=	(1 << 1),
		GraphicsShaderFlag_EnableDepthTesting			=	(1 << 2),
		GraphicsShaderFlag_EnableStencilTesting			=	(1 << 3),
		GraphicsShaderFlag_EnableBlend					=   (1 << 4),
		GraphicsShaderFlag_UseSeparateAttributeStriding =	(1 << 5)
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
			std::string m_sVertexShaderName;
			std::string m_sGeometryShaderName;
			std::string m_sFragmentShaderName;

			/* Shader modules are identified with 64 bit crc hash */
			uint64_t m_uCrcHash = 0;

			 /* Properties are described by certain bits starting from LSB:
				  * 1 bit - enable push constants
				  * 1 bit - enable custom viewport
				  * 1 bit - enable depth testing
				  * 1 bit - enable stencil testing
				  * 1 bit - enable blend
				  * 1 bit - 0: use interleaved striding; 1 - use separate attribute striding
				  * 2 bits - pipeline cull mode (0: None; 1: Clockwise; 2: CounterClockwise)
				  * 2 bits - primitive mode (0: None; 1: Points; 2: Lines; 3: Triangles)
			  */
			uint64_t m_uProperties = 0;

		public:
			IGraphicsPipeline(const char* _szVertexShaderName, const char* _szFragmentShaderName, const char* _szGeometryShaderName = "");
			void CalculatePipelineKeyHash();

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
				uint64_t uCullMode = (m_uCrcHash >> 6 & 0xff);
				return static_cast<GraphicsPipelineCullMode>(uCullMode);
			}

			
			inline void SetPipelineCullMode(GraphicsPipelineCullMode _pipelineCullMode)
			{
				// disable previous pipeline cull mode
				m_uCrcHash &= ~(static_cast<uint64_t>(_pipelineCullMode) << 6);
				m_uCrcHash |= (static_cast<uint64_t>(_pipelineCullMode) << 6);
			}

			
			inline GraphicsPipelinePrimitiveMode GetPipelinePrimitiveMode()
			{
				uint64_t uPrimitiveMode = (m_uCrcHash >> 8 & 0xff);
				return static_cast<GraphicsPipelinePrimitiveMode>(uPrimitiveMode);
			}


			inline void SetPipelinePrimitiveMode(GraphicsPipelinePrimitiveMode _pipelinePrimitiveMode)
			{
				m_uCrcHash &= ~(static_cast<uint64_t>(_pipelinePrimitiveMode) << 8);
				m_uCrcHash |= (static_cast<uint64_t>(_pipelinePrimitiveMode) << 8);
			}
	};
}