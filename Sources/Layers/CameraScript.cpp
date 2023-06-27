#define CAMERA_SCRIPT_CPP
#include "deng/Layers/CameraScript.h"


bool CameraScript::OnWindowResizedEvent(DENG::WindowResizedEvent& _event) {
	auto& camera = m_scene.GetComponent<DENG::CameraComponent>(m_idEntity);
	camera.mProjection = m_cameraTransformer.CalculateProjection(_event.GetWidth(), _event.GetHeight());
	return false; // pass the even to other listeners
}


bool CameraScript::OnKeyPressEvent(DENG::KeyPressedEvent& _event) {
	switch (_event.GetKeycode()) {
		case DENG::Keycode::LeftArrow:
			m_bRotations[0] = true;
			return true;

		case DENG::Keycode::UpArrow:
			m_bRotations[1] = true;
			return true;

		case DENG::Keycode::DownArrow:
			m_bRotations[2] = true;
			return true;

		case DENG::Keycode::RightArrow:
			m_bRotations[3] = true;
			return true;

		case DENG::Keycode::LeftBracket:
			m_bRotations[4] = true;
			return true;

		case DENG::Keycode::RightBracket:
			m_bRotations[5] = true;
			return true;

		case DENG::Keycode::A:
			m_bMovements[0] = true;
			return true;

		case DENG::Keycode::W:
			m_bMovements[1] = true;
			return true;

		case DENG::Keycode::S:
			m_bMovements[2] = true;
			return true;

		case DENG::Keycode::D:
			m_bMovements[3] = true;
			return true;

		default:
			return false;
	}
}


bool CameraScript::OnKeyReleasedEvent(DENG::KeyReleasedEvent& _event) {
	switch (_event.GetKeycode()) {
		case DENG::Keycode::LeftArrow:
			m_bRotations[0] = false;
			return true;

		case DENG::Keycode::UpArrow:
			m_bRotations[1] = false;
			return true;

		case DENG::Keycode::DownArrow:
			m_bRotations[2] = false;
			return true;

		case DENG::Keycode::RightArrow:
			m_bRotations[3] = false;
			return true;

		case DENG::Keycode::LeftBracket:
			m_bRotations[4] = false;
			return true;

		case DENG::Keycode::RightBracket:
			m_bRotations[5] = false;
			return true;

		case DENG::Keycode::A:
			m_bMovements[0] = false;
			return true;

		case DENG::Keycode::W:
			m_bMovements[1] = false;
			return true;

		case DENG::Keycode::S:
			m_bMovements[2] = false;
			return true;

		case DENG::Keycode::D:
			m_bMovements[3] = false;
			return true;

		default:
			return false;
	}
}

void CameraScript::OnAttach() {
	auto& camera = m_scene.GetComponent<DENG::CameraComponent>(m_idEntity);
	camera.mProjection = m_cameraTransformer.CalculateProjection(1280, 720);

	DENG::EventManager& eventManager = DENG::EventManager::GetInstance();
	eventManager.AddListener<CameraScript, DENG::WindowResizedEvent>(&CameraScript::OnWindowResizedEvent, this);
	eventManager.AddListener<CameraScript, DENG::KeyPressedEvent>(&CameraScript::OnKeyPressEvent, this);
	eventManager.AddListener<CameraScript, DENG::KeyReleasedEvent>(&CameraScript::OnKeyReleasedEvent, this);

	LOG("CameraScript::OnAttach() called");
}

void CameraScript::OnUpdate(float _fTimestep) {
	DENG::CameraComponent& camera = m_scene.GetComponent<DENG::CameraComponent>(m_idEntity);

	// yaw counter-clockwise
	if (m_bRotations[0] && !m_bRotations[3]) {
		float fYaw = m_cameraTransformer.GetYaw();
		fYaw += _fTimestep * m_fRotationSpeed;

		if (fYaw > 2.f * MF_PI)
			fYaw -= 2.f * MF_PI;

		m_cameraTransformer.SetYaw(fYaw);
	}
	// yaw clockwise
	else if (!m_bRotations[0] && m_bRotations[3]) {
		float fYaw = m_cameraTransformer.GetYaw();
		fYaw -= _fTimestep * m_fRotationSpeed;

		if (fYaw < -2.f * MF_PI)
			fYaw += 2.f * MF_PI;
		m_cameraTransformer.SetYaw(fYaw);
	}

	// pitch clockwise
	if (m_bRotations[1] && !m_bRotations[2]) {
		float fPitch = m_cameraTransformer.GetPitch();
		fPitch -= _fTimestep * m_fRotationSpeed;

		// clip rotation to pi/2
		if (fPitch < -MF_PI_2)
			fPitch = -MF_PI_2;
		m_cameraTransformer.SetPitch(fPitch);
	}
	// pitch counter-clockwise
	else if (!m_bRotations[1] && m_bRotations[2]) {
		float fPitch = m_cameraTransformer.GetPitch();
		fPitch += _fTimestep * m_fRotationSpeed;

		if (fPitch > MF_PI_2)
			fPitch = MF_PI_2;
		m_cameraTransformer.SetPitch(fPitch);
	}

	// roll counter-clockwise
	if (m_bRotations[4] && !m_bRotations[5]) {
		float fRoll = m_cameraTransformer.GetRoll();
		fRoll -= _fTimestep * m_fRotationSpeed;

		if (fRoll < -2.f * MF_PI)
			fRoll += 2.f * MF_PI;
		m_cameraTransformer.SetRoll(fRoll);
	}
	// roll counter-clockwise
	else if (!m_bRotations[4] && m_bRotations[5]) {
		float fRoll = m_cameraTransformer.GetRoll();
		fRoll += _fTimestep * m_fRotationSpeed;

		if (fRoll > 2.f * MF_PI)
			fRoll -= 2.f * MF_PI;
		m_cameraTransformer.SetRoll(fRoll);
	}
	m_cameraTransformer.CalculateLookAt();

	// move left
	float fMovementX = 0.f;
	if (m_bMovements[0] && !m_bMovements[3]) {
		fMovementX -= _fTimestep * m_fMovementSpeed;
	}
	// move right
	else if (!m_bMovements[0] && m_bMovements[3]) {
		fMovementX += _fTimestep * m_fMovementSpeed;
	}

	// move forward
	float fMovementZ = 0.f;
	if (m_bMovements[1] && !m_bMovements[2]) {
		fMovementZ -= _fTimestep * m_fMovementSpeed;
	}
	// move backward
	else if (!m_bMovements[1] && m_bMovements[2]) {
		fMovementZ += _fTimestep * m_fMovementSpeed;
	}
	m_cameraTransformer.RelativeMoveX(fMovementX);
	m_cameraTransformer.RelativeMoveZ(fMovementZ);

	camera.vCameraDirection = m_cameraTransformer.GetLookAtDirection();
	camera.vCameraRight = m_cameraTransformer.GetCameraRight();
	camera.vCameraUp = m_cameraTransformer.GetCameraUp();
	camera.vPosition = m_cameraTransformer.GetPosition();
}

void CameraScript::OnDestroy() {
	LOG("CameraScript::OnDestroy() called");
	
	DENG::EventManager& eventManager = DENG::EventManager::GetInstance();
	eventManager.RemoveListener<CameraScript, DENG::WindowResizedEvent>(this);
	eventManager.RemoveListener<CameraScript, DENG::KeyPressedEvent>(this);
	eventManager.RemoveListener<CameraScript, DENG::KeyReleasedEvent>(this);
}