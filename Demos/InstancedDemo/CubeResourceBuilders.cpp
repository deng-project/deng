// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: CubeResourceBuilders.h - cube resource builder implementations
// author: Karl-Mihkel Ott

#define CUBE_RESOURCE_BUILDERS_CPP
#include "CubeResourceBuilders.h"

namespace Application {

	DENG::MeshCommands CubePrefabMeshBuilder::Get() {
		size_t uOffset = m_pRenderer->AllocateMemory(sizeof(g_cCubeVertices), DENG::BufferDataType::Vertex);
		m_pRenderer->UpdateBuffer(g_cCubeVertices, sizeof(g_cCubeVertices), uOffset);
		
		DENG::MeshCommands meshCommands;
		meshCommands.sName = "InstancedCubeMesh";
		meshCommands.drawCommands.emplace_back();
		meshCommands.drawCommands.back().attributeOffsets.push_back(uOffset);
		meshCommands.drawCommands.back().attributeOffsets.push_back(uOffset + 3u * sizeof(float));
		meshCommands.drawCommands.back().attributeOffsets.push_back(uOffset + 6u * sizeof(float));
		meshCommands.drawCommands.back().uDrawCount = 36;

		return meshCommands;
	}


	DENG::IShader* CubePrefabShaderBuilder::Get() {
		DENG::FileSystemShader* pShader = new DENG::FileSystemShader("ShadedCube", "ShadedCube");
		pShader->PushAttributeType(DENG::VertexAttributeType::Vec3_Float);
		pShader->PushAttributeType(DENG::VertexAttributeType::Vec3_Float);
		pShader->PushAttributeType(DENG::VertexAttributeType::Vec2_Float);

		pShader->PushAttributeStride(8u * sizeof(float));
		pShader->PushAttributeStride(8u * sizeof(float));
		pShader->PushAttributeStride(8u * sizeof(float));

		pShader->SetProperty(DENG::ShaderPropertyBit_EnableDepthTesting |
							 DENG::ShaderPropertyBit_EnableBlend |
							 DENG::ShaderPropertyBit_EnablePushConstants);
		pShader->SetPushConstant(0, DENG::ShaderStageBit_Vertex | DENG::ShaderStageBit_Fragment, nullptr);
		
		// [DrawDescriptorIndices]
		pShader->PushUniformDataLayout(DENG::UniformDataType::StorageBuffer, DENG::ShaderStageBit_Vertex | DENG::ShaderStageBit_Fragment, 0);
		// [TransformComponent]
		pShader->PushUniformDataLayout(DENG::UniformDataType::StorageBuffer, DENG::ShaderStageBit_Vertex, 1);
		// [PointLightComponent]
		pShader->PushUniformDataLayout(DENG::UniformDataType::StorageBuffer, DENG::ShaderStageBit_Fragment, 2);
		// [DirectionalLightComponent]
		pShader->PushUniformDataLayout(DENG::UniformDataType::StorageBuffer, DENG::ShaderStageBit_Fragment, 3);
		// [SpotlightComponent]
		pShader->PushUniformDataLayout(DENG::UniformDataType::StorageBuffer, DENG::ShaderStageBit_Fragment, 4);
		// [Material]
		pShader->PushUniformDataLayout(DENG::UniformDataType::StorageBuffer, DENG::ShaderStageBit_Fragment, 5);
		
		return pShader;
	}


	DENG::Material CubePrefabMaterialBuilder::Get() {
		DENG::Material material;
		material.hshDiffuseMap = m_hshDiffuse;
		material.hshSpecularMap = m_hshSpecular;

		return material;
	}


	DENG::Texture CubePrefabTextureBuilder::Get() {
		DENG::Texture texture;
		DENG::ProgramFilesManager programFilesManager;

		auto imageData = programFilesManager.GetProgramFileContent(m_sFileName);

		int x, y, depth;
		stbi_uc* pTexels = stbi_load_from_memory(
			(stbi_uc*)imageData.data(),
			static_cast<int>(imageData.size()),
			&x, &y, &depth, 4);

		texture.bHeapAllocationFlag = true;
		texture.pRGBAData = reinterpret_cast<char*>(pTexels);
		texture.eLoadType = DENG::TextureLoadType::External_PNG;
		texture.eResourceType = DENG::TextureType::Image_2D;
		texture.uWidth = static_cast<uint32_t>(x);
		texture.uHeight = static_cast<uint32_t>(y);
		texture.uBitDepth = 4;

		return texture;
	}
}