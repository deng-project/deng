#include <random>
#include <cmath>
#include <unordered_map>

#include "deng/App.h"
#include "deng/ILayer.h"
#include "deng/SDLWindowContext.h"
#include "deng/VulkanRenderer.h"
#include "deng/Exceptions.h"
#include "deng/ErrorDefinitions.h"
#include "deng/CameraTransformer.h"
#include "deng/ImGuiLayer.h"
#include "deng/Scene.h"
#include "deng/WindowEvents.h"
#include "deng/InputEvents.h"

#include <das/stb_image.h>

#define WIDTH 1280
#define HEIGHT 720
#define SQ(x) (x*x)

static const float g_cCubeVertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};



class CameraScript : public DENG::ScriptBehaviour {
	private:
		DENG::CameraTransformer m_cameraTransformer;
		const float m_fRotationSpeed = (float)MF_PI_2;
		const float m_fMovementSpeed = 1.f;

		bool m_bRotations[6] = {};
		bool m_bMovements[4] = {};
		
	public:
		SCRIPT_DEFINE_CONSTRUCTOR(CameraScript)

		bool OnWindowResizedEvent(DENG::WindowResizedEvent& _event) {
			auto& camera = m_scene.GetComponent<DENG::CameraComponent>(m_idEntity);
			camera.mProjection = m_cameraTransformer.CalculateProjection(_event.GetWidth(), _event.GetHeight());
			return false; // pass the even to other listeners
		}


		bool OnKeyPressEvent(DENG::KeyPressedEvent& _event) {
			switch (_event.GetKeycode()) {
				case DENG::Keycode::LeftArrow:
					m_bRotations[0] = true;
					return true;

				case DENG::Keycode::UpArrow:
					m_bRotations[1] = true;
					return true;

				case DENG::Keycode::DownArrow:
					m_bRotations[2] = true;
					return true;

				case DENG::Keycode::RightArrow:
					m_bRotations[3] = true;
					return true;

				case DENG::Keycode::LeftBracket:
					m_bRotations[4] = true;
					return true;

				case DENG::Keycode::RightBracket:
					m_bRotations[5] = true;
					return true;

				case DENG::Keycode::A:
					m_bMovements[0] = true;
					return true;

				case DENG::Keycode::W:
					m_bMovements[1] = true;
					return true;

				case DENG::Keycode::S:
					m_bMovements[2] = true;
					return true;

				case DENG::Keycode::D:
					m_bMovements[3] = true;
					return true;

				default:
					return false;
			}
		}


		bool OnKeyReleasedEvent(DENG::KeyReleasedEvent& _event) {
			switch (_event.GetKeycode()) {
				case DENG::Keycode::LeftArrow:
					m_bRotations[0] = false;
					return true;

				case DENG::Keycode::UpArrow:
					m_bRotations[1] = false;
					return true;

				case DENG::Keycode::DownArrow:
					m_bRotations[2] = false;
					return true;

				case DENG::Keycode::RightArrow:
					m_bRotations[3] = false;
					return true;

				case DENG::Keycode::LeftBracket:
					m_bRotations[4] = false;
					return true;

				case DENG::Keycode::RightBracket:
					m_bRotations[5] = false;
					return true;

				case DENG::Keycode::A:
					m_bMovements[0] = false;
					return true;

				case DENG::Keycode::W:
					m_bMovements[1] = false;
					return true;

				case DENG::Keycode::S:
					m_bMovements[2] = false;
					return true;

				case DENG::Keycode::D:
					m_bMovements[3] = false;
					return true;

				default:
					return false;
			}
		}
		
