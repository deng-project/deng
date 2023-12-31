// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: ImGuiLayer.cpp - ImGui layer class implementation
// author: Karl-Mihkel Ott

#define IMGUI_LAYER_CPP
#include "deng/ImGuiLayer.h"

namespace DENG {

	static ImGuiMouseButton s_ImGuiMouseCodes[] = {
		ImGuiMouseButton_Left,
		ImGuiMouseButton_Middle,
		ImGuiMouseButton_Right
	};

	static ImGuiKey s_ImGuiKeyCodes[] = {
		ImGuiKey_Space,
		ImGuiKey_Apostrophe,
		ImGuiKey_Comma,
		ImGuiKey_Minus,
		ImGuiKey_Period,
		ImGuiKey_Slash,
		ImGuiKey_0, ImGuiKey_1, ImGuiKey_2, ImGuiKey_3, ImGuiKey_4,
		ImGuiKey_5, ImGuiKey_6, ImGuiKey_7, ImGuiKey_8, ImGuiKey_9,
		ImGuiKey_Semicolon,
		ImGuiKey_Equal,
		ImGuiKey_A, ImGuiKey_B, ImGuiKey_C, ImGuiKey_D, ImGuiKey_E,
		ImGuiKey_F, ImGuiKey_G, ImGuiKey_H, ImGuiKey_I, ImGuiKey_J,
		ImGuiKey_K, ImGuiKey_L, ImGuiKey_M, ImGuiKey_N, ImGuiKey_O,
		ImGuiKey_P, ImGuiKey_Q, ImGuiKey_R, ImGuiKey_S, ImGuiKey_T,
		ImGuiKey_U, ImGuiKey_V, ImGuiKey_W, ImGuiKey_X, ImGuiKey_Y,
		ImGuiKey_Z,
		ImGuiKey_LeftBracket,
		ImGuiKey_RightBracket,
		ImGuiKey_Backslash,
		ImGuiKey_GraveAccent,
		ImGuiKey_Escape,
		ImGuiKey_Enter,
		ImGuiKey_Tab,
		ImGuiKey_Backspace,
		ImGuiKey_Insert,
		ImGuiKey_Delete,
		ImGuiKey_RightArrow,
		ImGuiKey_LeftArrow,
		ImGuiKey_DownArrow,
		ImGuiKey_UpArrow,
		ImGuiKey_PageUp,
		ImGuiKey_PageDown,
		ImGuiKey_Home,
		ImGuiKey_End,
		ImGuiKey_CapsLock,
		ImGuiKey_ScrollLock,
		ImGuiKey_NumLock,
		ImGuiKey_PrintScreen,
		ImGuiKey_Pause,
		ImGuiKey_F1, ImGuiKey_F2, ImGuiKey_F3, ImGuiKey_F4, ImGuiKey_F5, ImGuiKey_F6,
		ImGuiKey_F7, ImGuiKey_F8, ImGuiKey_F9, ImGuiKey_F10, ImGuiKey_F11, ImGuiKey_F12,
		ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None,
		ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, ImGuiKey_None, 
		ImGuiKey_None,
		ImGuiKey_Keypad0, ImGuiKey_Keypad1, ImGuiKey_Keypad2, ImGuiKey_Keypad3, ImGuiKey_Keypad4,
		ImGuiKey_Keypad5, ImGuiKey_Keypad6, ImGuiKey_Keypad7, ImGuiKey_Keypad8, ImGuiKey_Keypad9,
		ImGuiKey_KeypadDecimal,
		ImGuiKey_KeypadDivide,
		ImGuiKey_KeypadMultiply,
		ImGuiKey_KeypadSubtract,
		ImGuiKey_KeypadAdd,
		ImGuiKey_KeypadEnter,
		ImGuiKey_KeypadEqual,
		ImGuiKey_LeftShift,
		ImGuiKey_LeftCtrl,
		ImGuiKey_LeftAlt,
		ImGuiKey_LeftSuper,
		ImGuiKey_RightShift,
		ImGuiKey_RightCtrl,
		ImGuiKey_RightAlt,
		ImGuiKey_RightSuper,
		ImGuiKey_Menu
	};

