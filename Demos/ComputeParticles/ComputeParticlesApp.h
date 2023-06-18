// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: ComputeParticlesApp.h - Compute particles application class header
// author: Karl-Mihkel Ott

#ifndef COMPUTE_PARTICLES_APP_H
#define COMPUTE_PARTICLES_APP_H

#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif

#include "deng/App.h"
#include "deng/SDLWindowContext.h"
#include "deng/VulkanRenderer.h"
#include "deng/Exceptions.h"
#include "deng/ErrorDefinitions.h"
#include "deng/ImGuiLayer.h"
#include "ComputeParticlesLayer.h"

namespace Application {
	class ComputeParticlesApp : public DENG::App {
		public:
			ComputeParticlesApp();
	};
}

#endif