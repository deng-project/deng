// Pydeng: DENG Python scripting API 
// licence: Apache, see LICENCE file
// file: Scriptable.h - Scriptable entities' class header for pydeng
// author: Karl-Mihkel Ott

#ifndef PYDENG_SCRIPTABLE_H
#define PYDENG_SCRIPTABLE_H

//#ifdef PYDENG_COMPONENT_CPP
	#include "deng/deng.h"
//#endif

namespace Pydeng {
	class Scriptable : public DENG::ScriptComponent {
		public:
			Scriptable(DENG::Entity* _ent) : ScriptComponent(_ent) {}

			inline std::string GetName() {
				return m_scriptable_entity->GetName();
			}

			inline uint32_t GetId() {
				return m_scriptable_entity->GetId();
			}
			inline bool GetAttachedBit() {
				return m_scriptable_entity->GetAttachedBit();
			}

			inline DENG::EntityType GetType() {
				return m_scriptable_entity->GetType();
			}
	};

	class CameraScriptable : public Scriptable {
		public:
			CameraScriptable(DENG::Entity *_ent) : Scriptable(_ent) {}

			inline void EnableCamera() {
				((DENG::Camera3D*)m_scriptable_entity)->EnableCamera();
			}

			inline void DisableCamera() {
				((DENG::Camera3D*)m_scriptable_entity)->DisableCamera();
			}

			inline void ChangeFov(float _fov) {
				((DENG::Camera3D*)m_scriptable_entity)->ChangeFov(_fov);
			}
	};


	class EditorCameraScriptable : public CameraScriptable {
		public:
			EditorCameraScriptable(DENG::Entity* _ent) : CameraScriptable(_ent) {}

			inline void ZoomIn() {
				((DENG::EditorCamera*)m_scriptable_entity)->ZoomIn();
			}

			inline void ZoomOut() {
				((DENG::EditorCamera*)m_scriptable_entity)->ZoomOut();
			}

			inline void SetZoomStep(float _step) {
				((DENG::EditorCamera*)m_scriptable_entity)->SetZoomStep(_step);
			}

			inline void SetActionDelay(float _delay) {
				((DENG::EditorCamera*)m_scriptable_entity)->SetActionDelay(_delay);
			}

			inline void SetRotationStep(float _delta) {
				((DENG::EditorCamera*)m_scriptable_entity)->SetRotationStep(_delta);
			}

			inline void SetMouseRotationStep(float _step) {
				((DENG::EditorCamera*)m_scriptable_entity)->SetMouseRotationStep(_step);
			}

			inline void SetOrigin(float _x, float _y, float _z) {
				((DENG::EditorCamera*)m_scriptable_entity)->SetOrigin(TRS::Point3D<float>(_x, _y, _z));
			}
	};


	class FirstPersonCameraScriptable : public CameraScriptable {
		public:
			FirstPersonCameraScriptable(DENG::Entity* _ent) : CameraScriptable(_ent) {}

			inline void Move(bool _x, bool _y, bool _z) {
				((DENG::FirstPersonCamera*)m_scriptable_entity)->Move(TRS::Vector3<bool>(_x, _y, _z));
			}

			inline void Rotate(float _x, float _y) {
				((DENG::FirstPersonCamera*)m_scriptable_entity)->Rotate(TRS::Point2D<float>(_x, _y));
			}
	};

	class VFXScriptable : public Scriptable {

	};

	class SFXScriptable : public Scriptable {

	};
}

#endif