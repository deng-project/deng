// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: GrassLayer.h - GrassLayer class header
// author: Karl-Mihkel Ott

#ifndef GRASS_LAYER_H
#define GRASS_LAYER_H

#include "deng/ILayer.h"
#include "deng/IRenderer.h"
#include "deng/IWindowContext.h"
#include "deng/WindowEvents.h"
#include "deng/Scene.h"

#ifdef GRASS_LAYER_CPP
using namespace DENG;
#include "deng/FileTextureBuilder.h"
#include "deng/ResourceIdTable.h"
#include "../Utils/CameraScript.h"
#include "GrassResourceBuilders.h"
#endif

namespace Application {

	class GrassLayer : public DENG::ILayer {
		private:
			DENG::Scene m_scene;
			DENG::IRenderer* m_pRenderer = nullptr;
			DENG::Entity m_idGrass = entt::null;
			DENG::Entity m_idCamera = entt::null;

		public:
			GrassLayer(DENG::IRenderer* _pRenderer, DENG::IFramebuffer* _pFramebuffer);
			virtual void Attach(DENG::IRenderer* _pRenderer, DENG::IWindowContext* _pWindowContext) override;
			virtual void Update(DENG::IFramebuffer* _pFramebuffer) override;

			bool OnWindowResizedEvent(DENG::WindowResizedEvent& _event);
	};
}

#endif