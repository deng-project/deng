// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: EditorCameraController.h - main game editor camera controller class header
// author: Karl-Mihkel Ott

#ifndef EDITOR_CAMERA_CONTROLLER_H
#define EDITOR_CAMERA_CONTROLLER_H

#ifdef EDITOR_CAMERA_CONTROLLER_CPP
	#include <string>
	#include <vector>
	#include <iostream>
	#include <unordered_map>
#ifdef _WIN32
	#include <wtypes.h>
#endif

	#include "trs/Points.h"
	#include "trs/Vector.h"
	#include "trs/Quaternion.h"
	#include "trs/Matrix.h"

	#include "deng/Api.h"
	#include "deng/BaseTypes.h"
	#include "deng/ErrorDefinitions.h"
	#include "deng/ModelUniforms.h"
	#include "deng/ShaderDefinitions.h"
	#include "deng/Renderer.h"
	#include "deng/Entity.h"
	#include "deng/ScriptableEntity.h"
	#include "deng/Camera3D.h"
	#include "deng/EditorCamera.h"
#endif

namespace DENG {
	namespace Editor {

		class EditorCameraController : public ScriptComponent {
			public:
				EditorCameraController(Entity *_cam);
				void OnUpdate();

		};
	}
}

#endif