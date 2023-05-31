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

	ImGuiLayer::ImGuiLayer(EventManager& _eventManager) :
		ILayer(_eventManager)
	{
		m_eventManager.AddListener<ImGuiLayer, KeyboardEvent>(&ImGuiLayer::OnKeyboardEvent, this);
		m_eventManager.AddListener<ImGuiLayer, MouseButtonEvent>(&ImGuiLayer::OnMouseButtonEvent, this);
		m_eventManager.AddListener<ImGuiLayer, MouseMovedEvent>(&ImGuiLayer::OnMouseMovedEvent, this);
		m_eventManager.AddListener<ImGuiLayer, MouseScrolledEvent>(&ImGuiLayer::OnMouseScrollEvent, this);
	}

	ImGuiLayer::~ImGuiLayer() {
		m_eventManager.RemoveListener<ImGuiLayer, KeyboardEvent>(this);
		m_eventManager.RemoveListener<ImGuiLayer, MouseButtonEvent>(this);
		m_eventManager.RemoveListener<ImGuiLayer, MouseMovedEvent>(this);
		m_eventManager.RemoveListener<ImGuiLayer, MouseScrolledEvent>(this);

		ImGui::SetCurrentContext(m_pImguiContext);
		ImGui::DestroyContext();
	}


	void ImGuiLayer::_UpdateIO(IFramebuffer* _pFramebuffer) {
		m_pIO->DeltaTime = m_fDeltaTime;
		m_pIO->DisplaySize.x = static_cast<float>(_pFramebuffer->GetWidth());
		m_pIO->DisplaySize.y = static_cast<float>(_pFramebuffer->GetHeight());
	}


	void ImGuiLayer::_CreateDrawCommands(ImDrawData* _pDrawData, IFramebuffer* _pFramebuffer) {
		m_meshComponent.drawCommands.clear();

		if (m_uVertexRegionOffset)
			m_pRenderer->DeallocateMemory(m_uVertexRegionOffset);
		if (m_uIndexRegionOffset)
			m_pRenderer->DeallocateMemory(m_uIndexRegionOffset);

		size_t uVertexSize = 0;
		size_t uIndexSize = 0;

		// calculate sizes
		for (int i = 0; i < _pDrawData->CmdListsCount; i++) {
			const ImDrawList* pDrawList = _pDrawData->CmdLists[i];
			uVertexSize += static_cast<size_t>(pDrawList->VtxBuffer.Size);
			uIndexSize += static_cast<size_t>(pDrawList->IdxBuffer.Size);
		}

		m_uVertexRegionOffset = m_pRenderer->AllocateMemory(uVertexSize * sizeof(ImDrawVert), BufferDataType::VERTEX);
		m_uIndexRegionOffset = m_pRenderer->AllocateMemory(uIndexSize * sizeof(ImDrawIdx), BufferDataType::INDEX);

		// create draw commands and copy data to buffers
		size_t uCommandVertexOffset = m_uVertexRegionOffset, uCommandIndexOffset = m_uIndexRegionOffset;
		for (int i = 0; i < _pDrawData->CmdListsCount; i++) {
			const ImDrawList* pDrawList = _pDrawData->CmdLists[i];
			const ImDrawVert* pVertexBuffer = pDrawList->VtxBuffer.Data;
			const ImDrawIdx* pIndexBuffer = pDrawList->IdxBuffer.Data;

			m_pRenderer->UpdateBuffer(
				pVertexBuffer, 
				static_cast<size_t>(pDrawList->VtxBuffer.Size) * sizeof(ImDrawVert), 
				uCommandVertexOffset);

			m_pRenderer->UpdateBuffer(
				pIndexBuffer,
				static_cast<size_t>(pDrawList->IdxBuffer.Size) * sizeof(ImDrawIdx),
				uCommandIndexOffset);

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

					m_meshComponent.drawCommands.emplace_back();
					const size_t uBaseVertexOffset = uCommandVertexOffset + drawCommand.VtxOffset * sizeof(ImDrawVert);
					const size_t uBaseIndexOffset = uCommandIndexOffset + drawCommand.IdxOffset * sizeof(ImDrawIdx);
					m_meshComponent.drawCommands.back().attributeOffsets.push_back(uBaseVertexOffset + offsetof(ImDrawVert, pos));
					m_meshComponent.drawCommands.back().attributeOffsets.push_back(uBaseVertexOffset + offsetof(ImDrawVert, uv));
					m_meshComponent.drawCommands.back().attributeOffsets.push_back(uBaseVertexOffset + offsetof(ImDrawVert, col));

					m_meshComponent.drawCommands.back().uIndicesOffset = static_cast<uint32_t>(uBaseIndexOffset);
					m_meshComponent.drawCommands.back().uDrawCount = drawCommand.ElemCount;
					
					m_meshComponent.drawCommands.back().scissor.offset = {
						static_cast<int32_t>(clip.x),
						static_cast<int32_t>(clip.y)
					};
					m_meshComponent.drawCommands.back().scissor.extent = {
						static_cast<uint32_t>(clip.z - clip.x),
						static_cast<uint32_t>(clip.w - clip.y)
					};
					m_meshComponent.drawCommands.back().scissor.bEnabled = true;
				}
			}

			uCommandVertexOffset += static_cast<size_t>(pDrawList->VtxBuffer.Size) * sizeof(ImDrawVert);
			uCommandIndexOffset += static_cast<size_t>(pDrawList->IdxBuffer.Size) * sizeof(ImDrawIdx);
		}
	}


	void ImGuiLayer::Attach(IRenderer* _pRenderer, IWindowContext* _pWindowContext) {
		m_uUniformRegionOffset = _pRenderer->AllocateMemory(sizeof(TRS::Point2D<float>), BufferDataType::UNIFORM);
		
		m_pImguiContext = ImGui::CreateContext();
		ImGui::SetCurrentContext(m_pImguiContext);
		m_pIO = &ImGui::GetIO();

		m_shaderComponent.pShader = new Shader("ImGui", "ImGui");
		m_shaderComponent.attributes.push_back(ATTRIBUTE_TYPE_VEC2_FLOAT);
		m_shaderComponent.attributes.push_back(ATTRIBUTE_TYPE_VEC2_FLOAT);
		m_shaderComponent.attributes.push_back(ATTRIBUTE_TYPE_VEC4_UBYTE);

		m_shaderComponent.attributeStrides.push_back(sizeof(ImDrawVert));
		m_shaderComponent.attributeStrides.push_back(sizeof(ImDrawVert));
		m_shaderComponent.attributeStrides.push_back(sizeof(ImDrawVert));

		m_shaderComponent.bEnableBlend = true;
		m_shaderComponent.bEnableScissor = true;
		m_shaderComponent.bEnable2DTextures = true;
		
		m_shaderComponent.uboDataLayouts.resize(2, {});
		m_shaderComponent.uboDataLayouts[0].block.uBinding = 0;
		m_shaderComponent.uboDataLayouts[0].block.uSize = static_cast<uint32_t>(sizeof(TRS::Point2D<float>));
		m_shaderComponent.uboDataLayouts[0].block.uOffset = static_cast<uint32_t>(m_uUniformRegionOffset);
		m_shaderComponent.uboDataLayouts[0].iStage = SHADER_STAGE_VERTEX;
		m_shaderComponent.uboDataLayouts[0].eType = UNIFORM_DATA_TYPE_BUFFER;

		m_shaderComponent.uboDataLayouts[1].block.uBinding = 1;
		m_shaderComponent.uboDataLayouts[1].iStage = SHADER_STAGE_FRAGMENT;
		m_shaderComponent.uboDataLayouts[1].eType = UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER;

		unsigned char* pPixels = nullptr;
		int iWidth = 0, iHeight = 0;
		m_pIO->Fonts->GetTexDataAsRGBA32(&pPixels, &iWidth, &iHeight);

		TextureResource textureResource;
		textureResource.eLoadType = TEXTURE_RESOURCE_LOAD_TYPE_EMBEDDED;
		textureResource.uBitDepth = 4;
		textureResource.uWidth = static_cast<uint32_t>(iWidth);
		textureResource.uHeight = static_cast<uint32_t>(iHeight);
		textureResource.bHeapAllocationFlag = false;
		textureResource.pRGBAData = reinterpret_cast<char*>(pPixels);
		textureResource.eResourceType = TEXTURE_RESOURCE_2D_IMAGE;

		m_uTextureHandle = _pRenderer->AddTextureResource(textureResource);
		m_pIO->Fonts->SetTexID(&m_uTextureHandle);

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
		_Callback(m_pUserData);
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
		m_pRenderer->DrawMesh(m_meshComponent, m_shaderComponent, _pFramebuffer, { m_uTextureHandle });
	}


	bool ImGuiLayer::OnKeyboardEvent(KeyboardEvent& _event) {
		switch (_event.GetEventType()) {
			case EventType::KeyPressed:
				m_pIO->AddKeyEvent(KEYLOOKUP(static_cast<KeyPressedEvent&>(_event).GetKeycode()), true);
				return true;

			case EventType::KeyReleased:
				m_pIO->AddKeyEvent(KEYLOOKUP(static_cast<KeyReleasedEvent&>(_event).GetKeycode()), false);
				return true;

			case EventType::KeyTyped:
				m_pIO->AddInputCharactersUTF8(static_cast<KeyTypedEvent&>(_event).GetBuffer());
				return true;

			default:
				return false;
		}
	}


	bool ImGuiLayer::OnMouseButtonEvent(MouseButtonEvent& _event) {
		switch (_event.GetEventType()) {
			case EventType::MouseButtonPressed:
				m_pIO->AddMouseButtonEvent(MOUSE_BTN_LOOKUP(_event.GetMouseButton()), true);
				return true;

			case EventType::MouseButtonReleased:
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