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
			m_arrLightOffsets[0] = m_pRenderer->AllocateMemory(m_uIntermediateStorageBufferSize, BufferDataType::Uniform);
		}

		size_t uOffset = 0;
		m_arrLightOffsets[1] = m_arrLightOffsets[0];
		m_arrLightOffsets[2] = m_arrLightOffsets[0];

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			TRS::Vector4<float> vAmbient = { _vAmbient[0], _vAmbient[1], _vAmbient[2], 0.f };
			std::memcpy(m_pIntermediateStorageBuffer + uOffset, &vAmbient, sizeof(TRS::Vector4<float>));
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


	void SceneRenderer::RenderInstances(
		const std::vector<InstanceInfo>& _instanceInfos,
		const std::vector<TransformComponent>& _transforms,
		const std::vector<Material>& _materials,
		const std::vector<DrawDescriptorIndices>& _drawDescriptorIndices,
		const CameraComponent& _camera)
	{
		ResourceManager& resourceManager = ResourceManager::GetInstance();
		
		uint32_t uFirstInstance = 0;
		for (auto it = _instanceInfos.begin(); it != _instanceInfos.end(); it++) {
			IShader* pShader = resourceManager.GetShader(it->hshShader);
			DENG_ASSERT(pShader);

			auto& uniformDataLayouts = pShader->GetUniformDataLayouts();
		
			if (uniformDataLayouts.size() >= 2 && !pShader->IsPropertySet(ShaderPropertyBit_NonStandardShader)) {
				// draw descriptor indices
				uniformDataLayouts[0].block.uOffset = static_cast<uint32_t>(m_uDrawDescriptorIndicesOffset);
				uniformDataLayouts[0].block.uSize = static_cast<uint32_t>(_drawDescriptorIndices.size() * sizeof(DrawDescriptorIndices));
			
				// transforms
				uniformDataLayouts[1].block.uOffset = static_cast<uint32_t>(m_uTransformsOffset);
				uniformDataLayouts[1].block.uSize = static_cast<uint32_t>(_transforms.size() * sizeof(TransformComponent));

				if (uniformDataLayouts.size() >= 6) {
					// lights
					uniformDataLayouts[2].block.uOffset = static_cast<uint32_t>(m_arrLightOffsets[0]);
					uniformDataLayouts[2].block.uSize = static_cast<uint32_t>(m_uUsedLightsSize / MAX_FRAMES_IN_FLIGHT);
					uniformDataLayouts[3].block.uOffset = static_cast<uint32_t>(m_arrLightOffsets[1]);
					uniformDataLayouts[3].block.uSize = static_cast<uint32_t>(m_uUsedLightsSize / MAX_FRAMES_IN_FLIGHT);
					uniformDataLayouts[4].block.uOffset = static_cast<uint32_t>(m_arrLightOffsets[2]);
					uniformDataLayouts[4].block.uSize = static_cast<uint32_t>(m_uUsedLightsSize / MAX_FRAMES_IN_FLIGHT);

					// material
					uniformDataLayouts[5].block.uOffset = static_cast<uint32_t>(m_uMaterialsOffset);
					uniformDataLayouts[5].block.uSize = static_cast<uint32_t>(_materials.size() * sizeof(Material));
				}
			}

			if (pShader->IsPropertySet(ShaderPropertyBit_EnablePushConstants)) {
				auto& pushConstant = pShader->GetPushConstant();
				pushConstant.uLength = sizeof(CameraComponent);
				pushConstant.pPushConstantData = &_camera;
			}

			m_pRenderer->DrawInstance(it->hshMesh, it->hshShader, m_pFramebuffer, it->uInstanceCount, uFirstInstance, it->hshMaterial);
			uFirstInstance += it->uInstanceCount;
		}
	}

	void SceneRenderer::UpdateTransformRegion(const TransformComponent* _pData, std::size_t _uDstOffset, std::size_t _uCount) {
		DENG_ASSERT(_uDstOffset + _uCount < m_uTransformsSize);
		m_pRenderer->UpdateBuffer(_pData, _uCount * sizeof(TransformComponent), m_uTransformsOffset + _uDstOffset * sizeof(TransformComponent));
	}


	void SceneRenderer::UpdateDirLightRegion(const DirectionalLightComponent* _pData, std::size_t _uDstOffset, std::size_t _uCount) {
		DENG_ASSERT(_uDstOffset + _uCount < m_arrLightOffsets[1]);
		m_pRenderer->UpdateBuffer(_pData, _uCount * sizeof(DirectionalLightComponent), m_arrLightOffsets[1] + _uDstOffset * sizeof(DirectionalLightComponent));
	}


	void SceneRenderer::UpdatePointLightRegion(const PointLightComponent* _pData, std::size_t _uDstOffset, std::size_t _uCount) {
		DENG_ASSERT(_uDstOffset + _uCount < m_arrLightOffsets[0]);
		m_pRenderer->UpdateBuffer(_pData, _uCount * sizeof(PointLightComponent), m_arrLightOffsets[0] + _uDstOffset * sizeof(PointLightComponent));
	}


	void SceneRenderer::UpdateSpotLightRegion(const SpotlightComponent* _pData, std::size_t _uDstOffset, std::size_t _uCount) {
		DENG_ASSERT(_uDstOffset + _uCount < m_arrLightOffsets[2]);
		m_pRenderer->UpdateBuffer(_pData, _uCount * sizeof(SpotlightComponent), m_arrLightOffsets[2] + _uDstOffset * sizeof(SpotlightComponent));
	}


	void SceneRenderer::UpdateStorageBuffers(
		const std::vector<TransformComponent>& _transforms, 
		const std::vector<Material>& _materials, 
		const std::vector<DrawDescriptorIndices>& _drawDescriptorIndices)
	{
		if (m_uMaterialsSize < _materials.size() * MAX_FRAMES_IN_FLIGHT) {
			m_uMaterialsSize = (_materials.size() * MAX_FRAMES_IN_FLIGHT * 3) >> 1;
			if (m_uMaterialsOffset) {
				m_pRenderer->DeallocateMemory(m_uMaterialsOffset);
			}

			m_uMaterialsOffset = m_pRenderer->AllocateMemory(m_uMaterialsSize * sizeof(Material), BufferDataType::Uniform);
		}

		if (_materials.size()) {
			size_t uOffset = m_uMaterialsOffset;
			for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
				m_pRenderer->UpdateBuffer(_materials.data(), _materials.size() * sizeof(Material), uOffset);
				uOffset += _materials.size() * sizeof(Material);
			}
		}

		if (m_uTransformsSize < _transforms.size()) {
			m_uTransformsSize = (_transforms.size() * 3) >> 1;
			if (m_uTransformsOffset) {
				m_pRenderer->DeallocateMemory(m_uTransformsOffset);
			}

			m_uTransformsOffset = m_pRenderer->AllocateMemory(m_uTransformsSize * sizeof(TransformComponent), BufferDataType::Uniform);
		}

		if (_transforms.size()) {
			size_t uOffset = m_uTransformsOffset;
			for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
				m_pRenderer->UpdateBuffer(_transforms.data(), _transforms.size() * sizeof(TransformComponent), uOffset);
				uOffset += _transforms.size() * sizeof(TransformComponent);
			}
		}

		if (m_uDrawDescriptorIndicesCount < _drawDescriptorIndices.size()) {
			m_uDrawDescriptorIndicesCount = (_drawDescriptorIndices.size() * 3) >> 1;
			if (m_uDrawDescriptorIndicesOffset) {
				m_pRenderer->DeallocateMemory(m_uDrawDescriptorIndicesOffset);
			}

			m_uDrawDescriptorIndicesOffset = m_pRenderer->AllocateMemory(m_uDrawDescriptorIndicesCount * sizeof(DrawDescriptorIndices), BufferDataType::Uniform);
		}

		if (_drawDescriptorIndices.size()) {
			size_t uOffset = m_uDrawDescriptorIndicesOffset;
			for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
				m_pRenderer->UpdateBuffer(_drawDescriptorIndices.data(), _drawDescriptorIndices.size() * sizeof(DrawDescriptorIndices), uOffset);
				uOffset += _drawDescriptorIndices.size() * sizeof(DrawDescriptorIndices);
			}
		}
	}

	void SceneRenderer::UpdateSkyboxScale(const TRS::Vector4<float>& _vScale) {
		if (!m_uSkyboxScaleOffset)
			m_uSkyboxScaleOffset = m_pRenderer->AllocateMemory(sizeof(TRS::Vector4<float>), BufferDataType::Vertex);
	
		m_pRenderer->UpdateBuffer(&_vScale, sizeof(TRS::Vector4<float>), m_uSkyboxScaleOffset);
	}

	void SceneRenderer::RenderSkybox(const CameraComponent& _camera, const SkyboxComponent& _skybox) {
		ResourceManager& resourceManager = ResourceManager::GetInstance();
		auto pShader = resourceManager.GetShader(_skybox.hshShader);
		DENG_ASSERT(pShader);
		DENG_ASSERT(m_uSkyboxScaleOffset != SIZE_MAX);

		pShader->GetUniformDataLayouts()[0].block.uOffset = static_cast<uint32_t>(m_uSkyboxScaleOffset);
		pShader->GetUniformDataLayouts()[0].block.uSize = static_cast<uint32_t>(sizeof(TRS::Vector4<float>));
		
		pShader->GetPushConstant().uLength = sizeof(CameraComponent);
		pShader->GetPushConstant().pPushConstantData = &_camera;
		m_pRenderer->DrawInstance(_skybox.hshMesh, _skybox.hshShader, m_pFramebuffer, 1);
	}
}