	ImGuiLayer::ImGuiLayer() {
		EventManager& eventManager = EventManager::GetInstance();
		eventManager.AddListener<ImGuiLayer, KeyboardEvent>(&ImGuiLayer::OnKeyboardEvent, this);
		eventManager.AddListener<ImGuiLayer, MouseButtonEvent>(&ImGuiLayer::OnMouseButtonEvent, this);
		eventManager.AddListener<ImGuiLayer, MouseMovedEvent>(&ImGuiLayer::OnMouseMovedEvent, this);
		eventManager.AddListener<ImGuiLayer, MouseScrolledEvent>(&ImGuiLayer::OnMouseScrollEvent, this);
	}

	ImGuiLayer::~ImGuiLayer() {
		EventManager& eventManager = EventManager::GetInstance();
		eventManager.RemoveListener<ImGuiLayer, KeyboardEvent>(this);
		eventManager.RemoveListener<ImGuiLayer, MouseButtonEvent>(this);
		eventManager.RemoveListener<ImGuiLayer, MouseMovedEvent>(this);
		eventManager.RemoveListener<ImGuiLayer, MouseScrolledEvent>(this);

		ImGui::SetCurrentContext(m_pImguiContext);
		ImGui::DestroyContext();
	}


	void ImGuiLayer::_UpdateIO(IFramebuffer* _pFramebuffer) {
		m_pIO->DeltaTime = m_fDeltaTime;
		m_pIO->DisplaySize.x = static_cast<float>(_pFramebuffer->GetWidth());
		m_pIO->DisplaySize.y = static_cast<float>(_pFramebuffer->GetHeight());
	}


