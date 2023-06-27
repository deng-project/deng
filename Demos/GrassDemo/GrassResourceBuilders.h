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
			std::size_t m_uTimerOffset;
			DENG::hash_t m_hshGrassTexture;
			DENG::hash_t m_hshWindTexture;

		public:
			GrassShaderBuilder(size_t _uTimerOffset, DENG::hash_t _hshGrassTexture, DENG::hash_t _hshWindTexture) :
				m_uTimerOffset(_uTimerOffset),
				m_hshGrassTexture(_hshGrassTexture),
				m_hshWindTexture(_hshWindTexture) {}

			DENG::IShader* Get();
	};
}

#endif