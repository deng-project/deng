// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: CubeResourceBuilders.h - header for cube resource builders
// author: Karl-Mihkel Ott

#ifndef CUBE_RESOURCE_BUILDERS_H
#define CUBE_RESOURCE_BUILDERS_H

#include "deng/SID.h"
#include "deng/IRenderer.h"
#include "deng/RenderResources.h"

#ifdef CUBE_RESOURCE_BUILDERS_CPP
#include "deng/ProgramFilesManager.h"
#include "deng/FileSystemShader.h"
#include "das/stb_image.h"
#include "../Utils/CubeVertices.h"
#endif

namespace Application {

	class CubePrefabMeshBuilder {
		private:
			DENG::IRenderer* m_pRenderer;

		public:
			CubePrefabMeshBuilder(DENG::IRenderer* _pRenderer) :
				m_pRenderer(_pRenderer) {}

			DENG::MeshCommands Get();
	};

	class CubePrefabShaderBuilder {
		public:
			CubePrefabShaderBuilder() = default;
			DENG::IShader* Get();
	};


	class CubePrefabMaterialBuilder {
		private:
			DENG::hash_t m_hshDiffuse;
			DENG::hash_t m_hshSpecular;

		public:
			CubePrefabMaterialBuilder(DENG::hash_t _hshDiffuse, DENG::hash_t _hshSpecular) :
				m_hshDiffuse(_hshDiffuse),
				m_hshSpecular(_hshSpecular) {}

			DENG::Material Get();
	};


	class CubePrefabTextureBuilder {
		private:
			const std::string m_sFileName;

		public:
			CubePrefabTextureBuilder(const std::string& _sFileName) :
				m_sFileName(_sFileName) {}

			DENG::Texture Get();
	};
}

#endif
