// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: CubeResourceBuilders.h - cube resource builder implementations
// author: Karl-Mihkel Ott

#define CUBE_RESOURCE_BUILDERS_CPP
#include "deng/Layers/CubeResourceBuilders.h"

namespace DENG {

	MeshCommands CubeMeshBuilder::Get() {
		MeshCommands meshCommands;
		meshCommands.sName = "InstancedCubeMesh";
		meshCommands.drawCommands.emplace_back();
		meshCommands.drawCommands.back().attributeOffsets.push_back(m_uVertexOffset);
		meshCommands.drawCommands.back().attributeOffsets.push_back(m_uVertexOffset + 3u * sizeof(float));
		meshCommands.drawCommands.back().attributeOffsets.push_back(m_uVertexOffset + 6u * sizeof(float));
		meshCommands.drawCommands.back().uDrawCount = 36;

		return meshCommands;
	}


	IShader* CubeShaderBuilder::Get() {
		FileSystemShader* pShader = new FileSystemShader("ShadedCube", "ShadedCube");
		pShader->PushAttributeType(VertexAttributeType::Vec3_Float);
		pShader->PushAttributeType(VertexAttributeType::Vec3_Float);
		pShader->PushAttributeType(VertexAttributeType::Vec2_Float);

		pShader->PushAttributeStride(8u * sizeof(float));
		pShader->PushAttributeStride(8u * sizeof(float));
		pShader->PushAttributeStride(8u * sizeof(float));

		pShader->SetProperty(ShaderPropertyBit_EnableDepthTesting |
							 ShaderPropertyBit_EnableBlend |
							 ShaderPropertyBit_EnablePushConstants);
		pShader->SetPushConstant(0, ShaderStageBit_Vertex | ShaderStageBit_Fragment, nullptr);
		
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


	Material CubeMaterialBuilder::Get() {
		Material material;
		material.hshDiffuseMap = m_hshDiffuse;
		material.hshSpecularMap = m_hshSpecular;

		return material;
	}


	Texture CubeTextureBuilder::Get() {
		Texture texture;
		ProgramFilesManager programFilesManager;

		auto imageData = programFilesManager.GetProgramFileContent(m_sFileName);

		int x, y, depth;
		stbi_uc* pTexels = stbi_load_from_memory(
			(stbi_uc*)imageData.data(),
			static_cast<int>(imageData.size()),
			&x, &y, &depth, 4);

		texture.bHeapAllocationFlag = true;
		texture.pRGBAData = reinterpret_cast<char*>(pTexels);
		texture.eLoadType = TextureLoadType::External_PNG;
		texture.eResourceType = TextureType::Image_2D;
		texture.uWidth = static_cast<uint32_t>(x);
		texture.uHeight = static_cast<uint32_t>(y);
		texture.uBitDepth = 4;

		return texture;
	}
}