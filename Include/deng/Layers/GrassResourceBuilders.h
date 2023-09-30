// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: GrassResourceBuilders.h - header for grass resource builders
// author: Karl-Mihkel Ott

#ifndef GRASS_RESOURCE_BUILDERS_H
#define GRASS_RESOURCE_BUILDERS_H

#include "deng/IRenderer.h"
#include "deng/RenderResources.h"
#include <cvar/SID.h>

#ifdef GRASS_RESOURCE_BUILDERS_CPP
#include "deng/FileSystemShader.h"
#endif

namespace DENG {

	class GrassMeshBuilder {
		private:
			IRenderer* m_pRenderer;

		public:
			GrassMeshBuilder(IRenderer* _pRenderer) :
				m_pRenderer(_pRenderer) {}
			
			MeshCommands Get();
	};


	class TerrainShaderBuilder {
		private:
			cvar::hash_t m_hshTerrainHeightTexture;
			cvar::hash_t m_hshTerrainTexture;

		public:
			TerrainShaderBuilder(cvar::hash_t _hshTerrainHeightTexture, cvar::hash_t _hshTerrainTexture) :
				m_hshTerrainHeightTexture(_hshTerrainHeightTexture),
				m_hshTerrainTexture(_hshTerrainTexture) {}

			IShader* Get();
	};


	class GrassShaderBuilder {
		private:
			std::size_t m_uTimerOffset;
			cvar::hash_t m_hshGrassTexture;
			cvar::hash_t m_hshWindTexture;
			cvar::hash_t m_hshHeightTexture;

		public:
			GrassShaderBuilder(size_t _uTimerOffset, cvar::hash_t _hshGrassTexture, cvar::hash_t _hshWindTexture, cvar::hash_t _hshHeightTexture) :
				m_uTimerOffset(_uTimerOffset),
				m_hshGrassTexture(_hshGrassTexture),
				m_hshWindTexture(_hshWindTexture),
				m_hshHeightTexture(_hshHeightTexture) {}

			IShader* Get();
	};
}

#endif