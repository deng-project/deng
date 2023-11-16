// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: IShader.h - shader interface class header
// author: Karl-Mihkel Ott

#pragma once

#include "deng/Api.h"
#include <cstdint>
#include <vector>
#include <cvar/SID.h>

namespace DENG {
	enum class UniformDataType : uint8_t {
		None,
		Buffer,
		StorageBuffer,
		ImageSampler2D,
		ImageSampler3D
	};

	enum ShaderStageBits_T : uint8_t {
		ShaderStageBit_None = 0,
		ShaderStageBit_Compute = 1 << 0,
		ShaderStageBit_Vertex = 1 << 1,
		ShaderStageBit_Geometry = 1 << 2,
		ShaderStageBit_Fragment = 1 << 3
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

	enum ShaderPropertyBits_T : uint16_t {
		ShaderPropertyBit_EnablePushConstants = (1 << 0),
		ShaderPropertyBit_EnableCustomViewport = (1 << 1),
		ShaderPropertyBit_Enable2DTextures = (1 << 2),
		ShaderPropertyBit_Enable3DTextures = (1 << 3),
		ShaderPropertyBit_EnableScissor = (1 << 4),
		ShaderPropertyBit_EnableDepthTesting = (1 << 5),
		ShaderPropertyBit_EnableStencilTesting = (1 << 6),
		ShaderPropertyBit_EnableBlend = (1 << 7),
		ShaderPropertyBit_EnableIndexing = (1 << 8),
		ShaderPropertyBit_IsNonStandardShader = (1 << 9)
	};

	struct PushConstant {
		uint32_t uLength = 0;
		ShaderStageBits bmShaderStage = ShaderStageBit_None;
		const void* pPushConstantData = nullptr;
	};

	class DENG_API IShader {
		protected:
			PushConstant m_pushConstant;
			std::vector<UniformDataLayout> m_uniformDataLayouts;
			cvar::hash_t m_hshDigestedProperties = 0;

		public:
			// UniformDataLayout modifications
			inline void PushUniformDataLayout(UniformDataType _eType, ShaderStageBits _bmShaderStage, uint32_t _uBinding = 0, uint32_t _uSize = 0, uint32_t _uOffset = 0) {
				m_uniformDataLayouts.push_back(UniformDataLayout{
					{ _uBinding, _uSize, _uOffset },
					_eType,
					_bmShaderStage
					});
			}

			inline const std::vector<UniformDataLayout>& GetUniformDataLayouts() const { return m_uniformDataLayouts; }
			inline std::vector<UniformDataLayout>& GetUniformDataLayouts() { return m_uniformDataLayouts; }

			// Push constants
			inline void SetPushConstant(uint32_t _uLength, ShaderStageBits _bmShaderStage, const void* _pData) {
				m_pushConstant.uLength = _uLength;
				m_pushConstant.bmShaderStage = _bmShaderStage;
				m_pushConstant.pPushConstantData = _pData;
			}

			inline const PushConstant& GetPushConstant() const { return m_pushConstant; }
			inline PushConstant& GetPushConstant() { return m_pushConstant; }
	};
}