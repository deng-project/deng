// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: SkyboxBuilders.h - skybox resource builders' header
// author: Karl-Mihkel Ott

#ifndef SKYBOX_BUILDERS_H
#define SKYBOX_BUILDERS_H

#include "deng/Api.h"
#include "deng/RenderResources.h"
#include "deng/IRenderer.h"

#ifdef SKYBOX_BUILDERS_CPP
#include "deng/FileSystemShader.h"
#endif

namespace DENG {

	class DENG_API SkyboxMeshBuilder {
		private:
			IRenderer* m_pRenderer;
			static size_t s_uVertexOffset;
			static const float s_skyboxVertices[];

		public:
			SkyboxMeshBuilder(IRenderer* _pRenderer) :
				m_pRenderer(_pRenderer) {}

			MeshCommands Get();
	};


	class DENG_API SkyboxShaderBuilder {
		private:
			hash_t m_hshTexture;

		public:
			SkyboxShaderBuilder(hash_t _hshTexture) :
				m_hshTexture(_hshTexture) {}
			IShader* Get();
	};
}

#endif
