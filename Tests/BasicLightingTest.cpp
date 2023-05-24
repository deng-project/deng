#include <random>

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

class CameraScript : public DENG::ScriptBehaviour {
	public:
		void OnAttach() {
			LOG("CameraScript::OnAttach() called");
		}

		void OnUpdate(float _fTimestep) {
			LOG("CameraScript::OnUpdate() called with timestep " << _fTimestep);
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
		std::array<DENG::Entity, 2> m_lightSources;

		size_t m_uCameraOffset = 0;
		std::array<size_t, 2> m_transformOffsets = {};
		size_t m_uLightOffset = 0;

	private:
		// ubos structure
		// 1. Camera uniform
		// 2. Transform uniform
		// 3. Lights uniform
		void _CreateShaderComponent(DENG::ShaderComponent& _shader, const std::string& _sShaderName, size_t _uTransformOffset) {
			_shader.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC3_FLOAT);
			_shader.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC3_FLOAT);
			_shader.attributeStrides.push_back(6u * sizeof(float));
			_shader.attributeStrides.push_back(6u * sizeof(float));

			_shader.uboDataLayouts.emplace_back();
			_shader.uboDataLayouts.back().block.uBinding = 0;
			_shader.uboDataLayouts.back().block.uOffset = static_cast<uint32_t>(m_uCameraOffset);
			_shader.uboDataLayouts.back().block.uSize = static_cast<uint32_t>(sizeof(DENG::CameraComponent));
			_shader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_BUFFER;
			_shader.uboDataLayouts.back().iStage = SHADER_STAGE_VERTEX | SHADER_STAGE_FRAGMENT;
			_shader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_SHADER;

			_shader.uboDataLayouts.emplace_back();
			_shader.uboDataLayouts.back().block.uBinding = 1;
			_shader.uboDataLayouts.back().block.uOffset = static_cast<uint32_t>(_uTransformOffset);
			_shader.uboDataLayouts.back().block.uSize = static_cast<uint32_t>(sizeof(DENG::TransformComponent));
			_shader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_BUFFER;
			_shader.uboDataLayouts.back().iStage = SHADER_STAGE_VERTEX;
			_shader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_MESH;

			_shader.uboDataLayouts.emplace_back();
			_shader.uboDataLayouts.back().block.uBinding = 2;
			_shader.uboDataLayouts.back().block.uOffset = static_cast<uint32_t>(m_uLightOffset);
			_shader.uboDataLayouts.back().block.uSize = static_cast<uint32_t>(2u * sizeof(DENG::LightComponent));
			_shader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_BUFFER;
			_shader.uboDataLayouts.back().iStage = SHADER_STAGE_FRAGMENT;
			_shader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_SHADER;

