// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: BasicLightingLayer.h - BasicLightingLayer class header
// author: Karl-Mihkel Ott

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
	#include "deng/ProgramFilesManager.h"
	#include "deng/ResourceIdTable.h"
	#include "deng/InitializerListBuilder.h"
	#include "deng/Layers/CubeResourceBuilders.h"
	#include "deng/Layers/LightSourceBuilders.h"
	#include "deng/FileTextureBuilder.h"
	#include <das/stb_image.h>

	#include "deng/Layers/CameraScript.h"
	#include "deng/Layers/CubeVertices.h"

	#define BOUNDS 50.f
#endif

namespace DENG {
	class BasicLightingLayer : public ILayer {
		private:
			Scene m_scene;

			// lights
			Entity m_idPointLight = entt::null;
			Entity m_idDirectionalLight = entt::null;
			Entity m_idSpotLight = entt::null;

			Entity m_idShadedCube = entt::null;
			std::array<uint32_t, 2> m_mapIds = {};

			const std::unordered_map<const char*, hash_t> m_hshLookup;

		private:
			void _SetupMaterials();
			void _InitialiseTransformsAndLights();
			void _ImGuiDrawLightProperties();

		public:
			BasicLightingLayer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer);

			virtual void Attach(IRenderer*, IWindowContext* _pWindowContext) override;
			virtual void Update(IFramebuffer* _pFramebuffer) override;

			bool OnWindowResizedEvent(WindowResizedEvent& _event);
			void OnImGuiDraw();
	};
}

#endif
