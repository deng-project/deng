// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Mesh.h - Mesh data structures header
// author: Karl-Mihkel Ott

#pragma once

#include <vector>
#include <array>
#include <glm/glm.hpp>


namespace DENG
{
	enum PBRSamplerBits_T : uint32_t
	{
		PBRSamplerBit_None = 0,
		PBRSamplerBit_AlbedoMap = (1 << 0),
		PBRSamplerBit_EmissionMap = (1 << 1),
		PBRSamplerBit_NormalMap = (1 << 2),
		PBRSamplerBit_MetallicMap = (1 << 3),
		PBRSamplerBit_RoughnessMap = (1 << 4),
		PBRSamplerBit_AmbientOcclusionMap = (1 << 5)
	};

	typedef uint32_t PBRSamplerBits;

	struct alignas(16) MaterialPBR
	{
		MaterialPBR() = default;
		MaterialPBR(const MaterialPBR&) = default;
		MaterialPBR(MaterialPBR&&) = default;

		glm::vec4 vAlbedoFactor = { 1.f, 1.f, 1.f, 1.f };
		glm::vec4 vEmissiveFactor = { 0.f, 0.f, 0.f, 1.f };
		float fRoughness = 0.0f;
		float fMetallic = 0.0f;
		float fAmbientOcclusion = 1.f;
		PBRSamplerBits uSamplerBits = PBRSamplerBit_None;
	};

	enum PhongSamplerBits_T : uint32_t
	{
		PhongSamplerBit_None = 0,
		PhongSamplerBit_DiffuseMap = (1 << 0),
		PhongSamplerBit_SpecularMap = (1 << 1)
	};

	typedef uint32_t PhongSamplerBits;


	struct alignas(16) MaterialPhong
	{
		MaterialPhong() = default;
		MaterialPhong(const MaterialPhong&) = default;
		MaterialPhong(MaterialPhong&&) = default;

		glm::vec4 vDiffuse = { 1.0f, 0.f, 0.f, 1.f };
		glm::vec4 vSpecular = { 1.0f, 0.f, 0.f, 1.f };
		PhongSamplerBits uSamplerBits = PhongSamplerBit_None;
	};


	struct Scissor
	{
		std::array<int32_t, 2> offsets;
		std::array<uint32_t, 2> extent;
	};


	struct Batch
	{
		std::vector<std::size_t> attributeOffsets;
		Scissor scissor;
		std::size_t uIndicesOffset = SIZE_MAX;
		uint32_t uDrawCount = 0;
	};
}
