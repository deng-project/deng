// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: GrassResourceBuilders.cpp - implementation for grass resource builders
// author: Karl-Mihkel Ott

#define GRASS_RESOURCE_BUILDERS_CPP
#include "GrassResourceBuilders.h"

#define GRASS_BOUND 20.f

namespace Application {

	DENG::MeshCommands GrassMeshBuilder::Get() {
		std::vector<TRS::Vector3<float>> positions;
		for (float x = -GRASS_BOUND; x < GRASS_BOUND; x += 0.2f) {
			for (float z = -GRASS_BOUND; z < GRASS_BOUND; z += 0.2f) {
				positions.emplace_back(x, 0.f, z);
			}
		}

		std::size_t uOffset = m_pRenderer->AllocateMemory(positions.size() * sizeof(TRS::Vector3<float>), DENG::BufferDataType::Vertex);
		m_pRenderer->UpdateBuffer(positions.data(), positions.size() * sizeof(TRS::Vector3<float>), uOffset);

		DENG::MeshCommands meshCommands;
		meshCommands.sName = "Grass";
		meshCommands.drawCommands.emplace_back();
		meshCommands.drawCommands.back().attributeOffsets.push_back(uOffset);
		meshCommands.drawCommands.back().uDrawCount = static_cast<uint32_t>(positions.size());
		
		return meshCommands;
	}

	DENG::IShader* GrassShaderBuilder::Get() {
		DENG::FileSystemShader* pShader = new DENG::FileSystemShader("Grass", "Grass");
		pShader->PushAttributeType(DENG::VertexAttributeType::Vec3_Float);
		pShader->PushAttributeStride(sizeof(TRS::Vector3<float>));

		pShader->SetProperty(DENG::ShaderPropertyBit_EnableDepthTesting |
							 DENG::ShaderPropertyBit_EnableBlend |
							 DENG::ShaderPropertyBit_EnablePushConstants |
							 DENG::ShaderPropertyBit_NonStandardShader);
		pShader->SetPushConstant(0, DENG::ShaderStageBit_Geometry, nullptr);
		pShader->SetPrimitiveMode(DENG::PrimitiveMode::Points);

		pShader->PushUniformDataLayout(DENG::UniformDataType::ImageSampler2D, DENG::ShaderStageBit_Fragment, 0);
		pShader->PushUniformDataLayout(DENG::UniformDataType::ImageSampler2D, DENG::ShaderStageBit_Geometry, 1);
		pShader->PushUniformDataLayout(DENG::UniformDataType::Buffer, 
									   DENG::ShaderStageBit_Geometry, 
									   2, 
									   static_cast<uint32_t>(sizeof(float)), 
									   static_cast<uint32_t>(m_uTimerOffset));

		pShader->PushTextureHash(m_hshGrassTexture);
		pShader->PushTextureHash(m_hshWindTexture);
		return pShader;
	}
}