		void OnAttach() {
			auto& camera = m_scene.GetComponent<DENG::CameraComponent>(m_idEntity);
			camera.mProjection = m_cameraTransformer.CalculateProjection(WIDTH, HEIGHT);

			m_eventManager.AddListener<CameraScript, DENG::WindowResizedEvent>(&CameraScript::OnWindowResizedEvent, this);
			m_eventManager.AddListener<CameraScript, DENG::KeyPressedEvent>(&CameraScript::OnKeyPressEvent, this);
			m_eventManager.AddListener<CameraScript, DENG::KeyReleasedEvent>(&CameraScript::OnKeyReleasedEvent, this);

			LOG("CameraScript::OnAttach() called");
		}

		void OnUpdate(float _fTimestep) {
			DENG::CameraComponent& camera = m_scene.GetComponent<DENG::CameraComponent>(m_idEntity);
			
			// yaw counter-clockwise
			if (m_bRotations[0] && !m_bRotations[3]) {
				float fYaw = m_cameraTransformer.GetYaw();
				fYaw += _fTimestep * m_fRotationSpeed;

				if (fYaw > 2.f * MF_PI)
					fYaw -= 2.f * MF_PI;

				m_cameraTransformer.SetYaw(fYaw);
			}
			// yaw clockwise
			else if (!m_bRotations[0] && m_bRotations[3]) {
				float fYaw = m_cameraTransformer.GetYaw();
				fYaw -= _fTimestep * m_fRotationSpeed;

				if (fYaw < -2.f * MF_PI)
					fYaw += 2.f * MF_PI;
				m_cameraTransformer.SetYaw(fYaw);
			}

			// pitch clockwise
			if (m_bRotations[1] && !m_bRotations[2]) {
				float fPitch = m_cameraTransformer.GetPitch();
				fPitch -= _fTimestep * m_fRotationSpeed;

				// clip rotation to pi/2
				if (fPitch < -MF_PI_2)
					fPitch = -MF_PI_2;
				m_cameraTransformer.SetPitch(fPitch);
			}
			// pitch counter-clockwise
			else if (!m_bRotations[1] && m_bRotations[2]) {
				float fPitch = m_cameraTransformer.GetPitch();
				fPitch += _fTimestep * m_fRotationSpeed;

				if (fPitch > MF_PI_2)
					fPitch = MF_PI_2;
				m_cameraTransformer.SetPitch(fPitch);
			}

			// roll counter-clockwise
			if (m_bRotations[4] && !m_bRotations[5]) {
				float fRoll = m_cameraTransformer.GetRoll();
				fRoll -= _fTimestep * m_fRotationSpeed;

				if (fRoll < -2.f * MF_PI)
					fRoll += 2.f * MF_PI;
				m_cameraTransformer.SetRoll(fRoll);
			}
			// roll counter-clockwise
			else if (!m_bRotations[4] && m_bRotations[5]) {
				float fRoll = m_cameraTransformer.GetRoll();
				fRoll += _fTimestep * m_fRotationSpeed;

				if (fRoll > 2.f * MF_PI)
					fRoll -= 2.f * MF_PI;
				m_cameraTransformer.SetRoll(fRoll);
			}
			m_cameraTransformer.CalculateLookAt();

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
			m_cameraTransformer.RelativeMoveX(fMovementX);
			m_cameraTransformer.RelativeMoveZ(fMovementZ);
		
			camera.vCameraDirection = m_cameraTransformer.GetLookAtDirection();
			camera.vCameraRight = m_cameraTransformer.GetCameraRight();
			camera.vCameraUp = m_cameraTransformer.GetCameraUp();
			camera.vPosition = m_cameraTransformer.GetPosition();
		}

		void OnDestroy() {
			LOG("CameraScript::OnDestroy() called");
		}
};

class BasicLightingLayer : public DENG::ILayer {
	private:
		DENG::Scene m_scene;
		DENG::Entity m_idLightSource = entt::null;
		DENG::Entity m_idShadedCube = entt::null;
		std::array<uint32_t, 2> m_mapIds = {};

