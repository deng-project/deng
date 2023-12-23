// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: IByteCodeToolset.h - Shader byte code tooling interface class implementation
// author: Karl-Mihkel Ott

#include "deng/IByteCodeToolset.h"

namespace DENG
{
	void IByteCodeToolset::_CalculateInputStrides()
	{
		m_inputStrides.reserve(m_inputFormats.size());

		for (ShaderInputFormat format : m_inputFormats)
		{
			switch (format)
			{
				case DENG::ShaderInputFormat::UNDEFINED:
					m_inputStrides.push_back(0);
					break;

				case DENG::ShaderInputFormat::R16_UINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(uint16_t)));
					break;
				
				case DENG::ShaderInputFormat::R16_SINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(int16_t)));
					break;
				
				case DENG::ShaderInputFormat::R16_SFLOAT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(int16_t)));
					break;
				
				case DENG::ShaderInputFormat::R16G16_UINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(uint16_t) * 2));
					break;
				
				case DENG::ShaderInputFormat::R16G16_SINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(int16_t) * 2));
					break;
				
				case DENG::ShaderInputFormat::R16G16_SFLOAT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(int16_t) * 2));
					break;
				
				case DENG::ShaderInputFormat::R16G16B16_UINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(uint16_t) * 3));
					break;
				
				case DENG::ShaderInputFormat::R16G16B16_SINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(int16_t) * 3));
					break;
				
				case DENG::ShaderInputFormat::R16G16B16_SFLOAT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(int16_t) * 3));
					break;
				
				case DENG::ShaderInputFormat::R16G16B16A16_UINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(uint16_t) * 4));
					break;
				
				case DENG::ShaderInputFormat::R16G16B16A16_SINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(int16_t) * 4));
					break;
				
				case DENG::ShaderInputFormat::R16G16B16A16_SFLOAT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(int16_t) * 4));
					break;
				
				case DENG::ShaderInputFormat::R32_UINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(uint32_t)));
					break;
				
				case DENG::ShaderInputFormat::R32_SINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(int32_t)));
					break;
				
				case DENG::ShaderInputFormat::R32_SFLOAT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(float)));
					break;
				
				case DENG::ShaderInputFormat::R32G32_UINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(uint32_t) * 2));
					break;
				
				case DENG::ShaderInputFormat::R32G32_SINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(int32_t) * 2));
					break;
				
				case DENG::ShaderInputFormat::R32G32_SFLOAT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(float) * 2));
					break;
				
				case DENG::ShaderInputFormat::R32G32B32_UINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(uint32_t) * 3));
					break;
				
				case DENG::ShaderInputFormat::R32G32B32_SINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(int32_t) * 3));
					break;
				
				case DENG::ShaderInputFormat::R32G32B32_SFLOAT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(float) * 3));
					break;
				
				case DENG::ShaderInputFormat::R32G32B32A32_UINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(uint32_t) * 4));
					break;
				
				case DENG::ShaderInputFormat::R32G32B32A32_SINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(int32_t) * 4));
					break;
				
				case DENG::ShaderInputFormat::R32G32B32A32_SFLOAT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(float) * 4));
					break;
				
				case DENG::ShaderInputFormat::R64_UINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(uint64_t)));
					break;
				
				case DENG::ShaderInputFormat::R64_SINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(int64_t)));
					break;
				
				case DENG::ShaderInputFormat::R64_SFLOAT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(double)));
					break;
				
				case DENG::ShaderInputFormat::R64G64_UINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(uint64_t) * 2));
					break;
				
				case DENG::ShaderInputFormat::R64G64_SINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(int64_t) * 2));
					break;
				
				case DENG::ShaderInputFormat::R64G64_SFLOAT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(double) * 2));
					break;
				
				case DENG::ShaderInputFormat::R64G64B64_UINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(uint64_t) * 3));
					break;
				
				case DENG::ShaderInputFormat::R64G64B64_SINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(int64_t) * 3));
					break;
				
				case DENG::ShaderInputFormat::R64G64B64_SFLOAT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(double) * 3));
					break;
				
				case DENG::ShaderInputFormat::R64G64B64A64_UINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(uint64_t) * 4));
					break;
				
				case DENG::ShaderInputFormat::R64G64B64A64_SINT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(int64_t) * 4));
					break;
				
				case DENG::ShaderInputFormat::R64G64B64A64_SFLOAT:
					m_inputStrides.push_back(static_cast<uint32_t>(sizeof(double) * 4));
					break;
				
				default:
					break;
			}
		}
	}
}