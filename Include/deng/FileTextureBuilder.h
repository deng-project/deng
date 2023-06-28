// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: FileTextureBuilder.h - FileTextureBuilder class header
// author: Karl-Mihkel Ott

#ifndef FILE_TEXTURE_BUILDER_H
#define FILE_TEXTURE_BUILDER_H

#include "deng/Api.h"
#include "deng/RenderResources.h"

#ifdef FILE_TEXTURE_BUILDER_CPP
	#include <fstream>
	#include <array>
	#include "deng/Exceptions.h"
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


	class DENG_API FileMonolithicCubeTextureBuilder {
		private:
			const std::string m_sFileName;

		public:
			FileMonolithicCubeTextureBuilder(const std::string& _sFileName) :
				m_sFileName(_sFileName) {}

			Texture Get();
	};


	class FileCubeTextureBuilder {
		private:
			const std::string m_sPosXFileName;
			const std::string m_sNegXFileName;
			const std::string m_sPosYFileName;
			const std::string m_sNegYFileName;
			const std::string m_sPosZFileName;
			const std::string m_sNegZFileName;

		public:
			FileCubeTextureBuilder(
				const std::string& _sPosXFileName,
				const std::string& _sNegXFileName,
				const std::string& _sPosYFileName,
				const std::string& _sNegYFileName,
				const std::string& _sPosZFileName,
				const std::string& _sNegZFileName) :
				m_sPosXFileName(_sPosXFileName),
				m_sNegXFileName(_sNegXFileName),
				m_sPosYFileName(_sPosYFileName),
				m_sNegYFileName(_sNegYFileName),
				m_sPosZFileName(_sPosZFileName),
				m_sNegZFileName(_sNegZFileName) {}

			Texture Get();
	};
}

#endif
