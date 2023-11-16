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
#include "deng/FileTextureBuilder.h"
#include "deng/Exceptions.h"
#include "deng/SkyboxBuilders.h"
#include "deng/ResourceIdTable.h"
#include "deng/Layers/CameraScript.h"
#include "deng/Layers/GrassResourceBuilders.h"
#endif

namespace DENG {

	class GrassLayer : public ILayer {
		private:
			Scene m_scene;
			IRenderer* m_pRenderer = nullptr;
			Entity m_idGrass = entt::null;
			Entity m_idCamera = entt::null;
			Entity m_idTerrain = entt::null;
			Entity m_idSkybox = entt::null;
			std::size_t m_uTimerOffset = 0;

			const std::chrono::time_point<std::chrono::high_resolution_clock> m_begTimestamp = 
				std::chrono::high_resolution_clock::now();
			std::chrono::time_point<std::chrono::high_resolution_clock> m_endTimestamp =
				std::chrono::high_resolution_clock::now();

		public:
			GrassLayer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer);
			virtual void Attach(IRenderer* _pRenderer, IWindowContext* _pWindowContext, IGraphicsShaderCompiler* _pCompiler) override;
			virtual void Update(IFramebuffer* _pFramebuffer) override;

			bool OnWindowResizedEvent(WindowResizedEvent& _event);
	};
}

#endif