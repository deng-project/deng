// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: PBRLayer.h - PBRLayer class header
// author: Karl-Mihkel Ott

#define PBR_LAYER_CPP
#include "deng/Layers/PBRLayer.h"

namespace DENG {

	PBRLayer::PBRLayer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer) :
		m_pRenderer(_pRenderer),
		m_scene(_pRenderer, _pFramebuffer) {}


	void PBRLayer::Attach(IRenderer*, IWindowContext*, IGraphicsShaderCompiler* _pCompiler) {
		m_pGraphicsShaderCompiler = _pCompiler;

		EventManager& eventManager = EventManager::GetInstance();
		eventManager.AddListener<PBRLayer, WindowResizedEvent>(&PBRLayer::OnWindowResizeEvent, this);
	
		// camera
		m_idCamera = m_scene.CreateEntity();
		m_scene.EmplaceComponent<CameraComponent>(m_idCamera);
		m_scene.EmplaceComponent<ScriptComponent>(m_idCamera)
			.BindScript<CameraScript>(m_idCamera, m_scene)
			.InitializePosition({ 0.0f, 0.f, -1.5f });
		m_scene.SetMainCamera(m_idCamera);

		// directional light
		m_idDirLight = m_scene.CreateEntity();
		m_scene.EmplaceComponent<PointLightComponent>(m_idDirLight);
		auto& ptLight = m_scene.GetComponent<PointLightComponent>(m_idDirLight);
		ptLight.vPosition = { 1.5f, 1.5f, 0.0f, 0.0f };
		ptLight.vColor = { 1.f, 1.f, 1.f, 1.f };

		ResourceManager& resourceManager = ResourceManager::GetInstance();
		resourceManager.AddMesh<PBRSphereBuilder>(dRO_SID("SphereMesh", PBRTable), m_pRenderer);
		resourceManager.AddGraphicsShader<PBRShaderBuilder>(dRO_SID("PBRShader", PBRTable), m_pGraphicsShaderCompiler);
		resourceManager.AddTexture<FileTextureBuilder>(dRO_SID("RustAlbedo", PBRTable), "Textures/RustPBR/rustediron2_basecolor.png");
		resourceManager.AddTexture<FileMonochromeTextureBuilder>(dRO_SID("RustMetallic", PBRTable), "Textures/RustPBR/rustediron2_metallic.png");
		//resourceManager.AddTexture<FileTextureBuilder>(dRO_SID("RustNormal", PBRTable), "Textures/RustPBR/rustediron2_normal.png");
		resourceManager.AddTexture<FileMonochromeTextureBuilder>(dRO_SID("RustRoughness", PBRTable), "Textures/RustPBR/rustediron2_roughness.png");
		resourceManager.AddMaterialPBR<PBRMaterialBuilder>(dRO_SID("RedMaterial", PBRTable),
			std::array<cvar::hash_t, MaterialPBR::s_uTextureCount>{ 
				dRO_SID("RustAlbedo", PBRTable), 
				SID("__MissingTexture2D__"),
				SID("__MissingTexture2D__"),
				dRO_SID("RustMetallic", PBRTable),
				dRO_SID("RustRoughness", PBRTable),
				SID("__MissingTexture2D__")
			});

		m_sphere = m_scene.CreateEntity();
		m_scene.EmplaceComponent<MeshComponent>(m_sphere, dRO_SID("SphereMesh", PBRTable));
		m_scene.EmplaceComponent<ShaderComponent>(m_sphere, dRO_SID("PBRShader", PBRTable));
		m_scene.EmplaceComponent<MaterialComponent>(m_sphere, dRO_SID("RedMaterial", PBRTable));
		m_scene.EmplaceComponent<TransformComponent>(m_sphere);

		eventManager.Dispatch<ComponentAddedEvent>(m_sphere, ComponentType_Mesh | ComponentType_Shader | ComponentType_Material);
		m_scene.AttachComponents();
	}

	void PBRLayer::Update(IFramebuffer*) {
		m_scene.RenderScene();
	}

	bool PBRLayer::OnWindowResizeEvent(WindowResizedEvent& _event) {
		m_pRenderer->UpdateViewport(_event.GetWidth(), _event.GetHeight());
		return true;
	}

	void PBRLayer::OnImGuiDraw() {
		ImGui::Begin("Settings");
		ImGui::Text("Current framerate %.3f", ImGui::GetIO().Framerate);
		ImGui::Separator();
		
		EventManager& eventManager = EventManager::GetInstance();
		ResourceManager& resourceManager = ResourceManager::GetInstance();
		auto pMaterial = resourceManager.GetMaterialPBR(dRO_SID("RedMaterial", PBRTable));
		
		if (ImGui::SliderFloat("Roughness", &pMaterial->material.fRoughness, 0.0f, 1.f)) {
			eventManager.Dispatch<ResourceModifiedEvent>(dRO_SID("RedMaterial", PBRTable), ResourceType::Material_PBR);
		}

		if (ImGui::SliderFloat("Metallic", &pMaterial->material.fMetallic, 0.0f, 1.f)) {
			eventManager.Dispatch<ResourceModifiedEvent>(dRO_SID("RedMaterial", PBRTable), ResourceType::Material_PBR);
		}

		ImGui::End();
	}
}