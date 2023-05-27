// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: SceneRenderer.cpp - scene renderer class implementation
// author: Karl-Mihkel Ott

#define SCENE_RENDERER_CPP
#include "deng/SceneRenderer.h"

namespace DENG {

	SceneRenderer::SceneRenderer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer) :
		m_pRenderer(_pRenderer),
		m_pFramebuffer(_pFramebuffer) 
	{
		m_uCameraOffset = _pRenderer->AllocateMemory(sizeof(CameraComponent), BufferDataType::UNIFORM);
	}

	void SceneRenderer::RenderLights(const std::vector<LightComponent>& _lights) {
		// check if area reallocation is required
		if (_lights.size() > static_cast<size_t>(m_uLightsCount)) {
			m_uLightsCount = static_cast<uint32_t>(_lights.size());

			if (m_uLightsOffset != SIZE_MAX) // aka light uniforms have been previously allocated
				m_pRenderer->DeallocateMemory(m_uLightsOffset);
			m_uLightsOffset = m_pRenderer->AllocateMemory(MAX_FRAMES_IN_FLIGHT * (m_uLightsCount * sizeof(LightComponent) + sizeof(TRS::Vector4<uint32_t>)), BufferDataType::UNIFORM);
		}

		size_t uOffset = m_uLightsOffset;
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			m_pRenderer->UpdateBuffer(&m_uLightsCount, sizeof(uint32_t), uOffset);
			uOffset += sizeof(TRS::Vector4<uint32_t>);
			m_pRenderer->UpdateBuffer(_lights.data(), _lights.size() * sizeof(LightComponent), uOffset);
			uOffset += _lights.size() * sizeof(LightComponent);
		}
	}

	void SceneRenderer::RenderMesh(
		const MeshComponent& _mesh,
		const CameraComponent& _camera,
		ShaderComponent& _shader,
		Entity _idEntity,
		const TransformComponent& _transform) 
	{
		DENG_ASSERT(_shader.uboDataLayouts.size() >= 2);
		DENG_ASSERT(_shader.bEnablePushConstants);

		// check if transform offset has not been allocated
		const size_t cuEntity = static_cast<size_t>(_idEntity);
		if (cuEntity >= m_transformOffsets.size() || m_transformOffsets[cuEntity] == SIZE_MAX) {
			m_transformOffsets.resize(cuEntity + 1, SIZE_MAX);
			m_transformOffsets[cuEntity] = m_pRenderer->AllocateMemory(sizeof(TransformComponent), BufferDataType::UNIFORM);
		}

		m_pRenderer->UpdateBuffer(&_transform, sizeof(TransformComponent), m_transformOffsets[cuEntity]);

		_shader.uPushConstantDataLength = static_cast<uint32_t>(sizeof(CameraComponent));
		_shader.pPushConstantData = &_camera;

		_shader.uboDataLayouts[0].block.uOffset = static_cast<uint32_t>(m_transformOffsets[cuEntity]);
		_shader.uboDataLayouts[0].block.uSize = static_cast<uint32_t>(sizeof(TransformComponent));
		_shader.uboDataLayouts[1].block.uOffset = static_cast<uint32_t>(m_uLightsOffset);
		_shader.uboDataLayouts[1].block.uSize = static_cast<uint32_t>(m_uLightsCount * sizeof(LightComponent) + sizeof(TRS::Vector4<uint32_t>));

		m_pRenderer->DrawMesh(_mesh, _shader, m_pFramebuffer, {});
	}
}