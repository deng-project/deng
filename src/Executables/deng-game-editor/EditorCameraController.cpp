// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: EditorCameraController.cpp - main game editor camera controller class implementation
// author: Karl-Mihkel Ott

#define EDITOR_CAMERA_CONTROLLER_CPP
#include "deng/Executables/deng-game-editor/EditorCameraController.h"

namespace DENG {
	namespace Editor {

		EditorCameraController::EditorCameraController(Entity *_cam, RendererViewport *_vp) : 
			ScriptComponent(_cam),
			m_viewport(_vp) {}


		void EditorCameraController::OnAttach() {
			std::cout << "OnAttach called" << std::endl;
			((EditorCamera*)m_scriptable_entity)->EnableCamera();
		}


		void EditorCameraController::OnUpdate() {
			auto& ir = m_viewport->GetInputRegistry();
			
			if (ir.IsKeyActive(NEKO_MOUSE_BTN_2)) {
				m_viewport->SetVirtualCursor();
				((EditorCamera*)m_scriptable_entity)->Rotate(m_viewport->GetVirtualCursorDelta());
			} else {
				m_viewport->UnsetVirtualCursor();
				if (ir.IsKeyActive(NEKO_MOUSE_SCROLL_DOWN)) {
					((EditorCamera*)m_scriptable_entity)->ZoomOut();
				}

				if (ir.IsKeyActive(NEKO_MOUSE_SCROLL_UP)) {
					((EditorCamera*)m_scriptable_entity)->ZoomIn();
				}
			}
		}


		void EditorCameraController::OnDestroy() {
			std::cout << "OnDestroy called" << std::endl;
		}

	}
}