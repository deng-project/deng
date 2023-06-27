// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: GrassLayer.cpp - GrassLayer class implementation
// author: Karl-Mihkel Ott

#define GRASS_LAYER_CPP
#include "GrassLayer.h"

namespace Application {

	dDECLARE_RESOURCE_ID_TABLE(ResourceTable)
		dRESOURCE_ID_ENTRY("GrassMesh"),
		dRESOURCE_ID_ENTRY("GrassShader"),
		dRESOURCE_ID_ENTRY("GrassTexture")
	dEND_RESOURCE_ID_TABLE(ResourceTable)
	
	GrassLayer::GrassLayer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer) :
		m_scene(_pRenderer, _pFramebuffer),
		m_pRenderer(_pRenderer) {}


	void GrassLayer::Attach(IRenderer*, IWindowContext*) {
		EventManager& eventManager = EventManager::GetInstance();
		eventManager.AddListener<GrassLayer, WindowResizedEvent>(&GrassLayer::OnWindowResizedEvent, this);
	
		ResourceManager& resourceManager = ResourceManager::GetInstance();
		resourceManager.AddMesh<GrassMeshBuilder>(dRO_SID("GrassMesh", ResourceTable), m_pRenderer);
		resourceManager.AddShader<GrassShaderBuilder>(dRO_SID("GrassShader", ResourceTable), dRO_SID("GrassTexture", ResourceTable));
		resourceManager.AddTexture<FileTextureBuilder>(dRO_SID("GrassTexture", ResourceTable), "Textures/Grass/grass_texture.png");
	
		m_idCamera = m_scene.CreateEntity();
		m_scene.EmplaceComponent<CameraComponent>(m_idCamera);
		m_scene.EmplaceComponent<ScriptComponent>(m_idCamera).BindScript<CameraScript>(m_idCamera, m_scene);
		m_scene.SetMainCamera(m_idCamera);

		m_idGrass = m_scene.CreateEntity();
		m_scene.EmplaceComponent<MeshComponent>(m_idGrass, dRO_SID("GrassMesh", ResourceTable));
		m_scene.EmplaceComponent<ShaderComponent>(m_idGrass, dRO_SID("GrassShader", ResourceTable));
		m_scene.EmplaceComponent<MaterialComponent>(m_idGrass, 0);

		eventManager.Dispatch<ComponentAddedEvent>(m_idGrass, ComponentType_Mesh | ComponentType_Shader | ComponentType_Material);
		m_scene.AttachComponents();
	}


	void GrassLayer::Update(DENG::IFramebuffer*) {
		m_scene.RenderScene();
	}


	bool GrassLayer::OnWindowResizedEvent(WindowResizedEvent& _event) {
		m_pRenderer->UpdateViewport(_event.GetWidth(), _event.GetHeight());
		return true;
	}
}