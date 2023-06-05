#define BASIC_LIGHTING_DEMO_CPP
#include "BasicLightingDemo.h"

#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif

BasicLightingDemoApp::BasicLightingDemoApp() {
	DENG::IWindowContext* pWindowContext = SetWindowContext(new DENG::SDLWindowContext(m_eventManager));
	DENG::IRenderer* pRenderer = SetRenderer(new DENG::VulkanRenderer);
	DENG::IFramebuffer* pMainFramebuffer = nullptr;
	pWindowContext->SetHints(DENG::WindowHint_Shown | DENG::WindowHint_Vulkan | DENG::WindowHint_Resizeable);

	try {
		pWindowContext->Create("BasicLightingTest | Vulkan | SDL", WIDTH, HEIGHT);
		pMainFramebuffer = SetMainFramebuffer(pRenderer->CreateContext(pWindowContext));
	}
	catch (const DENG::WindowContextException& e) {
		DISPATCH_ERROR_MESSAGE("WindowContextException", e.what(), ErrorSeverity::CRITICAL);
	}
	catch (const DENG::RendererException& e) {
		DISPATCH_ERROR_MESSAGE("RendererException", e.what(), ErrorSeverity::CRITICAL);
	}
	catch (const DENG::HardwareException& e) {
		DISPATCH_ERROR_MESSAGE("HardwareException", e.what(), ErrorSeverity::CRITICAL);
	}

	auto lightingLayer = PushLayer<BasicLightingLayer>(pRenderer, pMainFramebuffer);
	auto pImGuiLayer = PushLayer<DENG::ImGuiLayer>();
	pImGuiLayer->SetDrawCallback(&BasicLightingLayer::OnImGuiDraw, lightingLayer);
	AttachLayers();
}

DENG_MAIN_DECLARATION(BasicLightingDemoApp);