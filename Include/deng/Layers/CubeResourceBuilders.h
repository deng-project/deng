

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
#include "deng/Layers/CubeVertices.h"
#endif

namespace DENG {

	class CubeMeshBuilder {
		private:
			std::size_t m_uVertexOffset;

		public:
			CubeMeshBuilder(std::size_t _uVertexOffset) :
				m_uVertexOffset(_uVertexOffset) {}

			MeshCommands Get();
	};

	class CubeShaderBuilder {
		public:
			CubeShaderBuilder() = default;
			IShader* Get();
	};


	class CubeMaterialBuilder {
		private:
			hash_t m_hshDiffuse;
			hash_t m_hshSpecular;

		public:
			CubeMaterialBuilder(hash_t _hshDiffuse, hash_t _hshSpecular) :
				m_hshDiffuse(_hshDiffuse),
				m_hshSpecular(_hshSpecular) {}

			Material Get();
	};


	class CubeTextureBuilder {
		private:
			const std::string m_sFileName;

		public:
			CubeTextureBuilder(const std::string& _sFileName) :
				m_sFileName(_sFileName) {}

			Texture Get();
	};
}

#endif
