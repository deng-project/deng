// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Sandbox.cpp - Sandbox application implementation
// author: Karl-Mihkel Ott

#define SANDBOX_CPP
#include "deng/Sandbox/Sandbox.h"

namespace DENG {
	namespace Sandbox {

		SandboxApp::SandboxApp() {
			IWindowContext* pWindowContext = SetWindowContext(new SDLWindowContext);
			IRenderer* pRenderer = SetRenderer(new VulkanRenderer);
			IFramebuffer* pMainFramebuffer = nullptr;
			pWindowContext->SetHints(WindowHint_Shown | WindowHint_Vulkan | WindowHint_Resizeable);
		
			try {
				pWindowContext->Create("SandboxApp | DENG (Vulkan/SDL)", 1280, 720);
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

			auto mainLayer = PushLayer<PBRLayer>(pRenderer, pMainFramebuffer);
			PushLayer<ImGuiLayer>()->SetDrawCallback(&PBRLayer::OnImGuiDraw, mainLayer);
			AttachLayers();
		}

		void SandboxApp::OnImGuiDraw() {
			ImGui::Begin("Diagnostics");
			ImGui::Text("Current framerate %.3f", ImGui::GetIO().Framerate);
			ImGui::End();
		}
	}
}

DENG_MAIN_DECLARATION(DENG::Sandbox::SandboxApp)