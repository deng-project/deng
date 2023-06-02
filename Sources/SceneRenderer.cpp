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
		delete[] m_pLightBuffer;
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

		if (m_uUsedLightsSize > m_uLightBufferSize) {
			delete[] m_pLightBuffer;
			m_uLightBufferSize = (m_uUsedLightsSize * 3) >> 1;
			m_pLightBuffer = new char[m_uLightBufferSize]{};

			// deallocate and allocate on device memory
			if (m_arrLightOffsets[0] != SIZE_MAX)
				m_pRenderer->DeallocateMemory(m_arrLightOffsets[0]);
			m_arrLightOffsets[0] = m_pRenderer->AllocateMemory(m_uLightBufferSize, BufferDataType::UNIFORM);
		}

		size_t uOffset = 0;
		m_arrLightOffsets[1] = m_arrLightOffsets[0];
		m_arrLightOffsets[2] = m_arrLightOffsets[0];

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			TRS::Vector4<float> vCombined = { _vAmbient[0], _vAmbient[1], _vAmbient[2], 0.f };
			std::memcpy(m_pLightBuffer + uOffset, &vCombined, sizeof(TRS::Vector4<float>));
			uOffset += sizeof(TRS::Vector4<float>);
			TRS::Vector4<uint32_t> vLightCounts = {
				static_cast<uint32_t>(_pointLights.size()),
				static_cast<uint32_t>(_dirLights.size()),
				static_cast<uint32_t>(_spotLights.size()),
				0
			};
			std::memcpy(m_pLightBuffer + uOffset, &vLightCounts, sizeof(TRS::Vector4<uint32_t>));
			uOffset += sizeof(TRS::Vector4<uint32_t>);

			for (const PointLightComponent& pointLight : _pointLights) {
				std::memcpy(m_pLightBuffer + uOffset, &pointLight, sizeof(PointLightComponent));
				uOffset += sizeof(PointLightComponent);
			}

			if (i == 0) m_arrLightOffsets[1] += uOffset;
			
			for (const DirectionalLightComponent& dirLight : _dirLights) {
				std::memcpy(m_pLightBuffer + uOffset, &dirLight, sizeof(DirectionalLightComponent));
				uOffset += sizeof(DirectionalLightComponent);
			}

			if (i == 0) m_arrLightOffsets[2] += uOffset;

			for (const SpotlightComponent& spotLight : _spotLights) {
				std::memcpy(m_pLightBuffer + uOffset, &spotLight, sizeof(SpotlightComponent));
				uOffset += sizeof(SpotlightComponent);
			}
		}

		m_pRenderer->UpdateBuffer(m_pLightBuffer, m_uUsedLightsSize, m_arrLightOffsets[0]);
	}

	void SceneRenderer::RenderMesh(
		const MeshComponent& _mesh,
		ShaderComponent& _shader,
		Entity _idEntity,
		const MaterialComponent& _material,
		const TransformComponent& _transform,
		const CameraComponent& _camera) 
	{
		DENG_ASSERT(_shader.bEnablePushConstants);

		// check if transform offset has not been allocated
		const size_t cuEntity = static_cast<size_t>(_idEntity);

		_shader.uPushConstantDataLength = static_cast<uint32_t>(sizeof(CameraComponent));
		_shader.pPushConstantData = &_camera;
		
		// use custom transforms
		if (_shader.uboDataLayouts.size() >= 1) {
			// allocate memory for custom transforms
			if (cuEntity >= m_transformOffsets.size() || m_transformOffsets[cuEntity] == SIZE_MAX) {
				if (cuEntity >= m_transformOffsets.size())
					m_transformOffsets.resize(cuEntity + 1, SIZE_MAX);
				m_transformOffsets[cuEntity] = m_pRenderer->AllocateMemory(sizeof(TransformComponent), BufferDataType::UNIFORM);
			}
			m_pRenderer->UpdateBuffer(&_transform, sizeof(TransformComponent), m_transformOffsets[cuEntity]);

			_shader.uboDataLayouts[0].block.uOffset = static_cast<uint32_t>(m_transformOffsets[cuEntity]);
			_shader.uboDataLayouts[0].block.uSize = static_cast<uint32_t>(sizeof(TransformComponent));

			// use lights and materials
			if (_shader.uboDataLayouts.size() >= 7) {
				DENG_ASSERT(m_arrLightOffsets[0] != SIZE_MAX);

				if (cuEntity >= m_materialOffsets.size() || m_materialOffsets[cuEntity] == SIZE_MAX) {
					if (cuEntity >= m_materialOffsets.size())
						m_materialOffsets.resize(cuEntity + 1, SIZE_MAX);
					m_materialOffsets[cuEntity] = m_pRenderer->AllocateMemory(sizeof(MaterialComponent), BufferDataType::UNIFORM);
				}
				m_pRenderer->UpdateBuffer(&_material, sizeof(MaterialComponent), m_materialOffsets[cuEntity]);

				_shader.uboDataLayouts[1].block.uOffset = static_cast<uint32_t>(m_arrLightOffsets[0]);
				_shader.uboDataLayouts[1].block.uSize = static_cast<uint32_t>(m_uUsedLightsSize >> 1);
				_shader.uboDataLayouts[2].block.uOffset = static_cast<uint32_t>(m_arrLightOffsets[1]);
				_shader.uboDataLayouts[2].block.uSize = static_cast<uint32_t>(m_uUsedLightsSize >> 1);
				_shader.uboDataLayouts[3].block.uOffset = static_cast<uint32_t>(m_arrLightOffsets[2]);
				_shader.uboDataLayouts[3].block.uSize = static_cast<uint32_t>(m_uUsedLightsSize >> 1);
				_shader.uboDataLayouts[4].block.uOffset = static_cast<uint32_t>(m_materialOffsets[cuEntity]);
				_shader.uboDataLayouts[4].block.uSize = static_cast<uint32_t>(sizeof(MaterialComponent));
			}
		}

		std::vector<uint32_t> textureIds;
		if (_material.vMaps[0] && _material.vMaps[1]) {
			textureIds.push_back(_material.vMaps[0]);
			textureIds.push_back(_material.vMaps[1]);
		}
		m_pRenderer->DrawMesh(_mesh, _shader, m_pFramebuffer, textureIds);
	}
}