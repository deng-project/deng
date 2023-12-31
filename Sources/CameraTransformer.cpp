// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: CameraTransfomer.cpp - Camera transform abstraction class implementation
// author: Karl-Mihkel Ott

#define CAMERA_TRANSFORMER_CPP
#include "deng/CameraTransformer.h"

namespace DENG {
	
	TRS::Matrix4<float> CameraTransformer::CalculateProjection(uint32_t _uViewportWidth, uint32_t _uViewportHeight) {
		const float fInverseHalfTan = 1.f / std::tanf(m_fFieldOfView / 2.f);
		const float fAspectRatio =
			static_cast<float>(_uViewportWidth) / static_cast<float>(_uViewportHeight);

		return TRS::Matrix4<float> {
			{ fInverseHalfTan / fAspectRatio, 0.f, 0.f, 0.f },
			{ 0.f, fInverseHalfTan, 0.f, 0.f },
			{ 0.f, 0.f, (m_fNearPlane + m_fFarPlane) / (m_fNearPlane - m_fFarPlane), -1.f },
			{ 0.f, 0.f, (2.f * m_fFarPlane * m_fNearPlane) / (m_fNearPlane - m_fFarPlane), 0.f }
		};
	}


	void CameraTransformer::CalculateLookAt() {
		const float cfSinPitch = std::sinf(m_fPitch), cfCosPitch = std::cosf(m_fPitch);
		const float cfSinYaw = std::sinf(m_fYaw), cfCosYaw = std::cosf(m_fYaw);
		const float cfSinRoll = std::sinf(m_fRoll), cfCosRoll = std::cosf(m_fRoll);

		m_vLookAtDirection[0] = -cfSinYaw * cfCosPitch;
		m_vLookAtDirection[1] = cfSinPitch;
		m_vLookAtDirection[2] = -cfCosYaw * cfCosPitch;
		m_vLookAtDirection.Normalise();

		m_vUpDirection[0] = -cfSinRoll * cfCosYaw + cfSinPitch * cfSinYaw * cfCosRoll;
		m_vUpDirection[1] = cfCosPitch * cfCosRoll;
		m_vUpDirection[2] = cfSinYaw * cfSinRoll + cfSinPitch * cfCosRoll * cfCosYaw;
		m_vUpDirection.Normalise();

		const TRS::Vector3<float> vUpDirection = { m_vUpDirection.first, m_vUpDirection.second, m_vUpDirection.third };
		const TRS::Vector3<float> vLookAtDirection = { m_vLookAtDirection.first, m_vLookAtDirection.second, m_vLookAtDirection.third };
		m_vRightDirection = TRS::Vector3<float>::Cross(vUpDirection, vLookAtDirection);
		m_vRightDirection.Normalise();
	}
}