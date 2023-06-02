#include "deng/Api.h"
#include "deng/App.h"
#include "deng/ImGuiLayer.h"
#include "deng/VulkanRenderer.h"
#include "deng/SDLWindowContext.h"
#include "deng/Exceptions.h"
#include "deng/ErrorDefinitions.h"
#include "deng/WindowEvents.h"

#define WIDTH 1280
#define HEIGHT 720

class WindowResizeLayer : public DENG::ILayer {
	private:
		DENG::IRenderer* m_pRenderer;

	public:
		WindowResizeLayer(DENG::EventManager& _eventManager, DENG::IRenderer* _pRenderer) :
			ILayer(_eventManager),
			m_pRenderer(_pRenderer) {}

		virtual void Attach(DENG::IRenderer*, DENG::IWindowContext*) override {
			m_eventManager.AddListener<WindowResizeLayer, DENG::WindowResizedEvent>(&WindowResizeLayer::OnWindowResizedEvent, this);
		}

		virtual void Update(DENG::IFramebuffer*) override {}

		bool OnWindowResizedEvent(DENG::WindowResizedEvent& _event) {
			m_pRenderer->UpdateViewport(_event.GetWidth(), _event.GetHeight());
			return true;
		}
};

class ImGuiApp : public DENG::App {
	public:
		ImGuiApp() {
			DENG::IWindowContext* pWindowContext = SetWindowContext(new DENG::SDLWindowContext(m_eventManager));
			DENG::IRenderer* pRenderer = SetRenderer(new DENG::VulkanRenderer);
			pWindowContext->SetHints(DENG::WindowHint_Vulkan | DENG::WindowHint_Shown | DENG::WindowHint_Resizeable);
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

			PushLayer<WindowResizeLayer>(pRenderer);
			DENG::ImGuiLayer* pLayer = PushLayer<DENG::ImGuiLayer>();
			pLayer->SetDrawCallback(&ImGuiApp::ImGuiCallback, this);
			AttachLayers();
		}

		void ImGuiCallback() {
			ImGui::ShowDemoWindow();
		}
};


DENG_MAIN_DECLARATION(ImGuiApp);