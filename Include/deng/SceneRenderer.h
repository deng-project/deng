// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: SceneRenderer.h - scene renderer class header
// author: Karl-Mihkel Ott

#ifndef SCENE_RENDERER_H
#define SCENE_RENDERER_H

#include "deng/Api.h"
#include "deng/IRenderer.h"
#include "deng/Components.h"

#ifdef SCENE_RENDERER_CPP
	#include "trs/Vector.h"
#endif

namespace DENG {

	class DENG_API SceneRenderer {
		private:
			IRenderer* m_pRenderer = nullptr;
			IFramebuffer* m_pFramebuffer = nullptr;
			std::vector<size_t> m_transformOffsets;
			std::vector<size_t> m_materialOffsets;
			
			std::array<size_t, 3> m_arrLightOffsets = { SIZE_MAX, 0, 0 };
			size_t m_uUsedLightsSize = 0;

			char* m_pLightBuffer = nullptr;
			size_t m_uLightBufferSize = 0;

		public:
			SceneRenderer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer);
			~SceneRenderer();

			void RenderLights(
				const std::vector<PointLightComponent>& _pointLights, 
				const std::vector<DirectionalLightComponent>& _dirLights, 
				const std::vector<SpotlightComponent>& _spotLights,
				const TRS::Vector3<float>& _vAmbient);

			void RenderMesh(
				const MeshComponent& _mesh,
				ShaderComponent& _shader, 
				Entity _idEntity,
				const MaterialComponent& _material = MaterialComponent(),
				const TransformComponent& _transform = TransformComponent(),
				const CameraComponent& _camera = CameraComponent());
	};
}

#endif