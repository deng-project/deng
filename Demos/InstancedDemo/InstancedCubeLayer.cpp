// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: InstancedCubeLayer.cpp - Instance cube layer class implementation
// author: Karl-Mihkel Ott

#define INSTANCED_CUBE_LAYER_CPP
#include "InstancedCubeLayer.h"

namespace Application {
	
	InstancedCubeLayer::InstancedCubeLayer(DENG::EventManager& _eventManager, IRenderer* _pRenderer, IFramebuffer* _pFramebuffer) :
		ILayer(_eventManager),
		m_scene(_pRenderer, _pFramebuffer),
		m_pRenderer(_pRenderer) {}


	uint32_t InstancedCubeLayer::_LoadTexture(const std::string& _sPath, IRenderer* _pRenderer) {
		ProgramFilesManager programFilesManager;
		auto imageData = programFilesManager.GetProgramFileContent(_sPath);
		
		int x, y, depth;
		stbi_uc* pTexels = stbi_load_from_memory((stbi_uc*)imageData.data(), static_cast<int>(imageData.size()), &x, &y, &depth, 4);
	
		TextureResource textureResource;
		textureResource.bHeapAllocationFlag = true;
		textureResource.pRGBAData = reinterpret_cast<char*>(pTexels);
		textureResource.eLoadType = TEXTURE_RESOURCE_LOAD_TYPE_EXTERNAL_PNG;
		textureResource.eResourceType = TEXTURE_RESOURCE_2D_IMAGE;
		textureResource.uWidth = static_cast<uint32_t>(x);
		textureResource.uHeight = static_cast<uint32_t>(y);
		textureResource.uBitDepth = 4;

		return _pRenderer->AddTextureResource(textureResource);
	}


