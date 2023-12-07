// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Materials.h - Data structures for materials
// author: Karl-Mihkel Ott

#pragma once

#include <cstdint>
#include <array>
#include <trs/Vector.h>

namespace DENG {

	enum PBRSamplerBits_T : uint32_t {
		PBRSamplerBit_None = 0,
		PBRSamplerBit_AlbedoMap = (1 << 0),
		PBRSamplerBit_EmissionMap = (1 << 1),
		PBRSamplerBit_NormalMap = (1 << 2),
		PBRSamplerBit_MetallicMap = (1 << 3),
		PBRSamplerBit_RoughnessMap = (1 << 4),
		PBRSamplerBit_AmbientOcclusionMap = (1 << 5)
	};

	typedef uint32_t PBRSamplerBits;

	struct alignas(16) MaterialPBR {
		MaterialPBR() = default;
		MaterialPBR(const MaterialPBR&) = default;
		MaterialPBR(MaterialPBR&&) = default;

		TRS::Vector4<float> vAlbedoFactor = { 1.f, 1.f, 1.f, 1.f };
		TRS::Vector4<float> vEmissiveFactor = { 0.f, 0.f, 0.f, 1.f };
		float fRoughness = 0.0f;
		float fMetallic = 0.0f;
		float fAmbientOcclusion = 1.f;
		PBRSamplerBits uSamplerBits = PBRSamplerBit_None;

		static constexpr size_t s_uTextureCount = 6;
	};

	enum PhongSamplerBits_T : uint32_t {
		PhongSamplerBit_None = 0,
		PhongSamplerBit_DiffuseMap = (1 << 0),
		PhongSamplerBit_SpecularMap = (1 << 1)
	};

	typedef uint32_t PhongSamplerBits;

	struct alignas(16) MaterialPhong {
		MaterialPhong() = default;
		MaterialPhong(const MaterialPhong&) = default;
		MaterialPhong(MaterialPhong&&) = default;

		TRS::Vector4<float> vDiffuse = { 1.0f, 0.f, 0.f, 1.f };
		TRS::Vector4<float> vSpecular = { 1.0f, 0.f, 0.f, 1.f };
		PhongSamplerBits uSamplerBits = PhongSamplerBit_None;

		static constexpr size_t s_uTextureCount = 4;
	};

	template<typename T>
	struct Material {
		T material;
		std::array<cvar::hash_t, T::s_uTextureCount> textures{};
	};
}