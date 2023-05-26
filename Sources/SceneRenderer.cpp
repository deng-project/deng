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
		m_uCameraOffset = m_pRenderer->AllocateMemory(sizeof(CameraComponent), BufferDataType::UNIFORM);
	}

	void SceneRenderer::RenderLights(const std::vector<LightComponent>& _lights) {
		// check if area reallocation is required
		if (_lights.size() > m_uLightCount) {
			m_uLightCount = _lights.size();

			if (m_uLightsOffset != SIZE_MAX) // aka light uniforms have been previously allocated
				m_pRenderer->DeallocateMemory(m_uLightsOffset);
			m_uLightsOffset = m_pRenderer->AllocateMemory(m_uLightCount * sizeof(LightComponent), BufferDataType::UNIFORM);
		}

		m_pRenderer->UpdateBuffer(_lights.data(), _lights.size() * sizeof(LightComponent), m_uLightsOffset);
	}

	void SceneRenderer::RenderMesh(
		const MeshComponent& _mesh,
		ShaderComponent& _shader,
		Entity _idEntity,
		const TransformComponent& _transform) 
	{
		DENG_ASSERT(_shader.uboDataLayouts.size() >= 3);

		// check if transform offset has not been allocated
		const size_t cuEntity = static_cast<size_t>(_idEntity);
		if (cuEntity >= m_transformOffsets.size() || m_transformOffsets[cuEntity] == SIZE_MAX) {
			m_transformOffsets.resize(cuEntity + 1, SIZE_MAX);
			m_transformOffsets[cuEntity] = m_pRenderer->AllocateMemory(sizeof(TransformComponent), BufferDataType::UNIFORM);
		}

		m_pRenderer->UpdateBuffer(&_transform, sizeof(TransformComponent), m_transformOffsets[cuEntity]);

		_shader.uboDataLayouts[0].block.uOffset = static_cast<uint32_t>(m_uCameraOffset);
		_shader.uboDataLayouts[0].block.uSize = static_cast<uint32_t>(sizeof(CameraComponent));
		_shader.uboDataLayouts[1].block.uOffset = static_cast<uint32_t>(m_transformOffsets[cuEntity]);
		_shader.uboDataLayouts[1].block.uSize = static_cast<uint32_t>(sizeof(TransformComponent));
		_shader.uboDataLayouts[2].block.uOffset = static_cast<uint32_t>(m_uLightsOffset);
		_shader.uboDataLayouts[2].block.uSize = static_cast<uint32_t>(m_uLightCount * sizeof(LightComponent));

		m_pRenderer->DrawMesh(_mesh, _shader, m_pFramebuffer, {});
	}
}