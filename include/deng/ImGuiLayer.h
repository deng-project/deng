// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: ImGuiLayer.h - ImGui layer class header
// author: Karl-Mihkel Ott

#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#include <chrono>
#include <imgui.h>
#include "deng/Api.h"
#include "deng/ILayer.h"

#include "trs/Points.h"

#ifdef IMGUI_LAYER_CPP
	#include "deng/Exceptions.h"
	#include "deng/ErrorDefinitions.h"

	#define KEYLOOKUP(key) s_ImGuiKeyCodes[key - static_cast<uint32_t>(KeySymbol::KEY_UNKNOWN) - 1]
	#define MOUSE_BTN_LOOKUP(btn) s_ImGuiMouseCodes[btn - static_cast<uint32_t>(MouseEvent::BTN_UNKNOWN) - 1]
#endif

namespace DENG {

	typedef void(*PFN_ImGuiDrawCallback)(void* _pUserData);

	class DENG_API ImGuiLayer : public ILayer {
		private:
			ImGuiIO* m_pIO = nullptr;
			ImGuiContext* m_pImguiContext = nullptr;
			
			PFN_ImGuiDrawCallback _Callback = nullptr;
			void* m_pUserData = nullptr;

			TRS::Point2D<float> m_uniform;
			std::chrono::time_point<std::chrono::high_resolution_clock> m_beginTimePoint =
				std::chrono::high_resolution_clock::now();
			std::chrono::time_point<std::chrono::high_resolution_clock> m_endTimePoint =
				std::chrono::high_resolution_clock::now();

			size_t m_uVertexRegionOffset = 0;
			size_t m_uIndexRegionOffset = 0;
			size_t m_uUniformRegionOffset = 0;
			float m_fDeltaTime = 0.f;

			uint32_t m_uTextureHandle = 0;
			MeshComponent m_meshComponent;

			bool m_bIsInit = false;

		private:
			// bunch of translation calls

			void _UpdateIO(IFramebuffer* _pFramebuffer);
			void _CreateDrawCommands(ImDrawData* _pDrawData, IFramebuffer* _pFramebuffer);

		public:
			~ImGuiLayer();
			virtual void Attach(IRenderer* _pRenderer, IWindowContext* _pWindowContext) override;
			virtual void Update(IFramebuffer* _pFramebuffer) override;

			inline void SetDrawCallback(PFN_ImGuiDrawCallback _pfnCallback, void* _pUserData) {
				_Callback = _pfnCallback;
				m_pUserData = _pUserData;
			}
	};
}

#endif