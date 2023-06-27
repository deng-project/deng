// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: GrassApp.h - GrassApp class header
// author: Karl-Mihkel Ott

#ifndef GRASS_APP_H
#define GRASS_APP_H

#include "deng/App.h"

#ifdef GRASS_APP_CPP
#include "deng/SDLWindowContext.h"
#include "deng/VulkanRenderer.h"
#include "deng/Exceptions.h"
#include "deng/ImGuiLayer.h"

#include "GrassLayer.h"
#endif

using namespace DENG;

namespace Application {

	class GrassApp : public App {
		public:
			GrassApp();
			void OnImGuiDraw();
	};
}

#endif