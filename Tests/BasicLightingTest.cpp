#include <random>
#define _USE_MATH_CONSTANTS
#include <cmath>

#include "deng/App.h"
#include "deng/ILayer.h"
#include "deng/SDLWindowContext.h"
#include "deng/VulkanRenderer.h"
#include "deng/Exceptions.h"
#include "deng/ErrorDefinitions.h"
#include "deng/ImGuiLayer.h"
#include "deng/Scene.h"

#define WIDTH 1280
#define HEIGHT 720
#define SQ(x) (x*x)

static const float g_cCubeVertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

class LightScript : public DENG::ScriptBehaviour {
	private:
		const float m_cfRotationSpeed = (float)M_PI_4;
		float m_fCurrentRotation = 0.f;
		float m_fRadius = 0.f;

	public:
		LightScript() = default;

		void OnAttach() {
			DENG::LightComponent& light = pScene->GetComponent<DENG::LightComponent>(idEntity);
			
			m_fRadius = 2.5f;
			LOG("Light source radius from y-axis is " << m_fRadius);

			// set light source color and initial position
			light.vColor = { 0.f, 0.f, 1.f, 1.f };
			light.vPosition[1] = 0.67f;
			light.vPosition[2] = m_fRadius;
		}

		void OnUpdate(float _fTimestep) {
			DENG::LightComponent& light = pScene->GetComponent<DENG::LightComponent>(idEntity);
			m_fCurrentRotation += _fTimestep * m_cfRotationSpeed;

			// clip rotation
			if (m_fCurrentRotation > 2 * (float)M_PI) {
				m_fCurrentRotation -= 2 * (float)M_PI;
			}

			light.vPosition.first = std::sinf(m_fCurrentRotation) * m_fRadius;
			light.vPosition.third = std::cosf(m_fCurrentRotation) * m_fRadius;
		}
};


class LightScript2 : public DENG::ScriptBehaviour {
private:
	const float m_cfRotationSpeed = (float)M_PI_4;
	float m_fCurrentRotation = 0.f;
	float m_fRadius = 0.f;

public:
	LightScript2() = default;

	void OnAttach() {
		DENG::LightComponent& light = pScene->GetComponent<DENG::LightComponent>(idEntity);

		m_fRadius = 2.5f;
		LOG("Light source radius from y-axis is " << m_fRadius);

		// set light source color and initial position
		light.vColor = { 0.f, 1.f, 0.f, 1.f };
		light.vPosition[1] = 0.67f;
		light.vPosition[2] = m_fRadius;
	}

	void OnUpdate(float _fTimestep) {
		DENG::LightComponent& light = pScene->GetComponent<DENG::LightComponent>(idEntity);
		m_fCurrentRotation += _fTimestep * m_cfRotationSpeed;

		// clip rotation
		if (m_fCurrentRotation > 2 * (float)M_PI) {
			m_fCurrentRotation -= 2 * (float)M_PI;
		}

		light.vPosition.first = -std::sinf(m_fCurrentRotation) * m_fRadius;
		light.vPosition.third = std::cosf(m_fCurrentRotation) * m_fRadius;
	}
};


class CameraScript : public DENG::ScriptBehaviour {
	private:
		DENG::IWindowContext* m_pWindowContext;
		const float m_fFieldOfView = 65.f * (float)M_PI / 180.f;
		const float m_fNearPlane = 0.0001f;
		const float m_fFarPlane = 1000000.f;

		const float m_fRotationSpeed = (float)M_PI_2;
		const float m_fMovementSpeed = 1.f;

		bool m_bRotations[4] = {};
		bool m_bMovements[4] = {};

		float m_fPitch = 0.f;
		float m_fYaw = 0.f;

		TRS::Vector3<float> m_vRight = { 1.f, 0.f, 0.f };
		TRS::Vector3<float> m_vUp = { 0.f, 1.f, 0.f };
		TRS::Vector3<float> m_vDirection = { 0.f, 0.f, -1.f };
		
	private:
		inline TRS::Matrix4<float> _CalculateProjection() {
			const float fInverseHalfTan = 1.f / std::tanf(m_fFieldOfView / 2.f);
			const float fAspectRatio = 
				static_cast<float>(m_pWindowContext->GetWidth()) / static_cast<float>(m_pWindowContext->GetHeight());

			return TRS::Matrix4<float> {
				{ fInverseHalfTan / fAspectRatio, 0.f, 0.f, 0.f },
				{ 0.f, fInverseHalfTan, 0.f, 0.f },
				{ 0.f, 0.f, (m_fNearPlane + m_fFarPlane) / (m_fNearPlane - m_fFarPlane), (2 * m_fFarPlane * m_fNearPlane) / (m_fNearPlane - m_fFarPlane) },
				{ 0.f, 0.f, -1.f, 0.f }
			};
		}

