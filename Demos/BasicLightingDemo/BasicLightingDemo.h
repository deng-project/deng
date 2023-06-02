#ifndef BASIC_LIGHTING_DEMO_H
#define BASIC_LIGHTING_DEMO_H

#include <random>
#include <cmath>
#include <unordered_map>

#include "deng/App.h"
#include "deng/ILayer.h"
#include "deng/SDLWindowContext.h"
#include "deng/VulkanRenderer.h"
#include "deng/Exceptions.h"
#include "deng/ErrorDefinitions.h"
#include "deng/CameraTransformer.h"
#include "deng/ImGuiLayer.h"
#include "deng/Scene.h"
#include "deng/WindowEvents.h"
#include "deng/InputEvents.h"

#include <das/stb_image.h>

#include "CameraScript.h"
#include "BasicLightingLayer.h"


class BasicLightingDemoApp : public DENG::App {
	public:
		BasicLightingDemoApp();
};

#endif