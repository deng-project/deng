#ifndef BASIC_LIGHTING_LAYER_H
#define BASIC_LIGHTING_LAYER_H

#include "deng/Scene.h"
#include "deng/ILayer.h"
#include "deng/WindowEvents.h"

#ifdef BASIC_LIGHTING_LAYER_CPP
	#include "trs/Vector.h"
	#include "trs/Quaternion.h"

	#include "deng/Exceptions.h"
	#include "deng/ErrorDefinitions.h"
	#include "deng/ImGuiLayer.h"
	#include <das/stb_image.h>

	#include "CameraScript.h"
	#include "CubeVertices.h"

	#define BOUNDS 50.f
#endif

class BasicLightingLayer : public DENG::ILayer {
	private:
		DENG::Scene m_scene;

		// lights
		DENG::Entity m_idPointLight = entt::null;
		DENG::Entity m_idDirectionalLight = entt::null;
		DENG::Entity m_idSpotLight = entt::null;

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
		void _CreateShaderComponent(DENG::ShaderComponent& _shader, const std::string& _sShaderName);
		uint32_t _LoadTexture(const std::string& _filename);

		void _CreateShaders();
		void _CreateMeshes();
		void _InitialiseTransformsAndLights();
		void _ImGuiDrawLightProperties();

	public:
		BasicLightingLayer(DENG::EventManager& _eventManager, DENG::IRenderer* _pRenderer, DENG::IFramebuffer* _pFramebuffer);

		virtual void Attach(DENG::IRenderer*, DENG::IWindowContext* _pWindowContext) override;
		virtual void Update(DENG::IFramebuffer* _pFramebuffer) override;
		
		bool OnWindowResizedEvent(DENG::WindowResizedEvent& _event);
		void OnImGuiDraw();
};

#endif
