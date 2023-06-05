// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: SceneRenderer.cpp - scene renderer class implementation
// author: Karl-Mihkel Ott

#define SCENE_RENDERER_CPP
#include "deng/SceneRenderer.h"

namespace DENG {

	SceneRenderer::SceneRenderer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer) :
		m_pRenderer(_pRenderer),
		m_pFramebuffer(_pFramebuffer) {}

	SceneRenderer::~SceneRenderer() {
		delete[] m_pIntermediateStorageBuffer;
	}

	void SceneRenderer::RenderLights(
		const std::vector<PointLightComponent>& _pointLights,
		const std::vector<DirectionalLightComponent>& _dirLights,
		const std::vector<SpotlightComponent>& _spotLights,
		const TRS::Vector3<float>& _vAmbient) 
	{
		// check if intermediate buffer reallocation is required
		m_uUsedLightsSize = MAX_FRAMES_IN_FLIGHT * (_pointLights.size() * sizeof(PointLightComponent) +
			_dirLights.size() * sizeof(DirectionalLightComponent) +
			_spotLights.size() * sizeof(SpotlightComponent) + sizeof(TRS::Vector4<float>) + sizeof(TRS::Vector4<uint32_t>));

		if (m_uUsedLightsSize > m_uIntermediateStorageBufferSize) {
			delete[] m_pIntermediateStorageBuffer;
			m_uIntermediateStorageBufferSize = (m_uUsedLightsSize * 3) >> 1;
			m_pIntermediateStorageBuffer = new char[m_uIntermediateStorageBufferSize]{};

			// deallocate and allocate on device memory
			if (m_arrLightOffsets[0] != SIZE_MAX)
				m_pRenderer->DeallocateMemory(m_arrLightOffsets[0]);
			m_arrLightOffsets[0] = m_pRenderer->AllocateMemory(m_uIntermediateStorageBufferSize, BufferDataType::UNIFORM);
		}

		size_t uOffset = 0;
		m_arrLightOffsets[1] = m_arrLightOffsets[0];
		m_arrLightOffsets[2] = m_arrLightOffsets[0];

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			TRS::Vector4<float> vCombined = { _vAmbient[0], _vAmbient[1], _vAmbient[2], 0.f };
			std::memcpy(m_pIntermediateStorageBuffer + uOffset, &vCombined, sizeof(TRS::Vector4<float>));
			uOffset += sizeof(TRS::Vector4<float>);
			TRS::Vector4<uint32_t> vLightCounts = {
				static_cast<uint32_t>(_pointLights.size()),
				static_cast<uint32_t>(_dirLights.size()),
				static_cast<uint32_t>(_spotLights.size()),
				0
			};
			std::memcpy(m_pIntermediateStorageBuffer + uOffset, &vLightCounts, sizeof(TRS::Vector4<uint32_t>));
			uOffset += sizeof(TRS::Vector4<uint32_t>);

			for (const PointLightComponent& pointLight : _pointLights) {
				std::memcpy(m_pIntermediateStorageBuffer + uOffset, &pointLight, sizeof(PointLightComponent));
				uOffset += sizeof(PointLightComponent);
			}

			if (i == 0) m_arrLightOffsets[1] += uOffset;
			
			for (const DirectionalLightComponent& dirLight : _dirLights) {
				std::memcpy(m_pIntermediateStorageBuffer + uOffset, &dirLight, sizeof(DirectionalLightComponent));
				uOffset += sizeof(DirectionalLightComponent);
			}

			if (i == 0) m_arrLightOffsets[2] += uOffset;

			for (const SpotlightComponent& spotLight : _spotLights) {
				std::memcpy(m_pIntermediateStorageBuffer + uOffset, &spotLight, sizeof(SpotlightComponent));
				uOffset += sizeof(SpotlightComponent);
			}
		}

		m_pRenderer->UpdateBuffer(m_pIntermediateStorageBuffer, m_uUsedLightsSize, m_arrLightOffsets[0]);
	}


	void SceneRenderer::SubmitBatches(const CameraComponent& _camera) {
		// calculate required SSBO size
		size_t uTransformSize = 0;
		size_t uMaterialSize = 0;
		for (size_t i = 0; i < m_batches.size(); i++) {
			DENG_ASSERT(m_batches[i].pMeshComponent);
			DENG_ASSERT(m_batches[i].pShaderComponent);
			uTransformSize += m_batches[i].transforms.size();
			uMaterialSize += m_batches[i].materials.size();
		}

		uTransformSize *= sizeof(TransformComponent) * MAX_FRAMES_IN_FLIGHT;
		uMaterialSize *= sizeof(MaterialComponent) * MAX_FRAMES_IN_FLIGHT;

		// check if device memory allocation is required
		size_t uOffset = 0;
		if (uTransformSize + uMaterialSize > m_uInstancedSSBOSize) {
			if (m_uInstancedSSBOSize)
				m_pRenderer->DeallocateMemory(m_uInstancedSSBOOffset);
			m_uInstancedSSBOSize = ((uTransformSize + uMaterialSize) * 3) >> 1;

			m_uInstancedSSBOOffset = m_pRenderer->AllocateMemory(m_uInstancedSSBOSize, BufferDataType::UNIFORM);
		}
		
		uOffset = m_uInstancedSSBOOffset;

		// check if intermediate buffer reallocation is required
		if (uTransformSize + uMaterialSize > m_uIntermediateStorageBufferSize) {
			m_uIntermediateStorageBufferSize = ((uTransformSize + uMaterialSize) * 3) >> 1;
			delete[] m_pIntermediateStorageBuffer;
			m_pIntermediateStorageBuffer = new char[m_uIntermediateStorageBufferSize]{};
		}
	
		// update buffer objects
		size_t uIntermediateTransformOffset = 0;
		size_t uIntermediateMaterialOffset = uTransformSize;
		size_t uTransformOffset = uOffset;
		size_t uMaterialOffset = uTransformOffset + uTransformSize;

		for (size_t i = 0; i < m_batches.size(); i++) {
			m_batches[i].pShaderComponent->uboDataLayouts[0].block.uOffset = static_cast<uint32_t>(uTransformOffset);
			m_batches[i].pShaderComponent->uboDataLayouts[0].block.uSize = static_cast<uint32_t>(m_batches[i].transforms.size() * sizeof(TransformComponent));
			
			for (size_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
				std::memcpy(m_pIntermediateStorageBuffer + uIntermediateTransformOffset, m_batches[i].transforms.data(), m_batches[i].transforms.size() * sizeof(TransformComponent));
				uIntermediateTransformOffset += m_batches[i].transforms.size() * sizeof(TransformComponent);
			}

			uTransformOffset += m_batches[i].transforms.size() * sizeof(TransformComponent) * MAX_FRAMES_IN_FLIGHT;

			if (m_batches[i].pShaderComponent->uboDataLayouts.size() >= 7) {
				m_batches[i].pShaderComponent->uboDataLayouts[1].block.uOffset = static_cast<uint32_t>(m_arrLightOffsets[0]);
				m_batches[i].pShaderComponent->uboDataLayouts[1].block.uSize = static_cast<uint32_t>(m_uUsedLightsSize / MAX_FRAMES_IN_FLIGHT);
				m_batches[i].pShaderComponent->uboDataLayouts[2].block.uOffset = static_cast<uint32_t>(m_arrLightOffsets[1]);
				m_batches[i].pShaderComponent->uboDataLayouts[2].block.uSize = static_cast<uint32_t>(m_uUsedLightsSize / MAX_FRAMES_IN_FLIGHT);
				m_batches[i].pShaderComponent->uboDataLayouts[3].block.uOffset = static_cast<uint32_t>(m_arrLightOffsets[2]);
				m_batches[i].pShaderComponent->uboDataLayouts[3].block.uSize = static_cast<uint32_t>(m_uUsedLightsSize / MAX_FRAMES_IN_FLIGHT);
				m_batches[i].pShaderComponent->uboDataLayouts[4].block.uOffset = static_cast<uint32_t>(uMaterialOffset);
				m_batches[i].pShaderComponent->uboDataLayouts[4].block.uSize = static_cast<uint32_t>(m_batches[i].materials.size() * sizeof(MaterialComponent));

				m_batches[i].pShaderComponent->uPushConstantDataLength = sizeof(CameraComponent);								
				m_batches[i].pShaderComponent->pPushConstantData = &_camera;
				
				uMaterialOffset += m_batches[i].materials.size() * sizeof(MaterialComponent) * MAX_FRAMES_IN_FLIGHT;
				for (size_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
					std::memcpy(m_pIntermediateStorageBuffer + uIntermediateMaterialOffset, m_batches[i].materials.data(), m_batches[i].materials.size() * sizeof(MaterialComponent));
					uIntermediateMaterialOffset += m_batches[i].materials.size() * sizeof(MaterialComponent);
				}
			}
		}

		if (uTransformSize + uMaterialOffset) {
			m_pRenderer->UpdateBuffer(m_pIntermediateStorageBuffer, uTransformSize + uMaterialSize, uOffset);
		}

		// issue batch draw commands
		for (size_t i = 0; i < m_batches.size(); i++) {
			// check if diffuse and specular textures are used
			std::vector<uint32_t> textureIds;
			if (m_batches[i].materials.size()) {
				if (m_batches[i].materials.front().vMaps[0])
					textureIds.push_back(m_batches[i].materials.front().vMaps[0]);
				if (m_batches[i].materials.front().vMaps[1])
					textureIds.push_back(m_batches[i].materials.front().vMaps[1]);
			}
			m_pRenderer->DrawMesh(*m_batches[i].pMeshComponent, *m_batches[i].pShaderComponent, m_pFramebuffer, m_batches[i].uInstanceCount, textureIds);
			m_batches[i].materials.clear();
			m_batches[i].transforms.clear();
			m_batches[i].uInstanceCount = 0;
		}
	}
}