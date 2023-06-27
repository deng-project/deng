// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Sandbox.h - Sandbox application header
// author: Karl-Mihkel Ott

#ifndef SANDBOX_H
#define SANDBOX_H

#include "deng/App.h"

#ifdef SANDBOX_CPP
#include "deng/SDLWindowContext.h"
#include "deng/VulkanRenderer.h"
#include "deng/Exceptions.h"
#include "deng/ImGuiLayer.h"
#endif

#include "deng/Layers/Layers.h"

namespace DENG {
	namespace Sandbox {

		class SandboxApp : public App {
			public:
				SandboxApp();
				void OnImGuiDraw();
		};

	}
}

#endif