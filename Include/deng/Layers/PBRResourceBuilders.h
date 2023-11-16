// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: PBRResourceBuilders.h - header for PBR resource builders
// author: Karl-Mihkel Ott

#ifndef PBR_RESOURCE_BUILDERS_H
#define PBR_RESOURCE_BUILDERS_H

#include "deng/IRenderer.h"
#include "deng/RenderResources.h"
#include "deng/ResourceBuilders.h"

#ifdef PBR_RESOURCE_BUILDERS_CPP
#include "deng/FileSystemGraphicsShader.h"
#include "deng/MathConstants.h"
#include "deng/Layers/PBRTable.h"
#define SPHERE_SEGMENTS 64
#endif

namespace DENG {

	class PBRSphereBuilder : public IMeshBuilder {
		private:
			static size_t m_uVertexOffset;
			static size_t m_uIndicesOffset;
			static size_t m_uIndicesCount;
			IRenderer* m_pRenderer;

		private:
			static std::vector<uint32_t> _GenerateSphereIndices();
			static std::vector<Vertex<TRS::Vector3<float>, TRS::Vector3<float>, TRS::Vector2<float>>> _GenerateSphereVertices();

		public:
			PBRSphereBuilder(IRenderer* _pRenderer) :
				m_pRenderer(_pRenderer) {}
			MeshCommands Get();
	};


	class PBRShaderBuilder : public IGraphicsShaderBuilder {
		public:
			PBRShaderBuilder(IGraphicsShaderCompiler* _pCompiler) :
				IGraphicsShaderBuilder(_pCompiler) {}
			IGraphicsShader* Get();
	};

	class PBRMaterialBuilder : public IMaterialBuilder<MaterialPBR> {
		private:
			std::array<cvar::hash_t, MaterialPBR::s_uTextureCount> m_textures;

		public:
			PBRMaterialBuilder(const std::array<cvar::hash_t, MaterialPBR::s_uTextureCount>& _textures) :
				m_textures(_textures) {}
			Material<MaterialPBR> Get();
	};
}

#endif
