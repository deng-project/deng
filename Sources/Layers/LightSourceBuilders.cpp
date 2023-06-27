// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: LightSourceBuilders.cpp - light resource builders' implementation
// author: Karl-Mihkel Ott

#define LIGHT_SOURCE_BUILDERS_CPP
#include "deng/Layers/LightSourceBuilders.h"

namespace DENG {
	
	MeshCommands LightSourceMeshBuilder::Get() {
		MeshCommands meshCommands;
		meshCommands.drawCommands.emplace_back();
		meshCommands.drawCommands.back().attributeOffsets.push_back(m_uVertexOffset);
		meshCommands.drawCommands.back().uDrawCount = 36;

		return meshCommands;
	}


	IShader* LightSourceShaderBuilder::Get() {
		FileSystemShader* pShader = new FileSystemShader("WhiteCube", "WhiteCube");
		pShader->PushAttributeType(VertexAttributeType::Vec3_Float);
		pShader->PushAttributeStride(8u * sizeof(float));
		
		pShader->SetProperty(ShaderPropertyBit_EnableBlend |
							 ShaderPropertyBit_EnableDepthTesting |
							 ShaderPropertyBit_EnablePushConstants);

		pShader->SetPushConstant(0, ShaderStageBit_Vertex, nullptr);

		pShader->PushUniformDataLayout(UniformDataType::StorageBuffer, ShaderStageBit_Vertex, 0);
		pShader->PushUniformDataLayout(UniformDataType::StorageBuffer, ShaderStageBit_Vertex, 1);
	
		return pShader;
	}
}