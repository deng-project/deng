#include "deng/Api.h"
#include "deng/App.h"
#include "deng/ImGuiLayer.h"
#include "deng/VulkanRenderer.h"
#include "deng/SDLWindowContext.h"
#include "deng/Exceptions.h"
#include "deng/ErrorDefinitions.h"

#define WIDTH 1280
#define HEIGHT 720

class ImGuiApp : public DENG::App {
	private:
		static void _ImGuiCallback(void* _pUserData) {
			ImGui::ShowDemoWindow();
		}

	public:
		ImGuiApp() {
			DENG::IWindowContext* pWindowContext = SetWindowContext(new DENG::SDLWindowContext);
			DENG::IRenderer* pRenderer = SetRenderer(new DENG::VulkanRenderer);
			pWindowContext->SetHints(DENG::WindowHints::VULKAN | DENG::WindowHints::SHOWN);
			DENG::IFramebuffer* pMainFramebuffer = nullptr;

			try {
				pWindowContext->Create("ImGuiApp | SDL", WIDTH, HEIGHT);
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

			DENG::ImGuiLayer* pLayer = PushLayer<DENG::ImGuiLayer>();
			pLayer->SetDrawCallback(ImGuiApp::_ImGuiCallback, nullptr);
			AttachLayers();
		}
};


DENG_MAIN_DECLARATION(ImGuiApp);