#define BASIC_LIGHTING_LAYER_CPP
#include "BasicLightingLayer.h"

void BasicLightingLayer::_CreateShaderComponent(DENG::ShaderComponent& _shader, const std::string& _sShaderName) {
	_shader.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC3_FLOAT);
	_shader.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC3_FLOAT);
	_shader.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC2_FLOAT);
	_shader.attributeStrides.push_back(8u * sizeof(float));
	_shader.attributeStrides.push_back(8u * sizeof(float));
	_shader.attributeStrides.push_back(8u * sizeof(float));

	// transform
	_shader.uboDataLayouts.emplace_back();
	_shader.uboDataLayouts.back().block.uBinding = 0;
	_shader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_BUFFER;
	_shader.uboDataLayouts.back().iStage = SHADER_STAGE_VERTEX;
	_shader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_MESH;
	_shader.eCullMode = DENG::CULL_MODE_NONE;

	// point lights
	_shader.uboDataLayouts.emplace_back();
	_shader.uboDataLayouts.back().block.uBinding = 1;
	_shader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_STORAGE_BUFFER;
	_shader.uboDataLayouts.back().iStage = SHADER_STAGE_FRAGMENT;
	_shader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_SHADER;

	// directional lights
	_shader.uboDataLayouts.emplace_back();
	_shader.uboDataLayouts.back().block.uBinding = 2;
	_shader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_STORAGE_BUFFER;
	_shader.uboDataLayouts.back().iStage = SHADER_STAGE_FRAGMENT;
	_shader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_SHADER;

	// spot lights
	_shader.uboDataLayouts.emplace_back();
	_shader.uboDataLayouts.back().block.uBinding = 3;
	_shader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_STORAGE_BUFFER;
	_shader.uboDataLayouts.back().iStage = SHADER_STAGE_FRAGMENT;
	_shader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_SHADER;

	// material uniform
	_shader.uboDataLayouts.emplace_back();
	_shader.uboDataLayouts.back().block.uBinding = 4;
	_shader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_BUFFER;
	_shader.uboDataLayouts.back().iStage = SHADER_STAGE_FRAGMENT;
	_shader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_MESH;

	// diffuse sampler
	_shader.uboDataLayouts.emplace_back();
	_shader.uboDataLayouts.back().block.uBinding = 5;
	_shader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER;
	_shader.uboDataLayouts.back().iStage = SHADER_STAGE_FRAGMENT;
	_shader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_MESH;

	// specular sampler
	_shader.uboDataLayouts.emplace_back();
	_shader.uboDataLayouts.back().block.uBinding = 6;
	_shader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER;
	_shader.uboDataLayouts.back().iStage = SHADER_STAGE_FRAGMENT;
	_shader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_MESH;

	_shader.pShader = new DENG::Shader(_sShaderName, _sShaderName);
	_shader.iPushConstantShaderStage = SHADER_STAGE_VERTEX | SHADER_STAGE_FRAGMENT;
	_shader.bEnablePushConstants = true;
	_shader.bEnableBlend = true;
	_shader.bEnableIndexing = false;
	_shader.bEnableDepthTesting = true;
}


uint32_t BasicLightingLayer::_LoadTexture(const std::string& _filename) {
	DENG::ProgramFilesManager m_programFilesManager;
	auto content = m_programFilesManager.GetProgramFileContent(_filename);
	if (!content.size()) {
		throw DENG::IOException("File '" + _filename + "' does not exist");
	}
	int x, y;
	int channels;
	unsigned char* data = stbi_load_from_memory((const unsigned char*)content.data(), static_cast<int>(content.size()), &x, &y, &channels, 4);
	DENG::TextureResource resource;
	resource.bHeapAllocationFlag = true;
	resource.eLoadType = DENG::TEXTURE_RESOURCE_LOAD_TYPE_EXTERNAL_PNG;
	resource.eResourceType = DENG::TEXTURE_RESOURCE_2D_IMAGE;
	resource.uWidth = static_cast<uint32_t>(x);
	resource.uHeight = static_cast<uint32_t>(y);
	resource.uBitDepth = 4;
	resource.pRGBAData = (char*)data;
	return m_pRenderer->AddTextureResource(resource);
}

