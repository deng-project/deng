// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: MissingTextureBuilder.h - Header for missing texture builder classes
// author: Karl-Mihkel Ott

#ifndef MISSING_TEXTURE_BUILDER_H
#define MISSING_TEXTURE_BUILDER_H

#include "deng/Api.h"
#include "deng/RenderResources.h"

#ifdef MISSING_TEXTURE_BUILDER_CPP
#include "deng/Missing.h"
#endif

namespace DENG {

	class DENG_API MissingTextureBuilder2D {
		public:
			MissingTextureBuilder2D() = default;
			Texture Get();
	};


	class DENG_API MissingTextureBuilder3D {
		public:
			MissingTextureBuilder3D() = default;
			Texture Get();
	};
}

#endif
