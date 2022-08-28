// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: EditorCameraController.h - main game editor camera controller class header
// author: Karl-Mihkel Ott

#ifndef EDITOR_CAMERA_CONTROLLER_H
#define EDITOR_CAMERA_CONTROLLER_H

#ifdef EDITOR_CAMERA_CONTROLLER_CPP
	#include <iostream>
	#include <any>

	#include <wx/wx.h>
	#include "deng/deng.h"
	#include "deng/Executables/deng-game-editor/OpenGLLoader.h"
#ifdef _WIN32	
	#include "deng/Executables/deng-game-editor/OpenGLLoaderWin32.h"
#endif	
	#include "deng/Executables/deng-game-editor/RendererViewport.h"
#endif

namespace DENG {
	namespace Editor {

		class EditorCameraController : public ScriptComponent {
			private:
				RendererViewport *m_viewport;

			public:
				EditorCameraController(Entity *_cam, RendererViewport *_vp);
				void OnAttach();
				void OnUpdate();
				void OnDestroy();

		};
	}
}

#endif