	public:
		CameraScript(DENG::IWindowContext* _pWindowContext) :
			m_pWindowContext(_pWindowContext) {}

		void OnAttach() {
			DENG::CameraComponent& camera = pScene->GetComponent<DENG::CameraComponent>(idEntity);
			camera.mProjection = _CalculateProjection().Transpose();
			LOG("CameraScript::OnAttach() called");
		}

		void OnUpdate(float _fTimestep) {
			DENG::CameraComponent& camera = pScene->GetComponent<DENG::CameraComponent>(idEntity);

			// poll keyboard events
			while (m_pWindowContext->HasEvents()) {
				DENG::Event& event = m_pWindowContext->PeekEvent();

				if (event.eType == DENG::EventType::KEY_DOWN || event.eType == DENG::EventType::KEY_UP) {
					switch (static_cast<DENG::KeySymbol>(event.uDescription)) {
					case DENG::KeySymbol::KEY_LEFT:
						m_bRotations[0] = event.eType == DENG::EventType::KEY_DOWN;
						break;

					case DENG::KeySymbol::KEY_UP:
						m_bRotations[1] = event.eType == DENG::EventType::KEY_DOWN;
						break;

					case DENG::KeySymbol::KEY_DOWN:
						m_bRotations[2] = event.eType == DENG::EventType::KEY_DOWN;
						break;

					case DENG::KeySymbol::KEY_RIGHT:
						m_bRotations[3] = event.eType == DENG::EventType::KEY_DOWN;
						break;

					case DENG::KeySymbol::KEY_A:
						m_bMovements[0] = event.eType == DENG::EventType::KEY_DOWN;
						break;

					case DENG::KeySymbol::KEY_W:
						m_bMovements[1] = event.eType == DENG::EventType::KEY_DOWN;
						break;

					case DENG::KeySymbol::KEY_S:
						m_bMovements[2] = event.eType == DENG::EventType::KEY_DOWN;
						break;

					case DENG::KeySymbol::KEY_D:
						m_bMovements[3] = event.eType == DENG::EventType::KEY_DOWN;
						break;

					default:
						break;
					}
				}

				m_pWindowContext->PopEvent();
			}

			// yaw counter-clockwise
			if (m_bRotations[0] && !m_bRotations[3]) {
				m_fYaw -= _fTimestep * m_fRotationSpeed;

				if (m_fYaw < -2.f * M_PI)
					m_fYaw += 2.f * M_PI;
			}
			// yaw clockwise
			else if (!m_bRotations[0] && m_bRotations[3]) {
				m_fYaw += _fTimestep * m_fRotationSpeed;

				if (m_fYaw > 2.f * M_PI)
					m_fYaw -= 2.f * M_PI;
			}

			// pitch counter-clockwise
			if (m_bRotations[1] && !m_bRotations[2]) {
				m_fPitch -= _fTimestep * m_fRotationSpeed;

				// clip rotation to pi/2
				if (m_fPitch < -M_PI_2)
					m_fPitch = -M_PI_2;
			}
			// pitch clockwise
			else if (!m_bRotations[1] && m_bRotations[2]) {
				m_fPitch += _fTimestep * m_fRotationSpeed;

				if (m_fPitch > M_PI_2)
					m_fPitch = M_PI_2;
			}

			// move left
			float fMovementX = 0.f;
			if (m_bMovements[0] && !m_bMovements[3]) {
				fMovementX -= _fTimestep * m_fMovementSpeed;
			}
			// move right
			else if (!m_bMovements[0] && m_bMovements[3]) {
				fMovementX += _fTimestep * m_fMovementSpeed;
			}

			// move forward
			float fMovementZ = 0.f;
			if (m_bMovements[1] && !m_bMovements[2]) {
				fMovementZ -= _fTimestep * m_fMovementSpeed;
			}
			// move backward
			else if (!m_bMovements[1] && m_bMovements[2]) {
				fMovementZ += _fTimestep * m_fMovementSpeed;
			}
			
			m_vDirection.first = std::cosf(m_fYaw) * std::cosf(m_fPitch);
			m_vDirection.second = std::sinf(m_fPitch);
			m_vDirection.third = std::sinf(m_fYaw) * std::cosf(m_fPitch);
			m_vDirection.Normalise();
		
			const TRS::Vector3<float> cvUp = { 0.f, 1.f, 0.f };
			m_vRight = TRS::Vector3<float>::Cross(cvUp, m_vDirection);
			m_vRight.Normalise();
			m_vUp = TRS::Vector3<float>::Cross(m_vDirection, m_vRight);
			m_vUp.Normalise();

			const TRS::Vector3<float> vMovement = (m_vRight * fMovementX) + (m_vDirection * fMovementZ);
			camera.vPosition.first += vMovement.first;
			camera.vPosition.second += vMovement.second;
			camera.vPosition.third += vMovement.third;

			TRS::Matrix4<float> mTranslation;
			mTranslation[0][3] = -camera.vPosition.first;
			mTranslation[1][3] = -camera.vPosition.second;
			mTranslation[2][3] = -camera.vPosition.third;
			camera.mView = {
				{ m_vRight.first, m_vRight.second, m_vRight.third, 0.f },
				{ m_vUp.first, m_vUp.second, m_vUp.third, 0.f },
				{ m_vDirection.first, m_vDirection.second, m_vDirection.third, 0.f },
				{ 0.f, 0.f, 0.f, 1.f }
			};

			camera.mView *= mTranslation;
			camera.mView = camera.mView.Transpose();
		}

