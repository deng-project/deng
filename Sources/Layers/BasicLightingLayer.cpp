// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: BasicLightingLayer.cpp - BasicLightingLayer class implementation
// author: Karl-Mihkel Ott

#define BASIC_LIGHTING_LAYER_CPP
#include "deng/Layers/BasicLightingLayer.h"

namespace DENG {

	dDECLARE_RESOURCE_ID_TABLE(MaterialTable)
		dRESOURCE_ID_ENTRY("default"),
		dRESOURCE_ID_ENTRY("emerald"),
		dRESOURCE_ID_ENTRY("jade"),
		dRESOURCE_ID_ENTRY("obsidian"),
		dRESOURCE_ID_ENTRY("pearl"),
		dRESOURCE_ID_ENTRY("ruby"),
		dRESOURCE_ID_ENTRY("turquoise"),
		dRESOURCE_ID_ENTRY("brass"),
		dRESOURCE_ID_ENTRY("bronze"),
		dRESOURCE_ID_ENTRY("chrome"),
		dRESOURCE_ID_ENTRY("copper"),
		dRESOURCE_ID_ENTRY("gold"),
		dRESOURCE_ID_ENTRY("silver"),
		dRESOURCE_ID_ENTRY("black_plastic"),
		dRESOURCE_ID_ENTRY("cyan_plastic"),
		dRESOURCE_ID_ENTRY("green_plastic"),
		dRESOURCE_ID_ENTRY("red_plastic"),
		dRESOURCE_ID_ENTRY("white_plastic"),
		dRESOURCE_ID_ENTRY("yellow_plastic"),
		dRESOURCE_ID_ENTRY("black_rubber"),
		dRESOURCE_ID_ENTRY("cyan_rubber"),
		dRESOURCE_ID_ENTRY("green_rubber"),
		dRESOURCE_ID_ENTRY("red_rubber"),
		dRESOURCE_ID_ENTRY("white_rubber"),
		dRESOURCE_ID_ENTRY("yellow_rubber")
	dEND_RESOURCE_ID_TABLE(MaterialTable)

	dDECLARE_RESOURCE_ID_TABLE(CubeResourceTable)
		dRESOURCE_ID_ENTRY("ShadedCubeMesh"),
		dRESOURCE_ID_ENTRY("ShadedCubeShader"),
		dRESOURCE_ID_ENTRY("ShadedCubeDiffuseMap"),
		dRESOURCE_ID_ENTRY("ShadedCubeSpecularMap"),
		dRESOURCE_ID_ENTRY("WhiteCubeMesh"),
		dRESOURCE_ID_ENTRY("WhiteCubeShader")
	dEND_RESOURCE_ID_TABLE(CubeResourceTable)