void BasicLightingLayer::_CreateShaders() {
	auto& shadedMeshShader = m_scene.GetComponent<DENG::ShaderComponent>(m_idShadedCube);
	_CreateShaderComponent(shadedMeshShader, "ShadedCube");

	auto& pointLightShader = m_scene.GetComponent<DENG::ShaderComponent>(m_idPointLight);
	pointLightShader.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC3_FLOAT);
	pointLightShader.attributeStrides.push_back(8u * sizeof(float));
	pointLightShader.iPushConstantShaderStage = shadedMeshShader.iPushConstantShaderStage;

	pointLightShader.pShader = new DENG::Shader("WhiteCube", "WhiteCube");
	pointLightShader.uboDataLayouts.emplace_back();
	pointLightShader.uboDataLayouts.back().block.uBinding = 0;
	pointLightShader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_BUFFER;
	pointLightShader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_MESH;
	pointLightShader.uboDataLayouts.back().iStage = SHADER_STAGE_VERTEX;

	pointLightShader.bEnableBlend = true;
	pointLightShader.bEnableIndexing = false;
	pointLightShader.bEnableDepthTesting = true;
	pointLightShader.bEnablePushConstants = true;

	auto& spotLightShader = m_scene.GetComponent<DENG::ShaderComponent>(m_idSpotLight);
	spotLightShader = DENG::ShaderComponent(pointLightShader);
}

void BasicLightingLayer::_CreateMeshes() {
	const size_t uVertexOffset = m_pRenderer->AllocateMemory(sizeof(g_cCubeVertices), DENG::BufferDataType::VERTEX);
	m_pRenderer->UpdateBuffer(g_cCubeVertices, sizeof(g_cCubeVertices), uVertexOffset);

	auto& shadedMesh = m_scene.GetComponent<DENG::MeshComponent>(m_idShadedCube);
	shadedMesh.sName = "ShadedCube";
	shadedMesh.drawCommands.emplace_back();
	shadedMesh.drawCommands.back().attributeOffsets.push_back(uVertexOffset);
	shadedMesh.drawCommands.back().attributeOffsets.push_back(uVertexOffset + 3u * sizeof(float));
	shadedMesh.drawCommands.back().attributeOffsets.push_back(uVertexOffset + 6u * sizeof(float));
	shadedMesh.drawCommands.back().uDrawCount = 36;

	auto& pointLightMesh = m_scene.GetComponent<DENG::MeshComponent>(m_idPointLight);
	pointLightMesh.drawCommands.emplace_back();
	pointLightMesh.drawCommands.back().attributeOffsets.push_back(uVertexOffset);
	pointLightMesh.drawCommands.back().uDrawCount = 36;
	pointLightMesh.sName = "Point light mesh";

	auto& spotLightMesh = m_scene.GetComponent<DENG::MeshComponent>(m_idSpotLight);
	spotLightMesh.drawCommands.emplace_back();
	spotLightMesh.drawCommands.back().attributeOffsets.push_back(uVertexOffset);
	spotLightMesh.drawCommands.back().uDrawCount = 36;
	spotLightMesh.sName = "Spot light mesh";
}

void BasicLightingLayer::_InitialiseTransformsAndLights() {
	auto& pointLight = m_scene.GetComponent<DENG::PointLightComponent>(m_idPointLight);
	pointLight.vK = { 1.f, 0.22f, 0.2f };
	pointLight.vSpecular = { 1.f, 1.f, 1.f, 0.f };
	pointLight.vDiffuse = { 0.5f, 0.5f, 0.5f, 0.f };

	auto& dirLight = m_scene.GetComponent<DENG::DirectionalLightComponent>(m_idDirectionalLight);
	dirLight.vDiffuse = { 0.33f, 0.22f, 0.22f, 0.f };
	dirLight.vSpecular = { 0.66f, 0.44f, 0.44f, 0.f };
	dirLight.vDirection = { 0.f, -1.f, 0.f };
	
	auto& spotLight = m_scene.GetComponent<DENG::SpotlightComponent>(m_idSpotLight);
	spotLight.vDiffuse = { 0.5f, 0.f, 0.f, 0.f };
	spotLight.vSpecular = { 1.f, 0.f, 0.f, 0.f };

	auto& pointLightTransform = m_scene.GetComponent<DENG::TransformComponent>(m_idPointLight);
	pointLightTransform.vScale = { 0.1f, 0.1f, 0.1f };
	pointLightTransform.vTranslation = { 1.f, 1.f, 0.f, 0.f };

	auto& spotLightTransform = m_scene.GetComponent<DENG::TransformComponent>(m_idSpotLight);
	spotLightTransform.vScale = { 0.1f, 0.1f, 0.1f };
	spotLightTransform.vTranslation = { 1.f, 0.f, 1.f, 0.f };
}

