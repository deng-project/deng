// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: GrassApp.h - GrassApp class implementation
// author: Karl-Mihkel Ott

#define GRASS_APP_CPP
#include "GrassApp.h"

#ifndef WIDTH
#define WIDTH 1280
#endif
#ifndef HEIGHT
#define HEIGHT 720
#endif

namespace Application {

	GrassApp::GrassApp() {
		IWindowContext* pWindowContext = SetWindowContext(new SDLWindowContext);
		IRenderer* pRenderer = SetRenderer(new VulkanRenderer);
		IFramebuffer* pMainFramebuffer = nullptr;
		pWindowContext->SetHints(WindowHint_Shown | WindowHint_Vulkan | WindowHint_FullScreen);
	
		try {
			pWindowContext->Create("Grass demo | Vulkan | SDL", WIDTH, HEIGHT);
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

		auto grassLayer = PushLayer<GrassLayer>(pRenderer, pMainFramebuffer);
		PushLayer<ImGuiLayer>()->SetDrawCallback(&GrassApp::OnImGuiDraw, this);
		AttachLayers();
	}


	void GrassApp::OnImGuiDraw() {
		ImGui::Begin("Diagnostics");
		ImGui::Text("Current framerate %.3f", ImGui::GetIO().Framerate);
		ImGui::End();
	}
}

DENG_MAIN_DECLARATION(Application::GrassApp);