			_shader.pShader = new DENG::Shader(_sShaderName, _sShaderName);
			_shader.bEnableBlend = true;
			_shader.bEnableIndexing = false;
			_shader.bEnableDepthTesting = true;
		}

	public:
		BasicLightingLayer(DENG::IFramebuffer* _pFramebuffer) :
			m_scene(_pFramebuffer) {}

		virtual void Attach(DENG::IRenderer* _pRenderer, DENG::IWindowContext* _pWindowContext) {
			m_pRenderer = _pRenderer;
			m_pWindowContext = _pWindowContext;
			
			size_t uVertexOffset = _pRenderer->AllocateMemory(sizeof(g_cCubeVertices), DENG::BufferDataType::VERTEX);
			_pRenderer->UpdateBuffer(g_cCubeVertices, sizeof(g_cCubeVertices), uVertexOffset);
			
			m_uCameraOffset = _pRenderer->AllocateMemory(sizeof(DENG::CameraComponent), DENG::BufferDataType::UNIFORM);
			m_transformOffsets = {
				_pRenderer->AllocateMemory(sizeof(DENG::TransformComponent), DENG::BufferDataType::UNIFORM),
				_pRenderer->AllocateMemory(sizeof(DENG::TransformComponent), DENG::BufferDataType::UNIFORM)
			};
			m_uLightOffset = _pRenderer->AllocateMemory(2u * sizeof(DENG::LightComponent), DENG::BufferDataType::UNIFORM);

			m_idWhiteCube = m_scene.CreateEntity();
			m_scene.EmplaceComponent<DENG::MeshComponent>(m_idWhiteCube);
			m_scene.EmplaceComponent<DENG::ShaderComponent>(m_idWhiteCube);
			m_scene.EmplaceComponent<DENG::TransformComponent>(m_idWhiteCube);

			m_idShadedCube = m_scene.CreateEntity();
			m_scene.EmplaceComponent<DENG::MeshComponent>(m_idShadedCube);
			m_scene.EmplaceComponent<DENG::ShaderComponent>(m_idShadedCube);
			m_scene.EmplaceComponent<DENG::TransformComponent>(m_idShadedCube);

			// white cube mesh
			auto whiteMesh = m_scene.GetComponent<DENG::MeshComponent>(m_idWhiteCube);
			whiteMesh.sName = "WhiteCube";
			whiteMesh.drawCommands.emplace_back();
			whiteMesh.drawCommands.back().uDrawCount = 36;
			whiteMesh.drawCommands.back().attributeOffsets.push_back(uVertexOffset);
			whiteMesh.drawCommands.back().attributeOffsets.push_back(uVertexOffset + 3 * sizeof(float));

			// shaded cube mesh
			auto shadedMesh = m_scene.GetComponent<DENG::MeshComponent>(m_idShadedCube);
			shadedMesh.drawCommands = whiteMesh.drawCommands;
			shadedMesh.sName = "ShadedCube";

			// white cube shader
			auto& whiteCubeShader = m_scene.GetComponent<DENG::ShaderComponent>(m_idWhiteCube);
			_CreateShaderComponent(whiteCubeShader, "WhiteCube", m_transformOffsets[0]);

			// shaded cube shader
			auto& shadedCubeShader = m_scene.GetComponent<DENG::ShaderComponent>(m_idShadedCube);
			_CreateShaderComponent(shadedCubeShader, "ShadedCube", m_transformOffsets[1]);

			m_lightSources[0] = m_scene.CreateEntity();
			m_lightSources[1] = m_scene.CreateEntity();
			m_scene.EmplaceComponent<DENG::LightComponent>(m_lightSources[0]);
			m_scene.EmplaceComponent<DENG::LightComponent>(m_lightSources[1]);

			// camera components
			{
				DENG::Entity idCamera = m_scene.CreateEntity();
				auto& camera = m_scene.EmplaceComponent<DENG::CameraComponent>(idCamera);
				m_scene.EmplaceComponent<DENG::ScriptComponent>(idCamera).BindScript<CameraScript>(idCamera, &m_scene);
				_pRenderer->UpdateBuffer(&camera, sizeof(DENG::CameraComponent), m_uCameraOffset);
			}

			// update light uniforms
			{
				auto view = m_scene.GetRegistry().view<DENG::LightComponent>();
				std::default_random_engine engRandom((unsigned)time(nullptr));
				std::uniform_real_distribution<float> realDistPosition(1.f, 2.f);
				std::uniform_real_distribution<float> realDistColor(0.f, 1.f);
				std::bernoulli_distribution boolDistSign(0.5);

				size_t uCount = 0;
				for (DENG::Entity idLight : view) {
					auto& light = m_scene.GetComponent<DENG::LightComponent>(idLight);

					if (boolDistSign(engRandom))
						light.vPosition.first = realDistPosition(engRandom);
					else light.vPosition.first = -realDistPosition(engRandom);

					if (boolDistSign(engRandom))
						light.vPosition.second = realDistPosition(engRandom);
					else light.vPosition.second = -realDistPosition(engRandom);

					if (boolDistSign(engRandom))
						light.vPosition.third = realDistPosition(engRandom);
					else light.vPosition.third = realDistPosition(engRandom);
					light.vPosition.fourth = 1.0f;

					light.vColor = TRS::Vector4<float>{
						realDistColor(engRandom),
						realDistColor(engRandom),
						realDistColor(engRandom),
						1.f
					};

					LOG("Light position { " << light.vPosition.first << ", " << light.vPosition.second << ", " <<
						light.vPosition.third << " } and color { " << light.vColor.first << ", " << light.vColor.second << ", " <<
						light.vColor.third << " }");

					_pRenderer->UpdateBuffer(&light, sizeof(DENG::LightComponent), m_uLightOffset + sizeof(DENG::LightComponent) * uCount);
					uCount++;
				}

				// update transform uniforms
				{
					DENG::TransformComponent& whiteCubeTransform = m_scene.GetComponent<DENG::TransformComponent>(m_idWhiteCube);
					whiteCubeTransform.vTranslation.third = -1.f;

					DENG::TransformComponent& shadedCubeTransform = m_scene.GetComponent<DENG::TransformComponent>(m_idShadedCube);
				
					_pRenderer->UpdateBuffer(&whiteCubeTransform, sizeof(DENG::TransformComponent), m_transformOffsets[0]);
					_pRenderer->UpdateBuffer(&shadedCubeShader, sizeof(DENG::TransformComponent), m_transformOffsets[1]);
				}
			}
		}

		virtual void Update(DENG::IFramebuffer* _pFramebuffer) override {
			m_scene.RenderScene(m_pRenderer);
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
				pWindowContext->Create("LightingTest | Vulkan | SDL", WIDTH, HEIGHT);
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

			PushLayer<BasicLightingLayer>(pMainFramebuffer);
			AttachLayers();
		}
};

DENG_MAIN_DECLARATION(LightingTestApp);