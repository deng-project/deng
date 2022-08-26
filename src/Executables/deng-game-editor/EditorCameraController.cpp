// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: EditorCameraController.cpp - main game editor camera controller class implementation
// author: Karl-Mihkel Ott

#define EDITOR_CAMERA_CONTROLLER_CPP
#include "deng/Executables/deng-game-editor/EditorCameraController.h"

namespace DENG {
	namespace Editor {

		EditorCameraController::EditorCameraController(Entity *_cam)
			: ScriptComponent(_cam) {}


		void EditorCameraController::OnUpdate() {
			std::cout << "OnUpdate called" << std::endl;
		}

	}
}