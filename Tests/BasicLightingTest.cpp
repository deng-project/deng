#include "deng/App.h"
#include "deng/ILayer.h"
#include "deng/SDLWindowContext.h"
#include "deng/VulkanRenderer.h"
#include "deng/Exceptions.h"
#include "deng/ErrorDefinitions.h"
#include "deng/ImGuiLayer.h"

#define WIDTH 1280
#define HEIGHT 720

class SceneLayer : public DENG::ILayer {};

class LightingTestApp : public DENG::App {
	public:
		LightingTestApp() {
			DENG::IWindowContext* pWindowContext = SetWindowContext(new DENG::SDLWindowContext);
			DENG::IRenderer* pRenderer = SetRenderer(new DENG::VulkanRenderer);
			DENG::IFramebuffer* pMainFramebuffer = nullptr;

			try {
				pWindowContext->Create("LightingTest | Vulkan | SDL", WIDTH, HEIGHT);
				pMainFramebuffer = pRenderer->CreateContext(pWindowContext);
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

			SceneLayer* pScene = PushLayer<SceneLayer>();
			DENG::ImGuiLayer* pImGui = PushLayer<DENG::ImGuiLayer>();

			AttachLayers();
		}
};

DENG_MAIN_DECLARATION(LightingTestApp);