		using Vec4 = TRS::Vector4<uint32_t>;
		const std::unordered_map<std::string, DENG::MaterialComponent> m_cMaterials = {
			{ "default", DENG::MaterialComponent() },
			{ "emerald", { { 0.0215f, 0.1745f, 0.0215f, 0.f }, { 0.07568f, 0.61424f, 0.07568f, 0.f }, { 0.633f, 0.727811f, 0.633f, 0.f }, Vec4(), 0.6f } },
			{ "jade", { { 0.135f, 0.2225f, 0.1575f, 0.f }, { 0.54f, 0.89f, 0.63f, 0.f }, { 0.316228f, 0.316228f, 0.316228f, 0.f }, Vec4(), 0.1f, } },
			{ "obsidian", { { 0.05375f, 0.05f, 0.06625f, 0.f }, { 0.18275f, 0.17f, 0.22525f, 0.f }, { 0.332741f, 0.328634f, 0.346435f, 0.f }, Vec4(), 0.3f } },
			{ "pearl", { { 0.25f, 0.20725f, 0.20725f, 0.f }, { 1.0f, 0.829f, 0.829f, 0.f }, { 0.296648f, 0.296648f, 0.296648f, 0.f }, Vec4(), 0.088f } },
			{ "ruby", { { 0.1745f, 0.01175f, 0.01175f, 0.f }, { 0.61424f, 0.04136f, 0.04136f, 0.f }, { 0.727811f, 0.626959f, 0.626959f, 0.f }, Vec4(), 0.6f } },
			{ "turquoise", { { 0.1f, 0.18725f, 0.1745f, 0.f }, { 0.396f, 0.74151f, 0.69102f, 0.f }, { 0.297254f, 0.30829f, 0.306678f, 0.f }, Vec4(), 0.1f, } },
			{ "brass", { { 0.329412f, 0.223529f, 0.027451f, 0.f }, { 0.780392f, 0.568627f, 0.113725f, 0.f }, { 0.992157f, 0.941176f, 0.807843f, 0.f }, Vec4(), 0.21794872f } },
			{ "bronze", { { 0.2125f, 0.1275f, 0.054f, 0.f }, { 0.714f, 0.4284f, 0.18144f, 0.f }, { 0.393548f, 0.271906f, 0.166721f, 0.f }, Vec4(), 0.2f, } },
			{ "chrome", { { 0.25f, 0.25f, 0.25f, 0.f }, { 0.4f, 0.4f, 0.4f, 0.f }, { 0.774597f, 0.774597f, 0.774597f, 0.f }, Vec4(), 0.6f, } },
			{ "copper", { { 0.19125f, 0.0735f, 0.0225f, 0.f }, { 0.7038f, 0.27048f, 0.0828f, 0.f }, { 0.256777f, 0.137622f, 0.086014f, 0.f }, Vec4(), 0.1f, } },
			{ "gold", { { 0.24725f, 0.1995f, 0.0745f, 0.f }, { 0.75164f, 0.60648f, 0.22648f, 0.f }, { 0.628281f, 0.555802f, 0.366065f, 0.f }, Vec4(), 0.4f, } },
			{ "silver", { { 0.19225f, 0.19225f, 0.19225f, 0.f }, { 0.50754f, 0.50754f, 0.50754f, 0.f }, { 0.508273f, 0.508273f, 0.508273f, 0.f }, Vec4(), 0.4f, } },
			{ "black_plastic", { { 0.0f, 0.0f, 0.0f, 0.f }, { 0.01f, 0.01f, 0.01f, 0.f }, { 0.50f, 0.50f, 0.50f, 0.f }, Vec4(), 0.25f, } },
			{ "cyan_plastic", { { 0.0f, 0.1f, 0.06f, 0.f }, { 0.0f, 0.50980392f, 0.50980392f, 0.f }, { 0.50196078f, 0.50196078f, 0.50196078f, 0.f }, Vec4(), 0.25f, } },
			{ "green_plastic", { { 0.0f, 0.0f, 0.0f, 0.f }, { 0.1f, 0.35f, 0.1f, 0.f }, { 0.45f, 0.55f, 0.45f, 0.f }, Vec4(), 0.25f, } },
			{ "red_plastic", { { 0.0f, 0.0f, 0.0f, 0.f }, { 0.5f, 0.0f, 0.0f, 0.f }, { 0.7f, 0.6f, 0.6f, 0.f }, Vec4(), 0.25f, } },
			{ "white_plastic", { { 0.0f, 0.0f, 0.0f, 0.f }, { 0.55f, 0.55f, 0.55f, 0.f }, { 0.70f, 0.70f, 0.70f, 0.f }, Vec4(), 0.25f, } },
			{ "yellow_plastic", { { 0.0f, 0.0f, 0.0f, 0.f }, { 0.5f, 0.5f, 0.0f, 0.f }, { 0.60f, 0.60f, 0.50f, 0.f }, Vec4(), 0.25f, } },
			{ "black_rubber", { { 0.02f, 0.02f, 0.02f, 0.f }, { 0.01f, 0.01f, 0.01f, 0.f }, { 0.4f, 0.4f, 0.4f, 0.f }, Vec4(), 0.078125f, } },
			{ "cyan_rubber", { { 0.0f, 0.05f, 0.05f, 0.f }, { 0.4f, 0.5f, 0.5f, 0.f }, { 0.04f, 0.7f, 0.7f, 0.f }, Vec4(), 0.078125f, } },
			{ "green_rubber", { { 0.0f, 0.05f, 0.0f, 0.f }, { 0.4f, 0.5f, 0.4f, 0.f }, { 0.04f, 0.7f, 0.04f, 0.f }, Vec4(), 0.078125f, } },
			{ "red_rubber", { { 0.05f, 0.0f, 0.0f, 0.f }, { 0.5f, 0.4f, 0.4f, 0.f }, { 0.7f, 0.04f, 0.04f, 0.f }, Vec4(), 0.078125f, } },
			{ "white_rubber", { { 0.05f, 0.05f, 0.05f, 0.f }, { 0.5f, 0.5f, 0.5f, 0.f }, { 0.7f, 0.7f, 0.7f, 0.f }, Vec4(), 0.078125f, } },
			{ "yellow_rubber", { { 0.05f, 0.05f, 0.0f, 0.f }, { 0.5f, 0.5f, 0.4f, 0.f }, { 0.7f, 0.7f, 0.04f, 0.f }, Vec4(), 0.078125f, } },
		};




