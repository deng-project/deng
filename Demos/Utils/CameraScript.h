#ifndef CAMERA_SCRIPT_H
#define CAMERA_SCRIPT_H

#include "deng/Components.h"
#include "deng/WindowEvents.h"
#include "deng/InputEvents.h"
#include "deng/CameraTransformer.h"

#define WIDTH 1280
#define HEIGHT 720

#ifdef CAMERA_SCRIPT_CPP
#include "deng/Scene.h"
#define SQ(x) (x*x)
#endif

class CameraScript : public DENG::ScriptBehaviour {
	private:
		DENG::CameraTransformer m_cameraTransformer;
		const float m_fRotationSpeed = (float)MF_PI_2;
		const float m_fMovementSpeed = 1.f;
	
		bool m_bRotations[6] = {};
		bool m_bMovements[4] = {};
	
	public:
		SCRIPT_DEFINE_CONSTRUCTOR(CameraScript)

		bool OnWindowResizedEvent(DENG::WindowResizedEvent& _event);
		bool OnKeyPressEvent(DENG::KeyPressedEvent& _event);
		bool OnKeyReleasedEvent(DENG::KeyReleasedEvent& _event);
		void OnAttach();
		void OnUpdate(float _fTimestep);
		void OnDestroy();
};

#endif
