// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: App.h - application base class implementation
// author: Karl-Mihkel Ott

#define APP_CPP
#include "deng/App.h"

namespace DENG {

	App::~App() {
		for (ILayer* pLayer : m_layers)
			delete pLayer;

		delete m_pRenderer;
		delete m_pWindowContext;
	}

	void App::AttachLayers() {
		for (ILayer* pLayer : m_layers)
			pLayer->Attach(m_pRenderer, m_pWindowContext);
	}


	void App::Run() {
		DENG_ASSERT(m_pWindowContext);

		while (m_pWindowContext->IsAlive()) {
			m_pWindowContext->Update();

			try {
				bool bSuccessBit = m_pRenderer->SetupFrame();
				if (bSuccessBit) {
					m_pMainFramebuffer->BeginCommandBufferRecording({ 0.1f, 0.1f, 0.1f, 1.f });
					for (ILayer* pLayer : m_layers)
						pLayer->Update(m_pMainFramebuffer);
					m_pMainFramebuffer->EndCommandBufferRecording();
					m_pMainFramebuffer->RenderToFramebuffer();
				}
			}
			catch (const RendererException& e) {
				DISPATCH_ERROR_MESSAGE("RendererException", e.what(), ErrorSeverity::CRITICAL);
			}
			catch (const ShaderException& e) {
				DISPATCH_ERROR_MESSAGE("ShaderException", e.what(), ErrorSeverity::CRITICAL);
			}
		}
	}
}