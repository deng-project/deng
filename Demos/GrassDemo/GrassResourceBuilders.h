// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: GrassResourceBuilders.h - header for grass resource builders
// author: Karl-Mihkel Ott

#ifndef GRASS_RESOURCE_BUILDERS_H
#define GRASS_RESOURCE_BUILDERS_H

#include "deng/IRenderer.h"
#include "deng/RenderResources.h"
#include "deng/SID.h"

#ifdef GRASS_RESOURCE_BUILDERS_CPP
#include "deng/FileSystemShader.h"
#endif

namespace Application {

	class GrassMeshBuilder {
		private:
			DENG::IRenderer* m_pRenderer;

		public:
			GrassMeshBuilder(DENG::IRenderer* _pRenderer) :
				m_pRenderer(_pRenderer) {}
			
			DENG::MeshCommands Get();
	};


	class GrassShaderBuilder {
		private:
			DENG::hash_t m_hshTexture;

		public:
			GrassShaderBuilder(DENG::hash_t _hshTexture) :
				m_hshTexture(_hshTexture) {}

			DENG::IShader* Get();
	};
}

#endif