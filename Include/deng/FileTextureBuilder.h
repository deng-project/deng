// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: FileTextureBuilder.h - FileTextureBuilder class header
// author: Karl-Mihkel Ott

#ifndef FILE_TEXTURE_BUILDER_H
#define FILE_TEXTURE_BUILDER_H

#include "deng/Api.h"
#include "deng/RenderResources.h"

#ifdef FILE_TEXTURE_BUILDER_CPP
	#include "deng/ProgramFilesManager.h"
	#include "das/stb_image.h"
#endif

namespace DENG {

	class DENG_API FileTextureBuilder {
		private:
			const std::string m_sFileName;

		public:
			FileTextureBuilder(const std::string& _sFileName) :
				m_sFileName(_sFileName) {}

			Texture Get();
	};
}

#endif
