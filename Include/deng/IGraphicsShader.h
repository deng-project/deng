// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: IGraphicsShader.h - graphics shader interface class header
// author: Karl-Mihkel Ott

#pragma once

#include <mutex>
#include <vector>
#include <cvar/SID.h>
#include "deng/Api.h"
#include "deng/ErrorDefinitions.h"
#include "deng/IShader.h"

#include <bitset>

namespace DENG
{

	enum class VertexAttributeType
	{
		None,
		Float, Double, Byte, UnsignedByte, Short, UnsignedShort, Int, UnsignedInt,
		Vec2_Float, Vec2_Double, Vec2_Byte, Vec2_UnsignedByte, Vec2_Short, Vec2_UnsignedShort, Vec2_Int, Vec2_UnsignedInt,
		Vec3_Float, Vec3_Double, Vec3_Byte, Vec3_UnsignedByte, Vec3_Short, Vec3_UnsignedShort, Vec3_Int, Vec3_UnsignedInt,
		Vec4_Float, Vec4_Double, Vec4_Byte, Vec4_UnsignedByte, Vec4_Short, Vec4_UnsignedShort, Vec4_Int, Vec4_UnsignedInt
	};

	enum class PipelineCullMode : uint8_t
	{
		None,
		Clockwise,
		CounterClockwise
	};

	enum class PrimitiveMode : uint8_t
	{
		None,
		Points,
		Lines,
		Triangles
	};

	struct Viewport
	{
		uint32_t uX = 0;
		uint32_t uY = 0;
		uint32_t uWidth = 0;
		uint32_t uHeight = 0;
	};

#define GraphicsShaderBitIDLength 256
#define ShaderPropertyOffset 246
#define PipelineCullModePropertyOffset 244
#define PrimitiveModePropertyOffset 242
#define AttributeTypeCrc32PropertyOffset 210
#define AttributeStrideCrc32PropertyOffset 178
#define UniformDataCrc32PropertyOffset 146
#define CombinedShaderTextureHashOffset 114

#define VertexShaderNameCrc32PropertyOffset 82
#define GeometryShaderNameCrc32PropertyOffset 50
#define FragmentShaderNameCrc32PropertyOffset 18

#define EnableColorMultiplierPropertyOffset 113
#define EnabledJointsCountPropertyOffset 110
#define MorphTargetCountPropertyOffset 102
#define ShadingPropertyOffset 101

	typedef uint32_t ShaderPropertyBits;

	enum PipelineCacheStatus : uint8_t
	{
		PipelineCacheStatus_None,
		PipelineCacheStatus_NoCache,
		PipelineCacheStatus_PartialByteCode,
		PipelineCacheStatus_ByteCode,
		PipelineCacheStatus_CachedPipeline
	};


	class DENG_API IGraphicsShader : public IShader
	{
		private:
			std::vector<VertexAttributeType> m_attributeTypes;
			std::vector<std::size_t> m_attributeStrides;

			Viewport m_viewport;
			std::vector<cvar::hash_t> m_textureHashes;
			protected:
			/* Shader module properties are stored in a 256 bit integer.
			 * Starting from MSB the layout of this identifier is defined like this:
				* 1 bit - enable push constants
				* 1 bit - enable custom viewport
				* 1 bit - enable 2D textures
				* 1 bit - enable 3D textures
				* 1 bit - enable scissor
				* 1 bit - enable depth testing
				* 1 bit - enable stencil testing
				* 1 bit - enable blend
				* 1 bit - enable indexing
				* 1 bit - is non-standard shader
				* 2 bits - pipeline cull mode (0: None; 1: Clockwise; 2: CounterClockwise)
				* 2 bits - primitive mode (0: None; 1: Points; 2: Lines; 3: Triangles)
				* 32 bits - attribute type crc32 digest
				* 32 bits - attribute stride crc32 digest
				* 32 bits - uniform data layout crc32 digest
				* 32 bits - combined shader specific texture hash
				* if non_standard_shader:
				*		32 bits - vertex shader module hash
				*		32 bits - geometry shader module hash
				*		32 bits - fragment shader module hash
				* else:
				*		1 bit - enable color multiplier
				*		3 bits - enabled joints count
				*		8 bits - morph target count
				*		1 bit - 0: blinn-phong shading; 1: pbr shading
			 * Minimum number of bits required: 153
			 * Maximum number of bits required: 238
			 */
			std::bitset<GraphicsShaderBitIDLength> m_bProperties = 0;
			std::mutex m_fsMutex;