	private:
		// ubos structure
		// 1. Camera uniform
		// 2. Transform uniform
		// 3. Lights uniform
		void _CreateShaderComponent(DENG::ShaderComponent& _shader, const std::string& _sShaderName) {
			_shader.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC3_FLOAT);
			_shader.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC3_FLOAT);
			_shader.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC2_FLOAT);
			_shader.attributeStrides.push_back(8u * sizeof(float));
			_shader.attributeStrides.push_back(8u * sizeof(float));
			_shader.attributeStrides.push_back(8u * sizeof(float));

			_shader.uboDataLayouts.emplace_back();
			_shader.uboDataLayouts.back().block.uBinding = 0;
			_shader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_BUFFER;
			_shader.uboDataLayouts.back().iStage = SHADER_STAGE_VERTEX;
			_shader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_MESH;
			_shader.eCullMode = DENG::CULL_MODE_NONE;

			_shader.uboDataLayouts.emplace_back();
			_shader.uboDataLayouts.back().block.uBinding = 1;
			_shader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_STORAGE_BUFFER;
			_shader.uboDataLayouts.back().iStage = SHADER_STAGE_FRAGMENT;
			_shader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_SHADER;

			_shader.uboDataLayouts.emplace_back();
			_shader.uboDataLayouts.back().block.uBinding = 2;
			_shader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_BUFFER;
			_shader.uboDataLayouts.back().iStage = SHADER_STAGE_FRAGMENT;
			_shader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_MESH;

			_shader.uboDataLayouts.emplace_back();
			_shader.uboDataLayouts.back().block.uBinding = 3;
			_shader.uboDataLayouts.back().eType = DENG::UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER;
			_shader.uboDataLayouts.back().iStage = SHADER_STAGE_FRAGMENT;
			_shader.uboDataLayouts.back().eUsage = DENG::UNIFORM_USAGE_PER_MESH;

			_shader.uboDataLayouts.emplace_back();
			_shader.uboDataLayouts.back().block.uBinding = 4;
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


		uint32_t _LoadTexture(const std::string& _filename) {
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

	public:
		BasicLightingLayer(DENG::EventManager& _eventManager, DENG::IRenderer* _pRenderer, DENG::IFramebuffer* _pFramebuffer) :
			ILayer(_eventManager),
			m_scene(_pRenderer, _pFramebuffer) 
		{
			m_pRenderer = _pRenderer;
			m_eventManager.AddListener<BasicLightingLayer, DENG::WindowResizedEvent>(&BasicLightingLayer::OnWindowResizedEvent, this);
		}

		bool OnWindowResizedEvent(DENG::WindowResizedEvent& _event) {
			m_pRenderer->UpdateViewport(_event.GetWidth(), _event.GetHeight());
			return true;
		}

		virtual void Attach(DENG::IRenderer*, DENG::IWindowContext* _pWindowContext) {
			m_pWindowContext = _pWindowContext;
			
			size_t uVertexOffset = m_pRenderer->AllocateMemory(sizeof(g_cCubeVertices), DENG::BufferDataType::VERTEX);
			m_pRenderer->UpdateBuffer(g_cCubeVertices, sizeof(g_cCubeVertices), uVertexOffset);
			
			m_idLightSource = m_scene.CreateEntity();
			m_scene.EmplaceComponent<DENG::MeshComponent>(m_idLightSource);
			m_scene.EmplaceComponent<DENG::ShaderComponent>(m_idLightSource);
			m_scene.EmplaceComponent<DENG::TransformComponent>(m_idLightSource);
			auto& light = m_scene.EmplaceComponent<DENG::LightComponent>(m_idLightSource);
			light.vDiffuse = { 0.5f, 0.5f, 0.5f, 0.f };
			light.vSpecular = { 1.f, 1.f, 1.f, 0.f };

			m_idShadedCube = m_scene.CreateEntity();
			m_scene.EmplaceComponent<DENG::MeshComponent>(m_idShadedCube);
			m_scene.EmplaceComponent<DENG::ShaderComponent>(m_idShadedCube);
			m_scene.EmplaceComponent<DENG::TransformComponent>(m_idShadedCube);
			auto& material = m_scene.EmplaceComponent<DENG::MaterialComponent>(m_idShadedCube);
			
			try {
				m_mapIds[0] = _LoadTexture("Textures\\Container\\diffuse.png");
				m_mapIds[1] = _LoadTexture("Textures\\Container\\specular.png");
			}
			catch (const DENG::IOException& e) {
				DISPATCH_ERROR_MESSAGE("IOException", e.what(), ErrorSeverity::CRITICAL);
			}

			// white cube mesh
			auto& whiteMesh = m_scene.GetComponent<DENG::MeshComponent>(m_idLightSource);
			whiteMesh.sName = "WhiteCube";
			whiteMesh.drawCommands.emplace_back();
			whiteMesh.drawCommands.back().uDrawCount = 36;
			whiteMesh.drawCommands.back().attributeOffsets.push_back(uVertexOffset);
			whiteMesh.drawCommands.back().attributeOffsets.push_back(uVertexOffset + 3 * sizeof(float));
			whiteMesh.drawCommands.back().attributeOffsets.push_back(uVertexOffset + 6 * sizeof(float));

			// shaded cube mesh
			auto& shadedMesh = m_scene.GetComponent<DENG::MeshComponent>(m_idShadedCube);
			shadedMesh.drawCommands = whiteMesh.drawCommands;
			shadedMesh.sName = "ShadedCube";

			// white cube shader
			auto& whiteCubeShader = m_scene.GetComponent<DENG::ShaderComponent>(m_idLightSource);
			_CreateShaderComponent(whiteCubeShader, "WhiteCube");

			// shaded cube shader
			auto& shadedCubeShader = m_scene.GetComponent<DENG::ShaderComponent>(m_idShadedCube);
			_CreateShaderComponent(shadedCubeShader, "ShadedCube");

			// camera components
			{
				DENG::Entity idCamera = m_scene.CreateEntity();
				m_scene.SetMainCamera(idCamera);
				auto& camera = m_scene.EmplaceComponent<DENG::CameraComponent>(idCamera);
				m_scene.EmplaceComponent<DENG::ScriptComponent>(idCamera).BindScript<CameraScript>(idCamera, m_eventManager, m_scene);
			}


			// update transform uniforms
			{
				DENG::TransformComponent& whiteCubeTransform = m_scene.GetComponent<DENG::TransformComponent>(m_idLightSource);
				whiteCubeTransform.vTranslation.first = -6.f;
				whiteCubeTransform.vScale = { 0.1f, 0.1f, 0.1f };

				DENG::TransformComponent& shadedCubeTransform = m_scene.GetComponent<DENG::TransformComponent>(m_idShadedCube);
			}

			m_scene.AttachComponents();
		}

		virtual void Update(DENG::IFramebuffer* _pFramebuffer) override {
			m_scene.RenderScene();
		}


		void OnImGuiDraw() {
			ImGui::Begin("Diagnostics/Material properties");

			ImGui::Text("Current framerate: %d fps", static_cast<int>(ImGui::GetIO().Framerate));
			auto& [light, lightTransform] = m_scene.GetRegistry().get<DENG::LightComponent, DENG::TransformComponent>(m_idLightSource);
			ImGui::Text("Light position: { %f, %f, %f }", light.vPosition.first, light.vPosition.second, light.vPosition.third);

			ImGui::Separator();
			ImGui::SliderFloat("Light source translation X", &lightTransform.vTranslation.first, -50.f, 50.f);
			ImGui::SliderFloat("Light source translation Y", &lightTransform.vTranslation.second, -50.f, 50.f);
			ImGui::SliderFloat("Light source translation Z", &lightTransform.vTranslation.third, -50.f, 50.f);
			ImGui::Separator();
			ImGui::SliderAngle("Light source rotation X", &lightTransform.vRotation.first);
			ImGui::SliderAngle("Light source rotation Y", &lightTransform.vRotation.second);
			ImGui::SliderAngle("Light source rotation Z", &lightTransform.vRotation.third);
			ImGui::Separator();

			if (ImGui::TreeNode("Click to choose ambient color")) {
				ImGui::ColorPicker3("Ambient color", (float*)&m_scene.GetAmbient(), ImGuiColorEditFlags_NoPicker);
				ImGui::TreePop();
			}
			
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
};

class LightingTestApp : public DENG::App {
	public:
		LightingTestApp() {
			DENG::IWindowContext* pWindowContext = SetWindowContext(new DENG::SDLWindowContext(m_eventManager));
			DENG::IRenderer* pRenderer = SetRenderer(new DENG::VulkanRenderer);
			DENG::IFramebuffer* pMainFramebuffer = nullptr;
			pWindowContext->SetHints(DENG::WindowHint_Shown | DENG::WindowHint_Vulkan | DENG::WindowHint_Resizeable);

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

			auto lightingLayer = PushLayer<BasicLightingLayer>(pRenderer, pMainFramebuffer);
			auto pImGuiLayer = PushLayer<DENG::ImGuiLayer>();
			pImGuiLayer->SetDrawCallback(&BasicLightingLayer::OnImGuiDraw, lightingLayer);
			AttachLayers();
		}
};

DENG_MAIN_DECLARATION(LightingTestApp);