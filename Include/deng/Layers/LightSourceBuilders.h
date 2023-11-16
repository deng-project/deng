// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: LightSourceBuilders.h - light resource builders' header
// author: Karl-Mihkel Ott

#ifndef LIGHT_SOURCE_BUILDERS_H
#define LIGHT_SOURCE_BUILDERS_H

#include "deng/RenderResources.h"
#include "deng/ResourceBuilders.h"

#ifdef LIGHT_SOURCE_BUILDERS_CPP
#include "deng/FileSystemGraphicsShader.h"
#endif

namespace DENG {

	class LightSourceMeshBuilder : public IMeshBuilder {
		private:
			size_t m_uVertexOffset;

		public:
			LightSourceMeshBuilder(size_t _uVertexOffset) :
				m_uVertexOffset(_uVertexOffset) {}
			
			MeshCommands Get();
	};


	class LightSourceShaderBuilder : public IGraphicsShaderBuilder {
		public:
			LightSourceShaderBuilder(IGraphicsShaderCompiler* _pCompiler) :
				IGraphicsShaderBuilder(_pCompiler) {}
			IGraphicsShader* Get();
	};

}

#endif
