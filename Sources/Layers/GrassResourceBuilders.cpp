// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: GrassResourceBuilders.cpp - implementation for grass resource builders
// author: Karl-Mihkel Ott

#define GRASS_RESOURCE_BUILDERS_CPP
#include "deng/Layers/GrassResourceBuilders.h"

#define GRASS_BOUND 20.f

namespace DENG {

	MeshCommands GrassMeshBuilder::Get() {
		std::vector<TRS::Vector3<float>> positions;
		for (float x = -GRASS_BOUND; x < GRASS_BOUND; x += 0.2f) {
			for (float z = -GRASS_BOUND; z < GRASS_BOUND; z += 0.2f) {
				positions.emplace_back(x, 0.f, z);
			}
		}

		std::size_t uOffset = m_pRenderer->AllocateMemory(positions.size() * sizeof(TRS::Vector3<float>), BufferDataType::Vertex);
		m_pRenderer->UpdateBuffer(positions.data(), positions.size() * sizeof(TRS::Vector3<float>), uOffset);

		MeshCommands meshCommands;
		meshCommands.sName = "Grass";
		meshCommands.drawCommands.emplace_back();
		meshCommands.drawCommands.back().attributeOffsets.push_back(uOffset);
		meshCommands.drawCommands.back().uDrawCount = static_cast<uint32_t>(positions.size());
		
		return meshCommands;
	}

	IGraphicsShader* TerrainShaderBuilder::Get() {
		FileSystemGraphicsShader* pShader = new FileSystemGraphicsShader(m_pCompiler, "Terrain", "", "Terrain");
		pShader->PushAttributeType(VertexAttributeType::Vec3_Float);
		pShader->HashAttributeTypes();

		pShader->PushAttributeStride(sizeof(TRS::Vector3<float>));
		pShader->HashAttributeStrides();

		pShader->SetProperty(ShaderPropertyBit_EnableDepthTesting |
							 ShaderPropertyBit_EnableBlend |
							 ShaderPropertyBit_EnablePushConstants |
							 ShaderPropertyBit_IsNonStandardShader);
	
		pShader->SetPushConstant(0, ShaderStageBit_Geometry, nullptr);
		pShader->SetPrimitiveMode(PrimitiveMode::Points);

		pShader->PushUniformDataLayout(UniformDataType::ImageSampler2D, ShaderStageBit_Geometry, 0);
		pShader->PushUniformDataLayout(UniformDataType::ImageSampler2D, ShaderStageBit_Fragment, 1);
		pShader->HashUniformDataLayouts();

		pShader->PushTextureHash(m_hshTerrainHeightTexture);
		pShader->PushTextureHash(m_hshTerrainTexture);
		pShader->HashTextures();

		return pShader;
	}

	IGraphicsShader* GrassShaderBuilder::Get() {
		FileSystemGraphicsShader* pShader = new FileSystemGraphicsShader(m_pCompiler, "Grass", "", "Grass");
		pShader->PushAttributeType(VertexAttributeType::Vec3_Float);
		pShader->HashAttributeTypes();

		pShader->PushAttributeStride(sizeof(TRS::Vector3<float>));
		pShader->HashAttributeStrides();

		pShader->SetProperty(ShaderPropertyBit_EnableDepthTesting |
							 ShaderPropertyBit_EnableBlend |
							 ShaderPropertyBit_EnablePushConstants |
							 ShaderPropertyBit_IsNonStandardShader);
		pShader->SetPushConstant(0, ShaderStageBit_Geometry, nullptr);
		pShader->SetPrimitiveMode(PrimitiveMode::Points);

		pShader->PushUniformDataLayout(UniformDataType::ImageSampler2D, ShaderStageBit_Fragment, 0);
		pShader->PushUniformDataLayout(UniformDataType::ImageSampler2D, ShaderStageBit_Geometry, 1);
		pShader->PushUniformDataLayout(UniformDataType::ImageSampler2D, ShaderStageBit_Geometry, 2);
		pShader->PushUniformDataLayout(UniformDataType::Buffer, 
									   ShaderStageBit_Geometry, 
									   3, 
									   static_cast<uint32_t>(sizeof(float)), 
									   static_cast<uint32_t>(m_uTimerOffset));
		pShader->HashUniformDataLayouts();

		pShader->PushTextureHash(m_hshGrassTexture);
		pShader->PushTextureHash(m_hshWindTexture);
		pShader->PushTextureHash(m_hshHeightTexture);
		pShader->HashTextures();
		return pShader;
	}
}