	BasicLightingLayer::BasicLightingLayer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer) :
		m_scene(_pRenderer, _pFramebuffer),
		m_hshLookup(MaterialTable{}.MakeReverseUnorderedMap<hash_t, const char*>())
	{
		m_pRenderer = _pRenderer;
		EventManager& eventManager = EventManager::GetInstance();
		eventManager.AddListener<BasicLightingLayer, WindowResizedEvent>(&BasicLightingLayer::OnWindowResizedEvent, this);
	}

	void BasicLightingLayer::_SetupMaterials() {
		ResourceManager& resourceManager = ResourceManager::GetInstance();
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("default", MaterialTable));
		resourceManager.GetMaterial(dRO_SID("default", MaterialTable))->hshDiffuseMap = dRO_SID("ShadedCubeDiffuseMap", CubeResourceTable);
		resourceManager.GetMaterial(dRO_SID("default", MaterialTable))->hshSpecularMap = dRO_SID("ShadedCubeSpecularMap", CubeResourceTable);

		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("emerald", MaterialTable),
			Material{ { 0.0215f, 0.1745f, 0.0215f, 0.f }, { 0.07568f, 0.61424f, 0.07568f, 0.f }, { 0.633f, 0.727811f, 0.633f, 0.f }, 0, 0, 0.6f });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("jade", MaterialTable),
			Material{ { 0.135f, 0.2225f, 0.1575f, 0.f }, { 0.54f, 0.89f, 0.63f, 0.f }, { 0.316228f, 0.316228f, 0.316228f, 0.f }, 0, 0, 0.1f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("obsidian", MaterialTable),
			Material{ { 0.05375f, 0.05f, 0.06625f, 0.f }, { 0.18275f, 0.17f, 0.22525f, 0.f }, { 0.332741f, 0.328634f, 0.346435f, 0.f }, 0, 0, 0.3f });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("pearl", MaterialTable),
			Material{ { 0.25f, 0.20725f, 0.20725f, 0.f }, { 1.0f, 0.829f, 0.829f, 0.f }, { 0.296648f, 0.296648f, 0.296648f, 0.f }, 0, 0, 0.088f });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("ruby", MaterialTable),
			Material{ { 0.1745f, 0.01175f, 0.01175f, 0.f }, { 0.61424f, 0.04136f, 0.04136f, 0.f }, { 0.727811f, 0.626959f, 0.626959f, 0.f }, 0, 0, 0.6f });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("turquoise", MaterialTable),
			Material{ { 0.1f, 0.18725f, 0.1745f, 0.f }, { 0.396f, 0.74151f, 0.69102f, 0.f }, { 0.297254f, 0.30829f, 0.306678f, 0.f }, 0, 0, 0.1f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("brass", MaterialTable),
			Material{ { 0.329412f, 0.223529f, 0.027451f, 0.f }, { 0.780392f, 0.568627f, 0.113725f, 0.f }, { 0.992157f, 0.941176f, 0.807843f, 0.f }, 0, 0, 0.21794872f });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("bronze", MaterialTable),
			Material{ { 0.2125f, 0.1275f, 0.054f, 0.f }, { 0.714f, 0.4284f, 0.18144f, 0.f }, { 0.393548f, 0.271906f, 0.166721f, 0.f }, 0, 0, 0.2f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("chrome", MaterialTable),
			Material{ { 0.25f, 0.25f, 0.25f, 0.f }, { 0.4f, 0.4f, 0.4f, 0.f }, { 0.774597f, 0.774597f, 0.774597f, 0.f }, 0, 0, 0.6f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("copper", MaterialTable),
			Material{ { 0.19125f, 0.0735f, 0.0225f, 0.f }, { 0.7038f, 0.27048f, 0.0828f, 0.f }, { 0.256777f, 0.137622f, 0.086014f, 0.f }, 0, 0, 0.1f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("gold", MaterialTable),
			Material{ { 0.24725f, 0.1995f, 0.0745f, 0.f }, { 0.75164f, 0.60648f, 0.22648f, 0.f }, { 0.628281f, 0.555802f, 0.366065f, 0.f }, 0, 0, 0.4f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("silver", MaterialTable),
			Material{ { 0.19225f, 0.19225f, 0.19225f, 0.f }, { 0.50754f, 0.50754f, 0.50754f, 0.f }, { 0.508273f, 0.508273f, 0.508273f, 0.f }, 0, 0, 0.4f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("black_plastic", MaterialTable),
			Material{ { 0.0f, 0.0f, 0.0f, 0.f }, { 0.01f, 0.01f, 0.01f, 0.f }, { 0.50f, 0.50f, 0.50f, 0.f }, 0, 0, 0.25f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("cyan_plastic", MaterialTable),
			Material{ { 0.0f, 0.1f, 0.06f, 0.f }, { 0.0f, 0.50980392f, 0.50980392f, 0.f }, { 0.50196078f, 0.50196078f, 0.50196078f, 0.f }, 0, 0, 0.25f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("green_plastic", MaterialTable),
			Material{ { 0.0f, 0.0f, 0.0f, 0.f }, { 0.1f, 0.35f, 0.1f, 0.f }, { 0.45f, 0.55f, 0.45f, 0.f }, 0, 0, 0.25f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("red_plastic", MaterialTable),
			Material{ { 0.0f, 0.0f, 0.0f, 0.f }, { 0.5f, 0.0f, 0.0f, 0.f }, { 0.7f, 0.6f, 0.6f, 0.f }, 0, 0, 0.25f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("white_plastic", MaterialTable),
			Material{ { 0.0f, 0.0f, 0.0f, 0.f }, { 0.55f, 0.55f, 0.55f, 0.f }, { 0.70f, 0.70f, 0.70f, 0.f }, 0, 0, 0.25f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("yellow_plastic", MaterialTable),
			Material{ { 0.0f, 0.0f, 0.0f, 0.f }, { 0.5f, 0.5f, 0.0f, 0.f }, { 0.60f, 0.60f, 0.50f, 0.f }, 0, 0, 0.25f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("black_rubber", MaterialTable),
			Material{ { 0.02f, 0.02f, 0.02f, 0.f }, { 0.01f, 0.01f, 0.01f, 0.f }, { 0.4f, 0.4f, 0.4f, 0.f }, 0, 0, 0.078125f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("cyan_rubber", MaterialTable),
			Material{ { 0.0f, 0.05f, 0.05f, 0.f }, { 0.4f, 0.5f, 0.5f, 0.f }, { 0.04f, 0.7f, 0.7f, 0.f }, 0, 0, 0.078125f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("green_rubber", MaterialTable),
			Material{ { 0.0f, 0.05f, 0.0f, 0.f }, { 0.4f, 0.5f, 0.4f, 0.f }, { 0.04f, 0.7f, 0.04f, 0.f }, 0, 0, 0.078125f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("red_rubber", MaterialTable),
			Material{ { 0.05f, 0.0f, 0.0f, 0.f }, { 0.5f, 0.4f, 0.4f, 0.f }, { 0.7f, 0.04f, 0.04f, 0.f }, 0, 0, 0.078125f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("white_rubber", MaterialTable),
			Material{ { 0.05f, 0.05f, 0.05f, 0.f }, { 0.5f, 0.5f, 0.5f, 0.f }, { 0.7f, 0.7f, 0.7f, 0.f }, 0, 0, 0.078125f, });
		resourceManager.AddMaterial<InitializerListBuilder<Material>>(dRO_SID("yellow_rubber", MaterialTable),
			Material{ { 0.05f, 0.05f, 0.0f, 0.f }, { 0.5f, 0.5f, 0.4f, 0.f }, { 0.7f, 0.7f, 0.04f, 0.f }, 0, 0, 0.078125f, });
	}

	void BasicLightingLayer::_InitialiseTransformsAndLights() {
		auto& pointLight = m_scene.GetComponent<PointLightComponent>(m_idPointLight);
		pointLight.vK = { 1.f, 0.22f, 0.2f };
		pointLight.vSpecular = { 1.f, 1.f, 1.f, 0.f };
		pointLight.vDiffuse = { 0.5f, 0.5f, 0.5f, 0.f };

		auto& dirLight = m_scene.GetComponent<DirectionalLightComponent>(m_idDirectionalLight);
		dirLight.vDiffuse = { 0.33f, 0.22f, 0.22f, 0.f };
		dirLight.vSpecular = { 0.66f, 0.44f, 0.44f, 0.f };
		dirLight.vDirection = { 0.f, -1.f, 0.f };

		auto& spotLight = m_scene.GetComponent<SpotlightComponent>(m_idSpotLight);
		spotLight.vDiffuse = { 0.5f, 0.f, 0.f, 0.f };
		spotLight.vSpecular = { 1.f, 0.f, 0.f, 0.f };

		auto& pointLightTransform = m_scene.GetComponent<TransformComponent>(m_idPointLight);
		pointLightTransform.vScale = { 0.1f, 0.1f, 0.1f };
		pointLightTransform.vTranslation = { 1.f, 1.f, 0.f, 0.f };

		auto& spotLightTransform = m_scene.GetComponent<TransformComponent>(m_idSpotLight);
		spotLightTransform.vScale = { 0.1f, 0.1f, 0.1f };
		spotLightTransform.vTranslation = { 1.f, 0.f, 1.f, 0.f };
	}

	void BasicLightingLayer::_ImGuiDrawLightProperties() {
		auto& [pointLight, pointLightTransform] = m_scene.GetRegistry().get<PointLightComponent, TransformComponent>(m_idPointLight);
		auto& dirLight = m_scene.GetRegistry().get<DirectionalLightComponent>(m_idDirectionalLight);
		auto& [spotLight, spotLightTransform] = m_scene.GetRegistry().get<SpotlightComponent, TransformComponent>(m_idSpotLight);

		ImGui::Text("Point light position: { %.3f, %.3f, %.3f }", pointLight.vPosition[0], pointLight.vPosition[1], pointLight.vPosition[2]);
		ImGui::Text("Directional light direction { %.3f, %.3f, %.3f }", dirLight.vDirection[0], dirLight.vDirection[1], dirLight.vDirection[2]);
		ImGui::Text("Spot light position { %.3f, %.3f, %.3f }", spotLight.vPosition[0], spotLight.vPosition[1], spotLight.vPosition[2]);
		ImGui::Text("Spot light direction { %.3f, %.3f, %.3f }", spotLight.vDirection[0], spotLight.vDirection[1], spotLight.vDirection[2]);
		ImGui::Separator();

		if (ImGui::TreeNode("Click here to modify point light properties")) {
			ImGui::SliderFloat("Point light translation X", &pointLightTransform.vTranslation.first, -BOUNDS, BOUNDS);
			ImGui::SliderFloat("Point light translation Y", &pointLightTransform.vTranslation.second, -BOUNDS, BOUNDS);
			ImGui::SliderFloat("Point light translation Z", &pointLightTransform.vTranslation.third, -BOUNDS, BOUNDS);
			ImGui::Separator();
			ImGui::SliderAngle("Point light rotation X", &pointLightTransform.vRotation.first);
			ImGui::SliderAngle("Point light rotation Y", &pointLightTransform.vRotation.second);
			ImGui::SliderAngle("Point light rotation Z", &pointLightTransform.vRotation.third);
			ImGui::Separator();
			ImGui::SliderFloat3("Diffuse color", (float*)&pointLight.vDiffuse, 0.f, 1.f);
			ImGui::SliderFloat3("Specular color", (float*)&pointLight.vSpecular, 0.f, 1.f);
			ImGui::Separator();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Click here to modify directional light properties")) {
			static float fDirRotation = 0.f;
			ImGui::SliderAngle("Direction light rotation ", &fDirRotation, -90.f, 90.f);
			TRS::Quaternion qX = { std::sinf(fDirRotation / 2.f), 0.f, 0.f, std::cosf(fDirRotation / 2.f) };
			dirLight.vDirection = qX * TRS::Vector4<float>{ 0.f, -1.f, 0.f, 1.f };
			ImGui::Separator();
			ImGui::SliderFloat3("Diffuse color", (float*)&dirLight.vDiffuse, 0.f, 1.f);
			ImGui::SliderFloat3("Specular color", (float*)&dirLight.vSpecular, 0.f, 1.f);
			ImGui::Separator();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Click here to modify spot light properties")) {
			ImGui::SliderFloat("Spot light translation X", &spotLightTransform.vTranslation[0], -BOUNDS, BOUNDS);
			ImGui::SliderFloat("Spot light translation Y", &spotLightTransform.vTranslation[1], -BOUNDS, BOUNDS);
			ImGui::SliderFloat("Spot light translation Z", &spotLightTransform.vTranslation[2], -BOUNDS, BOUNDS);
			ImGui::Separator();
			ImGui::SliderAngle("Spot light rotation X", &spotLightTransform.vRotation[0]);
			ImGui::SliderAngle("Spot light rotation Y", &spotLightTransform.vRotation[1]);
			ImGui::SliderAngle("Spot light rotation Z", &spotLightTransform.vRotation[2]);
			ImGui::Separator();
			ImGui::SliderFloat3("Spot light direction vector", (float*)&spotLight.vDirection, -1.f, 1.f);
			ImGui::SliderFloat("Spot light inner cutoff", &spotLight.fInnerCutoff, 0.f, 1.f);
			ImGui::SliderFloat("Spot light outer cutoff", &spotLight.fOuterCutoff, 0.f, spotLight.fInnerCutoff);
			ImGui::Separator();
			ImGui::SliderFloat3("Diffuse color", (float*)&spotLight.vDiffuse, 0.f, 1.f);
			ImGui::SliderFloat3("Specular color", (float*)&spotLight.vSpecular, 0.f, 1.f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Click to choose ambient color")) {
			ImGui::ColorPicker3("Ambient color", (float*)&m_scene.GetAmbient(), ImGuiColorEditFlags_NoPicker);
			ImGui::TreePop();
		}
	}


	bool BasicLightingLayer::OnWindowResizedEvent(WindowResizedEvent& _event) {
		m_pRenderer->UpdateViewport(_event.GetWidth(), _event.GetHeight());
		return false;
	}


	void BasicLightingLayer::Attach(IRenderer*, IWindowContext* _pWindowContext) {
		m_pWindowContext = _pWindowContext;

		size_t uVertexOffset = m_pRenderer->AllocateMemory(sizeof(g_cCubeVertices), BufferDataType::Vertex);
		m_pRenderer->UpdateBuffer(g_cCubeVertices, sizeof(g_cCubeVertices), uVertexOffset);

		ResourceManager& resourceManager = ResourceManager::GetInstance();
		resourceManager.AddMesh<CubeMeshBuilder>(dRO_SID("ShadedCubeMesh", CubeResourceTable), uVertexOffset);
		resourceManager.AddMesh<LightSourceMeshBuilder>(dRO_SID("WhiteCubeMesh", CubeResourceTable), uVertexOffset);
		resourceManager.AddShader<CubeShaderBuilder>(dRO_SID("ShadedCubeShader", CubeResourceTable));
		resourceManager.AddShader<LightSourceShaderBuilder>(dRO_SID("WhiteCubeShader", CubeResourceTable));
		
		try {
			resourceManager.AddTexture<FileTextureBuilder>(dRO_SID("ShadedCubeDiffuseMap", CubeResourceTable), "Textures/Container/diffuse.png");
			resourceManager.AddTexture<FileTextureBuilder>(dRO_SID("ShadedCubeSpecularMap", CubeResourceTable), "Textures/Container/specular.png");
		}
		catch (const IOException& e) {
			DISPATCH_ERROR_MESSAGE("IOException", e.what(), ErrorSeverity::CRITICAL);
		}

		m_idPointLight = m_scene.CreateEntity();
		m_scene.EmplaceComponent<MeshComponent>(m_idPointLight, dRO_SID("WhiteCubeMesh", CubeResourceTable));
		m_scene.EmplaceComponent<ShaderComponent>(m_idPointLight, dRO_SID("WhiteCubeShader", CubeResourceTable));
		m_scene.EmplaceComponent<TransformComponent>(m_idPointLight);
		m_scene.EmplaceComponent<PointLightComponent>(m_idPointLight);

		m_idDirectionalLight = m_scene.CreateEntity();
		m_scene.EmplaceComponent<DirectionalLightComponent>(m_idDirectionalLight);

		m_idSpotLight = m_scene.CreateEntity();
		m_scene.EmplaceComponent<MeshComponent>(m_idSpotLight, dRO_SID("WhiteCubeMesh", CubeResourceTable));
		m_scene.EmplaceComponent<ShaderComponent>(m_idSpotLight, dRO_SID("WhiteCubeShader", CubeResourceTable));
		m_scene.EmplaceComponent<TransformComponent>(m_idSpotLight);
		m_scene.EmplaceComponent<SpotlightComponent>(m_idSpotLight);

		m_idShadedCube = m_scene.CreateEntity();
		m_scene.EmplaceComponent<MeshComponent>(m_idShadedCube, dRO_SID("ShadedCubeMesh", CubeResourceTable));
		m_scene.EmplaceComponent<ShaderComponent>(m_idShadedCube, dRO_SID("ShadedCubeShader", CubeResourceTable));
		m_scene.EmplaceComponent<TransformComponent>(m_idShadedCube);
		m_scene.EmplaceComponent<MaterialComponent>(m_idShadedCube);


		Entity idCamera = m_scene.CreateEntity();
		m_scene.EmplaceComponent<CameraComponent>(idCamera);
		m_scene.EmplaceComponent<ScriptComponent>(idCamera)
			.BindScript<CameraScript>(idCamera, m_scene);
		m_scene.SetMainCamera(idCamera);
		
		_InitialiseTransformsAndLights();
		m_scene.AttachComponents();
	}

	void BasicLightingLayer::Update(IFramebuffer* _pFramebuffer) {
		m_scene.RenderScene();
	}

	void BasicLightingLayer::OnImGuiDraw() {
		ImGui::Begin("Diagnostics/Material properties");

		ImGui::Text("Current framerate: %d fps", static_cast<int>(ImGui::GetIO().Framerate));
		_ImGuiDrawLightProperties();

		ImGui::Separator();
		ImGui::Text("Shaded cube material");
		const char* materials[] = {
			"default", "emerald", "jade", "obsidian", "pearl", "ruby", "turquoise", "brass", "bronze", "chrome",
			"copper", "gold", "silver", "black_plastic", "cyan_plastic", "green_plastic", "red_plastic", "white_plastic",
			"yellow_plastic", "black_rubber", "cyan_rubber", "green_rubber", "red_rubber", "white_rubber", "yellow_rubber"
		};

		static const char* s_szCurrentItem = materials[0];
		if (ImGui::BeginCombo("##combo", s_szCurrentItem)) {
			for (int i = 0; i < IM_ARRAYSIZE(materials); i++) {
				bool bIsSelected = (s_szCurrentItem == materials[i]);
				if (ImGui::Selectable(materials[i], bIsSelected))
					s_szCurrentItem = materials[i];
				if (bIsSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		auto& material = m_scene.GetComponent<MaterialComponent>(m_idShadedCube);
		auto selectedMaterial = m_hshLookup.find(s_szCurrentItem)->second;
		material.hshMaterial = selectedMaterial;

		ImGui::End();
	}
}