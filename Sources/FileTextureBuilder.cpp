// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: FileTextureBuilder.cpp - FileTextureBuilder class implementation
// author: Karl-Mihkel Ott

#define FILE_TEXTURE_BUILDER_CPP
#include "deng/FileTextureBuilder.h"

namespace DENG {

	TextureLoadType GetLoadType(const std::string& _sFileName) {
		char bytes[4]{};
		std::ifstream stream(_sFileName);

		stream.read(bytes, 4);

		// check magic bytes
		if (bytes[0] == 0xff && bytes[1] == 0xd8 && bytes[2] == 0xff && bytes[3] == 0xe0)
			return TextureLoadType::External_JPEG;
		else if (bytes[0] == 0x89 && bytes[1] == 0x50 && bytes[2] == 0x4e && bytes[3] == 0x47)
			return TextureLoadType::External_PNG;
		else if ((bytes[0] == 0x49 && bytes[1] == 0x49 && bytes[2] == 0x2a && bytes[3] == 0x00) ||
			(bytes[0] == 0x4d && bytes[1] == 0x4d && bytes[2] == 0x00 && bytes[3] == 0x2a))
			return TextureLoadType::External_TIFF;
		else if (bytes[0] == 0x42 && bytes[1] == 0x4d)
			return TextureLoadType::External_BMP;
		else if (bytes[0] == 0x47 && bytes[1] == 0x49 && bytes[2] == 0x46 && bytes[3] == 0x38)
			return TextureLoadType::External_GIF;

		return TextureLoadType::External_TGA;

	}

	Texture FileTextureBuilder::Get() {
		Texture texture;
		ProgramFilesManager programFilesManager;

		auto imageData = programFilesManager.GetProgramFileContent(m_sFileName);
		if (imageData.empty())
			throw IOException("Failed to read contents from " + m_sFileName);

		int x, y, depth;
		stbi_uc* pTexels = stbi_load_from_memory(
			(stbi_uc*)imageData.data(),
			static_cast<int>(imageData.size()),
			&x, &y, &depth, 4);

		texture.bHeapAllocationFlag = true;
		texture.pRGBAData = reinterpret_cast<char*>(pTexels);
		texture.eLoadType = GetLoadType(m_sFileName);
		texture.eResourceType = TextureType::Image_2D;
		texture.uWidth = static_cast<uint32_t>(x);
		texture.uHeight = static_cast<uint32_t>(y);
		texture.uBitDepth = 4;

		return texture;
	}


	Texture FileMonolithicCubeTextureBuilder::Get() {
		Texture texture;
		ProgramFilesManager programFilesManager;

		auto imageData = programFilesManager.GetProgramFileContent(m_sFileName);
		if (imageData.empty())
			throw IOException("Failed to read contents from " + m_sFileName);

		int x, y, depth;
		stbi_uc* pTexels = stbi_load_from_memory(
			(stbi_uc*)imageData.data(),
			static_cast<int>(imageData.size()),
			&x, &y, &depth, 4);

		texture.bHeapAllocationFlag = true;
		texture.pRGBAData = reinterpret_cast<char*>(pTexels);
		texture.eLoadType = GetLoadType(m_sFileName);
		texture.eResourceType = TextureType::Image_3D;
		texture.uWidth = static_cast<uint32_t>(x);
		texture.uHeight = static_cast<uint32_t>(y);
		texture.uBitDepth = 4;

		return texture;
	}


	Texture FileCubeTextureBuilder::Get() {
		std::array<std::string, 6> arrFileNames = {
			m_sPosXFileName, m_sNegXFileName,
			m_sPosYFileName, m_sNegYFileName,
			m_sPosZFileName, m_sNegZFileName };
		std::array<stbi_uc*, 6> arrTexels = {};

		Texture texture;
		ProgramFilesManager programFilesManager;

		for (auto it = arrFileNames.begin(); it != arrFileNames.end(); it++) {
			auto imageData = programFilesManager.GetProgramFileContent(*it);
			if (imageData.empty())
				throw IOException("Failed to read contents from " + *it);
			
			int x, y, depth;
			stbi_uc* pTexels = stbi_load_from_memory(
				(stbi_uc*)imageData.data(),
				static_cast<int>(imageData.size()),
				&x, &y, &depth, 4);

			if ((texture.uWidth || texture.uHeight) && (texture.uWidth != static_cast<uint32_t>(x) || texture.uHeight != static_cast<uint32_t>(y)))
				throw LogicException("Conflicting cube map resolutions");
			
			texture.uWidth = static_cast<uint32_t>(x);
			texture.uHeight = static_cast<uint32_t>(y);
			texture.uBitDepth = 4;
			texture.eLoadType = GetLoadType(*it);
			texture.eResourceType = TextureType::Image_3D_Array;
			arrTexels[it - arrFileNames.begin()] = pTexels;
		}

		texture.bHeapAllocationFlag = true;
		texture.pRGBAData = new char[texture.uWidth * texture.uHeight * texture.uBitDepth * 6] {};
		for (uint32_t i = 0; i < 6; i++)
			std::memcpy(texture.pRGBAData + i * texture.uWidth * texture.uHeight * texture.uBitDepth, arrTexels[i], texture.uWidth * texture.uHeight * texture.uBitDepth);
	
		for (auto ptr : arrTexels)
			stbi_image_free(ptr);

		return texture;
	}
}