		public:
			inline void PushAttributeType(VertexAttributeType _eType) { m_attributeTypes.push_back(_eType); }
			inline void HashAttributeTypes()
			{
				uint32_t uCrc32 = cvar::RuntimeCrc32(reinterpret_cast<const char*>(m_attributeTypes.data()), m_attributeTypes.size() * sizeof(VertexAttributeType));
				m_bProperties |= (std::bitset<256>(uCrc32) << AttributeTypeCrc32PropertyOffset);
			}
			inline const std::vector<VertexAttributeType>& GetAttributeTypes() const { return m_attributeTypes; }

			inline void PushAttributeStride(std::size_t _uStride) { m_attributeStrides.push_back(_uStride); }
			inline void HashAttributeStrides()
			{
				uint32_t uCrc32 = cvar::RuntimeCrc32(reinterpret_cast<const char*>(m_attributeStrides.data()), m_attributeStrides.size() * sizeof(std::size_t));
				m_bProperties |= (std::bitset<256>(uCrc32) << AttributeStrideCrc32PropertyOffset);
			}
			inline const std::vector<std::size_t>& GetAttributeStrides() const { return m_attributeStrides; }

			inline void HashUniformDataLayouts()
			{
				uint32_t uCrc32 = cvar::RuntimeCrc32(reinterpret_cast<const char*>(m_uniformDataLayouts.data()), m_uniformDataLayouts.size() * sizeof(UniformDataLayout));
				m_bProperties |= (std::bitset<256>(uCrc32) << UniformDataCrc32PropertyOffset);
			}

			inline void SetViewport(uint32_t _uX, uint32_t _uY, uint32_t _uWidth, uint32_t _uHeight)
			{
				m_viewport.uX = _uX;
				m_viewport.uY = _uY;
				m_viewport.uWidth = _uWidth;
				m_viewport.uHeight = _uHeight;
			}
			inline const Viewport& GetViewport() const { return m_viewport; }

			inline void SetPipelineCullMode(PipelineCullMode _eCullMode)
			{
				m_bProperties |= (std::bitset<256>{static_cast<uint64_t>(_eCullMode)} << PipelineCullModePropertyOffset);
			}
			inline PipelineCullMode GetPipelineCullMode() const
			{
				return static_cast<PipelineCullMode>(((m_bProperties & (std::bitset<256>{0b11} << PipelineCullModePropertyOffset)) >> PipelineCullModePropertyOffset).to_ulong());
			}

			inline void SetPrimitiveMode(PrimitiveMode _ePrimitiveMode)
			{
				m_bProperties |= (std::bitset<256>{static_cast<uint64_t>(_ePrimitiveMode)} << PrimitiveModePropertyOffset);
			}
			inline PrimitiveMode GetPrimitiveMode() const
			{
				return static_cast<PrimitiveMode>(((m_bProperties & (std::bitset<256>{static_cast<uint64_t>(0b11)} << PrimitiveModePropertyOffset)) >> PrimitiveModePropertyOffset).to_ulong());
			}

			inline bool IsPropertySet(ShaderPropertyBits _bmPropertyBits) const
			{
				return !(m_bProperties & (std::bitset<256>{static_cast<uint64_t>(_bmPropertyBits)} << ShaderPropertyOffset)).none();
			}

			inline void SetProperty(ShaderPropertyBits _bmPropertyBits, bool _bEnable = true)
			{
				if (_bEnable)
				{
					m_bProperties |= (std::bitset<256>{static_cast<uint64_t>(_bmPropertyBits)} << ShaderPropertyOffset);
				}
				else
				{
					_bmPropertyBits = ~_bmPropertyBits;
					m_bProperties ^= (std::bitset<256>{static_cast<uint64_t>(_bmPropertyBits)} << ShaderPropertyOffset);
				}
			}

			inline void SetEnableColorMultiplier(bool _bEnabled = true)
			{
				DENG_ASSERT(!IsPropertySet(ShaderPropertyBit_IsNonStandardShader));
				if (_bEnabled)
					m_bProperties |= (std::bitset<256>{1} << EnableColorMultiplierPropertyOffset);
				else
				{
					m_bProperties ^= (std::bitset<256>{static_cast<uint64_t>(!m_bProperties[EnableColorMultiplierPropertyOffset])} << EnableColorMultiplierPropertyOffset);
				}
			}

