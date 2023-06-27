// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: InstancedCubeLayer.cpp - Instance cube layer class implementation
// author: Karl-Mihkel Ott

#define INSTANCED_CUBE_LAYER_CPP
#include "deng/Layers/InstancedCubeLayer.h"


namespace DENG {

	dDECLARE_RESOURCE_ID_TABLE(ResourceTable)
		dRESOURCE_ID_ENTRY("CubeMesh"),
		dRESOURCE_ID_ENTRY("CubeShader"),
		dRESOURCE_ID_ENTRY("CubeDiffuseMap"),
		dRESOURCE_ID_ENTRY("CubeSpecularMap"),
		dRESOURCE_ID_ENTRY("CubeMaterial")
	dEND_RESOURCE_ID_TABLE(ResourceTable)

	InstancedCubeLayer::InstancedCubeLayer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer) :
		m_scene(_pRenderer, _pFramebuffer),
		m_pRenderer(_pRenderer) {}


	void InstancedCubeLayer::_ApplyTransforms() {
		const float cfGap = 0.5f;
		const float cfCubeSize = 1.f;
		const float cfStartX = -(float)ROW_LEN * (2.f * cfGap + cfCubeSize) / 2.f;
		float fCurrentHeight = 0.5f;

		// x
		for (int i = 0; i < ROW_LEN; i++) {
			// y
			for (int j = 0; j < ROW_LEN; j++) {
				auto& transform = m_scene.GetComponent<TransformComponent>(m_entities[i * ROW_LEN + j]);
				transform.vTranslation[0] = cfStartX + static_cast<float>(j) * (cfGap + cfCubeSize);
				transform.vTranslation[1] = fCurrentHeight;
			}

			fCurrentHeight += cfGap + cfCubeSize;
		}
	}


	void InstancedCubeLayer::Attach(IRenderer* _pRenderer, IWindowContext*) {
		// register for window resize events
		EventManager& eventManager = EventManager::GetInstance();
		eventManager.AddListener<InstancedCubeLayer, WindowResizedEvent>(&InstancedCubeLayer::OnWindowResizedEvent, this);
		
		m_scene.SetAmbient({ 0.5f, 0.5f, 0.5f });
		Entity idCamera = m_scene.CreateEntity();
		m_scene.EmplaceComponent<CameraComponent>(idCamera);
		m_scene.EmplaceComponent<ScriptComponent>(idCamera).BindScript<CameraScript>(idCamera, m_scene);
		m_scene.SetMainCamera(idCamera);

		std::size_t uVertexOffset = m_pRenderer->AllocateMemory(sizeof(g_cCubeVertices), BufferDataType::Vertex);
		m_pRenderer->UpdateBuffer(g_cCubeVertices, sizeof(g_cCubeVertices), uVertexOffset);

		ResourceManager& resourceManager = ResourceManager::GetInstance();
		resourceManager.AddMesh<CubeMeshBuilder>(dRO_SID("CubeMesh", ResourceTable), uVertexOffset);
		resourceManager.AddShader<CubeShaderBuilder>(dRO_SID("CubeShader", ResourceTable));
		resourceManager.AddTexture<CubeTextureBuilder>(dRO_SID("CubeDiffuseMap", ResourceTable), "Textures/Container/diffuse.png");
		resourceManager.AddTexture<CubeTextureBuilder>(dRO_SID("CubeSpecularMap", ResourceTable), "Textures/Container/specular.png");
		resourceManager.AddMaterial<CubeMaterialBuilder>(
			dRO_SID("CubeMaterial", ResourceTable), 
			dRO_SID("CubeDiffuseMap", ResourceTable), 
			dRO_SID("CubeSpecularMap", ResourceTable));
		
		for (size_t i = 0; i < m_entities.size(); i++) {
			m_entities[i] = m_scene.CreateEntity();
			m_scene.EmplaceComponent<MeshComponent>(m_entities[i], dRO_SID("CubeMesh", ResourceTable));
			m_scene.EmplaceComponent<ShaderComponent>(m_entities[i], dRO_SID("CubeShader", ResourceTable));
			m_scene.EmplaceComponent<MaterialComponent>(m_entities[i], dRO_SID("CubeMaterial", ResourceTable));
			m_scene.EmplaceComponent<TransformComponent>(m_entities[i]);
		}

		_ApplyTransforms();

		eventManager.Dispatch<ComponentAddedEvent>(m_entities[0], ComponentType_Mesh | ComponentType_Shader | ComponentType_Material | ComponentType_Transform);
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