	void ImGuiLayer::_CreateDrawCommands(ImDrawData* _pDrawData, IFramebuffer* _pFramebuffer) {
		ResourceManager& resourceManager = ResourceManager::GetInstance();
		auto pMesh = resourceManager.GetMesh(SID("__ImGui__"));
		DENG_ASSERT(pMesh);
		pMesh->drawCommands.clear();

		if (m_uVertexRegionOffset)
			m_pRenderer->DeallocateMemory(m_uVertexRegionOffset);

		size_t uVertexSize = 0;
		size_t uIndexSize = 0;

		// calculate sizes
		for (int i = 0; i < _pDrawData->CmdListsCount; i++) {
			const ImDrawList* pDrawList = _pDrawData->CmdLists[i];
			uVertexSize += static_cast<size_t>(pDrawList->VtxBuffer.Size);
			uIndexSize += static_cast<size_t>(pDrawList->IdxBuffer.Size);
		}

		uVertexSize *= sizeof(ImDrawVert);
		uIndexSize *= sizeof(ImDrawIdx);

		m_uVertexRegionOffset = m_pRenderer->AllocateMemory(uVertexSize + uIndexSize, BufferDataType::Vertex);
		
		// check if data buffer is big enough
		if (uVertexSize + uIndexSize > m_uDataBufferSize) {
			delete[] m_pDataBuffer;
			m_uDataBufferSize = ((uVertexSize + uIndexSize) * 3) >> 1;
			m_pDataBuffer = new char[m_uDataBufferSize];
		}

		// create draw commands and copy data to buffers
		size_t uOffset = 0;
		size_t uCommandVertexOffset = m_uVertexRegionOffset;
		size_t uCommandIndexOffset = m_uVertexRegionOffset;

		for (int i = 0; i < _pDrawData->CmdListsCount; i++) {
			const ImDrawList* pDrawList = _pDrawData->CmdLists[i];
			const ImDrawVert* pVertexBuffer = pDrawList->VtxBuffer.Data;
			const ImDrawIdx* pIndexBuffer = pDrawList->IdxBuffer.Data;

			std::memcpy(m_pDataBuffer + uOffset, pDrawList->VtxBuffer.Data, static_cast<size_t>(pDrawList->VtxBuffer.Size) * sizeof(ImDrawVert));
			uOffset += static_cast<size_t>(pDrawList->VtxBuffer.Size) * sizeof(ImDrawVert);
			uCommandIndexOffset += static_cast<size_t>(pDrawList->VtxBuffer.Size) * sizeof(ImDrawVert);

			std::memcpy(m_pDataBuffer + uOffset, pDrawList->IdxBuffer.Data, static_cast<size_t>(pDrawList->IdxBuffer.Size) * sizeof(ImDrawIdx));
			uOffset += static_cast<size_t>(pDrawList->IdxBuffer.Size) * sizeof(ImDrawIdx);

			for (int j = 0; j < pDrawList->CmdBuffer.Size; j++) {
				const ImDrawCmd& drawCommand = pDrawList->CmdBuffer[j];
				if (drawCommand.UserCallback)
					drawCommand.UserCallback(pDrawList, &drawCommand);
				else {
					ImVec2 position = _pDrawData->DisplayPos;
					ImVec4 clip = drawCommand.ClipRect;
					clip.x -= position.x; clip.y -= position.y;
					clip.z -= position.x; clip.w -= position.y;

					// clamp to viewport
					if (clip.x < 0.f) clip.x = 0.f;
					if (clip.y < 0.f) clip.y = 0.f;

					if (clip.z > static_cast<float>(_pFramebuffer->GetWidth())) 
						clip.z = static_cast<float>(_pFramebuffer->GetWidth());
					if (clip.w > static_cast<float>(_pFramebuffer->GetHeight())) 
						clip.w = static_cast<float>(_pFramebuffer->GetHeight());

					// check if draw call can be omitted
					if (clip.z <= clip.x || clip.w <= clip.y)
						continue;

					pMesh->drawCommands.emplace_back();
					const size_t uBaseVertexOffset = uCommandVertexOffset + drawCommand.VtxOffset * sizeof(ImDrawVert);
					const size_t uBaseIndexOffset = uCommandIndexOffset + drawCommand.IdxOffset * sizeof(ImDrawIdx);
					pMesh->drawCommands.back().attributeOffsets.push_back(uBaseVertexOffset + offsetof(ImDrawVert, pos));
					pMesh->drawCommands.back().attributeOffsets.push_back(uBaseVertexOffset + offsetof(ImDrawVert, uv));
					pMesh->drawCommands.back().attributeOffsets.push_back(uBaseVertexOffset + offsetof(ImDrawVert, col));

					pMesh->drawCommands.back().uIndicesOffset = static_cast<uint32_t>(uBaseIndexOffset);
					pMesh->drawCommands.back().uDrawCount = drawCommand.ElemCount;
					
					pMesh->drawCommands.back().scissor.offset = {
						static_cast<int32_t>(clip.x),
						static_cast<int32_t>(clip.y)
					};
					pMesh->drawCommands.back().scissor.extent = {
						static_cast<uint32_t>(clip.z - clip.x),
						static_cast<uint32_t>(clip.w - clip.y)
					};
				}
			}

			uCommandIndexOffset += static_cast<size_t>(pDrawList->IdxBuffer.Size) * sizeof(ImDrawIdx);
			uCommandVertexOffset = uCommandIndexOffset;
		}

		if (uVertexSize + uIndexSize)
			m_pRenderer->UpdateBuffer(m_pDataBuffer, uVertexSize + uIndexSize, m_uVertexRegionOffset);
	}