			inline bool GetEnableColorMultiplier()
			{
				DENG_ASSERT(!IsPropertySet(ShaderPropertyBit_IsNonStandardShader));
				return !(m_bProperties & (std::bitset<256>{1} << EnableColorMultiplierPropertyOffset)).none();
			}

			inline void SetEnabledJointsCount(uint32_t _uNJoints)
			{
				DENG_ASSERT(_uNJoints < 8 && !IsPropertySet(ShaderPropertyBit_IsNonStandardShader));
				std::bitset<256> bNJoints{ _uNJoints };
				m_bProperties |= (bNJoints << EnabledJointsCountPropertyOffset);
			}

			inline uint32_t GetEnabledJointsCount()
			{
				DENG_ASSERT(!IsPropertySet(ShaderPropertyBit_IsNonStandardShader));
				return static_cast<uint32_t>(((m_bProperties >> EnabledJointsCountPropertyOffset) & std::bitset<256>{0b111}).to_ullong());
			}

			inline void SetMorphTargetCount(uint32_t _uNMorphTargets)
			{
				DENG_ASSERT(_uNMorphTargets < 256 && !IsPropertySet(ShaderPropertyBit_IsNonStandardShader));
				std::bitset<256> bNMorphTargets{ _uNMorphTargets };
				m_bProperties |= (bNMorphTargets << MorphTargetCountPropertyOffset);
			}

			inline uint32_t GetMorphTargetCount()
			{
				DENG_ASSERT(!IsPropertySet(ShaderPropertyBit_IsNonStandardShader));
				return static_cast<uint32_t>(((m_bProperties >> MorphTargetCountPropertyOffset) & std::bitset<256>{0xff}).to_ullong());
			}

			inline void SetIsPbr(bool _bEnabled = true)
			{
				DENG_ASSERT(!IsPropertySet(ShaderPropertyBit_IsNonStandardShader));
				if (_bEnabled)
				{
					m_bProperties |= (std::bitset<256>{1} << ShadingPropertyOffset);
				}
				else
				{
					m_bProperties ^= (std::bitset<256>{!_bEnabled} << ShadingPropertyOffset);
				}
			}

			inline bool IsPbr()
			{
				DENG_ASSERT(!IsPropertySet(ShaderPropertyBit_IsNonStandardShader));
				return !(m_bProperties & (std::bitset<256>{1} << ShadingPropertyOffset)).none();
			}

			inline std::size_t PushTextureHash(cvar::hash_t _hshTexture)
			{
				m_textureHashes.push_back(_hshTexture);
				return m_textureHashes.size() - 1;
			}

			inline cvar::hash_t ReplaceTextureHash(size_t _uSamplerId, cvar::hash_t _hshTexture)
			{
				if (m_textureHashes.size() <= _uSamplerId)
					return 0;
				m_textureHashes[_uSamplerId] = _hshTexture;
				return _hshTexture;
			}

			inline void HashTextures()
			{
				uint32_t uCrc32 = cvar::RuntimeCrc32(reinterpret_cast<const char*>(m_textureHashes.data()), m_textureHashes.size() * sizeof(cvar::hash_t));
				m_bProperties |= (std::bitset<256>(uCrc32) << CombinedShaderTextureHashOffset);
			}

			inline cvar::hash_t GetTextureHash(size_t _uSamplerId) const
			{
				if (m_textureHashes.size() <= _uSamplerId)
					return 0;
				return m_textureHashes[_uSamplerId];
			}

			inline void DigestBits()
			{
				m_hshDigestedProperties = cvar::RuntimeCrc32(m_bProperties.to_string());
			}

			inline cvar::hash_t GetDigestedProperties() const
			{
				return m_hshDigestedProperties;
			}


			virtual std::vector<uint32_t> GetVertexShaderByteCode() const = 0;
			virtual std::vector<uint32_t> GetGeometryShaderByteCode() const { return std::move(std::vector<uint32_t>()); }
			virtual std::vector<uint32_t> GetFragmentShaderByteCode() const = 0;

			virtual std::vector<char> GetPipelineCache() const { return std::move(std::vector<char>()); }
			virtual void CachePipeline(const void*, size_t) const {}
			virtual PipelineCacheStatus GetPipelineCacheStatus() const { return PipelineCacheStatus_NoCache; }
	};
}