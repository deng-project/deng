// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: IGraphicsPipeline.h - graphics pipeline interface class implementation
// author: Karl-Mihkel Ott

#include <cvar/SID.h>
#include "deng/IGraphicsPipeline.h"

namespace DENG
{
	const char* IGraphicsPipeline::m_szVertexShaderSourcePath = "Shaders/Source/Vertex/%s.%s";
	const char* IGraphicsPipeline::m_szGeometryShaderSourcePath = "Shaders/Source/Geometry/%s.%s";
	const char* IGraphicsPipeline::m_szFragmentShaderSourcePath = "Shaders/Source/Fragment/%s.%s";

	const char* IGraphicsPipeline::m_szVertexShaderBinaryPath = "Shaders/Binary/%s/Vertex/%s.%s";
	const char* IGraphicsPipeline::m_szGeometryShaderBinaryPath = "Shaders/Binary/%s/Geometry/%s.%s";
	const char* IGraphicsPipeline::m_szFragmentShaderBinaryPath = "Shader/Binary/%s/Fragment/%s.%s";

	const char* IGraphicsPipeline::m_szPipelineCachePath = "Shader/PipelineCache/%s/%s.CACHE";

	IGraphicsPipeline::IGraphicsPipeline(const char* _szVertexShaderName, const char* _szFragmentShaderName, const char* _szGeometryShaderName) :
		m_sVertexShaderName(_szVertexShaderName),
		m_sGeometryShaderName(_szGeometryShaderName),
		m_sFragmentShaderName(_szFragmentShaderName)
	{
	}


	const char* IGraphicsPipeline::_CreateUUID()
	{
		static char szUUID[38] = {};
		std::snprintf(szUUID, 38, "%08llX-%04llX-%04llX-%04llX-%12llX",
					  m_uCrcHash >> 32, (m_uCrcHash >> 16) & 0xffff, m_uCrcHash & 0xffff,
					  m_uXXHash64 >> 48, m_uXXHash64 & 0xffffffffffff);

		return szUUID;
	}


#define HASH_COMBINE(lhs, rhs) (lhs ^ (rhs + 0x517cc1b727220a95 + (lhs << 6) + (lhs >> 2)))
	void IGraphicsPipeline::CalculatePipelineKeyHash()
	{
		m_uCrcHash = cvar::RuntimeCrc64(reinterpret_cast<const char*>(m_uCrcHash));
		m_uCrcHash = HASH_COMBINE(m_uCrcHash, cvar::RuntimeCrc64(m_sVertexShaderName));
		m_uCrcHash = HASH_COMBINE(m_uCrcHash, cvar::RuntimeCrc64(m_sGeometryShaderName));
		m_uCrcHash = HASH_COMBINE(m_uCrcHash, cvar::RuntimeCrc64(m_sFragmentShaderName));
	}
}