		void OnDestroy() {
			LOG("CameraScript::OnUpdate() called");
		}
};

class BasicLightingLayer : public DENG::ILayer {
	private:
		DENG::Scene m_scene;
		DENG::Entity m_idWhiteCube = entt::null;
		DENG::Entity m_idShadedCube = entt::null;
		std::array<DENG::Entity, 2> m_lightSources = {};

	private:
		// ubos structure
		// 1. Camera uniform
		// 2. Transform uniform
		// 3. Lights uniform
		void _CreateShaderComponent(DENG::ShaderComponent& _shader, const std::string& _sShaderName) {
			_shader.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC3_FLOAT);
			_shader.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC3_FLOAT);
			_shader.attributeStrides.push_back(6u * sizeof(float));
			_shader.attributeStrides.push_back(6u * sizeof(float));

			_shader.uboDataLayouts.emplace_back();
			_shader.uboDataLayouts.back().block.uBinding = 0;
			_shader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_BUFFER;
			_shader.uboDataLayouts.back().iStage = SHADER_STAGE_VERTEX | SHADER_STAGE_FRAGMENT;
			_shader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_SHADER;

			_shader.uboDataLayouts.emplace_back();
			_shader.uboDataLayouts.back().block.uBinding = 1;
			_shader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_BUFFER;
			_shader.uboDataLayouts.back().iStage = SHADER_STAGE_VERTEX;
			_shader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_MESH;
			_shader.eCullMode = DENG::CULL_MODE_NONE;

			_shader.uboDataLayouts.emplace_back();
			_shader.uboDataLayouts.back().block.uBinding = 2;
			_shader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_BUFFER;
			_shader.uboDataLayouts.back().iStage = SHADER_STAGE_FRAGMENT;
			_shader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_SHADER;

			_shader.pShader = new DENG::Shader(_sShaderName, _sShaderName);
			_shader.bEnableBlend = true;
			_shader.bEnableIndexing = false;
			_shader.bEnableDepthTesting = true;
		}

	public:
		BasicLightingLayer(DENG::IRenderer* _pRenderer, DENG::IFramebuffer* _pFramebuffer) :
			m_scene(_pRenderer, _pFramebuffer) 
		{
			m_pRenderer = _pRenderer;
		}

		virtual void Attach(DENG::IRenderer*, DENG::IWindowContext* _pWindowContext) {
			m_pWindowContext = _pWindowContext;
			
			size_t uVertexOffset = m_pRenderer->AllocateMemory(sizeof(g_cCubeVertices), DENG::BufferDataType::VERTEX);
			m_pRenderer->UpdateBuffer(g_cCubeVertices, sizeof(g_cCubeVertices), uVertexOffset);
			
			m_idWhiteCube = m_scene.CreateEntity();
			m_scene.EmplaceComponent<DENG::MeshComponent>(m_idWhiteCube);
			m_scene.EmplaceComponent<DENG::ShaderComponent>(m_idWhiteCube);
			m_scene.EmplaceComponent<DENG::TransformComponent>(m_idWhiteCube);

			m_idShadedCube = m_scene.CreateEntity();
			m_scene.EmplaceComponent<DENG::MeshComponent>(m_idShadedCube);
			m_scene.EmplaceComponent<DENG::ShaderComponent>(m_idShadedCube);
			m_scene.EmplaceComponent<DENG::TransformComponent>(m_idShadedCube);

			// white cube mesh
			auto& whiteMesh = m_scene.GetComponent<DENG::MeshComponent>(m_idWhiteCube);
			whiteMesh.sName = "WhiteCube";
			whiteMesh.drawCommands.emplace_back();
			whiteMesh.drawCommands.back().uDrawCount = 36;
			whiteMesh.drawCommands.back().attributeOffsets.push_back(uVertexOffset);
			whiteMesh.drawCommands.back().attributeOffsets.push_back(uVertexOffset + 3 * sizeof(float));

			// shaded cube mesh
			auto& shadedMesh = m_scene.GetComponent<DENG::MeshComponent>(m_idShadedCube);
			shadedMesh.drawCommands = whiteMesh.drawCommands;
			shadedMesh.sName = "ShadedCube";

			// white cube shader
			auto& whiteCubeShader = m_scene.GetComponent<DENG::ShaderComponent>(m_idWhiteCube);
			_CreateShaderComponent(whiteCubeShader, "WhiteCube");

			// shaded cube shader
			auto& shadedCubeShader = m_scene.GetComponent<DENG::ShaderComponent>(m_idShadedCube);
			_CreateShaderComponent(shadedCubeShader, "ShadedCube");

			m_lightSources[0] = m_scene.CreateEntity();
			m_lightSources[1] = m_scene.CreateEntity();
			m_scene.EmplaceComponent<DENG::LightComponent>(m_lightSources[0]);
			m_scene.EmplaceComponent<DENG::ScriptComponent>(m_lightSources[0]).BindScript<LightScript>(m_lightSources[0], &m_scene);
			m_scene.EmplaceComponent<DENG::LightComponent>(m_lightSources[1]);
			m_scene.EmplaceComponent<DENG::ScriptComponent>(m_lightSources[1]).BindScript<LightScript2>(m_lightSources[1], &m_scene);

			// camera components
			{
				DENG::Entity idCamera = m_scene.CreateEntity();
				m_scene.SetMainCamera(idCamera);
				auto& camera = m_scene.EmplaceComponent<DENG::CameraComponent>(idCamera);
				m_scene.EmplaceComponent<DENG::ScriptComponent>(idCamera).BindScript<CameraScript>(idCamera, &m_scene, m_pWindowContext);
			}


			// update transform uniforms
			{
				DENG::TransformComponent& whiteCubeTransform = m_scene.GetComponent<DENG::TransformComponent>(m_idWhiteCube);
				whiteCubeTransform.vTranslation.first = -0.5f;
				whiteCubeTransform.vTranslation.third = -1.f;

				DENG::TransformComponent& shadedCubeTransform = m_scene.GetComponent<DENG::TransformComponent>(m_idShadedCube);
				shadedCubeTransform.vTranslation.first = 0.5f;
				shadedCubeTransform.vTranslation.third = 1.f;
			}

			m_scene.AttachComponents();
		}

		virtual void Update(DENG::IFramebuffer* _pFramebuffer) override {
			m_scene.RenderScene();
		}
};

