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


	void SceneRenderer::RenderBatch(
		const MeshComponent& _mesh,
		ShaderComponent& _shader,
		const std::vector<MaterialComponent>& _materials,
		const std::vector<TransformComponent>& _transforms,
		const CameraComponent& _camera)
	{
		DENG_ASSERT(m_batchSSBOOffsets.size() == m_batchSSBOSizes.size());
		DENG_ASSERT(_materials.size() == _transforms.size());

		const size_t cuSize = MAX_FRAMES_IN_FLIGHT * (_materials.size() * sizeof(MaterialComponent) + _transforms.size() * sizeof(TransformComponent));

		// reallocation, reallocation, reallocation
		if (m_uBatchCounter >= m_batchSSBOOffsets.size()) {
			m_batchSSBOOffsets.push_back(
				m_pRenderer->AllocateMemory(cuSize, BufferDataType::UNIFORM));
			m_batchSSBOSizes.push_back(cuSize);
		}
		else if (m_batchSSBOSizes[m_uBatchCounter] < cuSize) {
			m_pRenderer->DeallocateMemory(m_batchSSBOOffsets[m_uBatchCounter]);
			m_batchSSBOOffsets[m_uBatchCounter] = m_pRenderer->AllocateMemory(cuSize, BufferDataType::UNIFORM);
			m_batchSSBOSizes[m_uBatchCounter] = cuSize;
		}

		// intermediate storage reallocation check
		if (m_uIntermediateStorageBufferSize < cuSize) {
			delete[] m_pIntermediateStorageBuffer;
			m_uIntermediateStorageBufferSize = (cuSize * 3) >> 1;
			m_pIntermediateStorageBuffer = new char[m_uIntermediateStorageBufferSize];
		}

		if (_shader.uboDataLayouts.size() >= 1) {
			_shader.uboDataLayouts[0].block.uOffset = static_cast<uint32_t>(m_batchSSBOOffsets[m_uBatchCounter]);
			_shader.uboDataLayouts[0].block.uSize = static_cast<uint32_t>(_transforms.size() * sizeof(TransformComponent));

			// copy to intermediate buffer
			size_t uOffset = 0;
			for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
				std::memcpy(m_pIntermediateStorageBuffer + uOffset, _transforms.data(), _transforms.size() * sizeof(TransformComponent));
				uOffset += _transforms.size() * sizeof(TransformComponent);
			}

			if (_shader.uboDataLayouts.size() >= 7) {
				_shader.uboDataLayouts[1].block.uOffset = static_cast<uint32_t>(m_arrLightOffsets[0]);
				_shader.uboDataLayouts[1].block.uSize = static_cast<uint32_t>(m_uUsedLightsSize / MAX_FRAMES_IN_FLIGHT);
				_shader.uboDataLayouts[2].block.uOffset = static_cast<uint32_t>(m_arrLightOffsets[1]);
				_shader.uboDataLayouts[2].block.uSize = static_cast<uint32_t>(m_uUsedLightsSize / MAX_FRAMES_IN_FLIGHT);
				_shader.uboDataLayouts[3].block.uOffset = static_cast<uint32_t>(m_arrLightOffsets[2]);
				_shader.uboDataLayouts[3].block.uSize = static_cast<uint32_t>(m_uUsedLightsSize / MAX_FRAMES_IN_FLIGHT);
				_shader.uboDataLayouts[4].block.uOffset = static_cast<uint32_t>(m_batchSSBOOffsets[m_uBatchCounter] + _transforms.size() * sizeof(TransformComponent) * MAX_FRAMES_IN_FLIGHT);
				_shader.uboDataLayouts[4].block.uSize = static_cast<uint32_t>(_materials.size() * sizeof(MaterialComponent));

				_shader.uPushConstantDataLength = sizeof(CameraComponent);
				_shader.pPushConstantData = &_camera;

				for (size_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
					std::memcpy(m_pIntermediateStorageBuffer + uOffset, _materials.data(), _materials.size() * sizeof(MaterialComponent));
					uOffset += _materials.size() * sizeof(MaterialComponent);
				}
			}
		}

		// check if updating buffer is necessary
		if (cuSize) {
			m_pRenderer->UpdateBuffer(m_pIntermediateStorageBuffer, cuSize, m_batchSSBOOffsets[m_uBatchCounter]);
		}

		// check if any texture were bound
		std::vector<uint32_t> textureIds = {};
		if (_materials.size() && _materials[0].vMaps[0] && _materials[0].vMaps[1]) {
			textureIds.push_back(_materials[0].vMaps[0]);
			textureIds.push_back(_materials[0].vMaps[1]);
		}

		const uint32_t uInstanceCount = static_cast<uint32_t>(_materials.size());
		m_pRenderer->DrawMesh(_mesh, _shader, m_pFramebuffer, uInstanceCount, textureIds);
	}
}