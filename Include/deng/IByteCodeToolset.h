// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: IByteCodeToolset.h - Shader byte code tooling interface class header
// author: Karl-Mihkel Ott

#pragma once

#include "deng/Api.h"
#include <vector>
#include <unordered_map>

namespace DENG
{
	enum class BoundResourceType : uint8_t
	{
		None,
		Buffer,
		StorageBuffer,
		ImageSampler,
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
		BoundResourceType type = BoundResourceType::None;
		ShaderStageBits uShaderStage = 0;
		uint32_t uSize = 0;

		inline bool operator==(const BoundResource& _other) const
		{
			return this->type == _other.type && this->uShaderStage == _other.uShaderStage && this->uSize == _other.uSize;
		}
	};

	enum class ShaderInputFormat
	{
		UNDEFINED = 0,
		R16_UINT = 74, // = VK_FORMAT_R16_UINT
		R16_SINT = 75, // = VK_FORMAT_R16_SINT
		R16_SFLOAT = 76, // = VK_FORMAT_R16_SFLOAT
		R16G16_UINT = 81, // = VK_FORMAT_R16G16_UINT
		R16G16_SINT = 82, // = VK_FORMAT_R16G16_SINT
		R16G16_SFLOAT = 83, // = VK_FORMAT_R16G16_SFLOAT
		R16G16B16_UINT = 88, // = VK_FORMAT_R16G16B16_UINT
		R16G16B16_SINT = 89, // = VK_FORMAT_R16G16B16_SINT
		R16G16B16_SFLOAT = 90, // = VK_FORMAT_R16G16B16_SFLOAT
		R16G16B16A16_UINT = 95, // = VK_FORMAT_R16G16B16A16_UINT
		R16G16B16A16_SINT = 96, // = VK_FORMAT_R16G16B16A16_SINT
		R16G16B16A16_SFLOAT = 97, // = VK_FORMAT_R16G16B16A16_SFLOAT
		R32_UINT = 98, // = VK_FORMAT_R32_UINT
		R32_SINT = 99, // = VK_FORMAT_R32_SINT
		R32_SFLOAT = 100, // = VK_FORMAT_R32_SFLOAT
		R32G32_UINT = 101, // = VK_FORMAT_R32G32_UINT
		R32G32_SINT = 102, // = VK_FORMAT_R32G32_SINT
		R32G32_SFLOAT = 103, // = VK_FORMAT_R32G32_SFLOAT
		R32G32B32_UINT = 104, // = VK_FORMAT_R32G32B32_UINT
		R32G32B32_SINT = 105, // = VK_FORMAT_R32G32B32_SINT
		R32G32B32_SFLOAT = 106, // = VK_FORMAT_R32G32B32_SFLOAT
		R32G32B32A32_UINT = 107, // = VK_FORMAT_R32G32B32A32_UINT
		R32G32B32A32_SINT = 108, // = VK_FORMAT_R32G32B32A32_SINT
		R32G32B32A32_SFLOAT = 109, // = VK_FORMAT_R32G32B32A32_SFLOAT
		R64_UINT = 110, // = VK_FORMAT_R64_UINT
		R64_SINT = 111, // = VK_FORMAT_R64_SINT
		R64_SFLOAT = 112, // = VK_FORMAT_R64_SFLOAT
		R64G64_UINT = 113, // = VK_FORMAT_R64G64_UINT
		R64G64_SINT = 114, // = VK_FORMAT_R64G64_SINT
		R64G64_SFLOAT = 115, // = VK_FORMAT_R64G64_SFLOAT
		R64G64B64_UINT = 116, // = VK_FORMAT_R64G64B64_UINT
		R64G64B64_SINT = 117, // = VK_FORMAT_R64G64B64_SINT
		R64G64B64_SFLOAT = 118, // = VK_FORMAT_R64G64B64_SFLOAT
		R64G64B64A64_UINT = 119, // = VK_FORMAT_R64G64B64A64_UINT
		R64G64B64A64_SINT = 120, // = VK_FORMAT_R64G64B64A64_SINT
		R64G64B64A64_SFLOAT = 121, // = VK_FORMAT_R64G64B64A64_SFLOAT
	};

	class DENG_API IByteCodeToolset
	{
		protected:
			const std::vector<uint32_t>& m_vertexShaderBytes;
			const std::vector<uint32_t>& m_geometryShaderBytes;
			const std::vector<uint32_t>& m_fragmentShaderBytes;
			std::vector<ShaderInputFormat> m_inputFormats;
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

			inline std::vector<ShaderInputFormat>& GetShaderInputFormats()
			{
				return m_inputFormats;
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
