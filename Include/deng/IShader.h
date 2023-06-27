// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: IShader.h - shader interface class header
// author: Karl-Mihkel Ott

#ifndef ISHADER_H
#define ISHADER_H

#include <mutex>
#include <vector>
#include "deng/Api.h"
#include "deng/SID.h"

namespace DENG {

	enum class VertexAttributeType {
		None,
		Float, Double, Byte, UnsignedByte, Short, UnsignedShort, Int, UnsignedInt,
		Vec2_Float, Vec2_Double, Vec2_Byte, Vec2_UnsignedByte, Vec2_Short, Vec2_UnsignedShort, Vec2_Int, Vec2_UnsignedInt,
		Vec3_Float, Vec3_Double, Vec3_Byte, Vec3_UnsignedByte, Vec3_Short, Vec3_UnsignedShort, Vec3_Int, Vec3_UnsignedInt,
		Vec4_Float, Vec4_Double, Vec4_Byte, Vec4_UnsignedByte, Vec4_Short, Vec4_UnsignedShort, Vec4_Int, Vec4_UnsignedInt
	};

	enum class UniformDataType {
		None,
		Buffer,
		StorageBuffer,
		ImageSampler2D,
		ImageSampler3D
	};

	enum class PipelineCullMode {
		None,
		Clockwise,
		CounterClockwise
	};

	enum class PrimitiveMode {
		None,
		Points,
		Lines,
		Triangles
	};

	enum ShaderStageBits_T : uint8_t {
		ShaderStageBit_None		= 0,
		ShaderStageBit_Vertex	= 1 << 0,
		ShaderStageBit_Geometry	= 1 << 1,
		ShaderStageBit_Fragment	= 1 << 2
	};

	typedef uint8_t ShaderStageBits;

	struct UniformStorageSpecification {
		UniformStorageSpecification() = default;
		UniformStorageSpecification(const UniformStorageSpecification&) = default;

		uint32_t uBinding = 0;
		uint32_t uSize = 0;
		uint32_t uOffset = 0;
	};

	struct UniformDataLayout {
		UniformDataLayout() = default;
		UniformDataLayout(const UniformDataLayout&) = default;

		UniformStorageSpecification block;
		UniformDataType eType = UniformDataType::Buffer;
		ShaderStageBits bmShaderStage = ShaderStageBit_None;
	};

	struct Viewport {
		uint32_t uX = 0;
		uint32_t uY = 0;
		uint32_t uWidth = 0;
		uint32_t uHeight = 0;
	};

	enum ShaderPropertyBits_T : uint32_t {
		ShaderPropertyBit_None					= 0,
		ShaderPropertyBit_EnablePushConstants	= 1 << 0,
		ShaderPropertyBit_EnableCustomViewport	= 1 << 1,
		ShaderPropertyBit_Enable2DTextures		= 1 << 2,
		ShaderPropertyBit_Enable3DTextures		= 1 << 3,
		ShaderPropertyBit_EnableScissor			= 1 << 4,
		ShaderPropertyBit_EnableDepthTesting	= 1 << 5,
		ShaderPropertyBit_EnableStencilTesting	= 1 << 6,
		ShaderPropertyBit_EnableBlend			= 1 << 7,
		ShaderPropertyBit_EnableIndexing		= 1 << 8,
		ShaderPropertyBit_EnableMaterial		= 1 << 9,
		ShaderPropertyBit_NonStandardShader		= 1 << 10
	};

	typedef uint32_t ShaderPropertyBits;

	struct PushConstant {
		uint32_t uLength = 0;
		ShaderStageBits bmShaderStage = ShaderStageBit_None;
		const void* pPushConstantData = nullptr;
	};


	enum class RendererType { OpenGL, Vulkan, DirectX };

	enum PipelineCacheStatusBits_T : uint8_t {
		PipelineCacheStatusBit_None = 0x00,
		PipelineCacheStatusBit_NoCache = 0x01,
		PipelineCacheStatusBit_PartialSpirv = 0x02,
		PipelineCacheStatusBit_Spirv = 0x04,
		PipelineCacheStatusBit_VulkanCache = 0x08,
		PipelineCacheStatusBit_OpenGLCache = 0x10,
		PipelineCacheStatusBit_DirectXCache = 0x20,
		PipelineCacheStatusBit_CachedPipeline = 0x38
	};

	typedef uint8_t PipelineCacheStatusBits;

	class DENG_API IShader {
		private:
			std::vector<VertexAttributeType> m_attributeTypes;
			std::vector<std::size_t> m_attributeStrides;
			std::vector<UniformDataLayout> m_uniformDataLayouts;
			Viewport m_viewport;
			PushConstant m_pushConstant;
			