	void InstancedCubeLayer::_CreatePrefab(IRenderer* _pRenderer) {
		size_t uOffset = _pRenderer->AllocateMemory(sizeof(g_cCubeVertices), BufferDataType::VERTEX);
		_pRenderer->UpdateBuffer(g_cCubeVertices, sizeof(g_cCubeVertices), uOffset);

		auto& mesh = m_scene.GetComponent<MeshComponent>(m_prefabEntity);
		auto& materials = m_scene.GetComponent<PrefabMaterialComponents>(m_prefabEntity).materials;
		auto& shader = m_scene.GetComponent<ShaderComponent>(m_prefabEntity);

		// mesh component
		mesh.sName = "Cube mesh prefab";
		mesh.drawCommands.emplace_back();
		mesh.drawCommands.back().attributeOffsets.push_back(uOffset);
		mesh.drawCommands.back().attributeOffsets.push_back(uOffset + 3u * sizeof(float));
		mesh.drawCommands.back().attributeOffsets.push_back(uOffset + 6u * sizeof(float));
		mesh.drawCommands.back().uDrawCount = 36;

		// material component
		try {
			materials[0].vMaps[0] = _LoadTexture("Textures/Container/diffuse.png", _pRenderer);		// diffuse map
			materials[0].vMaps[1] = _LoadTexture("Textures/Container/specular.png", _pRenderer);	// specular map
		}
		catch (const RendererException& e) {
			DISPATCH_ERROR_MESSAGE("RendererException", e.what(), ErrorSeverity::CRITICAL);
		}

		materials[0].fShininess = 0.2f;

		for (int i = 1; i < SQ(ROW_LEN); i++) {
			materials[i] = materials[0];
		}

		// shader component
		shader.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT);
		shader.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT);
		shader.attributes.push_back(ATTRIBUTE_TYPE_VEC2_FLOAT);

		shader.attributeStrides.push_back(8u * sizeof(float));
		shader.attributeStrides.push_back(8u * sizeof(float));
		shader.attributeStrides.push_back(8u * sizeof(float));

		shader.bEnableDepthTesting = true;
		shader.bEnableBlend = true;
		shader.bEnableIndexing = false;
		shader.bEnablePushConstants = true;
		shader.iPushConstantShaderStage = SHADER_STAGE_VERTEX | SHADER_STAGE_FRAGMENT;
		shader.pShader = new Shader("ShadedCube", "ShadedCube");

		shader.uboDataLayouts.reserve(7);
		shader.uboDataLayouts.emplace_back();
		shader.uboDataLayouts.back().block.uBinding = 0;
		shader.uboDataLayouts.back().eType = UNIFORM_DATA_TYPE_STORAGE_BUFFER;
		shader.uboDataLayouts.back().eUsage = UNIFORM_USAGE_PER_MESH;
		shader.uboDataLayouts.back().iStage = SHADER_STAGE_VERTEX;

		shader.uboDataLayouts.emplace_back();
		shader.uboDataLayouts.back().block.uBinding = 1;
		shader.uboDataLayouts.back().eType = UNIFORM_DATA_TYPE_STORAGE_BUFFER;
		shader.uboDataLayouts.back().eUsage = UNIFORM_USAGE_PER_SHADER;
		shader.uboDataLayouts.back().iStage = SHADER_STAGE_FRAGMENT;

		shader.uboDataLayouts.emplace_back();
		shader.uboDataLayouts.back().block.uBinding = 2;
		shader.uboDataLayouts.back().eType = UNIFORM_DATA_TYPE_STORAGE_BUFFER;
		shader.uboDataLayouts.back().eUsage = UNIFORM_USAGE_PER_SHADER;
		shader.uboDataLayouts.back().iStage = SHADER_STAGE_FRAGMENT;

		shader.uboDataLayouts.emplace_back();
		shader.uboDataLayouts.back().block.uBinding = 3;
		shader.uboDataLayouts.back().eType = UNIFORM_DATA_TYPE_STORAGE_BUFFER;
		shader.uboDataLayouts.back().eUsage = UNIFORM_USAGE_PER_SHADER;
		shader.uboDataLayouts.back().iStage = SHADER_STAGE_FRAGMENT;

		shader.uboDataLayouts.emplace_back();
		shader.uboDataLayouts.back().block.uBinding = 4;
		shader.uboDataLayouts.back().eType = UNIFORM_DATA_TYPE_STORAGE_BUFFER;
		shader.uboDataLayouts.back().eUsage = UNIFORM_USAGE_PER_MESH;
		shader.uboDataLayouts.back().iStage = SHADER_STAGE_FRAGMENT;

		shader.uboDataLayouts.emplace_back();
		shader.uboDataLayouts.back().block.uBinding = 5;
		shader.uboDataLayouts.back().eType = UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER;
		shader.uboDataLayouts.back().eUsage = UNIFORM_USAGE_PER_MESH;
		shader.uboDataLayouts.back().iStage = SHADER_STAGE_FRAGMENT;

		shader.uboDataLayouts.emplace_back();
		shader.uboDataLayouts.back().block.uBinding = 6;
		shader.uboDataLayouts.back().eType = UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER;
		shader.uboDataLayouts.back().eUsage = UNIFORM_USAGE_PER_MESH;
		shader.uboDataLayouts.back().iStage = SHADER_STAGE_FRAGMENT;
	}


	void InstancedCubeLayer::_ApplyTransforms() {
		const float cfGap = 0.5f;
		const float cfCubeSize = 1.f;
		const float cfStartX = -(float)ROW_LEN * (2.f * cfGap + cfCubeSize) / 2.f;
		float fCurrentHeight = 0.5f;

		auto& transforms = m_scene.GetComponent<PrefabTransformComponents>(m_prefabEntity).transforms;

		// x
		for (int i = 0; i < ROW_LEN; i++) {
			// y
			for (int j = 0; j < ROW_LEN; j++) {
				transforms[i * ROW_LEN + j].vTranslation[0] = cfStartX + static_cast<float>(j) * (cfGap + cfCubeSize);
				transforms[i * ROW_LEN + j].vTranslation[1] = fCurrentHeight;
			}

			fCurrentHeight += cfGap + cfCubeSize;
		}
	}


	void InstancedCubeLayer::Attach(IRenderer* _pRenderer, IWindowContext*) {
		// register for window resize events
		m_eventManager.AddListener<InstancedCubeLayer, WindowResizedEvent>(&InstancedCubeLayer::OnWindowResizedEvent, this);
		
		m_scene.SetAmbient({ 0.5f, 0.5f, 0.5f });
		Entity idCamera = m_scene.CreateEntity();
		m_scene.EmplaceComponent<CameraComponent>(idCamera);
		m_scene.EmplaceComponent<ScriptComponent>(idCamera).BindScript<CameraScript>(idCamera, m_eventManager, m_scene);
		m_scene.SetMainCamera(idCamera);

		m_prefabEntity = m_scene.CreateEntity();
		m_scene.EmplaceComponent<MeshComponent>(m_prefabEntity);
		m_scene.EmplaceComponent<ShaderComponent>(m_prefabEntity);
		m_scene.EmplaceComponent<PrefabTransformComponents>(m_prefabEntity).transforms.resize(SQ(ROW_LEN));
		m_scene.EmplaceComponent<PrefabMaterialComponents>(m_prefabEntity).materials.resize(SQ(ROW_LEN));

		_CreatePrefab(_pRenderer);
		_ApplyTransforms();
		m_scene.AttachComponents();
	}

	void InstancedCubeLayer::Update(DENG::IFramebuffer*) {
		m_scene.RenderScene();
	}


	bool InstancedCubeLayer::OnWindowResizedEvent(WindowResizedEvent& _event) {
		m_pRenderer->UpdateViewport(_event.GetWidth(), _event.GetHeight());
		return true;
	}


	void InstancedCubeLayer::OnImGuiDraw() {
		ImGui::Begin("Diagnostics");
		ImGui::Text("Current framerate: %.3f", ImGui::GetIO().Framerate);
		ImGui::Text("Currently drawing %d cubes", SQ(ROW_LEN));
		ImGui::End();
	}
}