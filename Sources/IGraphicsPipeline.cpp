// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: IGraphicsPipeline.h - graphics pipeline interface class implementation
// author: Karl-Mihkel Ott

#include <cvar/SID.h>
#include "deng/IGraphicsPipeline.h"

namespace DENG
{
	IGraphicsPipeline::IGraphicsPipeline(const char* _szVertexShaderName, const char* _szFragmentShaderName, const char* _szGeometryShaderName) :
		m_sVertexShaderName(_szVertexShaderName),
		m_sGeometryShaderName(_szGeometryShaderName),
		m_sFragmentShaderName(_szFragmentShaderName)
	{
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