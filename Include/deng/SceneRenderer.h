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

			std::array<size_t, 3> m_arrLightOffsets = { SIZE_MAX, 0, 0 };
			size_t m_uUsedLightsSize = 0;

			std::vector<size_t> m_batchSSBOOffsets;
			std::vector<size_t> m_batchSSBOSizes;
			size_t m_uBatchCounter = 0;

			char* m_pIntermediateStorageBuffer = nullptr;
			size_t m_uIntermediateStorageBufferSize = 0;

		public:
			SceneRenderer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer);
			~SceneRenderer();

			void RenderLights(
				const std::vector<PointLightComponent>& _pointLights, 
				const std::vector<DirectionalLightComponent>& _dirLights, 
				const std::vector<SpotlightComponent>& _spotLights,
				const TRS::Vector3<float>& _vAmbient);
			
			void RenderBatch(const MeshComponent& _mesh, 
							 ShaderComponent& _shader, 
							 const std::vector<MaterialComponent>& _materials,
							 const std::vector<TransformComponent>& _transform,
							 const CameraComponent& _camera);

			inline void Finalize() {
				m_uBatchCounter = 0;
			}
	};
}

#endif