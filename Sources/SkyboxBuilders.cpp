// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: SkyboxBuilders.cpp - skybox resource builders' implementation
// author: Karl-Mihkel Ott

#define SKYBOX_BUILDERS_CPP
#include "deng/SkyboxBuilders.h"

namespace DENG {

	size_t SkyboxMeshBuilder::s_uVertexOffset = SIZE_MAX;
    const float SkyboxMeshBuilder::s_skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

	MeshCommands SkyboxMeshBuilder::Get() {
	    // check if buffer allocation is required
        if (s_uVertexOffset == SIZE_MAX) {
            s_uVertexOffset = m_pRenderer->AllocateMemory(sizeof(s_skyboxVertices), BufferDataType::Vertex);
            m_pRenderer->UpdateBuffer(s_skyboxVertices, sizeof(s_skyboxVertices), s_uVertexOffset);
        }

        MeshCommands meshCommands;
        meshCommands.drawCommands.emplace_back();
        meshCommands.drawCommands.back().uDrawCount = 36;
        meshCommands.drawCommands.back().attributeOffsets.push_back(s_uVertexOffset);
        
        return meshCommands;
    }


    IShader* SkyboxShaderBuilder::Get() {
        FileSystemShader* pShader = new FileSystemShader("Skybox", "", "Skybox");
        pShader->SetProperty(ShaderPropertyBit_IsNonStandardShader |
                             ShaderPropertyBit_Enable3DTextures |
                             ShaderPropertyBit_EnableDepthTesting |
                             ShaderPropertyBit_EnableBlend |
                             ShaderPropertyBit_EnablePushConstants);

        pShader->PushAttributeType(VertexAttributeType::Vec3_Float);
        pShader->PushAttributeStride(sizeof(TRS::Vector3<float>));
        pShader->PushTextureHash(m_hshTexture);
        pShader->PushUniformDataLayout(UniformDataType::Buffer, ShaderStageBit_Vertex, 0);
        pShader->PushUniformDataLayout(UniformDataType::ImageSampler3D, ShaderStageBit_Fragment, 1);
        pShader->SetPushConstant(0, ShaderStageBit_Vertex, nullptr);
    
        return pShader;
    }
}