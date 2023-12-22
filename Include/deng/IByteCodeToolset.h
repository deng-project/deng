// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: IByteCodeToolset.h - Shader byte code tooling interface class header
// author: Karl-Mihkel Ott

#pragma once

#include "deng/Api.h"
#include <vector>

namespace DENG
{
	enum class BoundResourceType : uint8_t
	{
		None,
		Buffer,
		StorageBuffer,
		ImageSampler2D,
		ImageSampler3D
	};


	enum ShaderStageBits_T : uint8_t
	{
		ShaderStageBit_None = 0,
		ShaderStageBit_Vertex = (1 << 0),
		ShaderStageBit_Geometry = (1 << 1),
		ShaderStageBit_Fragment = (1 << 2)
	};

	typedef uint8_t ShaderStageBits;

	struct BoundResource
	{
		uint32_t uBinding;
		BoundResourceType type;
		ShaderStageBits uShaderStage;
	};

	class DENG_API IByteCodeToolset
	{
		protected:
			const std::vector<uint32_t>& m_vertexShaderBytes;
			const std::vector<uint32_t>& m_geometryShaderBytes;
			const std::vector<uint32_t>& m_fragmentShaderBytes;

			std::vector<BoundResource> m_boundResources;
			bool m_bHasPushConstants = false;
			uint32_t m_uPushConstantLength = 0;
			ShaderStageBits m_uPushConstantStageBits = 0;

		protected:
			inline bool _IsLittleEndian()
			{
				uint32_t n = 1;
				if (*(char*)&n == 1)
					return true;
				return false;
			}

		public:
			IByteCodeToolset(const std::vector<uint32_t>& _vertexBytes, const std::vector<uint32_t>& _geometryBytes, const std::vector<uint32_t>& _fragmentBytes) :
				m_vertexShaderBytes(_vertexBytes),
				m_geometryShaderBytes(_geometryBytes),
				m_fragmentShaderBytes(_fragmentBytes)
			{
			}

			inline std::vector<BoundResource>& GetBoundResources()
			{
				return m_boundResources;
			}

			inline const std::vector<BoundResource>& GetBoundResources() const
			{
				return m_boundResources;
			}

			inline bool HasPushConstants() const
			{
				return m_bHasPushConstants;
			}

			inline uint32_t GetEffectivePushConstantLength() const
			{
				return m_uPushConstantLength;
			}

			inline ShaderStageBits GetPushConstantStageBits() const
			{
				return m_uPushConstantStageBits;
			}
	};
}