class LightingTestApp : public DENG::App {
	public:
		LightingTestApp() {
			DENG::IWindowContext* pWindowContext = SetWindowContext(new DENG::SDLWindowContext);
			DENG::IRenderer* pRenderer = SetRenderer(new DENG::VulkanRenderer);
			DENG::IFramebuffer* pMainFramebuffer = nullptr;
			pWindowContext->SetHints(DENG::WindowHints::SHOWN | DENG::WindowHints::VULKAN);


			try {
				pWindowContext->Create("BasicLightingTest | Vulkan | SDL", WIDTH, HEIGHT);
				pMainFramebuffer = SetMainFramebuffer(pRenderer->CreateContext(pWindowContext));
			}
			catch (const DENG::WindowContextException& e) {
				DISPATCH_ERROR_MESSAGE("WindowContextException", e.what(), ErrorSeverity::CRITICAL);
			}
			catch (const DENG::RendererException& e) {
				DISPATCH_ERROR_MESSAGE("RendererException", e.what(), ErrorSeverity::CRITICAL);
			}
			catch (const DENG::HardwareException& e) {
				DISPATCH_ERROR_MESSAGE("HardwareException", e.what(), ErrorSeverity::CRITICAL);
			}

			PushLayer<BasicLightingLayer>(pRenderer, pMainFramebuffer);
			AttachLayers();
		}
};

DENG_MAIN_DECLARATION(LightingTestApp);