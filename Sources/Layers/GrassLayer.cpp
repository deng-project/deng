// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: GrassLayer.cpp - GrassLayer class implementation
// author: Karl-Mihkel Ott

#define GRASS_LAYER_CPP
#include "deng/Layers/GrassLayer.h"

namespace DENG {

	dDECLARE_RESOURCE_ID_TABLE(ResourceTable)
		dRESOURCE_ID_ENTRY("TerrainMesh"),
		dRESOURCE_ID_ENTRY("GrassShader"),
		dRESOURCE_ID_ENTRY("GrassTexture"),
		dRESOURCE_ID_ENTRY("WindTexture"),
		dRESOURCE_ID_ENTRY("TerrainShader"),
		dRESOURCE_ID_ENTRY("TerrainHeightTexture"),
		dRESOURCE_ID_ENTRY("TerrainTexture"),
		dRESOURCE_ID_ENTRY("SkyboxMesh"),
		dRESOURCE_ID_ENTRY("SkyboxShader"),
		dRESOURCE_ID_ENTRY("SkyboxTexture")
	dEND_RESOURCE_ID_TABLE(ResourceTable)
	
	GrassLayer::GrassLayer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer) :
		m_scene(_pRenderer, _pFramebuffer),
		m_pRenderer(_pRenderer) {}


	void GrassLayer::Attach(IRenderer*, IWindowContext*) {
		EventManager& eventManager = EventManager::GetInstance();
		eventManager.AddListener<GrassLayer, WindowResizedEvent>(&GrassLayer::OnWindowResizedEvent, this);
	
		m_uTimerOffset = m_pRenderer->AllocateMemory(sizeof(float), BufferDataType::Uniform);
		
		ResourceManager& resourceManager = ResourceManager::GetInstance();
		// skybox
		resourceManager.AddMesh<SkyboxMeshBuilder>(dRO_SID("SkyboxMesh", ResourceTable), m_pRenderer);
		resourceManager.AddShader<SkyboxShaderBuilder>(dRO_SID("SkyboxShader", ResourceTable), dRO_SID("SkyboxTexture", ResourceTable));
		

		// grass terrain resources
		resourceManager.AddMesh<GrassMeshBuilder>(dRO_SID("TerrainMesh", ResourceTable), m_pRenderer);
		resourceManager.AddShader<TerrainShaderBuilder>(dRO_SID("TerrainShader", ResourceTable),
			dRO_SID("TerrainHeightTexture", ResourceTable),
			dRO_SID("TerrainTexture", ResourceTable));

		resourceManager.AddShader<GrassShaderBuilder>(dRO_SID("GrassShader", ResourceTable),
													  m_uTimerOffset,
													  dRO_SID("GrassTexture", ResourceTable),
													  dRO_SID("WindTexture", ResourceTable),
													  dRO_SID("TerrainHeightTexture", ResourceTable));

		// textures
		try {
			resourceManager.AddTexture<FileMonochromeTextureBuilder>(dRO_SID("TerrainHeightTexture", ResourceTable), "Textures/Terrain/Height.png");
			resourceManager.AddTexture<FileTextureBuilder>(dRO_SID("TerrainTexture", ResourceTable), "Textures/Terrain/Foliage.png");
			resourceManager.AddTexture<FileTextureBuilder>(dRO_SID("GrassTexture", ResourceTable), "Textures/Grass/grass_texture.png");
			resourceManager.AddTexture<FileTextureBuilder>(dRO_SID("WindTexture", ResourceTable), "Textures/Grass/wind.png");
			resourceManager.AddTexture<FileCubeTextureBuilder>(dRO_SID("SkyboxTexture", ResourceTable),
				"Textures/Skybox/right.jpg",
				"Textures/Skybox/left.jpg",
				"Textures/Skybox/top.jpg",
				"Textures/Skybox/bottom.jpg",
				"Textures/Skybox/front.jpg",
				"Textures/Skybox/back.jpg");
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), ErrorSeverity::CRITICAL);
		}
		catch (const LogicException& e) {
			DISPATCH_ERROR_MESSAGE("LogicException", e.what(), ErrorSeverity::CRITICAL);
		}

		m_idSkybox = m_scene.CreateEntity();
		auto& skybox = m_scene.EmplaceComponent<SkyboxComponent>(m_idSkybox);
		skybox.vScale = { 1000.f, 1000.f, 1000.f, 1.f };
		skybox.hshMesh = dRO_SID("SkyboxMesh", ResourceTable);
		skybox.hshShader = dRO_SID("SkyboxShader", ResourceTable);
		m_scene.SetSkybox(m_idSkybox);

		m_idCamera = m_scene.CreateEntity();
		m_scene.EmplaceComponent<CameraComponent>(m_idCamera);
		m_scene.EmplaceComponent<ScriptComponent>(m_idCamera).BindScript<CameraScript>(m_idCamera, m_scene);
		m_scene.SetMainCamera(m_idCamera);

		m_idTerrain = m_scene.CreateEntity();
		m_scene.EmplaceComponent<MeshComponent>(m_idTerrain, dRO_SID("TerrainMesh", ResourceTable));
		m_scene.EmplaceComponent<ShaderComponent>(m_idTerrain, dRO_SID("TerrainShader", ResourceTable));
		m_scene.EmplaceComponent<MaterialComponent>(m_idTerrain, 0);

		m_idGrass = m_scene.CreateEntity();
		m_scene.EmplaceComponent<MeshComponent>(m_idGrass, dRO_SID("TerrainMesh", ResourceTable));
		m_scene.EmplaceComponent<ShaderComponent>(m_idGrass, dRO_SID("GrassShader", ResourceTable));
		m_scene.EmplaceComponent<MaterialComponent>(m_idGrass, 0);

		eventManager.Dispatch<ComponentAddedEvent>(m_idGrass, ComponentType_Mesh | ComponentType_Shader | ComponentType_Material);
		m_scene.AttachComponents();
	}


	void GrassLayer::Update(DENG::IFramebuffer*) {
		m_endTimestamp = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float, std::milli> timestep = m_endTimestamp - m_begTimestamp;
		const float fTimestep = timestep.count() / 1000.f;

		m_pRenderer->UpdateBuffer(&fTimestep, sizeof(float), m_uTimerOffset);
		m_scene.RenderScene();
	}


	bool GrassLayer::OnWindowResizedEvent(WindowResizedEvent& _event) {
		m_pRenderer->UpdateViewport(_event.GetWidth(), _event.GetHeight());
		return true;
	}
}