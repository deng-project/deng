// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: ComputeParticlesApp.cpp - Compute particles application class implementation
// author: Karl-Mihkel Ott

#define COMPUTE_PARTICLES_APP_CPP
#include "ComputeParticlesApp.h"

using namespace DENG;

#ifndef WIDTH
#define WIDTH 1280
#endif

#ifndef HEIGHT
#define HEIGHT 720
#endif

namespace Application {

	ComputeParticlesApp::ComputeParticlesApp() {
		IWindowContext* pWindowContext = SetWindowContext(new SDLWindowContext(m_eventManager));
		IRenderer* pRenderer = SetRenderer(new VulkanRenderer);
		IFramebuffer* pMainFramebuffer = nullptr;
		pWindowContext->SetHints(WindowHint_Shown | WindowHint_Vulkan | WindowHint_Resizeable);

		try {
			pWindowContext->Create("Compute particles demu | Vulkan | SDL", WIDTH, HEIGHT);
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

		// push layers
		auto computeParticlesLayer = PushLayer<ComputeParticlesLayer>();
		PushLayer<ImGuiLayer>()->SetDrawCallback(&ComputeParticlesLayer::OnImGuiDraw, computeParticlesLayer);
		AttachLayers();
	}
}