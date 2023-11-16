// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: ImGuiLayer.h - ImGui layer class header
// author: Karl-Mihkel Ott

#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#include <chrono>
#include <functional>

#include <imgui.h>
#include "deng/Api.h"
#include "deng/ILayer.h"

#include "trs/Points.h"

#include "deng/InputEvents.h"

#ifdef IMGUI_LAYER_CPP
	#include "deng/ImGuiResourceBuilders.h"
	#include "deng/RenderResources.h"	
	#include "deng/Exceptions.h"
	#include "deng/ErrorDefinitions.h"

	#define KEYLOOKUP(key) s_ImGuiKeyCodes[static_cast<size_t>(key) - 1]
	#define MOUSE_BTN_LOOKUP(btn) s_ImGuiMouseCodes[static_cast<size_t>(btn) - 1]
#endif

namespace DENG {

	class DENG_API ImGuiLayer : public ILayer {
		private:
			ImGuiIO* m_pIO = nullptr;
			ImGuiContext* m_pImguiContext = nullptr;
			
			std::function<void()> _CallbackLambda = nullptr;

			TRS::Point2D<float> m_uniform;
			std::chrono::time_point<std::chrono::high_resolution_clock> m_beginTimePoint =
				std::chrono::high_resolution_clock::now();
			std::chrono::time_point<std::chrono::high_resolution_clock> m_endTimePoint =
				std::chrono::high_resolution_clock::now();

			size_t m_uVertexRegionOffset = 0;
			size_t m_uUniformRegionOffset = 0;
			float m_fDeltaTime = 0.f;

			char* m_pDataBuffer = nullptr;
			size_t m_uDataBufferSize = 0;

			uint32_t m_uTextureHandle = 0;
			bool m_bIsInit = false;

		private:
			// bunch of translation calls

			void _UpdateIO(IFramebuffer* _pFramebuffer);
			void _CreateDrawCommands(ImDrawData* _pDrawData, IFramebuffer* _pFramebuffer);

		public:
			ImGuiLayer();
			~ImGuiLayer();
			virtual void Attach(IRenderer* _pRenderer, IWindowContext* _pWindowContext, IGraphicsShaderCompiler* _pGraphicsShaderCompiler) override;
			virtual void Update(IFramebuffer* _pFramebuffer) override;

			template<typename T>
			using PFN_ImGuiDrawCallbackMethod = void(T::*)();

			template<typename T>
			void SetDrawCallback(PFN_ImGuiDrawCallbackMethod<T> _pfnMethod, T* _pInstance) {
				_CallbackLambda = [=]() { (*_pInstance.*_pfnMethod)(); };
			}

			bool OnKeyboardEvent(KeyboardEvent& _event);
			bool OnMouseButtonEvent(MouseButtonEvent& _event);
			bool OnMouseMovedEvent(MouseMovedEvent& _event);
			bool OnMouseScrollEvent(MouseScrolledEvent& _event);
	};
}

#endif