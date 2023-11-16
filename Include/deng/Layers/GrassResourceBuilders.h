// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: GrassResourceBuilders.h - header for grass resource builders
// author: Karl-Mihkel Ott

#ifndef GRASS_RESOURCE_BUILDERS_H
#define GRASS_RESOURCE_BUILDERS_H

#include "deng/IRenderer.h"
#include "deng/RenderResources.h"
#include <cvar/SID.h>
#include "deng/ResourceBuilders.h"

#ifdef GRASS_RESOURCE_BUILDERS_CPP
#include "deng/FileSystemGraphicsShader.h"
#endif

namespace DENG {

	class GrassMeshBuilder : public IMeshBuilder {
		private:
			IRenderer* m_pRenderer;

		public:
			GrassMeshBuilder(IRenderer* _pRenderer) :
				m_pRenderer(_pRenderer) {}
			
			MeshCommands Get();
	};


	class TerrainShaderBuilder : public IGraphicsShaderBuilder {
		private:
			cvar::hash_t m_hshTerrainHeightTexture;
			cvar::hash_t m_hshTerrainTexture;

		public:
			TerrainShaderBuilder(IGraphicsShaderCompiler* _pCompiler, cvar::hash_t _hshTerrainHeightTexture, cvar::hash_t _hshTerrainTexture) :
				IGraphicsShaderBuilder(_pCompiler),
				m_hshTerrainHeightTexture(_hshTerrainHeightTexture),
				m_hshTerrainTexture(_hshTerrainTexture) {}

			IGraphicsShader* Get();
	};


	class GrassShaderBuilder : public IGraphicsShaderBuilder {
		private:
			std::size_t m_uTimerOffset;
			cvar::hash_t m_hshGrassTexture;
			cvar::hash_t m_hshWindTexture;
			cvar::hash_t m_hshHeightTexture;

		public:
			GrassShaderBuilder(IGraphicsShaderCompiler* _pCompiler, size_t _uTimerOffset, cvar::hash_t _hshGrassTexture, cvar::hash_t _hshWindTexture, cvar::hash_t _hshHeightTexture) :
				IGraphicsShaderBuilder(_pCompiler),
				m_uTimerOffset(_uTimerOffset),
				m_hshGrassTexture(_hshGrassTexture),
				m_hshWindTexture(_hshWindTexture),
				m_hshHeightTexture(_hshHeightTexture) {}

			IGraphicsShader* Get();
	};
}

#endif