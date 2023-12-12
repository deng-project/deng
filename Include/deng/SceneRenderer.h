// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: SceneRenderer.h - scene renderer class header
// author: Karl-Mihkel Ott

#ifndef SCENE_RENDERER_H
#define SCENE_RENDERER_H

#include "deng/Api.h"
#include "deng/IRenderer.h"
#include "deng/RenderResources.h"
#include "deng/Components.h"
#include "deng/AssetManager.h"
#include "deng/IGraphicsMemoryAllcator.h"

#ifdef SCENE_RENDERER_CPP
	#include "trs/Vector.h"
#endif

namespace DENG {

	class DENG_API SceneRenderer {
		private:
			AssetCollection m_assetCollection;

			IRenderer* m_pRenderer = nullptr;
			IFramebuffer* m_pFramebuffer = nullptr;

			// [0] - point lights offset
			// [1] - directional lights offset
			// [2] - spot lights offset
			std::array<size_t, 3> m_arrLightOffsets = { SIZE_MAX, 0, 0 };
			size_t m_uUsedLightsSize = 0;

			// asset pipeline
			size_t m_uTransformsOffset = 0;
			size_t m_uTransformsSize = 0;
			size_t m_uPbrMaterialsOffset = 0;
			size_t m_uPbrMaterialsSize = 0;
			size_t m_uPhongMaterialsOffset = 0;
			size_t m_uPhongMaterialsSize = 0;
			size_t m_uDrawDescriptorIndicesOffset = 0;
			size_t m_uDrawDescriptorIndicesCount = 0;
			size_t m_uBatchCounter = 0;

			// skybox
			size_t m_uSkyboxScaleOffset = 0;

			char* m_pIntermediateStorageBuffer = nullptr;
			size_t m_uIntermediateStorageBufferSize = 0;

		public:
			SceneRenderer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer, entt::registry& _registry);
			~SceneRenderer();

			void UpdateLight(Entity _ent);
			void SubmitWorkGroups();
			void UpdateSkyboxScale(const TRS::Vector4<float>& _vScale);
			void RenderSkybox(const CameraComponent& _camera, const SkyboxComponent& _skybox);

			inline void Finalize() {
				m_uBatchCounter = 0;
			}
	};
}

#endif