void BasicLightingLayer::_ImGuiDrawLightProperties() {
	auto& [pointLight, pointLightTransform] = m_scene.GetRegistry().get<DENG::PointLightComponent, DENG::TransformComponent>(m_idPointLight);
	auto& dirLight = m_scene.GetRegistry().get<DENG::DirectionalLightComponent>(m_idDirectionalLight);
	auto& [spotLight, spotLightTransform] = m_scene.GetRegistry().get<DENG::SpotlightComponent, DENG::TransformComponent>(m_idSpotLight);
	
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

BasicLightingLayer::BasicLightingLayer(DENG::EventManager& _eventManager, DENG::IRenderer* _pRenderer, DENG::IFramebuffer* _pFramebuffer) :
	ILayer(_eventManager),
	m_scene(_pRenderer, _pFramebuffer)
{
	m_pRenderer = _pRenderer;
	m_eventManager.AddListener<BasicLightingLayer, DENG::WindowResizedEvent>(&BasicLightingLayer::OnWindowResizedEvent, this);
}

bool BasicLightingLayer::OnWindowResizedEvent(DENG::WindowResizedEvent& _event) {
	m_pRenderer->UpdateViewport(_event.GetWidth(), _event.GetHeight());
	return true;
}

void BasicLightingLayer::Attach(DENG::IRenderer*, DENG::IWindowContext* _pWindowContext) {
	m_pWindowContext = _pWindowContext;

	size_t uVertexOffset = m_pRenderer->AllocateMemory(sizeof(g_cCubeVertices), DENG::BufferDataType::VERTEX);
	m_pRenderer->UpdateBuffer(g_cCubeVertices, sizeof(g_cCubeVertices), uVertexOffset);

	m_idPointLight = m_scene.CreateEntity();
	m_scene.EmplaceComponent<DENG::MeshComponent>(m_idPointLight);
	m_scene.EmplaceComponent<DENG::ShaderComponent>(m_idPointLight);
	m_scene.EmplaceComponent<DENG::TransformComponent>(m_idPointLight);
	m_scene.EmplaceComponent<DENG::PointLightComponent>(m_idPointLight);

	m_idDirectionalLight = m_scene.CreateEntity();
	m_scene.EmplaceComponent<DENG::DirectionalLightComponent>(m_idDirectionalLight);

	m_idSpotLight = m_scene.CreateEntity();
	m_scene.EmplaceComponent<DENG::MeshComponent>(m_idSpotLight);
	m_scene.EmplaceComponent<DENG::ShaderComponent>(m_idSpotLight);
	m_scene.EmplaceComponent<DENG::TransformComponent>(m_idSpotLight);
	m_scene.EmplaceComponent<DENG::SpotlightComponent>(m_idSpotLight);

	m_idShadedCube = m_scene.CreateEntity();
	m_scene.EmplaceComponent<DENG::MeshComponent>(m_idShadedCube);
	m_scene.EmplaceComponent<DENG::ShaderComponent>(m_idShadedCube);
	m_scene.EmplaceComponent<DENG::TransformComponent>(m_idShadedCube);
	m_scene.EmplaceComponent<DENG::MaterialComponent>(m_idShadedCube);

	try {
		m_mapIds[0] = _LoadTexture("Textures\\Container\\diffuse.png");
		m_mapIds[1] = _LoadTexture("Textures\\Container\\specular.png");
	}
	catch (const DENG::IOException& e) {
		DISPATCH_ERROR_MESSAGE("IOException", e.what(), ErrorSeverity::CRITICAL);
	}

	DENG::Entity idCamera = m_scene.CreateEntity();
	m_scene.EmplaceComponent<DENG::CameraComponent>(idCamera);
	m_scene.EmplaceComponent<DENG::ScriptComponent>(idCamera).BindScript<CameraScript>(idCamera, m_eventManager, m_scene);
	m_scene.SetMainCamera(idCamera);
	_CreateShaders();
	_CreateMeshes();
	_InitialiseTransformsAndLights();
	m_scene.AttachComponents();
}

void BasicLightingLayer::Update(DENG::IFramebuffer* _pFramebuffer) {
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

	auto& material = m_scene.GetComponent<DENG::MaterialComponent>(m_idShadedCube);
	auto& selectedMaterial = m_cMaterials.find(s_szCurrentItem)->second;
	material.vAmbient = selectedMaterial.vAmbient;
	material.vDiffuse = selectedMaterial.vDiffuse;
	material.vSpecular = selectedMaterial.vSpecular;
	material.vMaps = { 0, 0, 0, 0 };

	static bool bUseMaps = false;
	ImGui::Checkbox("Use diffuse and specular maps", &bUseMaps);

	if (bUseMaps) {
		material.vMaps[0] = m_mapIds[0];
		material.vMaps[1] = m_mapIds[1];

		ImGui::SliderFloat("Specular shininess", &material.fShininess, 0.f, 1.f);
	}
	else {
		material.fShininess = selectedMaterial.fShininess;
	}

	ImGui::End();
}