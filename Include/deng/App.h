// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: App.h - application base class header
// author: Karl-Mihkel Ott

#ifndef APP_H
#define APP_H

#include <vector>

#include "deng/Api.h"
#include "deng/ILayer.h"
#include "deng/IWindowContext.h"
#include "deng/IRenderer.h"
#include "deng/Event.h"

// NOTE: OpenGL support is incomplete
// #include "deng/OpenGLRenderer.h"

#ifdef APP_CPP
	#include "deng/ErrorDefinitions.h"
	#include "deng/Exceptions.h"
#endif

#define DENG_MAIN_DECLARATION(TApp) int main(void) {\
		DENG::App* pApp = new TApp();\
		pApp->Run();\
		delete pApp;\
		return 0;\
	}

namespace DENG {

	class DENG_API App {
		private:
			IWindowContext* m_pWindowContext = nullptr;
			IRenderer* m_pRenderer = nullptr;
			IFramebuffer* m_pMainFramebuffer = nullptr;
			std::vector<ILayer*> m_layers;

		protected:
			EventManager m_eventManager;

		protected:
			virtual void AttachLayers();

		public:
			App() = default;
			virtual ~App();

			template <typename T, typename... Args>
			T* PushLayer(Args&&... args) {
				m_layers.push_back(new T(m_eventManager, std::forward<Args>(args)...));
				return static_cast<T*>(m_layers.back());
			}

			inline IWindowContext* SetWindowContext(IWindowContext* _pWindowContext) {
				m_pWindowContext = _pWindowContext;
				return m_pWindowContext;
			}

			inline IRenderer* SetRenderer(IRenderer* _pRenderer) {
				m_pRenderer = _pRenderer;
				return m_pRenderer;
			}

			inline IFramebuffer* SetMainFramebuffer(IFramebuffer* _pFramebuffer) {
				m_pMainFramebuffer = _pFramebuffer;
				return m_pMainFramebuffer;
			}

			virtual void Run();
	};
}

#endif