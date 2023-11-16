// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: ResourceBuilders.h - resource builder interfaces
// author: Karl-Mihkel Ott

#pragma once

#include "deng/IGraphicsShader.h"
#include "deng/IGraphicsShaderCompiler.h"
#include "deng/RenderResources.h"

namespace DENG {

	class DENG_API IGraphicsShaderBuilder {
		protected:
			IGraphicsShaderCompiler* m_pCompiler;

		public:
			IGraphicsShaderBuilder(IGraphicsShaderCompiler* _pGraphicsShaderCompiler) :
				m_pCompiler(_pGraphicsShaderCompiler) {}
			virtual IGraphicsShader* Get() = 0;
	};

	class DENG_API ITextureBuilder {
		public:
			ITextureBuilder() = default;
			virtual Texture Get() = 0;
	};

	class DENG_API IMeshBuilder {
		public:
			IMeshBuilder() = default;
			virtual MeshCommands Get() = 0;
	};

	template <typename T>
	class DENG_API IMaterialBuilder {
		virtual Material<T> Get() = 0;
	};
}
