// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: ImGuiResourceBuilders.h - ImGui resource builder class implementations
// author: Karl-Mihkel Ott

#define IMGUI_RESOURCE_BUILDERS_CPP
#include "deng/ImGuiResourceBuilders.h"

namespace DENG {

	IShader* ImGuiShaderBuilder::Get() {
		FileSystemShader* pShader = new FileSystemShader("ImGui", "", "ImGui");
		pShader->PushAttributeType(VertexAttributeType::Vec2_Float);
		pShader->PushAttributeType(VertexAttributeType::Vec2_Float);
		pShader->PushAttributeType(VertexAttributeType::Vec4_UnsignedByte);
		pShader->HashAttributeTypes();

		pShader->PushAttributeStride(sizeof(ImDrawVert));
		pShader->PushAttributeStride(sizeof(ImDrawVert));
		pShader->PushAttributeStride(sizeof(ImDrawVert));
		pShader->HashAttributeStrides();

		pShader->SetProperty(ShaderPropertyBit_EnableBlend | 
							 ShaderPropertyBit_EnableScissor | 
							 ShaderPropertyBit_Enable2DTextures | 
							 ShaderPropertyBit_EnableIndexing);
		pShader->PushUniformDataLayout(
			UniformDataType::Buffer, 
			ShaderStageBit_Vertex,
			0,
			static_cast<uint32_t>(sizeof(TRS::Point2D<float>)),
			static_cast<uint32_t>(m_uUniformOffset));

		pShader->PushUniformDataLayout(
			UniformDataType::ImageSampler2D,
			ShaderStageBit_Fragment,
			1);
		pShader->HashUniformDataLayouts();

		pShader->PushTextureHash(m_hshTexture);
		pShader->HashTextures();
		return pShader;
	}


	Texture ImGuiTextureBuilder::Get() {
		Texture texture;
		texture.eResourceType = TextureType::Image_2D;
		texture.eLoadType = TextureLoadType::Embedded;
		texture.uBitDepth = 4;
		texture.uWidth = static_cast<uint32_t>(m_iWidth);
		texture.uHeight = static_cast<uint32_t>(m_iHeight);
		texture.bHeapAllocationFlag = false;
		texture.pRGBAData = reinterpret_cast<char*>(m_pTexels);
		
		return texture;
	}


	MeshCommands ImGuiMeshBuilder::Get() {
		MeshCommands commands;
		commands.sName = "__ImGui__";
		return commands;
	}
}