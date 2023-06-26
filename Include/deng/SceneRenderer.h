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

#ifdef SCENE_RENDERER_CPP
	#include "trs/Vector.h"
#endif

namespace DENG {

	class DENG_API SceneRenderer {
		private:
			IRenderer* m_pRenderer = nullptr;
			IFramebuffer* m_pFramebuffer = nullptr;

			// [0] - point lights offset
			// [1] - directional lights offset
			// [2] - spot lights offset
			std::array<size_t, 3> m_arrLightOffsets = { SIZE_MAX, 0, 0 };
			size_t m_uUsedLightsSize = 0;

			size_t m_uTransformsOffset = 0;
			size_t m_uTransformsSize = 0;
			size_t m_uMaterialsOffset = 0;
			size_t m_uMaterialsSize = 0;
			size_t m_uDrawDescriptorIndicesOffset = 0;
			size_t m_uDrawDescriptorIndicesCount = 0;
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
			
			void RenderInstances(const std::vector<InstanceInfo>& _instanceInfos, 
								 const std::vector<TransformComponent>& _transforms,
								 const std::vector<Material>& _materials,
								 const std::vector<DrawDescriptorIndices>& _drawDescriptorIndices,
								 const CameraComponent& _camera);

			void UpdateTransformRegion(const TransformComponent* _pData, std::size_t _uDstOffset, std::size_t _uCount);
			void UpdateDirLightRegion(const DirectionalLightComponent* _pData, std::size_t _uDstOffset, std::size_t _uCount);
			void UpdatePointLightRegion(const PointLightComponent* _pData, std::size_t _uDstOffset, std::size_t _uCount);
			void UpdateSpotLightRegion(const SpotlightComponent* _pData, std::size_t _uDstOffset, std::size_t _uCount);

			void UpdateStorageBuffers(const std::vector<TransformComponent>& _transforms,
									  const std::vector<Material>& _materials,
									  const std::vector<DrawDescriptorIndices>& _drawDescriptorIndices);

			inline void Finalize() {
				m_uBatchCounter = 0;
			}
	};
}

#endif