// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: InstancedCubeLayer.h - Instance cube layer class header
// author: Karl-Mihkel Ott

#ifndef INSTANCED_CUBE_LAYER_H
#define INSTANCED_CUBE_LAYER_H

#include <array>
#include "deng/ILayer.h"
#include "deng/IRenderer.h"
#include "deng/IWindowContext.h"
#include "deng/WindowEvents.h"
#include "deng/Scene.h"

#ifdef INSTANCED_CUBE_LAYER_CPP
using namespace DENG;
#include <das/stb_image.h>
#include "deng/ImGuiLayer.h"
#include "deng/Exceptions.h"
#include "../Utils/CameraScript.h"
#include "../Utils/CubeVertices.h"
#endif

#define SQ(x) (x*x)
#define ROW_LEN	316

namespace Application {

	class InstancedCubeLayer : public DENG::ILayer {
		private:
			DENG::Scene m_scene;
			DENG::IRenderer* m_pRenderer = nullptr;
			DENG::Entity m_prefabEntity = entt::null;
			std::array<uint32_t, 2> m_mapIds = {};

		private:
			uint32_t _LoadTexture(const std::string& _sPath, DENG::IRenderer* _pRenderer);
			void _CreatePrefab(DENG::IRenderer* _pRenderer);
			void _ApplyTransforms();

		public:
			InstancedCubeLayer(DENG::EventManager& _eventManager, DENG::IRenderer* _pRenderer, DENG::IFramebuffer* _pFramebuffer);
			virtual void Attach(DENG::IRenderer* _pRenderer, DENG::IWindowContext* _pWindowContext) override;
			virtual void Update(DENG::IFramebuffer* _pFramebuffer) override;

			bool OnWindowResizedEvent(DENG::WindowResizedEvent& _event);
			void OnImGuiDraw();
	};
}

#endif