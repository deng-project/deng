// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: IGraphicsMemory.cpp - abstractions for allocating and managing memory on GPU
// author: Karl-Mihkel Ott

#include <array>
#include <stb_image.h>

#include <deng/Exceptions.h>
#include <deng/IGraphicsMemory.h>
#include <deng/ProgramFilesManager.h>

namespace DENG {

	void IGPUImage::LoadTexture1DFromFile(const char* _szFileName) 
	{
		ProgramFilesManager programFilesManager;
		auto imageData = programFilesManager.GetProgramFileContent(_szFileName);
		
		if (imageData.empty())
			throw IOException("Failed to read contents from " + std::string(_szFileName));

		int x, y, depth;
		stbi_uc* pTexels = stbi_load_from_memory(
			(stbi_uc*)imageData.data(),
			static_cast<int>(imageData.size()),
			&x, &y, &depth, 0);

		this->LoadTexture1DFromMemory(pTexels, static_cast<size_t>(x * y * depth), depth);
		
		// free the memory
		stbi_image_free(pTexels);
	}


	void IGPUImage::LoadTexture2DFromFile(const char* _szFileName, bool _bCreateMipmaps) {
		ProgramFilesManager programFilesManager;
		auto imageData = programFilesManager.GetProgramFileContent(_szFileName);

		if (imageData.empty())
			throw IOException("Failed to read contents from " + std::string(_szFileName));

		int x, y, depth;
		stbi_uc* pTexels = stbi_load_from_memory(
			(stbi_uc*)imageData.data(),
			static_cast<int>(imageData.size()),
			&x, &y, &depth, 0);

		this->LoadTexture2DFromMemory(pTexels, static_cast<uint32_t>(x), static_cast<uint32_t>(y), static_cast<uint32_t>(depth), _bCreateMipmaps);
		
		// free the memory
		stbi_image_free(pTexels);
	}


	void IGPUImage::LoadTexture3DFromFiles(
		const char* _szPosXFileName,
		const char* _szNegXFileName,
		const char* _szPosYFileName,
		const char* _szNegYFileName,
		const char* _szPosZFileName,
		const char* _szNegZFileName,
		bool _bCreateMipMaps)
	{
		std::array<stbi_uc*, 6> dataPointers = {};
		std::array<const char*, 6> filenames = {
			_szPosXFileName,
			_szNegXFileName,
			_szPosYFileName,
			_szNegYFileName,
			_szPosZFileName,
			_szNegZFileName
		};

		ProgramFilesManager programFilesManager;
		int x, y, depth;

		// read files into dataPointers
		for (size_t i = 0; i < 6; i++) {
			auto imageData = programFilesManager.GetProgramFileContent(filenames[i]);

			if (imageData.empty())
				throw IOException("Failed to read contents from " + std::string(filenames[i]));

			dataPointers[i] = stbi_load_from_memory(
				(stbi_uc*)imageData.data(),
				static_cast<int>(imageData.size()),
				&x, &y, &depth, 0);
		}

		this->LoadTexture3DFromMemory(
			static_cast<uint32_t>(x),
			static_cast<uint32_t>(y),
			static_cast<uint32_t>(depth),
			dataPointers[0],
			dataPointers[1],
			dataPointers[2],
			dataPointers[3],
			dataPointers[4],
			dataPointers[5],
			_bCreateMipMaps);

		// free the allocated memory
		for (size_t i = 0; i < 6; i++) {
			stbi_image_free(dataPointers[i]);
		}
	}
}