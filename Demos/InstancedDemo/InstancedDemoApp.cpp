// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: InstancedDemoApp.cpp - InstancedDemoApp class implementation and program's entry point
// author: Karl-Mihkel Ott

#define INSTANCED_DEMO_APP_CPP
#include "InstancedDemoApp.h"

#ifndef WIDTH
#define WIDTH 1280
#endif
#ifndef HEIGHT
#define HEIGHT 720
#endif

namespace Application {
	InstancedDemoApp::InstancedDemoApp() {
		IWindowContext* pWindowContext = SetWindowContext(new SDLWindowContext(m_eventManager));
		IRenderer* pRenderer = SetRenderer(new VulkanRenderer);
		IFramebuffer* pMainFramebuffer = nullptr;
		pWindowContext->SetHints(WindowHint_Shown | WindowHint_Vulkan | WindowHint_Resizeable);

		try {
			pWindowContext->Create("Instanced demo | Vulkan | SDL", WIDTH, HEIGHT);
			pMainFramebuffer = SetMainFramebuffer(pRenderer->CreateContext(pWindowContext));
		}
		catch (const WindowContextException& e) {
			DISPATCH_ERROR_MESSAGE("WindowContextException", e.what(), ErrorSeverity::CRITICAL);
		}
		catch (const RendererException& e) {
			DISPATCH_ERROR_MESSAGE("RendererException", e.what(), ErrorSeverity::CRITICAL);
		}
		catch (const HardwareException& e) {
			DISPATCH_ERROR_MESSAGE("HardwareException", e.what(), ErrorSeverity::CRITICAL);
		}

		auto cubeLayer = PushLayer<InstancedCubeLayer>(pRenderer, pMainFramebuffer);
		PushLayer<ImGuiLayer>()->SetDrawCallback(&InstancedCubeLayer::OnImGuiDraw, cubeLayer);
		AttachLayers();
	}
}


DENG_MAIN_DECLARATION(Application::InstancedDemoApp)