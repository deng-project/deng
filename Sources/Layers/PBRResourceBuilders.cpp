// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: PBRResourceBuilders.cpp - implementations for PBR resource builders
// author: Karl-Mihkel Ott

#define PBR_RESOURCE_BUILDERS_CPP
#include "deng/Layers/PBRResourceBuilders.h"

namespace DENG {
	size_t PBRSphereBuilder::m_uIndicesOffset = SIZE_MAX;
	size_t PBRSphereBuilder::m_uVertexOffset = SIZE_MAX;
	size_t PBRSphereBuilder::m_uIndicesCount = SIZE_MAX;

	using _Vertices = std::vector<Vertex<TRS::Vector3<float>, TRS::Vector3<float>, TRS::Vector2<float>>>;
	_Vertices PBRSphereBuilder::_GenerateSphereVertices() {
		_Vertices vertices;
		for (uint32_t x = 0; x <= SPHERE_SEGMENTS; ++x) {
			for (uint32_t y = 0; y <= SPHERE_SEGMENTS; ++y) {
				float fXSegment = static_cast<float>(x) / static_cast<float>(SPHERE_SEGMENTS);
				float fYSegment = static_cast<float>(y) / static_cast<float>(SPHERE_SEGMENTS);

				float fXPos = std::cosf(fXSegment * 2.f * MF_PI) * std::sinf(fYSegment * MF_PI);
				float fYPos = std::cosf(fYSegment * MF_PI);
				float fZPos = std::sinf(fXSegment * 2.f * MF_PI) * std::sinf(fYSegment * MF_PI);

				vertices.push_back(Vertex<TRS::Vector3<float>, TRS::Vector3<float>, TRS::Vector2<float>>{
					TRS::Vector3<float>(fXPos, fYPos, fZPos),
					TRS::Vector3<float>(fXPos, fYPos, fZPos),
					TRS::Vector2<float>(fXSegment, fYSegment)
				});
			}
		}

		return vertices;
	}


	std::vector<uint32_t> PBRSphereBuilder::_GenerateSphereIndices() {
		std::vector<uint32_t> indices;

		for (uint32_t y = 0; y < SPHERE_SEGMENTS; ++y) {
			for (uint32_t x = 0; x < SPHERE_SEGMENTS; ++x) {
				indices.push_back(y * (SPHERE_SEGMENTS + 1) + x);
				indices.push_back((y + 1) * (SPHERE_SEGMENTS + 1) + x);
				indices.push_back(y * (SPHERE_SEGMENTS + 1) + (x + 1));

				indices.push_back((y + 1) * (SPHERE_SEGMENTS + 1) + x);
				indices.push_back((y + 1) * (SPHERE_SEGMENTS + 1) + (x + 1));
				indices.push_back(y * (SPHERE_SEGMENTS + 1) + (x + 1));
			}
		}

		return indices;
	}


	MeshCommands PBRSphereBuilder::Get() {
		using _Vertex = Vertex<TRS::Vector3<float>, TRS::Vector3<float>, TRS::Vector2<float>>;

		if (m_uIndicesOffset == SIZE_MAX && m_uVertexOffset == SIZE_MAX) {
			auto vertices = _GenerateSphereVertices();
			auto indices = _GenerateSphereIndices();

			m_uIndicesCount = indices.size();

			m_uVertexOffset = m_pRenderer->AllocateMemory(vertices.size() * sizeof(_Vertex), BufferDataType::Vertex);
			m_pRenderer->UpdateBuffer(vertices.data(), vertices.size() * sizeof(_Vertex), m_uVertexOffset);

			m_uIndicesOffset = m_pRenderer->AllocateMemory(indices.size() * sizeof(uint32_t), BufferDataType::Index);
			m_pRenderer->UpdateBuffer(indices.data(), indices.size() * sizeof(uint32_t), m_uIndicesOffset);
		}

		MeshCommands meshCommands;
		meshCommands.drawCommands.emplace_back();
		meshCommands.drawCommands.back().uDrawCount = static_cast<uint32_t>(m_uIndicesCount);
		meshCommands.drawCommands.back().attributeOffsets.push_back(m_uVertexOffset + offsetof(_Vertex, val));
		meshCommands.drawCommands.back().attributeOffsets.push_back(m_uVertexOffset + offsetof(_Vertex, next.val));
		meshCommands.drawCommands.back().attributeOffsets.push_back(m_uVertexOffset + offsetof(_Vertex, next.next.val));
		meshCommands.drawCommands.back().uIndicesOffset = m_uIndicesOffset;

		return meshCommands;
	}

	
	IShader* PBRShaderBuilder::Get() {
		FileSystemShader* pShader = new FileSystemShader("PBR", "PBR");
		pShader->PushAttributeType(VertexAttributeType::Vec3_Float);
		pShader->PushAttributeType(VertexAttributeType::Vec3_Float);
		pShader->PushAttributeType(VertexAttributeType::Vec2_Float);

		pShader->PushAttributeStride(8u * sizeof(float));
		pShader->PushAttributeStride(8u * sizeof(float));
		pShader->PushAttributeStride(8u * sizeof(float));

		pShader->SetProperty(ShaderPropertyBit_EnableDepthTesting |
							 ShaderPropertyBit_EnableBlend |
							 ShaderPropertyBit_EnablePushConstants |
							 ShaderPropertyBit_EnableIndexing);
		pShader->SetPushConstant(0, ShaderStageBit_Vertex | ShaderStageBit_Fragment, nullptr);
		pShader->SetPipelineCullMode(PipelineCullMode::None);

		// [DrawDescriptorIndices]
		pShader->PushUniformDataLayout(UniformDataType::StorageBuffer, ShaderStageBit_Vertex | ShaderStageBit_Fragment, 0);
		// [TransformComponent]
		pShader->PushUniformDataLayout(UniformDataType::StorageBuffer, ShaderStageBit_Vertex, 1);
		// [PointLightComponent]
		pShader->PushUniformDataLayout(UniformDataType::StorageBuffer, ShaderStageBit_Fragment, 2);
		// [DirectionalLightComponent]
		pShader->PushUniformDataLayout(UniformDataType::StorageBuffer, ShaderStageBit_Fragment, 3);
		// [SpotlightComponent]
		pShader->PushUniformDataLayout(UniformDataType::StorageBuffer, ShaderStageBit_Fragment, 4);
		// [Material]
		pShader->PushUniformDataLayout(UniformDataType::StorageBuffer, ShaderStageBit_Fragment, 5);

		return pShader;
	}
}