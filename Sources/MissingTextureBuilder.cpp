#define MISSING_TEXTURE_BUILDER_CPP
#include "deng/MissingTextureBuilder.h"

namespace DENG {

	Texture MissingTextureBuilder2D::Get() {
		int x, y, size;
		char* pTexels = GetMissingTextureRGBA(x, y, size);
		
		Texture texture;
		texture.uWidth = static_cast<uint32_t>(x);
		texture.uHeight = static_cast<uint32_t>(y);
		texture.uBitDepth = 4;
		texture.eLoadType = TextureLoadType::Embedded;
		texture.eResourceType = TextureType::Image_2D;
		texture.pRGBAData = pTexels;

		return texture;
	}


	Texture MissingTextureBuilder3D::Get() {
		int x, y, size;
		char* pTexels = GetMissingTextureRGBA(x, y, size);

		Texture texture;
		texture.uWidth = static_cast<uint32_t>(x);
		texture.uHeight = static_cast<uint32_t>(y);
		texture.uBitDepth = 4;
		texture.eLoadType = TextureLoadType::Embedded;
		texture.eResourceType = TextureType::Image_3D;
		texture.pRGBAData = new char[x * y * 24] {};
		texture.bHeapAllocationFlag = true;

		for (int i = 0; i < 6; i++)
			std::memcpy(texture.pRGBAData + i * x * y * 4, pTexels, x * y * 4);

		return texture;
	}
}