// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: SceneRenderer.h - scene renderer class header
// author: Karl-Mihkel Ott

#ifndef SCENE_RENDERER_H
#define SCENE_RENDERER_H

#include "deng/Api.h"
#include "deng/IRenderer.h"
#include "deng/Components.h"

#include "trs/Vector.h"

namespace DENG {

	class DENG_API SceneRenderer {
		private:
			IRenderer* m_pRenderer = nullptr;
			IFramebuffer* m_pFramebuffer = nullptr;
			std::vector<size_t> m_transformOffsets;
			std::vector<size_t> m_materialOffsets;
			size_t m_uLightsOffset = SIZE_MAX;
			size_t m_uLightsCount = 0;
			size_t m_uCameraOffset = 0;

		public:
			SceneRenderer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer);
			
			inline void RenderCamera(const CameraComponent& _camera) {
				m_pRenderer->UpdateBuffer(&_camera, sizeof(CameraComponent), m_uCameraOffset);
			}

			void RenderLights(const std::vector<LightComponent>& _lights, const TRS::Vector3<float>& _vAmbient);
			void RenderMesh(
				const MeshComponent& _mesh,
				const CameraComponent& _camera,
				ShaderComponent& _shader, 
				Entity _idEntity,
				const MaterialComponent& _material,
				const TransformComponent& _transform = TransformComponent());
	};
}

#endif