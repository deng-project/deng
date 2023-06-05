// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: InstancedDemoApp.h - InstancedDemoApp class header
// author: Karl-Mihkel Ott

#ifndef INSTANCED_DEMO_APP_H
#define INSTANCED_DEMO_APP_H

#include "deng/App.h"

#ifdef INSTANCED_DEMO_APP_CPP
#include "deng/SDLWindowContext.h"
#include "deng/VulkanRenderer.h"
#include "deng/Exceptions.h"
#include "deng/ImGuiLayer.h"

#include "InstancedCubeLayer.h"

#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif
#endif

using namespace DENG;

namespace Application {

	class InstancedDemoApp : public App {
		public:
			InstancedDemoApp();
	};
}

#endif