			PipelineCullMode m_ePipelineCullMode = PipelineCullMode::None;
			PrimitiveMode m_ePrimitiveMode = PrimitiveMode::Triangles;
			ShaderPropertyBits m_bmShaderProperties = ShaderPropertyBit_None;
			
			std::vector<hash_t> m_textureHashes;

		protected:
			std::mutex m_fsMutex;

		public:
			inline void PushAttributeType(VertexAttributeType _eType) { m_attributeTypes.push_back(_eType); }
			inline const std::vector<VertexAttributeType>& GetAttributeTypes() const { return m_attributeTypes; }

			inline void PushAttributeStride(std::size_t _uStride) { m_attributeStrides.push_back(_uStride); }
			inline const std::vector<std::size_t>& GetAttributeStrides() const { return m_attributeStrides; }
	
			inline void PushUniformDataLayout(UniformDataType _eType, ShaderStageBits _bmShaderStage, uint32_t _uBinding = 0, uint32_t _uSize = 0, uint32_t _uOffset = 0) {
				m_uniformDataLayouts.push_back(UniformDataLayout{
					{ _uBinding, _uSize, _uOffset },
					_eType,
					_bmShaderStage
					});
			}
			inline const std::vector<UniformDataLayout>& GetUniformDataLayouts() const { return m_uniformDataLayouts; }
			inline std::vector<UniformDataLayout>& GetUniformDataLayouts() { return m_uniformDataLayouts; }

			inline void SetViewport(uint32_t _uX, uint32_t _uY, uint32_t _uWidth, uint32_t _uHeight) {
				m_viewport.uX = _uX;
				m_viewport.uY = _uY;
				m_viewport.uWidth = _uWidth;
				m_viewport.uHeight = _uHeight;
			}
			inline const Viewport& GetViewport() const { return m_viewport; }

			inline void SetPushConstant(uint32_t _uLength, ShaderStageBits _bmShaderStage, const void* _pData) {
				m_pushConstant.uLength = _uLength;
				m_pushConstant.bmShaderStage = _bmShaderStage;
				m_pushConstant.pPushConstantData = _pData;
			}
			inline const PushConstant& GetPushConstant() const { return m_pushConstant; }
			inline PushConstant& GetPushConstant() { return m_pushConstant; }

			inline void SetPipelineCullMode(PipelineCullMode _eCullMode) { m_ePipelineCullMode = _eCullMode; }
			inline PipelineCullMode GetPipelineCullMode() const { return m_ePipelineCullMode; }

			inline void SetPrimitiveMode(PrimitiveMode _ePrimitiveMode) { m_ePrimitiveMode = _ePrimitiveMode; }
			inline PrimitiveMode GetPrimitiveMode() const { return m_ePrimitiveMode; }

			inline void SetProperty(ShaderPropertyBits _bmPropertyBits, bool _bEnable = true) {
				if (_bEnable)
					m_bmShaderProperties |= _bmPropertyBits;
				else m_bmShaderProperties &= ~_bmPropertyBits;
			}
			inline bool IsPropertySet(ShaderPropertyBits _bmPropertyBits) const { return m_bmShaderProperties & _bmPropertyBits; }
	
			inline std::size_t PushTextureHash(hash_t _hshTexture) { 
				m_textureHashes.push_back(_hshTexture);
				return m_textureHashes.size() - 1;
			}

			inline hash_t ReplaceTextureHash(size_t _uSamplerId, hash_t _hshTexture) {
				if (m_textureHashes.size() <= _uSamplerId)
					return 0;
				m_textureHashes[_uSamplerId] = _hshTexture;
				return _hshTexture;
			}

			inline hash_t GetTextureHash(size_t _uSamplerId) const { 
				if (m_textureHashes.size() <= _uSamplerId)
					return 0;
				return m_textureHashes[_uSamplerId]; 
			}


			virtual std::vector<uint32_t> GetVertexShaderSpirv() const = 0;
			virtual std::vector<uint32_t> GetGeometryShaderSpirv() const { return {}; }
			virtual std::vector<uint32_t> GetFragmentShaderSpirv() const = 0;
	
			virtual std::vector<char> GetPipelineCache(RendererType) const { return {}; }
			virtual void CachePipeline(RendererType, const void*, size_t) const {}
			virtual PipelineCacheStatusBits GetPipelineCacheStatus() const { return PipelineCacheStatusBit_NoCache; }
	};
}

#endif