	void ImGuiLayer::Attach(IRenderer* _pRenderer, IWindowContext* _pWindowContext) {
		m_uUniformRegionOffset = _pRenderer->AllocateMemory(sizeof(TRS::Point2D<float>), BufferDataType::Uniform);
		
		m_pImguiContext = ImGui::CreateContext();
		ImGui::SetCurrentContext(m_pImguiContext);
		m_pIO = &ImGui::GetIO();

		// query texture resources
		unsigned char* pPixels = nullptr;
		int iWidth = 0, iHeight = 0;
		m_pIO->Fonts->GetTexDataAsRGBA32(&pPixels, &iWidth, &iHeight);

		ResourceManager& resourceManager = ResourceManager::GetInstance();
		resourceManager.AddShader<ImGuiShaderBuilder>(SID("__ImGui__"), m_uUniformRegionOffset, SID("__ImGui__"));
		resourceManager.AddMesh<ImGuiMeshBuilder>(SID("__ImGui__"));
		resourceManager.AddTexture<ImGuiTextureBuilder>(SID("__ImGui__"), iWidth, iHeight, pPixels);

		m_pIO->Fonts->SetTexID(SID("__ImGui__"));

		m_pRenderer = _pRenderer;
		m_pWindowContext = _pWindowContext;
	}


	void ImGuiLayer::Update(IFramebuffer* _pFramebuffer) {
		m_endTimePoint = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float, std::milli> frametime = m_endTimePoint - m_beginTimePoint;
		m_fDeltaTime = frametime.count() / 1000.f;
		if (m_fDeltaTime < 0.f)
			m_fDeltaTime = 0.f;
		m_beginTimePoint = std::chrono::high_resolution_clock::now();

		ImGui::SetCurrentContext(m_pImguiContext);
		_UpdateIO(_pFramebuffer);
		ImGui::NewFrame();
		if (_CallbackLambda)
			_CallbackLambda();
		ImGui::EndFrame();

		ImGui::Render();
		ImDrawData* pDrawData = ImGui::GetDrawData();
		_CreateDrawCommands(pDrawData, _pFramebuffer);
		
		m_uniform = TRS::Point2D<float>{
			static_cast<float>(_pFramebuffer->GetWidth()),
			static_cast<float>(_pFramebuffer->GetHeight()) 
		};
		
		m_pRenderer->UpdateBuffer(&m_uniform, sizeof(TRS::Point2D<float>), m_uUniformRegionOffset);
		m_bIsInit = true;
		m_pRenderer->DrawInstance(
			SID("__ImGui__"),
			SID("__ImGui__"),
			_pFramebuffer, 1, 0, 0);
	}


	bool ImGuiLayer::OnKeyboardEvent(KeyboardEvent& _event) {
		switch (_event.GetEventType()) {
			case SID("KeyPressedEvent"):
				m_pIO->AddKeyEvent(KEYLOOKUP(static_cast<KeyPressedEvent&>(_event).GetKeycode()), true);
				return true;

			case SID("KeyReleasedEvent"):
				m_pIO->AddKeyEvent(KEYLOOKUP(static_cast<KeyReleasedEvent&>(_event).GetKeycode()), false);
				return true;

			case SID("KeyTypedEvent"):
				m_pIO->AddInputCharactersUTF8(static_cast<KeyTypedEvent&>(_event).GetBuffer());
				return true;

			default:
				return false;
		}
	}


	bool ImGuiLayer::OnMouseButtonEvent(MouseButtonEvent& _event) {
		switch (_event.GetEventType()) {
			case SID("MouseButtonPressedEvent"):
				m_pIO->AddMouseButtonEvent(MOUSE_BTN_LOOKUP(_event.GetMouseButton()), true);
				return true;

			case SID("MouseButtonReleasedEvent"):
				m_pIO->AddMouseButtonEvent(MOUSE_BTN_LOOKUP(_event.GetMouseButton()), false);
				return true;

			default:
				return false;
		}
	}


	bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent& _event) {
		m_pIO->AddMousePosEvent(
			static_cast<float>(_event.GetPositionX()),
			static_cast<float>(_event.GetPositionY()));

		return true;
	}


	bool ImGuiLayer::OnMouseScrollEvent(MouseScrolledEvent& _event) {
		m_pIO->AddMouseWheelEvent(
			static_cast<float>(_event.GetScrollDeltaX()),
			static_cast<float>(_event.GetScrollDeltaY()));

		return true;
	}
}