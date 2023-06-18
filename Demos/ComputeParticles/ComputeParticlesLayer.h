// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: ComputeParticlesLayer.h - Compute particles layer class header
// author: Karl-Mihkel Ott

#ifndef COMPUTE_PARTICLES_LAYER_H
#define COMPUTE_PARTICLES_LAYER_H

#include "deng/ILayer.h"
#include "deng/WindowEvents.h"

namespace Application {

	class ComputeParticlesLayer : public DENG::ILayer {
		public:
			virtual void Attach(DENG::IRenderer* _pRenderer, DENG::IWindowContext* _pWindowContext) override;
			virtual void Update(DENG::IFramebuffer* _pFramebuffer) override;

			bool OnWindowResizedEvent(DENG::WindowResizedEvent& _event);
			void OnImGuiDraw();
	};
}

#endif